/** 
 * @file  OptionsDef.h
 *
 * @brief Constants for option-names
 */
#pragma once

#include "UnicodeString.h"

// User's language
const TCHAR OPT_SELECTED_LANGUAGE[] = _T("Locale/LanguageId");

// View-menu
const TCHAR OPT_SHOW_UNIQUE_LEFT[] = _T("Settings/ShowUniqueLeft");
const TCHAR OPT_SHOW_UNIQUE_RIGHT[] = _T("Settings/ShowUniqueRight");
const TCHAR OPT_SHOW_DIFFERENT[] = _T("Settings/ShowDifferent");
const TCHAR OPT_SHOW_IDENTICAL[] = _T("Settings/ShowIdentical");
const TCHAR OPT_SHOW_BINARIES[] = _T("Settings/ShowBinaries");
const TCHAR OPT_SHOW_SKIPPED[] = _T("Settings/ShowSkipped");
const TCHAR OPT_TREE_MODE[] = _T("Settings/TreeMode");

// Show/hide toolbar/statusbar/tabbar
const TCHAR OPT_SHOW_TOOLBAR[] = _T("Settings/ShowToolbar");
const TCHAR OPT_SHOW_STATUSBAR[] = _T("Settings/ShowStatusbar");
const TCHAR OPT_SHOW_TABBAR[] = _T("Settings/ShowTabbar");
const TCHAR OPT_TOOLBAR_SIZE[] = _T("Settings/ToolbarSize");
const TCHAR OPT_RESIZE_PANES[] = _T("Settings/AutoResizePanes");

const TCHAR OPT_SYNTAX_HIGHLIGHT[] = _T("Settings/HiliteSyntax");
const TCHAR OPT_VIEW_WHITESPACE[] =  _T("Settings/ViewWhitespace");
const TCHAR OPT_CONNECT_MOVED_BLOCKS[] = _T("Settings/ConnectMovedBlocks");
const TCHAR OPT_SCROLL_TO_FIRST[] =  _T("Settings/ScrollToFirst");

// Difference (in-line) highlight
const TCHAR OPT_WORDDIFF_HIGHLIGHT[] = _T("Settings/HiliteWordDiff");
const TCHAR OPT_BREAK_ON_WORDS[] = _T("Settings/BreakOnWords");
const TCHAR OPT_BREAK_TYPE[] = _T("Settings/BreakType");
const TCHAR OPT_BREAK_SEPARATORS[] = _T("Settings/HiliteBreakSeparators");

// Backup options
const TCHAR OPT_BACKUP_FOLDERCMP[] = _T("Backup/EnableFolder");
const TCHAR OPT_BACKUP_FILECMP[] = _T("Backup/EnableFile");
const TCHAR OPT_BACKUP_LOCATION[] = _T("Backup/Location");
const TCHAR OPT_BACKUP_GLOBALFOLDER[] = _T("Backup/GlobalFolder");
const TCHAR OPT_BACKUP_ADD_BAK[] = _T("Backup/NameAddBak");
const TCHAR OPT_BACKUP_ADD_TIME[] = _T("Backup/NameAddTime");

const TCHAR OPT_DIRVIEW_SORT_COLUMN[] =  _T("Settings/DirViewSortCol");
const TCHAR OPT_DIRVIEW_SORT_COLUMN3[] =  _T("Settings/DirViewSortCol3");
const TCHAR OPT_DIRVIEW_SORT_ASCENDING[] =  _T("Settings/DirViewSortAscending");
const TCHAR OPT_DIRVIEW_EXPAND_SUBDIRS[] = _T("Settings/DirViewExpandSubdirs");

// File compare
const TCHAR OPT_AUTOMATIC_RESCAN[] = _T("Settings/AutomaticRescan");
const TCHAR OPT_ALLOW_MIXED_EOL[] = _T("Settings/AllowMixedEOL");
const TCHAR OPT_TAB_SIZE[] = _T("Settings/TabSize");
const TCHAR OPT_TAB_TYPE[] = _T("Settings/TabType");
const TCHAR OPT_WORDWRAP[] = _T("Settings/WordWrap");
const TCHAR OPT_VIEW_LINENUMBERS[] = _T("Settings/ViewLineNumbers");
const TCHAR OPT_VIEW_FILEMARGIN[] = _T("Settings/ViewFileMargin");
const TCHAR OPT_DIFF_CONTEXT[] = _T("Settings/DiffContext");

