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
#include "precomp.h"
#include "resource.h"
#include "Constants.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "StringTable.h"

void GetWindowText(HWindow *pwnd, SimpleString &str)
{
	int len = pwnd->GetWindowTextLength() + 1;
	str.SetSize(len);
	pwnd->GetWindowTextA(str, len);
}

void GetDlgItemText(HWindow *pwnd, int id, SimpleString &str)
{
	GetWindowText(pwnd->GetDlgItem(id), str);
}

BOOL EnableDlgItem(HWindow *pwnd, int id, BOOL enable)
{
	return pwnd->GetDlgItem(id)->EnableWindow(enable);
}

BOOL IsDlgItemEnabled(HWindow *pwnd, int id)
{
	return pwnd->GetDlgItem(id)->IsWindowEnabled();
}

int MessageBox(HWindow *pwnd, LPCTSTR text, UINT type)
{
	LPCTSTR app = GetLangString(IDS_APPNAME);
	return pwnd->MessageBox(text, app, type);
}

int CheckHResult(HWindow *pwnd, HRESULT hr, UINT type)
{
	int response = 0;
	if (FAILED(hr))
	{
		TCHAR text[1024];
		::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hr, 0, text, 1024, 0);
		response = MessageBox(pwnd, text, type);
	}
	return response;
}

C_ASSERT(sizeof(DragDropOptionsDlg) == sizeof(HexEditorWindow)); // disallow instance members

INT_PTR DragDropOptionsDlg::DlgProc(HWindow *pDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT checked = 0;
	switch (uMsg)
	{
	case WM_INITDIALOG:
		checked = enable_drag ? BST_CHECKED : BST_UNCHECKED;
		pDlg->CheckDlgButton(IDC_ENABLE_DRAG, checked);
		checked = enable_drop ? BST_CHECKED : BST_UNCHECKED;
		pDlg->CheckDlgButton(IDC_ENABLE_DROP, checked);
		checked = enable_scroll_delay_dd ? BST_CHECKED : BST_UNCHECKED;
		pDlg->CheckDlgButton(IDC_EN_SD_DD, checked);
		checked = enable_scroll_delay_sel ? BST_CHECKED : BST_UNCHECKED;
		pDlg->CheckDlgButton(IDC_EN_SD_SEL, checked);
		checked = always_pick_move_copy ? BST_CHECKED : BST_UNCHECKED;
		pDlg->CheckDlgButton(IDC_ALWAYS_CHOOSE, checked);
		checked = prefer_CF_HDROP ? BST_CHECKED : BST_UNCHECKED;
		pDlg->CheckDlgButton(IDC_DROP_CF_HDROP, checked);
		checked = prefer_CF_BINARYDATA ? BST_CHECKED : BST_UNCHECKED;
		pDlg->CheckDlgButton(IDC_DROP_BIN_DATA, checked);
		checked = prefer_CF_TEXT ? BST_CHECKED : BST_UNCHECKED;
		pDlg->CheckDlgButton(IDC_DROP_CF_TEXT, checked);
		checked = output_CF_BINARYDATA ? BST_CHECKED : BST_UNCHECKED;
		pDlg->CheckDlgButton(IDC_DRAG_BIN_DATA, checked);
		checked = output_CF_TEXT ? BST_CHECKED : BST_UNCHECKED;
		pDlg->CheckDlgButton(IDC_DRAG_CF_TEXT, checked);
		pDlg->CheckDlgButton(output_text_special ? IDC_TEXT_SPECIAL : IDC_TEXT_HEXDUMP, BST_CHECKED);
		checked = output_text_hexdump_display ? BST_CHECKED : BST_UNCHECKED;
		pDlg->CheckDlgButton(IDC_TEXT_DISPLAY, checked);
		checked = output_CF_RTF ? BST_CHECKED : BST_UNCHECKED;
		pDlg->CheckDlgButton(IDC_DRAG_RTF, checked);
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			enable_drag = pDlg->IsDlgButtonChecked(IDC_ENABLE_DRAG);
			enable_drop = pDlg->IsDlgButtonChecked(IDC_ENABLE_DROP);
			enable_scroll_delay_dd = pDlg->IsDlgButtonChecked(IDC_EN_SD_DD);
			enable_scroll_delay_sel = pDlg->IsDlgButtonChecked(IDC_EN_SD_SEL);
			always_pick_move_copy = pDlg->IsDlgButtonChecked(IDC_ALWAYS_CHOOSE);
			prefer_CF_HDROP = pDlg->IsDlgButtonChecked(IDC_DROP_CF_HDROP);
			prefer_CF_BINARYDATA = pDlg->IsDlgButtonChecked(IDC_DROP_BIN_DATA);
			prefer_CF_TEXT = pDlg->IsDlgButtonChecked(IDC_DROP_CF_TEXT);
			output_CF_BINARYDATA = pDlg->IsDlgButtonChecked(IDC_DRAG_BIN_DATA);
			output_CF_TEXT = pDlg->IsDlgButtonChecked(IDC_DRAG_CF_TEXT);
			output_text_special = pDlg->IsDlgButtonChecked(IDC_TEXT_SPECIAL);
			output_text_hexdump_display = pDlg->IsDlgButtonChecked(IDC_TEXT_DISPLAY);
			output_CF_RTF = pDlg->IsDlgButtonChecked(IDC_DRAG_RTF);

			DragAcceptFiles(pwnd->m_hWnd, !target || !enable_drop || enable_drop && prefer_CF_HDROP);

			if (target && enable_drop)
				RegisterDragDrop(pwnd->m_hWnd, target);
			else
				RevokeDragDrop(pwnd->m_hWnd);

			save_ini_data();
			// fall through
		case IDCANCEL:
			pDlg->EndDialog(wParam);
			return TRUE;
		}
		break;

	case WM_HELP:
		OnHelp(pDlg);
		break;
	}
	return FALSE;
}

