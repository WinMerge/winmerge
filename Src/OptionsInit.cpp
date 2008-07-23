/**
 * @file  OptionsInit.cpp
 *
 * @brief Options initialisation.
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include <mlang.h>
#include "Merge.h"
#include "MainFrm.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "DiffWrapper.h" // CMP_CONTENT

// Functions to copy values set by installer from HKLM to HKCU.
static void CopyHKLMValues();
static bool OpenHKLM(HKEY *key);
static bool OpenHKCU(HKEY *key);
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

	// Default language to English unless installer set it otherwise
	m_pOptions->InitOption(OPT_SELECTED_LANGUAGE, 0x409);

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
	m_pOptions->InitOption(OPT_WORDDIFF_HIGHLIGHT, true);
	m_pOptions->InitOption(OPT_WORDWRAP, false);
	m_pOptions->InitOption(OPT_VIEW_LINENUMBERS, false);
	m_pOptions->InitOption(OPT_DISABLE_SPLASH, false);
	m_pOptions->InitOption(OPT_VIEW_WHITESPACE, false);
	m_pOptions->InitOption(OPT_CONNECT_MOVED_BLOCKS, 0);
	m_pOptions->InitOption(OPT_SCROLL_TO_FIRST, false);
	m_pOptions->InitOption(OPT_VERIFY_OPEN_PATHS, true);
	m_pOptions->InitOption(OPT_AUTO_COMPLETE_SOURCE, (int)0);
	m_pOptions->InitOption(OPT_VIEW_FILEMARGIN, false);

	m_pOptions->InitOption(OPT_BACKUP_FOLDERCMP, false);
	m_pOptions->InitOption(OPT_BACKUP_FILECMP, true);
	m_pOptions->InitOption(OPT_BACKUP_LOCATION, (int)0);
	m_pOptions->InitOption(OPT_BACKUP_GLOBALFOLDER, _T(""));
	m_pOptions->InitOption(OPT_BACKUP_ADD_BAK, true);
	m_pOptions->InitOption(OPT_BACKUP_ADD_TIME, false);

	m_pOptions->InitOption(OPT_DIRVIEW_SORT_COLUMN, (int)-1);
	m_pOptions->InitOption(OPT_DIRVIEW_SORT_ASCENDING, true);

	m_pOptions->InitOption(OPT_AUTOMATIC_RESCAN, false);
	m_pOptions->InitOption(OPT_ALLOW_MIXED_EOL, false);
	m_pOptions->InitOption(OPT_TAB_SIZE, (int)4);
	m_pOptions->InitOption(OPT_TAB_TYPE, (int)0);	// 0 means tabs inserted

	m_pOptions->InitOption(OPT_EXT_EDITOR_CMD, GetDefaultEditor());
	m_pOptions->InitOption(OPT_USE_RECYCLE_BIN, true);
	m_pOptions->InitOption(OPT_SINGLE_INSTANCE, false);
	m_pOptions->InitOption(OPT_MERGE_MODE, false);
	// OPT_WORDDIFF_HIGHLIGHT is initialized above
	m_pOptions->InitOption(OPT_BREAK_ON_WORDS, true);
	m_pOptions->InitOption(OPT_BREAK_TYPE, 0);

	m_pOptions->InitOption(OPT_CLOSE_WITH_ESC, true);
	m_pOptions->InitOption(OPT_LOGGING, 0);
	m_pOptions->InitOption(OPT_IGNORE_SMALL_FILETIME, false);
	m_pOptions->InitOption(OPT_ASK_MULTIWINDOW_CLOSE, false);
	m_pOptions->InitOption(OPT_PRESERVE_FILETIMES, false);
	m_pOptions->InitOption(OPT_TREE_MODE, true);

	m_pOptions->InitOption(OPT_CMP_IGNORE_WHITESPACE, (int)0);
	m_pOptions->InitOption(OPT_CMP_IGNORE_BLANKLINES, false);
	m_pOptions->InitOption(OPT_CMP_FILTER_COMMENTLINES, false);
	m_pOptions->InitOption(OPT_CMP_IGNORE_CASE, false);
	m_pOptions->InitOption(OPT_CMP_IGNORE_EOL, false);
	m_pOptions->InitOption(OPT_CMP_METHOD, (int)CMP_CONTENT);
	m_pOptions->InitOption(OPT_CMP_MOVED_BLOCKS, false);
	m_pOptions->InitOption(OPT_CMP_MATCH_SIMILAR_LINES, false);
	m_pOptions->InitOption(OPT_CMP_STOP_AFTER_FIRST, false);
	m_pOptions->InitOption(OPT_CMP_QUICK_LIMIT, 4 * 1024 * 1024); // 4 Megs

	m_pOptions->InitOption(OPT_CLR_DIFF, (int)RGB(239,203,5));
	m_pOptions->InitOption(OPT_CLR_SELECTED_DIFF, (int)RGB(239,119,116));
	m_pOptions->InitOption(OPT_CLR_DIFF_DELETED, (int)RGB(192, 192, 192));
	m_pOptions->InitOption(OPT_CLR_SELECTED_DIFF_DELETED, (int)RGB(240, 192, 192));
	m_pOptions->InitOption(OPT_CLR_DIFF_TEXT, (int)RGB(0,0,0));
	m_pOptions->InitOption(OPT_CLR_SELECTED_DIFF_TEXT, (int)RGB(0,0,0));
	m_pOptions->InitOption(OPT_CLR_TRIVIAL_DIFF, (int)RGB(251,242,191));
	m_pOptions->InitOption(OPT_CLR_TRIVIAL_DIFF_DELETED, (int)RGB(233,233,233));
	m_pOptions->InitOption(OPT_CLR_TRIVIAL_DIFF_TEXT, (int)RGB(0,0,0));
	m_pOptions->InitOption(OPT_CLR_MOVEDBLOCK, (int)RGB(228,155,82));
	m_pOptions->InitOption(OPT_CLR_MOVEDBLOCK_DELETED, (int)RGB(192, 192, 192));
	m_pOptions->InitOption(OPT_CLR_MOVEDBLOCK_TEXT, (int)RGB(0,0,0));
	m_pOptions->InitOption(OPT_CLR_SELECTED_MOVEDBLOCK, (int)RGB(248,112,78));
	m_pOptions->InitOption(OPT_CLR_SELECTED_MOVEDBLOCK_DELETED, (int)RGB(252, 181, 163));
	m_pOptions->InitOption(OPT_CLR_SELECTED_MOVEDBLOCK_TEXT, (int)RGB(0,0,0));
	m_pOptions->InitOption(OPT_CLR_WORDDIFF, (int)RGB(241,226,173));
	m_pOptions->InitOption(OPT_CLR_SELECTED_WORDDIFF, (int)RGB(255,160,160));
	m_pOptions->InitOption(OPT_CLR_WORDDIFF_TEXT, (int)RGB(0,0,0));
	m_pOptions->InitOption(OPT_CLR_SELECTED_WORDDIFF_TEXT, (int)RGB(0,0,0));
	m_pOptions->InitOption(OPT_CLR_DEFAULT_TEXT_COLORING, true);

	m_pOptions->InitOption(OPT_PROJECTS_PATH, _T(""));
	m_pOptions->InitOption(OPT_USE_SYSTEM_TEMP_PATH, true);
	m_pOptions->InitOption(OPT_CUSTOM_TEMP_PATH, _T(""));

	m_pOptions->InitOption(OPT_MULTIDOC_DIRDOCS, false);
	m_pOptions->InitOption(OPT_MULTIDOC_MERGEDOCS, false);

	// Overwrite WinMerge 2.0 default colors with new colors
	if (m_pOptions->GetInt(OPT_CLR_DIFF) == RGB(255,255,92))
		m_pOptions->SaveOption(OPT_CLR_DIFF, (int)RGB(239,203,5));
	if (m_pOptions->GetInt(OPT_CLR_SELECTED_DIFF) == RGB(255,0,92))
		m_pOptions->SaveOption(OPT_CLR_SELECTED_DIFF, (int)RGB(239,119,116));

	m_pOptions->InitOption(OPT_LINEFILTER_ENABLED, false);
	m_pOptions->InitOption(OPT_FILEFILTER_CURRENT, _T("*.*"));
	// CMainFrame initializes this when it is empty.
	m_pOptions->InitOption(OPT_FILTER_USERPATH, GetDefaultFilterUserPath());
	m_pOptions->InitOption(OPT_FILEFILTER_SHARED, false);

	m_pOptions->InitOption(OPT_CP_DEFAULT_MODE, (int)0);
	m_pOptions->InitOption(OPT_CP_DEFAULT_CUSTOM, (int)GetACP());
	m_pOptions->InitOption(OPT_CP_DETECT, false);

	m_pOptions->InitOption(OPT_VCS_SYSTEM, VCS_NONE);
	m_pOptions->InitOption(OPT_VSS_PATH, _T(""));

	m_pOptions->InitOption(OPT_ARCHIVE_ENABLE, 1); // Enable by default
	m_pOptions->InitOption(OPT_ARCHIVE_PROBETYPE, false);

	m_pOptions->InitOption(OPT_PLUGINS_ENABLED, false);

	m_pOptions->InitOption(OPT_FONT_FILECMP_USECUSTOM, false);
	m_pOptions->InitOption(OPT_FONT_DIRCMP_USECUSTOM, false);

	m_pOptions->InitOption(OPT_FONT_FILECMP_HEIGHT, -16);
	m_pOptions->InitOption(OPT_FONT_FILECMP_WIDTH, 0);
	m_pOptions->InitOption(OPT_FONT_FILECMP_ESCAPEMENT, 0);
	m_pOptions->InitOption(OPT_FONT_FILECMP_ORIENTATION, 0);
	m_pOptions->InitOption(OPT_FONT_FILECMP_WEIGHT, FW_NORMAL);
	m_pOptions->InitOption(OPT_FONT_FILECMP_ITALIC, false);
	m_pOptions->InitOption(OPT_FONT_FILECMP_UNDERLINE, false);
	m_pOptions->InitOption(OPT_FONT_FILECMP_STRIKEOUT, false);
	m_pOptions->InitOption(OPT_FONT_FILECMP_OUTPRECISION, OUT_STRING_PRECIS);
	m_pOptions->InitOption(OPT_FONT_FILECMP_CLIPPRECISION, CLIP_STROKE_PRECIS);
	m_pOptions->InitOption(OPT_FONT_FILECMP_QUALITY, DRAFT_QUALITY);
	m_pOptions->InitOption(OPT_FONT_FILECMP_PITCHANDFAMILY, FF_MODERN | FIXED_PITCH);

	m_pOptions->InitOption(OPT_FONT_DIRCMP_HEIGHT, -16);
	m_pOptions->InitOption(OPT_FONT_DIRCMP_WIDTH, 0);
	m_pOptions->InitOption(OPT_FONT_DIRCMP_ESCAPEMENT, 0);
	m_pOptions->InitOption(OPT_FONT_DIRCMP_ORIENTATION, 0);
	m_pOptions->InitOption(OPT_FONT_DIRCMP_WEIGHT, FW_NORMAL);
	m_pOptions->InitOption(OPT_FONT_DIRCMP_ITALIC, false);
	m_pOptions->InitOption(OPT_FONT_DIRCMP_UNDERLINE, false);
	m_pOptions->InitOption(OPT_FONT_DIRCMP_STRIKEOUT, false);
	m_pOptions->InitOption(OPT_FONT_DIRCMP_OUTPRECISION, OUT_STRING_PRECIS);
	m_pOptions->InitOption(OPT_FONT_DIRCMP_CLIPPRECISION, CLIP_STROKE_PRECIS);
	m_pOptions->InitOption(OPT_FONT_DIRCMP_QUALITY, DRAFT_QUALITY);
	m_pOptions->InitOption(OPT_FONT_DIRCMP_PITCHANDFAMILY, FF_MODERN | FIXED_PITCH);

	SetFontDefaults();
}

/**
 * @brief Set default font values.
 */
