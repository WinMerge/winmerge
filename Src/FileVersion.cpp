/** 
 * @file  FileVersion.cpp
 *
 * @brief Implementation for FileVersion
 */

#include "pch.h"
#include "FileVersion.h"
#include "UnicodeString.h"

#ifndef HIWORD
#define LOWORD(l) ((unsigned short)((l) & 0xffff))
#define HIWORD(l) ((unsigned short)((l) >> 16))
#endif

/**
 * @brief Default constructor.
 */
FileVersion::FileVersion()
{
	Clear();
}

FileVersion::FileVersion(const FileVersion& other)
{
	m_fileVersion.store(other.m_fileVersion.load(std::memory_order_relaxed), std::memory_order_relaxed);
}

FileVersion& FileVersion::operator=(const FileVersion& other)
{
	if (this != &other)
		m_fileVersion.store(other.m_fileVersion.load(std::memory_order_relaxed), std::memory_order_relaxed);
	return *this;
}

/**
 * @brief Get file version as a string.
 * @return File version number as a string. Returns empty string if there is
 * no version number for the file.
 */
String FileVersion::GetFileVersionString() const
{
	auto version = m_fileVersion.load(std::memory_order_relaxed);
	if (version >= 0xFFFFFFFFFFFFFFFEULL)
		return _T("");

	unsigned int fileVersionMS = static_cast<unsigned int>(version >> 32);
	unsigned int fileVersionLS = static_cast<unsigned int>(version & 0xFFFFFFFFULL);
	return strutils::format(_T("%u.%u.%u.%u"), HIWORD(fileVersionMS),
		LOWORD(fileVersionMS), HIWORD(fileVersionLS),
		LOWORD(fileVersionLS));
}

