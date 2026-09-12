#!/usr/bin/env python3
"""Exercise the deployment script without a phone, signing, or a native build.

The external command double enforces the captured devicectl JSON/argv contract.
Assertions cover the real script's process selection, ordering and refusal to
install after an uncertain stop. No production parsing or control flow is mocked.
"""
import copy
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest


TOOLS = Path(__file__).resolve().parent
BUNDLE = "org.asianlegacylibrary.TibetanTrainer"
APP_URL = "file:///private/var/containers/Bundle/Application/ONE/Tibetan%20Trainer.app/"
EXE_URL = APP_URL + "TibetanTrainer"


def response(result):
    return {"info": {"jsonVersion": 3, "outcome": "success", "version": "518.33"},
            "result": {"deviceIdentifier": "device-fixture", **result}}


APPS = response({"apps": [{
    "appClip": False, "builtByDeveloper": True, "bundleIdentifier": BUNDLE,
    "bundleVersion": "1", "defaultApp": False, "hidden": False,
    "internalApp": False, "name": "Tibetan Trainer", "removable": True,
    "url": APP_URL, "version": "0.1",
}], "defaultAppsIncluded": False, "hiddenAppsIncluded": False,
    "internalAppsIncluded": False, "matchingBundleIdentifier": BUNDLE,
    "removableAppsIncluded": True})
PROCESSES = response({"runningProcesses": [
    {"executable": EXE_URL.replace("%20", " "), "processIdentifier": 42},
    {"executable": EXE_URL, "processIdentifier": 43},
    {"executable": "file:///sbin/launchd", "processIdentifier": 1},
    {"executable": "file:///other/TibetanTrainer.app/TibetanTrainer", "processIdentifier": 50},
    {"executable": APP_URL + "PlugIns/widget.appex/TibetanTrainer", "processIdentifier": 51},
]})

# This is the only phone boundary. An unexpected/unsupported argv fails, just as
# the actual tool rejects --bundle-identifier instead of a --pid on terminate.
DEVICE_DOUBLE = r'''
import json, os, sys
from pathlib import Path
p = Path(os.environ["IOS_DEPLOY_TEST_STATE"])
s = json.loads(p.read_text())
a = sys.argv[1:]
s["calls"].append(a)
def done(code=0):
    p.write_text(json.dumps(s))
    sys.exit(code)
def option(name):
    try:
        return a[a.index(name) + 1]
    except (ValueError, IndexError):
        done(2)
if a == ["devicectl", "list", "devices"]:
    print("Fixture device-fixture connected")
    done()
if a[:3] != ["devicectl", "device", "info"] and a[:3] != ["devicectl", "device", "process"] and a[:4] != ["devicectl", "device", "install", "app"]:
    done(2)
if option("--device") != "device-fixture":
    done(2)
if a[:4] in (["devicectl", "device", "info", "apps"], ["devicectl", "device", "info", "processes"]):
    kind = a[3]
    if kind == "apps" and option("--bundle-id") != "org.asianlegacylibrary.TibetanTrainer":
        done(2)
    if s.get("fail") == kind:
        done(5)
    payload = s[kind]
    Path(option("--json-output")).write_text(payload if isinstance(payload, str) else json.dumps(payload))
    done()
if a[:4] == ["devicectl", "device", "process", "terminate"]:
    pid = option("--pid")
    if "--bundle-identifier" in a or s.get("fail") == "terminate":
        done(5)
    if not pid.isdigit():
        done(2)
    s["terminated"].append(int(pid))
    done()
if a[:4] == ["devicectl", "device", "install", "app"]:
    s["installed"] = True
    done()
if a[:4] == ["devicectl", "device", "process", "launch"]:
    if "--terminate-existing" not in a or s.get("fail") == "launch":
        done(2)
    s["launched"] = True
    done()
done(2)
'''


def deploy(*, apps=APPS, processes=PROCESSES, fail=None, no_launch=False):
    with tempfile.TemporaryDirectory(prefix="ios-deploy-test-") as tmp:
        root = Path(tmp)
        (root / "tools").mkdir()
        for name in ("ios_deploy.sh", "ios_terminate_existing.py"):
            if (TOOLS / name).exists():
                shutil.copy2(TOOLS / name, root / "tools" / name)
        bindir = root / "bin"
        bindir.mkdir()
        for name in ("cmake", "xcodebuild", "git"):
            tool = bindir / name
            tool.write_text("#!/bin/sh\nexit 0\n")
            tool.chmod(0o755)
        tool = bindir / "xcrun"
        tool.write_text("#!" + sys.executable + "\n" + DEVICE_DOUBLE)
        tool.chmod(0o755)
        builder = root / "build/core/build_drill_pack"
        builder.parent.mkdir(parents=True)
        builder.write_text("#!/bin/sh\nexit 0\n")
        builder.chmod(0o755)
        app = root / "derived data/Build/Products/Debug-iphoneos/TibetanTrainer.app"
        app.mkdir(parents=True)
        (app / "drills.json").write_text("{}")
        statefile = root / "state.json"
        statefile.write_text(json.dumps({"apps": apps, "processes": processes, "fail": fail,
                                        "calls": [], "terminated": [], "installed": False,
                                        "launched": False}))
        env = {**os.environ, "PATH": str(bindir) + os.pathsep + os.environ["PATH"],
               "ALL_IOS_DEVICE": "device-fixture", "ALL_IOS_DERIVED": str(root / "derived data"),
               "IOS_DEPLOY_TEST_STATE": str(statefile), "PYTHONDONTWRITEBYTECODE": "1"}
        run = subprocess.run(["bash", str(root / "tools/ios_deploy.sh")]
                             + (["--no-launch"] if no_launch else []),
                             env=env, capture_output=True, text=True, timeout=20)
        return run, json.loads(statefile.read_text())


