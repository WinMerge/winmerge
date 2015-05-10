/**
* @file  OptionsDef.cpp
*
* @brief Constants for option-names
*/
#include "OptionsDef.h"

#define _T2(x) _T(x), sizeof(_T(x))/sizeof(TCHAR) - 1

// User's language
const String OPT_SELECTED_LANGUAGE(_T2("Locale/LanguageId"));

// View-menu
const String OPT_SHOW_UNIQUE_LEFT(_T2("Settings/ShowUniqueLeft"));
const String OPT_SHOW_UNIQUE_RIGHT(_T2("Settings/ShowUniqueRight"));
const String OPT_SHOW_DIFFERENT(_T2("Settings/ShowDifferent"));
const String OPT_SHOW_IDENTICAL(_T2("Settings/ShowIdentical"));
const String OPT_SHOW_BINARIES(_T2("Settings/ShowBinaries"));
const String OPT_SHOW_SKIPPED(_T2("Settings/ShowSkipped"));
const String OPT_TREE_MODE(_T2("Settings/TreeMode"));

// Show/hide toolbar/statusbar/tabbar
const String OPT_SHOW_TOOLBAR(_T2("Settings/ShowToolbar"));
const String OPT_SHOW_STATUSBAR(_T2("Settings/ShowStatusbar"));
const String OPT_SHOW_TABBAR(_T2("Settings/ShowTabbar"));
const String OPT_TOOLBAR_SIZE(_T2("Settings/ToolbarSize"));
const String OPT_RESIZE_PANES(_T2("Settings/AutoResizePanes"));

const String OPT_SYNTAX_HIGHLIGHT(_T2("Settings/HiliteSyntax"));
const String OPT_VIEW_WHITESPACE(_T2("Settings/ViewWhitespace"));
const String OPT_CONNECT_MOVED_BLOCKS(_T2("Settings/ConnectMovedBlocks"));
const String OPT_SCROLL_TO_FIRST(_T2("Settings/ScrollToFirst"));

// Difference (in-line) highlight
const String OPT_WORDDIFF_HIGHLIGHT(_T2("Settings/HiliteWordDiff"));
const String OPT_BREAK_ON_WORDS(_T2("Settings/BreakOnWords"));
const String OPT_BREAK_TYPE(_T2("Settings/BreakType"));
const String OPT_BREAK_SEPARATORS(_T2("Settings/HiliteBreakSeparators"));

// Backup options
const String OPT_BACKUP_FOLDERCMP(_T2("Backup/EnableFolder"));
const String OPT_BACKUP_FILECMP(_T2("Backup/EnableFile"));
const String OPT_BACKUP_LOCATION(_T2("Backup/Location"));
const String OPT_BACKUP_GLOBALFOLDER(_T2("Backup/GlobalFolder"));
const String OPT_BACKUP_ADD_BAK(_T2("Backup/NameAddBak"));
const String OPT_BACKUP_ADD_TIME(_T2("Backup/NameAddTime"));

const String OPT_DIRVIEW_SORT_COLUMN(_T2("Settings/DirViewSortCol"));
const String OPT_DIRVIEW_SORT_COLUMN3(_T2("Settings/DirViewSortCol3"));
const String OPT_DIRVIEW_SORT_ASCENDING(_T2("Settings/DirViewSortAscending"));
const String OPT_DIRVIEW_EXPAND_SUBDIRS(_T2("Settings/DirViewExpandSubdirs"));

// File compare
const String OPT_AUTOMATIC_RESCAN(_T2("Settings/AutomaticRescan"));
const String OPT_ALLOW_MIXED_EOL(_T2("Settings/AllowMixedEOL"));
const String OPT_TAB_SIZE(_T2("Settings/TabSize"));
const String OPT_TAB_TYPE(_T2("Settings/TabType"));
const String OPT_WORDWRAP(_T2("Settings/WordWrap"));
const String OPT_VIEW_LINENUMBERS(_T2("Settings/ViewLineNumbers"));
const String OPT_VIEW_FILEMARGIN(_T2("Settings/ViewFileMargin"));
const String OPT_DIFF_CONTEXT(_T2("Settings/DiffContext"));

const String OPT_EXT_EDITOR_CMD(_T2("Settings/ExternalEditor"));
const String OPT_USE_RECYCLE_BIN(_T2("Settings/UseRecycleBin"));
const String OPT_SINGLE_INSTANCE(_T2("Settings/SingleInstance"));
const String OPT_MERGE_MODE(_T2("Settings/MergingMode"));
const String OPT_CLOSE_WITH_ESC(_T2("Settings/CloseWithEsc"));
const String OPT_CLOSE_WITH_OK(_T2("Settings/CloseWithOK"));
const String OPT_VERIFY_OPEN_PATHS(_T2("Settings/VerifyOpenPaths"));
const String OPT_AUTO_COMPLETE_SOURCE(_T2("Settings/AutoCompleteSource"));
const String OPT_IGNORE_SMALL_FILETIME(_T2("Settings/IgnoreSmallFileTime"));
const String OPT_ASK_MULTIWINDOW_CLOSE(_T2("Settings/AskClosingMultipleWindows"));
const String OPT_PRESERVE_FILETIMES(_T2("Settings/PreserveFiletimes"));

