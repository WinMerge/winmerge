# WinMerge 2.16.52 Release Notes

* [About This Release](#about-this-release)
* [What Is New in 2.16.52](#what-is-new-in-21652)
* [What Is New in 2.16.51 beta?](#what-is-new-in-21651-beta)
* [Known issues](#known-issues)

October 2025

## About This Release

This is a WinMerge 2.16.52 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.52

### General

* BugFix: Fixed an issue where leftover WinMerge temporary folders were not 
    properly deleted on the next application exit (since version 2.16.51).
* [Feature request] Alternate hotkeys for tab navigation (#2942)
* Disabled darkmodelib in the ARM64 build due to an unresolved crash issue.

### Binary compare

* BugFix: Ctrl-W in Hex View : Close window? (#2294)

### Folder compare

* BugFix: Round timestamps before comparing to ensure consistent 1-second precision (#2973)
* Add toDateStr filter function.
* Add option for specifying additional comparison conditions (PR #2963)
* Add support for prop, leftprop, middleprop, and rightprop functions in
  filter expressions (PR #2974)
* Add "Existence" folder comparison method (PR #2980)


### Translations

* Translation updates:
  * Brazilian (PR #2981,#2977,#2995)
  * Chinese Simplified (PR #2978,#2991,#2996)
  * Corsican (PR #2999)
  * Italian (PR #2976,#2982,#2998)
  * Japanese
  * Korean (PR #2993)
  * Lithuanian (PR #2975,#2986,#2997)
  * Portuguese (PR #2971)
  * Polish (PR #2972,#2979,#3003,#3005)
  * Turkish (PR #2992)
  * Russian (PR #2983)

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
  * French (PR #2887,#2938,#2969)
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
