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
	void Create();
	String Create(const TCHAR *prefix = NULL, const TCHAR *ext = NULL);
	String CreateFromFile(const TCHAR *filepath, const TCHAR *prefix);
	String GetPath();
	bool Delete();

private:
	String m_path; /**< Temporary file path. */
};

void CleanupWMtemp();
bool CleanupWMtempfolder(std::vector <int> processIDs);
bool WMrunning(std::vector <int> processIDs, int iPI);
bool ClearTempfolder(const String &pathName);

#endif // _TEMP_FILE_
