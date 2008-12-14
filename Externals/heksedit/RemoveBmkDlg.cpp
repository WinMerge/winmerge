#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

BOOL RemoveBmkDlg::OnInitDialog(HWND hDlg)
{
	TCHAR buf[128];
	HWND hwndList = GetDlgItem(hDlg, IDC_LIST1);
	for (int i = 0 ; i < iBmkCount ; i++)
	{
		_stprintf(buf, pbmkList[i].name ? _T("%d) 0x%x:%s") : _T("%d) 0x%x"),
			i + 1, pbmkList[i].offset, pbmkList[i].name);
		SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)buf);
	}
	SendMessage(hwndList, LB_SETCURSEL, 0, 0);
	return TRUE;
}

BOOL RemoveBmkDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	int i;
	switch (wParam)
	{
	case IDOK:
		i = SendDlgItemMessage(hDlg, IDC_LIST1, LB_GETCURSEL, 0, 0);
		if (LPTSTR name = pbmkList[i].name)
			free(name);
		--iBmkCount;
		for ( ; i < iBmkCount ; i++)
			pbmkList[i] = pbmkList[i + 1];
		repaint();
		// fall through
	case IDCANCEL:
		EndDialog(hDlg, wParam);
		return TRUE;
	}
	return FALSE;
}

INT_PTR RemoveBmkDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
