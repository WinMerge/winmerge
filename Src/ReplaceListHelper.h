/**
 * @file  ReplaceListHelper.h
 * @brief Helper functions for replace-list UI operations
 */
#pragma once

#include "UnicodeString.h"
#include <vector>
#include <optional>

// Forward declaration
class CWnd;

namespace ReplaceListHelper
{
	// Max number of replace lists shown in menu
	constexpr int MaxReplaceListSize = 20;

	/**
	 * @brief Get the folder path for replace lists
	 * @param locationType 0 for AppData, 1 for MyDocuments
	 * @param isRegex true for regex replace lists, false for string replace lists
	 * @return Folder path for replace lists
	 */
	String GetReplaceListFolder(int locationType, bool isRegex);

	/**
	 * @brief Get list of replace list files
	 * @param isRegex true for regex replace lists, false for string replace lists
	 * @return Vector of file paths
	 */
	std::vector<String> GetReplaceLists(bool isRegex);

	/**
	 * @brief Create a template replace list file
	 * @param filepath Path where the file should be created
	 * @param isRegex true for regex template, false for string template
	 * @return true if successful, false otherwise
	 */
	bool CreateTemplateFile(const String& filepath, bool isRegex);

	/**
	 * @brief Replace app data or user profile folder paths with environment variables
	 * @param path Path to process
	 * @return Path with environment variables
	 */
	String ReplaceAppDataFolderOrUserProfileFolder(const String& path);

	/**
	 * @brief Create and select a replace list file (shows file dialog)
	 * @param pParentWnd Parent window for dialogs
	 * @param isRegex true for regex replace list, false for string replace list
	 * @return File path if successful, nullopt otherwise
	 */
	std::optional<String> CreateAndSelectReplaceListFile(CWnd* pParentWnd, bool isRegex);
}