const TCHAR OPT_EXT_EDITOR_CMD[] = _T("Settings/ExternalEditor");
const TCHAR OPT_USE_RECYCLE_BIN[] = _T("Settings/UseRecycleBin");
const TCHAR OPT_SINGLE_INSTANCE[] = _T("Settings/SingleInstance");
const TCHAR OPT_MERGE_MODE[] = _T("Settings/MergingMode");
const TCHAR OPT_CLOSE_WITH_ESC[] = _T("Settings/CloseWithEsc");
const TCHAR OPT_CLOSE_WITH_OK[] = _T("Settings/CloseWithOK");
const TCHAR OPT_VERIFY_OPEN_PATHS[] = _T("Settings/VerifyOpenPaths");
const TCHAR OPT_AUTO_COMPLETE_SOURCE[] = _T("Settings/AutoCompleteSource");
const TCHAR OPT_IGNORE_SMALL_FILETIME[] = _T("Settings/IgnoreSmallFileTime");
const TCHAR OPT_ASK_MULTIWINDOW_CLOSE[] = _T("Settings/AskClosingMultipleWindows");
const TCHAR OPT_PRESERVE_FILETIMES[] = _T("Settings/PreserveFiletimes");

const TCHAR OPT_CP_DEFAULT_MODE[] = _T("Settings/CodepageDefaultMode");
const TCHAR OPT_CP_DEFAULT_CUSTOM[] = _T("Settings/CodepageDefaultCustomValue");
const TCHAR OPT_CP_DETECT[] = _T("Settings/CodepageDetection");

const TCHAR OPT_PROJECTS_PATH[] = _T("Settings/ProjectsPath");
const TCHAR OPT_USE_SYSTEM_TEMP_PATH[] = _T("Settings/UseSystemTempPath");
const TCHAR OPT_CUSTOM_TEMP_PATH[] = _T("Settings/CustomTempPath");

const TCHAR OPT_SPLIT_HORIZONTALLY[] = _T("Settings/SplitHorizontally");

