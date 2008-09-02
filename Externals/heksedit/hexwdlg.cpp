#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "LangArray.h"

void GetWindowText(HWND hwnd, SimpleString &str)
{
	int len = GetWindowTextLength(hwnd) + 1;
	str.SetSize(len);
	GetWindowText(hwnd, str, len);
}

void GetDlgItemText(HWND hwnd, int id, SimpleString &str)
{
	hwnd = GetDlgItem(hwnd, id);
	GetWindowText(hwnd, str);
}

C_ASSERT(sizeof(DragDropOptionsDlg) == sizeof(HexEditorWindow)); // disallow instance members

INT_PTR DragDropOptionsDlg::DlgProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	switch (m)
	{
	case WM_INITDIALOG:
		CheckDlgButton(h, IDC_ENABLE_DRAG, enable_drag);
		CheckDlgButton(h, IDC_ENABLE_DROP, enable_drop);
		CheckDlgButton(h, IDC_EN_SD_DD, enable_scroll_delay_dd);
		CheckDlgButton(h, IDC_EN_SD_SEL, enable_scroll_delay_sel);
		CheckDlgButton(h, IDC_ALWAYS_CHOOSE, always_pick_move_copy);
		CheckDlgButton(h, IDC_DROP_CF_HDROP, prefer_CF_HDROP);
		CheckDlgButton(h, IDC_DROP_BIN_DATA, prefer_CF_BINARYDATA);
		CheckDlgButton(h, IDC_DROP_CF_TEXT, prefer_CF_TEXT);
		CheckDlgButton(h, IDC_DRAG_BIN_DATA, output_CF_BINARYDATA);
		CheckDlgButton(h, IDC_DRAG_CF_TEXT, output_CF_TEXT);
		CheckDlgButton(h, output_text_special ? IDC_TEXT_SPECIAL : IDC_TEXT_HEXDUMP, TRUE);
		CheckDlgButton(h, IDC_TEXT_DISPLAY, output_text_hexdump_display);
		CheckDlgButton(h, IDC_DRAG_RTF, output_CF_RTF);
		return TRUE;
	case WM_COMMAND:
		switch (w)
		{
		case IDOK:
			enable_drag = IsDlgButtonChecked(h, IDC_ENABLE_DRAG);
			enable_drop = IsDlgButtonChecked(h, IDC_ENABLE_DROP);
			enable_scroll_delay_dd = IsDlgButtonChecked(h, IDC_EN_SD_DD);
			enable_scroll_delay_sel = IsDlgButtonChecked(h, IDC_EN_SD_SEL);
			always_pick_move_copy = IsDlgButtonChecked(h, IDC_ALWAYS_CHOOSE);
			prefer_CF_HDROP = IsDlgButtonChecked(h, IDC_DROP_CF_HDROP);
			prefer_CF_BINARYDATA = IsDlgButtonChecked(h, IDC_DROP_BIN_DATA);
			prefer_CF_TEXT = IsDlgButtonChecked(h, IDC_DROP_CF_TEXT);
			output_CF_BINARYDATA = IsDlgButtonChecked( h, IDC_DRAG_BIN_DATA);
			output_CF_TEXT = IsDlgButtonChecked(h, IDC_DRAG_CF_TEXT);
			output_text_special = IsDlgButtonChecked(h, IDC_TEXT_SPECIAL);
			output_text_hexdump_display = IsDlgButtonChecked(h, IDC_TEXT_DISPLAY);
			output_CF_RTF = IsDlgButtonChecked(h, IDC_DRAG_RTF);

			DragAcceptFiles(hwnd, !target || !enable_drop || enable_drop && prefer_CF_HDROP);

			if (target && enable_drop)
				RegisterDragDrop(hwnd, target);
			else
				RevokeDragDrop(hwnd);

			save_ini_data();
			// fall through
		case IDCANCEL:
			EndDialog(h, w);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

INT_PTR CharacterSetDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, IDC_EDIT1, iFontSize, TRUE);
		switch (iCharacterSet)
		{
		case ANSI_FIXED_FONT:
			CheckDlgButton(hDlg, IDC_RADIO1, BST_CHECKED);
			break;
		case OEM_FIXED_FONT:
			CheckDlgButton(hDlg, IDC_RADIO2, BST_CHECKED);
			break;
		}
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			iFontSize = GetDlgItemInt(hDlg, IDC_EDIT1, 0, TRUE);
			iCharacterSet = IsDlgButtonChecked(hDlg, IDC_RADIO1) ?
				ANSI_FIXED_FONT : OEM_FIXED_FONT;
			save_ini_data();
			resize_window();
			// fall through
		case IDCANCEL:
			EndDialog(hDlg, wParam);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

INT_PTR BinaryModeDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		CheckDlgButton(hDlg, iBinaryMode == LITTLEENDIAN_MODE ? IDC_RADIO1 : IDC_RADIO2, BST_CHECKED);
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			iBinaryMode = IsDlgButtonChecked(hDlg, IDC_RADIO1) ? LITTLEENDIAN_MODE : BIGENDIAN_MODE;
			// fall through
		case IDCANCEL:
			EndDialog(hDlg, wParam);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

INT_PTR ChangeInstDlg::DlgProc(HWND hw, UINT m, WPARAM w, LPARAM l)
{
	//God damn spinners make life easy
	switch(m)
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
				char num[64];
				sprintf(num, "Software\\frhed\\v"CURRENT_VERSION"." SUB_RELEASE_NO "\\%d", i);
				if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, num, 0, KEY_EXECUTE, &hk))
					break;
				RegCloseKey(hk);//Close the key - just testing if it exists
				iLoadInst = i;
			}
			if (iLoadInst == -1)
			{
				MessageBox(hwnd,"No instance data present","Change Instance",MB_OK);
				EndDialog(hw, IDCANCEL);
				return TRUE;
			}
			//for both the spinners
			//iLoadInst is the max
			//iSaveInst is the min
			//iInstCount is the start pos
			LONG range = MAKELONG(iLoadInst, iSaveInst);
			LONG pos = MAKELONG(iInstCount, 0);
			HWND hWndUpDown = GetDlgItem(hw, IDC_SINST);
			SendMessage(hWndUpDown, UDM_SETRANGE, 0L, range);
			SendMessage(hWndUpDown, UDM_SETPOS, 0L, pos);
			hWndUpDown = GetDlgItem(hw, IDC_LINST);
			SendMessage(hWndUpDown, UDM_SETRANGE, 0L, range);
			SendMessage(hWndUpDown, UDM_SETPOS, 0L, pos);
			return TRUE;
		}
	case WM_COMMAND:
		switch (w)
		{
		case IDOK:
			iInstCount = SendDlgItemMessage(hw,IDC_LINST,UDM_GETPOS,0,0);
			read_ini_data();
			iInstCount = SendDlgItemMessage(hw,IDC_SINST,UDM_GETPOS,0,0);
			resize_window();
			// fall through
		case IDCANCEL:
			EndDialog(hw, w);
			return TRUE;
		}
	}
	return FALSE;
}
