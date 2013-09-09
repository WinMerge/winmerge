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
 * @file  PasteDlg.cpp
 *
 * @brief Implementation of the Paste dialog.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "BinTrans.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "StringTable.h"

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to dialog.
 * @return TRUE.
 */
BOOL PasteDlg::OnInitDialog(HWindow *pDlg)
{
	if (bSelected) // iPasteMode = 0
	{
		EnableDlgItem(pDlg, IDC_PASTE_OVERWRITE, FALSE);
		EnableDlgItem(pDlg, IDC_PASTE_INSERT, FALSE);
	}
	else if (bInsertMode) // iPasteMode = 2
	{
		pDlg->CheckDlgButton(IDC_PASTE_INSERT, BST_CHECKED);
	}
	else // iPasteMode = 1
	{
		pDlg->CheckDlgButton(IDC_PASTE_OVERWRITE, BST_CHECKED);
	}
	pDlg->SendDlgItemMessage(IDC_PASTE_CLIPBOARD, WM_PASTE);
	pDlg->SetDlgItemInt(IDC_PASTE_TIMES, iPasteTimes);
	pDlg->SetDlgItemInt(IDC_PASTE_SKIPBYTES, iPasteSkip);
	if (bPasteAsText)
		pDlg->CheckDlgButton(IDC_PASTE_BINARY, BST_CHECKED);
	else
		pDlg->CheckDlgButton(IDC_PASTE_BINARY, BST_UNCHECKED);

	// Limit edit text lengths
	pDlg->SendDlgItemMessage(IDC_PASTE_TIMES, EM_SETLIMITTEXT, 16, 0);
	pDlg->SendDlgItemMessage(IDC_PASTE_SKIPBYTES, EM_SETLIMITTEXT, 16, 0);

	return TRUE;
}

/**
 * @brief Paste the bytes.
 * @param [in] hDlg Handle to dialog.
 * @return TRUE if paste succeeded, FALSE if failed.
 */
BOOL PasteDlg::Apply(HWindow *pDlg)
{
	bPasteAsText = pDlg->IsDlgButtonChecked(IDC_PASTE_BINARY) == BST_CHECKED;
	iPasteTimes = pDlg->GetDlgItemInt(IDC_PASTE_TIMES);
	if (iPasteTimes <= 0)
	{
		MessageBox(pDlg, GetLangString(IDS_PASTE_ATLEAST_ONCE), MB_ICONERROR);
		return FALSE;
	}
	iPasteSkip = pDlg->GetDlgItemInt(IDC_PASTE_SKIPBYTES);
	HEdit *pwndEdit1 = static_cast<HEdit *>(pDlg->GetDlgItem(IDC_PASTE_CLIPBOARD));
	int destlen = pwndEdit1->GetWindowTextLength() + 1;
	char *pcPastestring = new char[destlen];
	destlen = pwndEdit1->GetWindowTextA(pcPastestring, destlen);
	if (!bPasteAsText)
	{
		char *pc = 0;
		destlen = create_bc_translation((BYTE **)&pc, pcPastestring,
			static_cast<int>(strlen(pcPastestring)), iCharacterSet, iBinaryMode);
		delete [] pcPastestring;
		pcPastestring = pc;
	}
	if (destlen == 0)
	{
		MessageBox(pDlg, GetLangString(IDS_PASTE_WAS_EMPTY), MB_ICONERROR);
		delete [] pcPastestring;
		return FALSE;
	}
	WaitCursor wc1;
	if (bSelected || pDlg->IsDlgButtonChecked(IDC_PASTE_INSERT))
	{
		// Insert at iCurByte. Bytes there will be pushed up.
		if (bSelected)
		{
			iCurByte = iGetStartOfSelection();
			int iEndByte = iGetEndOfSelection();
			m_dataArray.RemoveAt(iCurByte, iEndByte - iCurByte + 1);//Remove extraneous data
			bSelected = false; // Deselect
		}
		int i = iCurByte;
		for (int k = 0 ; k < iPasteTimes ; k++)
		{
			if (!m_dataArray.InsertAtGrow(i, (BYTE*)pcPastestring, 0, destlen))
			{
				MessageBox(pDlg, GetLangString(IDS_PASTE_NO_MEM), MB_ICONERROR);
				break;
			}
			i += destlen + iPasteSkip;
		}
		iFileChanged = TRUE;
		bFilestatusChanged = true;
		resize_window();
	}
	else
	{
		// Overwrite.
		// Enough space for writing?
		// m_dataArray.GetLength()-iCurByte = number of bytes from including curbyte to end.
		if (m_dataArray.GetLength() - iCurByte < (iPasteSkip + destlen) * iPasteTimes)
		{
			MessageBox(pDlg, GetLangString(IDS_PASTE_NO_SPACE), MB_ICONERROR);
			delete [] pcPastestring;
			return TRUE;
		}
		// Overwrite data.
		for (int k = 0 ; k < iPasteTimes ; k++)
		{
			for (int i = 0 ; i < destlen ; i++)
			{
				m_dataArray[iCurByte + k * (iPasteSkip + destlen) + i] = pcPastestring[i];
			}
		}
		iFileChanged = TRUE;
		bFilestatusChanged = true;
		repaint();
	}
	delete [] pcPastestring;
	return TRUE;
}

/**
 * @brief Dialog control message handler.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] wParam Control ID.
 */
BOOL PasteDlg::OnCommand(HWindow *pDlg, WPARAM wParam, LPARAM)
{
	switch (wParam)
	{
	case IDOK:
		if (Apply(pDlg))
		{
		case IDCANCEL:
			pDlg->EndDialog(wParam);
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
INT_PTR PasteDlg::DlgProc(HWindow *pDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
