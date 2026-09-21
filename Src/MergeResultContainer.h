/**
 * @file  MergeResultContainer.h
 *
 * @brief Declaration of CMergeResultContainer class
 *
 * Container window that hosts both the merge result view and status bar
 */
#pragma once

#include "MergeResultStatusBar.h"

class CMergeDoc;
class CMergeResultView;

/**
 * @brief Container window for merge result view and status bar
 *
 * This class acts as an intermediate container that manages both the
 * CMergeResultView (editor) and the status bar. The CMergeResultBar
 * uses SCBS_SIZECHILD which only allows one child window, so we need
 * this container to hold multiple children.
 */
class CMergeResultContainer : public CWnd
{
	DECLARE_DYNAMIC(CMergeResultContainer)

public:
	CMergeResultContainer();
	virtual ~CMergeResultContainer();

	BOOL Create(CWnd* pParentWnd, CMergeDoc* pDoc);
	void UpdateConflictInfo(const String& sOutputPath, int nConflicts, int nUnresolved, int nWhiteSpaceOnly);
	void UpdateResources();

	CMergeDoc* GetDocument() const { return m_pDoc; }
	CMergeResultStatusBar* GetStatusBar() { return &m_statusBar; }
	CMergeResultView* GetResultView() const { return m_pResultView; }
	void SetResultView(CMergeResultView* pResultView) { m_pResultView = pResultView; }

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

private:
	CMergeResultView* m_pResultView; /**< The merge result view */
	CMergeResultStatusBar m_statusBar; /**< Dedicated status bar for merge result */
	CMergeDoc* m_pDoc; /**< Associated document */

	static const int STATUS_BAR_HEIGHT = 20; /**< Height in pixels for status bar */
};



