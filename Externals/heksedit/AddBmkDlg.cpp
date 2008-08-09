#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

BOOL AddBmkDlg::OnInitDialog(HWND hDlg)
{
	char buf[32];
	sprintf(buf, "x%x", iCurByte);
	SetDlgItemText(hDlg, IDC_EDIT1, buf);
	return TRUE;
}

BOOL AddBmkDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	char buf[16];
	int i, offset;
	char name[BMKTEXTMAX];
	switch (wParam)
	{
	case IDOK:
		if (GetDlgItemText(hDlg, IDC_EDIT1, buf, 16) &&
			sscanf(buf, "x%x", &offset) == 0 &&
			sscanf(buf, "%d", &offset) == 0)
		{
			MessageBox(hDlg, "Start offset not recognized.", "Add bookmark", MB_ICONERROR);
			return TRUE;
		}
		if (offset < 0 || offset > DataArray.GetLength())
		{
			MessageBox(hDlg, "Can not set bookmark at that position.", "Add bookmark", MB_ICONERROR);
			return TRUE;
		}
		// Is there already a bookmark on this offset?
		for (i = 0 ; i < iBmkCount ; i++)
		{
			if (pbmkList[i].offset == offset)
			{
				MessageBox(hDlg, "There already is a bookmark on that position.", "Add bookmark", MB_ICONERROR);
				return TRUE;
			}
		}
		// No bookmark on that position yet.
		pbmkList[iBmkCount].offset = offset;
		pbmkList[iBmkCount].name = GetDlgItemText(hDlg, IDC_EDIT2, name, BMKTEXTMAX) ? strdup(name) : 0;
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
