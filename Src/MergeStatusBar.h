/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  MergeStatusBar.h
 *
 * @brief Interface of the CMergeStatusBar class.
 *
 */
#pragma once

#include "MergeEditStatus.h"
#include "BasicFlatStatusBar.h"
#include "OptionsDiffColors.h"
#include "UnicodeString.h"

class CMergeStatusBar : public CBasicFlatStatusBar
{
public : 
	CMergeStatusBar();
	~CMergeStatusBar();

	BOOL Create(CWnd* pParentWnd);
	void Resize(int widths[]);
	void SetPaneCount(int nPanes) { m_nPanes = nPanes; }
	void UpdateResources();
	IMergeEditStatus* GetIMergeEditStatus(int nPane) { return &m_status[nPane]; }
	IMergeEditStatus* GetResultStatus() { return &m_resultStatus; }
	void SetResultConflictInfo(int nConflicts, int nUnresolved, int nWhiteSpaceOnly) 
	{ 
		m_resultStatus.SetConflictInfo(nConflicts, nUnresolved, nWhiteSpaceOnly); 
	}

protected:
    afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP();

private:
	// this dialog uses custom edit boxes
	int m_nPanes;
	COLORSETTINGS m_cachedColors;
	bool m_bDiff[4];

protected:
	// Object that displays status line info for one side of a merge view
	class MergeStatus : public IMergeEditStatus
	{
	public:
		// ctr
		MergeStatus();
		// Implement MergeEditStatus
		void SetLineInfo(const tchar_t* szLine, int nColumn, int nColumns,
			int nChar, int nChars, int nSelectedLines, int nSelectedChars, const tchar_t* szEol, int nCodepage, bool bHasBom) override;
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
		int m_nSelectedLines;
		int m_nSelectedChars;
		bool m_bHasBom;
		String m_sEol;
		String m_sEolDisplay;
		String m_sCodepageName;
	};
	friend class MergeStatus; // MergeStatus accesses status bar
	MergeStatus m_status[3];

	// Object that displays merge result conflict statistics
	class MergeResultStatus : public IMergeEditStatus
	{
	public:
		// Constructor
		MergeResultStatus();
		// Implement IMergeEditStatus (not used for result pane, but required by interface)
		void SetLineInfo(const tchar_t* szLine, int nColumn, int nColumns,
			int nChar, int nChars, int nSelectedLines, int nSelectedChars, const tchar_t* szEol, int nCodepage, bool bHasBom) override;
		void UpdateResources();
		// Set conflict statistics
		void SetConflictInfo(int nConflicts, int nUnresolved, int nWhiteSpaceOnly);
	protected:
		void Update();
	public:
		CMergeStatusBar* m_pWndStatusBar;
		int m_base; /**< Base pane index for result status */
	private:
		String m_sLine; /**< Current line info */
		int m_nConflicts; /**< Total number of conflicts */
		int m_nUnresolved; /**< Unresolved differences */
		int m_nWhiteSpaceOnly; /**< White space only conflicts */
	};
	friend class MergeResultStatus; // MergeResultStatus accesses status bar
	MergeResultStatus m_resultStatus;
};
