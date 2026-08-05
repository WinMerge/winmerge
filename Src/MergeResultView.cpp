/**
 * @file  MergeResultView.cpp
 *
 * @brief Implementation of CMergeResultView (kdiff3-style merge result pane)
 */

#include "stdafx.h"
#include "MergeResultView.h"
#include "MergeResultPane.h"
#include "Merge.h"
#include "MergeDoc.h"
#include "MergeEditView.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "BCMenu.h"
#include "I18nGUI.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
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
	ON_WM_LBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_GETDLGCODE()
	// Difference/conflict navigation is implemented by the compare views;
	// forward it so it also works while the result view is active
	ON_COMMAND_RANGE(ID_PREVDIFF, ID_NEXTCONFLICT, OnForwardToMergeView)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PREVDIFF, ID_NEXTCONFLICT, OnUpdateForwardToMergeView)
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
		SetSelectionMargin(GetOptionsMgr()->GetBool(OPT_VIEW_FILEMARGIN));
		SetViewLineNumbers(GetOptionsMgr()->GetBool(OPT_VIEW_LINENUMBERS));
		SetViewTabs(GetOptionsMgr()->GetBool(OPT_VIEW_WHITESPACE));
		SetViewEols(GetOptionsMgr()->GetBool(OPT_VIEW_EOL),
			GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL));
		SetWordWrapping(GetOptionsMgr()->GetBool(OPT_WORDWRAP));
	}
	Options::DiffColors::Load(GetOptionsMgr(), m_cachedColors);
	if (m_hWnd != nullptr)
		Invalidate();
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
	if (pSegment == nullptr || pSegment->diffIdx == -1)
		return;

	const bool bCurrent = (pSegment->diffIdx == pDoc->GetCurrentDiff());
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

/**
 * @brief Clicking a resolved/conflict segment selects the matching diff
 * in the compare panes.
 */
void CMergeResultView::OnLButtonDown(UINT nFlags, CPoint point)
{
	TakeFocus();
	CGhostTextView::OnLButtonDown(nFlags, point);

	CMergeDoc* pDoc = GetDocument();
	if (pDoc == nullptr || m_pTextBuffer == nullptr)
		return;
	const CEPoint pos = GetCursorPos();
	const MergeResultSegment* pSegment = pDoc->GetResultSegmentByLine(pos.y);
	if (pSegment == nullptr || pSegment->diffIdx < 0)
		return;
	if (pSegment->diffIdx == pDoc->GetCurrentDiff())
		return;
	CMergeEditView* pView = pDoc->GetActiveMergeView();
	if (pView == nullptr)
		return;
	m_bSyncingCurrentDiff = true;
	pView->SelectDiff(pSegment->diffIdx, true, false);
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
	TakeFocus();

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

/**
 * @brief Is the hosting bar visible?
 */
bool CMergeResultView::IsResultPaneVisible() const
{
	const CWnd* pParent = GetParent();
	return pParent != nullptr && !!pParent->IsWindowVisible();
}
