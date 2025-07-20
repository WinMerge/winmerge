# WinMerge 2.16.49 Beta Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.49 beta?](#what-is-new-in-21649-beta)
- [Known issues](#known-issues)

July 2025

## About This Release

This is a WinMerge beta release which is meant for preview the current state of
WinMerge development. This release is not recommended for the production.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.49 Beta

## General

- [Enhancement] Tab Switching and Tab Bar Scrolling with Mouse Wheel (PR #2821)

### Appearance

- Use themed size grip, use toolbar's tooltips instead of controls bar's
    (PR #2769)

### File compare

- [Enhancement] Enable/disable automatic navigation to a line when clicking on
    the Location Panel (PR #2827)

### Folder compare

- Add the feature to compare multiple files in one action in the folder compare
    window requested in #324. (PR #2825)

### Options dialog

- BugFix: [Bug Fix] Enable/disable 'Switch to ...' options in Compare > Folder page
    (PR #2819) 

### Translations

- Translation updates:
  - Brazilian (PR #2829)
  - Bulgarian (PR #2763)
  - Chinese Simplified (PR #2831,#2838)
  - Corsican (PR #2844)
  - French (PR #2754,#2761,#2762)
  - Hungarian (PR #2836)
  - Italian (PR #2832)
  - Japanese
  - Korean (PR #2766)
  - Lithuanian (PR #2799,#2805,#2826,#2830)
  - Portuguese (PR #2768)
  - Russian (PR #2760,#2840)
  - Tamil (PR #2812,#2813)
  - Turkish (PR #2782)

## Known issues

 - Pressing OK in the Options window while the INI file specified by /inifile is open for comparison in WinMerge may corrupt the file. (#2685)
 - Suggestion to make the result of image comparison more reliable (#1391)
 - Crashes when comparing large files (#325)
 - Very slow to compare significantly different directories (#322)
 - Vertical scrollbar breaks after pasting text (#296)
