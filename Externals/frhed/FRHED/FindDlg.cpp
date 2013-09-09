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
 * @file  FindDlg.cpp
 *
 * @brief Implementation of the Find dialog.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "BinTrans.h"
#include "FindUtil.h"
#include "StringTable.h"
#include "FindCtxt.h"

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 */
BOOL FindDlg::OnInitDialog(HWindow *pDlg)
{
	// If there is selected data then make it the data to find.
	if (bSelected)
	{
		// Get start offset and length (is at least =1) of selection.
		int sel_start = iGetStartOfSelection();
		int select_len = iGetEndOfSelection() - sel_start + 1;
		// Get the length of the bytecode representation of the selection (including zero-byte at end).
		int findlen = Text2BinTranslator::iBytes2BytecodeDestLen(&m_dataArray[sel_start], select_len);
		if (findlen > FindCtxt::MAX_TEXT_LEN)
		{
			MessageBox(pDlg, GetLangString(IDS_FIND_SEL_TOO_LARGE), MB_ICONERROR);
			pDlg->EndDialog(IDCANCEL);
			return TRUE;
		}
		// Translate the selection into bytecode and write it into the find text buffer.
		int destLen = Text2BinTranslator::iBytes2BytecodeDestLen(&m_dataArray[sel_start], select_len);
		char *tmpBuf = new char[destLen + 1];
		ZeroMemory(tmpBuf, destLen + 1);
		Text2BinTranslator::iTranslateBytesToBC(tmpBuf, &m_dataArray[sel_start], select_len);
		m_pFindCtxt->SetText(tmpBuf);
		delete [] tmpBuf;
	}
	pDlg->SendDlgItemMessage(IDC_FIND_TEXT, EM_SETLIMITTEXT, FindCtxt::MAX_TEXT_LEN, 0);
	pDlg->SetDlgItemTextA(IDC_FIND_TEXT, m_pFindCtxt->GetText());
	//GK16AUG2K
	pDlg->CheckDlgButton(m_pFindCtxt->m_iDirection == -1 ? IDC_FIND_UP : IDC_FIND_DOWN, BST_CHECKED);
	const UINT matchCase = m_pFindCtxt->m_bMatchCase ? BST_CHECKED : BST_UNCHECKED;
	pDlg->CheckDlgButton(IDC_FIND_MATCHCASE, matchCase);
	const UINT findUnicode = m_pFindCtxt->m_bUnicode ? BST_CHECKED : BST_UNCHECKED;
	pDlg->CheckDlgButton(IDC_FIND_UNICODE, findUnicode);

	return TRUE;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Hanle to the dialog.
 * @param [in] wParam The command to handle.
 * @param [in] lParam Optional parameter for the command.
 * @return TRUE if the command was handled, FALSE otherwise.
 */
BOOL FindDlg::OnCommand(HWindow *pDlg, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case IDOK:
	{
		char *tmpBuf = new char[FindCtxt::MAX_TEXT_LEN + 1];
		ZeroMemory(tmpBuf, FindCtxt::MAX_TEXT_LEN + 1);
		int srclen = pDlg->GetDlgItemTextA(IDC_FIND_TEXT, tmpBuf, FindCtxt::MAX_TEXT_LEN);
		m_pFindCtxt->SetText(tmpBuf);
		delete [] tmpBuf;
		if (srclen)
		{
			UINT matchCase = pDlg->IsDlgButtonChecked(IDC_FIND_MATCHCASE);
			m_pFindCtxt->m_bMatchCase = matchCase == BST_CHECKED;
			UINT findUnicode = pDlg->IsDlgButtonChecked(IDC_FIND_UNICODE);
			m_pFindCtxt->m_bUnicode = findUnicode == BST_CHECKED;
			m_pFindCtxt->m_iDirection = pDlg->IsDlgButtonChecked(IDC_FIND_UP) ? -1 : 1;
			// Copy text in Edit-Control. Return the number of characters
			// in the Edit-control minus the zero byte at the end.
			BYTE *pcFindstring = 0;
			int destlen;
			if (m_pFindCtxt->m_bUnicode)
			{
				pcFindstring = new BYTE[srclen * 2];
				destlen = MultiByteToWideChar(CP_ACP, 0, m_pFindCtxt->GetText(),
						srclen, (WCHAR *)pcFindstring, srclen) * 2;
			}
			else
			{
				// Create findstring.
				destlen = create_bc_translation(&pcFindstring,
						m_pFindCtxt->GetText(), srclen, iCharacterSet,
						iBinaryMode);
			}
			if (destlen)
			{
				int i;
				SetCursor(LoadCursor(NULL, IDC_WAIT));
				// Find forward.
				if (m_pFindCtxt->m_iDirection == 1)
				{
					i = findutils_FindBytes(&m_dataArray[iCurByte + 1],
							m_dataArray.GetLength() - iCurByte - 1,
							pcFindstring, destlen, 1, m_pFindCtxt->m_bMatchCase);
					if (i != -1)
						iCurByte += i + 1;
				}
				// Find backward.
				else
				{
					i = findutils_FindBytes(&m_dataArray[0],
						min(iCurByte + (destlen - 1), m_dataArray.GetLength()),
						pcFindstring, destlen, -1, m_pFindCtxt->m_bMatchCase);
					if (i != -1)
						iCurByte = i;
				}
				SetCursor(LoadCursor(NULL, IDC_ARROW));

				if (i != -1)
				{
					// Select found interval.
					bSelected = true;
					iStartOfSelection = iCurByte;
					iEndOfSelection = iCurByte + destlen - 1;
					adjust_view_for_selection();
					repaint();
				}
				else
				{
					MessageBox(pDlg, GetLangString(IDS_FIND_CANNOT_FIND), MB_ICONWARNING);
				}
				//GK16AUG2K
			}
			else
			{
				MessageBox(pDlg, GetLangString(IDS_FIND_EMPTY_STRING), MB_ICONERROR);
			}
			delete [] pcFindstring;
		}
		else
		{
			MessageBox(pDlg, GetLangString(IDS_FIND_EMPTY_STRING), MB_ICONERROR);
		}
	}
	// fall through
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
INT_PTR FindDlg::DlgProc(HWindow *pDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
