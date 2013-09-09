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
 * @file  BitManipDlg.cpp
 *
 * @brief Implementation of bit manipulation dialog.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "StringTable.h"

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 */
BOOL BitManipDlg::OnInitDialog(HWindow *pDlg)
{
	TCHAR buf[64];
	_sntprintf(buf, RTL_NUMBER_OF(buf), GetLangString(IDS_BITMANIP_AT_OFFSET), iCurByte, iCurByte);
	pDlg->SetDlgItemText(IDC_MANIPBITS, buf);
	BYTE cBitValue = m_dataArray[iCurByte];
	if (cBitValue & 1)
		pDlg->CheckDlgButton(IDC_MANIPBITS_BIT1, BST_CHECKED);
	if (cBitValue & 2)
		pDlg->CheckDlgButton(IDC_MANIPBITS_BIT2, BST_CHECKED);
	if (cBitValue & 4)
		pDlg->CheckDlgButton(IDC_MANIPBITS_BIT3, BST_CHECKED);
	if (cBitValue & 8)
		pDlg->CheckDlgButton(IDC_MANIPBITS_BIT4, BST_CHECKED);
	if (cBitValue & 16)
		pDlg->CheckDlgButton(IDC_MANIPBITS_BIT5, BST_CHECKED);
	if (cBitValue & 32)
		pDlg->CheckDlgButton(IDC_MANIPBITS_BIT6, BST_CHECKED);
	if (cBitValue & 64)
		pDlg->CheckDlgButton(IDC_MANIPBITS_BIT7, BST_CHECKED);
	if (cBitValue & 128)
		pDlg->CheckDlgButton(IDC_MANIPBITS_BIT8, BST_CHECKED);
	Apply(pDlg, 0);
	return TRUE;
}

/**
 * @brief Apply the manipulation.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] wParam Command user selected.
 * @return TRUE if bytes were copied, FALSE otherwise.
 */
BOOL BitManipDlg::Apply(HWindow *pDlg, WPARAM wParam)
{
	BYTE cBitValue = 0;
	if (pDlg->IsDlgButtonChecked(IDC_MANIPBITS_BIT8))
		cBitValue |= 128;
	if (pDlg->IsDlgButtonChecked(IDC_MANIPBITS_BIT7))
		cBitValue |= 64;
	if (pDlg->IsDlgButtonChecked(IDC_MANIPBITS_BIT6))
		cBitValue |= 32;
	if (pDlg->IsDlgButtonChecked(IDC_MANIPBITS_BIT5))
		cBitValue |= 16;
	if (pDlg->IsDlgButtonChecked(IDC_MANIPBITS_BIT4))
		cBitValue |= 8;
	if (pDlg->IsDlgButtonChecked(IDC_MANIPBITS_BIT3))
		cBitValue |= 4;
	if (pDlg->IsDlgButtonChecked(IDC_MANIPBITS_BIT2))
		cBitValue |= 2;
	if (pDlg->IsDlgButtonChecked(IDC_MANIPBITS_BIT1))
		cBitValue |= 1;
	if (wParam == IDOK)
	{
		m_dataArray[iCurByte] = cBitValue;
		iFileChanged = TRUE;
		bFilestatusChanged = true;
		repaint();
		return TRUE;
	}
	TCHAR buf[64];
	_sntprintf(buf, RTL_NUMBER_OF(buf), GetLangString(IDS_BITMANIP_VALUE),
		(BYTE)cBitValue, (signed char)cBitValue, (BYTE)cBitValue);
	pDlg->SetDlgItemText(IDC_MANIPBITS_VALUE, buf);
	return FALSE;
}

/**
 * @brief Handle dialog messages.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg The message.
 * @param [in] wParam The command in the message.
 * @param [in] lParam The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
 */
INT_PTR BitManipDlg::DlgProc(HWindow *pDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(pDlg);
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
		case IDC_MANIPBITS_BIT1: case IDC_MANIPBITS_BIT2: case IDC_MANIPBITS_BIT3:
		case IDC_MANIPBITS_BIT4: case IDC_MANIPBITS_BIT5: case IDC_MANIPBITS_BIT6:
		case IDC_MANIPBITS_BIT7: case IDC_MANIPBITS_BIT8:
			if (Apply(pDlg, wParam))
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
