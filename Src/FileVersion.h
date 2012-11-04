/** 
 * @file  FileVersion.h
 *
 * @brief Declaration file for FileVersion
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _FILE_VERSION_H_
#define _FILE_VERSION_H_

#include "UnicodeString.h"

/**
 * @brief A class that contains file version information.
 * This class contains file version information that Windows allows
 * file to have (file version, product version).
 */
class FileVersion
{
private:
	bool m_bFileVersionSet; //*< Is file version set? */
	bool m_bProductVersionSet; //*< Is product version set? */
	unsigned m_fileVersionMS; //*< File version most significant dword. */
	unsigned m_fileVersionLS; //*< File version least significant dword. */
	unsigned m_productVersionMS; //*< Product version most significant dword. */
	unsigned m_productVersionLS; //*< Product version least significant dword. */

public:
	FileVersion();
	void Clear();
	void SetFileVersion(unsigned versionMS, unsigned versionLS);
	void SetProductVersion(unsigned versionMS, unsigned versionLS);

	String GetFileVersionString();
	String GetProductVersionString();
};

#endif // _FILE_VERSION_H_
