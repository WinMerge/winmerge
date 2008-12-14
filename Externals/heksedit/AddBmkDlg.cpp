#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

BOOL AddBmkDlg::OnInitDialog(HWND hDlg)
{
	TCHAR buf[32];
	_stprintf(buf, _T("x%x"), iCurByte);
	SetDlgItemText(hDlg, IDC_EDIT1, buf);
	return TRUE;
}

BOOL AddBmkDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	TCHAR buf[32];
	int i, offset;
	TCHAR name[BMKTEXTMAX];
	switch (wParam)
	{
	case IDOK:
		if (GetDlgItemText(hDlg, IDC_EDIT1, buf, RTL_NUMBER_OF(buf)) &&
			_stscanf(buf, _T("x%x"), &offset) == 0 &&
			_stscanf(buf, _T("%d"), &offset) == 0)
		{
			MessageBox(hDlg, _T("Start offset not recognized."), _T("Add bookmark"), MB_ICONERROR);
			return TRUE;
		}
		if (offset < 0 || offset > DataArray.GetLength())
		{
			MessageBox(hDlg, _T("Can not set bookmark at that position."), _T("Add bookmark"), MB_ICONERROR);
			return TRUE;
		}
		// Is there already a bookmark on this offset?
		for (i = 0 ; i < iBmkCount ; i++)
		{
			if (pbmkList[i].offset == offset)
			{
				MessageBox(hDlg, _T("There already is a bookmark on that position."), _T("Add bookmark"), MB_ICONERROR);
				return TRUE;
			}
		}
		// No bookmark on that position yet.
		pbmkList[iBmkCount].offset = offset;
		pbmkList[iBmkCount].name = GetDlgItemText(hDlg, IDC_EDIT2, name, BMKTEXTMAX) ? _tcsdup(name) : 0;
		iBmkCount++;
		repaint();
		// fall through
	case IDCANCEL:
		EndDialog(hDlg, wParam);
		return TRUE;
	}
	return FALSE;
}

INT_PTR AddBmkDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
