/** 
 * @file  OptionsDef.h
 *
 * @brief Constants for option-names
 */
#pragma once

#include "UnicodeString.h"

// User's language
inline const String OPT_SELECTED_LANGUAGE{_T("Locale/LanguageId"s)};

// View-menu
inline const String OPT_SHOW_UNIQUE_LEFT {_T("Settings/ShowUniqueLeft"s)};
inline const String OPT_SHOW_UNIQUE_MIDDLE {_T("Settings/ShowUniqueMiddle"s)};
inline const String OPT_SHOW_UNIQUE_RIGHT {_T("Settings/ShowUniqueRight"s)};
inline const String OPT_SHOW_DIFFERENT {_T("Settings/ShowDifferent"s)};
inline const String OPT_SHOW_IDENTICAL {_T("Settings/ShowIdentical"s)};
inline const String OPT_SHOW_BINARIES {_T("Settings/ShowBinaries"s)};
inline const String OPT_SHOW_SKIPPED {_T("Settings/ShowSkipped"s)};
inline const String OPT_SHOW_DIFFERENT_LEFT_ONLY {_T("Settings/ShowDifferentLeftOnly"s)};
inline const String OPT_SHOW_DIFFERENT_MIDDLE_ONLY {_T("Settings/ShowDifferentMiddleOnly"s)};
inline const String OPT_SHOW_DIFFERENT_RIGHT_ONLY {_T("Settings/ShowDifferentRightOnly"s)};
inline const String OPT_SHOW_MISSING_LEFT_ONLY {_T("Settings/ShowMissingLeftOnly"s)};
inline const String OPT_SHOW_MISSING_MIDDLE_ONLY {_T("Settings/ShowMissingMiddleOnly"s)};
inline const String OPT_SHOW_MISSING_RIGHT_ONLY {_T("Settings/ShowMissingRightOnly"s)};
inline const String OPT_TREE_MODE {_T("Settings/TreeMode"s)};

// Show/hide toolbar/statusbar/tabbar
inline const String OPT_SHOW_TOOLBAR {_T("Settings/ShowToolbar"s)};
inline const String OPT_SHOW_STATUSBAR {_T("Settings/ShowStatusbar"s)};
inline const String OPT_SHOW_TABBAR {_T("Settings/ShowTabbar"s)};
inline const String OPT_TOOLBAR_SIZE {_T("Settings/ToolbarSize"s)};
inline const String OPT_RESIZE_PANES {_T("Settings/AutoResizePanes"s)};

inline const String OPT_SYNTAX_HIGHLIGHT {_T("Settings/HiliteSyntax"s)};
inline const String OPT_VIEW_WHITESPACE {_T("Settings/ViewWhitespace"s)};
inline const String OPT_VIEW_EOL {_T("Settings/ViewEOL"s)};
inline const String OPT_SCROLL_TO_FIRST {_T("Settings/ScrollToFirst"s)};
inline const String OPT_SCROLL_TO_FIRST_INLINE_DIFF {_T("Settings/ScrollToFirstInlineDiff"s)};
inline const String OPT_RENDERING_MODE {_T("Settings/RenderingMode"s)};

// Difference (in-line) highlight
inline const String OPT_WORDDIFF_HIGHLIGHT {_T("Settings/HiliteWordDiff"s)};
inline const String OPT_BREAK_ON_WORDS {_T("Settings/BreakOnWords"s)};
inline const String OPT_BREAK_TYPE {_T("Settings/BreakType"s)};
inline const String OPT_BREAK_SEPARATORS {_T("Settings/HiliteBreakSeparators"s)};

// Backup options
inline const String OPT_BACKUP_FOLDERCMP {_T("Backup/EnableFolder"s)};
inline const String OPT_BACKUP_FILECMP {_T("Backup/EnableFile"s)};
inline const String OPT_BACKUP_LOCATION {_T("Backup/Location"s)};
inline const String OPT_BACKUP_GLOBALFOLDER {_T("Backup/GlobalFolder"s)};
inline const String OPT_BACKUP_ADD_BAK {_T("Backup/NameAddBak"s)};
inline const String OPT_BACKUP_ADD_TIME {_T("Backup/NameAddTime"s)};

