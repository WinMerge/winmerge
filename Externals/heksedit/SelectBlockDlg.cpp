/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  SelectBlockDlg.cpp
 *
 * @brief Implementation of the Block selecting dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: SelectBlockDlg.cpp 204 2008-12-10 16:53:30Z kimmov $

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

BOOL SelectBlockDlg::OnInitDialog(HWND hDlg)
{
	TCHAR buf[32];
	_stprintf(buf, _T("x%x"), bSelected ? iStartOfSelection : iCurByte);
	SetDlgItemText(hDlg, IDC_BLOCKSEL_OFFSET, buf);
	_stprintf(buf, _T("x%x"), bSelected ? iEndOfSelection : iCurByte);
	SetDlgItemText(hDlg, IDC_BLOCKSEL_OFFSETEND, buf);
	return TRUE;
}

BOOL SelectBlockDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	TCHAR buf[32];
	int iStartOfSelSetting;
	int iEndOfSelSetting;
	int maxb;
	switch (wParam)
	{
	case IDOK:
		if (GetDlgItemText(hDlg, IDC_BLOCKSEL_OFFSET, buf, RTL_NUMBER_OF(buf)) &&
			_stscanf(buf, _T("x%x"), &iStartOfSelSetting) == 0 &&
			_stscanf(buf, _T("%d"), &iStartOfSelSetting) == 0)
		{
			MessageBox(hDlg, _T("Start offset not recognized."), _T("Select block"), MB_ICONERROR);
			return TRUE;
		}
		if (GetDlgItemText(hDlg, IDC_BLOCKSEL_OFFSETEND, buf, RTL_NUMBER_OF(buf)) &&
			_stscanf(buf, _T("x%x"), &iEndOfSelSetting) == 0 &&
			_stscanf(buf, _T("%d"), &iEndOfSelSetting) == 0)
		{
			MessageBox(hDlg, _T("End offset not recognized."), _T("Select block"), MB_ICONERROR);
			return TRUE;
		}
		//make the selection valid if it is not
		maxb = DataArray.GetUpperBound();
		if (iStartOfSelSetting < 0)
			iStartOfSelSetting = 0;
		if (iStartOfSelSetting > maxb)
			iStartOfSelSetting = maxb;
		if (iEndOfSelSetting < 0)
			iEndOfSelSetting = 0;
		if (iEndOfSelSetting > maxb)
			iEndOfSelSetting = maxb;
		iStartOfSelection = iStartOfSelSetting;
		iEndOfSelection = iEndOfSelSetting;
		bSelected = TRUE;
		adjust_view_for_selection();
		repaint();
		// fall through
	case IDCANCEL:
		EndDialog(hDlg, wParam);
		return TRUE;
	}
	return FALSE;
}

INT_PTR SelectBlockDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hDlg);
	case WM_COMMAND:
		return OnCommand(hDlg, wParam, lParam);
	}
	return FALSE;
}
