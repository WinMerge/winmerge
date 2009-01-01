/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  AboutDlg.cpp
 *
 * @brief Implementation of the About dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: AboutDlg.cpp 189 2008-12-04 22:37:04Z kimmov $

#include "precomp.h"
#include "resource.h"
#include "Constants.h"
#include "StringTable.h"
#include "hexwnd.h"
#include "hexwdlg.h"

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 */
BOOL AboutDlg::OnInitDialog(HWND hDlg)
{
	// Set the version information.
	PString<4096> buf;
	buf->Format(S.AboutFrhed,
		FRHED_MAJOR_VERSION, FRHED_MINOR_VERSION, FRHED_SUB_RELEASE_NO, FRHED_BUILD_NO);
	SetDlgItemText(hDlg, IDC_ABOUT_VER, buf);
	// Set the homepage URL.
	SetDlgItemText(hDlg, IDC_ABOUT_URL, FrhedHomepageURL);
	// Set the icon.
	if (HWND hwndParent = GetParent(hDlg))
		if (DWORD dwIcon = GetClassLong(hwndParent, GCLP_HICON))
			SendDlgItemMessage(hDlg, IDC_APPICON, STM_SETICON, dwIcon, 0);
	return TRUE;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Hanle to the dialog.
 * @param [in] wParam The command to handle.
 * @param [in] lParam Optional parameter for the command.
 * @return TRUE if the command was handled, FALSE otherwise.
 */
BOOL AboutDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case IDOK:
	case IDCANCEL:
		EndDialog(hDlg, wParam);
		return TRUE;

	case IDC_ABOUT_OPENURL:
		{
			HINSTANCE hi = ShellExecute(hDlg, _T("open"), FrhedHomepageURL, NULL, NULL, SW_SHOWNORMAL);
			if ((UINT)hi <= HINSTANCE_ERROR)
				MessageBox(hDlg, _T("Could not call browser."), _T("Go to homepage"), MB_ICONERROR);
		}
		return TRUE;

	case IDC_ABOUTCONTRIBS:
		{
			TCHAR contrList[MAX_PATH] = {0};
			GetModuleFileName(0, contrList, MAX_PATH);
			PathRemoveFileSpec(contrList);
			PathAppend(contrList, ContributorsList);
			HINSTANCE hi = ShellExecute(hDlg, _T("open"), contrList, NULL, NULL, SW_SHOWNORMAL);
			if ((UINT)hi <= HINSTANCE_ERROR)
			{
				TCHAR buf[4096] = {0};
				_sntprintf(buf, RTL_NUMBER_OF(buf), _T("File\n%s\nnot found!"), ContributorsList);
				MessageBox(hDlg, buf, _T("Frhed"), MB_ICONERROR);
			}
		}
		return TRUE;
	}
	return FALSE;
}

/**
 * @brief Handle dialog messages.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg The message.
 * @param [in] wParam The command in the message.
 * @param [in] lParam The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
 */
INT_PTR AboutDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hDlg);

	case WM_COMMAND:
		return OnCommand(hDlg, wParam, lParam);
	}
	return FALSE;
}
