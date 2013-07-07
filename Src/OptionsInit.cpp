/**
 * @file  OptionsInit.cpp
 *
 * @brief Options initialisation.
 */
// ID line follows -- this is updated by SVN
// $Id: OptionsInit.cpp 6910 2009-07-12 09:06:54Z kimmov $

#include "stdafx.h"
#include <vector>
#include "Merge.h"
#include "MainFrm.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsDiffOptions.h"
#include "OptionsDiffColors.h"
#include "OptionsFont.h"
#include "DiffWrapper.h" // CMP_CONTENT
#include "unicoder.h"
#include "SourceControl.h"

// Functions to copy values set by installer from HKLM to HKCU.
static void CopyHKLMValues();
static bool OpenHKLM(HKEY *key, LPCTSTR relpath = NULL);
static bool OpenHKCU(HKEY *key, LPCTSTR relpath = NULL);
static bool IsFirstRun(HKEY key);
static void CopyFromLMtoCU(HKEY lmKey, HKEY cuKey, LPCTSTR valname);
static void ResetFirstRun(HKEY key);

/**
 * @brief Initialise options and set default values.
 *
 * @note Remember default values are what users see first time
 * using WinMerge and many users never change them. So pick
 * default values carefully!
 */
void CMergeApp::OptionsInit()
{
	// Copy some values from HKLM to HKCU
	CopyHKLMValues();

	m_pOptions->SetRegRootKey(_T("Thingamahoochie\\WinMerge\\"));

	LANGID LangId = GetUserDefaultLangID();
	if (PRIMARYLANGID(LangId) == LANG_JAPANESE)
	{
		// Default language to Japanese unless installer set it otherwise
		m_pOptions->InitOption(OPT_SELECTED_LANGUAGE, 0x411);
	}
	else
	{
		// Default language to English unless installer set it otherwise
		m_pOptions->InitOption(OPT_SELECTED_LANGUAGE, 0x409);
	}

	// Initialise options (name, default value)
	m_pOptions->InitOption(OPT_SHOW_UNIQUE_LEFT, true);
	m_pOptions->InitOption(OPT_SHOW_UNIQUE_RIGHT, true);
	m_pOptions->InitOption(OPT_SHOW_DIFFERENT, true);
	m_pOptions->InitOption(OPT_SHOW_IDENTICAL, true);
	m_pOptions->InitOption(OPT_SHOW_BINARIES, true);
	m_pOptions->InitOption(OPT_SHOW_SKIPPED, false);

	m_pOptions->InitOption(OPT_SHOW_TOOLBAR, true);
	m_pOptions->InitOption(OPT_SHOW_STATUSBAR, true);
	m_pOptions->InitOption(OPT_SHOW_TABBAR, true);
	m_pOptions->InitOption(OPT_TOOLBAR_SIZE, 0);
	m_pOptions->InitOption(OPT_RESIZE_PANES, false);

	m_pOptions->InitOption(OPT_SYNTAX_HIGHLIGHT, true);
	m_pOptions->InitOption(OPT_WORDWRAP, false);
	m_pOptions->InitOption(OPT_VIEW_LINENUMBERS, false);
	m_pOptions->InitOption(OPT_DISABLE_SPLASH, false);
	m_pOptions->InitOption(OPT_VIEW_WHITESPACE, false);
	m_pOptions->InitOption(OPT_CONNECT_MOVED_BLOCKS, 0);
	m_pOptions->InitOption(OPT_SCROLL_TO_FIRST, false);
	m_pOptions->InitOption(OPT_VERIFY_OPEN_PATHS, true);
	m_pOptions->InitOption(OPT_AUTO_COMPLETE_SOURCE, (int)1);
	m_pOptions->InitOption(OPT_VIEW_FILEMARGIN, false);
	m_pOptions->InitOption(OPT_DIFF_CONTEXT, (int)-1);
	m_pOptions->InitOption(OPT_SPLIT_HORIZONTALLY, false);

	m_pOptions->InitOption(OPT_WORDDIFF_HIGHLIGHT, true);
	m_pOptions->InitOption(OPT_BREAK_SEPARATORS, _T(".,:;?[](){}<>`'!\"#$%&^~\\|@+-*/"));

	m_pOptions->InitOption(OPT_BACKUP_FOLDERCMP, false);
	m_pOptions->InitOption(OPT_BACKUP_FILECMP, true);
	m_pOptions->InitOption(OPT_BACKUP_LOCATION, (int)0);
	m_pOptions->InitOption(OPT_BACKUP_GLOBALFOLDER, _T(""));
	m_pOptions->InitOption(OPT_BACKUP_ADD_BAK, true);
	m_pOptions->InitOption(OPT_BACKUP_ADD_TIME, false);

	m_pOptions->InitOption(OPT_DIRVIEW_SORT_COLUMN, (int)-1);
	m_pOptions->InitOption(OPT_DIRVIEW_SORT_COLUMN3, (int)-1);
	m_pOptions->InitOption(OPT_DIRVIEW_SORT_ASCENDING, true);
	m_pOptions->InitOption(OPT_DIRVIEW_ENABLE_SHELL_CONTEXT_MENU, false);
	m_pOptions->InitOption(OPT_SHOW_SELECT_FILES_AT_STARTUP, false);
	m_pOptions->InitOption(OPT_DIRVIEW_EXPAND_SUBDIRS, false);

	m_pOptions->InitOption(OPT_AUTOMATIC_RESCAN, false);
	m_pOptions->InitOption(OPT_ALLOW_MIXED_EOL, false);
	m_pOptions->InitOption(OPT_TAB_SIZE, (int)4);
	m_pOptions->InitOption(OPT_TAB_TYPE, (int)0);	// 0 means tabs inserted

	m_pOptions->InitOption(OPT_EXT_EDITOR_CMD, GetDefaultEditor());
	m_pOptions->InitOption(OPT_USE_RECYCLE_BIN, true);
	m_pOptions->InitOption(OPT_SINGLE_INSTANCE, false);
	m_pOptions->InitOption(OPT_MERGE_MODE, false);
	// OPT_WORDDIFF_HIGHLIGHT is initialized above
	m_pOptions->InitOption(OPT_BREAK_ON_WORDS, false);
	m_pOptions->InitOption(OPT_BREAK_TYPE, 0);

	m_pOptions->InitOption(OPT_CLOSE_WITH_ESC, true);
	m_pOptions->InitOption(OPT_IGNORE_SMALL_FILETIME, false);
	m_pOptions->InitOption(OPT_ASK_MULTIWINDOW_CLOSE, false);
	m_pOptions->InitOption(OPT_PRESERVE_FILETIMES, false);
	m_pOptions->InitOption(OPT_TREE_MODE, false);

	m_pOptions->InitOption(OPT_CMP_METHOD, (int)CMP_CONTENT);
	m_pOptions->InitOption(OPT_CMP_MOVED_BLOCKS, false);
	m_pOptions->InitOption(OPT_CMP_MATCH_SIMILAR_LINES, false);
	m_pOptions->InitOption(OPT_CMP_STOP_AFTER_FIRST, false);
	m_pOptions->InitOption(OPT_CMP_QUICK_LIMIT, 4 * 1024 * 1024); // 4 Megs
	m_pOptions->InitOption(OPT_CMP_WALK_UNIQUE_DIRS, false);
	m_pOptions->InitOption(OPT_CMP_IGNORE_REPARSE_POINTS, false);

	m_pOptions->InitOption(OPT_PROJECTS_PATH, _T(""));
	m_pOptions->InitOption(OPT_USE_SYSTEM_TEMP_PATH, true);
	m_pOptions->InitOption(OPT_CUSTOM_TEMP_PATH, _T(""));

	m_pOptions->InitOption(OPT_MULTIDOC_DIRDOCS, true);
	m_pOptions->InitOption(OPT_MULTIDOC_MERGEDOCS, true);

	m_pOptions->InitOption(OPT_LINEFILTER_ENABLED, false);
	m_pOptions->InitOption(OPT_FILEFILTER_CURRENT, _T("*.*"));
	// CMainFrame initializes this when it is empty.
	m_pOptions->InitOption(OPT_FILTER_USERPATH, GetDefaultFilterUserPath());
	m_pOptions->InitOption(OPT_FILEFILTER_SHARED, false);

	m_pOptions->InitOption(OPT_CP_DEFAULT_MODE, (int)0);
	m_pOptions->InitOption(OPT_CP_DEFAULT_CUSTOM, (int)GetACP());

	if (PRIMARYLANGID(LangId) == LANG_JAPANESE)
		m_pOptions->InitOption(OPT_CP_DETECT, (int)(50932 << 16) | 3);
	else if (LangId == MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED))
		m_pOptions->InitOption(OPT_CP_DETECT, (int)(50936 << 16) | 3);
	else if (PRIMARYLANGID(LangId) == LANG_KOREAN)
		m_pOptions->InitOption(OPT_CP_DETECT, (int)(50949 << 16) | 3);
	else if (LangId == MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL))
		m_pOptions->InitOption(OPT_CP_DETECT, (int)(50950 << 16) | 3);
	else
		m_pOptions->InitOption(OPT_CP_DETECT, (int)(50001 << 16) | 3);

	m_pOptions->InitOption(OPT_VCS_SYSTEM, SourceControl::VCS_NONE);
	m_pOptions->InitOption(OPT_VSS_PATH, _T(""));
	m_pOptions->InitOption(OPT_VSS_DATABASE, _T(""));
	m_pOptions->InitOption(OPT_VSS_PROJECT, _T(""));
	m_pOptions->InitOption(OPT_VSS_USER, _T(""));

	m_pOptions->InitOption(OPT_ARCHIVE_ENABLE, 1); // Enable by default
	m_pOptions->InitOption(OPT_ARCHIVE_PROBETYPE, false);

	m_pOptions->InitOption(OPT_PLUGINS_ENABLED, true);

	Options::DiffOptions::SetDefaults();
	Options::DiffColors::SetDefaults();
	Options::Font::SetDefaults();
}

