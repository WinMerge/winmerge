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
 * @file  PasteDlg.cpp
 *
 * @brief Implementation of the Paste dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: PasteDlg.cpp 195 2008-12-09 19:00:27Z kimmov $

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

BOOL PasteDlg::OnInitDialog(HWND hDlg)
{
	if (bSelected) // iPasteMode = 0
	{
		EnableWindow(GetDlgItem(hDlg, IDC_PASTE_OVERWRITE), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_PASTE_INSERT), FALSE);
	}
	else if (iInsertMode) // iPasteMode = 2
	{
		CheckDlgButton(hDlg, IDC_PASTE_INSERT, BST_CHECKED);
	}
	else // iPasteMode = 1
	{
		CheckDlgButton(hDlg, IDC_PASTE_OVERWRITE, BST_CHECKED);
	}
	SendDlgItemMessage(hDlg, IDC_PASTE_CLIPBOARD, WM_PASTE, 0, 0);
	SetDlgItemInt(hDlg, IDC_PASTE_TIMES, iPasteTimes, TRUE);
	SetDlgItemInt(hDlg, IDC_PASTE_SKIPBYTES, iPasteSkip, TRUE);
	CheckDlgButton(hDlg, IDC_PASTE_BINARY, iPasteAsText);
	return TRUE;
}

BOOL PasteDlg::Apply(HWND hDlg)
{
	iPasteAsText = IsDlgButtonChecked(hDlg, IDC_PASTE_BINARY);
	iPasteTimes = GetDlgItemInt(hDlg, IDC_PASTE_TIMES, 0, TRUE);
	if (iPasteTimes <= 0)
	{
		MessageBox(hDlg, _T("Number of times to paste must be at least 1."), _T("Paste"), MB_ICONERROR);
		return FALSE;
	}
	iPasteSkip = GetDlgItemInt(hDlg, IDC_PASTE_SKIPBYTES, 0, TRUE);
	HWND hwndEdit1 = GetDlgItem(hDlg, IDC_PASTE_CLIPBOARD);
	int destlen = GetWindowTextLength(hwndEdit1) + 1;
	char *pcPastestring = new char[destlen];
	destlen = GetWindowTextA(hwndEdit1, pcPastestring, destlen);
	if (iPasteAsText == 0)
	{
		char *pc = 0;
		destlen = create_bc_translation(&pc, pcPastestring, strlen(pcPastestring));
		delete [] pcPastestring;
		pcPastestring = pc;
	}
	if (destlen == 0)
	{
		MessageBox(hDlg, _T("Tried to paste zero-length array."), _T("Paste"), MB_ICONERROR);
		delete [] pcPastestring;
		return FALSE;
	}
	WaitCursor wc1;
	if (bSelected || IsDlgButtonChecked(hDlg, IDC_PASTE_INSERT))
	{
		// Insert at iCurByte. Bytes there will be pushed up.
		if (bSelected)
		{
			iCurByte = iGetStartOfSelection();
			int iEndByte = iGetEndOfSelection();
			DataArray.RemoveAt(iCurByte, iEndByte - iCurByte + 1);//Remove extraneous data
			bSelected = FALSE;//Deselect
		}
		int i = iCurByte;
		for (int k = 0 ; k < iPasteTimes ; k++)
		{
			if (!DataArray.InsertAtGrow(i, (unsigned char*)pcPastestring, 0, destlen))
			{
				MessageBox(hDlg, _T("Not enough memory for inserting."), _T("Paste"), MB_ICONERROR);
				break;
			}
			i += destlen + iPasteSkip;
		}
		iFileChanged = TRUE;
		bFilestatusChanged = TRUE;
		resize_window();
	}
	else
	{
		// Overwrite.
		// Enough space for writing?
		// DataArray.GetLength()-iCurByte = number of bytes from including curbyte to end.
		if (DataArray.GetLength() - iCurByte < (iPasteSkip + destlen) * iPasteTimes)
		{
			MessageBox(hDlg, _T("Not enough space for overwriting."), _T("Paste"), MB_ICONERROR);
			delete [] pcPastestring;
			return TRUE;
		}
		// Overwrite data.
		for (int k = 0 ; k < iPasteTimes ; k++)
		{
			for (int i = 0 ; i < destlen ; i++)
			{
				DataArray[iCurByte + k * (iPasteSkip + destlen) + i] = pcPastestring[i];
			}
		}
		iFileChanged = TRUE;
		bFilestatusChanged = TRUE;
		repaint();
	}
	delete [] pcPastestring;
	return TRUE;
}

