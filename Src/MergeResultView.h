/**
 * @file  MergeResultView.h
 *
 * @brief Declaration of CMergeResultView class (kdiff3-style merge result pane)
 */
#pragma once

#include "GhostTextView.h"
#include "OptionsDiffColors.h"

class CMergeDoc;
struct DIFFRANGE;

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
	virtual void DrawMargin(const CRect & rect, int nLineIndex, int nLineNumber) override;
	virtual void OnUpdateCaret() override;
	void UpdateStatusbar();

	void RefreshOptions();
	/** @brief Scroll view so that the segment of the given diff is visible. */
	void ScrollToDiff(int nDiff);
	bool IsResultPaneVisible() const;
	/** @brief Make this view the frame's active, focused view. */
	void TakeFocus();
	void GetSelectedDiffs(int& firstDiff, int& lastDiff);

private:
	int LineToDiff(int nLine) const;
	int NextSignificantDiffFromLine(int nLine) const;
	int PrevSignificantDiffFromLine(int nLine) const;

	void SelectDiff(int nDiff, bool bScroll = true, bool bSelectText = true);
	bool IsDiffVisible(int nDiff);
	bool IsDiffVisible(const DIFFRANGE& diff, int nLinesBelow = 0);
	bool IsDiffFiltered(int nDiff);
	bool IsDiffFiltered(const DIFFRANGE& diff);
	int FindFirstNonFilteredDiff();
	int FindLastNonFilteredDiff();
	int FindNextNonFilteredDiff(int startDiff);
	int FindPrevNonFilteredDiff(int startDiff);
	bool HasNextNonFilteredDiff();
	bool HasPrevNonFilteredDiff();
	int FindPendingResultDiff(bool bNext);
	void OnUpdateNext3wayDiff(CCmdUI* pCmdUI, int nDiffType);
	void OnUpdatePrev3wayDiff(CCmdUI* pCmdUI, int nDiffType);

protected:
	COLORSETTINGS m_cachedColors; /**< Cached color settings */
	bool m_bSyncingCurrentDiff; /**< true while this view drives diff selection */

	//{{AFX_MSG(CMergeResultView)
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnForwardToMergeView(UINT nID);
	afx_msg void OnUpdateForwardToMergeView(CCmdUI* pCmdUI);
	afx_msg void OnWMGoto();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnCurdiff();
	afx_msg void OnUpdateCurdiff(CCmdUI* pCmdUI);
	afx_msg void OnFirstdiff();
	afx_msg void OnUpdateFirstdiff(CCmdUI* pCmdUI);
	afx_msg void OnLastdiff();
	afx_msg void OnUpdateLastdiff(CCmdUI* pCmdUI);
	afx_msg void OnNextdiff();
	afx_msg void OnUpdateNextdiff(CCmdUI* pCmdUI);
	afx_msg void OnPrevdiff();
	afx_msg void OnUpdatePrevdiff(CCmdUI* pCmdUI);
	afx_msg void OnNextConflict();
	afx_msg void OnUpdateNextConflict(CCmdUI* pCmdUI);
	afx_msg void OnPrevConflict();
	afx_msg void OnUpdatePrevConflict(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
