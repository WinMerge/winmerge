/**
 * @file  MergeResultView.cpp
 *
 * @brief Implementation of CMergeResultView (kdiff3-style merge result pane)
 */

#include "stdafx.h"
#include "MergeResultView.h"
#include "MergeResultPane.h"
#include "MergeResultContainer.h"
#include "MergeResultStatusBar.h"
#include "MergeDiffNavigation.h"
#include "Merge.h"
#include "MergeDoc.h"
#include "MergeEditView.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "SyntaxColors.h"
#include "BCMenu.h"
#include "I18nGUI.h"
#include "IDirDoc.h"
#include "../Externals/crystaledit/editlib/dialogs/gotodlg.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef WM_MOUSEHWHEEL
#  define WM_MOUSEHWHEEL 0x20e
#endif

IMPLEMENT_DYNCREATE(CMergeResultView, CGhostTextView)

CMergeResultView::CMergeResultView()
: m_bSyncingCurrentDiff(false)
{
	memset(&m_cachedColors, 0, sizeof(m_cachedColors));
	Options::DiffColors::Load(GetOptionsMgr(), m_cachedColors);
}

CMergeResultView::~CMergeResultView()
{
	// Make sure the document does not keep a dangling pointer to us
	CMergeDoc* pDoc = GetDocument();
	if (pDoc != nullptr && pDoc->GetMergeResultView() == this)
		pDoc->SetMergeResultView(nullptr);
}

