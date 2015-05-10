/** 
 * @file  OptionsDef.h
 *
 * @brief Constants for option-names
 */
#pragma once

#include "UnicodeString.h"

// User's language
extern const String OPT_SELECTED_LANGUAGE;

// View-menu
extern const String OPT_SHOW_UNIQUE_LEFT;
extern const String OPT_SHOW_UNIQUE_RIGHT;
extern const String OPT_SHOW_DIFFERENT;
extern const String OPT_SHOW_IDENTICAL;
extern const String OPT_SHOW_BINARIES;
extern const String OPT_SHOW_SKIPPED;
extern const String OPT_TREE_MODE;

// Show/hide toolbar/statusbar/tabbar
extern const String OPT_SHOW_TOOLBAR;
extern const String OPT_SHOW_STATUSBAR;
extern const String OPT_SHOW_TABBAR;
extern const String OPT_TOOLBAR_SIZE;
extern const String OPT_RESIZE_PANES;

extern const String OPT_SYNTAX_HIGHLIGHT;
extern const String OPT_VIEW_WHITESPACE;
extern const String OPT_CONNECT_MOVED_BLOCKS;
extern const String OPT_SCROLL_TO_FIRST;

// Difference (in-line) highlight
extern const String OPT_WORDDIFF_HIGHLIGHT;
extern const String OPT_BREAK_ON_WORDS;
extern const String OPT_BREAK_TYPE;
extern const String OPT_BREAK_SEPARATORS;

// Backup options
extern const String OPT_BACKUP_FOLDERCMP;
extern const String OPT_BACKUP_FILECMP;
extern const String OPT_BACKUP_LOCATION;
extern const String OPT_BACKUP_GLOBALFOLDER;
extern const String OPT_BACKUP_ADD_BAK;
extern const String OPT_BACKUP_ADD_TIME;

extern const String OPT_DIRVIEW_SORT_COLUMN;
extern const String OPT_DIRVIEW_SORT_COLUMN3;
extern const String OPT_DIRVIEW_SORT_ASCENDING;
extern const String OPT_DIRVIEW_EXPAND_SUBDIRS;

// File compare
extern const String OPT_AUTOMATIC_RESCAN;
extern const String OPT_ALLOW_MIXED_EOL;
extern const String OPT_TAB_SIZE;
extern const String OPT_TAB_TYPE;
extern const String OPT_WORDWRAP;
extern const String OPT_VIEW_LINENUMBERS;
extern const String OPT_VIEW_FILEMARGIN;
extern const String OPT_DIFF_CONTEXT;

extern const String OPT_EXT_EDITOR_CMD;
extern const String OPT_USE_RECYCLE_BIN;
extern const String OPT_SINGLE_INSTANCE;
extern const String OPT_MERGE_MODE;
extern const String OPT_CLOSE_WITH_ESC;
extern const String OPT_CLOSE_WITH_OK;
extern const String OPT_VERIFY_OPEN_PATHS;
extern const String OPT_AUTO_COMPLETE_SOURCE;
extern const String OPT_IGNORE_SMALL_FILETIME;
extern const String OPT_ASK_MULTIWINDOW_CLOSE;
extern const String OPT_PRESERVE_FILETIMES;

extern const String OPT_CP_DEFAULT_MODE;
extern const String OPT_CP_DEFAULT_CUSTOM;
extern const String OPT_CP_DETECT;

extern const String OPT_PROJECTS_PATH;
extern const String OPT_USE_SYSTEM_TEMP_PATH;
extern const String OPT_CUSTOM_TEMP_PATH;

extern const String OPT_SPLIT_HORIZONTALLY;

