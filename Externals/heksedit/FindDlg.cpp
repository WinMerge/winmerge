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
			int sel_start, select_len;
			if (iEndOfSelection < iStartOfSelection)
			{
				sel_start = iEndOfSelection;
				select_len = iStartOfSelection - iEndOfSelection + 1;
			}
			else
			{
				sel_start = iStartOfSelection;
				select_len = iEndOfSelection - iStartOfSelection + 1;
			}

			// Get the length of the bytecode representation of the selection (including zero-byte at end).
			/*int findlen = */Text2BinTranslator::iBytes2BytecodeDestLen( (char*) &DataArray[sel_start], select_len );

			// New buffer length is at least FINDDLG_BUFLEN = 64K, bigger if findstring is bigger than 64K.
			// iFindDlgBufLen = max( FINDDLG_BUFLEN, findlen );

			// Signal dialogue function to display the text in the edit box.
			// iFindDlgLastLen = findlen;

			// Delete old buffer.
			// if( pcFindDlgBuffer != NULL )
			//	delete [] pcFindDlgBuffer;

			// Allocate new buffer.
			// pcFindDlgBuffer = new char[ iFindDlgBufLen ];
			// if( pcFindDlgBuffer == NULL )
			//	MessageBox( hwnd, "Could not allocate findstring buffer!", "Find ERROR", MB_OK | MB_ICONERROR );

			// Translate the selection into bytecode and write it into the edit box buffer.
			Text2BinTranslator::iTranslateBytesToBC(pcFindDlgBuffer, &DataArray[sel_start], select_len);
		}
		SendDlgItemMessage(hDlg, IDC_EDIT1, EM_SETLIMITTEXT, iFindDlgBufLen, 0);
		SetDlgItemText(hDlg, IDC_EDIT1, pcFindDlgBuffer);
		//GK16AUG2K
		CheckDlgButton(hDlg, iFindDlgDirection == -1 ? IDC_RADIO1 : IDC_RADIO2, BST_CHECKED);
		CheckDlgButton(hDlg, IDC_CHECK1, iFindDlgMatchCase);
		CheckDlgButton(hDlg, IDC_CHECK4, iFindDlgUnicode);
		return FALSE;

	case WM_COMMAND:
		//GK16AUG2K
		switch (wParam)
		{
		case IDOK:
			if (int srclen = GetDlgItemText(hDlg, IDC_EDIT1, pcFindDlgBuffer, iFindDlgBufLen))
			{
				iFindDlgMatchCase = IsDlgButtonChecked(hDlg, IDC_CHECK1);
				//GK16AUG2K: UNICODE search
				iFindDlgUnicode = IsDlgButtonChecked(hDlg, IDC_CHECK4);
				iFindDlgDirection = IsDlgButtonChecked(hDlg, IDC_RADIO1) ? -1 : 1;
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

					SetCursor (LoadCursor (NULL, IDC_WAIT));
					// Find forward.
					if (iFindDlgDirection == 1)
					{
						i = find_bytes ((char*) &(DataArray[iCurByte + 1]), DataArray.GetLength() - iCurByte - 1, pcFindstring, destlen, 1, cmp);
						if (i != -1)
							iCurByte += i + 1;
					}
					// Find backward.
					else
					{
						i = find_bytes( (char*) &(DataArray[0]),
							min( iCurByte + (destlen - 1), DataArray.GetLength() ),
							pcFindstring, destlen, -1, cmp );
						if (i != -1)
							iCurByte = i;
					}
					SetCursor (LoadCursor (NULL, IDC_ARROW));

					if (i != -1)
					{
						// Caret will be vertically centered if line of found string is not visible.
						/* OLD: ONLY SET CURSOR POSITION
						if( iCurByte/iBytesPerLine < iVscrollPos || iCurByte/iBytesPerLine > iVscrollPos + cyBuffer )
							iVscrollPos = max( 0, iCurByte/iBytesPerLine-cyBuffer/2 );
						adjust_vscrollbar();
						*/

						// NEW: Select found interval.
						bSelected = TRUE;
						iStartOfSelection = iCurByte;
						iEndOfSelection = iCurByte + destlen - 1;
						adjust_view_for_selection();
						repaint();
					}
					else
						MessageBox(hDlg, "Could not find data.", "Find", MB_ICONWARNING);
					//GK16AUG2K
				}
				else
				{
					MessageBox(hDlg, "Findstring is zero bytes long.", "Find", MB_ICONERROR);
				}
				delete[] pcFindstring;
			}
			else
			{
				MessageBox(hDlg, "Findstring is zero bytes long.", "Find", MB_ICONERROR);
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
