/** 
 * @file  FileVersion.h
 *
 * @brief Declaration file for FileVersion
 */
#pragma once

#include "UnicodeString.h"

/**
 * @brief A class that contains file version information.
 * This class contains file version information that Windows allows
 * file to have (file version, product version).
 */
class FileVersion
{
private:
	unsigned m_fileVersionMS; //*< File version most significant dword. */
	unsigned m_fileVersionLS; //*< File version least significant dword. */

public:
	FileVersion();
	void Clear();
	bool IsCleared() const { return m_fileVersionMS == 0xffffffff && m_fileVersionLS == 0xffffffff; };
	void SetFileVersion(unsigned versionMS, unsigned versionLS);
	void SetFileVersionNone() { m_fileVersionMS = 0xffffffff; m_fileVersionLS = 0xfffffffe; };
	String GetFileVersionString() const;
	uint64_t GetFileVersionQWORD() const { return (static_cast<uint64_t>(m_fileVersionMS) << 32) + m_fileVersionLS; };
};
