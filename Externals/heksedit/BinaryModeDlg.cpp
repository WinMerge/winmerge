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
 * @file  BinaryModeDlg.cpp
 *
 * @brief Implementation of the Binary mode selection dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: BinaryModeDlg.cpp 203 2008-12-10 16:49:54Z kimmov $

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

INT_PTR BinaryModeDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		CheckDlgButton(hDlg, iBinaryMode == ENDIAN_LITTLE ?
				IDC_BINMODE_LITTLEEND : IDC_BINMODE_BIGEND, BST_CHECKED);
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			iBinaryMode = IsDlgButtonChecked(hDlg, IDC_BINMODE_LITTLEEND) ?
					ENDIAN_LITTLE : ENDIAN_BIG;
			// fall through
		case IDCANCEL:
			EndDialog(hDlg, wParam);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
