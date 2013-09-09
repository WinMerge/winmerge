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
 * @file  ReplaceDlg.cpp
 *
 * @brief Implementation of the replace dialog.
 *
 */
#include "precomp.h"
#include "Simparr.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "BinTrans.h"
#include "FindUtil.h"
#include "StringTable.h"

// String containing data to replace.
SimpleString ReplaceDlg::strToReplaceData;
// String containing data to replace with.
SimpleString ReplaceDlg::strReplaceWithData;

//-------------------------------------------------------------------
// Translate the text in the string to binary data and store in the array.
int ReplaceDlg::transl_text_to_binary(SimpleArray<BYTE> &out)
{
	BYTE *pcOut;
	int destlen = create_bc_translation(&pcOut,
		strReplaceWithData, strReplaceWithData.StrLen(),
		iCharacterSet, iBinaryMode);
	if (destlen)
		out.Adopt(pcOut, destlen - 1, destlen);
	return destlen;
}

//-------------------------------------------------------------------
// Create a text representation of an array of bytes and save it in
// a SimpleString object.
int	ReplaceDlg::transl_binary_to_text(const BYTE *src, int len)
{
	// How long will the text representation of array of bytes be?
	int destlen = Text2BinTranslator::iBytes2BytecodeDestLen(src, len);
	strToReplaceData.SetSize(destlen);
	strToReplaceData.ExpandToSize();
	if (char *pd = strToReplaceData)
	{
		Text2BinTranslator::iTranslateBytesToBC(pd, (unsigned char*) src, len);
		return TRUE;
	}
	return FALSE;
}

//-------------------------------------------------------------------
bool ReplaceDlg::find_and_select_data(int finddir, bool case_sensitive)
{
	BYTE *tofind;
	// Create a translation from bytecode to char array of finddata.
	int destlen = create_bc_translation(&tofind, strToReplaceData,
		strToReplaceData.StrLen(), iCharacterSet, iBinaryMode);
	int i = iGetStartOfSelection();
	int n = iGetEndOfSelection() - i + 1;
	int j;
	if (finddir >= 0)
	{
		i += finddir * n;
		// Find forward.
		j = findutils_FindBytes(&m_dataArray[i],
			m_dataArray.GetLength() - i - 1,
			tofind,	destlen, 1, case_sensitive);
		if (j != -1)
			i += j;
	}
	else
	{
		// Find backward.
		j = findutils_FindBytes(&m_dataArray[0],
			min(iCurByte + (destlen - 1), m_dataArray.GetLength()),
			tofind, destlen, -1, case_sensitive);
		if (j != -1)
			i = j;
	}
	bool done = false;
	if (j != -1)
	{
		// NEW: Select found interval.
		bSelected = true;
		iStartOfSelection = iCurByte = i;
		iEndOfSelection = iStartOfSelection + destlen - 1;
		done = true;
	}
	delete [] tofind;
	return done;
}

//-------------------------------------------------------------------
// SimpleString replacedata contains data to replace with.
bool ReplaceDlg::replace_selected_data(HWindow *pDlg)
{
	if (!bSelected)
	{
		MessageBox(pDlg, GetLangString(IDS_REPL_NO_DATA), MB_ICONERROR);
		return false;
	}
	int i = iGetStartOfSelection();
	int n = iGetEndOfSelection() - i + 1;
	if (strReplaceWithData.IsEmpty())
	{
		// Selected data is to be deleted, since replace-with data is empty string.
		if (!m_dataArray.Replace(i, n, 0, 0))
		{
			MessageBox(pDlg, GetLangString(IDS_REPL_CANT_DELETE), MB_ICONERROR);
			return FALSE;
		}
		bSelected = false;
		iCurByte = iStartOfSelection;
	}
	else if (bPasteAsText)
	{
		// Replace with non-zero-length data.
		if (!m_dataArray.Replace(i, n, (const BYTE *)(const char *)strReplaceWithData, strReplaceWithData.StrLen()))
		{
			MessageBox(pDlg, GetLangString(IDS_REPL_FAILED), MB_ICONERROR);
			return false;
		}
		iEndOfSelection = iStartOfSelection + strReplaceWithData.StrLen() - 1;
	}
	else
	{
		// Input string contains special-syntax-coded binary data.
		SimpleArray<BYTE> out;
		if (!transl_text_to_binary(out))
		{
			MessageBox(pDlg, GetLangString(IDS_REPL_CANNOT_CONVERT), MB_ICONERROR);
			return false;
		}
		if (!m_dataArray.Replace(i, n, out, out.GetLength()))
		{
			MessageBox(pDlg, GetLangString(IDS_REPL_FAILED), MB_ICONERROR);
			return false;
		}
		iEndOfSelection = iStartOfSelection + out.GetLength() - 1;
	}
	bFilestatusChanged = true;
	iFileChanged = TRUE;
	return true;
}

