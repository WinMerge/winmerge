/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  FileSaveHelper.cpp
 *
 * @brief Implementation of file save helper functions
 */

#include "stdafx.h"
#include "FileSaveHelper.h"
#include "UnicodeString.h"
#include "FileTransform.h"
#include "paths.h"
#include "TFile.h"
#include "Plugins.h"
#include <ctime>

namespace FileSaveHelper
{

/** @brief Backup file extension. */
static const tchar_t BACKUP_FILE_EXT[] = _T("bak");

/**
 * @brief Creates backup of a file
 * @param [in] sourcePath Full path to file to backup
 * @param [in] options Backup options
 * @return BackupResult with success status and paths
 */
BackupResult CreateBackup(const String& sourcePath, const BackupOptions& options)
{
	BackupResult result;
	result.success = true;
	result.attempted = false;
	result.sourcePath = sourcePath;
	result.backupPath.clear();
	result.errorMessage.clear();

	// If backup is not enabled, return success
	if (!options.enableBackup)
		return result;

	// Check if source file exists
	if (paths::DoesPathExist(sourcePath) != paths::IS_EXISTING_FILE)
	{
		// File doesn't exist, nothing to backup
		return result;
	}

	result.attempted = true;

	// Split source path
	String path;
	String filename;
	String ext;
	paths::SplitFilename(paths::GetLongPath(sourcePath), &path, &filename, &ext);

	// Determine backup folder
	String bakPath;
	if (options.location == 0) // PropBackups::FOLDER_ORIGINAL
	{
		// Put backups to same folder as original file
		bakPath = path;
	}
	else if (options.location == 1) // PropBackups::FOLDER_GLOBAL
	{
		// Put backups to global folder
		bakPath = options.globalFolder;
		if (bakPath.empty())
		{
			bakPath = path;
		}
		else
		{
			bakPath = paths::GetLongPath(bakPath);
			if (!paths::CreateIfNeeded(bakPath))
			{
				result.success = false;
				result.errorMessage = _("Unable to create backup folder");
				return result;
			}
		}
	}
	else
	{
		result.success = false;
		result.errorMessage = _("Unknown backup location");
		return result;
	}

	// Add .bak extension if wanted
	if (options.addBakExtension)
	{
		// Don't add dot if there is no existing extension
		if (ext.size() > 0)
			ext += _T(".");
		ext += BACKUP_FILE_EXT;
	}

	// Append time to filename if wanted
	if (options.addTimestamp)
	{
		struct tm tm;
		time_t curtime = 0;
		time(&curtime);
		::localtime_s(&tm, &curtime);
		tchar_t timestr[32];
		_sntprintf_s(timestr, _countof(timestr), _TRUNCATE,
			_T("%04d%02d%02d%02d%02d%02d"),
			tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec);
		filename += _T("-");
		filename += timestr;
	}

	// Build final backup path
	bool pathOk = false;
	if ((bakPath.length() + filename.length() + ext.length()) < MAX_PATH_FULL)
	{
		pathOk = true;
		String backupFilename = filename;
		if (!ext.empty())
			backupFilename += _T(".") + ext;
		bakPath = paths::ConcatPath(bakPath, backupFilename);
		result.backupPath = bakPath;
	}

	if (!pathOk)
	{
		result.success = false;
		result.errorMessage = _("Backup path too long");
		return result;
	}

	// Perform the actual file copy
	if (!CopyFileW(TFile(sourcePath).wpath().c_str(),
		TFile(bakPath).wpath().c_str(), FALSE))
	{
		result.success = false;
		result.errorMessage = _("Unable to backup original file");
		return result;
	}

	result.success = true;
	return result;
}

/**
 * @brief Checks if file is read-only
 * @param [in] path Full path to file
 * @return ReadOnlyCheckResult with readonly status
 */
ReadOnlyCheckResult CheckReadOnly(const String& path)
{
	ReadOnlyCheckResult result;
	result.exists = false;
	result.isReadOnly = false;
	result.errorMessage.clear();

	if (path.empty())
		return result;

	try
	{
		TFile file(path);
		result.exists = file.exists();
		if (result.exists)
			result.isReadOnly = !file.canWrite();
	}
	catch (...)
	{
		result.errorMessage = _("Unable to check file status");
	}

	return result;
}

/**
 * @brief Removes read-only attribute from file
 * @param [in] path Full path to file
 * @return true if attribute was removed successfully
 */
bool RemoveReadOnlyAttribute(const String& path)
{
	try
	{
		CFileStatus status;
		if (!CFile::GetStatus(path.c_str(), status))
			return false;

		status.m_mtime = 0; // Avoid unwanted changes
		status.m_attribute &= ~CFile::readOnly;

		CFile::SetStatus(path.c_str(), status);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

/**
 * @brief Formats packing error message
 * @param [in] pane Pane index (0=left, 1=middle, 2=right)
 * @param [in] paneCount Total number of panes
 * @param [in] path File path
 * @param [in] plugin Plugin information
 * @return Formatted error message
 */
String GetPackingErrorMessage(int pane, int paneCount, const String& path, const PackingInfo& plugin)
{
	String pluginName = plugin.GetPluginPipeline();
	return strutils::format_string2(
		pane == 0 ?
			_("Plugin '%2' cannot pack changes to left file into '%1'.\n\nOriginal unchanged. Save unpacked version?")
			: (pane == paneCount - 1) ?
				_("Plugin '%2' cannot pack changes to right file into '%1'.\n\nOriginal unchanged. Save unpacked version?")
				: _("Plugin '%2' cannot pack changes to middle file into '%1'.\n\nOriginal unchanged. Save unpacked version?"),
		path, pluginName);
}

/**
 * @brief Generates message for readonly override prompt
 * @param [in] path Full path to readonly file
 * @param [in] isMultiFile true if handling multiple files/folders
 * @return Formatted message string
 */
String GetReadOnlyOverrideMessage(const String& path, bool isMultiFile)
{
	if (isMultiFile)
	{
		// Multiple files or folder
		return strutils::format_string1(_("%1\nis read-only. Override?"), path);
	}
	else
	{
		// Single file
		return strutils::format_string1(_("%1 is read-only. Override? Or 'No' to save as new filename?"), path);
	}
}

/**
 * @brief Gets MessageBox flags for readonly prompt
 * @param [in] isMultiFile true if handling multiple files/folders
 * @return MessageBox flags (MB_YESNOCANCEL, etc.)
 */
UINT GetReadOnlyMessageBoxFlags(bool isMultiFile)
{
	if (isMultiFile)
	{
		// Multiple files: Yes to All option available, default button is Cancel (3)
		return MB_YESNOCANCEL | MB_ICONWARNING | MB_DEFBUTTON3 | MB_DONT_ASK_AGAIN | MB_YES_TO_ALL;
	}
	else
	{
		// Single file: No Yes to All, default button is No (2)
		return MB_YESNOCANCEL | MB_ICONWARNING | MB_DEFBUTTON2 | MB_DONT_ASK_AGAIN;
	}
}

/**
 * @brief Interprets user's MessageBox choice into action
 * @param [in] mbResult Result from AfxMessageBox (IDYES, IDNO, etc.)
 * @param [in] isMultiFile true if handling multiple files/folders
 * @return UserChoiceResult with action and applyToAll flag
 */
UserChoiceResult InterpretUserChoice(UINT mbResult, bool isMultiFile)
{
	UserChoiceResult result;
	result.applyToAll = false;

	switch (mbResult)
	{
	case IDYESTOALL:
		result.applyToAll = true;
		[[fallthrough]];
	case IDYES:
		result.action = ReadOnlyAction::RemoveReadOnly;
		break;

	case IDNO:
		if (isMultiFile)
			result.action = ReadOnlyAction::Skip;
		else
			result.action = ReadOnlyAction::SelectNew;
		break;

	case IDCANCEL:
	default:
		result.action = ReadOnlyAction::Cancel;
		break;
	}

	return result;
}

} // namespace FileSaveHelper
