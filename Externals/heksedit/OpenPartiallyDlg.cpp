#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

int OpenPartiallyDlg::filehandle = 0;
int OpenPartiallyDlg::bShowFileStatsPL = 0;

BOOL OpenPartiallyDlg::OnInitDialog(HWND hDlg)
{
	int iPLFileLen = _filelength(filehandle);
	char buf[128];
	SetDlgItemText(hDlg, IDC_EDIT1, "x0");
	sprintf(buf, "Size of file: %l. Load how many bytes:", iPLFileLen);
	SetDlgItemText(hDlg, IDC_STATIC2, buf);
	SetDlgItemInt(hDlg, IDC_EDIT2, iPLFileLen, FALSE);
	CheckDlgButton(hDlg, IDC_RADIO1, BST_CHECKED);
	CheckDlgButton(hDlg, IDC_CHECK1, bShowFileStatsPL);
	return TRUE;
}

BOOL OpenPartiallyDlg::Apply(HWND hDlg)
{
	int iPLFileLen = _filelength(filehandle);
	bShowFileStatsPL = IsDlgButtonChecked(hDlg, IDC_CHECK1);
	char buf[128];
	int iNumBytesPl;
	// Only complain about wrong offset in start offset editbox if loading from start.
	if (GetDlgItemText(hDlg, IDC_EDIT2, buf, 128) &&
		sscanf(buf, "%u", &iNumBytesPl) == 0)
	{
		MessageBox(hDlg, "Number of bytes not recognized.", "Open partially", MB_ICONERROR);
		return FALSE;
	}
	int iStartPL;
	if (IsDlgButtonChecked(hDlg, IDC_RADIO2))
	{
		// Load from end of file: arguments must be adapted.
		iStartPL = iPLFileLen - iNumBytesPl;
		if (iStartPL < 0)
		{
			MessageBox(hDlg, "Specified number of bytes to load\ngreater than file size.", "Open partially", MB_ICONERROR);
			return FALSE;
		}
	}
	else if (GetDlgItemText(hDlg, IDC_EDIT1, buf, 128) &&
		sscanf(buf, "x%x", &iStartPL) == 0 &&
		sscanf(buf, "%u", &iStartPL) == 0)
	{
		MessageBox(hDlg, "Start offset not recognized.", "Open partially", MB_ICONERROR);
		return FALSE;
	}
	if (iStartPL + iNumBytesPl > iPLFileLen)
	{
		MessageBox(hDlg, "Too many bytes to load.", "Open partially", MB_ICONERROR);
		return FALSE;
	}
	//int filehandle = _open(szFileName, _O_RDONLY|_O_BINARY);
	/*if (filehandle == -1)
	{
		char buf[500];
		sprintf(buf, "Error code 0x%x occured while opening file %s.", errno, szFileName);
		MessageBox(hDlg, buf, "Open partially", MB_ICONERROR);
		return FALSE;
	}*/
	BOOL done = FALSE;
	//DataArray.ClearAll();
	//CMD_new
	if (DataArray.SetSize(iNumBytesPl))
	{
		DataArray.ExpandToSize();
		_lseek(filehandle, iStartPL, 0);
		iPartialOffset = iStartPL;
		iPartialOpenLen = iNumBytesPl;
		iPartialFileLen = iPLFileLen;
		bPartialStats = bShowFileStatsPL;
		if (_read(filehandle, DataArray, iNumBytesPl) != -1)
		{
			done = TRUE;
		}
		else
		{
			DataArray.ClearAll();
			MessageBox(hDlg, "Error while reading from file.", "Open partially", MB_ICONERROR);
		}
	}
	else
	{
		MessageBox(hDlg, "Not enough memory to load file.", "Open partially", MB_ICONERROR);
	}
	return done;
}

INT_PTR OpenPartiallyDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
			EnableWindow(GetDlgItem(hDlg, IDC_EDIT1), IsDlgButtonChecked(hDlg, IDC_RADIO1));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