inline const String OPT_DIRVIEW_SORT_COLUMN {_T("Settings/DirViewSortCol"s)};
inline const String OPT_DIRVIEW_SORT_COLUMN3 {_T("Settings/DirViewSortCol3"s)};
inline const String OPT_DIRVIEW_SORT_ASCENDING {_T("Settings/DirViewSortAscending"s)};
inline const String OPT_DIRVIEW_EXPAND_SUBDIRS {_T("Settings/DirViewExpandSubdirs"s)};
inline const String OPT_DIRVIEW_COLUMN_ORDERS {_T("DirView/ColumnOrders"s)};
inline const String OPT_DIRVIEW_COLUMN_WIDTHS {_T("DirView/ColumnWidths"s)};
inline const String OPT_DIRVIEW3_COLUMN_ORDERS {_T("DirView3/ColumnOrders"s)};
inline const String OPT_DIRVIEW3_COLUMN_WIDTHS {_T("DirView3/ColumnWidths"s)};

inline const String OPT_ADDITIONAL_PROPERTIES {_T("Settings/AdditionalProperties"s)};

// Window/Pane
inline const String OPT_ACTIVE_FRAME_MAX {_T("Settings/ActiveFrameMax"s)};
inline const String OPT_ACTIVE_PANE {_T("Settings/ActivePane"s)};

// Folder Compare Report
inline const String OPT_REPORTFILES_REPORTTYPE {_T("ReportFiles/ReportType"s)};
inline const String OPT_REPORTFILES_COPYTOCLIPBOARD {_T("ReportFiles/CopyToClipboard"s)};
inline const String OPT_REPORTFILES_INCLUDEFILECMPREPORT {_T("ReportFiles/IncludeFileCmpReport"s)};

// File compare
inline const String OPT_AUTOMATIC_RESCAN {_T("Settings/AutomaticRescan"s)};
inline const String OPT_ALLOW_MIXED_EOL {_T("Settings/AllowMixedEOL"s)};
// restore old copy behaviour (always copy "full line" instead of "selected text only"), with a hidden option
inline const String OPT_COPY_FULL_LINE {_T("Settings/CopyFullLine"s)};
inline const String OPT_TAB_SIZE {_T("Settings/TabSize"s)};
inline const String OPT_TAB_TYPE {_T("Settings/TabType"s)};
inline const String OPT_WORDWRAP {_T("Settings/WordWrap"s)};
inline const String OPT_WORDWRAP_TABLE {_T("Settings/WordWrapTable"s)};
inline const String OPT_VIEW_LINENUMBERS {_T("Settings/ViewLineNumbers"s)};
inline const String OPT_VIEW_FILEMARGIN {_T("Settings/ViewFileMargin"s)};
inline const String OPT_VIEW_TOPMARGIN {_T("Settings/ViewTopMargin"s)};
inline const String OPT_VIEW_TOPMARGIN_TABLE {_T("Settings/ViewTopMarginTable"s)};
inline const String OPT_VIEW_ZOOM {_T("Settings/ViewZoom"s)};
inline const String OPT_LINE_NUMBER_USED_AS_HEADERS {_T("Settings/LineNumberUsedAsHeaders"s)};
inline const String OPT_DIFF_CONTEXT {_T("Settings/DiffContextV2"s)};
inline const String OPT_INVERT_DIFF_CONTEXT {_T("Settings/InvertDiffContext"s)};

inline const String OPT_EXT_EDITOR_CMD {_T("Settings/ExternalEditor"s)};
inline const String OPT_USE_RECYCLE_BIN {_T("Settings/UseRecycleBin"s)};
inline const String OPT_SINGLE_INSTANCE {_T("Settings/SingleInstance"s)};
inline const String OPT_MERGE_MODE {_T("Settings/MergingMode"s)};
inline const String OPT_CLOSE_WITH_ESC {_T("Settings/CloseWithEsc"s)};
inline const String OPT_CLOSE_WITH_OK {_T("Settings/CloseWithOK"s)};
inline const String OPT_VERIFY_OPEN_PATHS {_T("Settings/VerifyOpenPaths"s)};
inline const String OPT_AUTO_COMPLETE_SOURCE {_T("Settings/AutoCompleteSource"s)};
inline const String OPT_IGNORE_SMALL_FILETIME {_T("Settings/IgnoreSmallFileTime"s)};
inline const String OPT_ASK_MULTIWINDOW_CLOSE {_T("Settings/AskClosingMultipleWindows"s)};
inline const String OPT_PRESERVE_FILETIMES {_T("Settings/PreserveFiletimes"s)};

