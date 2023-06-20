# WinMerge 2.16.31 Beta Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.31 beta?](#what-is-new-in-21631-beta)
- [Known issues](#known-issues)

June 2023

## About This Release

This is a WinMerge beta release which is meant for preview the current state of
WinMerge development. This release is not recommended for the production.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.31 Beta

### General

- BugFix: Some Substitution filter doesn't work (#1861)
- Add tasks to Jump List (PR #1828)
- Update DirCmpReport.cpp (PR #1892)

### File compare

- BugFix: Fix input range check processing in "Go to" dialog. (PR #1826)
- BugFix: End of line diff is a bit wanky (#1838, PR #1849)
- Confirm copy all in file merge (PR #1827)
- Modify the "Go to" dialog. (PR #1896)

### Folder compare

- BugFix: Display problem with Item totals : (#1840)
- BugFix: Bug in ignore whitespace ? (#1882)

### Plugins

- PrettifyJSON: Update jq to version 1.6 (#1871)
- Translate some plugin error messages (PR #1873)
- ApplyPatch: Update GNU patch to 2.7.6-1 (PR #1897)(#1871)

### Installer

- Silent install blocked (#1852)

### Translations

- Translation updates:
  - Brazilian (PR #1829,#1837,#1876,#1903)
  - Bulgarian (PR #1822)
  - Chinese Simplified (PR #1835,#1846,#1885,#1906)
  - Dutch (PR #1831)
  - French (PR #1841,#1842,#1894)
  - Galician (PR #1833)
  - German (PR #1850,#1875,#1907)
  - Hungarian (PR #1832,#1839,#1845,#1878,#1905)
  - Japanese
  - Korean (PR #1820,#1821,#1877)
  - Lithuanian (PR #1847,#1889,#1904)
  - Polish (PR #1869,#1870,#1884)
  - Portuguese (PR #1843,#1895)
  - Spanish (PR #1834)
  - Russian (PR #1824,#1825,#1862)

## Known issues

 - Suggestion to make the result of image comparison more reliable (#1391)
 - Crashes when comparing large files (#325)
 - Very slow to compare significantly different directories (#322)
 - Vertical scrollbar breaks after pasting text (#296)
