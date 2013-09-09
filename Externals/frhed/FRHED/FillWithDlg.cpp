/*
Frhed - Free hex editor
Copyright (C) 2000 Raihan Kibria

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

Last change: 2013-02-24 by Jochen Neubeck
*/
/** 
 * @file  FillWithDlg.cpp
 *
 * @brief Implementation of the Fill dialog.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "StringTable.h"

C_ASSERT(sizeof(FillWithDialog) == sizeof(HexEditorWindow)); // disallow instance members

TCHAR FillWithDialog::pcFWText[FW_MAX];//hex representation of bytes to fill with
BYTE FillWithDialog::buf[FW_MAX];//bytes to fill with
int FillWithDialog::buflen;//number of bytes to fill with
TCHAR FillWithDialog::szFWFileName[_MAX_PATH];//fill with file name
int FillWithDialog::FWFile;//fill with file
int FillWithDialog::FWFilelen;//fill with file len
LONG_PTR FillWithDialog::oldproc;//old hex box proc
HFONT FillWithDialog::hfon;//needed so possible to display infinity char in fill with dlg box
TCHAR FillWithDialog::curtyp;//filling with input-0 or file-1
TCHAR FillWithDialog::asstyp;
//see CMD_fw below

void FillWithDialog::inittxt(HWindow *pDlg)
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
		iEndOfSelSetting = m_dataArray.GetUpperBound();
	}
	//init all the readonly boxes down below
	TCHAR bufff[250];
	int tteemmpp = 1 + abs(iStartOfSelSetting - iEndOfSelSetting);
	_stprintf(bufff, _T("%d=0x%x"), iStartOfSelSetting, iStartOfSelSetting);
	pDlg->SetDlgItemText(IDC_STS, bufff);
	_stprintf(bufff, _T("%d=0x%x"), iEndOfSelSetting, iEndOfSelSetting);
	pDlg->SetDlgItemText(IDC_ES, bufff);
	_stprintf(bufff, _T("%d=0x%x"), tteemmpp, tteemmpp);
	pDlg->SetDlgItemText(IDC_SS, bufff);
	if (curtyp)
	{//1-file
		pDlg->SetDlgItemText(IDC_SI, _T("???"));
		pDlg->SetDlgItemText(IDC_IFS, _T("???"));
		pDlg->SetDlgItemText(IDC_R, _T("???"));
	}
	else
	{//0-input
		_stprintf(bufff, _T("%d=0x%x"), buflen, buflen);
		pDlg->SetDlgItemText(IDC_SI, bufff);
		if (buflen)
		{
			int d = tteemmpp / buflen;
			int m = tteemmpp % buflen;
			_stprintf(bufff, _T("%d=0x%x"), d, d);
			pDlg->SetDlgItemText(IDC_IFS, bufff);
			HFont *hfdef = pDlg->GetFont();
			pDlg->SendDlgItemMessage(IDC_IFS, WM_SETFONT, (WPARAM)hfdef, MAKELPARAM(TRUE, 0));
			_stprintf(bufff, _T("%d=0x%x"), m, m);
			pDlg->SetDlgItemText(IDC_R, bufff);
		}
		else
		{
			pDlg->SetDlgItemText(IDC_IFS, _T("\xa5"));//set to infinity symbol
			pDlg->SendDlgItemMessage(IDC_IFS, WM_SETFONT, (WPARAM)hfon, MAKELPARAM(TRUE, 0));
			pDlg->SetDlgItemText(IDC_R, _T("0=0x0"));
		}
	}
}

BYTE FillWithDialog::input(int index)
{
	return buf[index];
}

//see CMD_fw below
BYTE FillWithDialog::file(int index)
{
	BYTE x;
	_lseek(FWFile, index, SEEK_SET);
	_read(FWFile,&x,1);
	return x;
}

//convert a string of hex digits to a string of chars
void FillWithDialog::hexstring2charstring()
{
	// RK: removed definition of variable "a".
	int i, ii = static_cast<int>(_tcslen(pcFWText));
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
		_stscanf(pcFWText + i, _T("%2x"), buf + ii);//get byte from the hexstring
		ii++;
	}//for
	buflen = ii;//store length
}//func

//used to delete non-hex chars after the user pastes into the hexbox
void FillWithDialog::deletenonhex(HWND hEd)
{
	GetWindowText(hEd, pcFWText, FW_MAX);
	int ii = 0;
	for (int i = 0; pcFWText[i] != '\0'; i++)
	{
		if (_istxdigit(pcFWText[i]))
		{
			pcFWText[ii] = pcFWText[i];
			ii++;
		}
	}
	pcFWText[ii] = '\0';
	SetWindowText(hEd, pcFWText);
}

//hex box msg handler
LRESULT CALLBACK FillWithDialog::HexProc(HWND hEdit, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_CHAR:
		// only enter chars if they are hex digits or backspace
		if (!_istxdigit((TBYTE)wParam) && wParam != VK_BACK)
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

/**
 * @brief Handle dialog messages.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg The message.
 * @param [in] wParam The command in the message.
 * @param [in] lParam The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
 */
