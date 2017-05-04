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
#include "Picture.h"
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
 * If one of the paths is a project file, that projec file is loaded,
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
	String m_strPath[3];
	bool m_bReadOnly[3];
	PathContext m_files;
	bool	m_bRecurse;
	String	m_strExt;
	String	m_strUnpacker;
	//}}AFX_DATA

// other public data
	/// unpacker info
	std::array<DWORD, 3> m_dwFlags;
	PackingInfo m_infoHandler;

// Attributes
public:
	COpenDoc* GetDocument() const;

// Operations
public:
	void UpdateButtonStates();

// Implementation data
private:
	String m_strBrowsePath[3]; /**< Left/middle/right path from browse dialog. */
	CWinThread *m_pUpdateButtonStatusThread;
	CPicture m_picture; /**< Image loader/viewer for logo image */
	CRectTracker m_rectTracker;
	CSize m_sizeOrig;
	prdlg::CMoveConstraint m_constraint;
	CFont m_fontSwapButton;
	std::array<bool, 3> m_bAutoCompleteReady;
	DropHandler *m_pDropHandler;
// Overrides
	public:
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
public:
	virtual ~COpenView();

protected:
	void SetStatus(UINT msgID);
	void SetUnpackerStatus(UINT msgID);
	BOOL LoadProjectFile(const String &path);
	void TerminateThreadIfRunning();
	void TrimPaths();
	void OnButton(int index);
	void OnSelchangeCombo(int index);
	void LoadComboboxStates();
	void SaveComboboxStates();

// Generated message map functions
protected:
	//{{AFX_MSG(COpenView)
	template <int N>
	afx_msg void OnPathButton();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	template <int N>
	afx_msg void OnSelchangePathCombo();
	afx_msg void OnSetfocusPathCombo(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	template<int id1, int id2>
	afx_msg void OnSwapButton();
	afx_msg void OnEditEvent();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSelectUnpacker();
	afx_msg void OnSelectFilter();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	template <int MSG, int WPARAM = 0, int LPARAM = 0>
	afx_msg void OnEditAction();
	afx_msg void OnHelp();
	afx_msg void OnDropFiles(const std::vector<String>& files);
	afx_msg LRESULT OnUpdateStatus(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in OpenView.cpp
inline COpenDoc* COpenView::GetDocument() const
   { return reinterpret_cast<COpenDoc*>(m_pDocument); }
#endif

