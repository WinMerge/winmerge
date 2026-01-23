# WinMerge

[![logo](Docs/Logos/WinMerge_logo_24bit.png)](https://github.com/WinMerge/winmerge)

[WinMerge](https://winmerge.org/) is an open-source differencing and merging tool for Windows.  
It compares files and folders and presents differences in a clear, visual format that is easy to understand and work with.  
WinMerge has been actively developed since 2000.

[![Build status](https://ci.appveyor.com/api/projects/status/h3v3ap1kswi1tyyt?svg=true)](https://ci.appveyor.com/project/sdottaka/winmerge/build/artifacts)
[![CI](https://github.com/WinMerge/winmerge/workflows/CI/badge.svg)](https://github.com/WinMerge/winmerge/actions)
[![sourceforge.net downloads](https://img.shields.io/sourceforge/dt/winmerge)](https://sourceforge.net/projects/winmerge/files/)
[![GitHub Releases](https://img.shields.io/github/downloads/winmerge/winmerge/total.svg)](https://github.com/WinMerge/winmerge/releases/latest)
[![Translation status](https://img.shields.io/badge/translations-39-green)](https://github.com/WinMerge/winmerge/blob/master/Translations/TranslationsStatus.md)

---

## What Can WinMerge Do?

WinMerge provides a rich set of features for comparing and merging files and folders:

- **File Comparison**  
  Compare two or three text files side by side, with differences highlighted line by line.  
  Inline differences within a line can also be shown.

- **Folder Comparison**  
  Compare the contents of two folders to see which files differ, are missing, or exist only on one side.  
  Folder comparisons support **advanced filter expressions**, allowing you to include or exclude files and folders based on names, paths, sizes, timestamps, and other attributes.

- **Merging**  
  Merge changes between files or folders by selectively applying differences from one side to the other.

- **Syntax Highlighting**  
  Syntax highlighting for many programming languages and file formats helps make code and structured text easier to read and compare.

- **Patch File Creation**  
  Generate patch files in normal, context, or unified diff formats.

- **Flexible Ignore Options**  
  Ignore differences such as whitespace changes, case differences, or lines matching regular expressions.

- **Shell Integration**  
  Integrates with Windows Explorer, enabling direct comparison via the right-click context menu.

- **Archive Support**  
  Using 7-Zip, WinMerge can compare files inside many archive formats as if they were normal folders.

In short, WinMerge is useful for anyone who needs to track changes, compare versions of files or directories, or merge modifications efficiently.

---

## How to Contribute

If you find a bug or would like to request a feature, please [submit an issue](https://github.com/WinMerge/winmerge/issues).

To contribute code:

1. [Fork the WinMerge repository](https://github.com/WinMerge/winmerge/fork)
2. Create a feature branch on your fork
3. Follow the existing coding style (use [Allman indentation](https://en.wikipedia.org/wiki/Indentation_style#Allman_style))
4. Submit a [Pull Request](https://github.com/WinMerge/winmerge/pulls) describing your changes

---

## Folder Structure

The WinMerge repository is organized as follows:

- `ArchiveSupport/Merge7z`  
  Merge7z DLLs that connect WinMerge with 7-Zip. Required to build `WinMergeU.exe`.  
  A standalone installer for these DLLs is also included.

- `Build`  
  Output directory created during compilation. Contains executables, libraries, manuals, etc.

- `BuildTmp`  
  Temporary files created during compilation. Safe to delete.

- `ColorSchemes`  
  Color schemes and themes.

- `Docs`  
  User and developer documentation. Open `Docs/index.html` to browse locally.

- `Externals`  
  Third-party libraries used by WinMerge (some customized).

- `Filters`  
  File and folder filter definitions.

- `Installer`  
  WinMerge installer sources.

- `Plugins`  
  Runtime plugin DLLs and scripts.

- `ShellExtension`  
  Windows Explorer shell extension that adds context menu entries.

- `Src`  
  Main WinMerge source code.

- `Testing`  
  Test files and scripts, including unit tests based on the  
  [Google Test Framework](https://github.com/google/googletest).

- `Tools/Scripts`  
  Various development utilities.

- `Translation`  
  Language and translation files.

The changelog is available at:  
[Docs/Users/ChangeLog.md](https://github.com/WinMerge/winmerge/blob/master/Docs/Users/ChangeLog.md)

---

## How to Run and Debug

WinMerge provides multiple Visual Studio solution files (`.sln`) for building and debugging.

After running one of the `BuildAll` scripts, you can launch WinMerge from:

Build\X64\Release\WinMergeU.exe

If you built for a different architecture, check the corresponding output folder.
To debug, you can also start WinMerge normally and attach Visual Studio to the running process.

---

## Build WinMerge

### Visual Studio 2017

- Community / Professional / Enterprise
- VC++ 2017 (v141) toolset
- Visual C++ MFC and ATL
- Windows 10 SDK
- Optional: Windows XP support for C++

### Visual Studio 2022 or later

- Community / Professional / Enterprise
- MSVC v143 or newer
- Visual C++ MFC and ATL
- Windows 10 SDK

### Additional Tools

- git
- Inno Setup 5.x and 6.x
- 7-Zip
- Python
- Pandoc
- MSYS2 (including `po4a` and `diffutils`)

## How to Build

```bash
git clone --recurse-submodules https://github.com/WinMerge/winmerge
cd winmerge
DownloadDeps.cmd
BuildAll.cmd [x86|x64|ARM64]
# or
BuildAll.vs2022.cmd [x86|x64|ARM64]
# or
BuildAll.vs2017.cmd [x86|x64|ARM|ARM64]
```
