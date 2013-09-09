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
 * @file  FastPasteDlg.cpp
 *
 * @brief Implementation of the Fast Paste dialog.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "BinTrans.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "StringTable.h"

static HWND hwndNextViewer = NULL;

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE.
 */
BOOL FastPasteDlg::OnInitDialog(HWindow *pDlg)
{
	RefreshClipboardFormats(pDlg);
	hwndNextViewer = SetClipboardViewer(pDlg->m_hWnd);
	pDlg->SetDlgItemInt(IDC_FPASTE_PASTETIMES, iPasteTimes);
	pDlg->SetDlgItemInt(IDC_FPASTE_SKIPS, iPasteSkip);
	// Depending on INS or OVR mode, set the radio button.
	if (bSelected) // iPasteMode = 0
	{
		pDlg->GetDlgItem(IDC_FPASTE_OWERWRITE)->EnableWindow(FALSE);
		pDlg->GetDlgItem(IDC_FPASTE_INSERT)->EnableWindow(FALSE);
	}
	else if (bInsertMode) // iPasteMode = 2
	{
		pDlg->CheckDlgButton(IDC_FPASTE_INSERT, BST_CHECKED);
	}
	else // iPasteMode = 1
	{
		pDlg->CheckDlgButton(IDC_FPASTE_OWERWRITE, BST_CHECKED);
	}
	if (bPasteAsText)
		pDlg->CheckDlgButton(IDC_FPASTE_TXT, BST_CHECKED);
	else
		pDlg->CheckDlgButton(IDC_FPASTE_TXT, BST_UNCHECKED);

	// Limit edit text lengths
	pDlg->SendDlgItemMessage(IDC_FPASTE_PASTETIMES, EM_SETLIMITTEXT, 16);
	pDlg->SendDlgItemMessage(IDC_FPASTE_SKIPS, EM_SETLIMITTEXT, 16);

	return TRUE;
}

/**
 * @brief Paste the bytes to the editor.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE if bytes were pasted, FALSE if failed.
 */
