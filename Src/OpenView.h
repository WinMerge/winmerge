/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  OpenView.h
 *
 * @brief Declaration file for COpenView window
 *
 */

#pragma once

/////////////////////////////////////////////////////////////////////////////
// COpenView window
#include "SuperComboBox.h"
#include "FileTransform.h"
#include "PathContext.h"
#include "CMoveConstraint.h"
#include "TrDialogs.h"
#include <array>

class ProjectFile;
class COpenDoc;
class DropHandler;

/**
 * @brief The Open-View class.
 * The Open-View allows user to select paths to compare. In addition to
 * the two paths, there are controls for selecting filter and unpacker plugin.
 * If one of the paths is a project file, that project file is loaded,
 * overwriting possible other values in other dialog controls.
 * The dialog shows also a status of the selected paths (found/not found),
 * if enabled in the options (enabled by default).
 */
class COpenView : public CFormView, public DlgUtils<COpenView>
{
protected: // create from serialization only
	COpenView();
	DECLARE_DYNCREATE(COpenView)

public:
// Dialog Data
	//{{AFX_DATA(COpenView)
	enum { IDD = IDD_OPEN };
	CSuperComboBox	m_ctlExt;
	CSuperComboBox	m_ctlPath[3];
	CSuperComboBox	m_ctlUnpackerPipeline;
	CSuperComboBox	m_ctlPredifferPipeline;
	String m_strPath[3];
	bool m_bReadOnly[3];
	PathContext m_files;
	bool	m_bRecurse;
	String	m_strExt;
	String	m_strUnpackerPipeline;
	String	m_strPredifferPipeline;
	//}}AFX_DATA

// other public data
	/// unpacker info
	std::array<DWORD, 3> m_dwFlags;

// Attributes
public:
	COpenDoc* GetDocument() const;

// Operations
public:
	void UpdateButtonStates();
	void UpdateResources();

// Implementation data
private:
	String m_strBrowsePath[3]; /**< Left/middle/right path from browse dialog. */
	CWinThread *m_pUpdateButtonStatusThread;
	ATL::CImage m_image; /**< Image loader/viewer for logo image */
	CSize m_sizeOrig;
	prdlg::CMoveConstraint m_constraint;
	CFont m_fontSwapButton;
	HICON const m_hIconRotate;
	HCURSOR const m_hCursorNo;
	std::array<bool, 3> m_bAutoCompleteReady;
	DropHandler *m_pDropHandler;
	int m_retryCount;
	int m_nIgnoreWhite; /**< The value of the "Whitespaces" setting */
	bool m_bIgnoreBlankLines; /**< The value of the "Ignore blank lines" setting */
	bool m_bIgnoreCase; /**< The value of the "Ignore case" setting */
	bool m_bIgnoreEol; /**< The value of the "Ignore carriage return differences" setting */
	bool m_bIgnoreNumbers; /**< The value of the "Ignore numbers" setting */
	bool m_bIgnoreCodepage; /**< The value of the "Ignore codepage differences" setting */
	bool m_bFilterCommentsLines; /**< The value of the "Ignore comment differences" setting */
	bool m_bIgnoreMissingTrailingEol; /**< The value of the "Ignore missing trailing EOL" setting */
	int m_nCompareMethod; /**< The value of the "Compare method" setting */
	HTHEME m_hTheme; /**< Theme used for size grip on Vista and greater */
// Overrides
	public:
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

// Implementation
public:
	virtual ~COpenView();

protected:
	void SetStatus(UINT msgID);
	void TerminateThreadIfRunning();
	void TrimPaths();
	void LoadComboboxStates();
	void SaveComboboxStates();
	String AskProjectFileName(bool bOpen);
	void DropDown(NMHDR *pNMHDR, LRESULT *pResult, UINT nID, UINT nPopupID);

// Generated message map functions
protected:
	//{{AFX_MSG(COpenView)
	afx_msg void OnPathButton(UINT nID);
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnCompare(UINT nID);
	afx_msg void OnUpdateCompare(CCmdUI *pCmdUI);
	afx_msg void OnLoadProject();
	afx_msg void OnSaveProject();
	template<UINT id, UINT popupid>
	afx_msg void OnDropDown(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSelchangePathCombo(UINT nID);
	afx_msg void OnSetfocusPathCombo(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDragBeginPathCombo(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSwapButton(int id1, int id2);
	template<int id1, int id2>
	afx_msg void OnSwapButton();
	afx_msg void OnEditEvent(UINT nID);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSelectPlugin(UINT nID);
	afx_msg void OnSelectFilter();
	afx_msg void OnOptions();
	afx_msg void OnDiffWhitespace(UINT nID);
	afx_msg void OnUpdateDiffWhitespace(CCmdUI* pCmdUI);
	afx_msg void OnDiffIgnoreBlankLines();
	afx_msg void OnUpdateDiffIgnoreBlankLines(CCmdUI* pCmdUI);
	afx_msg void OnDiffIgnoreCase();
	afx_msg void OnUpdateDiffIgnoreCase(CCmdUI* pCmdUI);
	afx_msg void OnDiffIgnoreEOL();
	afx_msg void OnUpdateDiffIgnoreEOL(CCmdUI* pCmdUI);
	afx_msg void OnDiffIgnoreNumbers();
	afx_msg void OnUpdateDiffIgnoreNumbers(CCmdUI* pCmdUI);
	afx_msg void OnDiffIgnoreCP();
	afx_msg void OnUpdateDiffIgnoreCP(CCmdUI* pCmdUI);
	afx_msg void OnDiffIgnoreComments();
	afx_msg void OnUpdateDiffIgnoreComments(CCmdUI* pCmdUI);
	afx_msg void OnDiffIgnoreMissingTrailingEol();
	afx_msg void OnUpdateDiffIgnoreMissingTrailingEol(CCmdUI* pCmdUI);
	afx_msg void OnCompareMethod(UINT nID);
	afx_msg void OnUpdateCompareMethod(CCmdUI* pCmdUI);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnEditAction(int msg, WPARAM wParam, LPARAM LPARAM);
	template <int MSG, int WPARAM = 0, int LPARAM = 0>
	afx_msg void OnEditAction() { OnEditAction(MSG, WPARAM, LPARAM); }
	afx_msg void OnHelp();
	afx_msg void OnDropFiles(const std::vector<String>& files);
	afx_msg LRESULT OnUpdateStatus(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg LRESULT OnThemeChanged();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in OpenView.cpp
inline COpenDoc* COpenView::GetDocument() const
   { return reinterpret_cast<COpenDoc*>(m_pDocument); }
#endif

