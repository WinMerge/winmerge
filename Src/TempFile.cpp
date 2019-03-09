/**
 * @file  TempFile.cpp
 *
 * @brief Implementation file for TempFile
 *
 */

#include "pch.h"
#include "TempFile.h"
#include <windows.h>
#include <tlhelp32.h> 
#include "paths.h"
#include "TFile.h"
#include "Environment.h"
#include "Constants.h"
#include "unicoder.h"

using std::vector;

static bool CleanupWMtempfolder(const vector <int>& processIDs);
static bool WMrunning(const vector<int>& processIDs, int iPI);

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
				temp = tempext;
		}
		m_path = temp;
	}

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
String TempFile::CreateFromFile(const String& filepath, const String& prefix)
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
		// Scratchpads don't have a file to copy.
		m_path = temp;

		if (::CopyFileW(TFile(filepath).wpath().c_str(), temp.c_str(), FALSE))
		{
			::SetFileAttributes(temp.c_str(), FILE_ATTRIBUTE_NORMAL);
		}
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
	vector<int> processIDs;

	// Get the snapshot of the system
	HANDLE hSnapShot;
	hSnapShot = CreateToolhelp32Snapshot (TH32CS_SNAPALL, 0);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);

	// Get first process
	bool bRes = !!Process32First (hSnapShot, &pEntry);

	// Iterate through all processes to get
	// the ProcessIDs of all running WM instances
	while (bRes)
	{
		size_t exeFileLen = _tcslen(pEntry.szExeFile);
		if ((exeFileLen >= sizeof(ExecutableFilenameU)/sizeof(TCHAR)-1 && _tcsicmp(pEntry.szExeFile + exeFileLen - (sizeof(ExecutableFilenameU)/sizeof(TCHAR)-1), ExecutableFilenameU) == 0) ||
			(exeFileLen >= sizeof(ExecutableFilename )/sizeof(TCHAR)-1 && _tcsicmp(pEntry.szExeFile + exeFileLen - (sizeof(ExecutableFilename )/sizeof(TCHAR)-1), ExecutableFilename ) == 0))
		{
			processIDs.push_back(pEntry.th32ProcessID);
		}
		bRes = !!Process32Next (hSnapShot, &pEntry);
	}

	// Now remove temp folders that are not used.
	CleanupWMtempfolder(processIDs);
}

/** 
 * @brief Remove temp folders having process Ids in name.
 * This function removes temp folders whose name contains process ID from the
 * given list. These folders must have been earlier detected as unused.
 * @param [in] processIDs List of process IDs.
 * @return `true` if all temp folders were deleted, `false` otherwise.
 */
static bool CleanupWMtempfolder(const vector <int>& processIDs)
{
	String foldername;
	String tempfolderPID;
	String filepattern(TempFolderPrefix);
	filepattern += _T("*.*");
	String pattern = paths::GetParentPath(env::GetTemporaryPath());
	pattern = paths::ConcatPath(pattern, filepattern);
	WIN32_FIND_DATA ff;
	HANDLE h;
	bool res = true;
	bool bok = true;

	
	h = FindFirstFile (TFile(pattern).wpath().c_str(), &ff);
	if (h == INVALID_HANDLE_VALUE)
		bok = false;

	while (bok & res)
	{
		foldername = ff.cFileName;
		if (ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// Remove leading "WM_" from filename to get the ProcessID
			tempfolderPID = foldername.substr(_tcslen(TempFolderPrefix));

			// Check if this instance of WM is still running
			try
			{
				int pid = atoi(ucr::toUTF8(tempfolderPID).c_str());
				if (!WMrunning(processIDs, pid))
				{
					tempfolderPID = paths::ConcatPath(paths::GetParentPath(pattern), ff.cFileName); 
					res = ClearTempfolder(tempfolderPID);
					if (res)
						bok = !!FindNextFile(h, &ff) ;
					continue;
				}
			}
			catch (...)
			{
			}
		}
		bok = !!FindNextFile(h, &ff) ;
	}
	if (h != INVALID_HANDLE_VALUE)
		FindClose(h);
	return res;
}

/**
 * @brief Is WinMerge with given processID running?
 * @param [in] processIDs List of WinMerge processes.
 * @param [in] iPI ProcessID to check.
 * @return true if processID was found from the list, `false` otherwise.
 */
static bool WMrunning(const vector<int>& processIDs, int iPI)
{
	return std::find(processIDs.begin(), processIDs.end(), iPI) != processIDs.end();
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
