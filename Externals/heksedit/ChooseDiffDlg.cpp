#include "precomp.h"
#include "resource.h"
#include "clipboard.h"
#include "hexwnd.h"
#include "hexwdlg.h"

void ChooseDiffDlg::add_diff(HWND hwndList, int diff, int lower, int upper)
{
	char buf[100];
	sprintf(buf, "%d) 0x%x=%n%d to 0x%x=%n%d (%d bytes)", diff,
		lower, &lower, lower,
		upper, &upper, upper,
		upper - lower + 1);
	int i = SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)buf);
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
		switch (wParam)
		{
			// By pabs.
			case IDCOPY:
				{//copy button was pressed
					int sumlen=1;//length of buffer initially is 1 for the '\0'
					int len=0;//length of current string
					char*buf=(char*)malloc(1);//buffer = '\0'
					buf[0]=0;//init buffer with '\0'
					char*bt=NULL;//temporary pointer - used so that if realloc returns NULL buf does not lose its value
					HWND hwndList = GetDlgItem (hDlg, IDC_LIST1);//get the list
					int num = SendMessage(hwndList,LB_GETCOUNT,0,0);//get the # items in the list
					for(int i=0;i<num;i++)
					{	//loop num times
						len=SendMessage(hwndList,LB_GETTEXTLEN,i,0)+2;//get sise of next line +2 is for '\r\n' at the end of each line
						sumlen+=len;//add len to the buffer sise
						bt = (char*)realloc(buf,sumlen);//resise buffer
						if (bt == NULL)//not enough mem to re-alloc buffer
							break;//exit loop without changing buffer address
						buf = bt;//realloc succesful overwrite buffer address
						// the -1 is to counteract the initialisation of sumlen
						SendMessage(hwndList,LB_GETTEXT,i,(LPARAM)&buf[sumlen-len-1]);//get the string & add it to the end of the buffer
						strcat(buf,"\r\n");//add '\r\n' to the end of the line - this is '\r\n' rather than '\n' so that it can be pasted into notepad & dos programs
					}//end of the loop
					TextToClipboard(hwnd, buf);//copy the stuff to the clip ( this function needs work to clean it up )(len=1+strlen)
					free(buf);//free the buffer mem
					return TRUE;//yes we did process the message
				}
				break;


		case IDOK:
			{
				TCHAR buf[100];
				HWND hwndList = GetDlgItem(hDlg, IDC_LIST1);
				int i = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
				if (i != -1)
				{
					SendMessage(hwndList, LB_GETTEXT, i, (LPARAM)buf);
					DWORD dw = SendMessage(hwndList, LB_GETITEMDATA, i, 0);
					iStartOfSelection = iCurByte + StrToInt(buf + LOWORD(dw));
					iEndOfSelection = iCurByte + StrToInt(buf + HIWORD(dw));
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
		break;
	}
	return FALSE;
}

