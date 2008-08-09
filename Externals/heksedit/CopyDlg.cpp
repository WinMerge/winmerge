#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "BinTrans.h"

BOOL CopyDlg::OnInitDialog(HWND hDlg)
{
	int iStart = iGetStartOfSelection();
	int iEnd = iGetEndOfSelection();
	char buf[32];
	sprintf(buf, "x%x", iStart);
	SetDlgItemText(hDlg, IDC_EDIT1, buf);
	sprintf(buf, "x%x", iEnd);
	SetDlgItemText(hDlg, IDC_EDIT2, buf);
	SetDlgItemInt(hDlg, IDC_EDIT3, iEnd - iStart + 1, TRUE);
	CheckDlgButton(hDlg, IDC_RADIO1, BST_CHECKED);
	return TRUE;
}

BOOL CopyDlg::Apply(HWND hDlg)
{
	char buf[64];
	int iOffset;
	int iNumberOfBytes;
	if (GetDlgItemText(hDlg, IDC_EDIT1, buf, 64) &&
		sscanf(buf, "x%x", &iOffset) == 0 &&
		sscanf(buf, "%d", &iOffset) == 0)
	{
		MessageBox(hDlg, "Start offset not recognized.", "Copy", MB_ICONERROR);
		return FALSE;
	}
	if (IsDlgButtonChecked(hDlg, IDC_RADIO1))
	{
		if (GetDlgItemText(hDlg, IDC_EDIT2, buf, 64) &&
			sscanf(buf, "x%x", &iNumberOfBytes) == 0 &&
			sscanf(buf, "%d", &iNumberOfBytes) == 0)
		{
			MessageBox(hDlg, "End offset not recognized.", "Copy", MB_ICONERROR);
			return FALSE;
		}
		iNumberOfBytes = iNumberOfBytes - iOffset + 1;
	}
	else
	{// Get number of bytes.
		if (GetDlgItemText(hDlg, IDC_EDIT3, buf, 64) &&
			sscanf(buf, "%d", &iNumberOfBytes) == 0)
		{
			MessageBox(hDlg, "Number of bytes not recognized.", "Copy", MB_ICONERROR);
			return FALSE;
		}
	}
	// Can requested number be cut?
	// DataArray.GetLength ()-iCutOffset = number of bytes from current pos. to end.
	if (DataArray.GetLength() - iOffset < iNumberOfBytes)
	{
		MessageBox(hDlg, "Can't copy more bytes than are present.", "Copy", MB_ICONERROR);
		return FALSE;
	}
	// Transfer to cipboard.
	int destlen = Text2BinTranslator::iBytes2BytecodeDestLen((char*) &DataArray[iOffset], iNumberOfBytes);
	HGLOBAL hGlobal = GlobalAlloc(GHND, destlen);
	if (hGlobal == 0)
	{
		// Not enough memory for clipboard.
		MessageBox(hDlg, "Not enough memory for copying.", "Copy", MB_ICONERROR);
		return FALSE;
	}
	WaitCursor wc;
	char *pd = (char *)GlobalLock(hGlobal);
	Text2BinTranslator::iTranslateBytesToBC(pd, &DataArray[iOffset], iNumberOfBytes);
	GlobalUnlock(hGlobal);
	OpenClipboard(hwnd);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hGlobal);
	CloseClipboard();
	return TRUE;
}

INT_PTR CopyDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
		case IDC_RADIO1:
		case IDC_RADIO2:
			EnableWindow(GetDlgItem(hDlg, IDC_EDIT2), IsDlgButtonChecked(hDlg, IDC_RADIO1));
			EnableWindow(GetDlgItem(hDlg, IDC_EDIT3), IsDlgButtonChecked(hDlg, IDC_RADIO2));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
