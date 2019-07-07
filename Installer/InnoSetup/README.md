# Building WinMerge Installer

## Needed programs:

 * [Inno Setup](http://www.jrsoftware.org/)
 * [ISTool](https://sourceforge.net/projects/istool/) **recommended** - great help managing Inno Setup installer scripts

There is download package called "QuickStart Pack" available in Inno Setup downloads. It contains all needed components in one installer.

## Installer script:

Installer script is in `Installer/InnoSetup` folder: `WinMerge.iss`

## Checklist:

 * Installer reads version number from `WinMergeU.exe`. So make sure you have version resource correctly set.
 * Installer checks that files listed are present. But some files are included as folder (like `/Plugins/Dlls`) or as filemask (like `*.flt`) so take extra care to make sure all needed files are present in locations shown below. 

## Folder structure and files needed:

First make sure you have all needed files compiled and available: 

 * WinMerge executable: `WinMergeU.exe`
 * Executable translations: `/Translations/WinMerge/*.po`
 * Manual: `WinMerge.chm`
 * ShellExtension: `ShellExtensionU.dll` and `ShellExtensionX64.dll`
 * Documents: `/Docs/Users/*`
 * Filters: `/Filters/*`
 * Plugins: `/Plugins/dlls/*`
 * Other: *Frhed*, *WinIMerge*, *Merge7z* and *GnuWin32 Patch*

These files are produced by compiling WinMerge, documentation etc, but that's not subject of this document.

### Folder structure

 * `Build/`
   * `Frhed/`
     * `*.*`
   * `GnuWin32/`
     * `*.*`
   * `Manual/htmlhelp/`
     * `WinMerge.chm`
   * `Merge7z/`
     * `*.*`
   * `Release/`
     * `WinMergeU.exe`
   * `WinIMerge/`
     * `*.*`
   * `ShellExtension/`
     * `ShellExtensionU.dll`
     * `ShellExtensionX64.dll`
   * `x64`
     * `Frhed/`
       * `*.*`
     * `Merge7z/`
       * `*.*`
     * `Release/`
       * `WinMergeU.exe`
 * `Docs/`
   * `Users/`
     * `ChangeLog.txt`
     * `Contributors.txt`
     * `GPL.rtf`
     * `ReadMe.txt`
     * `ReleaseNotes.html`
 * `Filters/`
   * `FileFilter.tmpl`
   * `*.flt`
 * `Plugins/`
   * `dlls/`
     * `*.dll`
     * `*.sct`
     * `X64/`
       * `*.dll`
   * `WinMerge32BitPluginProxy/Release/`
     * `WinMerge32BitPluginProxy.exe`
 * `Translations/`
   * `Docs/`
     * `*.*`
   * `InnoSetup/`
     * `*.isl`
   * `WinMerge/`
     * `*.po`

## Running Inno Setup to create installer

`WinMerge.iss` (in InnoSetup folder) is script used to create WinMerge installer.

Rest of this section assumes ISTool is used. If not, then refer to InnoSetup manual for creating installer from script.

 1. Start ISTool and load `WinMerge.iss`
 2. Select "Project->Verify Files..." from ISTool menu to verify all needed files are present and in correct directories. Copy missing files to correct directories before continuing.
 3. Select "Project->Compile Setup" from ISTool menu to create installer. This takes a while...

If compile succeeds you have `WinMerge-[version]-Setup.exe` in folder `/Build`. You probably want to remove unnecessary zeros from the filename before uploading: if filename is `WinMerge-2.6.0.0-Setup.exe`, you may want to rename it to `WinMerge-2.6-Setup.exe`. Not a rule, but a recommendation.

**Test installer**