BEGIN_MESSAGE_MAP(CMergeResultView, CGhostTextView)
	//{{AFX_MSG_MAP(CMergeResultView)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_WM_GETDLGCODE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEHWHEEL()
	// Difference/conflict navigation and Auto Merge are implemented by the
	// compare views; forward them so they also work while this view is active
	// [Edit] menu
	ON_COMMAND(ID_EDIT_WMGOTO, OnWMGoto)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	// [View] menu
	ON_COMMAND_RANGE(ID_VIEW_WORDWRAP, ID_VIEW_WORDWRAP, OnForwardToMergeView)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_WORDWRAP, ID_VIEW_WORDWRAP, OnUpdateForwardToMergeView)
	ON_COMMAND_RANGE(ID_VIEW_LINENUMBERS, ID_VIEW_LINENUMBERS, OnForwardToMergeView)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_LINENUMBERS, ID_VIEW_LINENUMBERS, OnUpdateForwardToMergeView)
	ON_COMMAND_RANGE(ID_VIEW_WHITESPACE, ID_VIEW_WHITESPACE, OnForwardToMergeView)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_WHITESPACE, ID_VIEW_WHITESPACE, OnUpdateForwardToMergeView)
	ON_COMMAND_RANGE(ID_VIEW_EOL, ID_VIEW_EOL, OnForwardToMergeView)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_EOL, ID_VIEW_EOL, OnUpdateForwardToMergeView)
	ON_COMMAND_RANGE(ID_VIEW_TOPMARGIN, ID_VIEW_TOPMARGIN, OnForwardToMergeView)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_EOL, ID_VIEW_EOL, OnUpdateForwardToMergeView)
	ON_COMMAND_RANGE(ID_VIEW_ZOOMIN, ID_VIEW_ZOOMNORMAL, OnForwardToMergeView)
	// [Merge] menu
	ON_COMMAND(ID_CURDIFF, OnCurdiff)
	ON_UPDATE_COMMAND_UI(ID_CURDIFF, OnUpdateCurdiff)
	ON_COMMAND(ID_FIRSTDIFF, OnFirstdiff)
	ON_UPDATE_COMMAND_UI(ID_FIRSTDIFF, OnUpdateFirstdiff)
	ON_COMMAND(ID_LASTDIFF, OnLastdiff)
	ON_UPDATE_COMMAND_UI(ID_LASTDIFF, OnUpdateLastdiff)
	ON_COMMAND(ID_NEXTDIFF, OnNextdiff)
	ON_UPDATE_COMMAND_UI(ID_NEXTDIFF, OnUpdateNextdiff)
	ON_COMMAND(ID_PREVDIFF, OnPrevdiff)
	ON_UPDATE_COMMAND_UI(ID_PREVDIFF, OnUpdatePrevdiff)
	ON_COMMAND(ID_NEXTCONFLICT, OnNextConflict)
	ON_UPDATE_COMMAND_UI(ID_NEXTCONFLICT, OnUpdateNextConflict)
	ON_COMMAND(ID_PREVCONFLICT, OnPrevConflict)
	ON_UPDATE_COMMAND_UI(ID_PREVCONFLICT, OnUpdatePrevConflict)
	ON_COMMAND_RANGE(ID_AUTO_MERGE, ID_AUTO_MERGE, OnForwardToMergeView)
	ON_UPDATE_COMMAND_UI_RANGE(ID_AUTO_MERGE, ID_AUTO_MERGE, OnUpdateForwardToMergeView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMergeDoc* CMergeResultView::GetDocument() const
{
	return static_cast<CMergeDoc*>(m_pDocument);
}

/**
 * @brief The attached buffer is the document's merge result buffer.
 */
CCrystalTextBuffer *CMergeResultView::LocateTextBuffer()
{
	return GetDocument()->GetMergeResultBuffer();
}

int CMergeResultView::LineToDiff(int nLine) const
{
	const MergeResultSegment* pSeg = GetDocument()->GetResultSegmentByLine(nLine);
	return (pSeg != nullptr) ? pSeg->diffIdx : -1;
}

int CMergeResultView::NextSignificantDiffFromLine(int nLine) const
{
	const CMergeDoc* pDoc = GetDocument();
	int nDiff = -1;
	const int size = (int)pDoc->m_diffList.GetSize();

	for (int i = 0; i < size; i++)
	{
		const auto* seg = pDoc->GetResultSegmentByDiff(i);
		if (seg && seg->diffIdx >= 0 && seg->nStartLine >= static_cast<int>(nLine))
		{
			nDiff = seg->diffIdx;
			break;
		}
	}
	return nDiff;
}

int CMergeResultView::PrevSignificantDiffFromLine(int nLine) const
{
	const CMergeDoc* pDoc = GetDocument();
	int nDiff = -1;
	const int size = (int)pDoc->m_diffList.GetSize();

	for (int i = size - 1; i >= 0 ; i--)
	{
		const auto* seg = pDoc->GetResultSegmentByDiff(i);
		if (seg && seg->diffIdx >= 0 && seg->nStartLine + seg->nLines - 1 <= static_cast<int>(nLine))
		{
			nDiff = seg->diffIdx;
			break;
		}
	}
	return nDiff;
}

int CMergeResultView::NextSignificant3wayDiffFromLine(int line, int nDiffType) const
{
	const CMergeDoc* pDoc = GetDocument();
	const MergeResultSegment* seg = pDoc->GetResultSegmentByLine(line);
	while (seg != nullptr && seg->diffIdx < 0)
	{
		line += seg->nLines;
		seg = pDoc->GetResultSegmentByLine(line);
	}
	if (!seg || seg->diffIdx == -1)
		return -1;
	return pDoc->m_diffList.NextSignificant3wayDiff(seg->diffIdx, nDiffType);
}

int CMergeResultView::PrevSignificant3wayDiffFromLine(int line, int nDiffType) const
{
	const CMergeDoc* pDoc = GetDocument();
	const MergeResultSegment* seg = pDoc->GetResultSegmentByLine(line);
	while (seg != nullptr && seg->diffIdx < 0)
	{
		line = seg->nStartLine - 1;
		seg = pDoc->GetResultSegmentByLine(line);
	}
	if (!seg || seg->diffIdx == -1)
		return -1;
	return pDoc->m_diffList.PrevSignificant3wayDiff(seg->diffIdx, nDiffType);
}

bool CMergeResultView::QueryEditable()
{
	CCrystalTextBuffer* pBuf = m_pTextBuffer;
	return pBuf != nullptr && !pBuf->GetReadOnly();
}

void CMergeResultView::OnInitialUpdate()
{
	// Do not attach to the buffer yet: the result buffer is only
	// initialized after the first rescan (BuildMergeResult()).
	CView::OnInitialUpdate();
	LOGFONT lf = theApp.m_lfDiff;
	lf.lfHeight = static_cast<LONG>(lf.lfHeight * GetOptionsMgr()->GetInt(OPT_VIEW_ZOOM) / 1000.0);
	SetFont(lf);
	// All documents & views share one set of syntax colors and markers
	SetColorContext(theApp.GetMainSyntaxColors());
	SetMarkersContext(theApp.GetMainMarkers());
}

void CMergeResultView::RefreshOptions()
{
	// Apply the same view options as the compare panes. Layout-affecting
	// calls need an attached buffer; before the first BuildMergeResult
	// this view has none.
	if (m_pTextBuffer != nullptr)
	{
		SetRenderingMode(static_cast<RENDERING_MODE>(GetOptionsMgr()->GetInt(OPT_RENDERING_MODE)));
		SetInsertTabs(GetOptionsMgr()->GetInt(OPT_TAB_TYPE) == 0);
		// The margin always stays on: it carries the per-line provenance
		// markers (which pane each result line was taken from)
		SetSelectionMargin(true);
		SetTopMargin(GetOptionsMgr()->GetBool(
		GetDocument()->m_ptBuf[0]->GetTableEditing() ? OPT_VIEW_TOPMARGIN_TABLE : OPT_VIEW_TOPMARGIN));
		SetViewLineNumbers(GetOptionsMgr()->GetBool(OPT_VIEW_LINENUMBERS));
		SetViewTabs(GetOptionsMgr()->GetBool(OPT_VIEW_WHITESPACE));
		SetViewEols(GetOptionsMgr()->GetBool(OPT_VIEW_EOL),
			GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL));
		SetWordWrapping(GetOptionsMgr()->GetBool(OPT_WORDWRAP));
	}
	Options::DiffColors::Load(GetOptionsMgr(), m_cachedColors);
	if (m_hWnd != nullptr)
	{
		Invalidate();
		UpdateStatusbar();
	}
}

