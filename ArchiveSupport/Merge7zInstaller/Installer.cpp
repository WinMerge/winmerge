/* Installer.cpp: Merge7z plugin installer
 * Copyright (c) 2005 Jochen Tucht
 *
 * License:	This program is free software; you can redistribute it and/or modify
 *			it under the terms of the GNU General Public License as published by
 *			the Free Software Foundation; either version 2 of the License, or
 *			(at your option) any later version.
 *
 *			This program is distributed in the hope that it will be useful,
 *			but WITHOUT ANY WARRANTY; without even the implied warranty of
 *			MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *			GNU General Public License for more details.
 *
 *			You should have received a copy of the GNU General Public License
 *			along with this program; if not, write to the Free Software
 *			Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Remarks:	Bundles plugins for all supported versions of 7-Zip in a single
 *			installer. Also includes essential components from latest 7-Zip
 *			stable release for optional standalone operation.
 *			Files to be installed are embedded as resources (see Files.rc2).
 *			The resulting exe must be run through UPX (upx.sourceforge.net)
 *			to reduce size.

Please mind 2. a) of the GNU General Public License, and log your changes below.

DATE:		BY:					DESCRIPTION:
==========	==================	================================================
2005/01/15	Jochen Tucht		Created
*/

#include <windows.h>

#pragma intrinsic(memset) // do not depend on CRT

// Compute dwBuild from revision.txt
static const DWORD dwBuild =
(
	sizeof""
#	define VERSION(MAJOR,MINOR)
#	include "../Merge7z/revision.txt"
#	undef VERSION
);

const SYSTEMTIME *st = NULL; // initialized from SYSTEMTIME RCDATA in Files.rc2

void InstallFile(HWND hWnd, LPTSTR lpName, LPCTSTR lpType, LPTSTR path, int cchPath)
{
	HMODULE hModule = GetModuleHandle(0);
	HRSRC hResource = FindResource(hModule, lpName, lpType);
	DWORD dwSize = SizeofResource(hModule, hResource);
	LPVOID pResource = LoadResource(hModule, hResource);
	LPTSTR name = path + cchPath;
	if (name != lpName)
	{
		lstrcpy(name, lpName);
	}
	int cchName = lstrlen(name);
	int i;
	for (i = 0 ; i < cchName ; ++i)
	{
		if (name[i] == '/')
		{
			name[i] = '\0';
			CreateDirectory(path, 0);
			name[i] = '\\';
		}
	}
	HANDLE hFile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, 0);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwNumberOfBytesWritten;
		BOOL bSuccess = WriteFile(hFile, pResource, dwSize, &dwNumberOfBytesWritten, 0);
		FILETIME ft;
		if (SystemTimeToFileTime(st, &ft))
		{
			SetFileTime(hFile, &ft, &ft, &ft);
		}
		CloseHandle(hFile);
		if (!bSuccess || dwNumberOfBytesWritten != dwSize)
		{
			hFile = INVALID_HANDLE_VALUE;
		}
	}
	if (hFile == INVALID_HANDLE_VALUE)
	{
		LONG error = GetLastError();
		name[cchName++] = '\n';
		FormatMessage
		(
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
			name + cchName, MAX_PATH, NULL
		);
		int response = MessageBox(hWnd, path, 0, MB_ICONSTOP|MB_OKCANCEL);
		if (response == IDCANCEL)
		{
			ExitProcess(1);
		}
	}
}

BOOL CALLBACK fnPopulateList(HMODULE hModule, LPCTSTR lpType, LPTSTR lpName, LONG lParam)
{
	TCHAR acName[MAX_PATH];
	if (ATOM aName = FindAtom(lpName))
	{
		GetAtomName(aName, lpName = acName, sizeof acName);
	}
	SendDlgItemMessage((HWND)lParam, 100, LB_ADDSTRING, 0, (LPARAM)lpName);
	return TRUE;
}

BOOL CALLBACK fnInstallFiles(HMODULE hModule, LPCTSTR lpType, LPTSTR lpName, LONG lParam)
{
	HWND hWnd = (HWND)lParam;
	TCHAR path[8 * MAX_PATH];
	int cchPath = GetDlgItemText(hWnd, 203, path, 5 * MAX_PATH);
	int cchName = GetFileTitle(path, 0, 0);
	if (cchName < cchPath)
	{
		cchPath -= cchName;
	}
	if (cchPath != 3 || path[1] != ':')
	{
		path[cchPath++] = '\\';
	}
	TCHAR acName[MAX_PATH];
	if (ATOM aName = FindAtom(lpName))
	{
		GetAtomName(aName, lpName = acName, sizeof acName);
	}
	InstallFile(hWnd, lpName, lpType, path, cchPath);
	return TRUE;
}

