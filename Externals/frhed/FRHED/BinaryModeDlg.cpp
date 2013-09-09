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
 * @file  BinaryModeDlg.cpp
 *
 * @brief Implementation of the Binary mode selection dialog.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

INT_PTR BinaryModeDlg::DlgProc(HWindow *pDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		pDlg->CheckDlgButton(iBinaryMode == ENDIAN_LITTLE ?
				IDC_BINMODE_LITTLEEND : IDC_BINMODE_BIGEND, BST_CHECKED);
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			iBinaryMode = pDlg->IsDlgButtonChecked(IDC_BINMODE_LITTLEEND) ?
					ENDIAN_LITTLE : ENDIAN_BIG;
			// fall through
		case IDCANCEL:
			pDlg->EndDialog(wParam);
			return TRUE;
		}
		break;

	case WM_HELP:
		OnHelp(pDlg);
		break;
	}
	return FALSE;
}
