/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
// FilepathEdit.h : interface of the CFilepathEdit class
/** 
 * @file  FilePathEdit.h
 *
 * @brief Declaration file for CFilepathEdit class.
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef __FILEPATHEDIT_H__
#define __FILEPATHEDIT_H__

int FormatFilePathForDisplayWidth(CDC * pDC, int maxWidth, CString & sFilepath);

/** 
 * @brief Read-only control to display a filepath. 
 * The path is compacted (with ellipsis) to fill in the area. 
 * The tooltip displays the entire path. 
 * ContextMenu offers two copy functions : copy path, copy filename
 */
class CFilepathEdit : public CEdit
{
public : 
	BOOL SubClassEdit(UINT nID, CWnd* pParent);
	void RefreshDisplayText();
	LPCTSTR GetUpdatedTipText(CDC * pDC, int maxWidth);

	void SetActive(BOOL bActive);
	void SetWholeText(LPCTSTR lpszString );

protected:
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	afx_msg void OnNcPaint( );
	DECLARE_MESSAGE_MAP();

	void GetWholeText( CString& rString ) const;

private:
	void CustomCopy(int iBegin, int iEnd =-1);

	CString m_sToolTipString; /**< buffer for return data from GetUpdatedTipText */
	CString m_sWholeText; /**< Full path that was given to control */
	BOOL m_bActive;
};


/////////////////////////////////////////////////////////////////////////////

#endif //__FILEPATHEDIT_H__
