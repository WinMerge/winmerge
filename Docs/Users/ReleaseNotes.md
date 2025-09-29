# WinMerge 2.16.51 Beta Release Notes

* [About This Release](#about-this-release)
* [What Is New in 2.16.51 beta?](#what-is-new-in-21651-beta)
* [Known issues](#known-issues)

October 2025

## About This Release

This is a WinMerge beta release which is meant for preview the current state of
WinMerge development. This release is not recommended for the production.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.51 Beta

### General

* BugFix: The document does not scroll with the mouse if the documents are identical.
  Or the text input cursor jumps to unequal lines. (#2939)

### Appearance

* Initial dark mode support via darkmodelib (PR #2834,#2885)
* Improved header bar to reduce flicker

### File compare

* Add option to ignore line breaks (treat as spaces) (refs #373) (PR #2945)

### Folder compare

* Fixed an issue where adding exclusions via \[Add Exclude File] ->
  \[Editor Backup Files] incorrectly excluded most files.

### Filters dialog

* Allow resizing of filters window using MFC Dynamic Layout (#509) (PR #2907)

### Options dialog

* Allow saving modified colors as new scheme (#1180) (PR #2908)

### Translations

* Translation updates:

  * Brazilian (#2886,#2910,#2951)
  * Chinese Simplified (PR #2889,#2916,#2952)
  * Corsican (PR #2915)
  * French (PR #2887,#2938)
  * Hungarian (PR #2909,#2948)
  * Italian (PR #2892,#2911,#2956)
  * Japanese
  * Korean (PR #2913,#2934,#2950)
  * Lithuanian (PR #2912,#2936,#2953)
  * Portuguese (PR #2928)
  * Polish (PR #2929,#2930,#2931)
  * Russian (PR #2890,#2919,#2920,#2933,#2944,#2949)
  * Spanish (PR #2961)
  * Swedish
  * Turkish (PR #2906,#2914,#2957)
  * Ukrainian (PR #2932,#2937,#2954)

### Internals

* Refactor/use bindoption (PR #2955)

## Known issues

* Pressing OK in the Options window while the INI file specified by /inifile is open for comparison in WinMerge may corrupt the file. (#2685)
* Suggestion to make the result of image comparison more reliable (#1391)
* Crashes when comparing large files (#325)
* Very slow to compare significantly different directories (#322)
* Vertical scrollbar breaks after pasting text (#296)
