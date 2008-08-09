#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

char GoToDlg::buffer[16];

BOOL GoToDlg::Apply(HWND hDlg)
{
	int offset, i = 0, r = 0;
	GetDlgItemText(hDlg, IDC_EDIT1, buffer, sizeof buffer);
	// For a relative jump, read offset from 2nd character on.
	if (buffer[0] == '+' || buffer[0] == '-')
		r = 1;
	if (sscanf(buffer + r, "x%x", &offset) == 0 &&
		sscanf(buffer + r, "%d", &offset) == 0)
	{
		MessageBox(hDlg, "Offset not recognized.", "Go to", MB_ICONERROR);
		return FALSE;
	}
	if (r)
	{
		// Relative jump.
		if (buffer[0] == '-' )
			offset = -offset;
		offset += iCurByte;
	}
	// Absolute jump.
	if (offset < 0 || offset >= DataArray.GetLength())
	{
		MessageBox(hDlg, "Invalid offset.", "Go to", MB_ICONERROR);
		return FALSE;
	}
	iCurByte = offset;
	if (snap_caret())
		repaint();
	return TRUE;
}

INT_PTR GoToDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_EDIT1, buffer);
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