inline const String OPT_CP_DEFAULT_MODE {_T("Settings/CodepageDefaultMode"s)};
inline const String OPT_CP_DEFAULT_CUSTOM {_T("Settings/CodepageDefaultCustomValue"s)};
inline const String OPT_CP_DETECT {_T("Settings/CodepageDetection"s)};

inline const String OPT_PROJECTS_PATH {_T("Settings/ProjectsPath"s)};
inline const String OPT_USE_SYSTEM_TEMP_PATH {_T("Settings/UseSystemTempPath"s)};
inline const String OPT_CUSTOM_TEMP_PATH {_T("Settings/CustomTempPath"s)};

inline const String OPT_SPLIT_HORIZONTALLY {_T("Settings/SplitHorizontally"s)};
inline const String OPT_FILE_SIZE_THRESHOLD {_T("Settings/OPT_FILE_SIZE_THRESHOLD"s)};

inline const String OPT_AUTO_RELOAD_MODIFIED_FILES {_T("Settings/AutoReloadModifiedFiles"s)};

// Color options
// The difference color
inline const String OPT_CLR_DIFF {_T("Settings/DifferenceColor"s)};
// The selected difference color
inline const String OPT_CLR_SELECTED_DIFF {_T("Settings/SelectedDifferenceColor"s)};
// The difference deleted color
inline const String OPT_CLR_DIFF_DELETED {_T("Settings/DifferenceDeletedColor"s)};
// The selected difference deleted color
inline const String OPT_CLR_SELECTED_DIFF_DELETED {_T("Settings/SelectedDifferenceDeletedColor"s)};
// The difference text color
inline const String OPT_CLR_DIFF_TEXT {_T("Settings/DifferenceTextColor"s)};
// The selected difference text color
inline const String OPT_CLR_SELECTED_DIFF_TEXT {_T("Settings/SelectedDifferenceTextColor"s)};
// The ignored lines color
inline const String OPT_CLR_TRIVIAL_DIFF {_T("Settings/TrivialDifferenceColor"s)};
// The ignored and deleted lines color
inline const String OPT_CLR_TRIVIAL_DIFF_DELETED {_T("Settings/TrivialDifferenceDeletedColor"s)};
// The ignored text color
inline const String OPT_CLR_TRIVIAL_DIFF_TEXT {_T("Settings/TrivialDifferenceTextColor"s)};
// The moved block color
inline const String OPT_CLR_MOVEDBLOCK {_T("Settings/MovedBlockColor"s)};
// The moved block deleted lines color
inline const String OPT_CLR_MOVEDBLOCK_DELETED {_T("Settings/MovedBlockDeletedColor"s)};
// The moved block text color
inline const String OPT_CLR_MOVEDBLOCK_TEXT {_T("Settings/MovedBlockTextColor"s)};
// The selected moved block color
inline const String OPT_CLR_SELECTED_MOVEDBLOCK {_T("Settings/SelectedMovedBlockColor"s)};
// The selected moved block deleted lines
inline const String OPT_CLR_SELECTED_MOVEDBLOCK_DELETED {_T("Settings/SelectedMovedBlockDeletedColor"s)};
// The selected moved block text color
inline const String OPT_CLR_SELECTED_MOVEDBLOCK_TEXT {_T("Settings/SelectedMovedBlockTextColor"s)};
// The SNP block color
inline const String OPT_CLR_SNP {_T("Settings/SNPColor"s)};
// The SNP block deleted lines color
inline const String OPT_CLR_SNP_DELETED {_T("Settings/SNPDeletedColor"s)};
// The SNP block text color
inline const String OPT_CLR_SNP_TEXT {_T("Settings/SNPTextColor"s)};
// The selected SNP block color
inline const String OPT_CLR_SELECTED_SNP {_T("Settings/SelectedSNPColor"s)};
// The selected SNP block deleted lines
inline const String OPT_CLR_SELECTED_SNP_DELETED {_T("Settings/SelectedSNPDeletedColor"s)};
// The selected SNP block text color
inline const String OPT_CLR_SELECTED_SNP_TEXT {_T("Settings/SelectedSNPTextColor"s)};
// The word difference color
inline const String OPT_CLR_WORDDIFF {_T("Settings/WordDifferenceColor"s)};
// The word difference deleted color
inline const String OPT_CLR_WORDDIFF_DELETED {_T("Settings/WordDifferenceDeletedColor"s)};
// The word difference text color
inline const String OPT_CLR_WORDDIFF_TEXT {_T("Settings/WordDifferenceTextColor"s)};
// The selected word difference color
inline const String OPT_CLR_SELECTED_WORDDIFF {_T("Settings/SelectedWordDifferenceColor"s)};
// The word difference deleted color
inline const String OPT_CLR_SELECTED_WORDDIFF_DELETED {_T("Settings/SelectedWordDifferenceDeletedColor"s)};
// The selected word difference text color
inline const String OPT_CLR_SELECTED_WORDDIFF_TEXT {_T("Settings/SelectedWordDifferenceTextColor"s)};
// Whether to use default (theme) text colors
inline const String OPT_CLR_DEFAULT_TEXT_COLORING {_T("Settings/DefaultTextColoring"s)};

