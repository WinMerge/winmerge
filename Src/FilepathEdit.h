/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
// FilepathEdit.h : interface of the CFilepathEdit class
/** 
 * @file  FilePathEdit.h
 *
 * @brief Declaration file for CFilepathEdit class.
 */
#pragma once

/** 
 * @brief Read-only control to display a filepath. 
 * The path is compacted (with ellipsis) to fill in the area. 
 * The tooltip displays the entire path. 
 * ContextMenu offers two copy functions : copy path, copy filename
 */
class CFilepathEdit : public CEdit
{
public : 
	CFilepathEdit();

	bool SubClassEdit(UINT nID, CWnd* pParent);
	void RefreshDisplayText();
	const String& GetUpdatedTipText(CDC * pDC, int maxWidth);

	void SetActive(bool bActive);
	void SetOriginalText(const String& sString );
	void SetBackColor(COLORREF rgb);
	void SetTextColor(COLORREF rgb);

protected:
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	afx_msg void OnNcPaint();
	afx_msg void OnEditCopy();
	HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	DECLARE_MESSAGE_MAP();

	void GetOriginalText( String& rString ) const;

private:
	void CustomCopy(size_t iBegin, size_t iEnd = String::npos);

	String m_sToolTipString; /**< buffer for return data from GetUpdatedTipText */
	String m_sOriginalText; /**< Full path that was given to control */
	bool m_bActive; /**< Is the control active-looking? */
	COLORREF m_crText; /**< Control's text color. */
	COLORREF m_crBackGnd; /**< Control's background color. */
	CBrush m_brBackGnd; /**< Background brush for the control. */
};

/**
 * @brief Return the control's original text.
 * @return Control's original text.
 */
inline void CFilepathEdit::GetOriginalText(String& rString) const
{		
	rString = m_sOriginalText;
}

