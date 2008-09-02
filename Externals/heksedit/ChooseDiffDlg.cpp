#include "precomp.h"
#include "resource.h"
#include "StringTable.h"
#include "clipboard.h"
#include "hexwnd.h"
#include "hexwdlg.h"

void ChooseDiffDlg::add_diff(HWND hwndList, int diff, int lower, int upper)
{
	wchar_t buf[100];
	swprintf(buf,
		S.DiffListItemFormat, //"%d) 0x%x=%n%d to 0x%x=%n%d (%d bytes)",
		diff,
		lower, &lower, lower,
		upper, &upper, upper,
		upper - lower + 1);
	int i;
	if (IsWindowUnicode(hwndList))
	{
		i = SendMessageW(hwndList, LB_ADDSTRING, 0, (LPARAM)buf);
	}
	else
	{
		char bufA[100];
		WideCharToMultiByte(CP_ACP, 0, buf, -1, bufA, sizeof bufA, 0, 0);
		lower = WideCharToMultiByte(CP_ACP, 0, buf, lower, 0, 0, 0, 0);
		upper = WideCharToMultiByte(CP_ACP, 0, buf, upper, 0, 0, 0, 0);
		i = SendMessageA(hwndList, LB_ADDSTRING, 0, (LPARAM)bufA);
	}
	SendMessage(hwndList, LB_SETITEMDATA, i, MAKELONG(lower, upper));
}

//-------------------------------------------------------------------
// Transfer offsets of differences to pdiff.
int ChooseDiffDlg::get_diffs(HWND hwndList, char *ps, int sl, char *pd, int dl)
{
	int lower, upper;
	int i = 0, diff = 0, type = 1;
	// type=0 means differences, type=1 means equality at last char.
	while (i < sl && i < dl)
	{
		switch (type)
		{
		case 0:
			// Working on area of difference at the moment.
			if (ps[i] == pd[i])
			{
				// Chars equal again.
				add_diff(hwndList, ++diff, lower, upper);
				type = 1;
			}
			// else: chars still different.
			break;

		case 1:
			// Working on area of equality at the moment.
			if (ps[i] != pd[i])
			{
				// Start of area of difference found.
				lower = i; // Save start of area of difference.
				type = 0;
			}
			// else: chars still equal.
			break;
		}
		upper = i++;
	}
	if (type == 0) // If area of difference was at end of file.
	{
		add_diff(hwndList, ++diff, lower, upper);
	}
	return diff;
}

BOOL ChooseDiffDlg::OnInitDialog(HWND hDlg)
{
	char szFileName[_MAX_PATH];
	szFileName[0] = '\0';
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hwndOwner = hDlg;
	ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrTitle = "Choose file to compare with";
	if (!GetOpenFileName(&ofn))
		return FALSE;
	int filehandle = _open(szFileName, _O_RDONLY|_O_BINARY);
	if (filehandle == -1)
	{
		MessageBox(hDlg, "Error while opening file.", "Compare", MB_ICONERROR);
		return FALSE;
	}
	BOOL bDone = FALSE;
	if (int filelen = _filelength(filehandle))
	{
		int iDestFileLen = filelen;
		int iSrcFileLen = DataArray.GetLength() - iCurByte;
		if (char *cmpdata = new char[filelen])
		{
			// Read data.
			if (_read(filehandle, cmpdata, filelen) != -1)
			{
				HWND hwndList = GetDlgItem(hDlg, IDC_LIST1);
				if (int diff = get_diffs(hwndList, (char *)&DataArray[iCurByte], DataArray.GetLength() - iCurByte, cmpdata, filelen))
				{
					char buf[100];
					sprintf(buf, "%d areas of difference found.", diff);
					SetDlgItemText(hDlg, IDC_STATIC1, buf);
					sprintf(buf, "Remaining loaded data size: %d, size of file on disk: %d.", iSrcFileLen, iDestFileLen);
					SetDlgItemText(hDlg, IDC_STATIC2, buf);
					SendMessage(hwndList, LB_SETCURSEL, 0, 0);
					bDone = TRUE;
				}
				else
				{
					// No difference.
					MessageBox(hDlg, "Data matches exactly.", "Compare", MB_ICONINFORMATION);
				}
			}
			else
			{
				MessageBox(hDlg, "Error while reading from file.", "Compare", MB_ICONERROR);
			}
			delete[] cmpdata;
		}
		else
		{
			MessageBox(hDlg, "Not enough memory.", "Compare", MB_ICONERROR);
		}
	}
	_close(filehandle);
	return bDone;
}

