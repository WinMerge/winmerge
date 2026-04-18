# WinMerge 2.16.55 Beta Release Notes

* [About This Release](#about-this-release)
* [What Is New in 2.16.55 beta?](#what-is-new-in-21655-beta)
* [Known issues](#known-issues)

April 2026

## About This Release

This is a WinMerge beta release which is meant for preview the current state of
WinMerge development. This release is not recommended for production
environments.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.55 Beta

### General

* Fix incorrect GetAsyncKeyState usage for modifier key detection (PR #3257)

* Update POCO C++ Libraries to 1.15.1 (PR #3292)

### File compare

* BugFix: Fix keyboard input issue after Alt+mouse operations (PR #3254)

* BugFix: Register self-compare in MRU and refactor command line parameter
    buil… (PR #3266)
    
* Refactor save confirmation logic using SaveClosingDlg::ShowAndSave
    (PR #3256)

* Enable IME text reconversion in editor view (PR #3214)
    
### Image compare

* BugFix: Fix crash caused by OnClose being called twice in image compare
    window (PR #3255)

* Speedup image comparision slider during WipeEffect (PR winimerge/#33)

### Folder compare

* BugFix: Fix an issue where skipped items were not properly filtered during
    folder/file comparison. (PR #3267)

* BugFix: Fix potential hang in DiffWorker shutdown (PR #3300)

* Extend “Merge renamed items” option to support moved items (PR #3181)

* Refactor compare engines to use DiffContext (PR #3197)

* Refactor compare engines to write results into DIFFITEM and add
    FullQuickCompare (PR #3201)
    
* Add e:/e!: filter prefixes for files and directories (PR #3205)

* Add replaceWithList and regexReplaceWithList filter functions (PR #3213)

* Add Replace Lists menu for Rename/Move detection keys (PR #3215)

* Make replace and replaceWithList case-insensitive (PR #3222)

* Add directives (@cs, @ci, @name) to filter expressions (PR #3253)

* Add "Filter by This Column" to folder view header context menu (PR #3268)

* Add display filter attributes and UI for filtering by comparison results
    (PR #3284)

* Add ability to hide empty folders in tree mode (PR #3293)

* Add Unpacker and Prediffer fields to file filter expressions (PR #3295)

* Enable dynamic increase/decrease of compare threads (PR #3304)

### Plugins

* Add PrettifyPO and ValidatePO plugins for .po/.pot files (PR #3190)

* Feature request: In CompareMSExcelFiles do not append a sheet order number
    to individual tsv, txt or html files (#3212)

* Add CFR Java decompiler plugin for .class file comparison (PR #3242,#3243)

### Translations

* Translation updates:
  * Brazilian (PR #3185,#3194,#3219,#3231,#3245,#3275,#3287,#3303)
  * Chinese Simplified (PR #3183,#3191,#3220,#3228,#3244)
  * Corsican (PR #3259,#3299)
  * French (PR #3239)
  * German (PR #3187,#3196,#3217,#3232,#3237,#3251,#3274,#3288,#3297)
  * Hungarian (PR #3186,#3238,#3252)
  * Italian (PR #3199,#3218,#3229,#3235,#3236,#3246,#3249,#3269,#3272,#3285,#3302)
  * Japanese (PR #3258)
  * Korean (PR #3247,#3279,#3301)
  * Lithuanian (PR #3184,#3192,#3230,#3276,#3290)
  * Polish (PR #3233,#3273)
  * Russian (PR #3226,#3281)
  * Turkish (PR #3204,#3221,#3227,#3250,#3271,#3286,#3298)
  * Ukrainian (PR #)

### Documentation

* Added: Community standard files (like issue templates, pr template).
    (PR #3225)

* Add context IDs to PO files & strip comments for releases (PR #3296)

## Known issues

* Pressing OK in the Options window while the INI file specified by /inifile is open for comparison in WinMerge may corrupt the file. (#2685)
* Suggestion to make the result of image comparison more reliable (#1391)
* Crashes when comparing large files (#325)
* Very slow to compare significantly different directories (#322)
* Vertical scrollbar breaks after pasting text (#296)
