/**
 * @file  MergeResultView.h
 *
 * @brief Declaration of CMergeResultView class (kdiff3-style merge result pane)
 */
#pragma once

#include "GhostTextView.h"
#include "OptionsDiffColors.h"

class CMergeDoc;

/**
 * @brief Editable view showing the 3-way merge result.
 *
 * This view shows the merge result buffer (CMergeDoc::GetMergeResultBuffer()).
 * It is not one of the compared panes: it has its own line numbering and
 * highlights result segments by their resolution state instead of by
 * buffer line flags.
 */
class CMergeResultView : public CGhostTextView
{
public:
	CMergeResultView();
	virtual ~CMergeResultView();
	DECLARE_DYNCREATE(CMergeResultView)

	CMergeDoc* GetDocument() const;

	virtual CCrystalTextBuffer *LocateTextBuffer() override;
	virtual void GetLineColors(int nLineIndex, CEColor & crBkgnd,
			CEColor & crText, bool & bDrawWhitespace) override;
	virtual bool QueryEditable() override;
	virtual void OnInitialUpdate() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	void RefreshOptions();
	/** @brief Scroll view so that the segment of the given diff is visible. */
	void ScrollToDiff(int nDiff);
	bool IsResultPaneVisible() const;
	/** @brief Make this view the frame's active, focused view. */
	void TakeFocus();

protected:
	COLORSETTINGS m_cachedColors; /**< Cached color settings */
	bool m_bSyncingCurrentDiff; /**< true while this view drives diff selection */

	//{{AFX_MSG(CMergeResultView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnForwardToMergeView(UINT nID);
	afx_msg void OnUpdateForwardToMergeView(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
