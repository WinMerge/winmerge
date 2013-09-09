/*
Frhed - Free hex editor
Copyright (C) 2000 Raihan Kibria

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

Last change: 2013-02-24 by Jochen Neubeck
*/
/** 
 * @file  AboutDlg.cpp
 *
 * @brief Implementation of the About dialog.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "Constants.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "StringTable.h"

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 */
BOOL AboutDlg::OnInitDialog(HWindow *pDlg)
{
	// Set the version information.
	TCHAR buf[4096];
	buf[RTL_NUMBER_OF(buf) - 1] = _T('\0');
	_sntprintf(buf, RTL_NUMBER_OF(buf) - 1, GetLangString(IDS_ABOUTFRHEDVER),
		FRHED_MAJOR_VERSION, FRHED_MINOR_VERSION, FRHED_SUB_RELEASE_NO);
	pDlg->SetDlgItemText(IDC_ABOUT_VER, buf);
	// Set the homepage URL.
	pDlg->SetDlgItemText(IDC_ABOUT_URL, FrhedHomepageURL);
	// Set the icon.
	if (HWindow *pwndParent = pDlg->GetParent())
		if (DWORD_PTR dwIcon = pwndParent->GetClassLongPtr(GCLP_HICON))
			pDlg->SendDlgItemMessage(IDC_APPICON, STM_SETICON, dwIcon, 0);
	return TRUE;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Hanle to the dialog.
 * @param [in] wParam The command to handle.
 * @param [in] lParam Optional parameter for the command.
 * @return TRUE if the command was handled, FALSE otherwise.
 */
BOOL AboutDlg::OnCommand(HWindow *pDlg, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case IDOK:
	case IDCANCEL:
		pDlg->EndDialog(wParam);
		return TRUE;

	case IDC_ABOUT_OPENURL:
		{
			HINSTANCE hi = ShellExecute(pDlg->m_hWnd,
				_T("open"), FrhedHomepageURL, 0, NULL, SW_SHOWNORMAL);
			if ((UINT)hi <= HINSTANCE_ERROR)
			{
				MessageBox(pDlg, GetLangString(IDS_ABOUT_BROWSER_ERR), MB_ICONERROR);
			}
		}
		return TRUE;

	case IDC_ABOUTCONTRIBS:
		{
			TCHAR contrList[MAX_PATH];
			GetModuleFileName(NULL, contrList, MAX_PATH);
			PathRemoveFileSpec(contrList);
			PathAppend(contrList, ContributorsList);
			if (PathFileExists(contrList))
			{
				ShellExecute(pDlg->m_hWnd, _T("open"), _T("notepad.exe"), contrList, NULL, SW_SHOWNORMAL);
			}
			else
			{
				TCHAR buf[4096];
				_sntprintf(buf, RTL_NUMBER_OF(buf), GetLangString(IDS_ABOUT_FILENOTFOUND), ContributorsList);
				MessageBox(pDlg, buf, MB_ICONERROR);
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
INT_PTR AboutDlg::DlgProc(HWindow *pDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(pDlg);

	case WM_COMMAND:
		return OnCommand(pDlg, wParam, lParam);

	case WM_HELP:
		OnHelp(pDlg);
		break;
	}
	return FALSE;
}