/**
 * @brief Close the compare window with Esc, like the compare panes do.
 */
BOOL CMergeResultView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		if (GetOptionsMgr()->GetInt(OPT_CLOSE_WITH_ESC) != 0)
			GetParentFrame()->PostMessage(WM_CLOSE, 0, 0);
		return false;
	}
	return CGhostTextView::PreTranslateMessage(pMsg);
}

/**
 * @brief Color result lines by the resolution state of their segment.
 */
void CMergeResultView::GetLineColors(int nLineIndex, CEColor & crBkgnd,
		CEColor & crText, bool & bDrawWhitespace)
{
	// Default (syntax) colors first
	CGhostTextView::GetLineColors(nLineIndex, crBkgnd, crText, bDrawWhitespace);

	CMergeDoc* pDoc = GetDocument();
	if (pDoc == nullptr)
		return;
	const MergeResultSegment* pSegment = pDoc->GetResultSegmentByLine(nLineIndex);
	if (pSegment == nullptr || pSegment->state == ResultSegmentState::Common)
		return;

	const bool bCurrent = pSegment->diffIdx >= 0 &&
		(pSegment->diffIdx == pDoc->GetCurrentDiff());
	bDrawWhitespace = true;
	switch (pSegment->state)
	{
	case ResultSegmentState::Conflict:
		if (bCurrent)
		{
			crBkgnd = m_cachedColors.clrSelWordDiff;
			crText = m_cachedColors.clrSelWordDiffText;
		}
		else
		{
			crBkgnd = m_cachedColors.clrWordDiff;
			crText = m_cachedColors.clrWordDiffText;
		}
		break;
	case ResultSegmentState::Unresolved:
		// not decided yet, but the sides do not conflict
		if (bCurrent)
		{
			crBkgnd = m_cachedColors.clrSelDiff;
			crText = m_cachedColors.clrSelDiffText;
		}
		else
		{
			crBkgnd = m_cachedColors.clrDiff;
			crText = m_cachedColors.clrDiffText;
		}
		break;
	case ResultSegmentState::Auto:
	case ResultSegmentState::Chosen:
		if (bCurrent)
		{
			crBkgnd = m_cachedColors.clrSelDiff;
			crText = m_cachedColors.clrSelDiffText;
		}
		else
		{
			crBkgnd = m_cachedColors.clrDiff;
			crText = m_cachedColors.clrDiffText;
		}
		break;
	case ResultSegmentState::Edited:
		if (bCurrent)
		{
			crBkgnd = m_cachedColors.clrSelDiff;
			crText = m_cachedColors.clrSelDiffText;
		}
		else
		{
			crBkgnd = m_cachedColors.clrTrivial;
			crText = m_cachedColors.clrTrivialText;
		}
		break;
	default:
		break;
	}
}

