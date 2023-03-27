/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  MergeEditView.h
 *
 * @brief Declaration file for CMergeEditView
 *
 */
#pragma once

/** 
 * @brief Non-diff lines shown above diff when scrolling to it
 */
constexpr int CONTEXT_LINES_ABOVE = 5;

/** 
 * @brief Non-diff lines shown below diff when scrolling to it
 */
constexpr int CONTEXT_LINES_BELOW = 3;


constexpr unsigned FLAG_RESCAN_WAITS_FOR_IDLE = 1;


/////////////////////////////////////////////////////////////////////////////
// CMergeEditView view
#include "edtlib.h"
#include "GhostTextView.h"
#include "OptionsDiffColors.h"
#include <map>
#include <vector>

class IMergeEditStatus;
class CLocationView;
class CMergeDoc;
struct DIFFRANGE;

/**
This class is the base class for WinMerge editor panels.
It hooks the painting of ghost lines (GetLineColors), the shared
scrollbar (OnUpdateSibling...).
It offers the UI interface commands to work with diffs 

@todo
If we keep GetLineColors here, we should clear DIFF flag here
and not in CGhostTextBuffer (when insertText/deleteText). 
Small problem... This class doesn't derives from CGhostTextBuffer... 
We could define a new class which derives from CGhostTextBuffer to clear the DIFF flag.
and calls a virtual function for additional things to do on the flag.
Maybe in the future...
*/
class CMergeEditView : public CGhostTextView
{
protected:
	CMergeEditView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMergeEditView)
	CCrystalParser m_xParser; /**< Syntax parser used for syntax highlighting. */

// Attributes
public:
	/**
	 * Index of pane this view is attached to.
	 * This indicates the pane number the view is attached to. If we swap panes
	 * then these indexes are changed.
	 */
	int m_nThisPane;
	int m_nThisGroup;
	bool m_bDetailView;
	IMergeEditStatus * m_piMergeEditStatus; /**< interface to status bar */

protected:
	/// first line of diff (first displayable line)
	int m_lineBegin;
	/// last line of diff (last displayable line)
	int m_lineEnd; 

private:
	/** 
	This flag is set when we receive an OnTimer command, and we want 
	to wait for theApp::OnIdle before processing it 
	*/
	unsigned fTimerWaitingForIdle;
	COLORSETTINGS m_cachedColors; /**< Cached color settings */

	bool m_bCurrentLineIsDiff; /**< `true` if cursor is in diff line */

// Attributes
public:

