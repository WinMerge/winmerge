#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

C_ASSERT(sizeof(FillWithDialog) == sizeof(HexEditorWindow)); // disallow instance members

//Pabs changed - line insert
char FillWithDialog::pcFWText[FW_MAX];//hex representation of bytes to fill with
char FillWithDialog::buf[FW_MAX];//bytes to fill with
int FillWithDialog::buflen;//number of bytes to fill with
char FillWithDialog::szFWFileName[_MAX_PATH];//fill with file name
int FillWithDialog::FWFile;//fill with file
int FillWithDialog::FWFilelen;//fill with file len
LONG FillWithDialog::oldproc;//old hex box proc
//LONG cmdoldproc;//old command box proc
HFONT FillWithDialog::hfon;//needed so possible to display infinity char in fill with dlg box
char FillWithDialog::curtyp;//filling with input-0 or file-1
char FillWithDialog::asstyp;
//see CMD_fw below

void FillWithDialog::inittxt(HWND hDlg)
{
	int iStartOfSelSetting;
	int iEndOfSelSetting;
	if (bSelected)
	{
		iStartOfSelSetting = iStartOfSelection;
		iEndOfSelSetting = iEndOfSelection;
	}
	else
	{
		iStartOfSelSetting = 0;
		iEndOfSelSetting = DataArray.GetUpperBound();
	}
	//init all the readonly boxes down below
	char bufff[250];
	int tteemmpp = 1 + abs(iStartOfSelSetting - iEndOfSelSetting);
	sprintf(bufff,"%d=0x%x", iStartOfSelSetting, iStartOfSelSetting);
	SetDlgItemText(hDlg, IDC_STS,bufff);
	sprintf(bufff,"%d=0x%x", iEndOfSelSetting, iEndOfSelSetting);
	SetDlgItemText(hDlg, IDC_ES,bufff);
	sprintf(bufff,"%d=0x%x",tteemmpp,tteemmpp);
	SetDlgItemText(hDlg, IDC_SS,bufff);
	if (curtyp)
	{//1-file
		SetDlgItemText(hDlg,IDC_SI, "???");
		SetDlgItemText(hDlg,IDC_IFS, "???");
		SetDlgItemText(hDlg,IDC_R, "???");
	}
	else
	{//0-input
		sprintf(bufff, "%d=0x%x", buflen, buflen);
		SetDlgItemText(hDlg,IDC_SI, bufff);
		if (buflen)
		{
			int d = tteemmpp / buflen;
			int m = tteemmpp % buflen;
			sprintf(bufff,"%d=0x%x", d, d);
			SetDlgItemText(hDlg,IDC_IFS, bufff);
			HFONT hfdef = (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0);
			SendDlgItemMessage(hDlg,IDC_IFS,WM_SETFONT,(WPARAM) hfdef, MAKELPARAM(TRUE, 0));
			sprintf(bufff,"%d=0x%x", m, m);
			SetDlgItemText(hDlg,IDC_R, bufff);
		}
		else
		{
			SetDlgItemText(hDlg,IDC_IFS, "\xa5");//set to infinity symbol
			SendDlgItemMessage(hDlg,IDC_IFS,WM_SETFONT,(WPARAM) hfon,MAKELPARAM(TRUE, 0));
			SetDlgItemText(hDlg,IDC_R, "0=0x0");
		}
	}
}

//-------------------------------------------------------------------
// Following code by Pabs.

unsigned char FillWithDialog::input(int index)
{
	return buf[index];
}

//see CMD_fw below
unsigned char FillWithDialog::file(int index)
{
	unsigned char x;
	_lseek(FWFile, index, SEEK_SET);
	_read(FWFile,&x,1);
	return x;
}

//convert a string of hex digits to a string of chars
void FillWithDialog::hexstring2charstring()
{
	// RK: removed definition of variable "a".
	int i, ii = strlen(pcFWText);
	if (ii % 2)//if number of hex digits is odd
	{
		//concatenate them
		for (i = 0 ; i < ii ; i++)
			pcFWText[ii + i] = pcFWText[i];
		pcFWText[ii * 2] = 0;
	}
	for (i = ii = 0 ; pcFWText[i] != '\0' ; i += 2)
	{
		// RK: next two lines changed, would crash when compiled with VC++ 4.0.
		/*
		sscanf(pcTemp,"%2x",&a);//get byte from the hexstring
		buf[ii]=a;//store it
		*/
		// Replaced with this line:
		sscanf(pcFWText + i, "%2x", buf + ii);//get byte from the hexstring
		ii++;
	}//for
	buflen = ii;//store length
}//func

