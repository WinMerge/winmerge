# WinMerge 2.16.30 Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.30?](#what-is-new-in-21630)
- [What Is New in 2.16.29 beta?](#what-is-new-in-21629-beta)
- [Known issues](#known-issues)

April 2023

## About This Release

This is a WinMerge 2.16.30 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.30

### General

- BugFix: When using an ini file all differences are displayed as black
    sections (#1799)
- Reduced file size of WinMergeU.exe for 32-bit version

### File compare

- BugFix: Fixed an issue where XML format files, such as vcxproj, were not
    being syntax highlighted.
- BugFix: Fixed an issue where the EOL character was not being displayed in
    the ARM64 version.
- Modify the "Go to" dialog. (PR #1779)

### Image compare

- BugFix: Middle Click to close image tab crash (#1785)
- BugFix: Fixed an issue where the changed Color distance threshold value
    would revert back to 0 the next time WinMerge was started.
- Image compare: Increase color distance threshold (CD Threshold)
    (winmerge/winimerge #29)

### Filters

- Updated file filters to allow UTF-8 without BOM.

### Options dialog

- BugFix: Fixed an issue where the shell integration category page in the
    Options dialog was not displaying correctly in version 2.16.29.
- BugFix: Fixed an issue where plugin settings were not exported when
    exporting from the Options dialog.

### Plugins

- BugFix: Fixed an issue where the ApplyPatch plugin was not functioning
    correctly when the 'Plugins -> Manual Unpacking' menu item was checked.
- BugFix: Fixed the problem that the plug-in setting window cannot be opened
    on 32-bit OS
- Added a "Plugin Settings" button to the "Select Plugin" window.
- Make the plugin settings dialog translatable and enable saving settings to
    an INI file (PR #1783)

### Command line

- BugFix: Fixed the problem that the /fr option is ignored when specifying the
    command line option /new
- BugFix: Fixed the problem of not moving to the conflicted line after
    auto-merge.

### Project file

- BugFix: Opening a project fails when a path has environmental variables in
    it. (#1793)

### Installer

- BugFix: Unsuccessful installation, Portable Win32 version (#1802) 

### Translations

- Translation updates:
  - Brazilian (PR #1806)
  - Chinese Simplified (PR #1808)
  - German (PR #1807)
  - Hungarian (PR #1809)
  - Japanese
  - Korean (PR #1774,#1788,#1790,#1791)
  - Lithuanian (PR #1817)
  - Polish (PR #1815)
  - Portuguese (PR #1787)
  - Slovenian (#1812)

## What Is New in 2.16.29 Beta

### File compare

- BugFix: 3-way compare does not properly align identical lines when resolving
  conflicts, and show false highlights (#1696)
- BugFix: Failure to indent lines properly (#1740)
- BugFix: Fixed problems with non-default Diff algorithm:
  - Even though the Ignore carriage return differences option was disabled,
      EOL differences were ignored when the option to ignore whitespace
      differences was enabled.
  - Differences between CR and CR+LF were not ignored even though Ignore
      carriage return differences option was enabled.
- BugFix: Fixed a bug with Match whole word only options of Substitution
    filters
- Add MATLAB syntax highlighting. (PR #1766)

### Table compare

- The "View > Wrap Lines" menu item in the Table Compare window is now
  "Wrap Text" and its check status is saved separately from the same menu item
  in the Text Compare window. (osdn #47553)
- Added an option to change the CSV file separator from comma to semicolon or
    another character.

### Binary compare

- BugFix: Resizing WinMerge Window looses the correct scroll position for the
    memory address your cursor is at (WinMerge/frhed #13)
- BugFix: Fixed an issue where an infinite loop occurred when the file size was
    truncated during file loading.

### Image compare

- BugFix: Fixed issue preventing saving image comparison reports to a network
    share

### Webpage compare

- BugFix: Fixed issue with missing file path in header bar

### Folder compare

- BugFix: After I collapse a folder, the arrow next to it stays turned down
    (#1747)
- BugFix: Fixed an issue where the Left/Right Date and Left/Right Size columns
    would not update when deleting files on one side.
- Implement issue #1413: "Move" needs options "Left to Right" or
    "Right to Left" like "Copy" (PR #1732,#1720)
- Pressing F2 or Rename should not select file extension (#1735)

### Options dialog

- Added Enable Compare As menu option in Shell integration category

### Plugins

- BugFix: Plugin IgnoreLeadingLineNumbers hangs (#1715)

### Command line

- BugFix: Fixed a crash that occurred when comparing alternate data streams,
    such as file.zip:Zone.Identifier:$DATA, using the /self-compare command
    line option.
- BugFix: Fixed a crash that occurred when the /t command line option was
    specified with only one path provided

### Shell extension

- BugFix: Windows 11 Tabs - wrong folder selected from new menu (#1733)
- BugFix: Fixed the issue where the WinMerge menu would not appear when
    right-clicking on a non-item area in Windows 11 Explorer.
- Added Compare As menu item

### Installer

- Installer: components step unclear about "Patch GnuWin32" (#1698)
- The ShellExtension*.dll file is now renamed before installation to prevent
    installation failure when Explorer is loading ShellExtension*.dll.
- Added IgnoreLeadingLineNumbers plugin to non-x86 installers

### Translations

- BugFix: Fix an issue where some messages are not translated. (PR #1712)
- Translation updates:
  - Brazilian (PR #1711,#1713,#1727,#1728,#1756)
  - Chinese Simplified (PR #1714,#1719,#1722,#1731,#1765)
  - Corsican (PR #1717,#1723,#1752)
  - Dutch (PR #1757)
  - German (PR #1730,#1754)
  - Hungarian (PR #1725,#1726,#1758)
  - Japanese
  - Korean (PR #1689,#1690,#1692,#1693,#1694,#1718)
  - Lithuanian (PR #1729,#1755)
  - Polish (PR #1763,#1764)
  - Portuguese (PR #1737)
  - Russian (PR #1710,#1751)
  - Swedish (#1706,PR #1707)

## Known issues

 - Suggestion to make the result of image comparison more reliable (#1391)
 - Crashes when comparing large files (#325)
 - Very slow to compare significantly different directories (#322)
 - Vertical scrollbar breaks after pasting text (#296)