void CMergeApp::SetFontDefaults()
{
	USES_CONVERSION;

	LOGFONT lfDefault;
	ZeroMemory(&lfDefault, sizeof(LOGFONT));

	MIMECPINFO cpi = {0};
	cpi.bGDICharset = ANSI_CHARSET;
	wcscpy(cpi.wszFixedWidthFont, L"Courier New");
	IMultiLanguage *pMLang = NULL;

	HRESULT hr = CoCreateInstance(CLSID_CMultiLanguage, NULL,
		CLSCTX_INPROC_SERVER, IID_IMultiLanguage, (void **)&pMLang);
	if (SUCCEEDED(hr))
	{
		hr = pMLang->GetCodePageInfo(GetACP(), &cpi);
		pMLang->Release();
	}
	m_pOptions->InitOption(OPT_FONT_FILECMP_CHARSET, (int) cpi.bGDICharset);
	m_pOptions->InitOption(OPT_FONT_FILECMP_FACENAME, W2T(cpi.wszFixedWidthFont));

	m_pOptions->InitOption(OPT_FONT_DIRCMP_CHARSET, (int) cpi.bGDICharset);
	m_pOptions->InitOption(OPT_FONT_DIRCMP_FACENAME, W2T(cpi.wszFixedWidthFont));
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
			CopyFromLMtoCU(LMKey, CUKey, _T("Locale\\LanguageId"));
			RegCloseKey(CUKey);
		}
		RegCloseKey(LMKey);
	}
}

