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
/** 
 * @file  EditorFilePathBar.h
 *
 * @brief Interface of the CEditorFilePathBar class.
 *
 */
#pragma once

#include <memory>
#include "FilepathEdit.h"

/**
 * Interface to update the header data.
 */
class IHeaderBar
{
public:
	virtual void SetText(int pane, const String& sString) = 0;
	virtual void SetActive(int pane, bool bActive) = 0;
	virtual void SetPaneCount(int nPanes) = 0;
	virtual void Resize() = 0;
};


/**
 * @brief A dialog bar with two controls for left/right path.
 * This class is a dialog bar for the both files path in the editor. 
 * The bar looks like a statusBar (font, height). The control
 * displays a tip for each path (as a tooltip). 
 */
class CEditorFilePathBar : public CDialogBar, public IHeaderBar
{
public : 
	CEditorFilePathBar();
	~CEditorFilePathBar();

	BOOL Create( CWnd* pParentWnd);
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

// Dialog Data
	enum { IDD = IDD_EDITOR_HEADERBAR };
	
	void Resize();
	void Resize(int widths[]);

	// Implement IFilepathHeaders
	void SetText(int pane, const String& sString);
	void SetActive(int pane, bool bActive);
	void SetPaneCount(int nPanes);

protected:
	BOOL OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	DECLARE_MESSAGE_MAP();

private:
	// this dialog uses custom edit boxes
	CFilepathEdit m_Edit[3]; /**< Edit controls. */
	std::unique_ptr<CFont> m_pFont; /**< Font for editcontrols */
	int m_nPanes;
};
