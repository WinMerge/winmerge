/**
 * @file  MergeResultStatusBar.h
 *
 * @brief Declaration of CMergeResultStatusBar class
 *
 * Dedicated status bar for the Merge Result Pane displaying:
 * - Conflict / resolution statistics
 * - Caret position (Line, Column, Char, Selection)
 * - Encoding
 * - EOL mode
 * - Read-only indicator
 */
#pragma once

#include "BasicFlatStatusBar.h"

class CMergeDoc;

/**
 * @brief Dedicated status bar for the Merge Result Pane
 */
class CMergeResultStatusBar : public CBasicFlatStatusBar
{
	DECLARE_DYNAMIC(CMergeResultStatusBar)

public:
	enum PaneIndex
	{
		PANE_CONFLICT = 0,
		PANE_LINEINFO,
		PANE_ENCODING,
		PANE_EOL,
		PANE_RO,
		NUM_PANES
	};

	CMergeResultStatusBar();
	virtual ~CMergeResultStatusBar();

	BOOL Create(CWnd* pParentWnd);

	// Conflict statistics
	void SetConflictInfo(int nConflicts, int nUnresolved, int nWhiteSpaceOnly);

	// Caret / line info from CMergeResultView
	void SetLineInfo(const tchar_t* szLine, int nColumn, int nColumns,
		int nChar, int nChars, int nSelectedLines, int nSelectedChars);

	// Encoding and EOL info
	void SetEncodingAndEol(int nCodepage, bool bHasBom, const tchar_t* szEol);

	// ReadOnly status
	void SetReadOnly(bool bReadOnly);

	void UpdateResources();

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

private:
	void UpdateConflictText();
	void UpdateLineText();
	void UpdateEncodingText();
	void UpdateEolText();
	void UpdateRoText();

	CMergeDoc* GetDocument() const;

	// Conflict statistics
	int m_nConflicts;
	int m_nUnresolved;
	int m_nWhiteSpaceOnly;

	// Caret info
	String m_sLine;
	int m_nColumn;
	int m_nColumns;
	int m_nChar;
	int m_nChars;
	int m_nSelectedLines;
	int m_nSelectedChars;

	// Encoding & EOL info
	int m_nCodepage;
	bool m_bHasBom;
	String m_sCodepageName;
	String m_sEol;
	String m_sEolDisplay;

	// ReadOnly
	bool m_bReadOnly;
};
