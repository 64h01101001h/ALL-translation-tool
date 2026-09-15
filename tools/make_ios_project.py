#!/usr/bin/env python3
"""Generate ios/TibetanTrainer.xcodeproj.

The project is GENERATED rather than committed as a hand-edited blob, because
a .pbxproj is a merge-hostile file that Xcode rewrites on every incidental
click. Generating it means the app's build settings live in this readable
script, and a project mangled by an accidental drag can be thrown away and
remade in a second.

It uses Xcode 16+ synchronized folders (objectVersion 77): the target
references the DiamondDrills/ FOLDER, so adding a Swift file or an asset is
picked up with no project edit at all.

Signing is left as "Automatic" with an EMPTY team on purpose. Adam signs in
once in Xcode (Settings ▸ Accounts), picks Geshe Michael's team in the target's Signing
tab, and Xcode mints the certificate and provisioning profile itself. Neither
this script nor anyone else needs Geshe Michael's credentials.
"""
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PROJ = os.path.join(ROOT, "ios", "TibetanTrainer.xcodeproj")

APP_NAME = "Tibetan Trainer"
PRODUCT = "TibetanTrainer"
BUNDLE_ID = "org.asianlegacylibrary.TibetanTrainer"
DEPLOY = "17.0"
SRC_FOLDER = "DiamondDrills"

# Stable 24-hex object ids. Fixed rather than random so regenerating the
# project produces a byte-identical file and git shows no spurious diff.
ID = {k: ("%024X" % (0xA11C0DE000000000000000 + i))
      for i, k in enumerate([
          "project", "target", "mainGroup", "productsGroup", "productRef",
          "syncGroup", "sourcesPhase", "frameworksPhase", "resourcesPhase",
          "projConfigList", "targetConfigList",
          "projDebug", "projRelease", "targetDebug", "targetRelease"])}

PROJ_COMMON = {
    "ALWAYS_SEARCH_USER_PATHS": "NO",
    "CLANG_ENABLE_OBJC_ARC": "YES",
    "ENABLE_STRICT_OBJC_MSGSEND": "YES",
    "GCC_NO_COMMON_BLOCKS": "YES",
    "IPHONEOS_DEPLOYMENT_TARGET": DEPLOY,
    "SDKROOT": "iphoneos",
    # Without this the scheme offers NO simulator destination, and a
    # simulator build is how the project is proved without a signing
    # certificate.
    "SUPPORTED_PLATFORMS": '"iphoneos iphonesimulator"',
    "SWIFT_VERSION": "5.0",
    "TARGETED_DEVICE_FAMILY": '"1,2"',
}
TARGET_COMMON = {
    "ASSETCATALOG_COMPILER_APPICON_NAME": "AppIcon",
    "CODE_SIGN_STYLE": "Automatic",
    "DEVELOPMENT_TEAM": "@TEAM@",
    "CURRENT_PROJECT_VERSION": "1",
    "MARKETING_VERSION": "0.1",
    "GENERATE_INFOPLIST_FILE": "YES",
    "INFOPLIST_KEY_CFBundleDisplayName": '"%s"' % APP_NAME,
    "INFOPLIST_KEY_UILaunchScreen_Generation": "YES",
    "INFOPLIST_KEY_UISupportedInterfaceOrientations": (
        '"UIInterfaceOrientationPortrait '
        'UIInterfaceOrientationLandscapeLeft '
        'UIInterfaceOrientationLandscapeRight"'),
    "PRODUCT_BUNDLE_IDENTIFIER": BUNDLE_ID,
    "PRODUCT_NAME": '"$(TARGET_NAME)"',
    "SWIFT_EMIT_LOC_STRINGS": "YES",
    "ENABLE_PREVIEWS": "YES",
}


def developer_team():
    """Adam's 10-character Apple Developer Team ID, if this machine has it.

    A Team ID is not a secret — it is stamped into every provisioning profile
    and is visible in the Developer portal — but this repository is PUBLIC, so
    it is kept out of the tree anyway and read from a local file or the
    environment. Blank is fine: with automatic signing Xcode fills the team in
    from whoever is signed in, and the only cost of leaving it blank is
    re-picking it in the GUI after this script regenerates the project.

    Never put an Apple ID password, an App Store Connect API key (.p8), or a
    2FA code here. None of those are needed to build, and none should exist in
    this repository or pass through this script.
    """
    v = os.environ.get("ALL_IOS_TEAM", "").strip()
    if v:
        return v
    p = os.path.join(ROOT, "ios", "DeveloperTeam.txt")
    if os.path.exists(p):
        with open(p) as f:
            for line in f:
                line = line.split("#", 1)[0].strip()
                if line:
                    return line
    return ""