/**
 * @brief Draw the provenance marker into the margin, kdiff3 style:
 * '1'/'2'/'3' = line taken from that pane (the toolbar button numbers),
 * '?' = unresolved placeholder, 'm' = segment modified by hand.
 */
void CMergeResultView::DrawMargin(const CRect & rect, int nLineIndex, int nLineNumber)
{
	CGhostTextView::DrawMargin(rect, nLineIndex, nLineNumber);
	if (nLineIndex < 0 || m_pTextBuffer == nullptr)
		return;
	CMergeDoc* pDoc = GetDocument();
	if (pDoc == nullptr)
		return;
	const tchar_t marker = pDoc->GetResultLineMarker(nLineIndex);
	if (marker == 0)
		return;
	m_pCrystalRenderer->SetTextColor(GetColor(COLORINDEX_NORMALTEXT));
	int nWidth = GetCharWidth();
	m_pCrystalRenderer->SwitchFont(false, false);
	m_pCrystalRenderer->DrawText(rect.left + 2, rect.top, rect, &marker, 1, &nWidth);
}

/**
 * @brief Ask for all keyboard input.
 *
 * This view lives inside a docking control bar, whose input
 * pre-translation runs the dialog-navigation logic (IsDialogMessage).
 * Without DLGC_WANTALLKEYS that logic swallows printable keys as
 * potential dialog mnemonics and the result cannot be edited by typing.
 * The compare panes are not hosted in a control bar, which is why they
 * are not affected.
 */
UINT CMergeResultView::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS | DLGC_WANTARROWS | DLGC_WANTCHARS;
}

/**
 * @brief Make this view the frame's active, focused view.
 *
 * A view hosted inside a docking bar does not always become the active
 * view through the default CView mouse activation path, in which case
 * keyboard input keeps going to a (read-only) compare pane and the
 * result cannot be edited by typing. Force activation explicitly.
 */
void CMergeResultView::TakeFocus()
{
	CFrameWnd* pFrame = GetParentFrame();
	if (pFrame != nullptr && pFrame->GetActiveView() != this)
		pFrame->SetActiveView(this);
	if (::GetFocus() != m_hWnd)
		SetFocus();
}

void CMergeResultView::OnLButtonUp(UINT nFlags, CPoint point)
{
	__super::OnLButtonUp(nFlags, point);
	CMergeDoc *pDoc = GetDocument();
	// If we have a selected diff, deselect it
	int nCurrentDiff = pDoc->GetCurrentDiff();
	if (nCurrentDiff != -1)
	{
		CEPoint pos = GetCursorPos();
		const MergeResultSegment* pSegment = pDoc->GetResultSegmentByLine(pos.y);
		if (pSegment == nullptr || pSegment->diffIdx != nCurrentDiff)
		{
			pDoc->SetCurrentDiff(-1);
			Invalidate();
			pDoc->UpdateAllViews(this);
		}
	}
}

/**
 * @brief Clicking a resolved/conflict segment selects the matching diff
 * in the compare panes.
 */
void CMergeResultView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	__super::OnLButtonDblClk(nFlags, point);

	CMergeDoc* pDoc = GetDocument();
	if (pDoc == nullptr || m_pTextBuffer == nullptr)
		return;
	const CEPoint pos = GetCursorPos();
	const MergeResultSegment* pSegment = pDoc->GetResultSegmentByLine(pos.y);
	if (pSegment == nullptr || pSegment->diffIdx < 0)
		return;
	if (pSegment->diffIdx == pDoc->GetCurrentDiff())
		return;
	m_bSyncingCurrentDiff = true;
	SelectDiff(pSegment->diffIdx, true, false);
	m_bSyncingCurrentDiff = false;
	Invalidate();
}

/**
 * @brief Run a navigation command on a compare view.
 * The current-diff machinery lives in CMergeEditView; when this view is
 * the active one those commands would otherwise have no handler.
 */