//used to delete non-hex chars after the user pastes into the hexbox
void FillWithDialog::deletenonhex(HWND hEd)
{
	GetWindowText(hEd,pcFWText,FW_MAX);
	int ii=0;
	for (int i =0;pcFWText[i]!='\0';i++)
	{
		if(isxdigit(pcFWText[i])){pcFWText[ii]=pcFWText[i];ii++;}
	}
	pcFWText[ii]='\0';
	SetWindowText(hEd,pcFWText);
}

//hex box msg handler
LRESULT CALLBACK FillWithDialog::HexProc(HWND hEdit, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_CHAR:
		// only enter chars if they are hex digits or backspace
		if (!isxdigit((TBYTE)wParam) && wParam != VK_BACK)
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return 0;
		}
		break;
	case WM_PASTE:
		CallWindowProc((WNDPROC)oldproc, hEdit, iMsg, wParam, lParam);//paste as usual
		deletenonhex(hEdit);//but delete non-hex chars
		return 0;
	}
	return CallWindowProc((WNDPROC)oldproc, hEdit, iMsg, wParam, lParam);
}

INT_PTR FillWithDialog::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		{
			HWND hEditt=GetDlgItem(hDlg, IDC_HEX);//get the handle to the hex edit box
			SendMessage(hEditt, EM_SETLIMITTEXT, (WPARAM)FW_MAX, 0);//limit the amount of text the user can enter
			SetWindowText(hEditt, pcFWText);//init hex text
			SetFocus(hEditt);//give the hex box focus
			EnableWindow(hEditt,!curtyp);
			oldproc = SetWindowLong(hEditt, GWLP_WNDPROC, (LONG)HexProc);//override the old proc to be HexProc
			EnableWindow(GetDlgItem(hDlg, IDC_HEXSTAT),!curtyp);

			HWND typ = GetDlgItem(hDlg, IDC_TYPE);
			SendMessage(typ, CB_ADDSTRING ,0, (LPARAM)"Input");
			SendMessage(typ, CB_ADDSTRING ,0, (LPARAM)"File");
			SendMessage(typ, CB_SETCURSEL, (WPARAM)curtyp,0);//set cursel to previous

			//en/disable filename box and browse button
			HWND fn=GetDlgItem(hDlg, IDC_FN);
			SetWindowText(fn, szFWFileName);
			EnableWindow(fn, curtyp);
			EnableWindow(GetDlgItem(hDlg, IDC_BROWSE), curtyp);
			EnableWindow(GetDlgItem(hDlg, IDC_FILESTAT), curtyp);

			hfon = CreateFont(16,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Symbol");
			inittxt(hDlg);
			switch (asstyp)
			{
			case 0: CheckDlgButton(hDlg, IDC_EQ, BST_CHECKED); break;
			case 1: CheckDlgButton(hDlg, IDC_OR, BST_CHECKED); break;
			case 2: CheckDlgButton(hDlg, IDC_AND, BST_CHECKED); break;
			case 3: CheckDlgButton(hDlg, IDC_XOR, BST_CHECKED); break;
			}
			return 0;//stop the system from setting focus to the control handle in (HWND) wParam because we already set focus above
		}
		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK: //ok pressed
			{
				if (curtyp)
				{//1-file
					GetDlgItemText(hDlg, IDC_FN, szFWFileName, _MAX_PATH);//get file name
					FWFile = _open(szFWFileName,_O_RDONLY|_O_BINARY);
					if (FWFile == -1)
					{//if there is error opening
						MessageBox(hDlg,"Error opening file","Error", MB_ICONERROR);//tell user but don't close dlgbox
						return 1;//didn't process this message
					}//if
					FWFilelen = _filelength(FWFile);
					if (FWFilelen == 0)
					{//if filelen is zero
						MessageBox(hDlg, "Can't fill a selection with a file of zero size.", "Error", MB_ICONERROR);//tell user but don't close dlgbox
						_close(FWFile);//close file
						return 1;//didn't process this message
					}//if
					else if (FWFilelen == -1)
					{//error returned by _filelength
						MessageBox(hDlg, "Error opening file", "Error", MB_ICONERROR);//tell user but don't close dlgbox
						_close(FWFile);//close file
						return 1;//didn't process this message
					}//elseif
				}
				else
				{//0-input
					if (!buflen)
					{//no hex input
						MessageBox(hDlg, "Can't fill a selection with a string of zero size.", "Error", MB_ICONERROR);//tell user but don't close dlgbox
						return 1;//didn't process this message
					}//if
					int i = GetDlgItemText(hDlg, IDC_HEX, pcFWText, FW_MAX);
					if (i == 0 || i == FW_MAX - 1)
					{//error
						MessageBox(hDlg, "Too great a number of bytes to fill with or some other error.", "Error", MB_ICONERROR);//tell user but don't close dlgbox
						return 1;//didn't process this message
					}//if
					hexstring2charstring();//just in case
					//pcFWText[(aa?buflen:buflen*2)]='\0';//access violation if i do it in the above function

				}
				if (IsDlgButtonChecked(hDlg,IDC_EQ))
					asstyp = 0;
				else if (IsDlgButtonChecked(hDlg,IDC_OR))
					asstyp = 1;
				else if (IsDlgButtonChecked(hDlg,IDC_AND))
					asstyp = 2;
				else if (IsDlgButtonChecked(hDlg,IDC_XOR))
					asstyp = 3;

				// go ahead
				SetCursor(LoadCursor(NULL,IDC_WAIT));
				unsigned char (*fnc)(int);
				int iStartOfSelSetting;
				int iEndOfSelSetting;
				int iimax;
				if (curtyp)
				{//1-file
					fnc = file;
					iimax = FWFilelen;
				}//if
				else
				{//0-input
					fnc = input;
					iimax = buflen;
				}//else

				if (bSelected)
				{
					iStartOfSelSetting = iGetStartOfSelection();
					iEndOfSelSetting = iGetEndOfSelection();
				}
				else
				{
					iStartOfSelSetting = 0;
					iEndOfSelSetting = DataArray.GetUpperBound();
				}

				int i = iStartOfSelSetting;
				int ii = 0;
				switch (asstyp)
				{// use switch instead of pointers to funcs that just call an operator as its faster
				case 0:
					while (i <= iEndOfSelSetting)
					{
						DataArray[i++] = fnc(ii++);
						ii %= iimax;
					}
					break;
				case 1:
					while (i <= iEndOfSelSetting)
					{
						DataArray[i++] |= fnc(ii++);
						ii %= iimax;
					}
					break;
				case 2:
					while (i <= iEndOfSelSetting)
					{
						DataArray[i++] &= fnc(ii++);
						ii %= iimax;
					}
					break;
				case 3:
					while (i <= iEndOfSelSetting)
					{
						DataArray[i++] ^= fnc(ii++);
						ii %= iimax;
					}
					break;
				}
				if (curtyp) _close(FWFile);//close file
				SetCursor (LoadCursor (NULL, IDC_ARROW));
				iFileChanged = TRUE;//mark as changed
				bFilestatusChanged = TRUE;
				repaint();//you tell me
			}
			// fall through
		case IDCANCEL: //cancel pressed
			DeleteObject(hfon);// won't need till next time
			EndDialog(hDlg, wParam);//tell CMD_fw not to carry out the fill with operation
			return 1;//did process this message
		case MAKEWPARAM(IDC_TYPE, CBN_SELCHANGE):
			//thing to fill selection with changes
			curtyp = (char)SendMessage(GetDlgItem(hDlg, IDC_TYPE),CB_GETCURSEL,0,0);//get cursel
			EnableWindow(GetDlgItem(hDlg, IDC_FN),curtyp);//en/disable fnamebox and browse button
			EnableWindow(GetDlgItem(hDlg, IDC_BROWSE),curtyp);
			EnableWindow(GetDlgItem(hDlg, IDC_FILESTAT),curtyp);
			curtyp=!curtyp;//flip it for the others
			EnableWindow(GetDlgItem(hDlg, IDC_HEX),curtyp);//en/disable hexboxand relateds
			EnableWindow(GetDlgItem(hDlg, IDC_HEXSTAT),curtyp);
			curtyp=!curtyp;//restore original value -not for below -accurate value needed elsewhere
			//set text in boxes down below
			inittxt(hDlg);
			break;
		case IDC_BROWSE:
			{
				//prepare OPENFILENAME for the file open common dlg box
				szFWFileName[0] = '\0';
				OPENFILENAME ofn;
				ZeroMemory(&ofn, sizeof ofn);
				ofn.lStructSize = sizeof ofn;
				ofn.hwndOwner = hDlg;
				ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
				ofn.lpstrFile = szFWFileName;
				ofn.nMaxFile = _MAX_PATH;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
				//show open dlgbox and if file good save name & path in edit box
				if (GetOpenFileName(&ofn))
					SetDlgItemText(hDlg, IDC_FN, ofn.lpstrFile);
			}
			return TRUE;
		case MAKEWPARAM(IDC_HEX, EN_UPDATE): //hexedit updated
			GetWindowText(GetDlgItem(hDlg, IDC_HEX), pcFWText, FW_MAX);//gettext
			hexstring2charstring();//convert to char string
			//set text in boxes down below
			inittxt(hDlg);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