def settings(d, indent):
    pad = "\t" * indent
    return "".join("%s%s = %s;\n" % (pad, k, v) for k, v in sorted(d.items()))


TEMPLATE = r"""// !$*UTF8*$!
{
\tarchiveVersion = 1;
\tclasses = {
\t};
\tobjectVersion = 77;
\tobjects = {

/* Begin PBXFileReference section */
\t\t@productRef@ /* @product@.app */ = {isa = PBXFileReference; explicitFileType = wrapper.application; includeInIndex = 0; path = @product@.app; sourceTree = BUILT_PRODUCTS_DIR; };
/* End PBXFileReference section */

/* Begin PBXFileSystemSynchronizedRootGroup section */
\t\t@syncGroup@ /* @src@ */ = {
\t\t\tisa = PBXFileSystemSynchronizedRootGroup;
\t\t\tpath = @src@;
\t\t\tsourceTree = "<group>";
\t\t};
/* End PBXFileSystemSynchronizedRootGroup section */

/* Begin PBXFrameworksBuildPhase section */
\t\t@frameworksPhase@ = {
\t\t\tisa = PBXFrameworksBuildPhase;
\t\t\tbuildActionMask = 2147483647;
\t\t\tfiles = (
\t\t\t);
\t\t\trunOnlyForDeploymentPostprocessing = 0;
\t\t};
/* End PBXFrameworksBuildPhase section */

/* Begin PBXGroup section */
\t\t@mainGroup@ = {
\t\t\tisa = PBXGroup;
\t\t\tchildren = (
\t\t\t\t@syncGroup@ /* @src@ */,
\t\t\t\t@productsGroup@ /* Products */,
\t\t\t);
\t\t\tsourceTree = "<group>";
\t\t};
\t\t@productsGroup@ /* Products */ = {
\t\t\tisa = PBXGroup;
\t\t\tchildren = (
\t\t\t\t@productRef@ /* @product@.app */,
\t\t\t);
\t\t\tname = Products;
\t\t\tsourceTree = "<group>";
\t\t};
/* End PBXGroup section */

/* Begin PBXNativeTarget section */
\t\t@target@ /* @product@ */ = {
\t\t\tisa = PBXNativeTarget;
\t\t\tbuildConfigurationList = @targetConfigList@ /* Build configuration list for PBXNativeTarget "@product@" */;
\t\t\tbuildPhases = (
\t\t\t\t@sourcesPhase@ /* Sources */,
\t\t\t\t@frameworksPhase@ /* Frameworks */,
\t\t\t\t@resourcesPhase@ /* Resources */,
\t\t\t);
\t\t\tbuildRules = (
\t\t\t);
\t\t\tdependencies = (
\t\t\t);
\t\t\tfileSystemSynchronizedGroups = (
\t\t\t\t@syncGroup@ /* @src@ */,
\t\t\t);
\t\t\tname = @product@;
\t\t\tproductName = @product@;
\t\t\tproductReference = @productRef@ /* @product@.app */;
\t\t\tproductType = "com.apple.product-type.application";
\t\t};
/* End PBXNativeTarget section */

/* Begin PBXProject section */
\t\t@project@ /* Project object */ = {
\t\t\tisa = PBXProject;
\t\t\tattributes = {
\t\t\t\tBuildIndependentTargetsInParallel = 1;
\t\t\t\tLastSwiftUpdateCheck = 2660;
\t\t\t\tLastUpgradeCheck = 2660;
\t\t\t\tTargetAttributes = {
\t\t\t\t\t@target@ = {
\t\t\t\t\t\tCreatedOnToolsVersion = 26.6;
\t\t\t\t\t};
\t\t\t\t};
\t\t\t};
\t\t\tbuildConfigurationList = @projConfigList@ /* Build configuration list for PBXProject "@product@" */;
\t\t\tdevelopmentRegion = en;
\t\t\thasScannedForEncodings = 0;
\t\t\tknownRegions = (
\t\t\t\ten,
\t\t\t\tBase,
\t\t\t);
\t\t\tmainGroup = @mainGroup@;
\t\t\tminimizedProjectReferenceProxies = 1;
\t\t\tpreferredProjectObjectVersion = 77;
\t\t\tproductRefGroup = @productsGroup@ /* Products */;
\t\t\tprojectDirPath = "";
\t\t\tprojectRoot = "";
\t\t\ttargets = (
\t\t\t\t@target@ /* @product@ */,
\t\t\t);
\t\t};
/* End PBXProject section */

/* Begin PBXResourcesBuildPhase section */
\t\t@resourcesPhase@ = {
\t\t\tisa = PBXResourcesBuildPhase;
\t\t\tbuildActionMask = 2147483647;
\t\t\tfiles = (
\t\t\t);
\t\t\trunOnlyForDeploymentPostprocessing = 0;
\t\t};
/* End PBXResourcesBuildPhase section */

/* Begin PBXSourcesBuildPhase section */
\t\t@sourcesPhase@ = {
\t\t\tisa = PBXSourcesBuildPhase;
\t\t\tbuildActionMask = 2147483647;
\t\t\tfiles = (
\t\t\t);
\t\t\trunOnlyForDeploymentPostprocessing = 0;
\t\t};
/* End PBXSourcesBuildPhase section */

/* Begin XCBuildConfiguration section */
\t\t@projDebug@ /* Debug */ = {
\t\t\tisa = XCBuildConfiguration;
\t\t\tbuildSettings = {
@projDebugSettings@\t\t\t};
\t\t\tname = Debug;
\t\t};
\t\t@projRelease@ /* Release */ = {
\t\t\tisa = XCBuildConfiguration;
\t\t\tbuildSettings = {
@projReleaseSettings@\t\t\t};
\t\t\tname = Release;
\t\t};
\t\t@targetDebug@ /* Debug */ = {
\t\t\tisa = XCBuildConfiguration;
\t\t\tbuildSettings = {
@targetSettings@\t\t\t};
\t\t\tname = Debug;
\t\t};
\t\t@targetRelease@ /* Release */ = {
\t\t\tisa = XCBuildConfiguration;
\t\t\tbuildSettings = {
@targetSettings@\t\t\t};
\t\t\tname = Release;
\t\t};
/* End XCBuildConfiguration section */

/* Begin XCConfigurationList section */
\t\t@projConfigList@ /* Build configuration list for PBXProject "@product@" */ = {
\t\t\tisa = XCConfigurationList;
\t\t\tbuildConfigurations = (
\t\t\t\t@projDebug@ /* Debug */,
\t\t\t\t@projRelease@ /* Release */,
\t\t\t);
\t\t\tdefaultConfigurationIsVisible = 0;
\t\t\tdefaultConfigurationName = Release;
\t\t};
\t\t@targetConfigList@ /* Build configuration list for PBXNativeTarget "@product@" */ = {
\t\t\tisa = XCConfigurationList;
\t\t\tbuildConfigurations = (
\t\t\t\t@targetDebug@ /* Debug */,
\t\t\t\t@targetRelease@ /* Release */,
\t\t\t);
\t\t\tdefaultConfigurationIsVisible = 0;
\t\t\tdefaultConfigurationName = Release;
\t\t};
/* End XCConfigurationList section */
\t};
\trootObject = @project@ /* Project object */;
}
"""