BOOL PasteDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM)
{
	switch (wParam)
	{
	case IDOK:
		if (Apply(hDlg))
		{
		case IDCANCEL:
			EndDialog(hDlg, wParam);
		}
		return TRUE;
	}
	return FALSE;
}

INT_PTR PasteDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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

static HWND hwndNextViewer = NULL;

BOOL FastPasteDlg::OnInitDialog(HWND hDlg)
{
	SendMessage(hDlg, WM_COMMAND, IDC_REFRESH, 0);
	hwndNextViewer = SetClipboardViewer(hDlg);
	SetDlgItemInt(hDlg, IDC_PASTE_TIMES, iPasteTimes, TRUE);
	SetDlgItemInt(hDlg, IDC_PASTE_SKIPBYTES, iPasteSkip, TRUE);
	// Depending on INS or OVR mode, set the radio button.
	if (bSelected) // iPasteMode = 0
	{
		EnableWindow(GetDlgItem(hDlg, IDC_PASTE_OVERWRITE), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_PASTE_INSERT), FALSE);
	}
	else if (iInsertMode) // iPasteMode = 2
	{
		CheckDlgButton(hDlg, IDC_PASTE_INSERT, BST_CHECKED);
	}
	else // iPasteMode = 1
	{
		CheckDlgButton(hDlg, IDC_PASTE_OVERWRITE, BST_CHECKED);
	}
	CheckDlgButton(hDlg, IDC_PASTE_BINARY, iPasteAsText);
	return TRUE;
}

