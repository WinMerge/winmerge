/** 
 * @file  OptionsDef.h
 *
 * @brief Constants for option-names
 */
#pragma once

#include "UnicodeString.h"

#ifndef OP
#define OP(x) 
#endif

// User's language
extern const String OPT_SELECTED_LANGUAGE OP("Locale/LanguageId");

// View-menu
extern const String OPT_SHOW_UNIQUE_LEFT OP("Settings/ShowUniqueLeft");
extern const String OPT_SHOW_UNIQUE_MIDDLE OP("Settings/ShowUniqueMiddle");
extern const String OPT_SHOW_UNIQUE_RIGHT OP("Settings/ShowUniqueRight");
extern const String OPT_SHOW_DIFFERENT OP("Settings/ShowDifferent");
extern const String OPT_SHOW_IDENTICAL OP("Settings/ShowIdentical");
extern const String OPT_SHOW_BINARIES OP("Settings/ShowBinaries");
extern const String OPT_SHOW_SKIPPED OP("Settings/ShowSkipped");
extern const String OPT_SHOW_DIFFERENT_LEFT_ONLY OP("Settings/ShowDifferentLeftOnly");
extern const String OPT_SHOW_DIFFERENT_MIDDLE_ONLY OP("Settings/ShowDifferentMiddleOnly");
extern const String OPT_SHOW_DIFFERENT_RIGHT_ONLY OP("Settings/ShowDifferentRightOnly");
extern const String OPT_SHOW_MISSING_LEFT_ONLY OP("Settings/ShowMissingLeftOnly");
extern const String OPT_SHOW_MISSING_MIDDLE_ONLY OP("Settings/ShowMissingMiddleOnly");
extern const String OPT_SHOW_MISSING_RIGHT_ONLY OP("Settings/ShowMissingRightOnly");
extern const String OPT_TREE_MODE OP("Settings/TreeMode");

// Show/hide toolbar/statusbar/tabbar
extern const String OPT_SHOW_TOOLBAR OP("Settings/ShowToolbar");
extern const String OPT_SHOW_STATUSBAR OP("Settings/ShowStatusbar");
extern const String OPT_SHOW_TABBAR OP("Settings/ShowTabbar");
extern const String OPT_TOOLBAR_SIZE OP("Settings/ToolbarSize");
extern const String OPT_RESIZE_PANES OP("Settings/AutoResizePanes");

extern const String OPT_SYNTAX_HIGHLIGHT OP("Settings/HiliteSyntax");
extern const String OPT_VIEW_WHITESPACE OP("Settings/ViewWhitespace");
extern const String OPT_CONNECT_MOVED_BLOCKS OP("Settings/ConnectMovedBlocks");
extern const String OPT_SCROLL_TO_FIRST OP("Settings/ScrollToFirst");

// Difference (in-line) highlight
extern const String OPT_WORDDIFF_HIGHLIGHT OP("Settings/HiliteWordDiff");
extern const String OPT_BREAK_ON_WORDS OP("Settings/BreakOnWords");
extern const String OPT_BREAK_TYPE OP("Settings/BreakType");
extern const String OPT_BREAK_SEPARATORS OP("Settings/HiliteBreakSeparators");

// Backup options
extern const String OPT_BACKUP_FOLDERCMP OP("Backup/EnableFolder");
extern const String OPT_BACKUP_FILECMP OP("Backup/EnableFile");
extern const String OPT_BACKUP_LOCATION OP("Backup/Location");
extern const String OPT_BACKUP_GLOBALFOLDER OP("Backup/GlobalFolder");
extern const String OPT_BACKUP_ADD_BAK OP("Backup/NameAddBak");
extern const String OPT_BACKUP_ADD_TIME OP("Backup/NameAddTime");

