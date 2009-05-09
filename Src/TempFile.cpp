/**
 * @file  TempFile.cpp
 *
 * @brief Implementation file for TempFile
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include <windows.h>
#include <tlhelp32.h> 
#include <tchar.h>
#include <shlwapi.h>
#include <vector>
#include "UnicodeString.h"
#include "TempFile.h"
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
 * @return Created temp file path.
 */
String TempFile::Create(LPCTSTR prefix)
{
	String temp = env_GetTempPath(NULL);
	if (temp.empty())
	{
		return TEXT("");
	}

	String pref = prefix;
	if (pref.empty())
		pref = TEXT("wmtmp");

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
		return TEXT("");
	}

	String pref = prefix;
	if (pref.empty())
		pref = TEXT("wmtmp");

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
	hSnapShot = CreateToolhelp32Snapshot (TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);

	// Get first process
	BOOL hRes = Process32First (hSnapShot, &pEntry);

	// Iterate through all processes to get
	// the ProcessIDs of all running WM instances
	while (hRes)
	{
		if ((_tcscmp(pEntry.szExeFile, ExecutableFilenameU) == 0) ||
			(_tcscmp(pEntry.szExeFile, ExecutableFilename) == 0))
		{
			processIDs.push_back(pEntry.th32ProcessID);
		}
		hRes = Process32Next (hSnapShot, &pEntry);
	}

	// Now remove temp folders that are not used.
	CleanupWMtempfolder(processIDs);
}

/** 
 * @brief Remove temp folders having process Ids in name.
 * This function removes temp folders whose name contains process ID from the
 * given list. These folders must have been earlier detected as unused.
 * @param [in] processIDs List of process IDs.
 * @return TRUE if all temp folders were deleted, FALSE otherwise.
 */
BOOL CleanupWMtempfolder(vector <int> processIDs)
{
	String foldername;
	String tempfolderPID;
	String filepattern(TempFolderPrefix);
	filepattern += _T("*.*");
	String pattern = paths_GetParentPath(env_GetTempPath(NULL));
	pattern = paths_ConcatPath(pattern, filepattern.c_str());
	WIN32_FIND_DATA ff;
	HANDLE h = INVALID_HANDLE_VALUE;
	BOOL res = TRUE;
	BOOL bok = TRUE;

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
				tempfolderPID = paths_ConcatPath(paths_GetParentPath(pattern.c_str()), ff.cFileName); 
				if (res = ClearTempfolder(tempfolderPID.c_str()))
				{
					if (!res)
						break;
					bok = FindNextFile(h, &ff) ;
				}
				continue;
			}
		}
		bok = FindNextFile(h, &ff) ;
	}
	if (h)
		FindClose(h);
	return res;
}

/**
 * @brief Is WinMerge with given processID running?
 * @param [in] processIDs List of WinMerge processes.
 * @param [in] iPI ProcessID to check.
 * @return TRUE if processID was found from the list, FALSE otherwise.
 */
BOOL WMrunning(vector<int> processIDs, int iPI)
{
	vector<int>::iterator iter = processIDs.begin();
	while (iter != processIDs.end())
	{
		if (*iter == iPI)
			return TRUE;
		iter++;
	}
	return FALSE;
}

/**
 * @brief Remove the temp folder.
 * @param [in] pathName Folder to remove.
 * @return TRUE if removal succeeds, FALSE if fails.
 */
BOOL ClearTempfolder(const String &pathName)
{
	// SHFileOperation expects a ZZ terminated list of paths!
	const int pathSize = pathName.length() + 2;
	TCHAR *path = new TCHAR[pathSize];
	ZeroMemory(path, pathSize * sizeof(TCHAR));
	_tcscpy(path, pathName.c_str());

	SHFILEOPSTRUCT fileop = {0, FO_DELETE, path, 0, FOF_NOCONFIRMATION |
			FOF_SILENT | FOF_NOERRORUI, 0, 0, 0};
	SHFileOperation(&fileop);

	delete [] path;
	return TRUE;
}