/**
 * @brief Open HKLM registry key.
 * @param [out] key Pointer to open HKLM key.
 * @return true if opening succeeded.
 */
static bool OpenHKLM(HKEY *key)
{
	LONG retval = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			_T("Software\\Thingamahoochie\\WinMerge\\"),
			0, KEY_READ, key);
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
 * @return true if opening succeeded.
 */
static bool OpenHKCU(HKEY *key)
{
	LONG retval = RegOpenKeyEx(HKEY_CURRENT_USER,
			_T("Software\\Thingamahoochie\\WinMerge\\"),
			0, KEY_ALL_ACCESS, key);
	if (retval == ERROR_SUCCESS)
	{
		return true;
	}
	else if (retval == ERROR_FILE_NOT_FOUND)
	{
		retval = RegCreateKeyEx(HKEY_CURRENT_USER,
			_T("Software\\Thingamahoochie\\WinMerge\\"),
			0, NULL, 0, KEY_ALL_ACCESS, NULL, key, NULL);
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
			PBYTE buf = new BYTE[len];
			retval = RegQueryValueEx(lmKey, valname, 0, &type, buf, &len);
			if (retval == ERROR_SUCCESS)
			{
				retval = RegSetValueEx(cuKey, valname , 0, type,
					buf, len);
			}
			delete [] buf;
		}
	}
}