BOOL FastPasteDlg::Apply(HWND hDlg)
{
	iPasteAsText = IsDlgButtonChecked(hDlg, IDC_PASTE_BINARY);
	iPasteTimes = GetDlgItemInt(hDlg, IDC_PASTE_TIMES, 0, TRUE);
	if (iPasteTimes <= 0)
	{
		MessageBox(hDlg, _T("Number of times to paste must be at least 1."), _T("Paste"), MB_ICONERROR);
		return FALSE;
	}
	iPasteSkip = GetDlgItemInt(hDlg, IDC_PASTE_SKIPBYTES, 0, TRUE);
	HWND list = GetDlgItem(hDlg, IDC_LIST);
	int i = SendMessage(list, LB_GETCURSEL, 0, 0);
	if (i == LB_ERR)
	{
		MessageBox(hDlg, _T("You need to select a clipboard format to use."), _T("Paste"), MB_ICONERROR);
		return FALSE;
	}
	UINT uFormat = SendMessage(list, LB_GETITEMDATA, i, 0);
	char *pcPastestring = 0;
	int destlen = 0;
	BOOL bPasteBinary = FALSE;
	BOOL bPasteUnicode = FALSE;
	if (OpenClipboard(NULL))
	{
		if (HGLOBAL hClipMemory = GetClipboardData(uFormat))
		{
			int gsize = GlobalSize(hClipMemory);
			if (gsize)
			{
				char *pClipMemory = (char *)GlobalLock(hClipMemory);
				pcPastestring = new char[gsize];
				memcpy(pcPastestring, pClipMemory, gsize);
				GlobalUnlock(hClipMemory);
			}
			switch (uFormat)
			{
			case CF_UNICODETEXT:
				bPasteUnicode = TRUE;
			case CF_TEXT:
			case CF_DSPTEXT:
			case CF_OEMTEXT:
				break;
			default:
				destlen = gsize;//CRAP f**king M$ Windoze
				bPasteBinary = TRUE;
				break;
			}
		}
		CloseClipboard();
	}
	if (pcPastestring && !bPasteBinary)
	{
		if (iPasteAsText)
		{
			destlen = bPasteUnicode ? 2 * wcslen((WCHAR *)pcPastestring) : strlen(pcPastestring);
		}
		else
		{
			char *pc = 0;
			destlen = create_bc_translation(&pc, pcPastestring, strlen(pcPastestring));
			delete [] pcPastestring;
			pcPastestring = pc;
		}
	}
	if (destlen == 0)
	{
		MessageBox(hDlg, _T("Tried to paste zero-length array."), _T("Paste"), MB_ICONERROR);
		delete [] pcPastestring;
		return FALSE;
	}
	WaitCursor wc1;
	if (bSelected || IsDlgButtonChecked(hDlg, IDC_PASTE_INSERT))
	{
		// Insert at iCurByte. Bytes there will be pushed up.
		if (bSelected)
		{
			iCurByte = iGetStartOfSelection();
			int iEndByte = iGetEndOfSelection();
			DataArray.RemoveAt(iCurByte, iEndByte - iCurByte + 1);//Remove extraneous data
			bSelected = FALSE;//Deselect
		}
		int i = iCurByte;
		for (int k = 0 ; k < iPasteTimes ; k++)
		{
			if (!DataArray.InsertAtGrow(i, (unsigned char*)pcPastestring, 0, destlen))
			{
				MessageBox(hDlg, _T("Not enough memory for inserting."), _T("Paste"), MB_ICONERROR);
				break;
			}
			i += destlen + iPasteSkip;
		}
		iFileChanged = TRUE;
		bFilestatusChanged = TRUE;
		resize_window();
	}
	else
	{
		// Overwrite.
		// Enough space for writing?
		if (DataArray.GetLength() - iCurByte < (iPasteSkip + destlen) * iPasteTimes)
		{
			MessageBox(hDlg, _T("Not enough space for overwriting."), _T("Paste"), MB_ICONERROR);
			delete [] pcPastestring;
			return TRUE;
		}
		// Overwrite data.
		for (int k = 0 ; k < iPasteTimes ; k++)
		{
			for (int i = 0 ; i < destlen ; i++)
			{
				DataArray[iCurByte + k * (iPasteSkip + destlen) + i] = pcPastestring[i];
			}
		}
		iFileChanged = TRUE;
		bFilestatusChanged = TRUE;
		repaint();
	}
	delete [] pcPastestring;
	return TRUE;
}