class IOSDeployTest(unittest.TestCase):
    def test_windows_reports_an_explicit_skip_instead_of_running_posix_fixtures(self):
        # Select a harmless existing case in the child so a broken skip cannot
        # recursively invoke this test. No Windows APIs are needed to test the
        # entry point's unsupported-platform decision.
        code = ("import runpy, sys; path = sys.argv[1]; "
                "runpy.run_path(path, run_name='ios_deploy_portability_import'); "
                "sys.platform = 'win32'; "
                "sys.argv = [path, 'IOSDeployTest.test_first_install_and_already_stopped_app_are_valid']; "
                "runpy.run_path(path, run_name='__main__')")
        run = subprocess.run([sys.executable, "-c", code, str(Path(__file__).resolve())],
                             capture_output=True, text=True, timeout=20)
        self.assertEqual(run.returncode, 77, run.stdout + run.stderr)
        self.assertIn("SKIP", run.stdout)
        self.assertIn("Windows", run.stdout)

    def test_stops_only_exact_installed_executable_before_install_in_both_modes(self):
        for no_launch in (False, True):
            with self.subTest(no_launch=no_launch):
                run, state = deploy(no_launch=no_launch)
                self.assertEqual(run.returncode, 0, run.stdout + run.stderr)
                self.assertEqual(state["terminated"], [42, 43])
                self.assertTrue(state["installed"])
                self.assertEqual(state["launched"], not no_launch)
                actions = [c[3] for c in state["calls"] if c[:2] == ["devicectl", "device"]]
                self.assertEqual(actions, ["apps", "processes", "terminate", "terminate", "app"]
                                 + ([] if no_launch else ["launch"]))

    def test_first_install_and_already_stopped_app_are_valid(self):
        for apps, processes in ((response({"apps": []}), PROCESSES),
                                (APPS, response({"runningProcesses": PROCESSES["result"]["runningProcesses"][2:]}))):
            with self.subTest(apps=apps):
                run, state = deploy(apps=apps, processes=processes, no_launch=True)
                self.assertEqual(run.returncode, 0, run.stdout + run.stderr)
                self.assertEqual(state["terminated"], [])
                self.assertTrue(state["installed"])
                self.assertFalse(state["launched"])

    def test_query_or_termination_error_prevents_install_and_launch(self):
        for failure in ("apps", "processes", "terminate"):
            with self.subTest(failure=failure):
                run, state = deploy(fail=failure)
                self.assertNotEqual(run.returncode, 0)
                self.assertFalse(state["installed"])
                self.assertFalse(state["launched"])

    def test_failed_launch_is_reported_after_successful_install(self):
        run, state = deploy(fail="launch")
        self.assertNotEqual(run.returncode, 0)
        self.assertTrue(state["installed"])
        self.assertFalse(state["launched"])
        self.assertNotIn("ON THE PHONE:", run.stdout)

    def test_unreadable_or_ambiguous_device_data_prevents_install(self):
        invalid = [
            ("apps", "not JSON"),
            ("apps", response({})),
            ("apps", response({"apps": {}})),
            ("apps", response({"apps": [{"bundleIdentifier": BUNDLE}]})),
            ("apps", response({"apps": APPS["result"]["apps"] * 2})),
            ("processes", "not JSON"),
            ("processes", response({})),
            ("processes", response({"runningProcesses": {}})),
            ("processes", response({"runningProcesses": [{"executable": EXE_URL, "processIdentifier": "42"}]})),
            ("processes", response({"runningProcesses": [{"executable": EXE_URL, "processIdentifier": True}]})),
            ("processes", response({"runningProcesses": [{"executable": "TibetanTrainer", "processIdentifier": 42}]})),
        ]
        failed_outcome = copy.deepcopy(APPS)
        failed_outcome["info"]["outcome"] = "failure"
        invalid.append(("apps", failed_outcome))
        for kind, payload in invalid:
            with self.subTest(kind=kind, payload=payload):
                run, state = deploy(**{kind: payload}, no_launch=True)
                self.assertNotEqual(run.returncode, 0)
                self.assertFalse(state["installed"])
                self.assertEqual(state["terminated"], [])


if __name__ == "__main__":
    if sys.platform == "win32":
        print("[SKIP] ios_deploy_selftest: Windows does not support these Bash/POSIX executable fixtures.")
        sys.exit(77)
    unittest.main()
