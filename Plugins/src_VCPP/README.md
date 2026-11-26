# Visual C Plugins

## DisplayBinaryFiles

Quickly display binary files, like Windiff.

 * No packing (no save).
 * Event: `FILE_PACK_UNPACK`
 * File filter: `.exe`, `.dll`, `.ocx`, `.doc`, `.xls`

## DisplayXMLFiles

Display xml files nicely, by consistently inserting tabs and linebreaks.

 * No packing (no save).
 * Event: `FILE_PACK_UNPACK`
 * File filter: `.xml`

## EditBinaryFiles

Allow editing of binary files (if line based), using escape scheme for bytes under 0x20.

 * Event: `FILE_PACK_UNPACK`
 * File filter: `.exe`, `.dll`, `.ocx`, `.doc`, `.xls`

## HideFirstLetter

The first non-space character is invisible in the merge editor (and restored when saving).

 * Event: `FILE_PACK_UNPACK`
 * File filter: `*.nofirst`, `*.hidefirst`

## IgnoreColumns, IgnoreFieldsComma, IgnoreFieldsTab

Ignore differences in some columns or some fields (files with separators).

Update the name of the dll to pass the list of columns/fields.

 * Event: `BUFFER_PREDIFF`
 * File filter: `*.txt` (IgnoreColumns, IgnoreFieldsTab), `*.csv` (IgnoreFieldsComma)

## IgnoreCommentsC

Ignore comments in C/C++/PHP/JS files.

 * No packing (no save).
 * Event: `FILE_PACK_UNPACK`
 * File filter: `.cpp`, `.cxx`, `.h`, `.hxx`, `.c`, `.php`, `.js`

## RCLocalizationHelper

Ignore localized strings/layout in files .rc.

 * Event: `BUFFER_PREDIFF`
 * File filter: `*.rc`

## WatchBeginningOfLog, WatchEndOfLog

For long log files, user is often interested only in the first lines or in the
last lines.

 * *WatchBeginningOfLog* truncates the files to the first 1/10th.
 * *WatchEndOfLog* truncates the files to the first 1/10th.
 * Event: `FILE_PACK_UNPACK`
 * File filter: `*.log`

## IgnoreAccents

Ignore Unicode diacritics when comparing text while preserving case sensitivity.

 * Event: `FILE_PREDIFF`
 * File filter: common text formats (`*.txt`, `*.csv`, `*.log`, `*.xml`, `*.json`, `*.ini`, `*.cfg`, `*.md`, `*.html`, `*.htm`, `*.css`, `*.js`, `*.ts`, `*.cpp`, `*.h`, `*.cs`, `*.sql`)
 * Supports UTF-8 (with or without BOM) and UTF-16 LE/BE.