/**
 * @brief Copy some HKLM values to HKCU.
 * The installer sets HKLM values for "all users". This function copies
 * few of those values for "user" values. E.g. enabling ShellExtension
 * initially for user is done by this function.
 */
static void CopyHKLMValues()
{
	HKEY LMKey;
	HKEY CUKey;
	if (OpenHKLM(&LMKey))
	{
		if (OpenHKCU(&CUKey))
		{
			CopyFromLMtoCU(LMKey, CUKey, _T("ContextMenuEnabled"));
			CopyFromLMtoCU(LMKey, CUKey, _T("Executable"));
			RegCloseKey(CUKey);
		}
		RegCloseKey(LMKey);
	}
	if (OpenHKLM(&LMKey, _T("Locale")))
	{
		if (OpenHKCU(&CUKey, _T("Locale")))
		{
			CopyFromLMtoCU(LMKey, CUKey, _T("LanguageId"));
			RegCloseKey(CUKey);
		}
		RegCloseKey(LMKey);
	}
}

/**
 * @brief Open HKLM registry key.
 * @param [out] key Pointer to open HKLM key.
 * @param [in] relpath Relative registry path (to WinMerge reg path) to open, or NULL.
 * @return true if opening succeeded.
 */
static bool OpenHKLM(HKEY *key, LPCTSTR relpath)
{
	TCHAR valuename[256] = _T("Software\\Thingamahoochie\\WinMerge\\");
	if (relpath)
		lstrcat(valuename, relpath);
	LONG retval = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			valuename, 0, KEY_READ, key);
	if (retval == ERROR_SUCCESS)
	{
		return true;
	}
	return false;
}

