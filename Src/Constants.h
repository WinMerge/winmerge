/** 
 * @file Constants.h
 *
 * @brief WinMerge constants, URLs, paths etc.
 *
 */
#pragma once

#include "utils/ctchar.h"

/** @brief URL for hyperlink in About-dialog. */
inline const tchar_t WinMergeURL[] = _T("https://winmerge.org/");

/**
 * @brief URL to help index in internet.
 * We use internet help when local help file is not found (not installed).
 */
inline const tchar_t DocsURL[] = _T("https://manual.winmerge.org/index.html");

/** @brief URL to translations page in internet. */
inline const tchar_t TranslationsUrl[] = _T("https://winmerge.org/translations/");

/** @brief URL of the GPL license. */
inline const tchar_t LicenceUrl[] = _T("http://www.gnu.org/licenses/gpl-2.0.html");

/** @brief Relative (to WinMerge executable ) path to local help file. */
inline const tchar_t DocsPath[] = _T("Docs/WinMerge%s.chm");

/** @brief Contributors list. */
inline const tchar_t ContributorsPath[] = _T("contributors.txt");

/** @brief Release notes in HTML format. */
inline const tchar_t RelNotes[] = _T("\\Docs\\ReleaseNotes%s.html");

/** @brief GPL Licence local file name. */
inline const tchar_t LicenseFile[] = _T("Copying");

/** @brief WinMerge folder in My Folders-folder. */
inline const tchar_t WinMergeDocumentsFolder[] = _T("WinMerge");

/**
 * @brief Default relative path to "My Documents" for private filters.
 * We want to use WinMerge folder as general user-file folder in future.
 * So it makes sense to have own subfolder for filters.
 */
inline const tchar_t DefaultRelativeFilterPath[] = _T("WinMerge/Filters");

/** @brief Temp folder name prefix for WinMerge temp folders. */
inline const tchar_t TempFolderPrefix[] = _T("WinMerge_TEMP_");

/** @brief registry dir to WinMerge */
inline const tchar_t RegDir[] = _T("Software\\Thingamahoochie\\WinMerge");

/** @brief Location for file compare specific help to open. */
inline const tchar_t OptionsHelpLocation[] = _T("::/htmlhelp/Configuration.html");

/** @brief Location for folder compare specific help to open. */
inline const tchar_t DirViewHelpLocation[] = _T("::/htmlhelp/Compare_dirs.html");

/** @brief Location for Open-dialog specific help to open. */
inline const tchar_t OpenDlgHelpLocation[] = _T("::/htmlhelp/Open_paths.html");

/** @brief Location for plugins specific help to open. */
inline const tchar_t PluginsHelpLocation[] = _T("::/htmlhelp/Plugins.html");

/** @brief Location for file compare specific help to open. */
inline const tchar_t MergeViewHelpLocation[] = _T("::/htmlhelp/Compare_files.html");

/** @brief Location for hex compare specific help to open. */
inline const tchar_t HexMergeViewHelpLocation[] = _T("::/htmlhelp/Compare_bin.html");

/** @brief Location for image compare specific help to open. */
inline const tchar_t ImgMergeFrameHelpLocation[] = _T("::/htmlhelp/Compare_images.html");

/** @brief Location for Webpage compare specific help to open. */
inline const tchar_t WebPageDiffFrameHelpLocation[] = _T("::/htmlhelp/Compare_webpages.html");

/** @brief Location for command line help to open. */
inline const tchar_t CommandLineHelpLocation[] = _T("::/htmlhelp/Command_line.html");

/** @brief Location for filters specific help to open. */
inline const tchar_t FilterHelpLocation[] = _T("::/htmlhelp/Filters.html");