const String OPT_CP_DEFAULT_MODE(_T2("Settings/CodepageDefaultMode"));
const String OPT_CP_DEFAULT_CUSTOM(_T2("Settings/CodepageDefaultCustomValue"));
const String OPT_CP_DETECT(_T2("Settings/CodepageDetection"));

const String OPT_PROJECTS_PATH(_T2("Settings/ProjectsPath"));
const String OPT_USE_SYSTEM_TEMP_PATH(_T2("Settings/UseSystemTempPath"));
const String OPT_CUSTOM_TEMP_PATH(_T2("Settings/CustomTempPath"));

const String OPT_SPLIT_HORIZONTALLY(_T2("Settings/SplitHorizontally"));

// Color options
// The difference color
const String OPT_CLR_DIFF(_T2("Settings/DifferenceColor"));
// The selected difference color
const String OPT_CLR_SELECTED_DIFF(_T2("Settings/SelectedDifferenceColor"));
// The difference deleted color
const String OPT_CLR_DIFF_DELETED(_T2("Settings/DifferenceDeletedColor"));
// The selected difference deleted color
const String OPT_CLR_SELECTED_DIFF_DELETED(_T2("Settings/SelectedDifferenceDeletedColor"));
// The difference text color
const String OPT_CLR_DIFF_TEXT(_T2("Settings/DifferenceTextColor"));
// The selected difference text color
const String OPT_CLR_SELECTED_DIFF_TEXT(_T2("Settings/SelectedDifferenceTextColor"));
// The ignored lines color
const String OPT_CLR_TRIVIAL_DIFF(_T2("Settings/TrivialDifferenceColor"));
// The ignored and deleted lines color
const String OPT_CLR_TRIVIAL_DIFF_DELETED(_T2("Settings/TrivialDifferenceDeletedColor"));
// The ignored text color
const String OPT_CLR_TRIVIAL_DIFF_TEXT(_T2("Settings/TrivialDifferenceTextColor"));
// The moved block color
const String OPT_CLR_MOVEDBLOCK(_T2("Settings/MovedBlockColor"));
// The moved block deleted lines color
const String OPT_CLR_MOVEDBLOCK_DELETED(_T2("Settings/MovedBlockDeletedColor"));
// The moved block text color
const String OPT_CLR_MOVEDBLOCK_TEXT(_T2("Settings/MovedBlockTextColor"));
// The selected moved block color
const String OPT_CLR_SELECTED_MOVEDBLOCK(_T2("Settings/SelectedMovedBlockColor"));
// The selected moved block deleted lines
const String OPT_CLR_SELECTED_MOVEDBLOCK_DELETED(_T2("Settings/SelectedMovedBlockDeletedColor"));
// The selected moved block text color
const String OPT_CLR_SELECTED_MOVEDBLOCK_TEXT(_T2("Settings/SelectedMovedBlockTextColor"));
// The SNP block color
const String OPT_CLR_SNP(_T2("Settings/SNPColor"));
// The SNP block deleted lines color
const String OPT_CLR_SNP_DELETED(_T2("Settings/SNPDeletedColor"));
// The SNP block text color
const String OPT_CLR_SNP_TEXT(_T2("Settings/SNPTextColor"));
// The selected SNP block color
const String OPT_CLR_SELECTED_SNP(_T2("Settings/SelectedSNPColor"));
// The selected SNP block deleted lines
const String OPT_CLR_SELECTED_SNP_DELETED(_T2("Settings/SelectedSNPDeletedColor"));
// The selected SNP block text color
const String OPT_CLR_SELECTED_SNP_TEXT(_T2("Settings/SelectedSNPTextColor"));
// The word difference color
const String OPT_CLR_WORDDIFF(_T2("Settings/WordDifferenceColor"));
// The word difference deleted color
const String OPT_CLR_WORDDIFF_DELETED(_T2("Settings/WordDifferenceDeletedColor"));
// The word difference text color
const String OPT_CLR_WORDDIFF_TEXT(_T2("Settings/WordDifferenceTextColor"));
// The selected word difference color
const String OPT_CLR_SELECTED_WORDDIFF(_T2("Settings/SelectedWordDifferenceColor"));
// The word difference deleted color
const String OPT_CLR_SELECTED_WORDDIFF_DELETED(_T2("Settings/SelectedWordDifferenceDeletedColor"));
// The selected word difference text color
const String OPT_CLR_SELECTED_WORDDIFF_TEXT(_T2("Settings/SelectedWordDifferenceTextColor"));
// Whether to use default (theme) text colors
const String OPT_CLR_DEFAULT_TEXT_COLORING(_T2("Settings/DefaultTextColoring"));

