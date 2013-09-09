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
 * @file  ChooseDiffDlg.cpp
 *
 * @brief Implementation of the difference choosing dialog.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "clipboard.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "StringTable.h"

void ChooseDiffDlg::add_diff(HListBox *list, int diff, int lower, int upper)
{
	TCHAR buf[100];
	_stprintf(buf,
		// "%d) 0x%x=%u to 0x%x=%u (%d bytes)"
		GetLangString(IDS_DIFFLISTITEMFORMAT),
		diff, lower, lower,	upper, upper, upper - lower + 1);
	list->AddString(buf);
}

//-------------------------------------------------------------------
// Transfer offsets of differences to pdiff.
int ChooseDiffDlg::get_diffs(HListBox *list, BYTE *ps, int sl, BYTE *pd, int dl)
{
	int lower, upper;
	int i = 0, diff = 0, type = 1;
	// type=0 means differences, type=1 means equality at last char.
	while (i < sl && i < dl)
	{
		switch (type)
		{
		case 0:
			// Working on area of difference at the moment.
			if (ps[i] == pd[i])
			{
				// Chars equal again.
				add_diff(list, ++diff, lower, upper);
				type = 1;
			}
			// else: chars still different.
			break;

		case 1:
			// Working on area of equality at the moment.
			if (ps[i] != pd[i])
			{
				// Start of area of difference found.
				lower = i; // Save start of area of difference.
				type = 0;
			}
			// else: chars still equal.
			break;
		}
		upper = i++;
	}
	if (type == 0) // If area of difference was at end of file.
	{
		add_diff(list, ++diff, lower, upper);
	}
	return diff;
}

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to dialog.
 * @return TRUE
 */
BOOL ChooseDiffDlg::OnInitDialog(HWindow *pDlg)
{
	TCHAR szFileName[_MAX_PATH];
	szFileName[0] = '\0';

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hwndOwner = pDlg->m_hWnd;
	ofn.lpstrFilter = GetLangString(IDS_OPEN_ALL_FILES);
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrTitle = GetLangString(IDS_DIFF_CHOOSEFILE);
	if (!GetOpenFileName(&ofn))
		return FALSE;
	int filehandle = _topen(szFileName, _O_RDONLY|_O_BINARY);
	if (filehandle == -1)
	{
		MessageBox(pDlg, GetLangString(IDS_DIFF_ERROPEN), MB_ICONERROR);
		return FALSE;
	}
	BOOL bDone = FALSE;
	if (int filelen = _filelength(filehandle))
	{
		int iDestFileLen = filelen;
		int iSrcFileLen = m_dataArray.GetLength() - iCurByte;
		if (BYTE *cmpdata = new BYTE[filelen])
		{
			// Read data.
			if (_read(filehandle, cmpdata, filelen) != -1)
			{
				HListBox *list = static_cast<HListBox *>(pDlg->GetDlgItem(IDC_CHOOSEDIFF_DIFFLIST));
				if (int diff = get_diffs(list, &m_dataArray[iCurByte], m_dataArray.GetLength() - iCurByte, cmpdata, filelen))
				{
					TCHAR buf[100];
					_stprintf(buf, GetLangString(IDS_DIFF_AREAS_FOUND), diff);
					pDlg->SetDlgItemText(IDC_NUMDIFFS, buf);
					_stprintf(buf, GetLangString(IDS_DIFF_REMAINING_BYTES), iSrcFileLen, iDestFileLen);
					pDlg->SetDlgItemText(IDC_CHOOSEDIFF_FSIZES, buf);
					list->SetCurSel(0);
					bDone = TRUE;
				}
				else
				{
					// No difference.
					MessageBox(pDlg, GetLangString(IDS_DIFF_NO_DIFF), MB_ICONINFORMATION);
				}
			}
			else
			{
				MessageBox(pDlg, GetLangString(IDS_ERR_READING_FILE), MB_ICONERROR);
			}
			delete [] cmpdata;
		}
		else
		{
			MessageBox(pDlg, GetLangString(IDS_NO_MEMORY), MB_ICONERROR);
		}
	}
	_close(filehandle);
	return bDone;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Handle to dialog.
 * @param [in] wParam First param (depends on command).
 * @return TRUE if command (message) was handled, FALSE if not.
 */
BOOL ChooseDiffDlg::OnCommand(HWindow *pDlg, WPARAM wParam, LPARAM)
{
	switch (wParam)
	{
	// By pabs.
	case IDCOPY:
		{//copy button was pressed
			if (!pwnd->OpenClipboard()) //open clip
			{
				MessageBox(pwnd, GetLangString(IDS_CANNOT_ACCESS_CLIPBOARD), MB_ICONERROR);
				return TRUE;
			}
			EmptyClipboard(); //empty clip
			IStream *piStream = 0;
			if (SUCCEEDED(CreateStreamOnHGlobal(0, FALSE, &piStream)))
			{
				HListBox *list = static_cast<HListBox *>(pDlg->GetDlgItem(IDC_CHOOSEDIFF_DIFFLIST));
				CLIPFORMAT cf = sizeof(TCHAR) == sizeof(WCHAR) ? CF_UNICODETEXT : CF_TEXT;
				int num = list->GetCount();
				for (int i = 0 ; i < num ; i++)
				{
					String s;
					list->GetText(i, s);
					piStream->Write(s.c_str(), s.length() * sizeof(TCHAR), NULL);
					//add '\r\n' to the end of each line - this is '\r\n' rather than '\n' so that it can be pasted into notepad & dos programs
					static const TCHAR eol[] = _T("\r\n");
					piStream->Write(eol, i < num ? sizeof eol - sizeof *eol : sizeof eol, NULL);
				}
				HGLOBAL hMem = 0;
				if (SUCCEEDED(GetHGlobalFromStream(piStream, &hMem)))
				{
					SetClipboardData(cf, hMem); //copy to clip
				}
				piStream->Release();
			}
			CloseClipboard(); //close clip
		}
		return TRUE;

	case IDOK:
		{
			HListBox *pLb = static_cast<HListBox *>(pDlg->GetDlgItem(IDC_CHOOSEDIFF_DIFFLIST));
			int i = pLb->GetCurSel();
			if (i != -1)
			{
				String s;
				pLb->GetText(i, s);
				i = _stscanf(s.c_str(),
					// "%d) 0x%x=%u to 0x%x=%u (%d bytes)"
					GetLangString(IDS_DIFFLISTITEMFORMAT),
					&i, &i, &iStartOfSelection, &i, &iEndOfSelection, &i);
				iStartOfSelection += iCurByte;
				iEndOfSelection += iCurByte;
				iCurByte = iStartOfSelection;
				bSelected = true;
				adjust_view_for_selection();
				repaint();
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
INT_PTR ChooseDiffDlg::DlgProc(HWindow *pDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		if (!OnInitDialog(pDlg))
			pDlg->EndDialog(IDCANCEL);
		return TRUE;

	case WM_COMMAND:
		return OnCommand(pDlg, wParam, lParam);

	case WM_HELP:
		OnHelp(pDlg);
		break;
	}
	return FALSE;
}