INT_PTR FillWithDialog::DlgProc(HWindow *pDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		{
			HEdit *pEditt = static_cast<HEdit *>(pDlg->GetDlgItem(IDC_HEX));//get the handle to the hex edit box
			pEditt->LimitText(FW_MAX);//limit the amount of text the user can enter
			pEditt->SetWindowText(pcFWText);//init hex text
			pEditt->SetFocus();//give the hex box focus
			pEditt->EnableWindow(!curtyp);
			oldproc = (LONG_PTR) pEditt->SetWindowLongPtr(GWLP_WNDPROC, (LONG_PTR)HexProc);//override the old proc to be HexProc
			EnableDlgItem(pDlg, IDC_HEXSTAT, !curtyp);

			HComboBox *typ = static_cast<HComboBox *>(pDlg->GetDlgItem(IDC_TYPE));
			typ->AddString(_T("Input"));
			typ->AddString(_T("File"));
			typ->SetCurSel(curtyp);//set cursel to previous

			//en/disable filename box and browse button
			HWindow *fn = pDlg->GetDlgItem(IDC_FN);
			fn->SetWindowText(szFWFileName);
			fn->EnableWindow(curtyp);
			EnableDlgItem(pDlg, IDC_BROWSE, curtyp);
			EnableDlgItem(pDlg, IDC_FILESTAT, curtyp);

			hfon = CreateFont(16, 0, 0, 0, FW_NORMAL, 0, 0, 0,
					DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Symbol"));
			inittxt(pDlg);
			switch (asstyp)
			{
			case 0: 
				pDlg->CheckDlgButton(IDC_EQ, BST_CHECKED);
				break;
			case 1:
				pDlg->CheckDlgButton(IDC_OR, BST_CHECKED);
				break;
			case 2:
				pDlg->CheckDlgButton(IDC_AND, BST_CHECKED);
				break;
			case 3:
				pDlg->CheckDlgButton(IDC_XOR, BST_CHECKED);
				break;
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
					pDlg->GetDlgItemText(IDC_FN, szFWFileName, _MAX_PATH);//get file name
					FWFile = _topen(szFWFileName, _O_RDONLY | _O_BINARY);
					if (FWFile == -1)
					{//if there is error opening
						MessageBox(pDlg, GetLangString(IDS_ERR_OPENING_FILE), MB_ICONERROR);//tell user but don't close dlgbox
						return 1;//didn't process this message
					}//if
					FWFilelen = _filelength(FWFile);
					if (FWFilelen == 0)
					{//if filelen is zero
						MessageBox(pDlg, GetLangString(IDS_FILL_ZERO_SIZE_FILE), MB_ICONERROR);//tell user but don't close dlgbox
						_close(FWFile);//close file
						return 1;//didn't process this message
					}//if
					else if (FWFilelen == -1)
					{//error returned by _filelength
						MessageBox(pDlg, GetLangString(IDS_ERR_OPENING_FILE), MB_ICONERROR);//tell user but don't close dlgbox
						_close(FWFile);//close file
						return 1;//didn't process this message
					}//elseif
				}
				else
				{//0-input
					if (!buflen)
					{//no hex input
						MessageBox(pDlg, GetLangString(IDS_FILL_ZERO_SIZE_STR), MB_ICONERROR);//tell user but don't close dlgbox
						return 1;//didn't process this message
					}//if
					int i = pDlg->GetDlgItemText(IDC_HEX, pcFWText, FW_MAX);
					if (i == 0 || i == FW_MAX - 1)
					{//error
						MessageBox(pDlg, GetLangString(IDS_FILL_TOO_MANY_BYTES), MB_ICONERROR);//tell user but don't close dlgbox
						return 1;//didn't process this message
					}//if
					hexstring2charstring();//just in case
					//pcFWText[(aa?buflen:buflen*2)]='\0';//access violation if i do it in the above function

				}
				if (pDlg->IsDlgButtonChecked(IDC_EQ))
					asstyp = 0;
				else if (pDlg->IsDlgButtonChecked(IDC_OR))
					asstyp = 1;
				else if (pDlg->IsDlgButtonChecked(IDC_AND))
					asstyp = 2;
				else if (pDlg->IsDlgButtonChecked(IDC_XOR))
					asstyp = 3;

				// go ahead
				SetCursor(LoadCursor(NULL, IDC_WAIT));
				BYTE (*fnc)(int);
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
					iEndOfSelSetting = m_dataArray.GetUpperBound();
				}

				int i = iStartOfSelSetting;
				int ii = 0;
				switch (asstyp)
				{// use switch instead of pointers to funcs that just call an operator as its faster
				case 0:
					while (i <= iEndOfSelSetting)
					{
						m_dataArray[i++] = fnc(ii++);
						ii %= iimax;
					}
					break;
				case 1:
					while (i <= iEndOfSelSetting)
					{
						m_dataArray[i++] |= fnc(ii++);
						ii %= iimax;
					}
					break;
				case 2:
					while (i <= iEndOfSelSetting)
					{
						m_dataArray[i++] &= fnc(ii++);
						ii %= iimax;
					}
					break;
				case 3:
					while (i <= iEndOfSelSetting)
					{
						m_dataArray[i++] ^= fnc(ii++);
						ii %= iimax;
					}
					break;
				}
				if (curtyp)
					_close(FWFile);//close file
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				iFileChanged = TRUE;//mark as changed
				bFilestatusChanged = true;
				repaint();//you tell me
			}
			// fall through
		case IDCANCEL: //cancel pressed
			DeleteObject(hfon);// won't need till next time
			pDlg->EndDialog(wParam);//tell CMD_fw not to carry out the fill with operation
			return 1;//did process this message
		case MAKEWPARAM(IDC_TYPE, CBN_SELCHANGE):
			//thing to fill selection with changes
			curtyp = (char)pDlg->SendDlgItemMessage(IDC_TYPE, CB_GETCURSEL, 0, 0);//get cursel
			EnableDlgItem(pDlg, IDC_FN, curtyp);//en/disable fnamebox and browse button
			EnableDlgItem(pDlg, IDC_BROWSE, curtyp);
			EnableDlgItem(pDlg, IDC_FILESTAT, curtyp);
			curtyp = !curtyp;//flip it for the others
			EnableDlgItem(pDlg, IDC_HEX, curtyp);//en/disable hexboxand relateds
			EnableDlgItem(pDlg, IDC_HEXSTAT, curtyp);
			curtyp = !curtyp;//restore original value -not for below -accurate value needed elsewhere
			//set text in boxes down below
			inittxt(pDlg);
			break;
		case IDC_BROWSE:
			{
				//prepare OPENFILENAME for the file open common dlg box
				szFWFileName[0] = '\0';
				OPENFILENAME ofn;
				ZeroMemory(&ofn, sizeof ofn);
				ofn.lStructSize = sizeof ofn;
				ofn.hwndOwner = pDlg->m_hWnd;
				ofn.lpstrFilter = GetLangString(IDS_OPEN_ALL_FILES);
				ofn.lpstrFile = szFWFileName;
				ofn.nMaxFile = _MAX_PATH;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
				//show open dlgbox and if file good save name & path in edit box
				if (GetOpenFileName(&ofn))
					pDlg->SetDlgItemText(IDC_FN, ofn.lpstrFile);
			}
			return TRUE;
		case MAKEWPARAM(IDC_HEX, EN_UPDATE): //hexedit updated
			pDlg->GetDlgItemText(IDC_HEX, pcFWText, FW_MAX);//gettext
			hexstring2charstring();//convert to char string
			//set text in boxes down below
			inittxt(pDlg);
			return TRUE;
		}
		break;

	case WM_HELP:
		OnHelp(pDlg);
		break;
	}
	return FALSE;
}