void CMergeResultView::OnForwardToMergeView(UINT nID)
{
	CMergeEditView* pView = GetDocument()->GetActiveMergeView();
	if (pView != nullptr && pView->GetSafeHwnd() != nullptr)
		pView->SendMessage(WM_COMMAND, nID);
}

void CMergeResultView::OnUpdateForwardToMergeView(CCmdUI* pCmdUI)
{
	CMergeEditView* pView = GetDocument()->GetActiveMergeView();
	if (pView == nullptr || pView->GetSafeHwnd() == nullptr ||
		!pView->OnCmdMsg(pCmdUI->m_nID, CN_UPDATE_COMMAND_UI, pCmdUI, nullptr))
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Show the result pane context menu (merge, edit and save commands).
 */
void CMergeResultView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (point.x == -1 && point.y == -1)
	{
		// Keyboard invocation: pop up at the caret
		CEPoint ptCursor = GetCursorPos();
		CPoint ptClient = TextToClient(ptCursor);
		ClientToScreen(&ptClient);
		point = ptClient;
	}

	BCMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_MERGERESULTVIEW));
	I18n::TranslateMenu(menu.m_hMenu);
	BCMenu* pSub = static_cast<BCMenu*>(menu.GetSubMenu(0));
	if (pSub != nullptr)
		pSub->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
			point.x, point.y, AfxGetMainWnd());
}

/**
 * @brief Scroll so the segment belonging to nDiff is visible.
 */
void CMergeResultView::ScrollToDiff(int nDiff)
{
	if (m_bSyncingCurrentDiff || m_pTextBuffer == nullptr)
	{
		Invalidate();
		return;
	}
	CMergeDoc* pDoc = GetDocument();
	if (pDoc == nullptr)
		return;
	const MergeResultSegment* pSegment = pDoc->GetResultSegmentByDiff(nDiff);
	if (pSegment == nullptr)
		return;
	const int nLine = (std::min)(pSegment->nStartLine, m_pTextBuffer->GetLineCount() - 1);
	if (nLine < 0)
		return;
	const CEPoint pt(0, nLine);
	SetCursorPos(pt);
	SetAnchor(pt);
	SetSelection(pt, pt);
	EnsureVisible(pt);
	Invalidate();
}

void CMergeResultView::GetSelectedDiffs(int& firstDiff, int& lastDiff)
{
	firstDiff = -1;
	lastDiff = -1;

	CMergeDoc* pDoc = GetDocument();
	const int nDiffCount = pDoc->m_diffList.GetSize();
	if (nDiffCount == 0)
		return;

	int firstLine, lastLine;
	auto [ptStart, ptEnd] = GetSelection();
	firstLine = ptStart.y;
	lastLine = ptEnd.y;

	for (int i = 0; i < nDiffCount; ++i)
	{
		const MergeResultSegment* seg = pDoc->GetResultSegmentByDiff(i);
		if (!seg || seg->nLines <= 0)
			continue;
		const int nSegEnd = seg->nStartLine + seg->nLines - 1;
		if (nSegEnd < firstLine)
			continue; // segment fully before selection
		if (seg->nStartLine > lastLine)
			break; // segment fully after selection
		if (seg->diffIdx >= 0)
		{
			if (firstDiff == -1 || seg->diffIdx < firstDiff)
				firstDiff = seg->diffIdx;
			if (lastDiff == -1 || seg->diffIdx > lastDiff)
				lastDiff = seg->diffIdx;
		}
	}
}

/**
 * @brief Is the hosting bar visible?
 */
bool CMergeResultView::IsResultPaneVisible() const
{
	const CWnd* pParent = GetParent();
	return pParent != nullptr && !!pParent->IsWindowVisible();
}

void CMergeResultView::UpdateStatusbar()
{
	OnUpdateCaret();
}

