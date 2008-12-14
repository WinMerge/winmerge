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
 * @file  CharacterSetDlg.cpp
 *
 * @brief Implementation of the Character set selection dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: CharacterSetDlg.cpp 200 2008-12-09 20:29:21Z kimmov $

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

INT_PTR CharacterSetDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, IDC_CHSET_FONTSIZE, iFontSize, TRUE);
		switch (iCharacterSet)
		{
		case ANSI_FIXED_FONT:
			CheckDlgButton(hDlg, IDC_CHSET_ANSI, BST_CHECKED);
			break;
		case OEM_FIXED_FONT:
			CheckDlgButton(hDlg, IDC_CHSET_OEM, BST_CHECKED);
			break;
		}
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			iFontSize = GetDlgItemInt(hDlg, IDC_CHSET_FONTSIZE, 0, TRUE);
			iFontZoom = 0;
			iCharacterSet = IsDlgButtonChecked(hDlg, IDC_CHSET_ANSI) ?
				ANSI_FIXED_FONT : OEM_FIXED_FONT;
			save_ini_data();
			resize_window();
			// fall through
		case IDCANCEL:
			EndDialog(hDlg, wParam);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
