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
 * @file  ReverseDlg.cpp
 *
 * @brief Implementation of the Data reversing dialog.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "StringTable.h"
#include "offset.h"

/**
 * @brief Handle dialog messages.
 * @param [in] h Handle to the dialog.
 * @param [in] m The message.
 * @param [in] w The command in the message.
 * @param [in] l The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
 */
INT_PTR ReverseDlg::DlgProc(HWindow *pDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TCHAR buf[32];
	int iStartOfSelSetting = 0;
	int iEndOfSelSetting = 0;
	int maxb;
	switch (uMsg)
	{
	case WM_INITDIALOG:
		_stprintf(buf, _T("x%x"), iGetStartOfSelection());
		pDlg->SetDlgItemText(IDC_REVERSE_OFFSET, buf);
		_stprintf(buf, _T("x%x"), iGetEndOfSelection());
		pDlg->SetDlgItemText(IDC_REVERSE_OFFSETEND, buf);
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			if (pDlg->GetDlgItemText(IDC_REVERSE_OFFSET, buf, RTL_NUMBER_OF(buf)) &&
				!offset_parse(buf, iStartOfSelSetting))
			{
				MessageBox(pDlg, GetLangString(IDS_OFFSET_START_ERROR), MB_ICONERROR);
				return TRUE;
			}
			if (pDlg->GetDlgItemText(IDC_REVERSE_OFFSETEND, buf, RTL_NUMBER_OF(buf)) &&
				!offset_parse(buf, iEndOfSelSetting))
			{
				MessageBox(pDlg, GetLangString(IDS_OFFSET_END_ERROR), MB_ICONERROR);
				return TRUE;
			}
			if (iEndOfSelSetting == iStartOfSelSetting)
			{
				MessageBox(pDlg, GetLangString(IDS_REVERSE_ONE_BYTE), MB_ICONERROR);
				return TRUE;
			}

			SetCursor (LoadCursor (NULL, IDC_WAIT));
			if (iEndOfSelSetting < iStartOfSelSetting)
				swap(iEndOfSelSetting, iStartOfSelSetting);

			maxb = m_dataArray.GetUpperBound();
			if (iStartOfSelSetting < 0 || iEndOfSelSetting > maxb)
			{
				MessageBox(pDlg, GetLangString(IDS_REVERSE_BLOCK_EXTEND), MB_ICONERROR);
			}

			if (iStartOfSelSetting < 0)
				iStartOfSelSetting = 0;
			if (iEndOfSelSetting > maxb)
				iEndOfSelSetting = maxb;
			reverse_bytes(&m_dataArray[iStartOfSelSetting], &m_dataArray[iEndOfSelSetting]);
			if (bSelected)
			{
				//If the selection was inside the bit that was reversed, then reverse it too
				if (iStartOfSelSetting <= iStartOfSelection &&
					iStartOfSelSetting <= iEndOfSelection &&
					iEndOfSelSetting >= iStartOfSelection &&
					iEndOfSelSetting >= iEndOfSelection)
				{
					iStartOfSelection = iEndOfSelSetting - iStartOfSelection + iStartOfSelSetting;
					iEndOfSelection = iEndOfSelSetting - iEndOfSelection + iStartOfSelSetting;
				}
				else
				{
					bSelected = false;
				}//If the above is not true deselect - this may change when multiple selections are allowed
			}
			//Or if the current byte was in the reversed bytes reverse it too
			else if (iCurByte >= iStartOfSelSetting &&
				iCurByte <= iEndOfSelSetting)
			{
				iCurByte = iEndOfSelSetting - iCurByte + iStartOfSelSetting;
				iCurNibble = !iCurNibble;
			}
			SetCursor (LoadCursor (NULL, IDC_ARROW));
			iFileChanged = TRUE;
			bFilestatusChanged = true;
			repaint();
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