// Color options
// The difference color
const TCHAR OPT_CLR_DIFF[] = _T("Settings/DifferenceColor");
// The selected difference color
const TCHAR OPT_CLR_SELECTED_DIFF[] = _T("Settings/SelectedDifferenceColor");
// The difference deleted color
const TCHAR OPT_CLR_DIFF_DELETED[] = _T("Settings/DifferenceDeletedColor");
// The selected difference deleted color
const TCHAR OPT_CLR_SELECTED_DIFF_DELETED[] = _T("Settings/SelectedDifferenceDeletedColor");
// The difference text color
const TCHAR OPT_CLR_DIFF_TEXT[] = _T("Settings/DifferenceTextColor");
// The selected difference text color
const TCHAR OPT_CLR_SELECTED_DIFF_TEXT[] = _T("Settings/SelectedDifferenceTextColor");
// The ignored lines color
const TCHAR OPT_CLR_TRIVIAL_DIFF[] = _T("Settings/TrivialDifferenceColor");
// The ignored and deleted lines color
const TCHAR OPT_CLR_TRIVIAL_DIFF_DELETED[] = _T("Settings/TrivialDifferenceDeletedColor");
// The ignored text color
const TCHAR OPT_CLR_TRIVIAL_DIFF_TEXT[] = _T("Settings/TrivialDifferenceTextColor");
// The moved block color
const TCHAR OPT_CLR_MOVEDBLOCK[] = _T("Settings/MovedBlockColor");
// The moved block deleted lines color
const TCHAR OPT_CLR_MOVEDBLOCK_DELETED[] = _T("Settings/MovedBlockDeletedColor");
// The moved block text color
const TCHAR OPT_CLR_MOVEDBLOCK_TEXT[] = _T("Settings/MovedBlockTextColor");
// The selected moved block color
const TCHAR OPT_CLR_SELECTED_MOVEDBLOCK[] = _T("Settings/SelectedMovedBlockColor");
// The selected moved block deleted lines
const TCHAR OPT_CLR_SELECTED_MOVEDBLOCK_DELETED[] = _T("Settings/SelectedMovedBlockDeletedColor");
// The selected moved block text color
const TCHAR OPT_CLR_SELECTED_MOVEDBLOCK_TEXT[] = _T("Settings/SelectedMovedBlockTextColor");
// The SNP block color
const TCHAR OPT_CLR_SNP[] = _T("Settings/SNPColor");
// The SNP block deleted lines color
const TCHAR OPT_CLR_SNP_DELETED[] = _T("Settings/SNPDeletedColor");
// The SNP block text color
const TCHAR OPT_CLR_SNP_TEXT[] = _T("Settings/SNPTextColor");
// The selected SNP block color
const TCHAR OPT_CLR_SELECTED_SNP[] = _T("Settings/SelectedSNPColor");
// The selected SNP block deleted lines
const TCHAR OPT_CLR_SELECTED_SNP_DELETED[] = _T("Settings/SelectedSNPDeletedColor");
// The selected SNP block text color
const TCHAR OPT_CLR_SELECTED_SNP_TEXT[] = _T("Settings/SelectedSNPTextColor");
// The word difference color
const TCHAR OPT_CLR_WORDDIFF[] = _T("Settings/WordDifferenceColor");
// The word difference deleted color
const TCHAR OPT_CLR_WORDDIFF_DELETED[] = _T("Settings/WordDifferenceDeletedColor");
// The word difference text color
const TCHAR OPT_CLR_WORDDIFF_TEXT[] = _T("Settings/WordDifferenceTextColor");
// The selected word difference color
const TCHAR OPT_CLR_SELECTED_WORDDIFF[] = _T("Settings/SelectedWordDifferenceColor");
// The word difference deleted color
const TCHAR OPT_CLR_SELECTED_WORDDIFF_DELETED[] = _T("Settings/SelectedWordDifferenceDeletedColor");
// The selected word difference text color
const TCHAR OPT_CLR_SELECTED_WORDDIFF_TEXT[] = _T("Settings/SelectedWordDifferenceTextColor");
// Whether to use default (theme) text colors
const TCHAR OPT_CLR_DEFAULT_TEXT_COLORING[] = _T("Settings/DefaultTextColoring");

// Compare options
const TCHAR OPT_CMP_IGNORE_WHITESPACE[] = _T("Settings/IgnoreSpace");
const TCHAR OPT_CMP_IGNORE_BLANKLINES[] = _T("Settings/IgnoreBlankLines");
const TCHAR OPT_CMP_FILTER_COMMENTLINES[] = _T("Settings/FilterCommentsLines");
const TCHAR OPT_CMP_IGNORE_CASE[] = _T("Settings/IgnoreCase");
const TCHAR OPT_CMP_IGNORE_EOL[] = _T("Settings/IgnoreEol");
const TCHAR OPT_CMP_METHOD[] = _T("Settings/CompMethod2");
const TCHAR OPT_CMP_MOVED_BLOCKS[] = _T("Settings/MovedBlocks");
const TCHAR OPT_CMP_MATCH_SIMILAR_LINES[] = _T("Settings/MatchSimilarLines");
const TCHAR OPT_CMP_STOP_AFTER_FIRST[] = _T("Settings/StopAfterFirst");
const TCHAR OPT_CMP_QUICK_LIMIT[] = _T("Settings/QuickMethodLimit");
const TCHAR OPT_CMP_WALK_UNIQUE_DIRS[] = _T("Settings/ScanUnpairedDir");
const TCHAR OPT_CMP_IGNORE_REPARSE_POINTS[] = _T("Settings/IgnoreReparsePoints");