def pbxproj():
    debug = dict(PROJ_COMMON, ONLY_ACTIVE_ARCH="YES",
                 SWIFT_OPTIMIZATION_LEVEL="\"-Onone\"",
                 SWIFT_ACTIVE_COMPILATION_CONDITIONS="DEBUG",
                 GCC_OPTIMIZATION_LEVEL="0", ENABLE_TESTABILITY="YES",
                 DEBUG_INFORMATION_FORMAT="dwarf")
    release = dict(PROJ_COMMON, SWIFT_OPTIMIZATION_LEVEL="\"-O\"",
                   SWIFT_COMPILATION_MODE="wholemodule",
                   VALIDATE_PRODUCT="YES",
                   DEBUG_INFORMATION_FORMAT='"dwarf-with-dsym"')
    debug = dict(PROJ_COMMON, ONLY_ACTIVE_ARCH="YES",
                 SWIFT_OPTIMIZATION_LEVEL="\"-Onone\"",
                 SWIFT_ACTIVE_COMPILATION_CONDITIONS="DEBUG",
                 GCC_OPTIMIZATION_LEVEL="0", ENABLE_TESTABILITY="YES",
                 DEBUG_INFORMATION_FORMAT="dwarf")
    release = dict(PROJ_COMMON, SWIFT_OPTIMIZATION_LEVEL="\"-O\"",
                   SWIFT_COMPILATION_MODE="wholemodule",
                   VALIDATE_PRODUCT="YES",
                   DEBUG_INFORMATION_FORMAT='"dwarf-with-dsym"')
    out = TEMPLATE
    subs = dict(ID)
    subs["product"] = PRODUCT
    subs["src"] = SRC_FOLDER
    subs["projDebugSettings"] = settings(debug, 4)
    subs["projReleaseSettings"] = settings(release, 4)
    subs["targetSettings"] = settings(TARGET_COMMON, 4)
    # An empty value is a parse error in a .pbxproj, so a blank team
    # must still be written as a quoted empty string.
    team = developer_team()
    subs["TEAM"] = team if team else '""'
    for k, v in subs.items():
        out = out.replace("@%s@" % k, v)
    # The template is a raw string so its braces survive untouched; that also
    # keeps \t as two characters, and a .pbxproj is tab-indented.
    return out.replace("\\t", "\t")


