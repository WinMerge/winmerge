#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "BinTrans.h"

int CutDlg::iCutMode = 1;

BOOL CutDlg::OnInitDialog(HWND hDlg)
{
	int iStart = iGetStartOfSelection();
	int iEnd = iGetEndOfSelection();
	TCHAR buf[32];
	_stprintf(buf, _T("x%x"), iStart);
	SetDlgItemText(hDlg, IDC_CUT_STARTOFFSET, buf);
	_stprintf(buf, _T("x%x"), iEnd);
	SetDlgItemText(hDlg, IDC_CUT_NUMBYTES, buf);
	SetDlgItemInt(hDlg, IDC_CUT_NUMBYTES2, iEnd - iStart + 1, TRUE);
	CheckDlgButton(hDlg, IDC_CUT_CLIPBOARD, iCutMode);
	CheckDlgButton(hDlg, IDC_CUT_INCLUDEOFFSET, BST_CHECKED);
	return TRUE;
}

BOOL CutDlg::Apply(HWND hDlg)
{
	TCHAR buf[32];
	int iOffset;
	int iNumberOfBytes;
	if (GetDlgItemText(hDlg, IDC_CUT_STARTOFFSET, buf, RTL_NUMBER_OF(buf)) &&
		_stscanf(buf, _T("x%x"), &iOffset) == 0 &&
		_stscanf(buf, _T("%d"), &iOffset) == 0)
	{
		MessageBox(hDlg, _T("Start offset not recognized."), _T("Cut"), MB_ICONERROR);
		return FALSE;
	}
	if (IsDlgButtonChecked(hDlg, IDC_CUT_INCLUDEOFFSET))
	{
		if (GetDlgItemText(hDlg, IDC_CUT_NUMBYTES, buf, RTL_NUMBER_OF(buf)) &&
			_stscanf(buf, _T("x%x"), &iNumberOfBytes) == 0 &&
			_stscanf(buf, _T("%d"), &iNumberOfBytes) == 0)
		{
			MessageBox(hDlg, _T("End offset not recognized."), _T("Cut"), MB_ICONERROR);
			return FALSE;
		}
		iNumberOfBytes = iNumberOfBytes - iOffset + 1;
	}
	else
	{// Get number of bytes.
		if (GetDlgItemText(hDlg, IDC_CUT_NUMBYTES2, buf, RTL_NUMBER_OF(buf)) &&
			_stscanf(buf, _T("%d"), &iNumberOfBytes) == 0)
		{
			MessageBox(hDlg, _T("Number of bytes not recognized."), _T("Cut"), MB_ICONERROR);
			return FALSE;
		}
	}
	iCutMode = IsDlgButtonChecked(hDlg, IDC_CUT_CLIPBOARD);
	// Can requested number be cut?
	// DataArray.GetLength ()-iCutOffset = number of bytes from current pos. to end.
	if (DataArray.GetLength() - iOffset < iNumberOfBytes)
	{
		MessageBox(hDlg, _T("Can't cut more bytes than are present."), _T("Cut"), MB_ICONERROR);
		return FALSE;
	}
	// OK
	//int newlen = DataArray.GetLength () - iCutNumberOfBytes;
	// Cut to clipboard?
	if (iCutMode)
	{
		// Transfer to cipboard.
		int destlen = Text2BinTranslator::iBytes2BytecodeDestLen((char*) &DataArray[iOffset], iNumberOfBytes);
		HGLOBAL hGlobal = GlobalAlloc(GHND, destlen);
		if (hGlobal == 0)
		{
			// Not enough memory for clipboard.
			MessageBox(hDlg, _T("Not enough memory for cutting to clipboard."), _T("Cut"), MB_ICONERROR);
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
	}
	// Cut data.
	if (!DataArray.RemoveAt(iOffset, iNumberOfBytes))
	{
		MessageBox(hDlg, _T("Could not cut data."), _T("Cut"), MB_ICONERROR);
		return FALSE;
	}
	iCurByte = iOffset;
	if (iCurByte > DataArray.GetUpperBound())
		iCurByte = DataArray.GetUpperBound();
	if (iCurByte < 0)
		iCurByte = 0;
	iFileChanged = TRUE;
	bFilestatusChanged = TRUE;
	bSelected = FALSE;
	resize_window();
	return TRUE;
}

INT_PTR CutDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
		case IDC_CUT_INCLUDEOFFSET:
		case IDC_CUT_NUMBYTES:
			EnableWindow(GetDlgItem(hDlg, IDC_CUT_NUMBYTES), IsDlgButtonChecked(hDlg, IDC_CUT_INCLUDEOFFSET));
			EnableWindow(GetDlgItem(hDlg, IDC_CUT_NUMBYTES2), IsDlgButtonChecked(hDlg, IDC_CUT_NUMBYTES));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