// Compare options
const String OPT_CMP_IGNORE_WHITESPACE(_T2("Settings/IgnoreSpace"));
const String OPT_CMP_IGNORE_BLANKLINES(_T2("Settings/IgnoreBlankLines"));
const String OPT_CMP_FILTER_COMMENTLINES(_T2("Settings/FilterCommentsLines"));
const String OPT_CMP_IGNORE_CASE(_T2("Settings/IgnoreCase"));
const String OPT_CMP_IGNORE_EOL(_T2("Settings/IgnoreEol"));
const String OPT_CMP_METHOD(_T2("Settings/CompMethod2"));
const String OPT_CMP_MOVED_BLOCKS(_T2("Settings/MovedBlocks"));
const String OPT_CMP_MATCH_SIMILAR_LINES(_T2("Settings/MatchSimilarLines"));
const String OPT_CMP_STOP_AFTER_FIRST(_T2("Settings/StopAfterFirst"));
const String OPT_CMP_QUICK_LIMIT(_T2("Settings/QuickMethodLimit"));
const String OPT_CMP_WALK_UNIQUE_DIRS(_T2("Settings/ScanUnpairedDir"));
const String OPT_CMP_IGNORE_REPARSE_POINTS(_T2("Settings/IgnoreReparsePoints"));

// Image Compare options
const String OPT_CMP_IMG_FILEPATTERNS(_T2("Settings/ImageFilePatterns"));
const String OPT_CMP_IMG_SHOWDIFFERENCES(_T2("Settings/ImageShowDifferences"));
const String OPT_CMP_IMG_OVERLAYMOVE(_T2("Settings/ImageOverlayMode"));
const String OPT_CMP_IMG_ZOOM(_T2("Settings/ImageZoom"));
const String OPT_CMP_IMG_USEBACKCOLOR(_T2("Settings/ImageUseBackColor"));
const String OPT_CMP_IMG_BACKCOLOR(_T2("Settings/ImageBackColor"));
const String OPT_CMP_IMG_DIFFBLOCKSIZE(_T2("Settings/ImageDiffBlockSize"));
const String OPT_CMP_IMG_THRESHOLD(_T2("Settings/ImageColorDistanceThreshold"));

// Image Binary options
const String OPT_CMP_BIN_FILEPATTERNS(_T2("Settings/BinaryFilePatterns"));

/// Are regular expression linefilters enabled?
const String OPT_LINEFILTER_ENABLED(_T2("Settings/IgnoreRegExp"));
/// Currently selected filefilter
const String OPT_FILEFILTER_CURRENT(_T2("Settings/FileFilterCurrent"));
const String OPT_FILTER_USERPATH(_T2("Settings/UserFilterPath"));
const String OPT_FILEFILTER_SHARED(_T2("Settings/Filters/Shared"));

// Version control
const String OPT_VCS_SYSTEM(_T2("Settings/VersionSystem"));
const String OPT_VSS_PATH(_T2("Settings/VssPath"));

// Archive support
const String OPT_ARCHIVE_ENABLE(_T2("Merge7z/Enable"));
const String OPT_ARCHIVE_PROBETYPE(_T2("Merge7z/ProbeSignature"));

// Plugins
const String OPT_PLUGINS_ENABLED(_T2("Settings/PluginsEnabled"));

// Startup options
const String OPT_SHOW_SELECT_FILES_AT_STARTUP(_T2("Settings/ShowFileDialog"));

// MDI Tab Bar
const String OPT_TABBAR_AUTO_MAXWIDTH(_T2("Settings/TabBarAutoMaxWidth"));

// Font options
const String OPT_FONT_FILECMP(_T2("Font/"));
const String OPT_FONT_DIRCMP(_T2("FontDirCompare/"));

const String OPT_FONT_USECUSTOM(_T2("Specified"));
const String OPT_FONT_HEIGHT(_T2("Height"));
const String OPT_FONT_WIDTH(_T2("Width"));
const String OPT_FONT_ESCAPEMENT(_T2("Escapement"));
const String OPT_FONT_ORIENTATION(_T2("Orientation"));
const String OPT_FONT_WEIGHT(_T2("Weight"));
const String OPT_FONT_ITALIC(_T2("Italic"));
const String OPT_FONT_UNDERLINE(_T2("Underline"));
const String OPT_FONT_STRIKEOUT(_T2("StrikeOut"));
const String OPT_FONT_CHARSET(_T2("CharSet"));
const String OPT_FONT_OUTPRECISION(_T2("OutPrecision"));
const String OPT_FONT_CLIPPRECISION(_T2("ClipPrecision"));
const String OPT_FONT_QUALITY(_T2("Quality"));
const String OPT_FONT_PITCHANDFAMILY(_T2("PitchAndFamily"));
const String OPT_FONT_FACENAME(_T2("FaceName"));