extern const String OPT_DIRVIEW_SORT_COLUMN OP("Settings/DirViewSortCol");
extern const String OPT_DIRVIEW_SORT_COLUMN3 OP("Settings/DirViewSortCol3");
extern const String OPT_DIRVIEW_SORT_ASCENDING OP("Settings/DirViewSortAscending");
extern const String OPT_DIRVIEW_EXPAND_SUBDIRS OP("Settings/DirViewExpandSubdirs");

// Window/Pane
extern const String OPT_ACTIVE_FRAME_MAX OP("Settings/ActiveFrameMax");
extern const String OPT_ACTIVE_PANE OP("Settings/ActivePane");

// Folder Compare Report
extern const String OPT_REPORTFILES_REPORTTYPE OP("ReportFiles/ReportType");
extern const String OPT_REPORTFILES_COPYTOCLIPBOARD OP("ReportFiles/CopyToClipboard");
extern const String OPT_REPORTFILES_INCLUDEFILECMPREPORT OP("ReportFiles/IncludeFileCmpReport");

// File compare
extern const String OPT_AUTOMATIC_RESCAN OP("Settings/AutomaticRescan");
extern const String OPT_ALLOW_MIXED_EOL OP("Settings/AllowMixedEOL");
extern const String OPT_TAB_SIZE OP("Settings/TabSize");
extern const String OPT_TAB_TYPE OP("Settings/TabType");
extern const String OPT_WORDWRAP OP("Settings/WordWrap");
extern const String OPT_VIEW_LINENUMBERS OP("Settings/ViewLineNumbers");
extern const String OPT_VIEW_FILEMARGIN OP("Settings/ViewFileMargin");
extern const String OPT_DIFF_CONTEXT OP("Settings/DiffContextV2");

extern const String OPT_EXT_EDITOR_CMD OP("Settings/ExternalEditor");
extern const String OPT_USE_RECYCLE_BIN OP("Settings/UseRecycleBin");
extern const String OPT_SINGLE_INSTANCE OP("Settings/SingleInstance");
extern const String OPT_MERGE_MODE OP("Settings/MergingMode");
extern const String OPT_CLOSE_WITH_ESC OP("Settings/CloseWithEsc");
extern const String OPT_CLOSE_WITH_OK OP("Settings/CloseWithOK");
extern const String OPT_VERIFY_OPEN_PATHS OP("Settings/VerifyOpenPaths");
extern const String OPT_AUTO_COMPLETE_SOURCE OP("Settings/AutoCompleteSource");
extern const String OPT_IGNORE_SMALL_FILETIME OP("Settings/IgnoreSmallFileTime");
extern const String OPT_ASK_MULTIWINDOW_CLOSE OP("Settings/AskClosingMultipleWindows");
extern const String OPT_PRESERVE_FILETIMES OP("Settings/PreserveFiletimes");

extern const String OPT_CP_DEFAULT_MODE OP("Settings/CodepageDefaultMode");
extern const String OPT_CP_DEFAULT_CUSTOM OP("Settings/CodepageDefaultCustomValue");
extern const String OPT_CP_DETECT OP("Settings/CodepageDetection");

extern const String OPT_PROJECTS_PATH OP("Settings/ProjectsPath");
extern const String OPT_USE_SYSTEM_TEMP_PATH OP("Settings/UseSystemTempPath");
extern const String OPT_CUSTOM_TEMP_PATH OP("Settings/CustomTempPath");

extern const String OPT_SPLIT_HORIZONTALLY OP("Settings/SplitHorizontally");