// Operations
public:
	void RefreshOptions();
	bool IsReadOnly(int pane) const;
	void ShowDiff(bool bScroll, bool bSelectText);
	virtual void OnEditOperation(int nAction, const tchar_t* pszText, size_t cchText) override;
	bool IsLineInCurrentDiff(int nLine) const;
	void SelectNone();
	void SelectDiff(int nDiff, bool bScroll = true, bool bSelectText = true);
	void DeselectDiffIfCursorNotInCurrentDiff();
	virtual CCrystalTextBuffer *LocateTextBuffer () override;
	const CCrystalTextBuffer *LocateTextBuffer () const { return const_cast<CMergeEditView *>(this)->LocateTextBuffer(); };
	void GetFullySelectedDiffs(int & firstDiff, int & lastDiff);
	void GetFullySelectedDiffs(int & firstDiff, int & lastDiff, int & firstWordDiff,  int & lastWordDiff, const CEPoint *pptStart = nullptr, const CEPoint *ppEnd = nullptr);
	void GetSelectedDiffs(int & firstDiff, int & lastDiff);
	std::map<int, std::vector<int>> GetColumnSelectedWordDiffIndice();
	CString GetSelectedText();
	std::pair<int, int> GetSelectedLineAndCharacterCount();
	CString GetLineText(int idx);
	CMergeDoc* GetDocument();
	const CMergeDoc *GetDocument() const { return const_cast<CMergeEditView *>(this)->GetDocument(); }
	void UpdateResources();
	bool IsModified() { return (LocateTextBuffer()->IsModified()); }
	void PrimeListWithFile();
	void SetStatusInterface(IMergeEditStatus * piMergeEditStatus);
	void SelectArea(const CEPoint & ptStart, const CEPoint & ptEnd) { SetSelection(ptStart, ptEnd); } // make public
	using CGhostTextView::GetSelection;
	virtual void UpdateSiblingScrollPos (bool bHorz) override;
    virtual std::vector<CrystalLineParser::TEXTBLOCK> GetMarkerTextBlocks(int nLineIndex) const override;
	virtual std::vector<CrystalLineParser::TEXTBLOCK> GetAdditionalTextBlocks (int nLineIndex) override;
	virtual CEColor GetColor(int nColorIndex) const override;
	virtual void GetLineColors (int nLineIndex, CEColor & crBkgnd,
			CEColor & crText, bool & bDrawWhitespace) override;
	virtual void GetLineColors2 (int nLineIndex, DWORD ignoreFlags
		, CEColor & crBkgnd, CEColor & crText, bool & bDrawWhitespace);
	void WMGoto() { OnWMGoto(); };
	void GotoLine(UINT nLine, bool bRealLine, int pane, bool bMoveAnchor = true, int nChar = -1);
	int GetTopLine() const { return m_nTopLine; }
	using CCrystalTextView::GetScreenLines;
	int GetTopSubLine() const { return m_nTopSubLine; }
	using CCrystalTextView::GetSubLines;
	using CCrystalTextView::GetSubLineCount;
	using CCrystalTextView::GetSubLineIndex;
	using CCrystalTextView::GetLineBySubLine;
	virtual int GetEmptySubLines( int nLineIndex ) override;
	virtual void InvalidateSubLineIndexCache( int nLineIndex ) override;
	void RepaintLocationPane();
	void DocumentsLoaded();
	void UpdateLocationViewPosition(int nTopLine = -1, int nBottomLine = -1);
	virtual void RecalcPageLayouts(CDC * pdc, CPrintInfo * pInfo) override;
	virtual void GetPrintHeaderText(int nPageNum, CString & text) override;
	virtual void PrintHeader(CDC * pdc, int nPageNum) override;
	virtual void PrintFooter(CDC * pdc, int nPageNum) override;
	virtual void SetWordWrapping( bool bWordWrap ) override;
	void UpdateStatusbar();
	CMergeEditView *GetGroupView(int nPane) const;

	virtual void OnDisplayDiff(int nDiff=0);

	bool IsInitialized() const;
	bool IsCursorInDiff() const;
	bool IsDiffVisible(int nDiff);
	void ZoomText(short amount);
	virtual bool QueryEditable() override;
	virtual void EnsureVisible(CEPoint pt) override;
	virtual void EnsureVisible(CEPoint ptStart, CEPoint ptEnd) override;
	bool EnsureInDiff(CEPoint& pt);
	void SetSelection(const CEPoint& ptStart, const CEPoint& ptEnd, bool bUpdateView = true) override;
	void ScrollToSubLine(int nNewTopLine, bool bNoSmoothScroll = false, bool bTrackScrollBar = true) override;
	void SetActivePane();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeEditView)
	public:
	virtual void OnInitialUpdate() override;
	protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	virtual void OnBeginPrinting (CDC * pDC, CPrintInfo * pInfo) override;
	virtual void OnEndPrinting (CDC * pDC, CPrintInfo * pInfo) override;
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo) override;
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMergeEditView();
	virtual void OnUpdateSibling (CCrystalTextView * pUpdateSource, bool bHorz) override;
	virtual void OnUpdateCaret() override;
	bool MergeModeKeyDown(MSG* pMsg);
	bool IsDiffVisible(const DIFFRANGE& diff, int nLinesBelow = 0);
	void OnNext3wayDiff(int type);
	void OnUpdateNext3wayDiff(CCmdUI* pCmdUI, int type);
	void OnPrev3wayDiff(int type);
	void OnUpdatePrev3wayDiff(CCmdUI* pCmdUI, int type);
	void OnDropFiles(const std::vector<String>& files);

	// Generated message map functions
