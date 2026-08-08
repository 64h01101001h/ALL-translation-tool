// linebuild.cpp — port of BDRC's line_detection.py line-building chain
// (see linebuild.h). Geometry via OpenCV C++ == the canonical cv2's own
// native code. Upstream quirks preserved and marked [UPSTREAM QUIRK].
#include "allocr/linebuild.h"

#include <opencv2/imgproc.hpp>
#if __has_include(<opencv2/geometry/2d.hpp>)
#include <opencv2/geometry/2d.hpp>  // OpenCV 5: contourArea/minAreaRect etc.
#endif

#include <algorithm>
#include <numeric>

namespace allocr {

namespace {

using cv::Mat;

// get_rotation_angle_from_lines: minAreaRect angles of large contours,
// split into near-0 and near-90 populations, mean of the dominant one
double rotationAngle(const Mat& mask, double maxAngle = 5.0) {
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    const double areaThresh = mask.rows * mask.cols * 0.001;
    std::vector<double> low, high;
    for (const auto& c : contours) {
        if (cv::contourArea(c) <= areaThresh) continue;
        const double a = cv::minAreaRect(c).angle;
        if (std::abs(a) != 0.0 && a < maxAngle) low.push_back(a);
        if (std::abs(a) != 90.0 && a > (90 - maxAngle)) high.push_back(a);
    }
    auto mean = [](const std::vector<double>& v) {
        return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
    };
    if (low.size() > high.size() && !low.empty()) return mean(low);
    if (!high.empty()) return -(90 - mean(high));
    return 0.0;
}

Mat rotateFromAngle(const Mat& img, double angle) {
    Mat rot = cv::getRotationMatrix2D(
        cv::Point2f(img.cols / 2.0f, img.rows / 2.0f), angle, 1.0);
    Mat out;
    cv::warpAffine(img, out, rot, cv::Size(img.cols, img.rows),
                   cv::INTER_LINEAR, cv::BORDER_CONSTANT,
                   cv::Scalar(0, 0, 0));
    return out;
}

std::vector<cv::Point> optimizeContour(const std::vector<cv::Point>& cnt,
                                       double e = 0.001) {
    const double eps = e * cv::arcLength(cnt, true);
    std::vector<cv::Point> out;
    cv::approxPolyDP(cnt, out, eps, true);
    return out;
}

// get_line_threshold: slice the mask's bbox, take the slice with the most
// contours, median of contour y-centers // n_contours.
// [UPSTREAM QUIRK] the slice x-offset is x_steps*step (not
// slice_width*step) and the median is integer-divided by the contour
// count — both preserved verbatim.
double lineThreshold(const Mat& mask, int sliceWidth = 20) {
    cv::Rect bb = cv::boundingRect(mask);
    const int xSteps = (bb.width / sliceWidth) / 2;
    int bestN = -1;
    std::vector<std::vector<cv::Point>> bestContours;
    for (int step = 1; step <= xSteps; ++step) {
        const int xStart = bb.x + xSteps * step;
        const int xEnd = std::min(xStart + sliceWidth, mask.cols);
        if (xStart >= mask.cols || xStart >= xEnd) continue;
        Mat slice = mask(cv::Rect(xStart, bb.y, xEnd - xStart, bb.height))
                        .clone();
        std::vector<std::vector<cv::Point>> cs;
        cv::findContours(slice, cs, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
        if (static_cast<int>(cs.size()) > bestN) {
            bestN = static_cast<int>(cs.size());
            bestContours = cs;
        }
    }
    if (bestN <= 0) return 0.0;
    std::vector<int> ys;
    for (const auto& c : bestContours) {
        cv::Rect r = cv::boundingRect(c);
        ys.push_back(r.y + r.height / 2);
    }
    std::sort(ys.begin(), ys.end());
    const size_t n = ys.size();
    const double median = n % 2 ? ys[n / 2] : (ys[n / 2 - 1] + ys[n / 2]) / 2.0;
    return std::floor(median / bestN);
}

// sort_bbox_centers: group centers into lines by |mean_y(group) - y| vs
// threshold; each group sorted by x; groups REVERSED at the end (upstream
// relies on cv2's bottom-up contour order).
std::vector<std::vector<cv::Point>> sortBboxCenters(
    const std::vector<cv::Point>& centers, double threshold) {
    std::vector<std::vector<cv::Point>> out;
    std::vector<cv::Point> tmp;
    for (const auto& c : centers) {
        if (!tmp.empty()) {
            double meanY = 0;
            for (const auto& p : tmp) meanY += p.y;
            meanY /= tmp.size();
            if (std::abs(meanY - c.y) > threshold) {
                std::sort(tmp.begin(), tmp.end(),
                          [](const cv::Point& a, const cv::Point& b) {
                              return a.x < b.x;
                          });
                out.push_back(tmp);
                tmp.clear();
            }
        }
        tmp.push_back(c);
    }
    if (!tmp.empty()) out.push_back(tmp);
    for (auto& line : out)
        std::sort(line.begin(), line.end(),
                  [](const cv::Point& a, const cv::Point& b) {
                      return a.x < b.x;
                  });
    std::reverse(out.begin(), out.end());
    return out;
}

struct RawLine {
    std::vector<cv::Point> contour;
    cv::Rect bbox;
    cv::Point center;
};

RawLine buildLineData(const std::vector<cv::Point>& contour) {
    RawLine l;
    l.contour = optimizeContour(contour);
    l.bbox = cv::boundingRect(l.contour);
    l.center = {l.bbox.x + l.bbox.width / 2, l.bbox.y + l.bbox.height / 2};
    return l;
}

// group_line_chunks: same-row chunks merge via convex hull.
// [UPSTREAM QUIRK] the "adaptive_grouping" offsets mutate only local
// copies upstream (dead code) — omitted with this note.
std::vector<RawLine> groupLineChunks(
    const std::vector<std::vector<cv::Point>>& sortedCenters,
    const std::vector<RawLine>& lines) {
    std::vector<RawLine> out;
    for (const auto& group : sortedCenters) {
        if (group.size() > 1) {
            std::vector<cv::Point> stacked;
            for (const auto& c : group)
                for (const auto& l : lines)
                    if (l.center == c) {
                        stacked.insert(stacked.end(), l.contour.begin(),
                                       l.contour.end());
                        break;
                    }
            std::vector<cv::Point> hull;
            cv::convexHull(stacked, hull);
            RawLine nl;
            nl.contour = hull;
            nl.bbox = cv::boundingRect(hull);
            nl.center = {nl.bbox.x + nl.bbox.width / 2,
                         nl.bbox.y + nl.bbox.height / 2};
            out.push_back(std::move(nl));
        } else {
            for (const auto& c : group)
                for (const auto& l : lines)
                    if (l.center == c) {
                        out.push_back(l);
                        break;
                    }
        }
    }
    return out;
}

// mask_n_crop: zero outside the mask, then remove EVERY all-zero row and
// column ([UPSTREAM QUIRK]: interior gaps squeeze out too, not just the
// borders — np.delete semantics preserved).
Mat maskAndCrop(const Mat& image, const Mat& mask) {
    Mat masked;
    cv::bitwise_and(image, image, masked, mask);
    std::vector<int> rows, cols;
    for (int y = 0; y < masked.rows; ++y) {
        const uint8_t* p = masked.ptr<uint8_t>(y);
        bool any = false;
        for (int i = 0, n = masked.cols * masked.channels(); i < n; ++i)
            if (p[i]) { any = true; break; }
        if (any) rows.push_back(y);
    }
    for (int x = 0; x < masked.cols; ++x) {
        bool any = false;
        for (int y = 0; y < masked.rows && !any; ++y) {
            const uint8_t* p =
                masked.ptr<uint8_t>(y) + x * masked.channels();
            for (int c = 0; c < masked.channels(); ++c)
                if (p[c]) { any = true; break; }
        }
        if (any) cols.push_back(x);
    }
    Mat out(static_cast<int>(rows.size()), static_cast<int>(cols.size()),
            masked.type());
    for (size_t ry = 0; ry < rows.size(); ++ry)
        for (size_t rx = 0; rx < cols.size(); ++rx)
            for (int c = 0; c < masked.channels(); ++c)
                out.ptr<uint8_t>(static_cast<int>(ry))[rx * masked.channels() +
                                                       c] =
                    masked.ptr<uint8_t>(rows[ry])[cols[rx] *
                                                      masked.channels() +
                                                  c];
    return out;
}

Mat extractLine(const Mat& image, const Mat& mask, int bboxH,
                double kFactor) {
    const int kSize = static_cast<int>(bboxH * kFactor);
    Mat kernel = cv::getStructuringElement(
        cv::MORPH_RECT,
        cv::Size(std::max(kSize, 1),
                 std::max(static_cast<int>(kSize * kFactor), 1)));
    Mat dilated;
    cv::dilate(mask, dilated, kernel, cv::Point(-1, -1), 1);
    return maskAndCrop(image, dilated);
}

Mat getLineImage(const Mat& image, const Mat& mask, int bboxH,
                 double bboxTolerance, double kFactor, double& adaptedK) {
    double k = kFactor;
    Mat line = extractLine(image, mask, bboxH, k);
    int attempts = 0;
    while (line.rows > bboxH * bboxTolerance && attempts < 10) {
        k -= 0.1;
        if (k <= 0.1) break;
        line = extractLine(image, mask, bboxH, k);
        ++attempts;
    }
    adaptedK = k;
    return line;
}

}  // namespace

PageLines buildLines(const uint8_t* rgb, int w, int h, const Mask& mask,
                     double kFactor, double bboxTolerance, bool mergeLines) {
    Mat image(h, w, CV_8UC3, const_cast<uint8_t*>(rgb));
    Mat m(mask.h, mask.w, CV_8UC1, const_cast<uint8_t*>(mask.data.data()));

    // build_raw_line_data
    const double angle = rotationAngle(m);
    Mat rotMask = rotateFromAngle(m, angle);
    Mat rotImg = rotateFromAngle(image, angle);
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(rotMask, contours, cv::RETR_LIST,
                     cv::CHAIN_APPROX_SIMPLE);
    std::vector<std::vector<cv::Point>> kept;
    for (auto& c : contours)
        if (cv::contourArea(c) > 10) kept.push_back(std::move(c));

    // filter_line_contours
    std::vector<std::vector<cv::Point>> filtered;
    for (auto& c : kept) {
        cv::Rect r = cv::boundingRect(c);
        if (r.width > rotMask.cols * 0.01 && r.height > 10)
            filtered.push_back(std::move(c));
    }

    // build_line_data + sort_lines_by_threshold2
    std::vector<RawLine> lines;
    for (const auto& c : filtered) lines.push_back(buildLineData(c));
    std::vector<cv::Point> centers;
    for (const auto& l : lines) centers.push_back(l.center);
    const double thr = lineThreshold(rotMask);
    auto sortedCenters = sortBboxCenters(centers, thr);
    std::vector<RawLine> sorted;
    if (mergeLines) {
        sorted = groupLineChunks(sortedCenters, lines);
    } else {
        for (const auto& group : sortedCenters)
            for (const auto& c : group)
                for (const auto& l : lines)
                    if (l.center == c) { sorted.push_back(l); break; }
    }

    // extract_line_images (adaptive k carries across lines, as upstream)
    PageLines out;
    out.angle = angle;
    out.width = rotImg.cols;
    out.height = rotImg.rows;
    out.deskewedRgb.assign(rotImg.data,
                           rotImg.data + static_cast<size_t>(rotImg.total()) *
                                             rotImg.channels());
    double currentK = kFactor;
    for (const auto& l : sorted) {
        OcrLine ol;
        ol.x = l.bbox.x;
        ol.y = l.bbox.y;
        ol.w = l.bbox.width;
        ol.h = l.bbox.height;
        ol.cx = l.center.x;
        ol.cy = l.center.y;
        out.lines.push_back(ol);

        Mat tmpMask = Mat::zeros(rotImg.rows, rotImg.cols, CV_8UC1);
        std::vector<std::vector<cv::Point>> one{l.contour};
        cv::drawContours(tmpMask, one, -1, cv::Scalar(255, 255, 255),
                         cv::FILLED);
        double adaptedK = currentK;
        Mat li = getLineImage(rotImg, tmpMask, l.bbox.height, bboxTolerance,
                              currentK, adaptedK);
        if (currentK != adaptedK) currentK = adaptedK;
        LineImage lim;
        lim.w = li.cols;
        lim.h = li.rows;
        lim.rgb.assign(li.data, li.data + static_cast<size_t>(li.total()) *
                                              li.channels());
        out.images.push_back(std::move(lim));
    }
    return out;
}

}  // namespace allocr
