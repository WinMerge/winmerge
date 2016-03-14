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
 * @file  MergeStatusBar.h
 *
 * @brief Interface of the CMergeStatusBar class.
 *
 */
#pragma once

#include "MergeEditStatus.h"
#include "OptionsDiffColors.h"
#include "UnicodeString.h"

class CMergeStatusBar : public CStatusBar
{
public : 
	CMergeStatusBar();
	~CMergeStatusBar();

	BOOL Create(CWnd* pParentWnd);
	void Resize(int widths[]);
	void SetPaneCount(int nPanes) { m_nPanes = nPanes; }
	void UpdateResources();
	IMergeEditStatus* GetIMergeEditStatus(int nPane) { return &m_status[nPane]; }
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	DECLARE_MESSAGE_MAP();

private:
	// this dialog uses custom edit boxes
	int m_nPanes;
	COLORSETTINGS m_cachedColors;
	bool m_bDiff[4];
	unsigned m_dispFlags[4];

protected:
	// Object that displays status line info for one side of a merge view
	class MergeStatus : public IMergeEditStatus
	{
	public:
		// ctr
		MergeStatus();
		// Implement MergeEditStatus
		void SetLineInfo(LPCTSTR szLine, int nColumn, int nColumns,
			int nChar, int nChars, LPCTSTR szEol, int nCodepage, bool bHasBom);
		void UpdateResources();
	protected:
		void Update();
	public:
		CMergeStatusBar* m_pWndStatusBar;
		int m_base; /**< 0 for left, 1 for right */
	private:
		String m_sLine;
		int m_nColumn; /**< Current column, tab-expanded */
		int m_nColumns; /**< Amount of columns, tab-expanded */
		int m_nChar; /**< Current char */
		int m_nChars; /**< Amount of chars in line */
		int m_nCodepage;
		bool m_bHasBom;
		String m_sEol;
		String m_sEolDisplay;
		String m_sCodepageName;
	};
	friend class MergeStatus; // MergeStatus accesses status bar
	MergeStatus m_status[3];
};
