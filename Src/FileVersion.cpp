/** 
 * @file  FileVersion.cpp
 *
 * @brief Implementation for FileVersion
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "UnicodeString.h"
#include "FileVersion.h"

/**
 * @brief Default constructor.
 */
FileVersion::FileVersion()
: m_bFileVersionSet(FALSE)
, m_fileVersionMS(0)
, m_fileVersionLS(0)
, m_bProductVersionSet(FALSE)
, m_productVersionMS(0)
, m_productVersionLS(0)
{
}

/**
 * @brief Reset version data to zeroes.
 */
void FileVersion::Clear()
{
	m_bFileVersionSet = FALSE;
	m_fileVersionMS = 0;
	m_fileVersionLS = 0;
	m_bProductVersionSet = FALSE;
	m_productVersionMS = 0;
	m_productVersionLS = 0;
}

/**
 * @brief Set file version number.
 * @param [in] versionMS Most significant dword for version.
 * @param [in] versionLS Least significant dword for version.
 */
void FileVersion::SetFileVersion(DWORD versionMS, DWORD versionLS)
{
	m_bFileVersionSet = TRUE;
	m_fileVersionMS = versionMS;
	m_fileVersionLS = versionLS;
}

/**
 * @brief Set product version number.
 * @param [in] versionMS Most significant dword for version.
 * @param [in] versionLS Least significant dword for version.
 */
void FileVersion::SetProductVersion(DWORD versionMS, DWORD versionLS)
{
	m_bProductVersionSet = TRUE;
	m_fileVersionMS = versionMS;
	m_fileVersionLS = versionLS;
}

/**
 * @brief Get file version as a string.
 * @return File version number as a string. Returns empty string if there is
 * no version number for the file.
 */
String FileVersion::GetFileVersionString()
{
	if (!m_bFileVersionSet)
		return _T("");

	TCHAR ver[30] = {0};
	_stprintf(ver, _T("%u.%u.%u.%u"), HIWORD(m_fileVersionMS),
		LOWORD(m_fileVersionMS), HIWORD(m_fileVersionLS),
		LOWORD(m_fileVersionLS));
	return ver;
}

/**
 * @brief Get product version as a string.
 * @return Product version number as a string.
 */
String FileVersion::GetProductVersionString()
{
	if (!m_bProductVersionSet)
		return _T("0.0.0.0");

	TCHAR ver[30] = {0};	
	_stprintf(ver, _T("%u.%u.%u.%u"), HIWORD(m_productVersionMS),
		LOWORD(m_productVersionMS), HIWORD(m_productVersionLS),
		LOWORD(m_productVersionLS));
	return ver;
}