INT_PTR ChangeInstDlg::DlgProc(HWindow *pDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//God damn spinners make life easy
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			int i = 0;
			HKEY hk;
			int iLoadInst = -1;
			int iSaveInst = 0;
			for (;;)
			{
				int i = iLoadInst + 1;
				const int num_size = 64;
				TCHAR num[num_size] = {0};
				_sntprintf(num, num_size - 1, _T("%s\\%d"), OptionsRegistrySettingsPath, i);
				if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, num, 0, KEY_EXECUTE, &hk))
					break;
				RegCloseKey(hk);//Close the key - just testing if it exists
				iLoadInst = i;
			}
			if (iLoadInst == -1)
			{
				MessageBox(pwnd, _T("No instance data present"), MB_OK);
				pDlg->EndDialog(IDCANCEL);
				return TRUE;
			}
			//for both the spinners
			//iLoadInst is the max
			//iSaveInst is the min
			//iInstCount is the start pos
			LONG range = MAKELONG(iLoadInst, iSaveInst);
			LONG pos = MAKELONG(iInstCount, 0);
			HWindow *pWndUpDown = pDlg->GetDlgItem(IDC_SINST);
			pWndUpDown->SendMessage(UDM_SETRANGE, 0L, range);
			pWndUpDown->SendMessage(UDM_SETPOS, 0L, pos);
			pWndUpDown = pDlg->GetDlgItem(IDC_LINST);
			pWndUpDown->SendMessage(UDM_SETRANGE, 0L, range);
			pWndUpDown->SendMessage(UDM_SETPOS, 0L, pos);
			return TRUE;
		}
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			iInstCount = static_cast<int>(pDlg->SendDlgItemMessage(IDC_LINST, UDM_GETPOS, 0, 0));
			read_ini_data();
			iInstCount = static_cast<int>(pDlg->SendDlgItemMessage(IDC_SINST, UDM_GETPOS, 0, 0));
			resize_window();
			// fall through
		case IDCANCEL:
			pDlg->EndDialog(wParam);
			return TRUE;
		}
		break;

	case WM_HELP:
		OnHelp(pDlg);
		break;
	}
	return FALSE;
}