protected:
	//{{AFX_MSG(CMergeEditView)
	afx_msg void OnCurdiff();
	afx_msg void OnUpdateCurdiff(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
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
	afx_msg void OnNextdiffLM();
	afx_msg void OnUpdateNextdiffLM(CCmdUI* pCmdUI);
	afx_msg void OnPrevdiffLM();
	afx_msg void OnUpdatePrevdiffLM(CCmdUI* pCmdUI);
	afx_msg void OnNextdiffLR();
	afx_msg void OnUpdateNextdiffLR(CCmdUI* pCmdUI);
	afx_msg void OnPrevdiffLR();
	afx_msg void OnUpdatePrevdiffLR(CCmdUI* pCmdUI);
	afx_msg void OnNextdiffMR();
	afx_msg void OnUpdateNextdiffMR(CCmdUI* pCmdUI);
	afx_msg void OnPrevdiffMR();
	afx_msg void OnUpdatePrevdiffMR(CCmdUI* pCmdUI);
	afx_msg void OnNextdiffLO();
	afx_msg void OnUpdateNextdiffLO(CCmdUI* pCmdUI);
	afx_msg void OnPrevdiffLO();
	afx_msg void OnUpdatePrevdiffLO(CCmdUI* pCmdUI);
	afx_msg void OnNextdiffMO();
	afx_msg void OnUpdateNextdiffMO(CCmdUI* pCmdUI);
	afx_msg void OnPrevdiffMO();
	afx_msg void OnUpdatePrevdiffMO(CCmdUI* pCmdUI);
	afx_msg void OnNextdiffRO();
	afx_msg void OnUpdateNextdiffRO(CCmdUI* pCmdUI);
	afx_msg void OnPrevdiffRO();
	afx_msg void OnUpdatePrevdiffRO(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnAllLeft();
	afx_msg void OnUpdateAllLeft(CCmdUI* pCmdUI);
	afx_msg void OnAllRight();
	afx_msg void OnUpdateAllRight(CCmdUI* pCmdUI);
	afx_msg void OnAutoMerge();
	afx_msg void OnUpdateAutoMerge(CCmdUI* pCmdUI);
	afx_msg void OnCopyX2Y(UINT nID);
	afx_msg void OnCopyLinesX2Y(UINT nID);
	afx_msg void OnX2Y(int srcPane, int dstPane, bool selectedLineOnly = false);
	afx_msg void OnUpdateX2Y(CCmdUI* pCmdUI);
	afx_msg void OnL2r();
	afx_msg void OnUpdateL2r(CCmdUI* pCmdUI);
	afx_msg void OnLinesL2r();
	afx_msg void OnUpdateLinesL2r(CCmdUI* pCmdUI);
	afx_msg void OnR2l();
	afx_msg void OnUpdateR2l(CCmdUI* pCmdUI);
	afx_msg void OnLinesR2l();
	afx_msg void OnUpdateLinesR2l(CCmdUI* pCmdUI);
	afx_msg void OnCopyFromLeft();
	afx_msg void OnUpdateCopyFromLeft(CCmdUI* pCmdUI);
	afx_msg void OnCopyLinesFromLeft();
	afx_msg void OnUpdateCopyLinesFromLeft(CCmdUI* pCmdUI);
	afx_msg void OnCopyFromRight();
	afx_msg void OnUpdateCopyFromRight(CCmdUI* pCmdUI);
	afx_msg void OnCopyLinesFromRight();
	afx_msg void OnUpdateCopyLinesFromRight(CCmdUI* pCmdUI);
	afx_msg void OnAddSyncPoint();
	afx_msg void OnClearSyncPoints();
	afx_msg void OnUpdateClearSyncPoints(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	template<bool reversed>
	afx_msg void OnSelectLineDiff();
	afx_msg void OnUpdateSelectLineDiff(CCmdUI* pCmdUI);
	afx_msg void OnAddToSubstitutionFilters();
	afx_msg void OnUpdateAddToSubstitutionFilters(CCmdUI* pCmdUI);
	afx_msg void OnAddToLineFilters();
	afx_msg void OnUpdateAddToLineFilters(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdateEditReplace(CCmdUI* pCmdUI);
	afx_msg void OnConvertEolTo(UINT nID );
	afx_msg void OnUpdateConvertEolTo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatusEOL(CCmdUI* pCmdUI);
	afx_msg void OnL2RNext();
	afx_msg void OnUpdateL2RNext(CCmdUI* pCmdUI);
	afx_msg void OnR2LNext();
	afx_msg void OnUpdateR2LNext(CCmdUI* pCmdUI);
	afx_msg void OnChangePane(UINT nID);
	afx_msg void OnWMGoto();
	afx_msg void OnGotoMovedLineLM();
	afx_msg void OnUpdateGotoMovedLineLM(CCmdUI* pCmdUI);
	afx_msg void OnGotoMovedLineMR();
	afx_msg void OnUpdateGotoMovedLineMR(CCmdUI* pCmdUI);
	afx_msg void OnShellMenu();
	afx_msg void OnUpdateShellMenu(CCmdUI* pCmdUI);
	afx_msg void OnScripts(UINT nID );
	afx_msg void OnTransformWithScript();
	afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar * pScrollBar);
	afx_msg void OnVScroll (UINT nSBCode, UINT nPos, CScrollBar * pScrollBar);
	afx_msg void OnEditCopyLineNumbers();
	afx_msg void OnUpdateEditCopyLinenumbers(CCmdUI* pCmdUI);
	afx_msg void OnViewLineDiffs();
	afx_msg void OnUpdateViewLineDiffs(CCmdUI* pCmdUI);
	afx_msg void OnViewLineNumbers();
	afx_msg void OnUpdateViewLineNumbers(CCmdUI* pCmdUI);
	afx_msg void OnViewWordWrap();
	afx_msg void OnUpdateViewWordWrap(CCmdUI* pCmdUI);
	afx_msg void OnViewWhitespace();
	afx_msg void OnUpdateViewWhitespace(CCmdUI* pCmdUI);
	afx_msg void OnViewEOL();
	afx_msg void OnUpdateViewEOL(CCmdUI* pCmdUI);
	afx_msg void OnOpenFile();
	afx_msg void OnOpenFileWith();
	afx_msg void OnOpenFileWithEditor();
	afx_msg void OnOpenParentFolder();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHelp();
	afx_msg void OnViewMargin();
	afx_msg void OnUpdateViewMargin(CCmdUI* pCmdUI);
	afx_msg void OnViewTopMargin();
	afx_msg void OnUpdateViewTopMargin(CCmdUI* pCmdUI);
	afx_msg void OnUseFirstLineAsHeaders();
	afx_msg void OnUpdateUseFirstLineAsHeaders(CCmdUI* pCmdUI);
	afx_msg void OnAutoFitAllColumns();
	afx_msg void OnUpdateViewChangeScheme(CCmdUI *pCmdUI);
	afx_msg void OnChangeScheme(UINT nID);
	afx_msg void OnUpdateChangeScheme(CCmdUI* pCmdUI);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnViewZoomIn();
	afx_msg void OnViewZoomOut();
	afx_msg void OnViewZoomNormal();
	afx_msg void OnWindowSplit();
	afx_msg void OnUpdateWindowSplit(CCmdUI* pCmdUI);
	afx_msg void OnStatusBarClick(NMHDR* pNMHDR, LRESULT* pResult);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DiffView.cpp
inline CMergeDoc* CMergeEditView::GetDocument()
   { return reinterpret_cast<CMergeDoc*>(m_pDocument); }
#endif

/**
 * @brief Check if cursor is inside difference.
 * @return true if cursor is inside difference.
 */
inline bool CMergeEditView::IsCursorInDiff() const
{
	return m_bCurrentLineIsDiff;
}