BOOL ChooseDiffDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM)
{
	switch (wParam)
	{
	// By pabs.
	case IDCOPY:
		{//copy button was pressed
			if (!OpenClipboard(hwnd)) //open clip
			{
				MessageBox(hwnd,"Cannot get access to clipboard.", "Copy", MB_ICONERROR);
				return TRUE;
			}
			EmptyClipboard(); //empty clip
			IStream *piStream = 0;
			if (SUCCEEDED(CreateStreamOnHGlobal(0, FALSE, &piStream)))
			{
				HWND hwndList = GetDlgItem(hDlg, IDC_LIST1);//get the list
				CLIPFORMAT cf = IsWindowUnicode(hwndList) ? CF_UNICODETEXT : CF_TEXT;
				int num = SendMessage(hwndList, LB_GETCOUNT, 0, 0);//get the # items in the list
				for (int i = 0 ; i < num ; i++)
				{
					//add '\r\n' to the end of each line - this is '\r\n' rather than '\n' so that it can be pasted into notepad & dos programs
					if (cf == CF_UNICODETEXT)
					{
						wchar_t buf[100];
						int cch = SendMessageW(hwndList, LB_GETTEXT, i, (LPARAM)buf);
						piStream->Write(buf, cch * sizeof *buf, 0);
						static const wchar_t eol[] = L"\r\n";
						piStream->Write(eol, i < num ? sizeof eol - sizeof *eol : sizeof eol, 0);
					}
					else
					{
						char buf[100];
						int cch = SendMessageA(hwndList, LB_GETTEXT, i, (LPARAM)buf);
						piStream->Write(buf, cch * sizeof *buf, 0);
						static const char eol[] = "\r\n";
						piStream->Write(eol, i < num ? sizeof eol - sizeof *eol : sizeof eol, 0);
					}
				}
				HGLOBAL hMem = 0;
				if (SUCCEEDED(GetHGlobalFromStream(piStream, &hMem)))
				{
					SetClipboardData(cf, hMem); //copy to clip
				}
				piStream->Release();
			}
			CloseClipboard(); //close clip
		}
		return TRUE;

	case IDOK:
		{
			HWND hwndList = GetDlgItem(hDlg, IDC_LIST1);
			int i = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
			if (i != -1)
			{
				DWORD dw = SendMessage(hwndList, LB_GETITEMDATA, i, 0);
				if (IsWindowUnicode(hwndList))
				{
					wchar_t buf[100];
					SendMessageW(hwndList, LB_GETTEXT, i, (LPARAM)buf);
					iStartOfSelection = StrToIntW(buf + LOWORD(dw));
					iEndOfSelection = StrToIntW(buf + HIWORD(dw));
				}
				else
				{
					char buf[100];
					SendMessageA(hwndList, LB_GETTEXT, i, (LPARAM)buf);
					iStartOfSelection = StrToIntA(buf + LOWORD(dw));
					iEndOfSelection = StrToIntA(buf + HIWORD(dw));
				}
				iStartOfSelection += iCurByte;
				iEndOfSelection += iCurByte;
				iCurByte = iStartOfSelection;
				bSelected = TRUE;
				adjust_view_for_selection();
				repaint();
			}
		}
		// fall through
	case IDCANCEL:
		EndDialog(hDlg, wParam);
		return TRUE;
	}
	return FALSE;
}

//-------------------------------------------------------------------
INT_PTR ChooseDiffDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		if (!OnInitDialog(hDlg))
			EndDialog(hDlg, IDCANCEL);
		return TRUE;
	case WM_COMMAND:
		return OnCommand(hDlg, wParam, lParam);
	}
	return FALSE;
}

