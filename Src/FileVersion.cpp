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
: m_fileVersionMS(0xffffffff)
, m_fileVersionLS(0xffffffff)
{
}

/**
 * @brief Get file version as a string.
 * @return File version number as a string. Returns empty string if there is
 * no version number for the file.
 */
String FileVersion::GetFileVersionString() const
{
	if (m_fileVersionMS == 0xffffffff && m_fileVersionLS >= 0xfffffffe)
		return _T("");

	return strutils::format(_T("%u.%u.%u.%u"), HIWORD(m_fileVersionMS),
		LOWORD(m_fileVersionMS), HIWORD(m_fileVersionLS),
		LOWORD(m_fileVersionLS));
}

