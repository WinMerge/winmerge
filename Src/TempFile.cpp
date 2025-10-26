/**
 * @file  TempFile.cpp
 *
 * @brief Implementation file for TempFile
 *
 */

#include "pch.h"
#include "TempFile.h"
#include <windows.h>
#include "paths.h"
#include "TFile.h"
#include "DirTravel.h"
#include "DirItem.h"
#include "Environment.h"
#include "Constants.h"
#include "unicoder.h"

static bool WMrunning(int iPI);

/**
 * @brief Delete the temp file when instance is deleted.
 */
TempFile::~TempFile()
{
	Delete();
}


/**
 * @brief Create a temporary file with given prefix.
 * @param [in] prefix A prefix for temp file name.
 * @param [in] ext extension for temp file name.
 * @return Created temp file path.
 */
String TempFile::Create(const String& prefix, const String& ext)
{
	String temp = env::GetTemporaryPath();
	if (temp.empty())
	{
		return _T("");
	}

	String pref = prefix;
	if (pref.empty())
		pref = _T("wmtmp");

	temp = env::GetTemporaryFileName(temp, pref, nullptr);
	if (!temp.empty())
	{
		if (!ext.empty())
		{
			String tempext = temp + ext;
			if (MoveFile(temp.c_str(), tempext.c_str()))
				temp = std::move(tempext);
		}
		m_path = temp;
	}

	return temp;
}

/**
 * @brief Delete the temporary file, if it exists.
 * @return true if there was no error.
 */
bool TempFile::Delete()
{
	bool success = true;
	if (!m_path.empty())
		success = !!DeleteFile(m_path.c_str());
	if (success)
		m_path.clear();
	return success;
}

/**
 * @brief Delete the temp file when instance is deleted.
 */
TempFolder::~TempFolder()
{
	Delete();
}


/**
 * @brief Create a temporary folder with given prefix.
 * @return Created temp file path.
 */
String TempFolder::Create()
{
	String temp = env::GetTempChildPath();
	if (!temp.empty())
	{
		m_path = temp;
	}

	return temp;
}

/**
 * @brief Delete the temporary folder, if it exists.
 * @return true if there was no error.
 */
bool TempFolder::Delete()
{
	bool success = true;
	if (!m_path.empty())
		success = ClearTempfolder(m_path);
	if (success)
		m_path = _T("");
	return success;
}

/** 
 * @brief Cleanup tempfiles created by WinMerge.
 * This function finds temp folders which don't have WinMerge instance using
 * them anymore.
 */
void CleanupWMtemp()
{
	DirItemArray dirs, files;
	DirTravel::LoadFiles(paths::GetParentPath(env::GetTemporaryPath()), &dirs, &files, String(TempFolderPrefix) + _T("*"));

	for (auto& dir : dirs)
	{
		const String foldername = dir.filename;
		// Remove leading "WM_" from filename to get the ProcessID
		const String tempfolderPID = foldername.substr(tc::tcslen(TempFolderPrefix));
		// Check if this instance of WM is still running
		try
		{
			int pid = atoi(ucr::toUTF8(tempfolderPID).c_str());
			if (!WMrunning(pid))
				ClearTempfolder(paths::ConcatPath(dir.path, foldername));
		}
		catch (...)
		{
		}
	}
}

/**
 * @brief Is WinMerge with given processID running?
 * @param [in] processIDs List of WinMerge processes.
 * @param [in] iPI ProcessID to check.
 * @return true if processID was found from the list, `false` otherwise.
 */
static bool WMrunning(int iPI)
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, false, iPI);
	if (!hProcess)
		return (GetLastError() == ERROR_ACCESS_DENIED);
	CloseHandle(hProcess);
	return true;
}

/**
 * @brief Remove the temp folder.
 * @param [in] pathName Folder to remove.
 * @return true if removal succeeds, `false` if fails.
 */
bool ClearTempfolder(const String &pathName)
{
	try
	{
		TFile(pathName).remove(true);
	}
	catch (...)
	{
		return false;
	}
	return true;
}
