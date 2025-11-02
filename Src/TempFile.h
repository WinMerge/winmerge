/**
 *  @file TempFile.h
 *
 *  @brief Declaration of TempFile
 */
#pragma once

#include <vector>
#include "UnicodeString.h"

/**
 * @brief A simple temporary file holder class.
 * This class creates and holds temporary file names. When instance
 * gets destroyed, the temporary file is also deleted.
 */
class TempFile
{
public:
	TempFile() = default;
	TempFile(TempFile&& other) noexcept : m_path(other.m_path) { other.m_path.clear(); }
	~TempFile();
	String Create(const String& prefix = _T(""), const String& ext = _T(""));
	void Attach(const String& path) { Delete(); m_path = path; }
	/**
	 * @brief Get temp file path (including filename).
	 * @return Full path to temp file.
	 */
	const String& GetPath() const { return m_path; }
	bool Delete();

private:
	String m_path; /**< Temporary file path. */
};

/**
 * @brief A simple temporary folder holder class.
 * This class creates and holds temporary folder names. When instance
 * gets destroyed, the temporary folder is also deleted.
 */
class TempFolder
{
public:
	TempFolder() {}
	~TempFolder();
	String Create();
	/**
	 * @brief Get temp folder path
	 * @return Full path to temp folder.
	 */
	const String& GetPath() const { return m_path; }
	bool Delete();

private:
	String m_path; /**< Temporary folder path. */
};

void CleanupWMtemp();
bool ClearTempfolder(const String &pathName);
