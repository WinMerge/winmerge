/**
 * @file  TempFile.cpp
 *
 * @brief Implementation file for TempFile
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "Windows.h"
#include "UnicodeString.h"
#include "TempFile.h"
#include "paths.h"
#include "Environment.h"

/**
 * @brief Delete the temp file when instance is deleted.
 */
TempFile::~TempFile()
{
	Delete();
}

/**
 * @brief Create a temporary file name with default prefix.
 */
void TempFile::Create()
{
	Create(NULL);
}

/**
 * @brief Create a temporary file with given prefix.
 * @param [in] prefix A prefix for temp file name.
 * @return Created temp file path.
 */
String TempFile::Create(LPCTSTR prefix)
{
	String temp = env_GetTempPath(NULL);
	if (temp.empty())
	{
		return L"";
	}

	String pref = prefix;
	if (pref.empty())
		pref = L"wmtmp";

	temp = env_GetTempFileName(temp.c_str(), pref.c_str(), NULL);
	if (!temp.empty())
		m_path = temp;

	return temp;
}

/**
 * @brief Create a temporary file from existing file's contents.
 * This function creates a temporary file to temp folder and copies
 * given file's contents to there.
 * @param [in] filepath Full path to existing file.
 * @param [in] prefix Prefix for the temporary filename.
 * @return Full path to the temporary file.
 */
String TempFile::CreateFromFile(LPCTSTR filepath, LPCTSTR prefix)
{
	String temp = env_GetTempPath(NULL);
	if (temp.empty())
	{
		return L"";
	}

	String pref = prefix;
	if (pref.empty())
		pref = L"wmtmp";

	temp = env_GetTempFileName(temp.c_str(), pref.c_str(), NULL);
	if (!temp.empty())
	{
		// Scratchpads don't have a file to copy.
		m_path = temp;
		if (::CopyFile(filepath, temp.c_str(), FALSE))
		{
			::SetFileAttributes(temp.c_str(), FILE_ATTRIBUTE_NORMAL);
		}
	}
	return temp;
}

/**
 * @brief Get temp file path (including filename).
 * @return Full path to temp file.
 */
String TempFile::GetPath()
{
	return m_path;
}

/**
 * @brief Delete the temporary file, if it exists.
 * @return true if there was no error.
 */
bool TempFile::Delete()
{
	BOOL success = true;
	if (!m_path.empty())
		success = DeleteFile(m_path.c_str());
	if (success)
		m_path = L"";
	return !!success;
}