BOOL CALLBACK DlgMain_InitDialog(HWND hWnd, LPARAM lParam)
{
	TCHAR path[5 * MAX_PATH];
	wsprintf(path + GetWindowText(hWnd, path, MAX_PATH), " (dllbuild %04lu, %04u-%02u-%02u)",
		dwBuild, (UINT)st->wYear, (UINT)st->wMonth, (UINT)st->wDay);
	SetWindowText(hWnd, path);
	EnumResourceNames(GetModuleHandle(0), "Merge7z", fnPopulateList, (LONG)hWnd);
	LONG lCount = SendDlgItemMessage(hWnd, 100, LB_GETCOUNT , 0, 0);
	SendDlgItemMessage(hWnd, 100, LB_SELITEMRANGEEX, 0, lCount - 1);
	CheckRadioButton(hWnd, 201, 202, 201);
	GetModuleFileName(0, path, sizeof path);
	SetDlgItemText(hWnd, 203, path);
	return TRUE;
}

BOOL CALLBACK DlgMain_BrowseExe(HWND hWnd)
{
	struct
	{
		OPENFILENAME ofn;
		TCHAR buffer[5 * MAX_PATH];
	} path;
	ZeroMemory(&path, sizeof path);
	path.ofn.lStructSize = sizeof path.ofn;
	path.ofn.hwndOwner = hWnd;
	path.ofn.lpstrFile = path.buffer;
	path.ofn.nMaxFile = sizeof path.buffer;
	path.ofn.lpstrFilter = "*.exe\0*.exe\0";
	path.ofn.lpstrTitle = "Browse for application ...";
	path.ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_PATHMUSTEXIST;
	GetDlgItemText(hWnd, 203, path.buffer, sizeof path.buffer);
	if (GetOpenFileName(&path.ofn))
	{
		SetDlgItemText(hWnd, 203, path.buffer);
	}
	return FALSE;
}

BOOL CALLBACK DlgMain_InstallFiles(HWND hWnd)
{
	HCURSOR hCursor = SetCursor(LoadCursor(0, IDC_WAIT));
	HINSTANCE hModule = GetModuleHandle(0);
	int count = SendDlgItemMessage(hWnd, 100, LB_GETCOUNT , 0, 0);
	int index = 0;
	TCHAR path[8 * MAX_PATH];
	int cchPath;
	if (IsDlgButtonChecked(hWnd, 201))
	{
		cchPath = GetSystemDirectory(path, 4 * MAX_PATH);
	}
	else
	{
		cchPath = GetDlgItemText(hWnd, 203, path, 5 * MAX_PATH);
		int cchName = GetFileTitle(path, 0, 0);
		if (cchName < cchPath)
		{
			cchPath -= cchName;
		}
	}
	if (cchPath != 3 || path[1] != ':')
	{
		path[cchPath++] = '\\';
	}
	LPTSTR name = path + cchPath;
	while (index < count)
	{
		if (SendDlgItemMessage(hWnd, 100, LB_GETSEL, index, 0))
		{
			int cchName = SendDlgItemMessage(hWnd, 100, LB_GETTEXT, index, (LPARAM)name);
			InstallFile(hWnd, name, "Merge7z", path, cchPath);
		}
		++index;
	}
	if (IsDlgButtonChecked(hWnd, 205))
	{
		EnumResourceNames(GetModuleHandle(0), "7-ZIP", fnInstallFiles, (LONG)hWnd);
	}
	SetCursor(hCursor);
	return TRUE;
}

BOOL CALLBACK DlgMain_EnableStandalone(HWND hWnd)
{
	if (IsDlgButtonChecked(hWnd, 205))
	{
		int lower = SendDlgItemMessage(hWnd, 100, LB_FINDSTRINGEXACT, -1, (LPARAM)"Merge7z313.dll");
		int upper = SendDlgItemMessage(hWnd, 100, LB_FINDSTRINGEXACT, -1, (LPARAM)"Merge7z313U.dll");
		SendDlgItemMessage(hWnd, 100, LB_SELITEMRANGEEX, lower, upper);
	}
	return TRUE;
}

BOOL CALLBACK DlgMain(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		return DlgMain_InitDialog(hWnd, lParam);
	case WM_COMMAND:
		switch (wParam)
		{
		case 201:
			CheckDlgButton(hWnd, 205, 0);
			//fall through
		case 202:
			EnableWindow(GetDlgItem(hWnd, 203), wParam == 202);
			EnableWindow(GetDlgItem(hWnd, 204), wParam == 202);
			EnableWindow(GetDlgItem(hWnd, 205), wParam == 202);
			break;
		case 204:
			return DlgMain_BrowseExe(hWnd);
		case 205:
			if (IsDlgButtonChecked(hWnd, 205))
				SendDlgItemMessage(hWnd, 100, LB_SETSEL, 0, -1);
			//fall through
		case MAKELONG(100, LBN_SELCHANGE):
			return DlgMain_EnableStandalone(hWnd);
		case IDOK: if (DlgMain_InstallFiles(hWnd)) case IDCANCEL:
			EndDialog(hWnd, wParam);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

void WinMainCRTStartup(void)
{
	HINSTANCE hModule = GetModuleHandle(0);
	HRSRC hResource = FindResource(hModule, "SYSTEMTIME", RT_RCDATA);
	st = (SYSTEMTIME *)LoadResource(hModule, hResource);
	InitAtomTable(0x3001);
#	define IMPORT(name) AddAtom(#name); /##/
#	include "files.rc2"
	DialogBoxParam(hModule, MAKEINTRESOURCE(100), 0, DlgMain, 0);
	ExitProcess(0);
}
