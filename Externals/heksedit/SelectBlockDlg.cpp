#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

BOOL SelectBlockDlg::OnInitDialog(HWND hDlg)
{
	char buf[128];
	sprintf(buf, "x%x", bSelected ? iStartOfSelection : iCurByte);
	SetDlgItemText(hDlg, IDC_EDIT1, buf);
	sprintf(buf, "x%x", bSelected ? iEndOfSelection : iCurByte);
	SetDlgItemText(hDlg, IDC_EDIT2, buf);
	return TRUE;
}

BOOL SelectBlockDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	char buf[128];
	int iStartOfSelSetting;
	int iEndOfSelSetting;
	int maxb;
	switch (wParam)
	{
	case IDOK:
		if (GetDlgItemText(hDlg, IDC_EDIT1, buf, 128) &&
			sscanf(buf, "x%x", &iStartOfSelSetting) == 0 &&
			sscanf(buf, "%d", &iStartOfSelSetting) == 0)
		{
			MessageBox(hDlg, "Start offset not recognized.", "Select block", MB_ICONERROR);
			return TRUE;
		}
		if (GetDlgItemText(hDlg, IDC_EDIT2, buf, 128) &&
			sscanf(buf, "x%x", &iEndOfSelSetting) == 0 &&
			sscanf(buf, "%d", &iEndOfSelSetting) == 0)
		{
			MessageBox(hDlg, "End offset not recognized.", "Select block", MB_ICONERROR);
			return TRUE;
		}
		//make the selection valid if it is not
		maxb = DataArray.GetUpperBound();
		if (iStartOfSelSetting < 0)
			iStartOfSelSetting = 0;
		if (iStartOfSelSetting > maxb)
			iStartOfSelSetting = maxb;
		if (iEndOfSelSetting < 0)
			iEndOfSelSetting = 0;
		if (iEndOfSelSetting > maxb)
			iEndOfSelSetting = maxb;
		iStartOfSelection = iStartOfSelSetting;
		iEndOfSelection = iEndOfSelSetting;
		bSelected = TRUE;
		adjust_view_for_selection();
		repaint();
		// fall through
	case IDCANCEL:
		EndDialog(hDlg, wParam);
		return TRUE;
	}
	return FALSE;
}

INT_PTR SelectBlockDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
