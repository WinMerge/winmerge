#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

BOOL BitManipDlg::OnInitDialog(HWND hDlg)
{
	char buf[64];
	sprintf(buf, "Manipulate bits at offset 0x%x=%d", iCurByte, iCurByte);
	SetDlgItemText(hDlg, IDC_STATIC1, buf);
	BYTE cBitValue = DataArray[iCurByte];
	if (cBitValue & 1)
		CheckDlgButton(hDlg, IDC_CHECK1, BST_CHECKED);
	if (cBitValue & 2)
		CheckDlgButton(hDlg, IDC_CHECK2, BST_CHECKED);
	if (cBitValue & 4)
		CheckDlgButton(hDlg, IDC_CHECK3, BST_CHECKED);
	if (cBitValue & 8)
		CheckDlgButton(hDlg, IDC_CHECK4, BST_CHECKED);
	if (cBitValue & 16)
		CheckDlgButton(hDlg, IDC_CHECK5, BST_CHECKED);
	if (cBitValue & 32)
		CheckDlgButton(hDlg, IDC_CHECK6, BST_CHECKED);
	if (cBitValue & 64)
		CheckDlgButton(hDlg, IDC_CHECK7, BST_CHECKED);
	if (cBitValue & 128)
		CheckDlgButton(hDlg, IDC_CHECK8, BST_CHECKED);
	Apply(hDlg, 0);
	return TRUE;
}

BOOL BitManipDlg::Apply(HWND hDlg, WPARAM wParam)
{
	BYTE cBitValue = 0;
	if (IsDlgButtonChecked(hDlg, IDC_CHECK8))
		cBitValue |= 128;
	if (IsDlgButtonChecked(hDlg, IDC_CHECK7))
		cBitValue |= 64;
	if (IsDlgButtonChecked(hDlg, IDC_CHECK6))
		cBitValue |= 32;
	if (IsDlgButtonChecked(hDlg, IDC_CHECK5))
		cBitValue |= 16;
	if (IsDlgButtonChecked(hDlg, IDC_CHECK4))
		cBitValue |= 8;
	if (IsDlgButtonChecked(hDlg, IDC_CHECK3))
		cBitValue |= 4;
	if (IsDlgButtonChecked(hDlg, IDC_CHECK2))
		cBitValue |= 2;
	if (IsDlgButtonChecked(hDlg, IDC_CHECK1))
		cBitValue |= 1;
	if (wParam == IDOK)
	{
		DataArray[iCurByte] = cBitValue;
		iFileChanged = TRUE;
		bFilestatusChanged = TRUE;
		repaint();
		return TRUE;
	}
	char buf[64];
	sprintf(buf, "Value: 0x%x , %d signed, %u unsigned.",
		(unsigned char)cBitValue, (signed char)cBitValue, (unsigned char)cBitValue);
	SetDlgItemText(hDlg, IDC_STATIC2, buf);
	return FALSE;
}

INT_PTR BitManipDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hDlg);
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
		case IDC_CHECK1: case IDC_CHECK2: case IDC_CHECK3: case IDC_CHECK4:
		case IDC_CHECK5: case IDC_CHECK6: case IDC_CHECK7: case IDC_CHECK8:
			if (Apply(hDlg, wParam))
			{
			case IDCANCEL:
				EndDialog(hDlg, wParam);
			}
			return TRUE;
		}
		break;
	}
	return FALSE;
}
