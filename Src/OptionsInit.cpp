/**
 * @file  OptionsInit.cpp
 *
 * @brief Options initialisation.
 */

#include "pch.h"
#include <vector>
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "RegOptionsMgr.h"
#include "OptionsDiffOptions.h"
#include "OptionsDiffColors.h"
#include "OptionsDirColors.h"
#include "OptionsFont.h"
#include "DiffWrapper.h" // CMP_CONTENT
#include "paths.h"
#include "Environment.h"
#include "FileTransform.h"
#include "Constants.h"

// Functions to copy values set by installer from HKLM to HKCU.
static void CopyHKLMValues();
static bool OpenHKLM(HKEY *key, LPCTSTR relpath = nullptr);
static bool OpenHKCU(HKEY *key, LPCTSTR relpath = nullptr);
static void CopyFromLMtoCU(HKEY lmKey, HKEY cuKey, LPCTSTR valname);

namespace Options
{

/**
 * @brief Initialise options and set default values.
 *
 * @note Remember default values are what users see first time
 * using WinMerge and many users never change them. So pick
 * default values carefully!
 */
void Init(COptionsMgr *pOptions)
{
	// Copy some values from HKLM to HKCU
	CopyHKLMValues();

	static_cast<CRegOptionsMgr *>(pOptions)->SetRegRootKey(_T("Thingamahoochie\\WinMerge\\"));

	LANGID LangId = GetUserDefaultLangID();
	pOptions->InitOption(OPT_SELECTED_LANGUAGE, static_cast<int>(LangId));

	// Initialise options (name, default value)
	pOptions->InitOption(OPT_SHOW_UNIQUE_LEFT, true);
	pOptions->InitOption(OPT_SHOW_UNIQUE_MIDDLE, true);
	pOptions->InitOption(OPT_SHOW_UNIQUE_RIGHT, true);
	pOptions->InitOption(OPT_SHOW_DIFFERENT, true);
	pOptions->InitOption(OPT_SHOW_IDENTICAL, true);
	pOptions->InitOption(OPT_SHOW_BINARIES, true);
	pOptions->InitOption(OPT_SHOW_SKIPPED, false);
	pOptions->InitOption(OPT_SHOW_DIFFERENT_LEFT_ONLY, true);
	pOptions->InitOption(OPT_SHOW_DIFFERENT_MIDDLE_ONLY, true);
	pOptions->InitOption(OPT_SHOW_DIFFERENT_RIGHT_ONLY, true);
	pOptions->InitOption(OPT_SHOW_MISSING_LEFT_ONLY, true);
	pOptions->InitOption(OPT_SHOW_MISSING_MIDDLE_ONLY, true);
	pOptions->InitOption(OPT_SHOW_MISSING_RIGHT_ONLY, true);

	pOptions->InitOption(OPT_SHOW_TOOLBAR, true);
	pOptions->InitOption(OPT_SHOW_STATUSBAR, true);
	pOptions->InitOption(OPT_SHOW_TABBAR, true);
	pOptions->InitOption(OPT_TOOLBAR_SIZE, GetSystemMetrics(SM_CXSMICON) < 24 ? 0 : 1);
	pOptions->InitOption(OPT_RESIZE_PANES, false);

	pOptions->InitOption(OPT_SYNTAX_HIGHLIGHT, true);
	pOptions->InitOption(OPT_WORDWRAP, false);
	pOptions->InitOption(OPT_VIEW_LINENUMBERS, false);
	pOptions->InitOption(OPT_VIEW_WHITESPACE, false);
	pOptions->InitOption(OPT_CONNECT_MOVED_BLOCKS, 0);
	pOptions->InitOption(OPT_SCROLL_TO_FIRST, false);
	pOptions->InitOption(OPT_VERIFY_OPEN_PATHS, true);
	pOptions->InitOption(OPT_AUTO_COMPLETE_SOURCE, (int)1);
	pOptions->InitOption(OPT_VIEW_FILEMARGIN, false);
	pOptions->InitOption(OPT_DIFF_CONTEXT, (int)-1);
	pOptions->InitOption(OPT_SPLIT_HORIZONTALLY, false);

	pOptions->InitOption(OPT_WORDDIFF_HIGHLIGHT, true);
	pOptions->InitOption(OPT_BREAK_SEPARATORS, _T(".,:;?[](){}<>`'!\"#$%&^~\\|@+-*/"));

	pOptions->InitOption(OPT_BACKUP_FOLDERCMP, false);
	pOptions->InitOption(OPT_BACKUP_FILECMP, true);
	pOptions->InitOption(OPT_BACKUP_LOCATION, (int)0);
	pOptions->InitOption(OPT_BACKUP_GLOBALFOLDER, _T(""));
	pOptions->InitOption(OPT_BACKUP_ADD_BAK, true);
	pOptions->InitOption(OPT_BACKUP_ADD_TIME, false);

	pOptions->InitOption(OPT_DIRVIEW_SORT_COLUMN, (int)-1);
	pOptions->InitOption(OPT_DIRVIEW_SORT_COLUMN3, (int)-1);
	pOptions->InitOption(OPT_DIRVIEW_SORT_ASCENDING, true);
	pOptions->InitOption(OPT_SHOW_SELECT_FILES_AT_STARTUP, false);
	pOptions->InitOption(OPT_DIRVIEW_EXPAND_SUBDIRS, false);

	pOptions->InitOption(OPT_REPORTFILES_REPORTTYPE, 0);
	pOptions->InitOption(OPT_REPORTFILES_COPYTOCLIPBOARD, false);
	pOptions->InitOption(OPT_REPORTFILES_INCLUDEFILECMPREPORT, false);

	pOptions->InitOption(OPT_AUTOMATIC_RESCAN, false);
	pOptions->InitOption(OPT_ALLOW_MIXED_EOL, false);
	pOptions->InitOption(OPT_TAB_SIZE, (int)4);
	pOptions->InitOption(OPT_TAB_TYPE, (int)0);	// 0 means tabs inserted

	pOptions->InitOption(OPT_EXT_EDITOR_CMD, paths::ConcatPath(env::GetWindowsDirectory(), _T("NOTEPAD.EXE")));
	pOptions->InitOption(OPT_USE_RECYCLE_BIN, true);
	pOptions->InitOption(OPT_SINGLE_INSTANCE, false);
	pOptions->InitOption(OPT_MERGE_MODE, false);
	// OPT_WORDDIFF_HIGHLIGHT is initialized above
	pOptions->InitOption(OPT_BREAK_ON_WORDS, false);
	pOptions->InitOption(OPT_BREAK_TYPE, 1);

	pOptions->InitOption(OPT_CLOSE_WITH_ESC, true);
	pOptions->InitOption(OPT_CLOSE_WITH_OK, false);
	pOptions->InitOption(OPT_IGNORE_SMALL_FILETIME, false);
	pOptions->InitOption(OPT_ASK_MULTIWINDOW_CLOSE, false);
	pOptions->InitOption(OPT_PRESERVE_FILETIMES, false);
	pOptions->InitOption(OPT_TREE_MODE, true);

	pOptions->InitOption(OPT_CMP_METHOD, (int)CMP_CONTENT);
	pOptions->InitOption(OPT_CMP_MOVED_BLOCKS, false);
	pOptions->InitOption(OPT_CMP_MATCH_SIMILAR_LINES, false);
	pOptions->InitOption(OPT_CMP_STOP_AFTER_FIRST, false);
	pOptions->InitOption(OPT_CMP_QUICK_LIMIT, 4 * 1024 * 1024); // 4 Megs
	pOptions->InitOption(OPT_CMP_BINARY_LIMIT, 64 * 1024 * 1024); // 64 Megs
	pOptions->InitOption(OPT_CMP_COMPARE_THREADS, -1);
	pOptions->InitOption(OPT_CMP_WALK_UNIQUE_DIRS, false);
	pOptions->InitOption(OPT_CMP_IGNORE_REPARSE_POINTS, false);
	pOptions->InitOption(OPT_CMP_IGNORE_CODEPAGE, true);
	pOptions->InitOption(OPT_CMP_INCLUDE_SUBDIRS, true);

	pOptions->InitOption(OPT_CMP_BIN_FILEPATTERNS, _T("*.bin;*.frx"));

	pOptions->InitOption(OPT_CMP_IMG_FILEPATTERNS, _T("*.bmp;*.cut;*.dds;*.exr;*.g3;*.gif;*.hdr;*.ico;*.iff;*.lbm;*.j2k;*.j2c;*.jng;*.jp2;*.jpg;*.jif;*.jpeg;*.jpe;*.jxr;*.wdp;*.hdp;*.koa;*.mng;*.pcd;*.pcx;*.pfm;*.pct;*.pict;*.pic;*.png;*.pbm;*.pgm;*.ppm;*.psd;*.ras;*.sgi;*.rgb;*.rgba;*.bw;*.tga;*.targa;*.tif;*.tiff;*.wap;*.wbmp;*.wbm;*.webp;*.xbm;*.xpm"));
	pOptions->InitOption(OPT_CMP_IMG_SHOWDIFFERENCES, true);
	pOptions->InitOption(OPT_CMP_IMG_OVERLAYMOVE, 0);
	pOptions->InitOption(OPT_CMP_IMG_OVERLAYALPHA, 30);
	pOptions->InitOption(OPT_CMP_IMG_DRAGGING_MODE, 1);
	pOptions->InitOption(OPT_CMP_IMG_ZOOM, 1000);
	pOptions->InitOption(OPT_CMP_IMG_USEBACKCOLOR, true);
	pOptions->InitOption(OPT_CMP_IMG_BACKCOLOR, 0xFFFFFF);
	pOptions->InitOption(OPT_CMP_IMG_DIFFBLOCKSIZE, 8);
	pOptions->InitOption(OPT_CMP_IMG_DIFFCOLORALPHA, 70);
	pOptions->InitOption(OPT_CMP_IMG_THRESHOLD, 0);
	pOptions->InitOption(OPT_CMP_IMG_INSERTIONDELETIONDETECTION_MODE, 0);

	pOptions->InitOption(OPT_PROJECTS_PATH, _T(""));
	pOptions->InitOption(OPT_USE_SYSTEM_TEMP_PATH, true);
	pOptions->InitOption(OPT_CUSTOM_TEMP_PATH, _T(""));

	pOptions->InitOption(OPT_LINEFILTER_ENABLED, false);
	pOptions->InitOption(OPT_FILEFILTER_CURRENT, _T("*.*"));
	// CMainFrame initializes this when it is empty.
	pOptions->InitOption(OPT_FILTER_USERPATH, paths::ConcatPath(env::GetMyDocuments(), DefaultRelativeFilterPath));
	pOptions->InitOption(OPT_FILEFILTER_SHARED, false);

	pOptions->InitOption(OPT_CP_DEFAULT_MODE, (int)0);
	pOptions->InitOption(OPT_CP_DEFAULT_CUSTOM, (int)GetACP());

	if (PRIMARYLANGID(LangId) == LANG_JAPANESE)
		pOptions->InitOption(OPT_CP_DETECT, (int)(50932 << 16) | 3);
	else if (LangId == MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED))
		pOptions->InitOption(OPT_CP_DETECT, (int)(50936 << 16) | 3);
	else if (PRIMARYLANGID(LangId) == LANG_KOREAN)
		pOptions->InitOption(OPT_CP_DETECT, (int)(50949 << 16) | 3);
	else if (LangId == MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL))
		pOptions->InitOption(OPT_CP_DETECT, (int)(50950 << 16) | 3);
	else
		pOptions->InitOption(OPT_CP_DETECT, (int)(50001 << 16) | 1);

	pOptions->InitOption(OPT_ARCHIVE_ENABLE, 1); // Enable by default
	pOptions->InitOption(OPT_ARCHIVE_PROBETYPE, false);
	pOptions->InitOption(OPT_ARCHIVE_FILTER_INDEX, 1);

	pOptions->InitOption(OPT_PLUGINS_ENABLED, true);
	pOptions->InitOption(OPT_PLUGINS_DISABLED_LIST, _T(""));
	pOptions->InitOption(OPT_PLUGINS_UNPACKER_MODE, PLUGIN_MANUAL);
	pOptions->InitOption(OPT_PLUGINS_PREDIFFER_MODE, PLUGIN_MANUAL);
	pOptions->InitOption(OPT_PLUGINS_UNPACK_DONT_CHECK_EXTENSION, false);

	pOptions->InitOption(OPT_PATCHCREATOR_PATCH_STYLE, 0);
	pOptions->InitOption(OPT_PATCHCREATOR_CONTEXT_LINES, 0);
	pOptions->InitOption(OPT_PATCHCREATOR_CASE_SENSITIVE, true);
	pOptions->InitOption(OPT_PATCHCREATOR_EOL_SENSITIVE, true);
	pOptions->InitOption(OPT_PATCHCREATOR_IGNORE_BLANK_LINES, false);
	pOptions->InitOption(OPT_PATCHCREATOR_WHITESPACE, WHITESPACE_COMPARE_ALL);
	pOptions->InitOption(OPT_PATCHCREATOR_OPEN_TO_EDITOR, false);
	pOptions->InitOption(OPT_PATCHCREATOR_INCLUDE_CMD_LINE, false);

	pOptions->InitOption(OPT_TABBAR_AUTO_MAXWIDTH, true);
	pOptions->InitOption(OPT_ACTIVE_FRAME_MAX, true);
	pOptions->InitOption(OPT_ACTIVE_PANE, 0);

	pOptions->InitOption(OPT_MRU_MAX, 9);

	Options::DiffOptions::SetDefaults(pOptions);
	Options::DiffColors::SetDefaults(pOptions);
	Options::DirColors::SetDefaults(pOptions);
	Options::Font::SetDefaults(pOptions);
}

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
 * @param [in] relpath Relative registry path (to WinMerge reg path) to open, or nullptr.
 * @return true if opening succeeded.
 */
