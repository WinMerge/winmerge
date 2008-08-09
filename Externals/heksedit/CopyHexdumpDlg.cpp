#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

int CopyHexdumpDlg::iCopyHexdumpMode = 0;
int CopyHexdumpDlg::iCopyHexdumpType = IDC_EXPORTDISPLAY;

BOOL CopyHexdumpDlg::OnInitDialog(HWND hDlg)
{
	int iCopyHexdumpDlgStart, iCopyHexdumpDlgEnd;
	if (!bSelected)
	{
		// Assume whole file is to be hexdumped. (except the last line (if incomplete))
		iCopyHexdumpDlgStart = 0;
		iCopyHexdumpDlgEnd = DataArray.GetUpperBound() / iBytesPerLine * iBytesPerLine;
	}
	else
	{
		// Assume selected area is to be hexdumped.
		// Make sure end of selection is greater than start of selection.
//Pabs changed - line remove & insert - works better now for both exporting types
		iCopyHexdumpDlgStart = iStartOfSelection ;
		iCopyHexdumpDlgEnd = iEndOfSelection ;
		if (iCopyHexdumpDlgEnd < iCopyHexdumpDlgStart)
			swap(iCopyHexdumpDlgStart, iCopyHexdumpDlgEnd);
//end
	}
	char buf[16];
	sprintf(buf, "%x", iCopyHexdumpDlgStart);
	SetDlgItemText(hDlg, IDC_EDIT1, buf);
	sprintf(buf, "%x", iCopyHexdumpDlgEnd);
	SetDlgItemText(hDlg, IDC_EDIT2, buf);
	CheckDlgButton(hDlg, iCopyHexdumpMode ? IDC_RADIO2 : IDC_RADIO1, BST_CHECKED);
//Pabs changed - line insert
	CheckDlgButton(hDlg, iCopyHexdumpType, BST_CHECKED);
//end
	return TRUE;
}

BOOL CopyHexdumpDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	int iCopyHexdumpDlgStart, iCopyHexdumpDlgEnd;
	char buf[16];
	switch (wParam)
	{
	case IDOK:
		if (GetDlgItemText(hDlg, IDC_EDIT1, buf, 16) &&
			sscanf(buf, "%x", &iCopyHexdumpDlgStart) &&
			GetDlgItemText(hDlg, IDC_EDIT2, buf, 16) &&
			sscanf(buf, "%x", &iCopyHexdumpDlgEnd))
		{
			iCopyHexdumpMode = IsDlgButtonChecked(hDlg, IDC_RADIO2);
//Pabs changed - line insert
			if (IsDlgButtonChecked(hDlg, IDC_EXPORTDISPLAY))
				iCopyHexdumpType = IDC_EXPORTDISPLAY;
			else if (IsDlgButtonChecked(hDlg, IDC_EXPORTDIGITS))
				iCopyHexdumpType = IDC_EXPORTDIGITS;
			else if (IsDlgButtonChecked(hDlg, IDC_EXPORTRTF))
				iCopyHexdumpType = IDC_EXPORTRTF;
//end
			EndDialog(hDlg, wParam);
			WaitCursor w1;
			CMD_copy_hexdump(iCopyHexdumpMode, iCopyHexdumpType, iCopyHexdumpDlgStart, iCopyHexdumpDlgEnd);
			return TRUE;
		}
		break;
	case IDCANCEL:
		EndDialog(hDlg, wParam);
		return TRUE;
	}
	return FALSE;
}
//-------------------------------------------------------------------
INT_PTR CopyHexdumpDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
