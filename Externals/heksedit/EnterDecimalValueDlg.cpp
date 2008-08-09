#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

int EnterDecimalValueDlg::iDecValDlgSize = 1;

BOOL EnterDecimalValueDlg::OnInitDialog(HWND hDlg)
{
	int iDecValDlgValue = 0;
	if (iCurByte >= 0 && iCurByte < DataArray.GetLength())
	{
		int t = DataArray.GetLength() - iCurByte;
		//Set the size down a bit if someone called this func with a size thats too large
		while (iDecValDlgSize > t)
			iDecValDlgSize /= 2;
		//Get the right value
		if (iDecValDlgSize == 2)
			iDecValDlgValue = *(WORD *)&DataArray[iCurByte];
		else if (iDecValDlgSize == 4)
			iDecValDlgValue = *(DWORD *)&DataArray[iCurByte];
		else
			iDecValDlgValue = (int)DataArray[iCurByte];
	}
	char buf[16];
	SetDlgItemInt(hDlg, IDC_EDIT1, iDecValDlgValue, TRUE);
	sprintf(buf, "x%x", iCurByte);
	SetDlgItemText(hDlg, IDC_EDIT2, buf);
	SetDlgItemInt(hDlg, IDC_EDIT3, 1, TRUE);
	CheckDlgButton(hDlg,
		iDecValDlgSize == 4 ? IDC_RADIO3 :
		iDecValDlgSize == 2 ? IDC_RADIO2 :
		IDC_RADIO1,
		BST_CHECKED);
	return TRUE;
}

BOOL EnterDecimalValueDlg::Apply(HWND hDlg)
{
	iDecValDlgSize =
		IsDlgButtonChecked(hDlg, IDC_RADIO3) ? 4 :
		IsDlgButtonChecked(hDlg, IDC_RADIO2) ? 2 :
		1;
	char buf[16];
	BOOL translated;
	int iDecValDlgValue = GetDlgItemInt(hDlg, IDC_EDIT1, &translated, TRUE);
	if (!translated)
	{
		MessageBox(hDlg, "Decimal value not recognized.", "Enter decimal value", MB_ICONERROR);
		return FALSE;
	}
	int iDecValDlgOffset;
	if (GetDlgItemText(hDlg, IDC_EDIT2, buf, 16) &&
		sscanf(buf, "%d", &iDecValDlgOffset) == 0 && 
		sscanf(buf, "x%x", &iDecValDlgOffset) == 0)
	{
		MessageBox(hDlg, "Offset not recognized.", "Enter decimal value", MB_ICONERROR);
		return FALSE;
	}
	int iDecValDlgTimes = GetDlgItemInt(hDlg, IDC_EDIT3, &translated, TRUE);
	if (!translated)
	{
		MessageBox(hDlg, "Number of times not recognized.", "Enter decimal value", MB_ICONERROR);
		return FALSE;
	}
	if (iDecValDlgOffset < 0 || iDecValDlgOffset > DataArray.GetUpperBound())
	{
		MessageBox(hDlg, "Invalid start offset.", "Enter decimal value", MB_ICONERROR);
		return FALSE;
	}
	if (iDecValDlgOffset + iDecValDlgSize * iDecValDlgTimes > DataArray.GetLength())
	{
		MessageBox(hDlg, "Not enough space for writing decimal values.", "Enter decimal value", MB_ICONERROR);
		return FALSE;
	}
	WaitCursor wc;
	while (iDecValDlgTimes)
	{
		if (iBinaryMode == LITTLEENDIAN_MODE)
		{
			switch (iDecValDlgSize)
			{
			case 1:
				DataArray[iDecValDlgOffset++] = (BYTE)iDecValDlgValue;
				break;

			case 2:
				DataArray[iDecValDlgOffset++] = (BYTE) (iDecValDlgValue & 0xff);
				DataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff00) >> 8);
				break;

			case 4:
				DataArray[iDecValDlgOffset++] = (BYTE) (iDecValDlgValue & 0xff);
				DataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff00) >> 8);
				DataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff0000) >> 16);
				DataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff000000) >> 24);
				break;
			}
		}
		else
		{
			switch (iDecValDlgSize)
			{
			case 1:
				DataArray[iDecValDlgOffset++] = (BYTE)iDecValDlgValue;
				break;

			case 2:
				DataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff00) >> 8);
				DataArray[iDecValDlgOffset++] = (BYTE) (iDecValDlgValue & 0xff);
				break;

			case 4:
				DataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff000000) >> 24);
				DataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff0000) >> 16);
				DataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff00) >> 8);
				DataArray[iDecValDlgOffset++] = (BYTE) (iDecValDlgValue & 0xff);
				break;
			}
		}
		--iDecValDlgTimes;
	}
	iFileChanged = TRUE;
	bFilestatusChanged = TRUE;
	repaint();
	return TRUE;
}

INT_PTR EnterDecimalValueDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hDlg);
	case WM_COMMAND:
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
		break;
	}
	return FALSE;
}