BOOL FastPasteDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM)
{
	switch (wParam)
	{
	case IDOK:
		if (Apply(hDlg))
		{
		case IDCANCEL:
			EndDialog(hDlg, wParam);
		}
		return TRUE;
	case IDC_REFRESH:
		{
			//Get all the Clipboard formats
			HWND list = GetDlgItem(hDlg, IDC_LIST);
			SendMessage(list, LB_RESETCONTENT, 0, 0);
			if (CountClipboardFormats() && OpenClipboard(NULL))
			{
				UINT uFormat;
				int i;
				TCHAR szFormatName[100];
				int SetSel = 0;
				LPCTSTR lpFormatName;

				uFormat = EnumClipboardFormats(0);
				while (uFormat)
				{
					lpFormatName = NULL;
					// For registered formats, get the registered name.
					if (GetClipboardFormatName(uFormat, szFormatName, RTL_NUMBER_OF(szFormatName)))
					{
						lpFormatName = szFormatName;
					}
					else
					{
						//Get the name of the standard clipboard format.
						switch (uFormat)
						{
							#define CASE(a,b) case a: lpFormatName = _T(#a); SetSel = b; break;
								CASE(CF_TEXT,1)
							#undef CASE
							#define CASE(a) case a: lpFormatName = _T(#a); break;
								CASE(CF_BITMAP) CASE(CF_METAFILEPICT) CASE(CF_SYLK)
								CASE(CF_DIF) CASE(CF_TIFF) CASE(CF_OEMTEXT)
								CASE(CF_DIB) CASE(CF_PALETTE) CASE(CF_PENDATA)
								CASE(CF_RIFF) CASE(CF_WAVE) CASE(CF_UNICODETEXT)
								CASE(CF_ENHMETAFILE) CASE(CF_HDROP) CASE(CF_LOCALE)
								CASE(CF_MAX) CASE(CF_OWNERDISPLAY) CASE(CF_DSPTEXT)
								CASE(CF_DSPBITMAP) CASE(CF_DSPMETAFILEPICT)
								CASE(CF_DSPENHMETAFILE) CASE(CF_PRIVATEFIRST)
								CASE(CF_PRIVATELAST) CASE(CF_GDIOBJFIRST)
								CASE(CF_GDIOBJLAST) CASE(CF_DIBV5)
							#undef CASE
							default:
								#define CASE(a) if (uFormat>a##FIRST&&uFormat<a##LAST) _stprintf(szFormatName, _T(#a) _T("%d"), uFormat - a##FIRST);
								CASE(CF_PRIVATE) else CASE(CF_GDIOBJ) else break;
								#undef CASE
								/*Format ideas for future: hex number, system/msdn constant, registered format, WM_ASKFORMATNAME, tickbox for delay rendered or not*/
								/*else if(uFormat>0xC000&&uFormat<0xFFFF){
									sprintf(szFormatName,"CF_REGISTERED%d",uFormat-0xC000);
								}*/
								lpFormatName = szFormatName;
							break;
						}
					}
					if (lpFormatName == NULL)
					{
						_stprintf(szFormatName, _T("0x%.8x"), uFormat);
						lpFormatName = szFormatName;
					}
					//Insert into the list
					i = SendMessage(list, LB_ADDSTRING, 0, (LPARAM) lpFormatName);
					if (SetSel == 1)
					{
						SetSel = 2;
						SendMessage(list, LB_SETCURSEL, i, 0);
					}
					SendMessage(list, LB_SETITEMDATA, i, uFormat);

					uFormat = EnumClipboardFormats(uFormat);
				}
				CloseClipboard();
				if (!SetSel)
					SendMessage(list, LB_SETCURSEL, 0, 0);
			}
		}
		return TRUE;
	case MAKEWPARAM(IDC_LIST, LBN_SELCHANGE):
		{
			HWND list = GetDlgItem(hDlg, IDC_LIST);
			int i = SendMessage(list, LB_GETCURSEL, 0, 0);
			UINT f = SendMessage(list, LB_GETITEMDATA, i, 0);
			if (f == CF_UNICODETEXT)
				CheckDlgButton(hDlg, IDC_PASTE_BINARY, BST_CHECKED);
		}
		return TRUE;
	}
	return FALSE;
}

INT_PTR FastPasteDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hDlg);
	case WM_COMMAND:
		return OnCommand(hDlg, wParam, lParam);
	case WM_CHANGECBCHAIN:
		// If the next window is closing, repair the chain.
		if ((HWND) wParam == hwndNextViewer)
			hwndNextViewer = (HWND) lParam;
		// Otherwise, pass the message to the next link.
		else if (hwndNextViewer != NULL)
			SendMessage(hwndNextViewer, iMsg, wParam, lParam);
		break;
	case WM_DRAWCLIPBOARD:  // clipboard contents changed.
		// Update the window by using Auto clipboard format.
		SendMessage(hDlg, WM_COMMAND, IDC_REFRESH, 0);
		// Pass the message to the next window in clipboard viewer chain.
		SendMessage(hwndNextViewer, iMsg, wParam, lParam);
		break;
	case WM_DESTROY:
		ChangeClipboardChain(hDlg, hwndNextViewer);
		hwndNextViewer = NULL;
		break;
	}
	return FALSE;
}

