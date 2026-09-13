// Does QDialogButtonBox re-assert a default button on show, undoing
// setDefault(false)? And does Return in a QLineEdit fire it?
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPushButton>
#include <cstdio>

// The candidate fix: Return inside a line edit moves to the next field, the
// reflex the user actually has, and never reaches the dialog's default button.
// Robust against QDialogButtonBox re-asserting a default on show, which is
// what made setDefault(false) inert.
struct ReturnMovesFocus : QObject {
    using QObject::QObject;
    bool eventFilter(QObject* o, QEvent* e) override {
        if (e->type() == QEvent::KeyPress) {
            auto* k = static_cast<QKeyEvent*>(e);
            if (k->key() == Qt::Key_Return || k->key() == Qt::Key_Enter) {
                if (auto* w = qobject_cast<QWidget*>(o)) {
                    // focusNextChild() is protected; focusNextPrevChild is
                    // reached through the public focus chain instead.
                    if (QWidget* nx = w->nextInFocusChain())
                        nx->setFocus(Qt::TabFocusReason);
                    return true;
                }
            }
        }
        return QObject::eventFilter(o, e);
    }
};

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QWidget main; main.show(); QApplication::processEvents();   // like the app
    QDialog d;
    auto* form = new QFormLayout(&d);
    auto* fill = new QPushButton("Auto-fill from catalog");
    form->addRow(fill);
    auto* hy = new QPushButton("Pair-hyphenate author");
    form->addRow(hy);
    auto* le = new QLineEdit;
    form->addRow("Title", le);
    auto* box = new QDialogButtonBox;
    auto* ins = box->addButton("Insert into draft", QDialogButtonBox::AcceptRole);
    ins->setAutoDefault(false);
    ins->setDefault(false);
    form->addRow(box);
    fill->setAutoDefault(false);
    hy->setAutoDefault(false);
    // inert fix only

    std::printf("BEFORE show:\n");
    std::printf("  fill  autoDefault=%d default=%d\n", fill->autoDefault(), fill->isDefault());
    std::printf("  hy    autoDefault=%d default=%d\n", hy->autoDefault(), hy->isDefault());
    std::printf("  ins   autoDefault=%d default=%d\n", ins->autoDefault(), ins->isDefault());

    d.show();
    QApplication::processEvents();

    std::printf("AFTER show:\n");
    std::printf("  fill  autoDefault=%d default=%d\n", fill->autoDefault(), fill->isDefault());
    std::printf("  hy    autoDefault=%d default=%d\n", hy->autoDefault(), hy->isDefault());
    std::printf("  ins   autoDefault=%d default=%d\n", ins->autoDefault(), ins->isDefault());

    int fired = 0;
    QObject::connect(fill, &QPushButton::clicked, [&]{ fired = 1; });
    QObject::connect(hy,   &QPushButton::clicked, [&]{ fired = 2; });
    QObject::connect(ins,  &QPushButton::clicked, [&]{ fired = 3; });

    le->setFocus();
    QApplication::processEvents();
    QKeyEvent press(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(le, &press);
    QApplication::processEvents();
    std::printf("Return in the line edit fired: %s\n",
                fired == 0 ? "NOTHING" : fired == 1 ? "Auto-fill"
                           : fired == 2 ? "Pair-hyphenate" : "Insert into draft");
    return 0;
}