// Color options
// The difference color
extern const String OPT_CLR_DIFF OP("Settings/DifferenceColor");
// The selected difference color
extern const String OPT_CLR_SELECTED_DIFF OP("Settings/SelectedDifferenceColor");
// The difference deleted color
extern const String OPT_CLR_DIFF_DELETED OP("Settings/DifferenceDeletedColor");
// The selected difference deleted color
extern const String OPT_CLR_SELECTED_DIFF_DELETED OP("Settings/SelectedDifferenceDeletedColor");
// The difference text color
extern const String OPT_CLR_DIFF_TEXT OP("Settings/DifferenceTextColor");
// The selected difference text color
extern const String OPT_CLR_SELECTED_DIFF_TEXT OP("Settings/SelectedDifferenceTextColor");
// The ignored lines color
extern const String OPT_CLR_TRIVIAL_DIFF OP("Settings/TrivialDifferenceColor");
// The ignored and deleted lines color
extern const String OPT_CLR_TRIVIAL_DIFF_DELETED OP("Settings/TrivialDifferenceDeletedColor");
// The ignored text color
extern const String OPT_CLR_TRIVIAL_DIFF_TEXT OP("Settings/TrivialDifferenceTextColor");
// The moved block color
extern const String OPT_CLR_MOVEDBLOCK OP("Settings/MovedBlockColor");
// The moved block deleted lines color
extern const String OPT_CLR_MOVEDBLOCK_DELETED OP("Settings/MovedBlockDeletedColor");
// The moved block text color
extern const String OPT_CLR_MOVEDBLOCK_TEXT OP("Settings/MovedBlockTextColor");
// The selected moved block color
extern const String OPT_CLR_SELECTED_MOVEDBLOCK OP("Settings/SelectedMovedBlockColor");
// The selected moved block deleted lines
extern const String OPT_CLR_SELECTED_MOVEDBLOCK_DELETED OP("Settings/SelectedMovedBlockDeletedColor");
// The selected moved block text color
extern const String OPT_CLR_SELECTED_MOVEDBLOCK_TEXT OP("Settings/SelectedMovedBlockTextColor");
// The SNP block color
extern const String OPT_CLR_SNP OP("Settings/SNPColor");
// The SNP block deleted lines color
extern const String OPT_CLR_SNP_DELETED OP("Settings/SNPDeletedColor");
// The SNP block text color
extern const String OPT_CLR_SNP_TEXT OP("Settings/SNPTextColor");
// The selected SNP block color
extern const String OPT_CLR_SELECTED_SNP OP("Settings/SelectedSNPColor");
// The selected SNP block deleted lines
extern const String OPT_CLR_SELECTED_SNP_DELETED OP("Settings/SelectedSNPDeletedColor");
// The selected SNP block text color
extern const String OPT_CLR_SELECTED_SNP_TEXT OP("Settings/SelectedSNPTextColor");
// The word difference color
extern const String OPT_CLR_WORDDIFF OP("Settings/WordDifferenceColor");
// The word difference deleted color
extern const String OPT_CLR_WORDDIFF_DELETED OP("Settings/WordDifferenceDeletedColor");
// The word difference text color
extern const String OPT_CLR_WORDDIFF_TEXT OP("Settings/WordDifferenceTextColor");
// The selected word difference color
extern const String OPT_CLR_SELECTED_WORDDIFF OP("Settings/SelectedWordDifferenceColor");
// The word difference deleted color
extern const String OPT_CLR_SELECTED_WORDDIFF_DELETED OP("Settings/SelectedWordDifferenceDeletedColor");
// The selected word difference text color
extern const String OPT_CLR_SELECTED_WORDDIFF_TEXT OP("Settings/SelectedWordDifferenceTextColor");
// Whether to use default (theme) text colors
extern const String OPT_CLR_DEFAULT_TEXT_COLORING OP("Settings/DefaultTextColoring");

// DirCompare Color options
// The item equal background color
extern const String OPT_DIRCLR_ITEM_EQUAL OP("Settings/DirItemEqualColor");
// The item equal text color
extern const String OPT_DIRCLR_ITEM_EQUAL_TEXT OP("Settings/DirItemEqualTextColor");
// The item diff background color
extern const String OPT_DIRCLR_ITEM_DIFF OP("Settings/DirItemDiffColor");
// The item diff text color
extern const String OPT_DIRCLR_ITEM_DIFF_TEXT OP("Settings/DirItemDiffTextColor");
// The item not-exist-all background color
extern const String OPT_DIRCLR_ITEM_NOT_EXIST_ALL OP("Settings/DirItemNotExistAllColor");
// The item not-exist-all text color
extern const String OPT_DIRCLR_ITEM_NOT_EXIST_ALL_TEXT OP("Settings/DirItemNotExistAllTextColor");
// The item filtered background color
extern const String OPT_DIRCLR_ITEM_FILTERED OP("Settings/DirItemFilteredColor");
// The item filtered text color
extern const String OPT_DIRCLR_ITEM_FILTERED_TEXT OP("Settings/DirItemFilteredTextColor");
// Use dir compare coloring?
extern const String OPT_DIRCLR_USE_COLORS OP("Settings/UseDirCompareColors");