SCHEME = """<?xml version="1.0" encoding="UTF-8"?>
<Scheme LastUpgradeVersion = "2660" version = "1.7">
   <BuildAction parallelizeBuildables = "YES" buildImplicitDependencies = "YES">
      <BuildActionEntries>
         <BuildActionEntry buildForTesting = "YES" buildForRunning = "YES"
                           buildForProfiling = "YES" buildForArchiving = "YES"
                           buildForAnalyzing = "YES">
            <BuildableReference BuildableIdentifier = "primary"
               BlueprintIdentifier = "{target}" BuildableName = "{product}.app"
               BlueprintName = "{product}" ReferencedContainer = "container:TibetanTrainer.xcodeproj">
            </BuildableReference>
         </BuildActionEntry>
      </BuildActionEntries>
   </BuildAction>
   <TestAction buildConfiguration = "Debug" selectedDebuggerIdentifier = "Xcode.DebuggerFoundation.Debugger.LLDB"
      selectedLauncherIdentifier = "Xcode.DebuggerFoundation.Launcher.LLDB" shouldUseLaunchSchemeArgsEnv = "YES">
      <Testables></Testables>
   </TestAction>
   <LaunchAction buildConfiguration = "Debug" selectedDebuggerIdentifier = "Xcode.DebuggerFoundation.Debugger.LLDB"
      selectedLauncherIdentifier = "Xcode.DebuggerFoundation.Launcher.LLDB" launchStyle = "0"
      useCustomWorkingDirectory = "NO" ignoresPersistentStateOnLaunch = "NO"
      debugDocumentVersioning = "YES" debugServiceExtension = "internal" allowLocationSimulation = "YES">
      <BuildableProductRunnable runnableDebuggingMode = "0">
         <BuildableReference BuildableIdentifier = "primary"
            BlueprintIdentifier = "{target}" BuildableName = "{product}.app"
            BlueprintName = "{product}" ReferencedContainer = "container:TibetanTrainer.xcodeproj">
         </BuildableReference>
      </BuildableProductRunnable>
   </LaunchAction>
   <ProfileAction buildConfiguration = "Release" shouldUseLaunchSchemeArgsEnv = "YES"
      savedToolIdentifier = "" useCustomWorkingDirectory = "NO" debugDocumentVersioning = "YES">
      <BuildableProductRunnable runnableDebuggingMode = "0">
         <BuildableReference BuildableIdentifier = "primary"
            BlueprintIdentifier = "{target}" BuildableName = "{product}.app"
            BlueprintName = "{product}" ReferencedContainer = "container:TibetanTrainer.xcodeproj">
         </BuildableReference>
      </BuildableProductRunnable>
   </ProfileAction>
   <AnalyzeAction buildConfiguration = "Debug"></AnalyzeAction>
   <ArchiveAction buildConfiguration = "Release" revealArchiveInOrganizer = "YES"></ArchiveAction>
</Scheme>
""".format(target=ID["target"], product=PRODUCT)


def main():
    os.makedirs(PROJ, exist_ok=True)
    with open(os.path.join(PROJ, "project.pbxproj"), "w") as f:
        f.write(pbxproj())
    sd = os.path.join(PROJ, "xcshareddata", "xcschemes")
    os.makedirs(sd, exist_ok=True)
    with open(os.path.join(sd, "%s.xcscheme" % PRODUCT), "w") as f:
        f.write(SCHEME)
    print("wrote %s" % PROJ)
    print("  target      %s" % PRODUCT)
    print("  bundle id   %s" % BUNDLE_ID)
    print("  sources     ios/%s/  (synchronized folder - new files need no edit)" % SRC_FOLDER)
    team = developer_team()
    print("  signing     Automatic, team %s"
          % (team if team else "blank - pick it once in Xcode, or put it in "
                              "ios/DeveloperTeam.txt"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