/**
 * @brief Open HKCU registry key.
 * Opens the HKCU key for WinMerge. If the key does not exist, creates one.
 * @param [out] key Pointer to open HKCU key.
 * @param [in] relpath Relative registry path (to WinMerge reg path) to open, or NULL.
 * @return true if opening succeeded.
 */
static bool OpenHKCU(HKEY *key, LPCTSTR relpath)
{
	TCHAR valuename[256] = _T("Software\\Thingamahoochie\\WinMerge\\");
	if (relpath)
		lstrcat(valuename, relpath);
	LONG retval = RegOpenKeyEx(HKEY_CURRENT_USER,
			valuename, 0, KEY_ALL_ACCESS, key);
	if (retval == ERROR_SUCCESS)
	{
		return true;
	}
	else if (retval == ERROR_FILE_NOT_FOUND)
	{
		retval = RegCreateKeyEx(HKEY_CURRENT_USER,
			valuename, 0, NULL, 0, KEY_ALL_ACCESS, NULL, key, NULL);
		if (retval == ERROR_SUCCESS)
			return true;
	}
	return false;
}

/**
 * @brief Copy value from HKLM to HKCU.
 * @param [in] lmKey HKLM key from where to copy.
 * @param [in] cuKey HKCU key to where to copy.
 * @param [in] valname Name of the value to copy.
 */
static void CopyFromLMtoCU(HKEY lmKey, HKEY cuKey, LPCTSTR valname)
{
	DWORD len = 0;
	LONG retval = RegQueryValueEx(cuKey, valname, 0, NULL, NULL, &len);
	if (retval == ERROR_FILE_NOT_FOUND)
	{
		retval = RegQueryValueEx(lmKey, valname, 0, NULL, NULL, &len);
		if (retval == ERROR_SUCCESS)
		{
			DWORD type = 0;
			std::vector<BYTE> buf(len);
			retval = RegQueryValueEx(lmKey, valname, 0, &type, &buf[0], &len);
			if (retval == ERROR_SUCCESS)
			{
				retval = RegSetValueEx(cuKey, valname , 0, type,
					&buf[0], len);
			}
		}
	}
}
