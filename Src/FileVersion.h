/** 
 * @file  FileVersion.h
 *
 * @brief Declaration file for FileVersion
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _FILE_VERSION_H_
#define _FILE_VERSION_H_

/**
 * @brief A class that contains file version information.
 * This class contains file version information that Windows allows
 * file to have (file version, product version).
 */
class FileVersion
{
private:
	BOOL m_bFileVersionSet; //*< Is file version set? */
	BOOL m_bProductVersionSet; //*< Is product version set? */
	DWORD m_fileVersionMS; //*< File version most significant dword. */
	DWORD m_fileVersionLS; //*< File version least significant dword. */
	DWORD m_productVersionMS; //*< Product version most significant dword. */
	DWORD m_productVersionLS; //*< Product version least significant dword. */

public:
	FileVersion();
	void Clear();
	void SetFileVersion(DWORD versionMS, DWORD versionLS);
	void SetProductVersion(DWORD versionMS, DWORD versionLS);

	String GetFileVersionString();
	String GetProductVersionString();
};

#endif // _FILE_VERSION_H_
