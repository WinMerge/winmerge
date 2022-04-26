# Building WinMerge Installer

## Needed programs:

 * [Inno Setup](https://jrsoftware.org/isinfo.php) (both versions 5.x and 6.x)
 * [ISTool](https://sourceforge.net/projects/istool/) **recommended** - great help managing Inno Setup installer scripts


## Installer script:

Installer script is in `Installer/InnoSetup` folder: `WinMerge{arch}.iss` (for Inno Setup 5.x) and `WinMerge{arch}.is6.iss` (for Inno Setup 6.x)

## Checklist:

 * Installer reads version number from `WinMergeU.exe`. So make sure you have version resource correctly set.
 * Installer checks that files listed are present. But some files are included as folder (like `/Plugins/{arch}/Dlls`) or as filemask (like `*.flt`) so take extra care to make sure all needed files are present in locations shown below. 

## Folder structure and files needed:

First make sure you have all needed files compiled and available: 

 * WinMerge executable: `WinMergeU.exe`
 * Executable translations: `/Translations/WinMerge/*.po`
 * Manual: `WinMerge.chm`
 * ShellExtension: `ShellExtensionU.dll` and `ShellExtension{arch}.dll`
 * Documents: `/Docs/Users/*`
 * Filters: `/Filters/*`
 * ColorSchemes: `/ColorSchemes/*`
 * Plugins: `/Plugins/{arch}/dlls/*`
 * Other: *Frhed*, *WinIMerge*, *WinWebDiff*, *Merge7z* and *GnuWin32 Patch*
   (The above files can be downloaded by executing DownloadDeps.cmd)

These files are produced by compiling WinMerge, documentation etc, but that's not subject of this document.

### Folder structure

 * `Build/`
   * `GnuWin32/`
     * `*.*`
   * `Manual/htmlhelp/`
     * `WinMerge*.chm`
   * `ShellExtension/`
     * `ShellExtension*.dll`
     * `{arch}`
       * `WinMergeContextMenu.dll`
     * `WinMergeContextMenuPackage.msix`
   * `{arch}`
     * `Release/`
       * `Frhed/`
         * `*.*`
       * `Merge7z/`
         * `*.*`
       * `WinMergeU.exe`
 * `Docs/`
   * `Users/`
     * `ChangeLog.html`
     * `Contributors.txt`
     * `GPL.rtf`
     * `ReadMe.txt`
     * `ReleaseNotes.html`
 * `ColorSchemes/`
   * `*.ini`
 * `Filters/`
   * `FileFilter.tmpl`
   * `*.flt`
 * `Plugins/`
   * `Commands/`
     * `**/*.bat`
     * `**/*.txt`
   * `dlls/`
     * `{arch}`
       * `*.dll`
     * `*.sct`
   * `WinMerge32BitPluginProxy/Release/`
     * `WinMerge32BitPluginProxy.exe`
 * `Translations/`
   * `Docs/`
     * `*.*`
   * `InnoSetup/`
     * `*.isl`
   * `ShellExtension/`
     * `*.po`
   * `WinMerge/`
     * `*.po`

## Running Inno Setup to create installer

`WinMerge{arch}*.iss` (in InnoSetup folder) is script used to create WinMerge installer.

Rest of this section assumes ISTool is used. If not, then refer to InnoSetup manual for creating installer from script.

 1. Start ISTool and load `WinMerge.iss`
 2. Select "Project->Verify Files..." from ISTool menu to verify all needed files are present and in correct directories. Copy missing files to correct directories before continuing.
 3. Select "Project->Compile Setup" from ISTool menu to create installer. This takes a while...

If compile succeeds you have `WinMerge-[version]-Setup.exe` in folder `/Build`. You probably want to remove unnecessary zeros from the filename before uploading: if filename is `WinMerge-2.6.0.0-Setup.exe`, you may want to rename it to `WinMerge-2.6-Setup.exe`. Not a rule, but a recommendation.

**Test installer**
