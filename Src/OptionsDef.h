/** 
 * @file  OptionsDef.h
 *
 * @brief Constants for option-names
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef _OPTIONS_DEF_H_
#define _OPTIONS_DEF_H_

// View-menu
const TCHAR OPT_SHOW_UNIQUE_LEFT[] = _T("Settings/ShowUniqueLeft");
const TCHAR OPT_SHOW_UNIQUE_RIGHT[] = _T("Settings/ShowUniqueRight");
const TCHAR OPT_SHOW_DIFFERENT[] = _T("Settings/ShowDifferent");
const TCHAR OPT_SHOW_IDENTICAL[] = _T("Settings/ShowIdentical");
const TCHAR OPT_SHOW_BINARIES[] = _T("Settings/ShowBinaries");
const TCHAR OPT_SHOW_SKIPPED[] = _T("Settings/ShowSkipped");

// Show/hide toolbar/statusbar
const TCHAR OPT_SHOW_TOOLBAR[] = _T("Settings/ShowToolbar");
const TCHAR OPT_SHOW_STATUSBAR[] = _T("Settings/ShowStatusbar");

const TCHAR OPT_SYNTAX_HIGHLIGHT[] = _T("Settings/HiliteSyntax");
const TCHAR OPT_WORDDIFF_HIGHLIGHT[] = _T("Settings/HiliteWordDiff");
const TCHAR OPT_DISABLE_SPLASH[] = _T("Settings/DisableSplash");
const TCHAR OPT_CREATE_BACKUPS[] = _T("Settings/BackupFile");
const TCHAR OPT_VIEW_WHITESPACE[] =  _T("Settings/ViewWhitespace");
const TCHAR OPT_CONNECT_MOVED_BLOCKS[] = _T("Settings/ConnectMovedBlocks");
const TCHAR OPT_SCROLL_TO_FIRST[] =  _T("Settings/ScrollToFirst");

const TCHAR OPT_AUTOMATIC_RESCAN[] = _T("Settings/AutomaticRescan");
const TCHAR OPT_ALLOW_MIXED_EOL[] = _T("Settings/AllowMixedEOL");
const TCHAR OPT_TAB_SIZE[] = _T("Settings/TabSize");
const TCHAR OPT_TAB_TYPE[] = _T("Settings/TabType");
const TCHAR OPT_UNREC_APPLYSYNTAX[] = _T("Settings/ApplyUnrecSyntax");

const TCHAR OPT_EXT_EDITOR_CMD[] = _T("Settings/ExternalEditor");
const TCHAR OPT_USE_RECYCLE_BIN[] = _T("Settings/UseRecycleBin");
const TCHAR OPT_AUTOCLOSE_CMPPANE[] = _T("Settings/AutoCloseCmpPane");
const TCHAR OPT_SINGLE_INSTANCE[] = _T("Settings/SingleInstance");
const TCHAR OPT_MERGE_MODE[] = _T("Settings/MergingMode");
const TCHAR OPT_CLOSE_WITH_ESC[] = _T("Settings/CloseWithEsc");
const TCHAR OPT_LOGGING[] = _T("Settings/Logging");
const TCHAR OPT_VERIFY_OPEN_PATHS[] = _T("Settings/VerifyOpenPaths");

const TCHAR OPT_CP_DEFAULT_MODE[] = _T("Settings/CodepageDefaultMode");
const TCHAR OPT_CP_DEFAULT_CUSTOM[] = _T("Settings/CodepageDefaultCustomValue");
const TCHAR OPT_CP_DETECT[] = _T("Settings/CodepageDetection");

const TCHAR OPT_PROJECTS_PATH[] =  _T("Settings/ProjectsPath");

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
// The word difference color
const TCHAR OPT_CLR_WORDDIFF[] = _T("Settings/WordDifferenceColor");
// The selected word difference color
const TCHAR OPT_CLR_SELECTED_WORDDIFF[] = _T("Settings/SelectedWordDifferenceColor");
// The word difference text color
const TCHAR OPT_CLR_WORDDIFF_TEXT[] = _T("Settings/WordDifferenceTextColor");
// The selected word difference text color
const TCHAR OPT_CLR_SELECTED_WORDDIFF_TEXT[] = _T("Settings/SelectedWordDifferenceTextColor");

// Compare options
const TCHAR OPT_CMP_IGNORE_WHITESPACE[] = _T("Settings/IgnoreSpace");
const TCHAR OPT_CMP_IGNORE_BLANKLINES[] = _T("Settings/IgnoreBlankLines");
const TCHAR OPT_CMP_IGNORE_CASE[] = _T("Settings/IgnoreCase");
const TCHAR OPT_CMP_EOL_SENSITIVE[] = _T("Settings/EolSensitive");
const TCHAR OPT_CMP_METHOD[] = _T("Settings/CompMethod");
const TCHAR OPT_CMP_MOVED_BLOCKS[] = _T("Settings/MovedBlocks");

/// Are regular expression linefilters enabled?
const TCHAR OPT_LINEFILTER_ENABLED[] = _T("Settings/IgnoreRegExp");
///Regular expression linefilters
const TCHAR OPT_LINEFILTER_REGEXP[] = _T("Settings/RegExps");
/// Currently selected filefilter
const TCHAR OPT_FILEFILTER_CURRENT[] = _T("Settings/FileFilterCurrent");

// Fonts
const TCHAR OPT_FONT_FILECMP_USECUSTOM[] = _T("Font/Specified");
const TCHAR OPT_FONT_DIRCMP_USECUSTOM[] = _T("FontDirCompare/Specified");

// Version control
const TCHAR OPT_VCS_SYSTEM[] = _T("Settings/VersionSystem");
const TCHAR OPT_VSS_PATH[] = _T("Settings/VssPath");

#endif // _OPTIONS_DEF_H_