// Image Compare options
const TCHAR OPT_CMP_IMG_FILEPATTERNS[] = _T("Settings/ImageFilePatterns");
const TCHAR OPT_CMP_IMG_SHOWDIFFERENCES[] = _T("Settings/ImageShowDifferences");
const TCHAR OPT_CMP_IMG_OVERLAYMOVE[] = _T("Settings/ImageOverlayMode");
const TCHAR OPT_CMP_IMG_ZOOM[] = _T("Settings/ImageZoom");
const TCHAR OPT_CMP_IMG_USEBACKCOLOR[] = _T("Settings/ImageUseBackColor");
const TCHAR OPT_CMP_IMG_BACKCOLOR[] = _T("Settings/ImageBackColor");
const TCHAR OPT_CMP_IMG_DIFFBLOCKSIZE[] = _T("Settings/ImageDiffBlockSize");
const TCHAR OPT_CMP_IMG_THRESHOLD[] = _T("Settings/ImageColorDistanceThreshold");

// Image Binary options
const TCHAR OPT_CMP_BIN_FILEPATTERNS[] = _T("Settings/BinaryFilePatterns");

// Multidoc enable/disable per document type
const TCHAR OPT_MULTIDOC_DIRDOCS[] = _T("Settings/MultiDirDocs");
const TCHAR OPT_MULTIDOC_MERGEDOCS[] = _T("Settings/MultiMergeDocs");

/// Are regular expression linefilters enabled?
const TCHAR OPT_LINEFILTER_ENABLED[] = _T("Settings/IgnoreRegExp");
/// Currently selected filefilter
const TCHAR OPT_FILEFILTER_CURRENT[] = _T("Settings/FileFilterCurrent");
const TCHAR OPT_FILTER_USERPATH[] = _T("Settings/UserFilterPath");
const TCHAR OPT_FILEFILTER_SHARED[] = _T("Settings/Filters/Shared");

// Version control
const TCHAR OPT_VCS_SYSTEM[] = _T("Settings/VersionSystem");
const TCHAR OPT_VSS_PATH[] = _T("Settings/VssPath");
const TCHAR OPT_VSS_DATABASE[] = _T("Settings/VssDatabase");
const TCHAR OPT_VSS_PROJECT[] = _T("Settings/VssProject");
const TCHAR OPT_VSS_USER[] = _T("Settings/VssUser");

// Archive support
const TCHAR OPT_ARCHIVE_ENABLE[] = _T("Merge7z/Enable");
const TCHAR OPT_ARCHIVE_PROBETYPE[] = _T("Merge7z/ProbeSignature");

// Plugins
const TCHAR OPT_PLUGINS_ENABLED[] = _T("Settings/PluginsEnabled");

// Startup options
const TCHAR OPT_SHOW_SELECT_FILES_AT_STARTUP[] = _T("Settings/ShowFileDialog");

// MDI Tab Bar
const TCHAR OPT_TABBAR_AUTO_MAXWIDTH[] = _T("Settings/TabBarAutoMaxWidth");

// Font options
const TCHAR OPT_FONT_FILECMP[] = _T("Font/");
const TCHAR OPT_FONT_DIRCMP[] = _T("FontDirCompare/");

const TCHAR OPT_FONT_USECUSTOM[] = _T("Specified");
const TCHAR OPT_FONT_HEIGHT[] = _T("Height");
const TCHAR OPT_FONT_WIDTH[] = _T("Width");
const TCHAR OPT_FONT_ESCAPEMENT[] = _T("Escapement");
const TCHAR OPT_FONT_ORIENTATION[] = _T("Orientation");
const TCHAR OPT_FONT_WEIGHT[] = _T("Weight");
const TCHAR OPT_FONT_ITALIC[] = _T("Italic");
const TCHAR OPT_FONT_UNDERLINE[] = _T("Underline");
const TCHAR OPT_FONT_STRIKEOUT[] = _T("StrikeOut");
const TCHAR OPT_FONT_CHARSET[] = _T("CharSet");
const TCHAR OPT_FONT_OUTPRECISION[] = _T("OutPrecision");
const TCHAR OPT_FONT_CLIPPRECISION[] = _T("ClipPrecision");
const TCHAR OPT_FONT_QUALITY[] = _T("Quality");
const TCHAR OPT_FONT_PITCHANDFAMILY[] = _T("PitchAndFamily");
const TCHAR OPT_FONT_FACENAME[] = _T("FaceName");
