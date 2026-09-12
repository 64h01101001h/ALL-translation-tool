#!/usr/bin/env python3
"""Stop only the installed app's main executable; uncertainty aborts deployment."""
import argparse
import json
from pathlib import Path, PurePosixPath
import subprocess
import sys
import tempfile
from urllib.parse import unquote, urlsplit


def file_path(url):
    if not isinstance(url, str):
        raise ValueError("device executable/app URL is not a string")
    parts = urlsplit(url)
    path = unquote(parts.path, errors="strict")
    if (parts.scheme != "file" or parts.netloc or parts.query or parts.fragment
            or not path.startswith("/") or "\0" in path
            or ".." in PurePosixPath(path).parts):
        raise ValueError("device executable/app URL is not an absolute local file URL")
    return str(PurePosixPath(path))


def query(device, kind, folder, extra=()):
    output = folder / (kind + ".json")
    subprocess.run(["xcrun", "devicectl", "device", "info", kind,
                    "--device", device, *extra, "--quiet", "--timeout", "20",
                    "--json-output", str(output)], check=True)
    with output.open(encoding="utf-8") as source:
        payload = json.load(source)
    if (not isinstance(payload, dict) or not isinstance(payload.get("info"), dict)
            or payload["info"].get("outcome") != "success"
            or not isinstance(payload.get("result"), dict)):
        raise ValueError("devicectl did not return a successful " + kind + " result")
    return payload["result"]


def terminate_existing(device, bundle_id, executable):
    if not executable or PurePosixPath(executable).name != executable or executable in (".", ".."):
        raise ValueError("executable must be a single filename")
    with tempfile.TemporaryDirectory(prefix="ios-terminate-") as tmp:
        folder = Path(tmp)
        apps = query(device, "apps", folder, ("--bundle-id", bundle_id)).get("apps")
        if not isinstance(apps, list) or any(
                not isinstance(app, dict) or not isinstance(app.get("bundleIdentifier"), str)
                for app in apps):
            raise ValueError("devicectl apps result has an invalid schema")
        matches = [app for app in apps if app["bundleIdentifier"] == bundle_id]
        if not matches:
            return  # First install: there is no installed executable to stop.
        if len(matches) != 1:
            raise ValueError("devicectl returned ambiguous installed app paths")
        app_path = file_path(matches[0].get("url"))
        if not app_path.endswith(".app"):
            raise ValueError("installed app URL does not name an app bundle")
        target = app_path + "/" + executable
        processes = query(device, "processes", folder).get("runningProcesses")
        if not isinstance(processes, list):
            raise ValueError("devicectl processes result has an invalid schema")
        pids = set()
        # Validate the entire result before stopping anything. A malformed row
        # cannot be mistaken for proof that the old app is not running.
        for process in processes:
            if not isinstance(process, dict):
                raise ValueError("devicectl process record is not an object")
            pid = process.get("processIdentifier")
            if type(pid) is not int or pid < 0:
                raise ValueError("devicectl process identifier is not a nonnegative integer")
            if file_path(process.get("executable")) == target:
                if pid == 0:
                    raise ValueError("refusing to terminate process identifier zero")
                pids.add(pid)
        for pid in sorted(pids):
            subprocess.run(["xcrun", "devicectl", "device", "process", "terminate",
                            "--device", device, "--pid", str(pid),
                            "--quiet", "--timeout", "20"], check=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--device", required=True)
    parser.add_argument("--bundle-id", required=True)
    parser.add_argument("--executable", required=True)
    args = parser.parse_args()
    try:
        terminate_existing(args.device, args.bundle_id, args.executable)
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        print("Cannot confirm the existing app is stopped: " + str(error), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
