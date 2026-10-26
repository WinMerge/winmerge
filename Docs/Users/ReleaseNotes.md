# WinMerge 2.16.60 Release Notes

* [About This Release](#about-this-release)
* [What Is New in 2.16.60?](#what-is-new-in-21660)
* [What Is New in 2.16.59 Beta?](#what-is-new-in-21659-beta)
* [Known issues](#known-issues)

October 2026

## About This Release

This is a WinMerge 2.16.60 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.60?

### General

* Keep an own recent comparison list (PR #3663)

* Update darkmodelib to 0.77.1 and enable ARM64 support

### File compare

* BugFix: Did not get line filter with Moved working (#3680, PR #3681)

* BugFix: 2 fixes for 3-way-merge (PR #3671)

* BugFix: Fix AutoIt syntax highlighting for identifiers and nested comments
    (PR #3685)

* BugFix: [BUG] Adjust the height of OutputPane will crash (#3733)

* Add RTF syntax highlighting (PR #3686)

* BugFix: Fix overlapping Tree-sitter highlight captures (PR #3718)

* Localize merge copy messages in the Output Pane (#2927, PR #3701)

### Image compare

* BugFix: WinIMerge x86 crashes with FreeImage plugins (winimerge/#43)

### Status bar

* BugFix: Fix difference count in the status bar (#3649, PR #3707)

### Select files or folders dialog

* Open view: collapsible recent comparison list (PR #3673)

* Add side-specific filename, folder, and extension columns to folder
  comparison views. (PR #3682)

* Keep an internal recent-comparison list when the Windows jump list has no
  history. (PR #3663)

### Folder compare

* Add side-specific filename, folder, and extension columns to folder
  comparison views. (PR #3682)

### Image compare

* BugFix: Fix image paste and undo handling (winimerge/#41)

* Update FreeImage to 3.19.25

* Extend destination canvas when copying image differences (winimerge/#42)

### Archive support

* Update 7-Zip to 26.05.

### Plugins

* Set MiniMax presets to M3.1 Flash by default (PR #3709)

### Installer

* BugFix: Align Inno Setup 5 translations and fix Indonesian installer paths
    (#3737)

### Translations

* Translation updates:
  * Brazilian (PR #3665, PR #3687, PR #3672, PR #3697, PR #3708, PR #3717)
  * Chinese Simplified (PR #3660, PR #3691, PR #3694, PR #3712, PR #3721)
  * Chinese Traditional (PR #3738)
  * Corsican (PR #3676)
  * French (PR #3670, PR #3693, PR #3705, PR #3722)
  * German (PR #3659, PR #3679, PR #3689, PR #3695, PR #3713, PR #3720)
  * Hungarian (PR #3669)
  * Italian (PR #3666, PR #3688, PR #3698, PR #3711, PR #3728)
  * Korean (PR #3703, PR #3704, PR #3734)
  * Lithuanian (PR #3667, PR #3690, PR #3710, PR #3715)
  * Polish (PR #3661, PR #3684, PR #3692, PR #3723, PR #3727, PR #3730, PR #3731, PR #3735)
  * Spanish (PR #3699, PR #3700)
  * Turkish (PR #3719)

### Manual

* Refresh untranslated ReadMe files and update support links (PR #3723,
  PR #3725)

* Fix the ReadMe template path in the translation status script (PR #3724)

* Show Git update dates and Inno Setup translation coverage (PR #3739)

## What Is New in 2.16.59 Beta?

### General

* [BUG] MDI window controls (Minimize/Maximize/Close) are completely hidden
    unless hoveredbug (#3511)(PR #3552)

* Add an option to remember and restore splitter positions for text, image,
    web page, and binary comparisons. (PR #3611)

### Appearance

* BugFix: Fix cramped toolbar icon spacing at high DPI

* Migrate toolbar and margin icons from BMP to PNG (GDI+ decoding) (PR #3510)

* Rebuild toolbar buttons based on active window (PR #3631)

### File compare

* Add a Merge Result pane for reviewing and resolving 2-way conflicts and
    3-way merges. (PR #3515)

* BugFix: Fix Shift+Insert paste going to file instead of Filter Bar (#3585)

* BugFix: Fix cursor movement and scrolling across gaps created by the display
    filter (#3601)(PR #3622)

* BugFix: Fix initial status bar display for file comparisons (PR #3644)

* BugFix: Fix issue [BUG] Keyboard shortcut to "Copy to right" does not work in
    3-way merge (#3645)

* BugFix: Fix undo/redo after swapping panes and avoid invalid undo target
    tracking (PR #3646, PR #3647)

* BugFix: Flush undo groups when deleting text fails

* BugFix: Fix blank line handling in comment difference filtering (PR #3651)

* BugFix: Fix ignored diff ranges with missing trailing EOL (#3653)(PR #3657)

* Show the selection margin when adding a bookmark

* Add an option to prefer the WIC decoder when loading images for image
    comparison (PR #3537)

* Add Save command to filepath bar context menuheader bar (PR #3654)

* tree-sitter: php and f-sharp grammars update (PR #3656)

### Table compare

* Add column range filtering and ignore columns in comparison (PR #3565)

### Image compare

* Remember the image compare splitter position (PR #3593)

### Folder compare

* Preserve the sort order for columns that can be safely sorted while folder
    comparison is running. (#3579)(PR #3581)

### Filter expressions

* Add the FilterExpression support required by the upcoming ApplyLineFilter
    plugin (PR #3530)

* BugFix: Fix case-insensitive contains in filter expressions (#3586)

### Status bar

* Make merge mode indicator clickable and reduce its width (PR #3529)

### Archive support

* BugFix: Fix issue #3588: Cannot compare folders inside zip files

* BugFix: [BUG] Comparing 7z-archives: file cannot be accessed by the system
    settings are restored. (#3633)

* Update 7-Zp to 26.03 (PR #3613)

### Plugins

* BugFix: Fix off-by-one bug losing the last byte of pack/unpack plugin buffer
    output
* BugFix: Special UTF-8 characters not compared correctly (#3071)(PR #3568)

* Add plugin selection buttons to the status bar (PR #3518)

* Support adding plugins to pipelines from menus (PR #3523)

* Add filter expressions to plugin pipelines (PR #3540)

* Add MiniMax provider support to AI plugin (PR #3499)

* Add local LLM and custom OpenAI-compatible API support (PR #3589)

* Validate plugin pipeline filter expressions (PR #3578)

* SelectLines plugin: Add literal string matching with -F option.
    (#3500)(PR #3598)

### Translations

* New translation:
  * Azerbaijani (PR #3624)
  * Indonesian (PR #3636)

* Translation updates:
  * Brazilian (PR #3526, PR #3528, PR #3536, PR #3538, PR #3558, PR #3571, PR #3592, PR #3605, PR #3618)
  * Chinese Simplified (PR #3531, PR #3542, PR #3554, PR #3572, PR #3595, PR #3604, PR #3619, PR #3637)
  * Chinese Traditional (PR #3642)
  * French (PR #3534, PR #3547, PR #3569, PR #3580, PR #3634)
  * German (PR #3524, PR #3541, PR #3555, PR #3576, PR #3597, PR #3603, PR #3612)
  * Hungarian (PR #3564, PR #3590)
  * Korean (PR #3548, PR #3563, PR #3621)
  * Italian (PR #3551, PR #3561, PR #3573, PR #3594, PR #3607, PR #3623)
  * Lithuanian (PR #3543, PR #3574, PR #3608, PR #3620)
  * Polish (PR #3527, PR #3549, PR #3556, PR #3577, PR #3616)
  * Spanish (PR #3614, PR #3615)
  * Russian (PR #3546, PR #3583, PR #3600, PR #3626)
  * Turkish (PR #3532, PR #3544, PR #3562, PR #3575, PR #3596, PR #3627)

### Manual

* BugFix: [BUG] Spanish help is in French (#3658)

* Set localization parameters for French, Italian, and Spanish manuals
    (PR #3617)


## Known issues

* Pressing OK in the Options window while the INI file specified by /inifile is open for comparison in WinMerge may corrupt the file. (#2685)
* Suggestion to make the result of image comparison more reliable (#1391)
* Crashes when comparing large files (#325)
* Very slow to compare significantly different directories (#322)
* Vertical scrollbar breaks after pasting text (#296)

