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
 * @file  CopyHexdumpDlg.cpp
 *
 * @brief Implementation of the Hexdump copying dialog.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "offset.h"

int CopyHexdumpDlg::iCopyHexdumpMode = 0;
int CopyHexdumpDlg::iCopyHexdumpType = IDC_EXPORTDISPLAY;

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 */
BOOL CopyHexdumpDlg::OnInitDialog(HWindow *pDlg)
{
	int iCopyHexdumpDlgStart, iCopyHexdumpDlgEnd;
	if (!bSelected)
	{
		// Assume whole file is to be hexdumped. (except the last line (if incomplete))
		iCopyHexdumpDlgStart = 0;
		iCopyHexdumpDlgEnd = m_dataArray.GetUpperBound() / iBytesPerLine * iBytesPerLine;
	}
	else
	{
		// Assume selected area is to be hexdumped.
		// Make sure end of selection is greater than start of selection.
		iCopyHexdumpDlgStart = iGetStartOfSelection();
		iCopyHexdumpDlgEnd = iGetEndOfSelection();
	}
	TCHAR buf[16];
	_stprintf(buf, _T("0x%x"), iCopyHexdumpDlgStart);
	pDlg->SetDlgItemText(IDC_HEXDUMP_OFFSET, buf);
	_stprintf(buf, _T("0x%x"), iCopyHexdumpDlgEnd);
	pDlg->SetDlgItemText(IDC_HEXDUMP_OFFSET2, buf);
	pDlg->CheckDlgButton(iCopyHexdumpMode ? IDC_HEXDUMP_EXPORTCLIPB :
			IDC_HEXDUMP_EXPORTFILE, BST_CHECKED);
	pDlg->CheckDlgButton(iCopyHexdumpType, BST_CHECKED);
	return TRUE;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Hanle to the dialog.
 * @param [in] wParam The command to handle.
 * @param [in] lParam Optional parameter for the command.
 * @return TRUE if the command was handled, FALSE otherwise.
 */
BOOL CopyHexdumpDlg::OnCommand(HWindow *pDlg, WPARAM wParam, LPARAM lParam)
{
	int iCopyHexdumpDlgStart, iCopyHexdumpDlgEnd;
	const int bufSize = 16;
	TCHAR buf[bufSize + 1];
	switch (wParam)
	{
	case IDOK:
		if (pDlg->GetDlgItemText(IDC_HEXDUMP_OFFSET, buf, bufSize) &&
			offset_parse(buf, iCopyHexdumpDlgStart) &&
			pDlg->GetDlgItemText(IDC_HEXDUMP_OFFSET2, buf, bufSize) &&
			offset_parse(buf, iCopyHexdumpDlgEnd))
		{
			iCopyHexdumpMode = pDlg->IsDlgButtonChecked(IDC_HEXDUMP_EXPORTCLIPB);
			if (pDlg->IsDlgButtonChecked(IDC_EXPORTDISPLAY))
				iCopyHexdumpType = IDC_EXPORTDISPLAY;
			else if (pDlg->IsDlgButtonChecked(IDC_EXPORTDIGITS))
				iCopyHexdumpType = IDC_EXPORTDIGITS;
			else if (pDlg->IsDlgButtonChecked(IDC_EXPORTRTF))
				iCopyHexdumpType = IDC_EXPORTRTF;

			pDlg->EndDialog(wParam);
			WaitCursor w1;
			CMD_copy_hexdump(iCopyHexdumpMode, iCopyHexdumpType, iCopyHexdumpDlgStart, iCopyHexdumpDlgEnd);
			return TRUE;
		}
		break;
	case IDCANCEL:
		pDlg->EndDialog(wParam);
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
INT_PTR CopyHexdumpDlg::DlgProc(HWindow *pDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
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
