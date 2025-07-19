/** 
 * @file  FileVersion.h
 *
 * @brief Declaration file for FileVersion
 */
#pragma once

#include <atomic>
#include "UnicodeString.h"

/**
 * @brief A class that contains file version information.
 * This class contains file version information that Windows allows
 * file to have (file version, product version).
 */
class FileVersion
{
private:
	std::atomic_uint64_t m_fileVersion;

public:
	FileVersion();
	FileVersion(const FileVersion& other);
	FileVersion& operator=(const FileVersion& other);
	void Clear();
	bool IsCleared() const;
	void SetFileVersion(unsigned versionMS, unsigned versionLS);
	void SetFileVersionNone();
	String GetFileVersionString() const;
	uint64_t GetFileVersionQWORD() const;
};

/**
 * @brief Reset version data to zeroes.
 */
inline void FileVersion::Clear()
{
	m_fileVersion.store(0xFFFFFFFFFFFFFFFFULL, std::memory_order_relaxed);
}

inline bool FileVersion::IsCleared() const
{
	return m_fileVersion.load(std::memory_order_relaxed) == 0xFFFFFFFFFFFFFFFFULL;
}

inline void FileVersion::SetFileVersion(unsigned versionMS, unsigned versionLS)
{
	uint64_t combined = (static_cast<uint64_t>(versionMS) << 32) | versionLS;
	m_fileVersion.store(combined, std::memory_order_relaxed);
}

inline void FileVersion::SetFileVersionNone()
{
	m_fileVersion.store(0xFFFFFFFFFFFFFFFEULL, std::memory_order_relaxed);
}

/**
 * @brief Set file version number.
 * @param [in] versionMS Most significant dword for version.
 * @param [in] versionLS Least significant dword for version.
 */
inline uint64_t FileVersion::GetFileVersionQWORD() const
{
	return m_fileVersion.load(std::memory_order_relaxed);
}