// DirCompare Color options
// The item equal background color
inline const String OPT_DIRCLR_ITEM_EQUAL {_T("Settings/DirItemEqualColor"s)};
// The item equal text color
inline const String OPT_DIRCLR_ITEM_EQUAL_TEXT {_T("Settings/DirItemEqualTextColor"s)};
// The item diff background color
inline const String OPT_DIRCLR_ITEM_DIFF {_T("Settings/DirItemDiffColor"s)};
// The item diff text color
inline const String OPT_DIRCLR_ITEM_DIFF_TEXT {_T("Settings/DirItemDiffTextColor"s)};
// The item not-exist-all background color
inline const String OPT_DIRCLR_ITEM_NOT_EXIST_ALL {_T("Settings/DirItemNotExistAllColor"s)};
// The item not-exist-all text color
inline const String OPT_DIRCLR_ITEM_NOT_EXIST_ALL_TEXT {_T("Settings/DirItemNotExistAllTextColor"s)};
// The item filtered background color
inline const String OPT_DIRCLR_ITEM_FILTERED {_T("Settings/DirItemFilteredColor"s)};
// The item filtered text color
inline const String OPT_DIRCLR_ITEM_FILTERED_TEXT {_T("Settings/DirItemFilteredTextColor"s)};
// The background color
inline const String OPT_DIRCLR_MARGIN {_T("Settings/DirMarginColor"s)};
// Use dir compare coloring?
inline const String OPT_DIRCLR_USE_COLORS {_T("Settings/UseDirCompareColors"s)};

inline const String OPT_COLOR_SCHEME {_T("Settings/ColorScheme"s)};

// Compare options
inline const String OPT_CMP_IGNORE_WHITESPACE {_T("Settings/IgnoreSpace"s)};
inline const String OPT_CMP_IGNORE_BLANKLINES {_T("Settings/IgnoreBlankLines"s)};
inline const String OPT_CMP_FILTER_COMMENTLINES {_T("Settings/FilterCommentsLines"s)};
inline const String OPT_CMP_IGNORE_CASE {_T("Settings/IgnoreCase"s)};
inline const String OPT_CMP_IGNORE_NUMBERS {_T("Settings/IgnoreNumbers"s)};
inline const String OPT_CMP_IGNORE_EOL {_T("Settings/IgnoreEol"s)};
inline const String OPT_CMP_IGNORE_CODEPAGE {_T("Settings/IgnoreCodepage"s)};
inline const String OPT_CMP_METHOD {_T("Settings/CompMethod2"s)};
inline const String OPT_CMP_MOVED_BLOCKS {_T("Settings/MovedBlocks"s)};
inline const String OPT_CMP_MATCH_SIMILAR_LINES {_T("Settings/MatchSimilarLines"s)};
inline const String OPT_CMP_STOP_AFTER_FIRST {_T("Settings/StopAfterFirst"s)};
inline const String OPT_CMP_QUICK_LIMIT {_T("Settings/QuickMethodLimit"s)};
inline const String OPT_CMP_BINARY_LIMIT {_T("Settings/BinaryMethodLimit"s)};
inline const String OPT_CMP_COMPARE_THREADS {_T("Settings/CompareThreads"s)};
inline const String OPT_CMP_WALK_UNIQUE_DIRS {_T("Settings/ScanUnpairedDir"s)};
inline const String OPT_CMP_IGNORE_REPARSE_POINTS {_T("Settings/IgnoreReparsePoints"s)};
inline const String OPT_CMP_INCLUDE_SUBDIRS {_T("Settings/Recurse"s)};
inline const String OPT_CMP_DIFF_ALGORITHM {_T("Settings/DiffAlgorithm"s)};
inline const String OPT_CMP_INDENT_HEURISTIC {_T("Settings/IndentHeuristic"s)};
inline const String OPT_CMP_COMPLETELY_BLANK_OUT_IGNORED_CHANGES {_T("Settings/CompletelyBlankOutIgnoredChanges"s)};

