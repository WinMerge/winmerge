/** 
 * @file Constants.h
 *
 * @brief WinMerge constants, URLs, paths etc.
 *
 */
#pragma once

#include <tchar.h>

/** @brief URL for hyperlink in About-dialog. */
inline const TCHAR WinMergeURL[] = _T("https://winmerge.org/");

/**
 * @brief URL to help index in internet.
 * We use internet help when local help file is not found (not installed).
 */
inline const TCHAR DocsURL[] = _T("https://manual.winmerge.org/index.html");

/** @brief URL to translations page in internet. */
inline const TCHAR TranslationsUrl[] = _T("https://winmerge.org/translations/");

/** @brief URL of the GPL license. */
inline const TCHAR LicenceUrl[] = _T("http://www.gnu.org/licenses/gpl-2.0.html");

/** @brief Relative (to WinMerge executable ) path to local help file. */
inline const TCHAR DocsPath[] = _T("Docs/WinMerge%s.chm");

/** @brief Contributors list. */
inline const TCHAR ContributorsPath[] = _T("contributors.txt");

/** @brief Release notes in HTML format. */
inline const TCHAR RelNotes[] = _T("\\Docs\\ReleaseNotes%s.html");

/** @brief GPL Licence local file name. */
inline const TCHAR LicenseFile[] = _T("Copying");

/** @brief WinMerge folder in My Folders-folder. */
inline const TCHAR WinMergeDocumentsFolder[] = _T("WinMerge");

/**
 * @brief Default relative path to "My Documents" for private filters.
 * We want to use WinMerge folder as general user-file folder in future.
 * So it makes sense to have own subfolder for filters.
 */
inline const TCHAR DefaultRelativeFilterPath[] = _T("WinMerge/Filters");

/** @brief Temp folder name prefix for WinMerge temp folders. */
inline const TCHAR TempFolderPrefix[] = _T("WinMerge_TEMP_");

/** @brief registry dir to WinMerge */
inline const TCHAR RegDir[] = _T("Software\\Thingamahoochie\\WinMerge");

/** @brief Location for file compare specific help to open. */
inline const TCHAR OptionsHelpLocation[] = _T("::/htmlhelp/Configuration.html");

/** @brief Location for folder compare specific help to open. */
inline const TCHAR DirViewHelpLocation[] = _T("::/htmlhelp/Compare_dirs.html");

/** @brief Location for Open-dialog specific help to open. */
inline const TCHAR OpenDlgHelpLocation[] = _T("::/htmlhelp/Open_paths.html");

/** @brief Location for plugins specific help to open. */
inline const TCHAR PluginsHelpLocation[] = _T("::/htmlhelp/Plugins.html");

/** @brief Location for file compare specific help to open. */
inline const TCHAR MergeViewHelpLocation[] = _T("::/htmlhelp/Compare_files.html");

/** @brief Location for hex compare specific help to open. */
inline const TCHAR HexMergeViewHelpLocation[] = _T("::/htmlhelp/Compare_bin.html");

/** @brief Location for image compare specific help to open. */
inline const TCHAR ImgMergeFrameHelpLocation[] = _T("::/htmlhelp/Compare_images.html");

/** @brief Location for Webpage compare specific help to open. */
inline const TCHAR WebPageDiffFrameHelpLocation[] = _T("::/htmlhelp/Compare_webpages.html");

/** @brief Location for command line help to open. */
inline const TCHAR CommandLineHelpLocation[] = _T("::/htmlhelp/Command_line.html");

/** @brief Location for filters specific help to open. */
inline const TCHAR FilterHelpLocation[] = _T("::/htmlhelp/Filters.html");

/**
 * @brief Flags used when opening files
 */
enum
{
	FFILEOPEN_NONE		= 0x0000,
	FFILEOPEN_NOMRU		= 0x0001, /**< Do not add this path to MRU list */
	FFILEOPEN_READONLY	= 0x0002, /**< Open this path as read-only */
	FFILEOPEN_MODIFIED  = 0x0004, /**< Mark file modified after opening. */
	FFILEOPEN_CMDLINE	= 0x0010, /**< Path is read from commandline */
	FFILEOPEN_PROJECT	= 0x0020, /**< Path is read from project-file */
	FFILEOPEN_SETFOCUS  = 0x0040, /**< Set focus to this pane */
	FFILEOPEN_AUTOMERGE = 0x0080, /**< auto-merge at this pane */
};
