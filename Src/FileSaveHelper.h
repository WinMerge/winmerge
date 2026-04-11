/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  FileSaveHelper.h
 *
 * @brief Declaration of file save helper functions
 * 
 * This module provides helper functions for safe file saving operations,
 * including backup creation, read-only file handling, and error messaging.
 */
#pragma once

#include "UnicodeString.h"

class PackingInfo;

namespace FileSaveHelper
{
	/**
	 * @brief Result of backup operation
	 */
	struct BackupResult
	{
		bool success;           ///< true if backup succeeded or wasn't needed
		bool attempted;         ///< true if backup was actually attempted
		String sourcePath;      ///< Original file path
		String backupPath;      ///< Backup file path (empty if not attempted)
		String errorMessage;    ///< Error message if failed
	};

	/**
	 * @brief Options for backup creation
	 */
	struct BackupOptions
	{
		bool enableBackup;      ///< Whether backup is enabled
		int location;           ///< Backup location (0=original folder, 1=global folder)
		String globalFolder;    ///< Global backup folder path
		bool addBakExtension;   ///< Add .bak extension
		bool addTimestamp;      ///< Add timestamp to filename
	};

	/**
	 * @brief Result of readonly check operation
	 */
	struct ReadOnlyCheckResult
	{
		bool exists;            ///< true if file exists
		bool isReadOnly;        ///< true if file is read-only
		String errorMessage;    ///< Error message if check failed
	};

	/**
	 * @brief Creates backup of a file
	 * @param [in] sourcePath Full path to file to backup
	 * @param [in] options Backup options
	 * @return BackupResult with success status and paths
	 */
	BackupResult CreateBackup(const String& sourcePath, const BackupOptions& options);

	/**
	 * @brief Checks if file is read-only
	 * @param [in] path Full path to file
	 * @return ReadOnlyCheckResult with readonly status
	 */
	ReadOnlyCheckResult CheckReadOnly(const String& path);

	/**
	 * @brief Removes read-only attribute from file
	 * @param [in] path Full path to file
	 * @return true if attribute was removed successfully
	 */
	bool RemoveReadOnlyAttribute(const String& path);

	/**
	 * @brief Formats packing error message
	 * @param [in] pane Pane index (0=left, 1=middle, 2=right)
	 * @param [in] paneCount Total number of panes
	 * @param [in] path File path
	 * @param [in] plugin Plugin information
	 * @return Formatted error message
	 */
	String GetPackingErrorMessage(int pane, int paneCount, const String& path, const PackingInfo& plugin);

	/**
	 * @brief User action for readonly file handling
	 */
	enum class ReadOnlyAction
	{
		RemoveReadOnly,  ///< Remove readonly attribute and continue
		SelectNew,       ///< Select new filename (single file only)
		Skip,            ///< Skip this file (continue without saving)
		Cancel           ///< Cancel entire operation
	};

	/**
	 * @brief Result of user choice interpretation for readonly handling
	 */
	struct UserChoiceResult
	{
		ReadOnlyAction action;  ///< Action to take
		bool applyToAll;        ///< Apply this choice to all remaining files
	};

	/**
	 * @brief Generates message for readonly override prompt
	 * @param [in] path Full path to readonly file
	 * @param [in] isMultiFile true if handling multiple files/folders
	 * @return Formatted message string
	 */
	String GetReadOnlyOverrideMessage(const String& path, bool isMultiFile);

	/**
	 * @brief Gets MessageBox flags for readonly prompt
	 * @param [in] isMultiFile true if handling multiple files/folders
	 * @return MessageBox flags (MB_YESNOCANCEL, etc.)
	 */
	UINT GetReadOnlyMessageBoxFlags(bool isMultiFile);

	/**
	 * @brief Interprets user's MessageBox choice into action
	 * @param [in] mbResult Result from AfxMessageBox (IDYES, IDNO, etc.)
	 * @param [in] isMultiFile true if handling multiple files/folders
	 * @return UserChoiceResult with action and applyToAll flag
	 */
	UserChoiceResult InterpretUserChoice(UINT mbResult, bool isMultiFile);

} // namespace FileSaveHelper
