#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

BOOL AppendDlg::Apply(HWND hDlg)
{
	char buf[64];
	int iAppendbytes;
	if (GetDlgItemText(hDlg, IDC_EDIT1, buf, 64) &&
		sscanf(buf, "%d", &iAppendbytes) == 0)
	{
		MessageBox(hDlg, "Number of bytes to append not recognized.", "Append", MB_ICONERROR);
		return FALSE;
	}
	int oldupbound = DataArray.GetLength();
	if (!DataArray.SetSize(DataArray.GetSize() + iAppendbytes))
	{
		MessageBox(hDlg, "Not enough memory for appending.", "Append", MB_ICONERROR);
		return FALSE;
	}
	DataArray.ExpandToSize();
	WaitCursor wc;
	for (int i = 0 ; i < iAppendbytes ; i++)
		DataArray[oldupbound + i] = 0;
	iFileChanged = TRUE;
	bFilestatusChanged = TRUE;
	resize_window();
	return TRUE;
}

INT_PTR AppendDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, IDC_EDIT1, 1, TRUE);
		return TRUE;
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