BOOL FastPasteDlg::Apply(HWindow *pDlg)
{
	bPasteAsText = pDlg->IsDlgButtonChecked(IDC_FPASTE_TXT) == BST_CHECKED;
	iPasteTimes = pDlg->GetDlgItemInt(IDC_FPASTE_PASTETIMES);
	if (iPasteTimes <= 0)
	{
		MessageBox(pDlg, GetLangString(IDS_PASTE_ATLEAST_ONCE), MB_ICONERROR);
		return FALSE;
	}
	iPasteSkip = pDlg->GetDlgItemInt(IDC_FPASTE_SKIPS, 0, TRUE);
	HListBox *list = static_cast<HListBox *>(pDlg->GetDlgItem(IDC_FPASTE_CFORMAT));
	int i = list->GetCurSel();
	if (i == LB_ERR)
	{
		MessageBox(pDlg, GetLangString(IDS_CLIPBOARD_SELECT_FORMAT), MB_ICONERROR);
		return FALSE;
	}
	UINT uFormat = list->GetItemData(i);
	char *pcPastestring = 0;
	int destlen = 0;
	BOOL bPasteBinary = FALSE;
	BOOL bPasteUnicode = FALSE;
	if (OpenClipboard(NULL))
	{
		if (HGLOBAL hClipMemory = GetClipboardData(uFormat))
		{
			int gsize = 0;
			if (SIZE_T size = GlobalSize(hClipMemory))
			{
				gsize = size <= INT_MAX ? static_cast<int>(size) : INT_MAX;
				void *pClipMemory = GlobalLock(hClipMemory);
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
		if (bPasteAsText)
		{
			destlen = bPasteUnicode ? 2 * wcslen((WCHAR *)pcPastestring) : strlen(pcPastestring);
		}
		else
		{
			BYTE *pc = 0;
			destlen = create_bc_translation(&pc, pcPastestring,
				strlen(pcPastestring), iCharacterSet, iBinaryMode);
			delete [] pcPastestring;
			pcPastestring = (char *)pc;
		}
	}
	if (destlen == 0)
	{
		MessageBox(pDlg, GetLangString(IDS_PASTE_WAS_EMPTY), MB_ICONERROR);
		delete [] pcPastestring;
		return FALSE;
	}
	WaitCursor wc1;
	if (bSelected || pDlg->IsDlgButtonChecked(IDC_FPASTE_INSERT))
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
 * @brief Refresh the clipboard formats list.
 * This function gets all supported clipboard formats and updates the list to
 * the dialog.
 * @param [in] hDlg Handle to the dialog.
 */
void FastPasteDlg::RefreshClipboardFormats(HWindow *pDlg)
{
	HListBox *const list = static_cast<HListBox *>(pDlg->GetDlgItem(IDC_FPASTE_CFORMAT));
	list->ResetContent();
	if (CountClipboardFormats() && OpenClipboard(NULL))
	{
		UINT uFormat;
		int i;
		TCHAR szFormatName[100];
		TCHAR SetSel = 0;
		LPTSTR lpFormatName;

		uFormat = EnumClipboardFormats(0);
		while (uFormat)
		{
			lpFormatName = NULL;

			// For registered formats, get the registered name.
			if (GetClipboardFormatName(uFormat, szFormatName, sizeof(szFormatName)))
				lpFormatName = szFormatName;
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
						if (i != i)
							;
						#define CASE(a) else if(uFormat>a##FIRST&&uFormat<a##LAST) _stprintf(szFormatName,_T(#a) _T("%d"),uFormat-a##FIRST);
							CASE(CF_PRIVATE) CASE(CF_GDIOBJ)
						#undef CASE
						/*Format ideas for future: hex number, system/msdn constant, registered format, WM_ASKFORMATNAME, tickbox for delay rendered or not*/
						/*else if(uFormat>0xC000&&uFormat<0xFFFF){
							sprintf(szFormatName,"CF_REGISTERED%d",uFormat-0xC000);
						}*/
						else
							break;
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
			if (lpFormatName)
			{
				i = list->AddString(lpFormatName);
				if (SetSel == 1)
				{
					SetSel = 2;
					list->SetCurSel(i);
				}
				list->SetItemData(i, uFormat);
			}

			uFormat = EnumClipboardFormats(uFormat);
		}
		CloseClipboard();
		if (!SetSel)
			list->SetCurSel(0);
	}
}

/**
 * @brief Dialog control message handler.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] wParam Control ID.
 */
BOOL FastPasteDlg::OnCommand(HWindow *pDlg, WPARAM wParam, LPARAM)
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
	case IDC_FPASTE_REFRESH:
		RefreshClipboardFormats(pDlg);
		return TRUE;
	case MAKEWPARAM(IDC_FPASTE_CFORMAT, LBN_SELCHANGE):
		{
			HListBox *const list = static_cast<HListBox *>(pDlg->GetDlgItem(IDC_FPASTE_CFORMAT));
			int i = list->GetCurSel();
			UINT f = list->GetItemData(i);
			if (f == CF_UNICODETEXT)
				pDlg->CheckDlgButton(IDC_FPASTE_TXT, BST_CHECKED);
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
INT_PTR FastPasteDlg::DlgProc(HWindow *pDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(pDlg);
	case WM_COMMAND:
		return OnCommand(pDlg, wParam, lParam);
	case WM_CHANGECBCHAIN:
		// If the next window is closing, repair the chain.
		if ((HWND)wParam == hwndNextViewer)
			hwndNextViewer = (HWND)lParam;
		// Otherwise, pass the message to the next link.
		else if (hwndNextViewer != NULL)
			SendMessage(hwndNextViewer, uMsg, wParam, lParam);
		break;
	case WM_DRAWCLIPBOARD:  // clipboard contents changed.
		// Update the window by using Auto clipboard format.
		pDlg->SendMessage(WM_COMMAND, IDC_FPASTE_REFRESH, 0);
		// Pass the message to the next window in clipboard viewer chain.
		SendMessage(hwndNextViewer, uMsg, wParam, lParam);
		break;
	case WM_DESTROY:
		ChangeClipboardChain(pDlg->m_hWnd, hwndNextViewer);
		hwndNextViewer = NULL;
		break;

	case WM_HELP:
		OnHelp(pDlg);
		break;
	}
	return FALSE;
}
