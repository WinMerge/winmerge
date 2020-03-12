# Visual Basic Plugins

## CompareMSExcelFiles

Displays the text content of a MS Excel file, stripping away all formatting and embedded objects.

 * No packing (no save).
 * Event: `FILE_PACK_UNPACK`
 * File filter: `*.xls`

## CompareMSWordFiles

Displays the text content of a MS Word file, stripping away all formatting and embedded objects.

 * No packing (no save).
 * Event: `FILE_PACK_UNPACK`
 * File filter: `*.doc`

## HideLastLetter

The last non-space character is invisible in the merge editor (and restored when saving).

 * Event: `BUFFER_PACK_UNPACK`
 * File filter: `*.last`

## IgnoreLeadingLineNumbers

Ignore leading line numbers in text files (e.g. NC and BASIC files).

 * Event: `BUFFER_PREDIFF`
 * File filter: `*.nc`

## IgnoreTwoFirstCharsOrder

Sort the first two characters of both files before diffing.

 * Event: `BUFFER_PREDIFF`
 * File filter: `*.txt`

## ToUpper

Convert the selection to upper case.

 * Event: `EDITOR_SCRIPT`
