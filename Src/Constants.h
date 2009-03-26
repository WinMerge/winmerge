/** 
 * @file Constants.h
 *
 * @brief WinMerge constants, URLs, paths etc.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

/** @brief URL for hyperlink in About-dialog. */
const TCHAR WinMergeURL[] = _T("http://winmerge.org/");

/**
 * @brief URL to help index in internet.
 * We use internet help when local help file is not found (not installed).
 */
const TCHAR DocsURL[] = _T("http://winmerge.org/docs/manual/index.html");

/** @brief URL to translations page in internet. */
const TCHAR TranslationsUrl[] = _T("http://winmerge.org/translations/");

/** @brief URL of the GPL license. */
const TCHAR LicenceUrl[] = _T("http://www.gnu.org/licenses/gpl-2.0.html");

/** @brief WinMerge download page URL. */
const TCHAR DownloadUrl[] = _T("http://winmerge.org/downloads/");

/** @brief Relative (to WinMerge executable ) path to local help file. */
const TCHAR DocsPath[] = _T("\\Docs\\WinMerge.chm");

/** @brief Contributors list. */
const TCHAR ContributorsPath[] = _T("\\contributors.txt");

/** @brief Release notes in HTML format. */
const TCHAR RelNotes[] = _T("\\Docs\\ReleaseNotes.html");

/** @brief GLP Licence local file name. */
const TCHAR LicenseFile[] = _T("\\Copying");

/**
 * @brief Default relative path to "My Documents" for private filters.
 * We want to use WinMerge folder as general user-file folder in future.
 * So it makes sense to have own subfolder for filters.
 */
const TCHAR DefaultRelativeFilterPath[] = _T("WinMerge\\Filters");

/** @brief Executable Filename for ANSI build. */
const TCHAR ExecuteFilename[] = _T("WinMerge.exe");
/** @brief Executable Filename for Unicode build. */
const TCHAR ExecuteFilenameU[] = _T("WinMergeU.exe");

/** @brief Temp folder name prefix for WinMerge temp folders. */
const TCHAR TempFolderPrefix[] = _T("WM_");

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
};

#endif // _CONSTANTS_H_
