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

/**
 * Interface to update the header data
 */
class IHeaderBar
{
public:
	virtual void SetText(int pane, LPCTSTR lpszString) = 0;
	virtual void SetActive(int pane, BOOL bActive) = 0;
};


/**
 * @brief dialog bar with two controls for left/right path
 */
class CEditorFilePathBar : public CDialogBar, public IHeaderBar
{
public : 
	BOOL Create( CWnd* pParentWnd);

// Dialog Data
	enum { IDD = IDD_EDITOR_HEADERBAR };

	BOOL LookLikeThisWnd(CWnd * pWnd);
	void Resize();
	void Resize(int leftWidth, int rightWidth);

	// Implement IFilepathHeaders
	void SetText(int pane, LPCTSTR lpszString);
	void SetActive(int pane, BOOL bActive);

protected:
	BOOL OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	DECLARE_MESSAGE_MAP();

private:
	// this dialog uses custom edit boxes
	CFilepathEdit m_EditLeft, m_EditRight;
};


#endif //__EDITORFILEPATHBAR_H__