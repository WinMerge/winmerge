/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  FileOrFolderSelect.cpp
 *
 * @brief Implementation of the file and folder selection routines.
 */

#include "pch.h"
#include <windows.h>
#include "FileOrFolderSelect.h"
#pragma warning (push)			// prevent "warning C4091: 'typedef ': ignored on left of 'tagGPFIDL_FLAGS' when no variable is declared"
#pragma warning (disable:4091)	// VC bug when using XP enabled toolsets.
#include <shlobj.h>
#pragma warning (pop)
#include <sys/stat.h>
#include "Environment.h"
#include "paths.h"
#include "MergeApp.h"

static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam,
		LPARAM lpData);
static void ConvertFilter(tchar_t* filterStr);

/** @brief Last selected folder for folder selection dialog. */
static String LastSelectedFolder;

/**
 * @brief Helper function for selecting folder or file.
 * This function shows standard Windows file selection dialog for selecting
 * file or folder to open or file to save. The last parameter @p is_open selects
 * between open or save modes. Biggest difference is that in save-mode Windows
 * asks if user wants to override existing file.
 * @param [in] parent Handle to parent window. Can be `nullptr`, but then
 *     CMainFrame is used which can cause modality problems.
 * @param [out] path Selected path is returned in this string
 * @param [in] initialPath Initial path (and file) shown when dialog is opened
 * @param [in] titleid Resource string ID for dialog title.
 * @param [in] filterid 0 or STRING ID for filter string
 *     - 0 means "All files (*.*)". Note the string formatting!
 * @param [in] is_open Selects Open/Save -dialog (mode).
 * @note Be careful when setting @p parent to `nullptr` as there are potential
 * modality problems with this. Dialog can be lost behind other windows!
 * @param [in] defaultExtension Extension to append if user doesn't provide one
 */
bool SelectFile(HWND parent, String& path, bool is_open /*= true*/,
		const tchar_t* initialPath /*= nullptr*/, const String& stitle /*=_T("")*/,
		const String& sfilter /*=_T("")*/, const tchar_t* defaultExtension /*= nullptr*/)
{
	path.clear(); // Clear output param

	// This will tell common file dialog what to show
	// and also this will hold its return value
	tchar_t sSelectedFile[MAX_PATH_FULL] = {0};
	String sInitialDir;

	// check if specified path is a file
	if (initialPath && initialPath[0])
	{
		// If initial path info includes a file
		// we put the bare filename into sSelectedFile
		// so the common file dialog will start up with that file selected
		if (paths::DoesPathExist(initialPath) == paths::IS_EXISTING_FILE)
		{
			String temp;
			paths::SplitFilename(initialPath, 0, &temp, 0);
			lstrcpy(sSelectedFile, temp.c_str());
			sInitialDir = paths::GetParentPath(initialPath);
		}
		else
		{
			sInitialDir = initialPath;
		}
	}

	String filters = sfilter, title = stitle;
	if (sfilter.empty())
		filters = _("All Files (*.*)|*.*||");
	if (stitle.empty())
	{
		title = is_open ? _("Open") : _("Save As");
	}

	// Convert extension mask from MFC style separators ('|')
	//  to Win32 style separators ('\0')
	tchar_t* filtersStr = &*filters.begin();
	ConvertFilter(filtersStr);

	OPENFILENAME_NT4 ofn = { sizeof OPENFILENAME_NT4 };
	ofn.hwndOwner = parent;
	ofn.lpstrFilter = filtersStr;
	ofn.lpstrCustomFilter = nullptr;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = sSelectedFile;
	ofn.nMaxFile = MAX_PATH_FULL;
	ofn.lpstrInitialDir = sInitialDir.empty() ? nullptr : sInitialDir.c_str();
	ofn.lpstrTitle = title.c_str();
	ofn.lpstrFileTitle = nullptr;
	if (defaultExtension)
		ofn.lpstrDefExt = defaultExtension;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

	bool bRetVal = false;
	if (is_open)
		bRetVal = !!GetOpenFileName((OPENFILENAME *)&ofn);
	else
		bRetVal = !!GetSaveFileName((OPENFILENAME *)&ofn);
	// common file dialog populated sSelectedFile variable's buffer

	if (bRetVal)
		path = sSelectedFile;
	
	return bRetVal;
}

/** 
 * @brief Helper function for selecting directory
 * @param [out] path Selected path is returned in this string
 * @param [in] root_path Initial path shown when dialog is opened
 * @param [in] titleid Resource string ID for dialog title.
 * @param [in] hwndOwner Handle to owner window or `nullptr`
 * @return `true` if valid folder selected (not cancelled)
 */