static bool OpenHKLM(HKEY *key, LPCTSTR relpath)
{
	TCHAR valuename[256];
	if (relpath)
		wsprintf(valuename, _T("%s\\%s"), RegDir, relpath);
	else
		lstrcpy(valuename, RegDir);
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
 * @param [in] relpath Relative registry path (to WinMerge reg path) to open, or nullptr.
 * @return true if opening succeeded.
 */
static bool OpenHKCU(HKEY *key, LPCTSTR relpath)
{
	TCHAR valuename[256];
	if (relpath)
		wsprintf(valuename, _T("%s\\%s"), RegDir, relpath);
	else
		lstrcpy(valuename, RegDir);
	LONG retval = RegOpenKeyEx(HKEY_CURRENT_USER,
			valuename, 0, KEY_ALL_ACCESS, key);
	if (retval == ERROR_SUCCESS)
	{
		return true;
	}
	else if (retval == ERROR_FILE_NOT_FOUND)
	{
		retval = RegCreateKeyEx(HKEY_CURRENT_USER,
			valuename, 0, nullptr, 0, KEY_ALL_ACCESS, nullptr, key, nullptr);
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
	LONG retval = RegQueryValueEx(cuKey, valname, 0, nullptr, nullptr, &len);
	if (retval == ERROR_FILE_NOT_FOUND)
	{
		retval = RegQueryValueEx(lmKey, valname, 0, nullptr, nullptr, &len);
		if (retval == ERROR_SUCCESS)
		{
			DWORD type = 0;
			std::vector<BYTE> buf(len);
			retval = RegQueryValueEx(lmKey, valname, 0, &type, &buf[0], &len);
			if (retval == ERROR_SUCCESS)
			{
				RegSetValueEx(cuKey, valname , 0, type,
					&buf[0], len);
			}
		}
	}
}