void CMergeResultView::OnUpdateCaret()
{
	if (m_bCursorHidden || !IsTextBufferInitialized())
		return;

	CMergeResultContainer* pContainer = dynamic_cast<CMergeResultContainer*>(GetParent());
	if (pContainer == nullptr)
		return;

	CMergeResultStatusBar* pStatusBar = pContainer->GetStatusBar();
	if (pStatusBar == nullptr)
		return;

	CEPoint cursorPos = GetCursorPos();
	int nScreenLine = cursorPos.y;
	CString sLine;
	sLine.Format(_T("%d"), nScreenLine + 1);
	int curChar = cursorPos.x + 1;
	int chars = GetLineLength(nScreenLine);
	int column = CalculateActualOffset(nScreenLine, cursorPos.x, true) + 1;
	int columns = CalculateActualOffset(nScreenLine, chars, true) + 1;
	chars++;

	auto [selectedLines, selectedChars] = GetSelectedLineAndCharacterCount();

	pStatusBar->SetLineInfo(sLine, column, columns, curChar, chars, selectedLines, selectedChars);

	// Update encoding and EOL info if available
	CMergeDoc* pDoc = GetDocument();
	if (pDoc != nullptr)
	{
		CMergeResultTextBuffer* pBuf = pDoc->GetMergeResultBuffer();
		if (pBuf != nullptr && pBuf->IsInitialized())
		{
			pStatusBar->SetEncodingAndEol(pBuf->getCodepage(), pBuf->getHasBom(), pBuf->GetDefaultEol());
			pStatusBar->SetReadOnly(pBuf->GetReadOnly());
		}
	}
}

void CMergeResultView::OnWMGoto()
{
	CGotoDlg dlg(this);
	dlg.DoModal();
}

void CMergeResultView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	__super::OnUpdateEditUndo(pCmdUI);
	pCmdUI->SetText(_("&Undo\tCtrl+Z").c_str());
}

void CMergeResultView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	__super::OnUpdateEditRedo(pCmdUI);
	pCmdUI->SetText(_("&Redo\tCtrl+Y").c_str());
}

/**
 * @brief Called when mouse's wheel is scrolled.
 */
BOOL CMergeResultView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (nFlags == MK_CONTROL)
	{
		CMergeEditView* pView = GetDocument()->GetActiveMergeView();
		if (pView != nullptr && pView->GetSafeHwnd() != nullptr)
			return (BOOL)pView->SendMessage(WM_MOUSEWHEEL, MAKELONG(nFlags, zDelta), MAKELONG(pt.x, pt.y));
	}
	if (nFlags == MK_SHIFT)
	{
		HandleHorizontalScrollWheel(-zDelta);
		// no default CCrystalTextView
		return CView::OnMouseWheel(nFlags, zDelta, pt);
	}
	return CGhostTextView::OnMouseWheel(nFlags, zDelta, pt);
}

/**
 * @brief Called when mouse's horizontal wheel is scrolled.
 */
void CMergeResultView::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	HandleHorizontalScrollWheel(zDelta);

	// no default CCrystalTextView
	CView::OnMouseHWheel(nFlags, zDelta, pt);
}

/**
 * @brief Selects diff by number and syncs other file
 * @param [in] nDiff Diff to select, must be >= 0
 * @param [in] bScroll Scroll diff to view
 * @param [in] bSelectText Select diff text
 * @sa CMergeEditView::ShowDiff()
 * @sa CMergeDoc::SetCurrentDiff()
 * @todo Parameter bSelectText is never used?
 */
void CMergeResultView::SelectDiff(int nDiff, bool bScroll /*= true*/, bool bSelectText /*= true*/)
{
	CMergeDoc *pDoc = GetDocument();
	CMergeEditView* pView = pDoc->GetActiveMergeView();
	if (pView == nullptr)
		return;
	pView->SelectDiff(nDiff, true, false);
}

/**
* @brief Determine if difference is visible on screen.
* @param [in] nDiff Number of diff to check.
* @return true if difference is visible.
*/
bool CMergeResultView::IsDiffVisible(int nDiff)
{
	return true;
}

/**
 * @brief Check if difference is hidden by line filter.
 * @param [in] nDiff Number of diff to check.
 * @return true if all lines in the diff block are hidden by filter.
 */
bool CMergeResultView::IsDiffFiltered(int nDiff)
{
	return false;
}

void CMergeResultView::OnCurdiff()
{
	CMergeDiffNavigation::OnCurdiff(this);
}

void CMergeResultView::OnUpdateCurdiff(CCmdUI* pCmdUI)
{
	CMergeDiffNavigation::OnUpdateCurdiff(this, pCmdUI);
}

