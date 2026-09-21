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
#include "Merge.h"
#include "MergeDoc.h"
#include "MergeEditView.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "SyntaxColors.h"
#include "BCMenu.h"
#include "I18nGUI.h"
#include "../Externals/crystaledit/editlib/dialogs/gotodlg.h"
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
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEHWHEEL()
	// Difference/conflict navigation and Auto Merge are implemented by the
	// compare views; forward them so they also work while this view is active
	ON_COMMAND_RANGE(ID_PREVDIFF, ID_NEXTCONFLICT, OnForwardToMergeView)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PREVDIFF, ID_NEXTCONFLICT, OnUpdateForwardToMergeView)
	ON_COMMAND_RANGE(ID_FIRSTDIFF, ID_LASTDIFF, OnForwardToMergeView)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FIRSTDIFF, ID_LASTDIFF, OnUpdateForwardToMergeView)
	ON_COMMAND_RANGE(ID_AUTO_MERGE, ID_AUTO_MERGE, OnForwardToMergeView)
	ON_UPDATE_COMMAND_UI_RANGE(ID_AUTO_MERGE, ID_AUTO_MERGE, OnUpdateForwardToMergeView)
	ON_COMMAND(ID_EDIT_WMGOTO, OnWMGoto)
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
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
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

	if (!IsSelection())
	{
		CPoint pointClient = point;
		ScreenToClient(&pointClient);
		OnLButtonDown(0, pointClient);
		OnLButtonUp(0, pointClient);
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
		SCROLLINFO si = { sizeof SCROLLINFO };
		si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;

		VERIFY(GetScrollInfo(SB_HORZ, &si));

		// new horz pos
		si.nPos -= zDelta / 40;
		if (si.nPos > si.nMax) si.nPos = si.nMax;
		if (si.nPos < si.nMin) si.nPos = si.nMin;

		SetScrollInfo(SB_HORZ, &si);

		// for update
		SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, si.nPos) , NULL );

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
	SCROLLINFO si = { sizeof SCROLLINFO };
	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;

	VERIFY(GetScrollInfo(SB_HORZ, &si));

	// new horz pos
	si.nPos += zDelta / 40;
	if (si.nPos > si.nMax) si.nPos = si.nMax;
	if (si.nPos < si.nMin) si.nPos = si.nMin;

	SetScrollInfo(SB_HORZ, &si);

	// for update
	SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, si.nPos) , NULL );

	// no default CCrystalTextView
	CView::OnMouseHWheel(nFlags, zDelta, pt);
}

