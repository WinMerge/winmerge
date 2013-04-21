/**
 * @file  TempFile.cpp
 *
 * @brief Implementation file for TempFile
 *
 */
// ID line follows -- this is updated by SVN
// $Id: TempFile.cpp 6723 2009-05-09 09:13:26Z sdottaka $

#include "TempFile.h"
#include <windows.h>
#include <tlhelp32.h> 
#include <shlwapi.h>
#include "paths.h"
#include "Environment.h"
#include "Constants.h"

using std::vector;

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
 * @param [in] ext extension for temp file name.
 * @return Created temp file path.
 */
String TempFile::Create(LPCTSTR prefix, LPCTSTR ext)
{
	String temp = env_GetTempPath();
	if (temp.empty())
	{
		return TEXT("");
	}

	String pref = prefix;
	if (pref.empty())
		pref = TEXT("wmtmp");

	temp = env_GetTempFileName(temp, pref, NULL);
	if (!temp.empty())
	{
		if (ext)
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
String TempFile::CreateFromFile(LPCTSTR filepath, LPCTSTR prefix)
{
	String temp = env_GetTempPath();
	if (temp.empty())
	{
		return TEXT("");
	}

	String pref = prefix;
	if (pref.empty())
		pref = TEXT("wmtmp");

	temp = env_GetTempFileName(temp, pref, NULL);
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
	bool success = true;
	if (!m_path.empty())
		success = !!DeleteFile(m_path.c_str());
	if (success)
		m_path = TEXT("");
	return !!success;
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
	bool hRes = !!Process32First (hSnapShot, &pEntry);

	// Iterate through all processes to get
	// the ProcessIDs of all running WM instances
	while (hRes)
	{
		size_t exeFileLen = _tcslen(pEntry.szExeFile);
		if ((exeFileLen >= sizeof(ExecutableFilenameU)/sizeof(TCHAR)-1 && _tcsicmp(pEntry.szExeFile + exeFileLen - (sizeof(ExecutableFilenameU)/sizeof(TCHAR)-1), ExecutableFilenameU) == 0) ||
			(exeFileLen >= sizeof(ExecutableFilename )/sizeof(TCHAR)-1 && _tcsicmp(pEntry.szExeFile + exeFileLen - (sizeof(ExecutableFilename )/sizeof(TCHAR)-1), ExecutableFilename ) == 0))
		{
			processIDs.push_back(pEntry.th32ProcessID);
		}
		hRes = !!Process32Next (hSnapShot, &pEntry);
	}

	// Now remove temp folders that are not used.
	CleanupWMtempfolder(processIDs);
}

/** 
 * @brief Remove temp folders having process Ids in name.
 * This function removes temp folders whose name contains process ID from the
 * given list. These folders must have been earlier detected as unused.
 * @param [in] processIDs List of process IDs.
 * @return true if all temp folders were deleted, FALSE otherwise.
 */
bool CleanupWMtempfolder(vector <int> processIDs)
{
	String foldername;
	String tempfolderPID;
	String filepattern(TempFolderPrefix);
	filepattern += _T("*.*");
	String pattern = paths_GetParentPath(env_GetTempPath());
	pattern = paths_ConcatPath(pattern, filepattern);
	WIN32_FIND_DATA ff;
	HANDLE h = INVALID_HANDLE_VALUE;
	bool res = true;
	bool bok = true;

	h = FindFirstFile (pattern.c_str(), &ff);
	if (h == INVALID_HANDLE_VALUE)
		bok = FALSE;

	while (bok & res)
	{
		foldername = ff.cFileName;
		if (ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// Remove leading "WM_" from filename to get the ProcessID
			tempfolderPID = foldername.substr(_tcslen(TempFolderPrefix));

			// Check if this instance of WM is still running
			if (!WMrunning(processIDs, _ttoi (tempfolderPID.c_str())))
			{
				tempfolderPID = paths_ConcatPath(paths_GetParentPath(pattern), ff.cFileName); 
				if (res = ClearTempfolder(tempfolderPID))
				{
					if (!res)
						break;
					bok = !!FindNextFile(h, &ff) ;
				}
				continue;
			}
		}
		bok = !!FindNextFile(h, &ff) ;
	}
	if (h)
		FindClose(h);
	return res;
}

/**
 * @brief Is WinMerge with given processID running?
 * @param [in] processIDs List of WinMerge processes.
 * @param [in] iPI ProcessID to check.
 * @return true if processID was found from the list, FALSE otherwise.
 */
bool WMrunning(vector<int> processIDs, int iPI)
{
	vector<int>::iterator iter = processIDs.begin();
	while (iter != processIDs.end())
	{
		if (*iter == iPI)
			return true;
		++iter;
	}
	return FALSE;
}

/**
 * @brief Remove the temp folder.
 * @param [in] pathName Folder to remove.
 * @return true if removal succeeds, FALSE if fails.
 */
bool ClearTempfolder(const String &pathName)
{
	// SHFileOperation expects a ZZ terminated list of paths!
	String normalizedPathName = pathName;
	paths_normalize(normalizedPathName); // remove trailing slash
	const size_t pathSize = normalizedPathName.length() + 2;
	std::vector<TCHAR> path(pathSize, 0);
	memcpy(&path[0], normalizedPathName.c_str(), normalizedPathName.length() * sizeof(TCHAR));

	SHFILEOPSTRUCT fileop = {0, FO_DELETE, &path[0], 0, FOF_NOCONFIRMATION |
			FOF_SILENT | FOF_NOERRORUI, 0, 0, 0};
	SHFileOperation(&fileop);

	return true;
}