void CMergeResultView::OnFirstdiff()
{
	CMergeDiffNavigation::OnFirstdiff(this);
}

void CMergeResultView::OnUpdateFirstdiff(CCmdUI* pCmdUI)
{
	CMergeDiffNavigation::OnUpdateFirstdiff(this, pCmdUI);
}

void CMergeResultView::OnLastdiff()
{
	CMergeDiffNavigation::OnLastdiff(this);
}

void CMergeResultView::OnUpdateLastdiff(CCmdUI* pCmdUI)
{
	CMergeDiffNavigation::OnUpdateLastdiff(this, pCmdUI);
}

void CMergeResultView::OnNextdiff()
{
	CMergeDiffNavigation::OnNextdiff(this);
}

void CMergeResultView::OnUpdateNextdiff(CCmdUI* pCmdUI)
{
	CMergeDiffNavigation::OnUpdateNextdiff(this, pCmdUI);
}

void CMergeResultView::OnPrevdiff()
{
	CMergeDiffNavigation::OnPrevdiff(this);
}

void CMergeResultView::OnUpdatePrevdiff(CCmdUI* pCmdUI)
{
	CMergeDiffNavigation::OnUpdatePrevdiff(this, pCmdUI);
}

void CMergeResultView::OnNextConflict()
{
	CMergeDiffNavigation::OnNextConflict(this);
}

void CMergeResultView::OnUpdateNextConflict(CCmdUI* pCmdUI)
{
	CMergeDiffNavigation::OnUpdateNextConflict(this, pCmdUI);
}

void CMergeResultView::OnPrevConflict()
{
	CMergeDiffNavigation::OnPrevConflict(this);
}

void CMergeResultView::OnUpdatePrevConflict(CCmdUI* pCmdUI)
{
	CMergeDiffNavigation::OnUpdatePrevConflict(this, pCmdUI);
}

void CMergeResultView::OnNext3wayDiff(int nDiffType)
{
	CMergeDiffNavigation::OnNext3wayDiff(this, nDiffType);
}

void CMergeResultView::OnPrev3wayDiff(int nDiffType)
{
	CMergeDiffNavigation::OnPrev3wayDiff(this, nDiffType);
}

/**
 * @brief Update "Next 3-way diff" UI items
 */
void CMergeResultView::OnUpdateNext3wayDiff(CCmdUI* pCmdUI, int nDiffType)
{
	CMergeDoc *pd = GetDocument();

	if (pd->m_nBuffers < 3)
	{
		pCmdUI->Enable(false);
		return;
	}

	const int nDiff = pd->m_diffList.LastSignificant3wayDiff(nDiffType);
	const MergeResultSegment* pSegment = pd->GetResultSegmentByDiff(nDiff);

	if (pSegment == nullptr)
	{
		// There aren't any significant differences
		pCmdUI->Enable(false);
	}
	else
	{
		// Enable if the beginning of the last significant difference is after caret
		CEPoint pos = GetCursorPos();
		pCmdUI->Enable((pd->GetCurrentDiff() < 0 && pos.y <= (long)pSegment->nStartLine)
			|| (pos.y < (long)pSegment->nStartLine));
	}
}

/**
 * @brief Update "Previous diff X and Y" UI items
 */
void CMergeResultView::OnUpdatePrev3wayDiff(CCmdUI* pCmdUI, int nDiffType)
{
	CMergeDoc *pd = GetDocument();

	if (pd->m_nBuffers < 3)
	{
		pCmdUI->Enable(false);
		return;
	}

	const int nDiff = pd->m_diffList.FirstSignificant3wayDiff(nDiffType);
	const MergeResultSegment* pSegment = pd->GetResultSegmentByDiff(nDiff);

	if (pSegment == nullptr)
	{
		// There aren't any significant differences
		pCmdUI->Enable(false);
	}
	else
	{
		// Enable if the end of the first significant difference is before caret
		CEPoint pos = GetCursorPos();
		pCmdUI->Enable((pd->GetCurrentDiff() < 0 && pos.y >= (long)pSegment->nStartLine + pSegment->nLines - 1)
			|| (pos.y > (long)pSegment->nStartLine + pSegment->nLines - 1));
	}
}

