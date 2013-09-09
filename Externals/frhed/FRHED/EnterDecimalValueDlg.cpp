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
 * @file  EnterDecimalValueDlg.cpp
 *
 * @brief Implementation of the dialog for entering decimal value.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "StringTable.h"
#include "offset.h"

int EnterDecimalValueDlg::iDecValDlgSize = 1;
bool EnterDecimalValueDlg::bSigned = false;

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE.
 */
BOOL EnterDecimalValueDlg::OnInitDialog(HWindow *pDlg)
{
	// Handle value as unsigned, convert to signed (if needed) when sending
	// the value to GUI.
	UINT iDecValDlgValue = 0;
	if (iCurByte >= 0 && iCurByte < m_dataArray.GetLength())
	{
		int t = m_dataArray.GetLength() - iCurByte;
		//Set the size down a bit if someone called this func with a size thats too large
		while (iDecValDlgSize > t)
			iDecValDlgSize /= 2;
		//Get the right value
		if (iDecValDlgSize == 2)
			iDecValDlgValue = *(WORD *)&m_dataArray[iCurByte];
		else if (iDecValDlgSize == 4)
			iDecValDlgValue = *(DWORD *)&m_dataArray[iCurByte];
		else
			iDecValDlgValue = (int)m_dataArray[iCurByte];
	}
	TCHAR buf[16];
	pDlg->SetDlgItemInt(IDC_DECIMAL_VALUE, iDecValDlgValue, bSigned ? TRUE : FALSE);
	_stprintf(buf, _T("0x%x"), iCurByte);
	pDlg->SetDlgItemText(IDC_DECIMAL_OFFSET, buf);
	pDlg->SetDlgItemInt(IDC_DECIMAL_TIMES, 1, TRUE);
	pDlg->CheckDlgButton(
		iDecValDlgSize == 4 ? IDC_DECIMAL_DWORD :
		iDecValDlgSize == 2 ? IDC_DECIMAL_WORD :
		IDC_DECIMAL_BYTE,
		BST_CHECKED);
	return TRUE;
}

/**
 * @brief Apply values from the dialog when dialog is closed.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE if values are valid, FALSE if there are errors.
 */
BOOL EnterDecimalValueDlg::Apply(HWindow *pDlg)
{
	iDecValDlgSize =
		pDlg->IsDlgButtonChecked(IDC_DECIMAL_DWORD) ? 4 :
		pDlg->IsDlgButtonChecked(IDC_DECIMAL_WORD) ? 2 :
		1;
	TCHAR buf[16];
	BOOL translated;
	UINT iDecValDlgValue = pDlg->GetDlgItemInt(IDC_DECIMAL_VALUE, &translated, bSigned);
	if (!translated)
	{
		MessageBox(pDlg, GetLangString(IDS_DECI_UNKNOWN), MB_ICONERROR);
		return FALSE;
	}
	int iDecValDlgOffset;
	if (!pDlg->GetDlgItemText(IDC_DECIMAL_OFFSET, buf, 16) ||
		!offset_parse(buf, iDecValDlgOffset))
	{
		MessageBox(pDlg, GetLangString(IDS_OFFSET_ERROR), MB_ICONERROR);
		return FALSE;
	}
	int iDecValDlgTimes = pDlg->GetDlgItemInt(IDC_DECIMAL_TIMES, &translated, TRUE);
	if (!translated)
	{
		MessageBox(pDlg, GetLangString(IDS_DECI_UNKNOWN_TIMES), MB_ICONERROR);
		return FALSE;
	}
	if (iDecValDlgOffset < 0 || iDecValDlgOffset > m_dataArray.GetUpperBound())
	{
		MessageBox(pDlg, GetLangString(IDS_DECI_INVALID_START), MB_ICONERROR);
		return FALSE;
	}
	if (iDecValDlgOffset + iDecValDlgSize * iDecValDlgTimes > m_dataArray.GetLength())
	{
		MessageBox(pDlg, GetLangString(IDS_DECI_NO_SPACE), MB_ICONERROR);
		return FALSE;
	}
	WaitCursor wc;
	while (iDecValDlgTimes)
	{
		if (iBinaryMode == ENDIAN_LITTLE)
		{
			switch (iDecValDlgSize)
			{
			case 1:
				m_dataArray[iDecValDlgOffset++] = (BYTE)iDecValDlgValue;
				break;

			case 2:
				m_dataArray[iDecValDlgOffset++] = (BYTE) (iDecValDlgValue & 0xff);
				m_dataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff00) >> 8);
				break;

			case 4:
				m_dataArray[iDecValDlgOffset++] = (BYTE) (iDecValDlgValue & 0xff);
				m_dataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff00) >> 8);
				m_dataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff0000) >> 16);
				m_dataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff000000) >> 24);
				break;
			}
		}
		else
		{
			switch (iDecValDlgSize)
			{
			case 1:
				m_dataArray[iDecValDlgOffset++] = (BYTE)iDecValDlgValue;
				break;

			case 2:
				m_dataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff00) >> 8);
				m_dataArray[iDecValDlgOffset++] = (BYTE) (iDecValDlgValue & 0xff);
				break;

			case 4:
				m_dataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff000000) >> 24);
				m_dataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff0000) >> 16);
				m_dataArray[iDecValDlgOffset++] = (BYTE) ((iDecValDlgValue & 0xff00) >> 8);
				m_dataArray[iDecValDlgOffset++] = (BYTE) (iDecValDlgValue & 0xff);
				break;
			}
		}
		--iDecValDlgTimes;
	}
	iFileChanged = TRUE;
	bFilestatusChanged = true;
	repaint();
	return TRUE;
}

/**
 * @brief Handle dialog messages.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg The message.
 * @param [in] wParam The command in the message.
 * @param [in] lParam The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
 */
INT_PTR EnterDecimalValueDlg::DlgProc(HWindow *pDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(pDlg);
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			if (Apply(pDlg))
			{
			case IDCANCEL:
				pDlg->EndDialog(wParam);
			}
			return TRUE;
		}
		break;

	case WM_HELP:
		OnHelp(pDlg);
		break;
	}
	return FALSE;
}