void ReplaceDlg::find_directed(HWindow *pDlg, int finddir)
{
	GetDlgItemText(pDlg, IDC_TO_REPLACE_EDIT, strToReplaceData);
	bool case_sensitive = pDlg->IsDlgButtonChecked(IDC_MATCHCASE_CHECK) == BST_CHECKED;
	if (find_and_select_data(finddir, case_sensitive))
	{
		adjust_view_for_selection();
		repaint();
		synch_sibling();
		EnableDlgItem(pDlg, IDC_REPLACE_BUTTON, TRUE);
	}
	else
	{
		MessageBox(pDlg, GetLangString(IDS_FIND_CANNOT_FIND), MB_ICONWARNING);
	}
}

void ReplaceDlg::replace_directed(HWindow *pDlg, int finddir, bool showCount)
{
	bool case_sensitive = pDlg->IsDlgButtonChecked(IDC_MATCHCASE_CHECK) == BST_CHECKED;
	GetDlgItemText(pDlg, IDC_TO_REPLACE_EDIT, strToReplaceData);
	GetDlgItemText(pDlg, IDC_REPLACEWITH_EDIT, strReplaceWithData);
	bPasteAsText = pDlg->IsDlgButtonChecked(IDC_USETRANSLATION_CHECK) == BST_UNCHECKED;
	//------------------
	// Don't do anything if to-replace and replace-with data are same.
	Text2BinTranslator tr_find(strToReplaceData), tr_replace(strReplaceWithData);
	if (tr_find.bCompareBin(tr_replace, iCharacterSet, iBinaryMode))
	{
		MessageBox(pDlg, GetLangString(IDS_REPL_SAME_STRS), MB_ICONERROR);
		return;
	}
	WaitCursor wc;
	int occ_num = 0;
	HWindow *pwndFocus = HWindow::GetFocus();
	if (EnableDlgItem(pDlg, IDC_REPLACE_BUTTON, FALSE) == FALSE)
	{
		// Don't lose focus.
		if (!pwndFocus->IsWindowEnabled())
			pDlg->SetFocus();
		occ_num++;
		replace_selected_data(pDlg);
	}
	if (finddir)
	{
		while (find_and_select_data(finddir, case_sensitive))
		{
			occ_num++;
			replace_selected_data(pDlg);
		};
	}
	if (occ_num)
	{
		set_wnd_title();
		adjust_view_for_selection();
		resize_window();
		synch_sibling();

		if (showCount)
		{
			TCHAR tbuf[100];
			_stprintf(tbuf, GetLangString(IDS_REPL_COUNT), occ_num);
			MessageBox(pDlg, tbuf, MB_ICONINFORMATION);
		}
	}
}

/**
 * @brief Handle dialog messages.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg The message.
 * @param [in] wParam The command in the message.
 * @param [in] lParam The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
 */
INT_PTR ReplaceDlg::DlgProc(HWindow *pDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		// If there is selected data then make it the data to find.
		if (bSelected)
		{
			int sel_start = iGetStartOfSelection();
			int select_len = iGetEndOfSelection() - sel_start + 1;
			if (!transl_binary_to_text(&m_dataArray[sel_start], select_len))
			{
				MessageBox(pDlg, GetLangString(IDS_REPL_BAD_SELECT), MB_OK);
				pDlg->EndDialog(IDCANCEL);
				return TRUE;
			}
		}
		if (bPasteAsText)
			pDlg->CheckDlgButton(IDC_USETRANSLATION_CHECK, BST_UNCHECKED);
		else
			pDlg->CheckDlgButton(IDC_USETRANSLATION_CHECK, BST_CHECKED);
		if (char *pstr = strToReplaceData)
			pDlg->SetDlgItemTextA(IDC_TO_REPLACE_EDIT, pstr);
		if (char *pstr = strReplaceWithData)
			pDlg->SetDlgItemTextA(IDC_REPLACEWITH_EDIT, pstr);
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDCANCEL:
			bPasteAsText = pDlg->IsDlgButtonChecked(IDC_USETRANSLATION_CHECK) == BST_UNCHECKED;
			pDlg->EndDialog(wParam);
			return TRUE;

		case IDC_FINDNEXT_BUTTON:
			find_directed(pDlg, 1);
			break;

		case IDC_FINDPREVIOUS_BUTTON:
			find_directed(pDlg, -1);
			break;

		case IDC_FOLLOCC_BUTTON:
			replace_directed(pDlg, 1, true);
			break;

		case IDC_PREVOCC_BUTTON:
			replace_directed(pDlg, -1, true);
			break;

		case IDC_REPLACE_BUTTON:
			replace_directed(pDlg, 0, false);
			break;
		}
		break;

	case WM_HELP:
		OnHelp(pDlg);
		break;
	}
	return FALSE;
}