bool SelectFolder(String& path, const tchar_t* root_path /*= nullptr*/, 
			const String& stitle /*=_T("")*/, 
			HWND hwndOwner /*= nullptr*/) 
{
	BROWSEINFO bi;
	LPITEMIDLIST pidl;
	tchar_t szPath[MAX_PATH_FULL] = {0};
	bool bRet = false;
	String title = stitle;
	if (root_path == nullptr)
		LastSelectedFolder.clear();
	else
		LastSelectedFolder = root_path;

	bi.hwndOwner = hwndOwner;
	bi.pidlRoot = nullptr;  // Start from desktop folder
	bi.pszDisplayName = szPath;
	bi.lpszTitle = title.c_str();
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI | BIF_VALIDATE;
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = (LPARAM)root_path;

	pidl = SHBrowseForFolder(&bi);
	if (pidl != nullptr)
	{
		if (SHGetPathFromIDList(pidl, szPath))
		{
			path = szPath;
			bRet = true;
		}
		CoTaskMemFree(pidl);
	}
	return bRet;
}

/**
 * @brief Callback function for setting selected folder for folder dialog.
 */
static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam,
		LPARAM lpData)
{
	// Look for BFFM_INITIALIZED
	if (uMsg == BFFM_INITIALIZED)
	{
		if (lpData != NULL)
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		else
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)LastSelectedFolder.c_str());
	}
	else if (uMsg == BFFM_VALIDATEFAILED)
	{
		String strMessage = 
			strutils::format_string1(_("%1 does not exist. Do you want to create it?"), (tchar_t *)lParam);
		int answer = MessageBox(hwnd, strMessage.c_str(), nullptr, MB_YESNO);
		if (answer == IDYES)
		{
			if (!paths::CreateIfNeeded((tchar_t*)lParam))
			{
				MessageBox(hwnd, _("Failed to create folder.").c_str(), nullptr, MB_OK | MB_ICONWARNING);
			}
		}
		return 1;
	}
	return 0;
}

/** 
 * @brief Shows file/folder selection dialog.
 *
 * We need this custom function so we can select files and folders with the
 * same dialog.
 * - If existing filename is selected return it
 * - If filename in (CFileDialog) editbox and current folder doesn't form
 * a valid path to file, return current folder.
 * @param [in] parent Handle to parent window. Can be `nullptr`, but then
 *     CMainFrame is used which can cause modality problems.
 * @param [out] path Selected folder/filename
 * @param [in] initialPath Initial file or folder shown/selected.
 * @return `true` if user choosed a file/folder, `false` if user canceled dialog.
 */
bool SelectFileOrFolder(HWND parent, String& path, const tchar_t* initialPath /*= nullptr*/)
{
	String title = _("Open");

	// This will tell common file dialog what to show
	// and also this will hold its return value
	tchar_t sSelectedFile[MAX_PATH_FULL];

	// check if specified path is a file
	if (initialPath!=nullptr && initialPath[0] != '\0')
	{
		// If initial path info includes a file
		// we put the bare filename into sSelectedFile
		// so the common file dialog will start up with that file selected
		if (paths::DoesPathExist(initialPath) == paths::IS_EXISTING_FILE)
		{
			String temp;
			paths::SplitFilename(initialPath, 0, &temp, 0);
			lstrcpy(sSelectedFile, temp.c_str());
		}
	}

	String filters = _("All Files (*.*)|*.*||");

	// Convert extension mask from MFC style separators ('|')
	//  to Win32 style separators ('\0')
	tchar_t* filtersStr = &*filters.begin();
	ConvertFilter(filtersStr);

	String dirSelTag = _("Folder Selection");

	// Set initial filename to folder selection tag
	dirSelTag += _T("."); // Treat it as filename
	lstrcpy(sSelectedFile, dirSelTag.c_str()); // What is assignment above good for?

	OPENFILENAME_NT4 ofn = { sizeof OPENFILENAME_NT4 };
	ofn.hwndOwner = parent;
	ofn.lpstrFilter = filtersStr;
	ofn.lpstrCustomFilter = nullptr;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = sSelectedFile;
	ofn.nMaxFile = MAX_PATH_FULL;
	ofn.lpstrInitialDir = initialPath;
	ofn.lpstrTitle = title.c_str();
	ofn.lpstrFileTitle = nullptr;
	ofn.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOTESTFILECREATE | OFN_NOCHANGEDIR;

	bool bRetVal = !!GetOpenFileName((OPENFILENAME *)&ofn);

	if (bRetVal)
	{
		path = sSelectedFile;
		if (paths::DoesPathExist(path) == paths::DOES_NOT_EXIST)
		{
			// We have a valid folder name, but propably garbage as a filename.
			// Return folder name
			String folder = paths::GetPathOnly(sSelectedFile);
			path = paths::AddTrailingSlash(folder);
		}
	}
	return bRetVal;
}


/** 
 * @brief Helper function for converting filter format.
 *
 * MFC functions separate filter strings with | char which is also
 * good choice to safe into resource. But WinAPI32 functions we use
 * needs '\0' as separator. This function replaces '|'s with '\0's.
 *
 * @param [in,out] filterStr
 * - in Mask string to convert
 * - out Converted string
 */
static void ConvertFilter(tchar_t* filterStr)
{
	tchar_t *ch;
	while ( (ch = tc::tcschr(filterStr, '|')) != nullptr)
	{
		filterStr = ch + 1;
		*ch = '\0';
	}
}
