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
 * @file  MoveCopyDlg.cpp
 *
 * @brief Implementation of the data moving/copying dialog.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "LangArray.h"
#include "StringTable.h"
#include "offset.h"

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 */
BOOL MoveCopyDlg::OnInitDialog(HWindow *pDlg)
{
	TCHAR buf[30];
	int iMove1stEnd = iGetStartOfSelection();
	int iMove2ndEndorLen = iGetEndOfSelection();
	_stprintf(buf, _T("0x%x"), iMove1stEnd);
	pDlg->SetDlgItemText(IDC_1STOFFSET, buf);
	_stprintf(buf, _T("0x%x"), iMove2ndEndorLen);
	pDlg->SetDlgItemText(IDC_2NDDELIM, buf);
	pDlg->CheckDlgButton(IDC_OTHEREND, BST_CHECKED);
	_stprintf(buf, _T("0x%x"), iMovePos);
	pDlg->SetDlgItemText(IDC_MOVEMENT, buf);
	pDlg->CheckDlgButton(IDC_FPOS, BST_CHECKED);
	if (iMoveOpTyp == OPTYP_MOVE)
		pDlg->CheckDlgButton(IDC_MOVE, BST_CHECKED);
	else if (iMoveOpTyp == OPTYP_COPY)
		pDlg->CheckDlgButton(IDC_COPY, BST_CHECKED);
	return TRUE;
}

/**
 * @brief Read start offset value from the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @param [out] value Value read from the dialog.
 * @return true if the value was read, false if value could not be read.
 */
bool MoveCopyDlg::ReadStartOffset(HWindow *pDlg, int &value)
{
	TCHAR buf[30];
	if (!pDlg->GetDlgItemText(IDC_1STOFFSET, buf, RTL_NUMBER_OF(buf)) ||
		!offset_parse(buf, value))
	{
		MessageBox(pDlg, GetLangString(IDS_OFFSET_START_ERROR), MB_ICONERROR);
		return false;
	}
	if (value < 0)
	{
		MessageBox(pDlg, GetLangString(IDS_CM_NEGATIVE_OFFSET), MB_ICONERROR);
		return false;
	}
	return true;
}

/**
 * @brief Read end offset value from the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @param [out] value Value read from the dialog.
 * @return true if the value was read, false if value could not be read.
 */
bool MoveCopyDlg::ReadEndOffset(HWindow *pDlg, int &value)
{
	TCHAR buf[30];
	if (!pDlg->GetDlgItemText(IDC_2NDDELIM, buf, RTL_NUMBER_OF(buf)) ||
		!offset_parse(buf, value))
	{
		MessageBox(pDlg, GetLangString(IDS_OFFSET_END_ERROR), MB_ICONERROR);
		return false;
	}
	// Relative jump?
	if (value < 0 && !pDlg->IsDlgButtonChecked(IDC_LEN))
	{
		MessageBox(pDlg, GetLangString(IDS_CM_NEGATIVE_OFFSET), MB_ICONERROR);
		return false;
	}
	return true;
}

/**
 * @brief Read target offset value from the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @param [out] value Value read from the dialog.
 * @return true if the value was read, false if value could not be read.
 */
bool MoveCopyDlg::ReadTargetOffset(HWindow *pDlg, int &value)
{
	TCHAR buf[30];
	if (!pDlg->GetDlgItemText(IDC_MOVEMENT, buf, RTL_NUMBER_OF(buf)) ||
		!offset_parse(buf, value))
	{
		MessageBox(pDlg, GetLangString(IDS_CM_INVALID_TARGET), MB_ICONERROR);
		return false;
	}
	if (value < 0 && !pDlg->IsDlgButtonChecked(IDC_FORWARD))
	{
		MessageBox(pDlg, GetLangString(IDS_CM_NEGATIVE_OFFSET), MB_ICONERROR);
		return false;
	}
	return true;
}

/**
 * @brief Apply the copy/move of the data.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE if operation succeeded, FALSE otherwise.
 */
BOOL MoveCopyDlg::Apply(HWindow *pDlg)
{
	int vals[3];
	if (!ReadStartOffset(pDlg, vals[0]))
		return FALSE;
	if (!ReadEndOffset(pDlg, vals[1]))
		return FALSE;
	if (!ReadTargetOffset(pDlg, vals[2]))
		return FALSE;
	
	int clen = m_dataArray.GetLength();
	int iMove1stEnd = vals[0];
	int iMove2ndEndorLen = vals[1];
	if (!pDlg->IsDlgButtonChecked(IDC_OTHEREND))
	{
		if (iMove2ndEndorLen == 0)
		{
			MessageBox(pDlg, GetLangString(IDS_CM_ZERO_LEN), MB_ICONERROR);
			return FALSE;
		}
		if (iMove2ndEndorLen > 0)
			iMove2ndEndorLen--;
		else
			iMove2ndEndorLen++;
		iMove2ndEndorLen += iMove1stEnd;
	}

	if (iMove1stEnd < 0 ||
		iMove1stEnd >= clen ||
		iMove2ndEndorLen < 0 ||
		iMove2ndEndorLen >= clen)
	{
		MessageBox(pDlg, GetLangString(IDS_CM_INVALID_BLOCK), MB_ICONERROR);
		return FALSE;
	}

	if (iMove1stEnd > iMove2ndEndorLen)
		swap(iMove1stEnd, iMove2ndEndorLen);

	if (pDlg->IsDlgButtonChecked(IDC_FPOS))
		iMovePos = vals[2];
	else if (pDlg->IsDlgButtonChecked(IDC_LPOS))
		iMovePos = iMove1stEnd + vals[2] - iMove2ndEndorLen;
	else
		iMovePos = iMove1stEnd + vals[2];

	iMoveOpTyp = pDlg->IsDlgButtonChecked(IDC_MOVE) ? OPTYP_MOVE : OPTYP_COPY;

	if (iMovePos == iMove1stEnd && iMoveOpTyp == OPTYP_MOVE)
	{
		MessageBox(pDlg, GetLangString(IDS_CM_NOT_MOVED), MB_ICONEXCLAMATION);
		pDlg->EndDialog(0);
		return FALSE;
	}

	if (iMovePos < 0 || (iMoveOpTyp == OPTYP_MOVE ?
		iMovePos + iMove2ndEndorLen - iMove1stEnd >= clen :
		iMovePos > clen))
	{
		MessageBox(pDlg, GetLangString(IDS_CM_OUTSIDE_DATA), MB_ICONERROR);
		return FALSE;
	}
	CMD_move_copy(iMove1stEnd, iMove2ndEndorLen, true);
	return TRUE;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Hanle to the dialog.
 * @param [in] wParam The command to handle.
 * @param [in] lParam Optional parameter for the command.
 * @return TRUE if the command was handled, FALSE otherwise.
 */
BOOL MoveCopyDlg::OnCommand(HWindow *pDlg, WPARAM wParam, LPARAM lParam)
{
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
INT_PTR MoveCopyDlg::DlgProc(HWindow *pDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(pDlg);
	case WM_COMMAND:
		return OnCommand(pDlg, wParam, lParam);

	case WM_HELP:
		OnHelp(pDlg);
		break;
	}
	return FALSE;
}