// Compare options
extern const String OPT_CMP_IGNORE_WHITESPACE OP("Settings/IgnoreSpace");
extern const String OPT_CMP_IGNORE_BLANKLINES OP("Settings/IgnoreBlankLines");
extern const String OPT_CMP_FILTER_COMMENTLINES OP("Settings/FilterCommentsLines");
extern const String OPT_CMP_IGNORE_CASE OP("Settings/IgnoreCase");
extern const String OPT_CMP_IGNORE_EOL OP("Settings/IgnoreEol");
extern const String OPT_CMP_IGNORE_CODEPAGE OP("Settings/IgnoreCodepage");
extern const String OPT_CMP_METHOD OP("Settings/CompMethod2");
extern const String OPT_CMP_MOVED_BLOCKS OP("Settings/MovedBlocks");
extern const String OPT_CMP_MATCH_SIMILAR_LINES OP("Settings/MatchSimilarLines");
extern const String OPT_CMP_STOP_AFTER_FIRST OP("Settings/StopAfterFirst");
extern const String OPT_CMP_QUICK_LIMIT OP("Settings/QuickMethodLimit");
extern const String OPT_CMP_BINARY_LIMIT OP("Settings/BinaryMethodLimit");
extern const String OPT_CMP_COMPARE_THREADS OP("Settings/CompareThreads");
extern const String OPT_CMP_WALK_UNIQUE_DIRS OP("Settings/ScanUnpairedDir");
extern const String OPT_CMP_IGNORE_REPARSE_POINTS OP("Settings/IgnoreReparsePoints");
extern const String OPT_CMP_INCLUDE_SUBDIRS OP("Settings/Recurse");
extern const String OPT_CMP_DIFF_ALGORITHM OP("Settings/DiffAlgorithm");
extern const String OPT_CMP_INDENT_HEURISTIC OP("Settings/IndentHeuristic");

// Image Compare options
extern const String OPT_CMP_IMG_FILEPATTERNS OP("Settings/ImageFilePatterns");
extern const String OPT_CMP_IMG_SHOWDIFFERENCES OP("Settings/ImageShowDifferences");
extern const String OPT_CMP_IMG_OVERLAYMOVE OP("Settings/ImageOverlayMode");
extern const String OPT_CMP_IMG_OVERLAYALPHA OP("Settings/ImageOverlayAlpha");
extern const String OPT_CMP_IMG_DRAGGING_MODE OP("Settings/ImageDraggingMode");
extern const String OPT_CMP_IMG_ZOOM OP("Settings/ImageZoom");
extern const String OPT_CMP_IMG_USEBACKCOLOR OP("Settings/ImageUseBackColor");
extern const String OPT_CMP_IMG_BACKCOLOR OP("Settings/ImageBackColor");
extern const String OPT_CMP_IMG_DIFFBLOCKSIZE OP("Settings/ImageDiffBlockSize");
extern const String OPT_CMP_IMG_DIFFCOLORALPHA OP("Settings/ImageDiffColorAlpha");
extern const String OPT_CMP_IMG_THRESHOLD OP("Settings/ImageColorDistanceThreshold");
extern const String OPT_CMP_IMG_INSERTIONDELETIONDETECTION_MODE OP("Settings/ImageInsertionDeletionDetectionMode");

// Image Binary options
extern const String OPT_CMP_BIN_FILEPATTERNS OP("Settings/BinaryFilePatterns");

