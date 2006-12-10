/**
 * @file  OptionsInit.cpp
 *
 * @brief Options initialisation.
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "Merge.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "DiffWrapper.h" // CMP_CONTENT
#include "MainFrm.h"

/**
 * @brief Initialise options and set default values.
 *
 * @note Remember default values are what users see first time
 * using WinMerge and many users never change them. So pick
 * default values carefully!
 */
void CMainFrame::OptionsInit()
{
	m_options.SetRegRootKey(_T("Thingamahoochie\\WinMerge\\"));

	// Initialise options (name, default value)
	m_options.InitOption(OPT_SHOW_UNIQUE_LEFT, true);
	m_options.InitOption(OPT_SHOW_UNIQUE_RIGHT, true);
	m_options.InitOption(OPT_SHOW_DIFFERENT, true);
	m_options.InitOption(OPT_SHOW_IDENTICAL, true);
	m_options.InitOption(OPT_SHOW_BINARIES, true);
	m_options.InitOption(OPT_SHOW_SKIPPED, false);

	m_options.InitOption(OPT_SHOW_TOOLBAR, true);
	m_options.InitOption(OPT_SHOW_STATUSBAR, true);

	m_options.InitOption(OPT_SYNTAX_HIGHLIGHT, true);
	m_options.InitOption(OPT_WORDDIFF_HIGHLIGHT, true);
	m_options.InitOption(OPT_WORDWRAP, false);
	m_options.InitOption(OPT_VIEW_LINENUMBERS, false);
	m_options.InitOption(OPT_DISABLE_SPLASH, false);
	m_options.InitOption(OPT_CREATE_BACKUPS, true);
	m_options.InitOption(OPT_VIEW_WHITESPACE, false);
	m_options.InitOption(OPT_CONNECT_MOVED_BLOCKS, 0);
	m_options.InitOption(OPT_SCROLL_TO_FIRST, false);
	m_options.InitOption(OPT_VERIFY_OPEN_PATHS, true);
	m_options.InitOption(OPT_AUTO_COMPLETE_SOURCE, (int)0);

	m_options.InitOption(OPT_DIRVIEW_SORT_COLUMN, (int)-1);
	m_options.InitOption(OPT_DIRVIEW_SORT_ASCENDING, true);

	m_options.InitOption(OPT_AUTOMATIC_RESCAN, false);
	m_options.InitOption(OPT_ALLOW_MIXED_EOL, false);
	m_options.InitOption(OPT_TAB_SIZE, (int)4);
	m_options.InitOption(OPT_TAB_TYPE, (int)0);	// 0 means tabs inserted

	m_options.InitOption(OPT_EXT_EDITOR_CMD, GetDefaultEditor());
	m_options.InitOption(OPT_USE_RECYCLE_BIN, true);
	m_options.InitOption(OPT_SINGLE_INSTANCE, false);
	m_options.InitOption(OPT_MERGE_MODE, false);
	// OPT_WORDDIFF_HIGHLIGHT is initialized above
	m_options.InitOption(OPT_BREAK_ON_WORDS, true);
	m_options.InitOption(OPT_BREAK_TYPE, 0);

	m_options.InitOption(OPT_CLOSE_WITH_ESC, true);
	m_options.InitOption(OPT_LOGGING, 0);
	m_options.InitOption(OPT_IGNORE_SMALL_FILETIME, false);

	m_options.InitOption(OPT_CMP_IGNORE_WHITESPACE, (int)0);
	m_options.InitOption(OPT_CMP_IGNORE_BLANKLINES, false);
	m_options.InitOption(OPT_CMP_FILTER_COMMENTLINES, false);
	m_options.InitOption(OPT_CMP_IGNORE_CASE, false);
	m_options.InitOption(OPT_CMP_IGNORE_EOL, false);
	m_options.InitOption(OPT_CMP_METHOD, (int)CMP_CONTENT);
	m_options.InitOption(OPT_CMP_MOVED_BLOCKS, false);
	m_options.InitOption(OPT_CMP_MATCH_SIMILAR_LINES, false);
	m_options.InitOption(OPT_CMP_STOP_AFTER_FIRST, false);
	m_options.InitOption(OPT_CMP_QUICK_LIMIT, 4 * 1024 * 1024); // 4 Megs

	m_options.InitOption(OPT_CLR_DIFF, (int)RGB(239,203,5));
	m_options.InitOption(OPT_CLR_SELECTED_DIFF, (int)RGB(239,119,116));
	m_options.InitOption(OPT_CLR_DIFF_DELETED, (int)RGB(192, 192, 192));
	m_options.InitOption(OPT_CLR_SELECTED_DIFF_DELETED, (int)RGB(240, 192, 192));
	m_options.InitOption(OPT_CLR_DIFF_TEXT, (int)RGB(0,0,0));
	m_options.InitOption(OPT_CLR_SELECTED_DIFF_TEXT, (int)RGB(0,0,0));
	m_options.InitOption(OPT_CLR_TRIVIAL_DIFF, (int)RGB(251,242,191));
	m_options.InitOption(OPT_CLR_TRIVIAL_DIFF_DELETED, (int)RGB(233,233,233));
	m_options.InitOption(OPT_CLR_TRIVIAL_DIFF_TEXT, (int)RGB(0,0,0));
	m_options.InitOption(OPT_CLR_MOVEDBLOCK, (int)RGB(228,155,82));
	m_options.InitOption(OPT_CLR_MOVEDBLOCK_DELETED, (int)RGB(192, 192, 192));
	m_options.InitOption(OPT_CLR_MOVEDBLOCK_TEXT, (int)RGB(0,0,0));
	m_options.InitOption(OPT_CLR_SELECTED_MOVEDBLOCK, (int)RGB(248,112,78));
	m_options.InitOption(OPT_CLR_SELECTED_MOVEDBLOCK_DELETED, (int)RGB(252, 181, 163));
	m_options.InitOption(OPT_CLR_SELECTED_MOVEDBLOCK_TEXT, (int)RGB(0,0,0));
	m_options.InitOption(OPT_CLR_WORDDIFF, (int)RGB(241,226,173));
	m_options.InitOption(OPT_CLR_SELECTED_WORDDIFF, (int)RGB(255,160,160));
	m_options.InitOption(OPT_CLR_WORDDIFF_TEXT, (int)RGB(0,0,0));
	m_options.InitOption(OPT_CLR_SELECTED_WORDDIFF_TEXT, (int)RGB(0,0,0));
	m_options.InitOption(OPT_CLR_DEFAULT_TEXT_COLORING, true);

	m_options.InitOption(OPT_PROJECTS_PATH,_T(""));

	m_options.InitOption(OPT_MULTIDOC_DIRDOCS, false);
	m_options.InitOption(OPT_MULTIDOC_MERGEDOCS, false);

	// Overwrite WinMerge 2.0 default colors with new colors
	if (m_options.GetInt(OPT_CLR_DIFF) == RGB(255,255,92))
		m_options.SaveOption(OPT_CLR_DIFF, (int)RGB(239,203,5));
	if (m_options.GetInt(OPT_CLR_SELECTED_DIFF) == RGB(255,0,92))
		m_options.SaveOption(OPT_CLR_SELECTED_DIFF, (int)RGB(239,119,116));

	m_options.InitOption(OPT_LINEFILTER_ENABLED, false);
	m_options.InitOption(OPT_LINEFILTER_REGEXP, _T(""));
	m_options.InitOption(OPT_FILEFILTER_CURRENT, _T("*.*"));
	m_options.InitOption(OPT_FILTER_USERPATH, GetDefaultFilterUserPath());

	m_options.InitOption(OPT_CP_DEFAULT_MODE, (int)0);
	m_options.InitOption(OPT_CP_DEFAULT_CUSTOM, (int)GetACP());
	m_options.InitOption(OPT_CP_DETECT, false);

	m_options.InitOption(OPT_FONT_FILECMP_USECUSTOM, false);
	m_options.InitOption(OPT_FONT_DIRCMP_USECUSTOM, false);

	m_options.InitOption(OPT_VCS_SYSTEM, VCS_NONE);
	m_options.InitOption(OPT_VSS_PATH, _T(""));

	m_options.InitOption(OPT_ARCHIVE_ENABLE, 1); // Enable by default
	m_options.InitOption(OPT_ARCHIVE_PROBETYPE, false);

	// Read last used filter from registry
	// If filter fails to set, reset to default
	BOOL bFilterSet = theApp.m_globalFileFilter.SetFilter(m_options.GetString(OPT_FILEFILTER_CURRENT));
	if (!bFilterSet)
	{
		CString filter = theApp.m_globalFileFilter.GetFilterNameOrMask();
		m_options.SaveOption(OPT_FILEFILTER_CURRENT, filter);
	}
}
