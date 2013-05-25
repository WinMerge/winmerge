/**
 *  @file TempFile.h
 *
 *  @brief Declaration of TempFile
 */
// ID line follows -- this is updated by SVN
// $Id: TempFile.h 6566 2009-03-12 18:16:39Z kimmov $

#ifndef _TEMP_FILE_
#define _TEMP_FILE_

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

#endif // _TEMP_FILE_
