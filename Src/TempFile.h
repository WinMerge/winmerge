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
	TempFile() {}
	~TempFile();
	String Create(const String& prefix = _T(""), const String& ext = _T(""));
	String CreateFromFile(const String& filepath, const String& prefix);
	/**
	 * @brief Get temp file path (including filename).
	 * @return Full path to temp file.
	 */
	const String& GetPath() const { return m_path; }
	bool Delete();

private:
	String m_path; /**< Temporary file path. */
};

void CleanupWMtemp();
bool ClearTempfolder(const String &pathName);