// Image Compare options
inline const String OPT_CMP_IMG_FILEPATTERNS {_T("Settings/ImageFilePatterns"s)};
inline const String OPT_CMP_IMG_SHOWDIFFERENCES {_T("Settings/ImageShowDifferences"s)};
inline const String OPT_CMP_IMG_OVERLAYMODE {_T("Settings/ImageOverlayMode"s)};
inline const String OPT_CMP_IMG_OVERLAYALPHA {_T("Settings/ImageOverlayAlpha"s)};
inline const String OPT_CMP_IMG_DRAGGING_MODE {_T("Settings/ImageDraggingMode"s)};
inline const String OPT_CMP_IMG_ZOOM {_T("Settings/ImageZoom"s)};
inline const String OPT_CMP_IMG_USEBACKCOLOR {_T("Settings/ImageUseBackColor"s)};
inline const String OPT_CMP_IMG_BACKCOLOR {_T("Settings/ImageBackColor"s)};
inline const String OPT_CMP_IMG_DIFFBLOCKSIZE {_T("Settings/ImageDiffBlockSize"s)};
inline const String OPT_CMP_IMG_DIFFCOLORALPHA {_T("Settings/ImageDiffColorAlpha"s)};
inline const String OPT_CMP_IMG_THRESHOLD {_T("Settings/ImageColorDistanceThreshold"s)};
inline const String OPT_CMP_IMG_INSERTIONDELETIONDETECTION_MODE {_T("Settings/ImageInsertionDeletionDetectionMode"s)};
inline const String OPT_CMP_IMG_VECTOR_IMAGE_ZOOM_RATIO {_T("Settings/ImageVectorImageZoomRatio"s)};
inline const String OPT_CMP_ENABLE_IMGCMP_IN_DIRCMP {_T("Settings/EnableImageCompareInFolderCompare"s)};
inline const String OPT_CMP_IMG_OCR_RESULT_TYPE {_T("Settings/ImageOcrResultType"s)};

// WebPage Compare options
inline const String OPT_CMP_WEB_USERDATAFOLDER_TYPE {_T("Settings/WebPageUserDataFolderType"s)};
inline const String OPT_CMP_WEB_USERDATAFOLDER_PERPANE {_T("Settings/WebPageUserDataFolderPerPane"s)};
inline const String OPT_CMP_WEB_FIT_TO_WINDOW {_T("Settings/WebPageFitToWindow"s)};
inline const String OPT_CMP_WEB_SHOWDIFFERENCES {_T("Settings/WebPageShowDifferences"s)};
inline const String OPT_CMP_WEB_VIEW_WIDTH {_T("Settings/WebPageViewWidth"s)};
inline const String OPT_CMP_WEB_VIEW_HEIGHT {_T("Settings/WebPageViewHeight"s)};
inline const String OPT_CMP_WEB_ZOOM {_T("Settings/WebZoom"s)};
inline const String OPT_CMP_WEB_USER_AGENT {_T("Settings/WebUserAgent"s)};
inline const String OPT_CMP_WEB_URL_PATTERN_TO_INCLUDE {_T("Settings/WebPageURLPatternToInclude"s)};
inline const String OPT_CMP_WEB_URL_PATTERN_TO_EXCLUDE {_T("Settings/WebPageURLPatternToExclude"s)};

// Binary options
inline const String OPT_CMP_BIN_FILEPATTERNS {_T("Settings/BinaryFilePatterns"s)};

// Table options
inline const String OPT_CMP_CSV_FILEPATTERNS {_T("Settings/CSVFilePatterns"s)};
inline const String OPT_CMP_CSV_DELIM_CHAR   {_T("Settings/CSVDelimiterCharacter"s)};
inline const String OPT_CMP_TSV_FILEPATTERNS {_T("Settings/TSVFilePatterns"s)};
inline const String OPT_CMP_DSV_FILEPATTERNS {_T("Settings/DSVFilePatterns"s)};
inline const String OPT_CMP_DSV_DELIM_CHAR   {_T("Settings/DSVDelimiterCharacter"s)};
inline const String OPT_CMP_TBL_ALLOW_NEWLINES_IN_QUOTES {_T("Settings/TableAllowNewlinesInQuotes"s)};
inline const String OPT_CMP_TBL_QUOTE_CHAR   {_T("Settings/TableQuoteCharacter"s)};