/// Are regular expression linefilters enabled?
extern const String OPT_LINEFILTER_ENABLED OP("Settings/IgnoreRegExp");
/// Currently selected filefilter
extern const String OPT_FILEFILTER_CURRENT OP("Settings/FileFilterCurrent");
extern const String OPT_FILTER_USERPATH OP("Settings/UserFilterPath");
extern const String OPT_FILEFILTER_SHARED OP("Settings/Filters/Shared");

// Archive support
extern const String OPT_ARCHIVE_ENABLE OP("Merge7z/Enable");
extern const String OPT_ARCHIVE_PROBETYPE OP("Merge7z/ProbeSignature");
extern const String OPT_ARCHIVE_FILTER_INDEX OP("Merge7z/FilterIndex");

// Patch Creator
extern const String OPT_PATCHCREATOR_PATCH_STYLE OP("PatchCreator/PatchStyle");
extern const String OPT_PATCHCREATOR_CONTEXT_LINES OP("PatchCreator/ContextLines");
extern const String OPT_PATCHCREATOR_CASE_SENSITIVE OP("PatchCreator/CaseSensitive");
extern const String OPT_PATCHCREATOR_EOL_SENSITIVE OP("PatchCreator/EOLSensitive");
extern const String OPT_PATCHCREATOR_IGNORE_BLANK_LINES OP("PatchCreator/IgnoreBlankLines");
extern const String OPT_PATCHCREATOR_WHITESPACE OP("PatchCreator/Whitespace");
extern const String OPT_PATCHCREATOR_OPEN_TO_EDITOR OP("PatchCreator/OpenToEditor");
extern const String OPT_PATCHCREATOR_INCLUDE_CMD_LINE OP("PatchCreator/IncludeCmdLine");

// Plugins
extern const String OPT_PLUGINS_ENABLED OP("Settings/PluginsEnabled");
extern const String OPT_PLUGINS_DISABLED_LIST OP("Settings/PluginsDisabledList");
extern const String OPT_PLUGINS_UNPACKER_MODE OP("Settings/UnpackerMode");
extern const String OPT_PLUGINS_PREDIFFER_MODE OP("Settings/PredifferMode");
extern const String OPT_PLUGINS_UNPACK_DONT_CHECK_EXTENSION OP("Plugins/UnpackDontCheckExtension");

// Startup options
extern const String OPT_SHOW_SELECT_FILES_AT_STARTUP OP("Settings/ShowFileDialog");

// MDI Tab Bar
extern const String OPT_TABBAR_AUTO_MAXWIDTH OP("Settings/TabBarAutoMaxWidth");

// MRU
extern const String OPT_MRU_MAX OP("Settings/MRUMax");

// Font options
extern const String OPT_FONT_FILECMP OP("Font/");
extern const String OPT_FONT_DIRCMP OP("FontDirCompare/");

extern const String OPT_FONT_USECUSTOM OP("Specified");
extern const String OPT_FONT_POINTSIZE OP("PointSize");
extern const String OPT_FONT_HEIGHT OP("Height");
extern const String OPT_FONT_ESCAPEMENT OP("Escapement");
extern const String OPT_FONT_ORIENTATION OP("Orientation");
extern const String OPT_FONT_WEIGHT OP("Weight");
extern const String OPT_FONT_ITALIC OP("Italic");
extern const String OPT_FONT_UNDERLINE OP("Underline");
extern const String OPT_FONT_STRIKEOUT OP("StrikeOut");
extern const String OPT_FONT_CHARSET OP("CharSet");
extern const String OPT_FONT_OUTPRECISION OP("OutPrecision");
extern const String OPT_FONT_CLIPPRECISION OP("ClipPrecision");
extern const String OPT_FONT_QUALITY OP("Quality");
extern const String OPT_FONT_PITCHANDFAMILY OP("PitchAndFamily");
extern const String OPT_FONT_FACENAME OP("FaceName");
