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
// EditorFilePathBar.h : interface of the CEditorFilePathBar class

// a dialog bar for the both files path in the editor
//   looks like a statusBar (font, height)
//   use CFilepathEdit instead of CEdit
//   display a tip for each path (as a tooltip)

/////////////////////////////////////////////////////////////////////////////

#ifndef __EDITORFILEPATHBAR_H__
#define __EDITORFILEPATHBAR_H__

#include "FilepathEdit.h"



class CEditorFilePathBar : public CDialogBar
{
public : 
	BOOL Create( CWnd* pParentWnd);

// Dialog Data
	enum { IDD = IDD_EDITOR_HEADERBAR };

	BOOL LookLikeThisWnd(CWnd * pWnd);
	void Resize(int leftWidth, int rightWidth);

	void SetFilePath(int pane, const CString &text);

protected:
	BOOL OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	DECLARE_MESSAGE_MAP();

private:
	// this dialog uses custom edit boxes
	CFilepathEdit m_EditLeft, m_EditRight;
};


#endif //__EDITORFILEPATHBAR_H__