/// Are regular expression linefilters enabled?
inline const String OPT_LINEFILTER_ENABLED {_T("Settings/IgnoreRegExp"s)};
/// Currently selected filefilter
inline const String OPT_FILEFILTER_CURRENT {_T("Settings/FileFilterCurrent"s)};
inline const String OPT_FILTER_USERPATH {_T("Settings/UserFilterPath"s)};
inline const String OPT_FILEFILTER_SHARED {_T("Settings/Filters/Shared"s)};

/// Substitution Filters
inline const String OPT_SUBSTITUTION_FILTERS_ENABLED {_T("Settings/SubstitutionFiltersEnabled"s)};

// Archive support
inline const String OPT_ARCHIVE_ENABLE {_T("Merge7z/Enable"s)};
inline const String OPT_ARCHIVE_PROBETYPE {_T("Merge7z/ProbeSignature"s)};
inline const String OPT_ARCHIVE_FILTER_INDEX {_T("Merge7z/FilterIndex"s)};

// Patch Creator
inline const String OPT_PATCHCREATOR_PATCH_STYLE {_T("PatchCreator/PatchStyle"s)};
inline const String OPT_PATCHCREATOR_CONTEXT_LINES {_T("PatchCreator/ContextLines"s)};
inline const String OPT_PATCHCREATOR_OPEN_TO_EDITOR {_T("PatchCreator/OpenToEditor"s)};
inline const String OPT_PATCHCREATOR_INCLUDE_CMD_LINE {_T("PatchCreator/IncludeCmdLine"s)};
inline const String OPT_PATCHCREATOR_COPY_TO_CLIPBOARD {_T("PatchCreator/CopyToClipboard"s)};

// Plugins
inline const String OPT_PLUGINS_ENABLED {_T("Settings/PluginsEnabled"s)};
inline const String OPT_PLUGINS_CUSTOM_SETTINGS_LIST {_T("Settings/PluginsCustomSettingsList"s)};
inline const String OPT_PLUGINS_UNPACKER_MODE {_T("Settings/UnpackerMode"s)};
inline const String OPT_PLUGINS_PREDIFFER_MODE {_T("Settings/PredifferMode"s)};
inline const String OPT_PLUGINS_UNPACK_DONT_CHECK_EXTENSION {_T("Plugins/UnpackDontCheckExtension"s)};
inline const String OPT_PLUGINS_OPEN_IN_SAME_FRAME_TYPE {_T("Plugins/OpenInSameFrameType"s)};

// Startup options
inline const String OPT_SHOW_SELECT_FILES_AT_STARTUP {_T("Settings/ShowFileDialog"s)};

// MDI Tab Bar
inline const String OPT_TABBAR_AUTO_MAXWIDTH {_T("Settings/TabBarAutoMaxWidth"s)};

// MRU
inline const String OPT_MRU_MAX {_T("Settings/MRUMax"s)};

// Font options
inline const String OPT_FONT_FILECMP {_T("Font/"s)};
inline const String OPT_FONT_DIRCMP {_T("FontDirCompare/"s)};

inline const String OPT_FONT_USECUSTOM {_T("Specified"s)};
inline const String OPT_FONT_POINTSIZE {_T("PointSize"s)};
inline const String OPT_FONT_HEIGHT {_T("Height"s)};
inline const String OPT_FONT_ESCAPEMENT {_T("Escapement"s)};
inline const String OPT_FONT_ORIENTATION {_T("Orientation"s)};
inline const String OPT_FONT_WEIGHT {_T("Weight"s)};
inline const String OPT_FONT_ITALIC {_T("Italic"s)};
inline const String OPT_FONT_UNDERLINE {_T("Underline"s)};
inline const String OPT_FONT_STRIKEOUT {_T("StrikeOut"s)};
inline const String OPT_FONT_CHARSET {_T("CharSet"s)};
inline const String OPT_FONT_OUTPRECISION {_T("OutPrecision"s)};
inline const String OPT_FONT_CLIPPRECISION {_T("ClipPrecision"s)};
inline const String OPT_FONT_QUALITY {_T("Quality"s)};
inline const String OPT_FONT_PITCHANDFAMILY {_T("PitchAndFamily"s)};
inline const String OPT_FONT_FACENAME {_T("FaceName"s)};
