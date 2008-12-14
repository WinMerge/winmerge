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
 * @file  ChooseDiffDlg.cpp
 *
 * @brief Implementation of the difference choosing dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: ChooseDiffDlg.cpp 201 2008-12-10 16:15:27Z kimmov $

#include "precomp.h"
#include "resource.h"
#include "StringTable.h"
#include "clipboard.h"
#include "hexwnd.h"
#include "hexwdlg.h"

void ChooseDiffDlg::add_diff(HWND hwndList, int diff, int lower, int upper)
{
	TCHAR buf[100];
	_stprintf(buf,
		S.DiffListItemFormat, //"%d) 0x%x=%n%d to 0x%x=%n%d (%d bytes)",
		diff,
		lower, &lower, lower,
		upper, &upper, upper,
		upper - lower + 1);
	int i = SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)buf);
	SendMessage(hwndList, LB_SETITEMDATA, i, MAKELONG(lower, upper));
}

//-------------------------------------------------------------------
// Transfer offsets of differences to pdiff.
int ChooseDiffDlg::get_diffs(HWND hwndList, char *ps, int sl, char *pd, int dl)
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
				add_diff(hwndList, ++diff, lower, upper);
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
		add_diff(hwndList, ++diff, lower, upper);
	}
	return diff;
}

BOOL ChooseDiffDlg::OnInitDialog(HWND hDlg)
{
	TCHAR szFileName[MAX_PATH];
	szFileName[0] = _T('\0');
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hwndOwner = hDlg;
	ofn.lpstrFilter = _T("All Files (*.*)\0*.*\0\0");
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrTitle = _T("Choose file to compare with");
	if (!GetOpenFileName(&ofn))
		return FALSE;
	int filehandle = _topen(szFileName, _O_RDONLY|_O_BINARY);
	if (filehandle == -1)
	{
		MessageBox(hDlg, _T("Error while opening file."), _T("Compare"), MB_ICONERROR);
		return FALSE;
	}
	BOOL bDone = FALSE;
	if (int filelen = _filelength(filehandle))
	{
		int iDestFileLen = filelen;
		int iSrcFileLen = DataArray.GetLength() - iCurByte;
		if (char *cmpdata = new char[filelen])
		{
			// Read data.
			if (_read(filehandle, cmpdata, filelen) != -1)
			{
				HWND hwndList = GetDlgItem(hDlg, IDC_CHOOSEDIFF_DIFFLIST);
				if (int diff = get_diffs(hwndList, (char *)&DataArray[iCurByte], DataArray.GetLength() - iCurByte, cmpdata, filelen))
				{
					TCHAR buf[100];
					_stprintf(buf, _T("%d areas of difference found."), diff);
					SetDlgItemText(hDlg, IDC_NUMDIFFS, buf);
					_stprintf(buf, _T("Remaining loaded data size: %d, size of file on disk: %d."), iSrcFileLen, iDestFileLen);
					SetDlgItemText(hDlg, IDC_CHOOSEDIFF_FSIZES, buf);
					SendMessage(hwndList, LB_SETCURSEL, 0, 0);
					bDone = TRUE;
				}
				else
				{
					// No difference.
					MessageBox(hDlg, _T("Data matches exactly."), _T("Compare"), MB_ICONINFORMATION);
				}
			}
			else
			{
				MessageBox(hDlg, _T("Error while reading from file."), _T("Compare"), MB_ICONERROR);
			}
			delete[] cmpdata;
		}
		else
		{
			MessageBox(hDlg, _T("Not enough memory."), _T("Compare"), MB_ICONERROR);
		}
	}
	_close(filehandle);
	return bDone;
}

BOOL ChooseDiffDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM)
{
	switch (wParam)
	{
	// By pabs.
	case IDCOPY:
		{//copy button was pressed
			if (!OpenClipboard(hwnd)) //open clip
			{
				MessageBox(hwnd, _T("Cannot get access to clipboard."), _T("Copy"), MB_ICONERROR);
				return TRUE;
			}
			EmptyClipboard(); //empty clip
			IStream *piStream = 0;
			if (SUCCEEDED(CreateStreamOnHGlobal(0, FALSE, &piStream)))
			{
				HWND hwndList = GetDlgItem(hDlg, IDC_CHOOSEDIFF_DIFFLIST);//get the list
				int num = SendMessage(hwndList, LB_GETCOUNT, 0, 0);//get the # items in the list
				for (int i = 0 ; i < num ; i++)
				{
					//add '\r\n' to the end of each line - this is '\r\n' rather than '\n' so that it can be pasted into notepad & dos programs
					TCHAR buf[100];
					int cch = SendMessage(hwndList, LB_GETTEXT, i, (LPARAM)buf);
					piStream->Write(buf, cch * sizeof *buf, 0);
					static const TCHAR eol[] = _T("\r\n");
					piStream->Write(eol, i < num ? sizeof eol - sizeof *eol : sizeof eol, 0);
				}
				HGLOBAL hMem = 0;
				if (SUCCEEDED(GetHGlobalFromStream(piStream, &hMem)))
				{
					SetClipboardData(CF_PLAINTEXT, hMem); //copy to clip
				}
				piStream->Release();
			}
			CloseClipboard(); //close clip
		}
		return TRUE;

	case IDOK:
		{
			HWND hwndList = GetDlgItem(hDlg, IDC_CHOOSEDIFF_DIFFLIST);
			int i = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
			if (i != -1)
			{
				DWORD dw = SendMessage(hwndList, LB_GETITEMDATA, i, 0);
				TCHAR buf[100];
				SendMessage(hwndList, LB_GETTEXT, i, (LPARAM)buf);
				iStartOfSelection = StrToInt(buf + LOWORD(dw));
				iEndOfSelection = StrToInt(buf + HIWORD(dw));
				iStartOfSelection += iCurByte;
				iEndOfSelection += iCurByte;
				iCurByte = iStartOfSelection;
				bSelected = TRUE;
				adjust_view_for_selection();
				repaint();
			}
		}
		// fall through
	case IDCANCEL:
		EndDialog(hDlg, wParam);
		return TRUE;
	}
	return FALSE;
}

//-------------------------------------------------------------------
INT_PTR ChooseDiffDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		if (!OnInitDialog(hDlg))
			EndDialog(hDlg, IDCANCEL);
		return TRUE;
	case WM_COMMAND:
		return OnCommand(hDlg, wParam, lParam);
	}
	return FALSE;
}