// Color options
// The difference color
extern const String OPT_CLR_DIFF;
// The selected difference color
extern const String OPT_CLR_SELECTED_DIFF;
// The difference deleted color
extern const String OPT_CLR_DIFF_DELETED;
// The selected difference deleted color
extern const String OPT_CLR_SELECTED_DIFF_DELETED;
// The difference text color
extern const String OPT_CLR_DIFF_TEXT;
// The selected difference text color
extern const String OPT_CLR_SELECTED_DIFF_TEXT;
// The ignored lines color
extern const String OPT_CLR_TRIVIAL_DIFF;
// The ignored and deleted lines color
extern const String OPT_CLR_TRIVIAL_DIFF_DELETED;
// The ignored text color
extern const String OPT_CLR_TRIVIAL_DIFF_TEXT;
// The moved block color
extern const String OPT_CLR_MOVEDBLOCK;
// The moved block deleted lines color
extern const String OPT_CLR_MOVEDBLOCK_DELETED;
// The moved block text color
extern const String OPT_CLR_MOVEDBLOCK_TEXT;
// The selected moved block color
extern const String OPT_CLR_SELECTED_MOVEDBLOCK;
// The selected moved block deleted lines
extern const String OPT_CLR_SELECTED_MOVEDBLOCK_DELETED;
// The selected moved block text color
extern const String OPT_CLR_SELECTED_MOVEDBLOCK_TEXT;
// The SNP block color
extern const String OPT_CLR_SNP;
// The SNP block deleted lines color
extern const String OPT_CLR_SNP_DELETED;
// The SNP block text color
extern const String OPT_CLR_SNP_TEXT;
// The selected SNP block color
extern const String OPT_CLR_SELECTED_SNP;
// The selected SNP block deleted lines
extern const String OPT_CLR_SELECTED_SNP_DELETED;
// The selected SNP block text color
extern const String OPT_CLR_SELECTED_SNP_TEXT;
// The word difference color
extern const String OPT_CLR_WORDDIFF;
// The word difference deleted color
extern const String OPT_CLR_WORDDIFF_DELETED;
// The word difference text color
extern const String OPT_CLR_WORDDIFF_TEXT;
// The selected word difference color
extern const String OPT_CLR_SELECTED_WORDDIFF;
// The word difference deleted color
extern const String OPT_CLR_SELECTED_WORDDIFF_DELETED;
// The selected word difference text color
extern const String OPT_CLR_SELECTED_WORDDIFF_TEXT;
// Whether to use default (theme) text colors
extern const String OPT_CLR_DEFAULT_TEXT_COLORING;

// Compare options
extern const String OPT_CMP_IGNORE_WHITESPACE;
extern const String OPT_CMP_IGNORE_BLANKLINES;
extern const String OPT_CMP_FILTER_COMMENTLINES;
extern const String OPT_CMP_IGNORE_CASE;
extern const String OPT_CMP_IGNORE_EOL;
extern const String OPT_CMP_METHOD;
extern const String OPT_CMP_MOVED_BLOCKS;
extern const String OPT_CMP_MATCH_SIMILAR_LINES;
extern const String OPT_CMP_STOP_AFTER_FIRST;
extern const String OPT_CMP_QUICK_LIMIT;
extern const String OPT_CMP_WALK_UNIQUE_DIRS;
extern const String OPT_CMP_IGNORE_REPARSE_POINTS;

// Image Compare options
extern const String OPT_CMP_IMG_FILEPATTERNS;
extern const String OPT_CMP_IMG_SHOWDIFFERENCES;
extern const String OPT_CMP_IMG_OVERLAYMOVE;
extern const String OPT_CMP_IMG_ZOOM;
extern const String OPT_CMP_IMG_USEBACKCOLOR;
extern const String OPT_CMP_IMG_BACKCOLOR;
extern const String OPT_CMP_IMG_DIFFBLOCKSIZE;
extern const String OPT_CMP_IMG_THRESHOLD;

// Image Binary options
extern const String OPT_CMP_BIN_FILEPATTERNS;

/// Are regular expression linefilters enabled?
extern const String OPT_LINEFILTER_ENABLED;
/// Currently selected filefilter
extern const String OPT_FILEFILTER_CURRENT;
extern const String OPT_FILTER_USERPATH;
extern const String OPT_FILEFILTER_SHARED;

// Version control
extern const String OPT_VCS_SYSTEM;
extern const String OPT_VSS_PATH;
extern const String OPT_VSS_DATABASE;
extern const String OPT_VSS_PROJECT;
extern const String OPT_VSS_USER;

// Archive support
extern const String OPT_ARCHIVE_ENABLE;
extern const String OPT_ARCHIVE_PROBETYPE;

// Plugins
extern const String OPT_PLUGINS_ENABLED;

// Startup options
extern const String OPT_SHOW_SELECT_FILES_AT_STARTUP;

// MDI Tab Bar
extern const String OPT_TABBAR_AUTO_MAXWIDTH;

// Font options
extern const String OPT_FONT_FILECMP;
extern const String OPT_FONT_DIRCMP;

extern const String OPT_FONT_USECUSTOM;
extern const String OPT_FONT_HEIGHT;
extern const String OPT_FONT_WIDTH;
extern const String OPT_FONT_ESCAPEMENT;
extern const String OPT_FONT_ORIENTATION;
extern const String OPT_FONT_WEIGHT;
extern const String OPT_FONT_ITALIC;
extern const String OPT_FONT_UNDERLINE;
extern const String OPT_FONT_STRIKEOUT;
extern const String OPT_FONT_CHARSET;
extern const String OPT_FONT_OUTPRECISION;
extern const String OPT_FONT_CLIPPRECISION;
extern const String OPT_FONT_QUALITY;
extern const String OPT_FONT_PITCHANDFAMILY;
extern const String OPT_FONT_FACENAME;
