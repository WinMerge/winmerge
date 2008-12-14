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
 * @file  FindDlg.cpp
 *
 * @brief Implementation of the Find dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: FindDlg.cpp 192 2008-12-05 15:18:57Z kimmov $

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "BinTrans.h"

int FindDlg::iFindDlgMatchCase = 0;
int FindDlg::iFindDlgDirection = 0;

//GK16AUG2K: additional options for the find dialog
int FindDlg::iFindDlgUnicode = 0;
int FindDlg::iFindDlgBufLen = 64 * 1024 - 1;

char *FindDlg::pcFindDlgBuffer = (char *)LocalAlloc(LPTR, iFindDlgBufLen);

INT_PTR FindDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		// If there is selected data then make it the data to find.
		if (bSelected)
		{
			// Get start offset and length (is at least =1) of selection.
			int sel_start = iGetStartOfSelection();
			int select_len = iGetEndOfSelection() - sel_start + 1;
			// Get the length of the bytecode representation of the selection (including zero-byte at end).
			int findlen = Text2BinTranslator::iBytes2BytecodeDestLen((char *)&DataArray[sel_start], select_len);
			if (findlen > iFindDlgBufLen)
			{
				MessageBox(hDlg, _T("Selection too large."), _T("Find"), MB_ICONERROR);
				EndDialog(hDlg, IDCANCEL);
				return TRUE;
			}
			// Translate the selection into bytecode and write it into the edit box buffer.
			Text2BinTranslator::iTranslateBytesToBC(pcFindDlgBuffer, &DataArray[sel_start], select_len);
		}
		SendDlgItemMessage(hDlg, IDC_FIND_TEXT, EM_SETLIMITTEXT, iFindDlgBufLen, 0);
		SetDlgItemTextA(hDlg, IDC_FIND_TEXT, pcFindDlgBuffer);
		//GK16AUG2K
		CheckDlgButton(hDlg, iFindDlgDirection == -1 ? IDC_FIND_UP : IDC_FIND_DOWN, BST_CHECKED);
		CheckDlgButton(hDlg, IDC_FIND_MATCHCASE, iFindDlgMatchCase);
		CheckDlgButton(hDlg, IDC_FIND_UNICODE, iFindDlgUnicode);
		return TRUE;

	case WM_COMMAND:
		//GK16AUG2K
		switch (wParam)
		{
		case IDOK:
			if (int srclen = GetDlgItemTextA(hDlg, IDC_FIND_TEXT, pcFindDlgBuffer, iFindDlgBufLen))
			{
				iFindDlgMatchCase = IsDlgButtonChecked(hDlg, IDC_FIND_MATCHCASE);
				//GK16AUG2K: UNICODE search
				iFindDlgUnicode = IsDlgButtonChecked(hDlg, IDC_FIND_UNICODE);
				iFindDlgDirection = IsDlgButtonChecked(hDlg, IDC_FIND_UP) ? -1 : 1;
				// Copy text in Edit-Control. Return the number of characters
				// in the Edit-control minus the zero byte at the end.
				char *pcFindstring = 0;
				//GK16AUG2K
				int destlen;
				if (iFindDlgUnicode)
				{
					pcFindstring = new char[srclen * 2];
					destlen = MultiByteToWideChar(CP_ACP, 0, pcFindDlgBuffer, srclen, (WCHAR *)pcFindstring, srclen) * 2;
				}
				else
				{
					// Create findstring.
					destlen = create_bc_translation(&pcFindstring, pcFindDlgBuffer, srclen);
				}
				if (destlen)
				{
					int i;
					char (*cmp)(char) = iFindDlgMatchCase ? equal : lower_case;

					SetCursor(LoadCursor(NULL, IDC_WAIT));
					// Find forward.
					if (iFindDlgDirection == 1)
					{
						i = find_bytes((char *)&DataArray[iCurByte + 1], DataArray.GetLength() - iCurByte - 1, pcFindstring, destlen, 1, cmp);
						if (i != -1)
							iCurByte += i + 1;
					}
					// Find backward.
					else
					{
						i = find_bytes((char *)&DataArray[0],
							min(iCurByte + (destlen - 1), DataArray.GetLength()),
							pcFindstring, destlen, -1, cmp);
						if (i != -1)
							iCurByte = i;
					}
					SetCursor(LoadCursor(NULL, IDC_ARROW));

					if (i != -1)
					{
						// Select found interval.
						bSelected = TRUE;
						iStartOfSelection = iCurByte;
						iEndOfSelection = iCurByte + destlen - 1;
						adjust_view_for_selection();
						repaint();
					}
					else
						MessageBox(hDlg, _T("Could not find data."), _T("Find"), MB_ICONWARNING);
					//GK16AUG2K
				}
				else
				{
					MessageBox(hDlg, _T("Findstring is zero bytes long."), _T("Find"), MB_ICONERROR);
				}
				delete[] pcFindstring;
			}
			else
			{
				MessageBox(hDlg, _T("Findstring is zero bytes long."), _T("Find"), MB_ICONERROR);
			}
			// fall through
		case IDCANCEL:
			EndDialog(hDlg, wParam);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
