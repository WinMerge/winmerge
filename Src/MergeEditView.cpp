/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  MergeEditView.cpp
 *
 * @brief Implementation of the CMergeEditView class
 */

#include "StdAfx.h"
#include "MergeEditView.h"
#include <vector>
#include "BCMenu.h"
#include "Merge.h"
#include "LocationView.h"
#include "MergeDoc.h"
#include "MainFrm.h"
#include "OptionsMgr.h"
#include "OptionsDiffColors.h"
#include "WMGotoDlg.h"
#include "OptionsDef.h"
#include "SyntaxColors.h"
#include "MergeEditFrm.h"
#include "MergeLineFlags.h"
#include "paths.h"
#include "DropHandler.h"
#include "DirDoc.h"
#include "ShellContextMenu.h"
#include "editcmd.h"
#include "Shell.h"
#include "SelectPluginDlg.h"
#include "Constants.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef WM_MOUSEHWHEEL
#  define WM_MOUSEHWHEEL 0x20e
#endif

using std::vector;
using CrystalLineParser::TEXTBLOCK;

/** @brief Timer ID for delayed rescan. */
const UINT IDT_RESCAN = 2;
/** @brief Timer timeout for delayed rescan. */
const UINT RESCAN_TIMEOUT = 1000;

/////////////////////////////////////////////////////////////////////////////
// CMergeEditView

IMPLEMENT_DYNCREATE(CMergeEditView, CCrystalEditViewEx)

CMergeEditView::CMergeEditView()
: m_bCurrentLineIsDiff(false)
, m_nThisPane(0)
, m_nThisGroup(0)
, m_bDetailView(false)
, m_piMergeEditStatus(nullptr)
, fTimerWaitingForIdle(0)
, m_lineBegin(0)
, m_lineEnd(-1)
{
	SetParser(&m_xParser);
	
	Options::DiffColors::Load(GetOptionsMgr(), m_cachedColors);
}

CMergeEditView::~CMergeEditView()
{
}


BEGIN_MESSAGE_MAP(CMergeEditView, CCrystalEditViewEx)
	//{{AFX_MSG_MAP(CMergeEditView)
	ON_WM_CONTEXTMENU()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_VSCROLL ()
	ON_WM_HSCROLL ()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEHWHEEL()
	// [File] menu
	ON_COMMAND_RANGE(ID_EOL_TO_DOS, ID_EOL_TO_MAC, OnConvertEolTo)
	ON_UPDATE_COMMAND_UI_RANGE(ID_EOL_TO_DOS, ID_EOL_TO_MAC, OnUpdateConvertEolTo)
	// [Edit] menu
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REPLACE, OnUpdateEditReplace)
	ON_COMMAND(ID_EDIT_WMGOTO, OnWMGoto)
	ON_COMMAND(ID_EDIT_COPY_LINENUMBERS, OnEditCopyLineNumbers)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY_LINENUMBERS, OnUpdateEditCopyLinenumbers)
	// [View] menu
	ON_COMMAND(ID_SELECTLINEDIFF, OnSelectLineDiff<false>)
	ON_UPDATE_COMMAND_UI(ID_SELECTLINEDIFF, OnUpdateSelectLineDiff)
	ON_COMMAND(ID_SELECTPREVLINEDIFF, OnSelectLineDiff<true>)
	ON_UPDATE_COMMAND_UI(ID_SELECTPREVLINEDIFF, OnUpdateSelectLineDiff)
	ON_COMMAND(ID_VIEW_LINEDIFFS, OnViewLineDiffs)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LINEDIFFS, OnUpdateViewLineDiffs)
	ON_COMMAND(ID_VIEW_WORDWRAP, OnViewWordWrap)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WORDWRAP, OnUpdateViewWordWrap)
	ON_COMMAND(ID_VIEW_LINENUMBERS, OnViewLineNumbers)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LINENUMBERS, OnUpdateViewLineNumbers)
	ON_COMMAND(ID_VIEW_WHITESPACE, OnViewWhitespace)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WHITESPACE, OnUpdateViewWhitespace)
	ON_COMMAND(ID_VIEW_EOL, OnViewEOL)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EOL, OnUpdateViewEOL)
	ON_COMMAND(ID_VIEW_SELMARGIN, OnViewMargin)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SELMARGIN, OnUpdateViewMargin)
	ON_COMMAND(ID_VIEW_TOPMARGIN, OnViewTopMargin)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOPMARGIN, OnUpdateViewTopMargin)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CHANGESCHEME, OnUpdateViewChangeScheme)
	ON_COMMAND_RANGE(ID_COLORSCHEME_FIRST, ID_COLORSCHEME_LAST, OnChangeScheme)
	ON_UPDATE_COMMAND_UI_RANGE(ID_COLORSCHEME_FIRST, ID_COLORSCHEME_LAST, OnUpdateChangeScheme)
	ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomOut)
	ON_COMMAND(ID_VIEW_ZOOMNORMAL, OnViewZoomNormal)
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
	ON_COMMAND(ID_NEXTDIFFLM, OnNextdiffLM)
	ON_UPDATE_COMMAND_UI(ID_NEXTDIFFLM, OnUpdateNextdiffLM)
	ON_COMMAND(ID_PREVDIFFLM, OnPrevdiffLM)
	ON_UPDATE_COMMAND_UI(ID_PREVDIFFLM, OnUpdatePrevdiffLM)
	ON_COMMAND(ID_NEXTDIFFLR, OnNextdiffLR)
	ON_UPDATE_COMMAND_UI(ID_NEXTDIFFLR, OnUpdateNextdiffLR)
	ON_COMMAND(ID_PREVDIFFLR, OnPrevdiffLR)
	ON_UPDATE_COMMAND_UI(ID_PREVDIFFLR, OnUpdatePrevdiffLR)
	ON_COMMAND(ID_NEXTDIFFMR, OnNextdiffMR)
	ON_UPDATE_COMMAND_UI(ID_NEXTDIFFMR, OnUpdateNextdiffMR)
	ON_COMMAND(ID_PREVDIFFMR, OnPrevdiffMR)
	ON_UPDATE_COMMAND_UI(ID_PREVDIFFMR, OnUpdatePrevdiffMR)
	ON_COMMAND(ID_NEXTDIFFLO, OnNextdiffLO)
	ON_UPDATE_COMMAND_UI(ID_NEXTDIFFLO, OnUpdateNextdiffLO)
	ON_COMMAND(ID_PREVDIFFLO, OnPrevdiffLO)
	ON_UPDATE_COMMAND_UI(ID_PREVDIFFLO, OnUpdatePrevdiffLO)
	ON_COMMAND(ID_NEXTDIFFMO, OnNextdiffMO)
	ON_UPDATE_COMMAND_UI(ID_NEXTDIFFMO, OnUpdateNextdiffMO)
	ON_COMMAND(ID_PREVDIFFMO, OnPrevdiffMO)
	ON_UPDATE_COMMAND_UI(ID_PREVDIFFMO, OnUpdatePrevdiffMO)
	ON_COMMAND(ID_NEXTDIFFRO, OnNextdiffRO)
	ON_UPDATE_COMMAND_UI(ID_NEXTDIFFRO, OnUpdateNextdiffRO)
	ON_COMMAND(ID_PREVDIFFRO, OnPrevdiffRO)
	ON_UPDATE_COMMAND_UI(ID_PREVDIFFRO, OnUpdatePrevdiffRO)
	ON_COMMAND(ID_ALL_LEFT, OnAllLeft)
	ON_UPDATE_COMMAND_UI(ID_ALL_LEFT, OnUpdateAllLeft)
	ON_COMMAND(ID_ALL_RIGHT, OnAllRight)
	ON_UPDATE_COMMAND_UI(ID_ALL_RIGHT, OnUpdateAllRight)
	ON_COMMAND(ID_AUTO_MERGE, OnAutoMerge)
	ON_UPDATE_COMMAND_UI(ID_AUTO_MERGE, OnUpdateAutoMerge)
	ON_COMMAND(ID_L2R, OnL2r)
	ON_UPDATE_COMMAND_UI(ID_L2R, OnUpdateL2r)
	ON_COMMAND(ID_LINES_L2R, OnLinesL2r)
	ON_UPDATE_COMMAND_UI(ID_LINES_L2R, OnUpdateLinesL2r)
	ON_COMMAND(ID_R2L, OnR2l)
	ON_UPDATE_COMMAND_UI(ID_R2L, OnUpdateR2l)
	ON_COMMAND(ID_LINES_R2L, OnLinesR2l)
	ON_UPDATE_COMMAND_UI(ID_LINES_R2L, OnUpdateLinesR2l)
	ON_COMMAND(ID_COPY_FROM_LEFT, OnCopyFromLeft)
	ON_UPDATE_COMMAND_UI(ID_COPY_FROM_LEFT, OnUpdateCopyFromLeft)
	ON_COMMAND(ID_COPY_LINES_FROM_LEFT, OnCopyLinesFromLeft)
	ON_UPDATE_COMMAND_UI(ID_COPY_LINES_FROM_LEFT, OnUpdateCopyLinesFromLeft)
	ON_COMMAND(ID_COPY_FROM_RIGHT, OnCopyFromRight)
	ON_UPDATE_COMMAND_UI(ID_COPY_FROM_RIGHT, OnUpdateCopyFromRight)
	ON_COMMAND(ID_COPY_LINES_FROM_RIGHT, OnCopyLinesFromRight)
	ON_UPDATE_COMMAND_UI(ID_COPY_LINES_FROM_RIGHT, OnUpdateCopyLinesFromRight)
	ON_COMMAND(ID_L2RNEXT, OnL2RNext)
	ON_UPDATE_COMMAND_UI(ID_L2RNEXT, OnUpdateL2RNext)
	ON_COMMAND(ID_R2LNEXT, OnR2LNext)
	ON_UPDATE_COMMAND_UI(ID_R2LNEXT, OnUpdateR2LNext)
	ON_COMMAND(ID_ADD_SYNCPOINT, OnAddSyncPoint)
	ON_COMMAND(ID_CLEAR_SYNCPOINTS, OnClearSyncPoints)
	ON_UPDATE_COMMAND_UI(ID_CLEAR_SYNCPOINTS, OnUpdateClearSyncPoints)
	ON_COMMAND_RANGE(ID_COPY_TO_MIDDLE_L, ID_COPY_FROM_LEFT_R, OnCopyX2Y)
	ON_UPDATE_COMMAND_UI_RANGE(ID_COPY_TO_MIDDLE_L, ID_COPY_FROM_LEFT_R, OnUpdateX2Y)
	ON_COMMAND_RANGE(ID_COPY_LINES_TO_MIDDLE_L, ID_COPY_LINES_FROM_LEFT_R, OnCopyLinesX2Y)
	ON_UPDATE_COMMAND_UI_RANGE(ID_COPY_LINES_TO_MIDDLE_L, ID_COPY_LINES_FROM_LEFT_R, OnUpdateX2Y)
	// [Plugins] menu
	ON_COMMAND_RANGE(ID_SCRIPT_FIRST, ID_SCRIPT_LAST, OnScripts)
	ON_COMMAND(ID_TRANSFORM_WITH_SCRIPT, OnTransformWithScript)
	// [Window] menu
	ON_COMMAND_RANGE(ID_NEXT_PANE, ID_PREV_PANE, OnChangePane)
	ON_COMMAND(ID_WINDOW_SPLIT, OnWindowSplit)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_SPLIT, OnUpdateWindowSplit)
	// [Help] menu
	ON_COMMAND(ID_HELP, OnHelp)
	// Context menu
	ON_COMMAND(ID_ADD_TO_IGNORED_SUBSTITUTIONS, OnAddToSubstitutionFilters)
	ON_UPDATE_COMMAND_UI(ID_ADD_TO_IGNORED_SUBSTITUTIONS, OnUpdateAddToSubstitutionFilters)
	ON_COMMAND(ID_ADD_TO_LINE_FILTERS, OnAddToLineFilters)
	ON_UPDATE_COMMAND_UI(ID_ADD_TO_LINE_FILTERS, OnUpdateAddToLineFilters)
	ON_COMMAND(ID_GOTO_MOVED_LINE_LM, OnGotoMovedLineLM)
	ON_UPDATE_COMMAND_UI(ID_GOTO_MOVED_LINE_LM, OnUpdateGotoMovedLineLM)
	ON_COMMAND(ID_GOTO_MOVED_LINE_MR, OnGotoMovedLineMR)
	ON_UPDATE_COMMAND_UI(ID_GOTO_MOVED_LINE_MR, OnUpdateGotoMovedLineMR)
	ON_COMMAND(ID_FILE_SHELLMENU, OnShellMenu)
	ON_UPDATE_COMMAND_UI(ID_FILE_SHELLMENU, OnUpdateShellMenu)
	ON_COMMAND(ID_FILE_OPEN_REGISTERED, OnOpenFile)
	ON_COMMAND(ID_FILE_OPEN_WITHEDITOR, OnOpenFileWithEditor)
	ON_COMMAND(ID_FILE_OPEN_WITH, OnOpenFileWith)
	ON_COMMAND(ID_FILE_OPEN_PARENT_FOLDER, OnOpenParentFolder)
	// Context menu (Header)
	ON_COMMAND(ID_USE_FIRST_LINE_AS_HEADERS, OnUseFirstLineAsHeaders)
	ON_UPDATE_COMMAND_UI(ID_USE_FIRST_LINE_AS_HEADERS, OnUpdateUseFirstLineAsHeaders)
	ON_COMMAND(ID_AUTO_FIT_ALL_COLUMNS, OnAutoFitAllColumns)
	// Status bar
	ON_NOTIFY(NM_CLICK, AFX_IDW_STATUS_BAR, OnStatusBarClick)
	ON_UPDATE_COMMAND_UI(ID_STATUS_PANE0FILE_EOL, OnUpdateStatusEOL)
	ON_UPDATE_COMMAND_UI(ID_STATUS_PANE1FILE_EOL, OnUpdateStatusEOL)
	ON_UPDATE_COMMAND_UI(ID_STATUS_PANE2FILE_EOL, OnUpdateStatusEOL)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMergeEditView diagnostics

#ifdef _DEBUG
CMergeDoc* CMergeEditView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMergeDoc)));
	return (CMergeDoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMergeEditView message handlers

/**
 * @brief Return text buffer for file in view
 */
CCrystalTextBuffer *CMergeEditView::LocateTextBuffer()
{
	return GetDocument()->m_ptBuf[m_nThisPane].get();
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CMergeEditView::UpdateResources()
{
}

CMergeEditView *CMergeEditView::GetGroupView(int nBuffer) const
{
	return GetDocument()->GetView(m_nThisGroup, nBuffer);
}

void CMergeEditView::PrimeListWithFile()
{
	// Set the tab size now, just in case the options change...
	// We don't update it at the end of OnOptions,
	// we can update it safely now
	SetTabSize(GetOptionsMgr()->GetInt(OPT_TAB_SIZE));
}
/**
 * @brief Return text from line given
 */
CString CMergeEditView::GetLineText(int idx)
{
	return GetLineChars(idx);
}

/**
 * @brief Return text from selection
 */
CString CMergeEditView::GetSelectedText()
{
	CString strText;
	auto [ptStart, ptEnd] = GetSelection();
	if (ptStart != ptEnd)
		GetTextWithoutEmptys(ptStart.y, ptStart.x, ptEnd.y, ptEnd.x, strText);
	return strText;
}

/**
 * @brief Return number of selected characters
 */
std::pair<int, int> CMergeEditView::GetSelectedLineAndCharacterCount()
{
	auto [ptStart, ptEnd] = GetSelection();
	int nCharsOrColumns =0;
	int nSelectedLines = 0;
	for (int nLine = ptStart.y; nLine <= ptEnd.y; ++nLine)
	{
		if ((GetLineFlags(nLine) & (LF_GHOST | LF_INVISIBLE)) == 0)
		{
			int nLineLength = GetLineLength(nLine) + (m_pTextBuffer->GetLineEol(nLine)[0] ? 1 : 0);
			nCharsOrColumns += (nLine == ptEnd.y) ? ptEnd.x : nLineLength;
			if (nLine == ptStart.y)
				nCharsOrColumns -= ptStart.x;
			if (nLine < ptEnd.y || (ptStart != ptEnd && ptEnd.x > 0))
				++nSelectedLines;
		}
	}
	if (m_bRectangularSelection)
	{
		int nStartLeft, nStartRight, nEndLeft, nEndRight;
		GetColumnSelection(ptStart.y, nStartLeft, nStartRight);
		GetColumnSelection(ptEnd.y, nEndLeft, nEndRight);
		nCharsOrColumns = (std::max)(nStartRight, nEndRight) - (std::min)(nStartLeft, nEndLeft);
	}
	return { nSelectedLines, nCharsOrColumns };
}

/**
 * @brief Get diffs inside selection.
 * @param [out] firstDiff First diff inside selection
 * @param [out] lastDiff Last diff inside selection
 * @note -1 is returned in parameters if diffs cannot be determined
 * @todo This shouldn't be called when there is no diffs, so replace
 * first 'if' with ASSERT()?
 */
void CMergeEditView::GetFullySelectedDiffs(int & firstDiff, int & lastDiff)
{
	firstDiff = -1;
	lastDiff = -1;

	CMergeDoc *pd = GetDocument();
	const int nDiffs = pd->m_diffList.GetSignificantDiffs();
	if (nDiffs == 0)
		return;

	int firstLine, lastLine;
	GetFullySelectedLines(firstLine, lastLine);
	if (lastLine < firstLine)
		return;

	firstDiff = pd->m_diffList.NextSignificantDiffFromLine(firstLine);
	lastDiff = pd->m_diffList.PrevSignificantDiffFromLine(lastLine);
	if (firstDiff != -1 && lastDiff != -1)
	{
		DIFFRANGE di;

		// Check that first selected line is first diff's first line or above it
		VERIFY(pd->m_diffList.GetDiff(firstDiff, di));
		if ((int)di.dbegin < firstLine)
		{
			if (firstDiff < lastDiff)
				++firstDiff;
		}

		// Check that last selected line is last diff's last line or below it
		VERIFY(pd->m_diffList.GetDiff(lastDiff, di));
		if ((int)di.dend > lastLine)
		{
			if (firstDiff < lastDiff)
				--lastDiff;
		}

		// Special case: one-line diff is not selected if cursor is in it
		if (firstLine == lastLine)
		{
			firstDiff = -1;
			lastDiff = -1;
		}
	}
}

/**
 * @brief Get diffs inside selection.
 * @param [out] firstDiff First diff inside selection
 * @param [out] lastDiff Last diff inside selection
 * @param [out] firstWordDiff First word level diff inside selection
 * @param [out] lastWordDiff Last word level diff inside selection
 * @note -1 is returned in parameters if diffs cannot be determined
 * @todo This shouldn't be called when there is no diffs, so replace
 * first 'if' with ASSERT()?
 */
void CMergeEditView::GetFullySelectedDiffs(int & firstDiff, int & lastDiff, int & firstWordDiff, int & lastWordDiff, const CEPoint *pptStart, const CEPoint *pptEnd)
{
	firstDiff = -1;
	lastDiff = -1;
	firstWordDiff = -1;
	lastWordDiff = -1;

	CMergeDoc *pd = GetDocument();
	const int nDiffs = pd->m_diffList.GetSignificantDiffs();
	if (nDiffs == 0)
		return;

	int firstLine, lastLine;
	auto [ptStart, ptEnd] = GetSelection();
	if (pptStart != nullptr)
		ptStart = *pptStart;
	if (pptEnd != nullptr)
		ptEnd = *pptEnd;
	firstLine = ptStart.y;
	lastLine = ptEnd.y;

	firstDiff = pd->m_diffList.LineToDiff(firstLine);
	bool firstLineIsNotInDiff = firstDiff == -1;
	if (firstDiff == -1)
	{
		firstDiff = pd->m_diffList.NextSignificantDiffFromLine(firstLine);
		if (firstDiff == -1)
			return;
		firstWordDiff = 0;
	}
	lastDiff = pd->m_diffList.LineToDiff(lastLine);
	bool lastLineIsNotInDiff = lastDiff == -1;	
	if (lastDiff == -1)
		lastDiff = pd->m_diffList.PrevSignificantDiffFromLine(lastLine);
	if (lastDiff < firstDiff)
	{
		firstDiff = -1;
		firstWordDiff = -1;
		return;
	}

	if (firstDiff != -1 && lastDiff != -1)
	{
		DIFFRANGE di;
		
		if (pd->EqualCurrentWordDiff(m_nThisPane, ptStart, ptEnd))
		{
			firstWordDiff = lastWordDiff = static_cast<int>(pd->GetCurrentWordDiff().nWordDiff);
		}
		else if (ptStart != ptEnd)
		{
			VERIFY(pd->m_diffList.GetDiff(firstDiff, di));
			constexpr int LineLimit = 256;
			if ((lastLineIsNotInDiff && (firstLineIsNotInDiff || (di.dbegin == firstLine && ptStart.x == 0))) ||
			    (di.dend - di.dbegin > LineLimit))
			{
				firstWordDiff = -1;
				return;
			}

			if (firstWordDiff == -1)
			{
				vector<WordDiff> worddiffs = pd->GetWordDiffArrayInDiffBlock(firstDiff);
				for (size_t i = 0; i < worddiffs.size(); ++i)
				{
					int worddiffLen = worddiffs[i].end[m_nThisPane] - worddiffs[i].begin[m_nThisPane];
					if (worddiffs[i].endline[m_nThisPane] > firstLine ||
						(firstLine == worddiffs[i].endline[m_nThisPane] && 
						 worddiffs[i].end[m_nThisPane] - (worddiffLen == 0 ? 0 : 1) > ptStart.x))
					{
						firstWordDiff = static_cast<int>(i);
						break;
					}
				}

				if (firstLine >= di.dbegin && firstWordDiff == -1)
				{
					++firstDiff;
					firstWordDiff = 0;
				}
			}

			VERIFY(pd->m_diffList.GetDiff(lastDiff, di));
			vector<WordDiff> worddiffs = pd->GetWordDiffArrayInDiffBlock(lastDiff);
			for (size_t i = worddiffs.size() - 1; i != (size_t)-1; --i)
			{
				if (worddiffs[i].beginline[m_nThisPane] < lastLine ||
				    (lastLine == worddiffs[i].beginline[m_nThisPane] && worddiffs[i].begin[m_nThisPane] + 1 <= ptEnd.x))
				{
					lastWordDiff = static_cast<int>(i);
					break;
				}
			}

			if (lastLine <= di.dend && lastWordDiff == -1)
				--lastDiff;

			if (firstDiff == lastDiff && (lastWordDiff != -1 && lastWordDiff < firstWordDiff))
			{
				firstDiff = -1;
				lastDiff = -1;
				firstWordDiff = -1;
				lastWordDiff = -1;
			}
			else if (lastDiff < firstDiff || (firstDiff == lastDiff && firstWordDiff == -1 && lastWordDiff == -1))
			{
				firstDiff = -1;
				lastDiff = -1;
				firstWordDiff = -1;
				lastWordDiff = -1;
			}
		}
		else
		{
			firstDiff = -1;
			lastDiff = -1;
			firstWordDiff = -1;
			lastWordDiff = -1;
		}
	}

	ASSERT(firstDiff == -1 ? (lastDiff  == -1 && firstWordDiff == -1 && lastWordDiff == -1) : true);
	ASSERT(lastDiff  == -1 ? (firstDiff == -1 && firstWordDiff == -1 && lastWordDiff == -1) : true);
	ASSERT(firstDiff != -1 ? firstWordDiff != -1 : true);
}

void CMergeEditView::GetSelectedDiffs(int & firstDiff, int & lastDiff)
{
	firstDiff = -1;
	lastDiff = -1;

	CMergeDoc *pd = GetDocument();
	const int nDiffs = pd->m_diffList.GetSignificantDiffs();
	if (nDiffs == 0)
		return;

	int firstLine, lastLine;
	auto [ptStart, ptEnd] = GetSelection();
	firstLine = ptStart.y;
	lastLine = ptEnd.y;

	firstDiff = pd->m_diffList.LineToDiff(firstLine);
	if (firstDiff == -1)
	{
		firstDiff = pd->m_diffList.NextSignificantDiffFromLine(firstLine);
		if (firstDiff == -1)
			return;
	}
	lastDiff = pd->m_diffList.LineToDiff(lastLine);
	if (lastDiff == -1)
		lastDiff = pd->m_diffList.PrevSignificantDiffFromLine(lastLine);
	if (lastDiff < firstDiff)
	{
		firstDiff = -1;
		return;
	}

	ASSERT(firstDiff == -1 ? (lastDiff  == -1) : true);
	ASSERT(lastDiff  == -1 ? (firstDiff == -1) : true);
}

std::map<int, std::vector<int>> CMergeEditView::GetColumnSelectedWordDiffIndice()
{
	CMergeDoc *pDoc = GetDocument();
	std::map<int, std::vector<int>> ret;
	std::map<int, std::vector<int> *> list;
	auto [ptStart, ptEnd] = GetSelection();
	for (int nLine = ptStart.y; nLine <= ptEnd.y; ++nLine)
	{
		if (pDoc->m_diffList.LineToDiff(nLine) != -1)
		{
			int firstDiff, lastDiff, firstWordDiff, lastWordDiff;
			int nLeft, nRight;
			GetColumnSelection(nLine, nLeft, nRight);
			CEPoint ptStart2, ptEnd2;
			ptStart2.x = nLeft;
			ptEnd2.x = nRight;
			ptStart2.y = ptEnd2.y = nLine;
			GetFullySelectedDiffs(firstDiff, lastDiff, firstWordDiff, lastWordDiff, &ptStart2, &ptEnd2);
			if (firstDiff != -1 && lastDiff != -1)
			{
				std::vector<int> *pWordDiffs;
				if (list.find(firstDiff) == list.end())
					list.emplace(firstDiff, new std::vector<int>());
				pWordDiffs = list[firstDiff];
				for (int i = firstWordDiff; i <= lastWordDiff; ++i)
				{
					if (pWordDiffs->empty() || i != (*pWordDiffs)[pWordDiffs->size() - 1])
						pWordDiffs->push_back(i);
				}
			}
		}
	}
	for (auto& it : list)
		ret.emplace(it.first, *it.second);
	return ret;
}

void CMergeEditView::OnInitialUpdate()
{
	PushCursors();
	CCrystalEditViewEx::OnInitialUpdate();
	PopCursors();
	LOGFONT lf = dynamic_cast<CMainFrame*>(AfxGetMainWnd())->m_lfDiff;
	lf.lfHeight = static_cast<LONG>(lf.lfHeight * GetOptionsMgr()->GetInt(OPT_VIEW_ZOOM) / 1000.0);
	SetFont(lf);
	SetAlternateDropTarget(new DropHandler(std::bind(&CMergeEditView::OnDropFiles, this, std::placeholders::_1)));

	m_lineBegin = 0;
	m_lineEnd = -1;
}

void CMergeEditView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CCrystalEditViewEx::OnActivateView(bActivate, pActivateView, pDeactiveView);

	CMergeDoc* pDoc = GetDocument();
	pDoc->UpdateHeaderActivity(m_nThisPane, !!bActivate);
}

std::vector<CrystalLineParser::TEXTBLOCK> CMergeEditView::GetMarkerTextBlocks(int nLineIndex) const
{
	if (m_bDetailView)
	{
		if (nLineIndex < m_lineBegin || nLineIndex > m_lineEnd)
			return std::vector<CrystalLineParser::TEXTBLOCK>();
	}
	return CCrystalTextView::GetMarkerTextBlocks(nLineIndex);
}

std::vector<TEXTBLOCK> CMergeEditView::GetAdditionalTextBlocks (int nLineIndex)
{
	static const std::vector<TEXTBLOCK> emptyBlocks;
	if (m_bDetailView)
	{
		if (nLineIndex < m_lineBegin || nLineIndex > m_lineEnd)
			return emptyBlocks;
	}

	lineflags_t dwLineFlags = GetLineFlags(nLineIndex);
	if ((dwLineFlags & LF_SNP) == LF_SNP || (dwLineFlags & LF_DIFF) != LF_DIFF || (dwLineFlags & LF_MOVED) == LF_MOVED)
		return emptyBlocks;

	if (!GetOptionsMgr()->GetBool(OPT_WORDDIFF_HIGHLIGHT))
		return emptyBlocks;

	CMergeDoc *pDoc = GetDocument();
	if (pDoc->IsEditedAfterRescan(m_nThisPane))
		return emptyBlocks;
	
	int nDiff = pDoc->m_diffList.LineToDiff(nLineIndex);
	if (nDiff == -1)
		return emptyBlocks;

	DIFFRANGE cd;
	pDoc->m_diffList.GetDiff(nDiff, cd);
	int unemptyLineCount = 0;
	for (int nPane = 0; nPane < pDoc->m_nBuffers; nPane++)
	{
		if (cd.begin[nPane] != cd.end[nPane] + 1)
			unemptyLineCount++;
	}
	if (unemptyLineCount < 2)
		return emptyBlocks;

	vector<WordDiff> worddiffs = pDoc->GetWordDiffArray(nLineIndex);
	size_t nWordDiffs = worddiffs.size();

	bool lineInCurrentDiff = IsLineInCurrentDiff(nLineIndex);

	std::vector<TEXTBLOCK> blocks(nWordDiffs * 2 + 1);
	blocks[0].m_nCharPos = 0;
	blocks[0].m_nColorIndex = COLORINDEX_NONE;
	blocks[0].m_nBgColorIndex = COLORINDEX_NONE;
	size_t i, j;
	for (i = 0, j = 1; i < nWordDiffs; i++)
	{
		if (worddiffs[i].beginline[m_nThisPane] > nLineIndex || worddiffs[i].endline[m_nThisPane] < nLineIndex )
			continue;
		if (pDoc->m_nBuffers > 2)
		{
			if (m_nThisPane == 0 && worddiffs[i].op == OP_3RDONLY)
				continue;
			else if (m_nThisPane == 2 && worddiffs[i].op == OP_1STONLY)
				continue;
		}
		int begin[3]{}, end[3]{};
		bool deleted = false;
		for (int pane = 0; pane < pDoc->m_nBuffers; pane++)
		{
			begin[pane] = (worddiffs[i].beginline[pane] < nLineIndex) ? 0 : worddiffs[i].begin[pane];
			end[pane]   = (worddiffs[i].endline[pane]   > nLineIndex) ? GetGroupView(pane)->GetLineLength(nLineIndex) : worddiffs[i].end[pane];
			if (worddiffs[i].beginline[pane] == worddiffs[i].endline[pane] &&
				worddiffs[i].begin[pane] == worddiffs[i].end[pane])
				deleted = true;
		}
		blocks[j].m_nCharPos = begin[m_nThisPane];
		if (lineInCurrentDiff)
		{
			blocks[j].m_nColorIndex = COLORINDEX_APPLYFORCE |
				((m_cachedColors.clrSelWordDiffText != CLR_NONE) ? COLORINDEX_HIGHLIGHTTEXT1 : COLORINDEX_NONE);
			blocks[j].m_nBgColorIndex = COLORINDEX_APPLYFORCE | 
				(deleted ? COLORINDEX_HIGHLIGHTBKGND4 : COLORINDEX_HIGHLIGHTBKGND1);
		}
		else
		{
			blocks[j].m_nColorIndex = COLORINDEX_APPLYFORCE |
				((m_cachedColors.clrWordDiffText != CLR_NONE) ? COLORINDEX_HIGHLIGHTTEXT2 : COLORINDEX_NONE);
			blocks[j].m_nBgColorIndex = COLORINDEX_APPLYFORCE |
				(deleted ? COLORINDEX_HIGHLIGHTBKGND3 : COLORINDEX_HIGHLIGHTBKGND2);
		}
		j++;
		blocks[j].m_nCharPos = end[m_nThisPane];
		blocks[j].m_nColorIndex = COLORINDEX_NONE;
		blocks[j].m_nBgColorIndex = COLORINDEX_NONE;
		j++;
	}

	blocks.resize(j);

	return blocks;
}

CEColor CMergeEditView::GetColor(int nColorIndex) const
{
	switch (nColorIndex & ~COLORINDEX_MASK)
	{
	case COLORINDEX_HIGHLIGHTBKGND1:
		return m_cachedColors.clrSelWordDiff;
	case COLORINDEX_HIGHLIGHTTEXT1:
		return m_cachedColors.clrSelWordDiffText;
	case COLORINDEX_HIGHLIGHTBKGND2:
		return m_cachedColors.clrWordDiff;
	case COLORINDEX_HIGHLIGHTTEXT2:
		return m_cachedColors.clrWordDiffText;
	case COLORINDEX_HIGHLIGHTBKGND3:
		return m_cachedColors.clrWordDiffDeleted;
	case COLORINDEX_HIGHLIGHTBKGND4:
		return m_cachedColors.clrSelWordDiffDeleted;

	default:
		return CCrystalTextView::GetColor(nColorIndex);
	}
}

/**
 * @brief Determine text and background color for line
 * @param [in] nLineIndex Index of line in view (NOT line in file)
 * @param [out] crBkgnd Backround color for line
 * @param [out] crText Text color for line
 */
void CMergeEditView::GetLineColors(int nLineIndex, CEColor & crBkgnd,
                                CEColor & crText, bool & bDrawWhitespace)
{
	DWORD ignoreFlags = 0;
	GetLineColors2(nLineIndex, ignoreFlags, crBkgnd, crText, bDrawWhitespace);
}

/**
 * @brief Determine text and background color for line
 * @param [in] nLineIndex Index of line in view (NOT line in file)
 * @param [in] ignoreFlags Flags that caller wishes ignored
 * @param [out] crBkgnd Backround color for line
 * @param [out] crText Text color for line
 *
 * This version allows caller to suppress particular flags
 */
void CMergeEditView::GetLineColors2(int nLineIndex, DWORD ignoreFlags, CEColor & crBkgnd,
                                CEColor & crText, bool & bDrawWhitespace)
{
	if (GetLineCount() <= nLineIndex)
		return;

	lineflags_t dwLineFlags = GetLineFlags(nLineIndex);

	if (dwLineFlags & ignoreFlags)
		dwLineFlags &= (~ignoreFlags);

	if (m_bDetailView)
	{
		// Line with WinMerge flag, 
		// Lines with only the LF_DIFF/LF_TRIVIAL flags are not colored with Winmerge colors
		if (dwLineFlags & (LF_WINMERGE_FLAGS & ~LF_DIFF & ~LF_TRIVIAL & ~LF_MOVED & ~LF_SNP))
		{
			crText = m_cachedColors.clrDiffText;
			bDrawWhitespace = true;

			if (dwLineFlags & LF_GHOST)
			{
				crBkgnd = m_cachedColors.clrDiffDeleted;
			}
		}
		else
		{
			// If no syntax hilighting
			if (!GetOptionsMgr()->GetBool(OPT_SYNTAX_HIGHLIGHT))
			{
				crBkgnd = GetColor (COLORINDEX_BKGND);
				crText = GetColor (COLORINDEX_NORMALTEXT);
				bDrawWhitespace = false;
			}
			else
				// Line not inside diff, get colors from CrystalEditor
				CCrystalEditViewEx::GetLineColors(nLineIndex, crBkgnd,
					crText, bDrawWhitespace);
		}
		if (nLineIndex < m_lineBegin || nLineIndex > m_lineEnd)
		{
			crBkgnd = GetColor (COLORINDEX_WHITESPACE);
			crText = GetColor (COLORINDEX_WHITESPACE);
			bDrawWhitespace = false;
		}
		return;
	}

	// Line inside diff
	if (dwLineFlags & LF_WINMERGE_FLAGS)
	{
		crText = m_cachedColors.clrDiffText;
		bDrawWhitespace = true;
		bool lineInCurrentDiff = IsLineInCurrentDiff(nLineIndex);

		if (dwLineFlags & LF_SNP)
		{
			if (lineInCurrentDiff)
			{
				if (dwLineFlags & LF_GHOST)
					crBkgnd = m_cachedColors.clrSelSNPDeleted;
				else
					crBkgnd = m_cachedColors.clrSelSNP;
				crText = m_cachedColors.clrSelSNPText;
			}
			else
			{
				if (dwLineFlags & LF_GHOST)
					crBkgnd = m_cachedColors.clrSNPDeleted;
				else
					crBkgnd = m_cachedColors.clrSNP;
				crText = m_cachedColors.clrSNPText;
			}
			return;
		}
		else if (dwLineFlags & LF_DIFF)
		{
			if (lineInCurrentDiff)
			{
				if (dwLineFlags & LF_MOVED)
				{
					crBkgnd = m_cachedColors.clrSelMoved;
					crText = m_cachedColors.clrSelMovedText;
				}
				else
				{
					crBkgnd = m_cachedColors.clrSelDiff;
					crText = m_cachedColors.clrSelDiffText;
				}
			
			}
			else
			{
				if (dwLineFlags & LF_MOVED)
				{
					crBkgnd = m_cachedColors.clrMoved;
					crText = m_cachedColors.clrMovedText;
				}
				else
				{
					crBkgnd = m_cachedColors.clrDiff;
					crText = m_cachedColors.clrDiffText;
				}
			}
			return;
		}
		else if (dwLineFlags & LF_TRIVIAL)
		{
			// trivial diff can not be selected
			if (dwLineFlags & LF_GHOST)
				// ghost lines in trivial diff has their own color
				crBkgnd = m_cachedColors.clrTrivialDeleted;
			else
				crBkgnd = m_cachedColors.clrTrivial;
			crText = m_cachedColors.clrTrivialText;
			return;
		}
		else if (dwLineFlags & LF_GHOST)
		{
			if (lineInCurrentDiff)
			{
				if (dwLineFlags & LF_MOVED)
					crBkgnd = m_cachedColors.clrSelMovedDeleted;
				else
					crBkgnd = m_cachedColors.clrSelDiffDeleted;
			}
			else
			{
				if (dwLineFlags & LF_MOVED)
					crBkgnd = m_cachedColors.clrMovedDeleted;
				else
					crBkgnd = m_cachedColors.clrDiffDeleted;
			}
			return;
		}
	}
	else
	{
		// Line not inside diff,
		if (!GetOptionsMgr()->GetBool(OPT_SYNTAX_HIGHLIGHT))
		{
			// If no syntax hilighting, get windows default colors
			crBkgnd = GetColor (COLORINDEX_BKGND);
			crText = GetColor (COLORINDEX_NORMALTEXT);
			bDrawWhitespace = false;
		}
		else
			// Syntax highlighting, get colors from CrystalEditor
			CCrystalEditViewEx::GetLineColors(nLineIndex, crBkgnd,
				crText, bDrawWhitespace);
	}
}

/**
 * @brief Sync other pane position
 */
void CMergeEditView::UpdateSiblingScrollPos (bool bHorz)
{
	CSplitterWnd *pSplitterWnd = GetParentSplitter (this, false);
	if (pSplitterWnd != nullptr)
	{
		//  See CSplitterWnd::IdFromRowCol() implementation for details
		int nCurrentRow = (GetDlgCtrlID () - AFX_IDW_PANE_FIRST) / 16;
		int nCurrentCol = (GetDlgCtrlID () - AFX_IDW_PANE_FIRST) % 16;
		ASSERT (nCurrentRow >= 0 && nCurrentRow < pSplitterWnd->GetRowCount ());
		ASSERT (nCurrentCol >= 0 && nCurrentCol < pSplitterWnd->GetColumnCount ());

		// limit the TopLine : must be smaller than GetLineCount for all the panels
		int newTopSubLine = m_nTopSubLine;
		int nRows = pSplitterWnd->GetRowCount ();
		int nCols = pSplitterWnd->GetColumnCount ();
		int nRow=0;
//		for (nRow = 0; nRow < nRows; nRow++)
//		{
//			for (int nCol = 0; nCol < nCols; nCol++)
//			{
//				CMergeEditView *pSiblingView = static_cast<CMergeEditView*>(GetSiblingView (nRow, nCol));
//				if (pSiblingView != nullptr)
//					if (pSiblingView->GetSubLineCount() <= newTopSubLine)
//						newTopSubLine = pSiblingView->GetSubLineCount()-1;
//			}
//		}
		if (m_nTopSubLine != newTopSubLine)
			ScrollToSubLine(newTopSubLine);

		for (nRow = 0; nRow < nRows; nRow++)
		{
			for (int nCol = 0; nCol < nCols; nCol++)
			{
				if (!(nRow == nCurrentRow && nCol == nCurrentCol))  //  We don't need to update ourselves
				{
					CMergeEditView *pSiblingView = static_cast<CMergeEditView*>(GetSiblingView (nRow, nCol));
					if (pSiblingView != nullptr && pSiblingView->m_nThisGroup == m_nThisGroup)
						pSiblingView->OnUpdateSibling (this, bHorz);
				}
			}
		}
	}
}

/**
 * @brief Update other panes
 */
void CMergeEditView::OnUpdateSibling (CCrystalTextView * pUpdateSource, bool bHorz)
{
	if (pUpdateSource != this)
	{
		ASSERT (pUpdateSource != nullptr);
		ASSERT_KINDOF (CCrystalTextView, pUpdateSource);
		CMergeEditView *pSrcView = static_cast<CMergeEditView*>(pUpdateSource);
		if (!bHorz)  // changed this so bHorz works right
		{
			ASSERT (pSrcView->m_nTopSubLine >= 0);

			// This ASSERT is wrong: panes have different files and
			// different linecounts
			// ASSERT (pSrcView->m_nTopLine < GetLineCount ());
			if (pSrcView->m_nTopSubLine != m_nTopSubLine)
			{
				ScrollToSubLine (pSrcView->m_nTopSubLine, true, false);
				UpdateCaret ();
				RecalcVertScrollBar(true);
				InvalidateHorzScrollBar();
			}
		}
		else
		{
			ASSERT (pSrcView->m_nOffsetChar >= 0);

			// This ASSERT is wrong: panes have different files and
			// different linelengths
			// ASSERT (pSrcView->m_nOffsetChar < GetMaxLineLength ());
			if (pSrcView->m_nOffsetChar != m_nOffsetChar)
			{
				ScrollToChar (pSrcView->m_nOffsetChar, true, false);
				UpdateCaret ();
				RecalcHorzScrollBar(true);
				InvalidateHorzScrollBar();
			}
		}
	}
}

void CMergeEditView::OnDisplayDiff(int nDiff /*=0*/)
{
	int newlineBegin, newlineEnd;
	CMergeDoc *pd = GetDocument();
	if (nDiff < 0 || nDiff >= pd->m_diffList.GetSize())
	{
		newlineBegin = 0;
		newlineEnd = -1;
	}
	else
	{
		DIFFRANGE curDiff;
		VERIFY(pd->m_diffList.GetDiff(nDiff, curDiff));

		newlineBegin = curDiff.dbegin;
		ASSERT (newlineBegin >= 0);
		newlineEnd = curDiff.dend;
	}

	m_lineBegin = newlineBegin;
	m_lineEnd = newlineEnd;

	int nLineCount = GetLineCount();
	if (m_lineBegin > nLineCount)
		m_lineBegin = nLineCount - 1;
	if (m_lineEnd > nLineCount)
		m_lineEnd = nLineCount - 1;

	if (m_nTopLine == newlineBegin)
		return;

	// scroll to the first line of the diff
	vector<WordDiff> worddiffs;
	if (GetOptionsMgr()->GetBool(OPT_SCROLL_TO_FIRST_INLINE_DIFF))
		worddiffs = pd->GetWordDiffArrayInDiffBlock(nDiff);
	CEPoint pt = worddiffs.size() > 0 ?
		CEPoint{ worddiffs[0].begin[m_nThisPane], worddiffs[0].beginline[m_nThisPane] } : 
		CEPoint{ 0, m_lineBegin };
	ScrollToLine(m_lineBegin);
	if (pt.x > 0)
		EnsureVisible(pt);

	// update the width of the horizontal scrollbar
	InvalidateHorzScrollBar();
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
void CMergeEditView::SelectDiff(int nDiff, bool bScroll /*= true*/, bool bSelectText /*= true*/)
{
	CMergeDoc *pd = GetDocument();

	// Check that nDiff is valid
	if (nDiff < 0)
		_RPTF1(_CRT_ERROR, "Diffnumber negative (%d)", nDiff);
	if (nDiff >= pd->m_diffList.GetSize())
		_RPTF2(_CRT_ERROR, "Selected diff > diffcount (%d >= %d)",
			nDiff, pd->m_diffList.GetSize());

	SelectNone();
	pd->SetCurrentDiff(nDiff);
	ShowDiff(bScroll, bSelectText);
	pd->UpdateAllViews(this);
	UpdateSiblingScrollPos(false);

	// notify either side, as it will notify the other one
	pd->ForEachView ([&](auto& pView) { if (pView->m_bDetailView) pView->OnDisplayDiff(nDiff); });
}

void CMergeEditView::DeselectDiffIfCursorNotInCurrentDiff()
{
	CMergeDoc *pd = GetDocument();
	// If we have a selected diff, deselect it
	int nCurrentDiff = pd->GetCurrentDiff();
	if (nCurrentDiff != -1)
	{
		CEPoint pos = GetCursorPos();
		if (!IsLineInCurrentDiff(pos.y))
		{
			pd->SetCurrentDiff(-1);
			Invalidate();
			pd->UpdateAllViews(this);
		}
	}
}

/**
 * @brief Called when user selects "Current Difference".
 * Goes to active diff. If no active diff, selects diff under cursor
 * @sa CMergeEditView::SelectDiff()
 * @sa CMergeDoc::GetCurrentDiff()
 * @sa CMergeDoc::LineToDiff()
 */
void CMergeEditView::OnCurdiff()
{
	CMergeDoc *pd = GetDocument();

	// If no diffs, nothing to select
	if (!pd->m_diffList.HasSignificantDiffs())
		return;

	// GetCurrentDiff() returns -1 if no diff selected
	int nDiff = pd->GetCurrentDiff();
	if (nDiff != -1)
	{
		// Scroll to the first line of the currently selected diff
		SelectDiff(nDiff, true, false);
	}
	else
	{
		// If cursor is inside diff, select that diff
		CEPoint pos = GetCursorPos();
		nDiff = pd->m_diffList.LineToDiff(pos.y);
		if (nDiff != -1 && pd->m_diffList.IsDiffSignificant(nDiff))
			SelectDiff(nDiff, true, false);
	}
}

/**
 * @brief Called when "Current diff" item is updated
 */
void CMergeEditView::OnUpdateCurdiff(CCmdUI* pCmdUI)
{
	CMergeDoc *pd = GetDocument();
	int nCurrentDiff = pd->GetCurrentDiff();
	if (nCurrentDiff == -1)
	{
		CEPoint pos = GetCursorPos();
		int nNewDiff = pd->m_diffList.LineToDiff(pos.y);
		pCmdUI->Enable(nNewDiff != -1 && pd->m_diffList.IsDiffSignificant(nNewDiff));
	}
	else
		pCmdUI->Enable(true);
}

/**
 * @brief Copy selected text to clipboard
 */
void CMergeEditView::OnEditCopy()
{
	CMergeDoc * pDoc = GetDocument();
	auto [ptSelStart, ptSelEnd] = GetSelection();

	// Nothing selected
	if (ptSelStart == ptSelEnd)
		return;

	CString text;

	if (!m_bRectangularSelection)
	{
		CDiffTextBuffer * buffer = pDoc->m_ptBuf[m_nThisPane].get();

		String sText;
		buffer->GetTextWithoutEmptys(ptSelStart.y, ptSelStart.x,
			ptSelEnd.y, ptSelEnd.x, sText);
		text.SetString(sText.c_str(), static_cast<int>(sText.length())); // TODO: Use String instead of CString
	}
	else
		GetTextWithoutEmptysInColumnSelection(text);

	PutToClipboard(text, text.GetLength(), m_bRectangularSelection);
}

/**
 * @brief Called when "Copy" item is updated
 */
void CMergeEditView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	CCrystalEditViewEx::OnUpdateEditCopy(pCmdUI);
}

/**
 * @brief Cut current selection to clipboard
 */
void CMergeEditView::OnEditCut()
{
	if (!QueryEditable())
		return;

	CMergeDoc * pDoc = GetDocument();
	auto [ptSelStart, ptSelEnd] = GetSelection();

	// Nothing selected
	if (ptSelStart == ptSelEnd)
		return;

	CString text;
	if (!m_bRectangularSelection)
	{
		String sText;
		pDoc->m_ptBuf[m_nThisPane]->GetTextWithoutEmptys(ptSelStart.y, ptSelStart.x,
			ptSelEnd.y, ptSelEnd.x, sText);
		text.SetString(sText.c_str(), static_cast<int>(sText.length())); // TODO: Use String instead of CString
	}
	else
		GetTextWithoutEmptysInColumnSelection(text);

	PutToClipboard(text, text.GetLength(), m_bRectangularSelection);

	if (!m_bRectangularSelection)
	{
		CEPoint ptCursorPos = ptSelStart;
		ASSERT_VALIDTEXTPOS(ptCursorPos);
		SetAnchor(ptCursorPos);
		SetSelection(ptCursorPos, ptCursorPos);
		SetCursorPos(ptCursorPos);
		EnsureVisible(ptCursorPos);

		pDoc->m_ptBuf[m_nThisPane]->DeleteText(this, ptSelStart.y, ptSelStart.x, ptSelEnd.y,
			ptSelEnd.x, CE_ACTION_CUT);
	}
	else
		DeleteCurrentColumnSelection (CE_ACTION_CUT);

	m_pTextBuffer->SetModified(true);
}

/**
 * @brief Called when "Cut" item is updated
 */
void CMergeEditView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	if (QueryEditable())
		CCrystalEditViewEx::OnUpdateEditCut(pCmdUI);
	else
		pCmdUI->Enable(false);
}

/**
 * @brief Paste text from clipboard
 */
void CMergeEditView::OnEditPaste()
{
	if (!QueryEditable())
		return;

	CCrystalEditViewEx::Paste();
	m_pTextBuffer->SetModified(true);
}

/**
 * @brief Called when "Paste" item is updated
 */
void CMergeEditView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	if (QueryEditable())
		CCrystalEditViewEx::OnUpdateEditPaste(pCmdUI);
	else
		pCmdUI->Enable(false);
}

/**
 * @brief Undo last action
 */
void CMergeEditView::OnEditUndo()
{
	CWaitCursor waitstatus;
	CMergeDoc* pDoc = GetDocument();
	CMergeEditView *tgt = pDoc->GetView(m_nThisGroup, *(pDoc->curUndo-1));
	if(tgt==this)
	{
		if (!QueryEditable())
			return;

		GetParentFrame()->SetActiveView(this, true);
		if(CCrystalEditViewEx::DoEditUndo())
		{
			--pDoc->curUndo;
			pDoc->UpdateHeaderPath(m_nThisPane);
			pDoc->FlushAndRescan();

			int nAction;
			m_pTextBuffer->GetRedoActionCode(nAction);
			if (nAction == CE_ACTION_MERGE)
				// select the diff so we may just merge it again
				OnCurdiff();
		}
	}
	else
	{
		tgt->SendMessage(WM_COMMAND, ID_EDIT_UNDO);
	}
	if (!pDoc->CanUndo())
		pDoc->SetAutoMerged(false);
}

/**
 * @brief Called when "Undo" item is updated
 */
void CMergeEditView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	CMergeDoc* pDoc = GetDocument();
	if (pDoc->curUndo!=pDoc->undoTgt.begin())
	{
		CMergeEditView *tgt = pDoc->GetView(m_nThisGroup, *(pDoc->curUndo-1));
		pCmdUI->Enable( !IsReadOnly(tgt->m_nThisPane));
	}
	else
		pCmdUI->Enable(false);
}

/**
 * @brief Go to first diff
 *
 * Called when user selects "First Difference"
 * @sa CMergeEditView::SelectDiff()
 */
void CMergeEditView::OnFirstdiff()
{
	CMergeDoc *pd = GetDocument();
	if (pd->m_diffList.HasSignificantDiffs())
	{
		int nDiff = pd->m_diffList.FirstSignificantDiff();
		SelectDiff(nDiff, true, false);
	}
}

/**
 * @brief Update "First diff" UI items
 */
void CMergeEditView::OnUpdateFirstdiff(CCmdUI* pCmdUI)
{
	OnUpdatePrevdiff(pCmdUI);
}

/**
 * @brief Go to last diff
 */
void CMergeEditView::OnLastdiff()
{
	CMergeDoc *pd = GetDocument();
	if (pd->m_diffList.HasSignificantDiffs())
	{
		int nDiff = pd->m_diffList.LastSignificantDiff();
		SelectDiff(nDiff, true, false);
	}
}

/**
 * @brief Update "Last diff" UI items
 */
void CMergeEditView::OnUpdateLastdiff(CCmdUI* pCmdUI)
{
	OnUpdateNextdiff(pCmdUI);
}

/**
 * @brief Go to next diff and select it.
 *
 * Finds and selects next difference. There are several cases:
 * - if there is selected difference, and that difference is visible
 * on screen, next found difference is selected.
 * - if there is selected difference but it is not visible, next
 * difference from cursor position is selected. This is what user
 * expects to happen and is natural thing to do. Also reduces
 * needless scrolling.
 * - if there is no selected difference, next difference from cursor
 * position is selected.
 */
void CMergeEditView::OnNextdiff()
{
	CMergeDoc *pd = GetDocument();
	int cnt = pd->m_ptBuf[0]->GetLineCount();
	if (cnt <= 0)
		return;

	// Returns -1 if no diff selected
	int nextDiff = -1;
	int curDiff = pd->GetCurrentDiff();
	if (curDiff != -1)
	{
		// We're on a diff
		if (!IsDiffVisible(curDiff))
		{
			// Selected difference not visible, select next from cursor
			int line = GetCursorPos().y;
			// Make sure we aren't in the first line of the diff
			++line;
			if (!IsValidTextPosY(CEPoint(0, line)))
				line = m_nTopLine;
			nextDiff = pd->m_diffList.NextSignificantDiffFromLine(line);
		}
		else
		{
			// Find out if there is a following significant diff
			if (curDiff < pd->m_diffList.GetSize() - 1)
			{
				nextDiff = pd->m_diffList.NextSignificantDiff(curDiff);
			}
		}
	}
	else
	{
		// We don't have a selected difference,
		// but cursor can be inside inactive diff
		int line = GetCursorPos().y;
		if (!IsValidTextPosY(CEPoint(0, line)))
			line = m_nTopLine;
		nextDiff = pd->m_diffList.NextSignificantDiffFromLine(line);
	}

	int lastDiff = pd->m_diffList.LastSignificantDiff();
	if (nextDiff >= 0 && nextDiff <= lastDiff)
		SelectDiff(nextDiff, true, false);
	else if (CDirDoc *pDirDoc = pd->GetDirDoc())
	{
		if (pDirDoc->MoveableToNextDiff())
			pDirDoc->MoveToNextDiff(pd);
	}
}

/**
 * @brief Update "Next diff" UI items
 */
void CMergeEditView::OnUpdateNextdiff(CCmdUI* pCmdUI)
{
	CMergeDoc *pd = GetDocument();
	const DIFFRANGE * dfi = pd->m_diffList.LastSignificantDiffRange();
	bool enabled;

	if (dfi == nullptr)
	{
		// There aren't any significant differences
		enabled = false;
	}
	else
	{
		// Enable if the beginning of the last significant difference is after caret
		enabled = (GetCursorPos().y < (long)dfi->dbegin);
	}

	if (!enabled && pd->GetDirDoc())
		enabled = pd->GetDirDoc()->MoveableToNextDiff();

	pCmdUI->Enable(enabled);
}

/**
 * @brief Go to previous diff and select it.
 *
 * Finds and selects previous difference. There are several cases:
 * - if there is selected difference, and that difference is visible
 * on screen, previous found difference is selected.
 * - if there is selected difference but it is not visible, previous
 * difference from cursor position is selected. This is what user
 * expects to happen and is natural thing to do. Also reduces
 * needless scrolling.
 * - if there is no selected difference, previous difference from cursor
 * position is selected.
 */
void CMergeEditView::OnPrevdiff()
{
	CMergeDoc *pd = GetDocument();
	int cnt = pd->m_ptBuf[0]->GetLineCount();
	if (cnt <= 0)
		return;

	// GetCurrentDiff() returns -1 if no diff selected
	int prevDiff = -1;
	int curDiff = pd->GetCurrentDiff();
	if (curDiff != -1)
	{
		// We're on a diff
		if (!IsDiffVisible(curDiff))
		{
			// Selected difference not visible, select previous from cursor
			int line = GetCursorPos().y;
			// Make sure we aren't in the last line of the diff
			--line;
			if (!IsValidTextPosY(CEPoint(0, line)))
				line = m_nTopLine;
			prevDiff = pd->m_diffList.PrevSignificantDiffFromLine(line);
		}
		else
		{
			// Find out if there is a preceding significant diff
			if (curDiff > 0)
			{
				prevDiff = pd->m_diffList.PrevSignificantDiff(curDiff);
			}
		}
	}
	else
	{
		// We don't have a selected difference,
		// but cursor can be inside inactive diff
		int line = GetCursorPos().y;
		if (!IsValidTextPosY(CEPoint(0, line)))
			line = m_nTopLine;
		prevDiff = pd->m_diffList.PrevSignificantDiffFromLine(line);
	}

	int firstDiff = pd->m_diffList.FirstSignificantDiff();
	if (prevDiff >= 0 && prevDiff >= firstDiff)
		SelectDiff(prevDiff, true, false);
	else if (CDirDoc *pDirDoc = pd->GetDirDoc())
	{
		if (pDirDoc->MoveableToPrevDiff())
			pDirDoc->MoveToPrevDiff(pd);
	}
}

/**
 * @brief Update "Previous diff" UI items
 */
void CMergeEditView::OnUpdatePrevdiff(CCmdUI* pCmdUI)
{
	CMergeDoc *pd = GetDocument();
	const DIFFRANGE * dfi = pd->m_diffList.FirstSignificantDiffRange();
	bool enabled;

	if (dfi == nullptr)
	{
		// There aren't any significant differences
		enabled = false;
	}
	else
	{
		// Enable if the end of the first significant difference is before caret
		enabled = (GetCursorPos().y > (long)dfi->dend);
	}

	if (!enabled && pd->GetDirDoc())
		enabled = pd->GetDirDoc()->MoveableToPrevDiff();

	pCmdUI->Enable(enabled);
}

void CMergeEditView::OnNextConflict()
{
	OnNext3wayDiff(THREEWAYDIFFTYPE_CONFLICT);
}

/**
 * @brief Update "Next Conflict" UI items
 */
void CMergeEditView::OnUpdateNextConflict(CCmdUI* pCmdUI)
{
	OnUpdateNext3wayDiff(pCmdUI, THREEWAYDIFFTYPE_CONFLICT);
}

void CMergeEditView::OnPrevConflict()
{
	OnPrev3wayDiff(THREEWAYDIFFTYPE_CONFLICT);
}

/**
 * @brief Update "Prev Conflict" UI items
 */
void CMergeEditView::OnUpdatePrevConflict(CCmdUI* pCmdUI)
{
	OnUpdatePrev3wayDiff(pCmdUI, THREEWAYDIFFTYPE_CONFLICT);
}

/**
 * @brief Go to next 3-way diff and select it.
 */
void CMergeEditView::OnNext3wayDiff(int nDiffType)
{
	CMergeDoc *pd = GetDocument();
	int cnt = pd->m_ptBuf[0]->GetLineCount();
	if (cnt <= 0)
		return;

	// Returns -1 if no diff selected
	int curDiff = pd->GetCurrentDiff();
	if (curDiff != -1)
	{
		// We're on a diff
		int nextDiff = curDiff;
		if (!IsDiffVisible(curDiff))
		{
			// Selected difference not visible, select next from cursor
			int line = GetCursorPos().y;
			// Make sure we aren't in the first line of the diff
			++line;
			if (!IsValidTextPosY(CEPoint(0, line)))
				line = m_nTopLine;
			nextDiff = pd->m_diffList.NextSignificant3wayDiffFromLine(line, nDiffType);
		}
		else
		{
			// Find out if there is a following significant diff
			if (curDiff < pd->m_diffList.GetSize() - 1)
			{
				nextDiff = pd->m_diffList.NextSignificant3wayDiff(curDiff, nDiffType);
			}
		}
		if (nextDiff == -1)
			nextDiff = curDiff;

		// nextDiff is the next one if there is one, else it is the one we're on
		SelectDiff(nextDiff, true, false);
	}
	else
	{
		// We don't have a selected difference,
		// but cursor can be inside inactive diff
		int line = GetCursorPos().y;
		if (!IsValidTextPosY(CEPoint(0, line)))
			line = m_nTopLine;
		curDiff = pd->m_diffList.NextSignificant3wayDiffFromLine(line, nDiffType);
		if (curDiff >= 0)
			SelectDiff(curDiff, true, false);
	}
}

/**
 * @brief Update "Next 3-way diff" UI items
 */
void CMergeEditView::OnUpdateNext3wayDiff(CCmdUI* pCmdUI, int nDiffType)
{
	CMergeDoc *pd = GetDocument();

	if (pd->m_nBuffers < 3)
	{
		pCmdUI->Enable(false);
		return;
	}

	const DIFFRANGE * dfi = pd->m_diffList.LastSignificant3wayDiffRange(nDiffType);

	if (dfi == nullptr)
	{
		// There aren't any significant differences
		pCmdUI->Enable(false);
	}
	else
	{
		// Enable if the beginning of the last significant difference is after caret
		CEPoint pos = GetCursorPos();
		pCmdUI->Enable(pos.y < (long)dfi->dbegin);
	}
}

/**
 * @brief Go to previous 3-way diff and select it.
 */
void CMergeEditView::OnPrev3wayDiff(int nDiffType)
{
	CMergeDoc *pd = GetDocument();

	int cnt = pd->m_ptBuf[0]->GetLineCount();
	if (cnt <= 0)
		return;

	// GetCurrentDiff() returns -1 if no diff selected
	int curDiff = pd->GetCurrentDiff();
	if (curDiff != -1)
	{
		// We're on a diff
		int prevDiff = curDiff;
		if (!IsDiffVisible(curDiff))
		{
			// Selected difference not visible, select previous from cursor
			int line = GetCursorPos().y;
			// Make sure we aren't in the last line of the diff
			--line;
			if (!IsValidTextPosY(CEPoint(0, line)))
				line = m_nTopLine;
			prevDiff = pd->m_diffList.PrevSignificant3wayDiffFromLine(line, nDiffType);
		}
		else
		{
			// Find out if there is a preceding significant diff
			if (curDiff > 0)
			{
				prevDiff = pd->m_diffList.PrevSignificant3wayDiff(curDiff, nDiffType);
			}
		}
		if (prevDiff == -1)
			prevDiff = curDiff;

		// prevDiff is the preceding one if there is one, else it is the one we're on
		SelectDiff(prevDiff, true, false);
	}
	else
	{
		// We don't have a selected difference,
		// but cursor can be inside inactive diff
		int line = GetCursorPos().y;
		if (!IsValidTextPosY(CEPoint(0, line)))
			line = m_nTopLine;
		curDiff = pd->m_diffList.PrevSignificant3wayDiffFromLine(line, nDiffType);
		if (curDiff >= 0)
			SelectDiff(curDiff, true, false);
	}
}

/**
 * @brief Update "Previous diff X and Y" UI items
 */
void CMergeEditView::OnUpdatePrev3wayDiff(CCmdUI* pCmdUI, int nDiffType)
{
	CMergeDoc *pd = GetDocument();

	if (pd->m_nBuffers < 3)
	{
		pCmdUI->Enable(false);
		return;
	}

	const DIFFRANGE * dfi = pd->m_diffList.FirstSignificant3wayDiffRange(nDiffType);

	if (dfi == nullptr)
	{
		// There aren't any significant differences
		pCmdUI->Enable(false);
	}
	else
	{
		// Enable if the end of the first significant difference is before caret
		CEPoint pos = GetCursorPos();
		pCmdUI->Enable(pos.y > (long)dfi->dend);
	}
}

void CMergeEditView::OnNextdiffLM()
{
	OnNext3wayDiff(THREEWAYDIFFTYPE_LEFTMIDDLE);
}

void CMergeEditView::OnUpdateNextdiffLM(CCmdUI* pCmdUI)
{
	OnUpdateNext3wayDiff(pCmdUI, THREEWAYDIFFTYPE_LEFTMIDDLE);
}

void CMergeEditView::OnNextdiffLR()
{
	OnNext3wayDiff(THREEWAYDIFFTYPE_LEFTRIGHT);
}

void CMergeEditView::OnUpdateNextdiffLR(CCmdUI* pCmdUI)
{
	OnUpdateNext3wayDiff(pCmdUI, THREEWAYDIFFTYPE_LEFTRIGHT);
}

void CMergeEditView::OnNextdiffMR()
{
	OnNext3wayDiff(THREEWAYDIFFTYPE_MIDDLERIGHT);
}

void CMergeEditView::OnUpdateNextdiffMR(CCmdUI* pCmdUI)
{
	OnUpdateNext3wayDiff(pCmdUI, THREEWAYDIFFTYPE_MIDDLERIGHT);
}

void CMergeEditView::OnNextdiffLO()
{
	OnNext3wayDiff(THREEWAYDIFFTYPE_LEFTONLY);
}

void CMergeEditView::OnUpdateNextdiffLO(CCmdUI* pCmdUI)
{
	OnUpdateNext3wayDiff(pCmdUI, THREEWAYDIFFTYPE_LEFTONLY);
}

void CMergeEditView::OnNextdiffMO()
{
	OnNext3wayDiff(THREEWAYDIFFTYPE_MIDDLEONLY);
}

void CMergeEditView::OnUpdateNextdiffMO(CCmdUI* pCmdUI)
{
	OnUpdateNext3wayDiff(pCmdUI, THREEWAYDIFFTYPE_MIDDLEONLY);
}

void CMergeEditView::OnNextdiffRO()
{
	OnNext3wayDiff(THREEWAYDIFFTYPE_RIGHTONLY);
}

void CMergeEditView::OnUpdateNextdiffRO(CCmdUI* pCmdUI)
{
	OnUpdateNext3wayDiff(pCmdUI, THREEWAYDIFFTYPE_RIGHTONLY);
}

void CMergeEditView::OnPrevdiffLM()
{
	OnPrev3wayDiff(THREEWAYDIFFTYPE_LEFTMIDDLE);
}

void CMergeEditView::OnUpdatePrevdiffLM(CCmdUI* pCmdUI)
{
	OnUpdatePrev3wayDiff(pCmdUI, THREEWAYDIFFTYPE_LEFTMIDDLE);
}

void CMergeEditView::OnPrevdiffLR()
{
	OnPrev3wayDiff(THREEWAYDIFFTYPE_LEFTRIGHT);
}

void CMergeEditView::OnUpdatePrevdiffLR(CCmdUI* pCmdUI)
{
	OnUpdatePrev3wayDiff(pCmdUI, THREEWAYDIFFTYPE_LEFTRIGHT);
}

void CMergeEditView::OnPrevdiffMR()
{
	OnPrev3wayDiff(THREEWAYDIFFTYPE_MIDDLERIGHT);
}

void CMergeEditView::OnUpdatePrevdiffMR(CCmdUI* pCmdUI)
{
	OnUpdatePrev3wayDiff(pCmdUI, THREEWAYDIFFTYPE_MIDDLERIGHT);
}

void CMergeEditView::OnPrevdiffLO()
{
	OnPrev3wayDiff(THREEWAYDIFFTYPE_LEFTONLY);
}

void CMergeEditView::OnUpdatePrevdiffLO(CCmdUI* pCmdUI)
{
	OnUpdatePrev3wayDiff(pCmdUI, THREEWAYDIFFTYPE_LEFTONLY);
}

void CMergeEditView::OnPrevdiffMO()
{
	OnPrev3wayDiff(THREEWAYDIFFTYPE_MIDDLEONLY);
}

void CMergeEditView::OnUpdatePrevdiffMO(CCmdUI* pCmdUI)
{
	OnUpdatePrev3wayDiff(pCmdUI, THREEWAYDIFFTYPE_MIDDLEONLY);
}

void CMergeEditView::OnPrevdiffRO()
{
	OnPrev3wayDiff(THREEWAYDIFFTYPE_RIGHTONLY);
}

void CMergeEditView::OnUpdatePrevdiffRO(CCmdUI* pCmdUI)
{
	OnUpdatePrev3wayDiff(pCmdUI, THREEWAYDIFFTYPE_RIGHTONLY);
}

/**
 * @brief Clear selection
 */
void CMergeEditView::SelectNone()
{
	SetSelection (GetCursorPos(), GetCursorPos());
	UpdateCaret();
}

/**
 * @brief Check if line is inside currently selected diff
 * @param [in] nLine 0-based linenumber in view
 * @sa CMergeDoc::GetCurrentDiff()
 * @sa CMergeDoc::LineInDiff()
 */
bool CMergeEditView::IsLineInCurrentDiff(int nLine) const
{
	// Check validity of nLine
#ifdef _DEBUG
	if (nLine < 0)
		_RPTF1(_CRT_ERROR, "Linenumber is negative (%d)!", nLine);
	int nLineCount = LocateTextBuffer()->GetLineCount();
	if (nLine >= nLineCount)
		_RPTF2(_CRT_ERROR, "Linenumber > linecount (%d>%d)!", nLine, nLineCount);
#endif

	const CMergeDoc *pd = GetDocument();
	int curDiff = pd->GetCurrentDiff();
	if (curDiff == -1)
		return false;
	return pd->m_diffList.LineInDiff(nLine, curDiff);
}

/**
 * @brief Called when mouse left-button double-clicked
 *
 * Double-clicking mouse inside diff selects that diff
 */
void CMergeEditView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CMergeDoc *pd = GetDocument();
	CEPoint pos = GetCursorPos();

	int diff = pd->m_diffList.LineToDiff(pos.y);
	if (diff != -1 && pd->m_diffList.IsDiffSignificant(diff))
		SelectDiff(diff, false, false);

	CCrystalEditViewEx::OnLButtonDblClk(nFlags, point);
}

/**
 * @brief Called when mouse left button is released.
 *
 * If button is released outside diffs, current diff
 * is deselected.
 */
void CMergeEditView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CCrystalEditViewEx::OnLButtonUp(nFlags, point);
	DeselectDiffIfCursorNotInCurrentDiff();
}

/**
 * @brief Called when mouse right button is pressed.
 *
 * If right button is pressed outside diffs, current diff
 * is deselected.
 */
void CMergeEditView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CCrystalEditViewEx::OnRButtonDown(nFlags, point);
	DeselectDiffIfCursorNotInCurrentDiff();
}

void CMergeEditView::OnX2Y(int srcPane, int dstPane, bool selectedLineOnly)
{
	// Check that right side is not readonly
	if (IsReadOnly(dstPane))
		return;

	CMergeDoc *pDoc = GetDocument();
	int currentDiff = pDoc->GetCurrentDiff();

	if (currentDiff == -1)
	{
		// No selected diff
		// If cursor is inside diff get number of that diff
		if (m_bCurrentLineIsDiff)
		{
			CEPoint pt = GetCursorPos();
			currentDiff = pDoc->m_diffList.LineToDiff(pt.y);
		}
	}

	auto [ptStart, ptEnd] = GetSelection();
	if (IsSelection() || pDoc->EqualCurrentWordDiff(srcPane, ptStart, ptEnd))
	{
		if (!m_bRectangularSelection)
		{
			if (selectedLineOnly)
			{
				int firstDiff, lastDiff;
				GetSelectedDiffs(firstDiff, lastDiff);
				if (firstDiff != -1 && lastDiff != -1)
				{
					CWaitCursor waitstatus;
					pDoc->CopyMultiplePartialList(srcPane, dstPane, firstDiff, lastDiff, ptStart.y, ptEnd.y);
				}
			}
			else
			{
				int firstDiff, lastDiff, firstWordDiff, lastWordDiff;
				GetFullySelectedDiffs(firstDiff, lastDiff, firstWordDiff, lastWordDiff);
				if (firstDiff != -1 && lastDiff != -1)
				{
					CWaitCursor waitstatus;
					
					// Setting CopyFullLine (OPT_COPY_FULL_LINE)
					// restore old copy behaviour (always copy "full line" instead of "selected text only"), with a hidden option
					if (GetOptionsMgr()->GetBool(OPT_COPY_FULL_LINE))
					{
						// old behaviour: copy full line
						pDoc->CopyMultipleList(srcPane, dstPane, firstDiff, lastDiff);
					}
					else
					{
						// new behaviour: copy selected text only
						pDoc->CopyMultipleList(srcPane, dstPane, firstDiff, lastDiff, firstWordDiff, lastWordDiff);
					}
				}
			}
		}
		else
		{
			CWaitCursor waitstatus;
			auto wordDiffs = GetColumnSelectedWordDiffIndice();
			int i = 0;
			std::for_each(wordDiffs.rbegin(), wordDiffs.rend(), [&](auto& it) {
				pDoc->WordListCopy(srcPane, dstPane, it.first, it.second[0], it.second[it.second.size() - 1], &it.second, i != 0, i == 0);
				++i;
			});
		}
	}
	else if (currentDiff != -1 && pDoc->m_diffList.IsDiffSignificant(currentDiff))
	{
		if (selectedLineOnly)
		{
			CWaitCursor waitstatus;
			pDoc->PartialListCopy(srcPane, dstPane, currentDiff, ptStart.y, ptEnd.y);
		}
		else
		{
			CWaitCursor waitstatus;
			pDoc->ListCopy(srcPane, dstPane, currentDiff);
		}
	}
}

void CMergeEditView::OnUpdateX2Y(CCmdUI* pCmdUI)
{
	auto [srcPane, dstPane] = CMergeFrameCommon::MenuIDtoXY(pCmdUI->m_nID, m_nThisPane, GetDocument()->m_nBuffers);
	if (GetDocument()->m_nBuffers > 2)
		CMergeFrameCommon::ChangeMergeMenuText(srcPane, dstPane, pCmdUI);
	if (srcPane < 0 || dstPane < 0)
	{
		pCmdUI->Enable(false);
		return;
	}
	// Check that right side is not readonly
	if (!IsReadOnly(dstPane))
	{
		// If one or more diffs inside selection OR
		// there is an active diff OR
		// cursor is inside diff
		auto [ptStart, ptEnd] = GetSelection();
		if (IsSelection() || GetDocument()->EqualCurrentWordDiff(m_nThisPane, ptStart, ptEnd))
		{
			int firstDiff, lastDiff, firstWordDiff, lastWordDiff;
			GetFullySelectedDiffs(firstDiff, lastDiff, firstWordDiff, lastWordDiff);

			pCmdUI->Enable((firstDiff != -1 && lastDiff != -1) || (firstWordDiff != -1 && lastWordDiff != -1));
		}
		else
		{
			const int currDiff = GetDocument()->GetCurrentDiff();
			pCmdUI->Enable(m_bCurrentLineIsDiff || (currDiff != -1 && GetDocument()->m_diffList.IsDiffSignificant(currDiff)));
		}
	}
	else
		pCmdUI->Enable(false);
}

void CMergeEditView::OnCopyX2Y(UINT nID)
{
	auto [srcPane, dstPane] = CMergeFrameCommon::MenuIDtoXY(nID, m_nThisPane, GetDocument()->m_nBuffers);
	if (srcPane >= 0 && dstPane >= 0)
		OnX2Y(srcPane, dstPane);
}

void CMergeEditView::OnCopyLinesX2Y(UINT nID)
{
	auto [srcPane, dstPane] = CMergeFrameCommon::MenuIDtoXY(nID, m_nThisPane, GetDocument()->m_nBuffers);
	if (srcPane >= 0 && dstPane >= 0)
		OnX2Y(srcPane, dstPane, true);
}

/**
 * @brief Copy diff from left pane to right pane
 *
 * Difference is copied from left to right when
 * - difference is selected
 * - difference is inside selection (allows merging multiple differences).
 * - cursor is inside diff
 *
 * If there is selected diff outside selection, we copy selected
 * difference only.
 */
void CMergeEditView::OnL2r()
{
	OnCopyX2Y(ID_L2R);
}

/**
 * @brief Called when "Copy to Right" item is updated
 */
void CMergeEditView::OnUpdateL2r(CCmdUI* pCmdUI)
{
	OnUpdateX2Y(pCmdUI);
}

void CMergeEditView::OnLinesL2r()
{
	OnCopyLinesX2Y(ID_LINES_L2R);
}

void CMergeEditView::OnUpdateLinesL2r(CCmdUI* pCmdUI)
{
	OnUpdateX2Y(pCmdUI);
}

/**
 * @brief Copy diff from right pane to left pane
 *
 * Difference is copied from left to right when
 * - difference is selected
 * - difference is inside selection (allows merging multiple differences).
 * - cursor is inside diff
 *
 * If there is selected diff outside selection, we copy selected
 * difference only.
 */
void CMergeEditView::OnR2l()
{
	OnCopyX2Y(ID_R2L);
}

/**
 * @brief Called when "Copy to Left" item is updated
 */
void CMergeEditView::OnUpdateR2l(CCmdUI* pCmdUI)
{
	OnUpdateX2Y(pCmdUI);
}

void CMergeEditView::OnLinesR2l()
{
	OnCopyLinesX2Y(ID_LINES_R2L);
}

void CMergeEditView::OnUpdateLinesR2l(CCmdUI* pCmdUI)
{
	OnUpdateX2Y(pCmdUI);
}

void CMergeEditView::OnCopyFromLeft()
{
	OnCopyX2Y(ID_COPY_FROM_LEFT);
}

void CMergeEditView::OnUpdateCopyFromLeft(CCmdUI* pCmdUI)
{
	OnUpdateX2Y(pCmdUI);
}

void CMergeEditView::OnCopyLinesFromLeft()
{
	OnCopyLinesX2Y(ID_COPY_LINES_FROM_LEFT);
}

void CMergeEditView::OnUpdateCopyLinesFromLeft(CCmdUI* pCmdUI)
{
	OnUpdateX2Y(pCmdUI);
}

void CMergeEditView::OnCopyFromRight()
{
	OnCopyX2Y(ID_COPY_FROM_RIGHT);
}

void CMergeEditView::OnUpdateCopyFromRight(CCmdUI* pCmdUI)
{
	OnUpdateX2Y(pCmdUI);
}

void CMergeEditView::OnCopyLinesFromRight()
{
	OnCopyLinesX2Y(ID_COPY_LINES_FROM_RIGHT);
}

void CMergeEditView::OnUpdateCopyLinesFromRight(CCmdUI* pCmdUI)
{
	OnUpdateX2Y(pCmdUI);
}

/**
 * @brief Copy all diffs from right pane to left pane
 */
void CMergeEditView::OnAllLeft()
{
	UINT userChoice = 0;
	String msg = _("Are you sure you want to copy all differences to the other file?");
	userChoice = AfxMessageBox(msg.c_str(), MB_YESNO |
		MB_ICONWARNING | MB_DEFBUTTON2 | MB_DONT_ASK_AGAIN, IDS_CONFIRM_COPY_ALL_DIFFS);
	if (userChoice == IDNO)
		return;

	// Check that left side is not readonly
	auto [srcPane, dstPane] = CMergeFrameCommon::MenuIDtoXY(ID_ALL_LEFT, m_nThisPane, GetDocument()->m_nBuffers);
	if (IsReadOnly(dstPane))
		return;
	CWaitCursor waitstatus;

	GetDocument()->CopyAllList(srcPane, dstPane);
}

/**
 * @brief Called when "Copy all to left" item is updated
 */
void CMergeEditView::OnUpdateAllLeft(CCmdUI* pCmdUI)
{
	// Check that left side is not readonly
	[[maybe_unused]] auto [srcPane, dstPane] = CMergeFrameCommon::MenuIDtoXY(pCmdUI->m_nID, m_nThisPane, GetDocument()->m_nBuffers);
	if (!IsReadOnly(dstPane))
		pCmdUI->Enable(GetDocument()->m_diffList.HasSignificantDiffs());
	else
		pCmdUI->Enable(false);
	if (GetDocument()->m_nBuffers > 2)
		CMergeFrameCommon::ChangeMergeMenuText(m_nThisPane, dstPane, pCmdUI);
}

/**
 * @brief Copy all diffs from left pane to right pane
 */
void CMergeEditView::OnAllRight()
{
	UINT userChoice = 0;
	String msg = _("Are you sure you want to copy all differences to the other file?");
	userChoice = AfxMessageBox(msg.c_str(), MB_YESNO |
		MB_ICONWARNING | MB_DEFBUTTON2 | MB_DONT_ASK_AGAIN, IDS_CONFIRM_COPY_ALL_DIFFS);
	if (userChoice == IDNO)
		return;

	// Check that right side is not readonly
	auto [srcPane, dstPane] = CMergeFrameCommon::MenuIDtoXY(ID_ALL_RIGHT, m_nThisPane, GetDocument()->m_nBuffers);
	if (IsReadOnly(dstPane))
		return;

	CWaitCursor waitstatus;

	GetDocument()->CopyAllList(srcPane, dstPane);
}

/**
 * @brief Called when "Copy all to right" item is updated
 */
void CMergeEditView::OnUpdateAllRight(CCmdUI* pCmdUI)
{
	// Check that right side is not readonly
	[[maybe_unused]] auto [srcPane, dstPane] = CMergeFrameCommon::MenuIDtoXY(pCmdUI->m_nID, m_nThisPane, GetDocument()->m_nBuffers);
	if (!IsReadOnly(dstPane))
		pCmdUI->Enable(GetDocument()->m_diffList.HasSignificantDiffs());
	else
		pCmdUI->Enable(false);
	if (GetDocument()->m_nBuffers > 2)
		CMergeFrameCommon::ChangeMergeMenuText(m_nThisPane, dstPane, pCmdUI);
}

/**
 * @brief Do Auto merge
 */
void CMergeEditView::OnAutoMerge()
{
	// Check current pane is not readonly
	if (GetDocument()->IsModified() || GetDocument()->GetAutoMerged() || !QueryEditable())
		return;

	CWaitCursor waitstatus;

	GetDocument()->DoAutoMerge(m_nThisPane);
}

/**
 * @brief Called when "Auto Merge" item is updated
 */
void CMergeEditView::OnUpdateAutoMerge(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->m_nBuffers == 3 && 
		!GetDocument()->IsModified() && 
		!GetDocument()->GetAutoMerged() && 
		QueryEditable());
}

/**
 * @brief Add synchronization point
 */
void CMergeEditView::OnAddSyncPoint()
{
	GetDocument()->AddSyncPoint();
}

/**
 * @brief Clear synchronization points
 */
void CMergeEditView::OnClearSyncPoints()
{
	GetDocument()->ClearSyncPoints();
}

/**
 * @brief Called when "Clear Synchronization Points" item is updated
 */
void CMergeEditView::OnUpdateClearSyncPoints(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->HasSyncPoints());
}

/**
 * @brief This function is called before other edit events.
 * @param [in] nAction Edit operation to do
 * @param [in] pszText Text to insert, delete etc
 * @sa CCrystalEditView::OnEditOperation()
 * @todo More edit-events for rescan delaying?
 */
void CMergeEditView::OnEditOperation(int nAction, const tchar_t* pszText, size_t cchText)
{
	if (!QueryEditable())
	{
		// We must not arrive here, and assert helps detect troubles
		ASSERT(false);
		return;
	}

	CMergeDoc* pDoc = GetDocument();
	pDoc->SetEditedAfterRescan(m_nThisPane);

	// simple hook for multiplex undo operations
	// deleted by jtuc 2003-06-28
	// now AddUndoRecords does it (so we don't create entry for OnEditOperation with no Undo data in m_pTextBuffer)
	/*if(dynamic_cast<CMergeDoc::CDiffTextBuffer*>(m_pTextBuffer)->curUndoGroup())
	{
		pDoc->undoTgt.erase(pDoc->curUndo, pDoc->undoTgt.end());
		pDoc->undoTgt.push_back(this);
		pDoc->curUndo = pDoc->undoTgt.end();
	}*/

	// perform original function
	CCrystalEditViewEx::OnEditOperation(nAction, pszText, cchText);

	// augment with additional operations

	// Change header to inform about changed doc
	pDoc->UpdateHeaderPath(m_nThisPane);

	// If automatic rescan enabled, rescan after edit events
	if (pDoc->GetAutomaticRescan())
	{
		// keep document up to date	
		// (Re)start timer to rescan only when user edits text
		// If timer starting fails, rescan immediately
		if (nAction == CE_ACTION_TYPING ||
			nAction == CE_ACTION_REPLACE ||
			nAction == CE_ACTION_BACKSPACE ||
			nAction == CE_ACTION_INDENT ||
			nAction == CE_ACTION_PASTE ||
			nAction == CE_ACTION_DELSEL ||
			nAction == CE_ACTION_DELETE ||
			nAction == CE_ACTION_CUT)
		{
			if (!SetTimer(IDT_RESCAN, RESCAN_TIMEOUT, nullptr))
				pDoc->FlushAndRescan();
		}
		else
			pDoc->FlushAndRescan();
	}
	else
	{
		if (m_bWordWrap)
		{
			// Update other pane for sync line.
			for (int nPane = 0; nPane < pDoc->m_nBuffers; nPane++)
			{
				if (nPane == m_nThisPane)
					continue;
				CCrystalEditView *pView = GetGroupView(nPane);
				if (pView != nullptr)
					pView->Invalidate();
			}
		}
	}
}

/**
 * @brief Redo last action
 */
void CMergeEditView::OnEditRedo()
{
	CWaitCursor waitstatus;
	CMergeDoc* pDoc = GetDocument();
	CMergeEditView *tgt = pDoc->GetView(m_nThisGroup, *(pDoc->curUndo));
	if(tgt==this)
	{
		if (!QueryEditable())
			return;

		GetParentFrame()->SetActiveView(this, true);
		if(CCrystalEditViewEx::DoEditRedo())
		{
			++pDoc->curUndo;
			pDoc->UpdateHeaderPath(m_nThisPane);
			pDoc->FlushAndRescan();
		}
	}
	else
	{
		tgt->SendMessage(WM_COMMAND, ID_EDIT_REDO);
	}
}

/**
 * @brief Called when "Redo" item is updated
 */
void CMergeEditView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	CMergeDoc* pDoc = GetDocument();
	if (pDoc->curUndo!=pDoc->undoTgt.end())
	{
		CMergeEditView *tgt = pDoc->GetView(m_nThisGroup, *(pDoc->curUndo));
		pCmdUI->Enable( !IsReadOnly(tgt->m_nThisPane));
	}
	else
		pCmdUI->Enable(false);
}

void CMergeEditView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CCrystalEditViewEx::OnUpdate(pSender, lHint, pHint);
}

/**
 * @brief Scrolls to current diff and/or selects diff text
 * @param [in] bScroll If true scroll diff to view
 * @param [in] bSelectText If true select diff text
 * @note If bScroll and bSelectText are false, this does nothing!
 * @todo This shouldn't be called when no diff is selected, so
 * somebody could try to ASSERT(nDiff > -1)...
 */
void CMergeEditView::ShowDiff(bool bScroll, bool bSelectText)
{
	CMergeDoc *pd = GetDocument();
	const int nDiff = pd->GetCurrentDiff();

	// Try to trap some errors
	if (nDiff >= pd->m_diffList.GetSize())
		_RPTF2(_CRT_ERROR, "Selected diff > diffcount (%d > %d)!",
			nDiff, pd->m_diffList.GetSize());

	if (nDiff >= 0 && nDiff < pd->m_diffList.GetSize())
	{
		CEPoint ptStart, ptEnd;
		DIFFRANGE curDiff;
		pd->m_diffList.GetDiff(nDiff, curDiff);

		ptStart.x = 0;
		ptStart.y = curDiff.dbegin;
		ptEnd.x = 0;
		ptEnd.y = curDiff.dend;

		if (bScroll && !m_bDetailView)
		{
			if (!IsDiffVisible(curDiff, CONTEXT_LINES_BELOW))
			{
				// Difference is not visible, scroll it so that max amount of
				// scrolling is done while keeping the diff in screen. So if
				// scrolling is downwards, scroll the diff to as up in screen
				// as possible. This usually brings next diff to the screen
				// and we don't need to scroll into it.
				int nLine = GetSubLineIndex(ptStart.y);
				if (nLine > CONTEXT_LINES_ABOVE)
				{
					nLine -= CONTEXT_LINES_ABOVE;
				}
				GetGroupView(m_nThisPane)->ScrollToSubLine(nLine);
				for (int nPane = 0; nPane < pd->m_nBuffers; nPane++)
				{
					if (nPane != m_nThisPane)
						GetGroupView(nPane)->ScrollToSubLine(nLine);
				}
			}

			vector<WordDiff> worddiffs;
			if (GetOptionsMgr()->GetBool(OPT_SCROLL_TO_FIRST_INLINE_DIFF))
				worddiffs = pd->GetWordDiffArrayInDiffBlock(nDiff);
			CEPoint pt = worddiffs.size() > 0 ?
				CEPoint{ worddiffs[0].begin[m_nThisPane], worddiffs[0].beginline[m_nThisPane] } :
				ptStart;
			GetGroupView(m_nThisPane)->SetCursorPos(pt);
			GetGroupView(m_nThisPane)->SetAnchor(pt);
			GetGroupView(m_nThisPane)->SetSelection(pt, pt);
			GetGroupView(m_nThisPane)->EnsureVisible(pt);
			for (int nPane = 0; nPane < pd->m_nBuffers; nPane++)
			{
				if (nPane != m_nThisPane)
				{
					if (worddiffs.size() > 0)
					{
						pt.x = worddiffs[0].begin[nPane];
						pt.y = worddiffs[0].beginline[nPane];
					}
					GetGroupView(nPane)->SetCursorPos(pt);
					GetGroupView(nPane)->SetAnchor(pt);
					GetGroupView(nPane)->SetSelection(pt, pt);
				}
			}
		}

		if (bSelectText)
		{
			ptEnd.x = GetLineLength(ptEnd.y);
			SetSelection(ptStart, ptEnd);
			UpdateCaret();
		}
		else
			Invalidate();
	}
}


void CMergeEditView::OnTimer(UINT_PTR nIDEvent)
{
	// Maybe we want theApp::OnIdle to proceed before processing a timer message
	// ...but for this the queue must be empty
	// The timer message is a low priority message but the queue is maybe not yet empty
	// So we set a flag, wait for OnIdle to proceed, then come back here...
	// We come back here with a IDLE_TIMER OnTimer message (send with SendMessage
	// not with SetTimer so there is no delay)

	// IDT_RESCAN was posted because the app wanted to do a flushAndRescan with some delay

	// IDLE_TIMER is the false timer used to come back here after OnIdle
	// fTimerWaitingForIdle is a bool to store the commands waiting for idle
	// (one normal timer = one flag = one command)

	if (nIDEvent == IDT_RESCAN)
	{
		KillTimer(IDT_RESCAN);
		fTimerWaitingForIdle |= FLAG_RESCAN_WAITS_FOR_IDLE;
		// notify the app to come back after OnIdle
		theApp.SetNeedIdleTimer();
	}

	if (nIDEvent == IDLE_TIMER)
	{
		// not a real timer, just come back after OnIdle
		// look to flags to know what to do
		if (fTimerWaitingForIdle & FLAG_RESCAN_WAITS_FOR_IDLE)
			GetDocument()->RescanIfNeeded(RESCAN_TIMEOUT/1000);
		fTimerWaitingForIdle = 0;
	}

	CCrystalEditViewEx::OnTimer(nIDEvent);
}

/**
 * @brief Returns if buffer is read-only
 * @note This has no any relation to file being read-only!
 */
bool CMergeEditView::IsReadOnly(int pane) const
{
	return m_bDetailView ? true : (GetDocument()->m_ptBuf[pane]->GetReadOnly() != false);
}

/**
 * @brief Handle some keys when in merging mode
 */
bool CMergeEditView::MergeModeKeyDown(MSG* pMsg)
{
	bool bHandled = false;

	// Allow default text selection when SHIFT pressed
	if (::GetAsyncKeyState(VK_SHIFT))
		return false;

	// Allow default editor functions when CTRL pressed
	if (::GetAsyncKeyState(VK_CONTROL))
		return false;

	// If we are in merging mode (merge with cursor keys)
	// handle some keys here
	switch (pMsg->wParam)
	{
	case VK_LEFT:
		OnR2l();
		bHandled = true;
		break;

	case VK_RIGHT:
		OnL2r();
		bHandled = true;
		break;

	case VK_UP:
		PostMessage(WM_COMMAND, ID_PREVDIFF);
		bHandled = true;
		break;

	case VK_DOWN:
		PostMessage(WM_COMMAND, ID_NEXTDIFF);
		bHandled = true;
		break;
	}

	return bHandled;
}

/**
 * @brief Called before messages are translated.
 *
 * Checks if ESC key was pressed, saves and closes doc.
 * Also if in merge mode traps cursor keys.
 */
BOOL CMergeEditView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		// If we are in merging mode (merge with cursor keys)
		// handle some keys here
		if (theApp.GetMergingMode())
		{
			bool bHandled = MergeModeKeyDown(pMsg);
			if (bHandled)
				return false;
		}

		// Close window if user has allowed it from options
		if (pMsg->wParam == VK_ESCAPE)
		{
			int nCloseWithEsc = GetOptionsMgr()->GetInt(OPT_CLOSE_WITH_ESC);
			if (nCloseWithEsc != 0)
				GetParentFrame()->PostMessage(WM_CLOSE, 0, 0);
			return false;
		}
	}

	return CCrystalEditViewEx::PreTranslateMessage(pMsg);
}

/// Store interface we use to display status line info
void CMergeEditView::SetStatusInterface(IMergeEditStatus * piMergeEditStatus)
{
	ASSERT(m_piMergeEditStatus == nullptr);
	m_piMergeEditStatus = piMergeEditStatus;
}

/**
 * @brief Update status bar contents.
 */
void CMergeEditView::UpdateStatusbar()
{
	OnUpdateCaret();
}

/**
 * @brief Update statusbar info, Override from CCrystalTextView
 * @note we tab-expand column, but we don't tab-expand char count,
 * since we want to show how many chars there are and tab is just one
 * character although it expands to several spaces.
 */
void CMergeEditView::OnUpdateCaret()
{
	if (m_bCursorHidden || m_piMergeEditStatus == nullptr || !IsTextBufferInitialized())
		return;

	CEPoint cursorPos = GetCursorPos();
	int nScreenLine = cursorPos.y;
	const int nRealLine = ComputeRealLine(nScreenLine);
	CString sLine;
	int chars = -1;
	CString sEol;
	int column = -1;
	int columns = -1;
	int curChar = -1;
	auto [selectedLines, selectedChars] = GetSelectedLineAndCharacterCount();
	lineflags_t dwLineFlags = 0;

	dwLineFlags = m_pTextBuffer->GetLineFlags(nScreenLine);
	// Is this a ghost line ?
	if (dwLineFlags & LF_GHOST)
	{
		// Ghost lines display eg "Line 12-13"
		sLine.Format(_T("%d-%d"), nRealLine, nRealLine+1);
		sEol = _T("hidden");
	}
	else
	{
		// Regular lines display eg "Line 13 Characters: 25 EOL: CRLF"
		sLine.Format(_T("%d"), nRealLine+1);
		curChar = cursorPos.x + 1;
		chars = GetLineLength(nScreenLine);
		column = CalculateActualOffset(nScreenLine, cursorPos.x, true) + 1;
		columns = CalculateActualOffset(nScreenLine, chars, true) + 1;
		chars++;
		if (GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL) ||
				GetDocument()->IsMixedEOL(m_nThisPane))
		{
			sEol = GetTextBufferEol(nScreenLine);
		}
		else
			sEol = _T("hidden");
	}
	m_piMergeEditStatus->SetLineInfo(sLine, column, columns,
		curChar, chars, selectedLines, selectedChars,
		sEol, GetDocument()->m_ptBuf[m_nThisPane]->getCodepage(), GetDocument()->m_ptBuf[m_nThisPane]->getHasBom());

	// Is cursor inside difference?
	if (dwLineFlags & LF_NONTRIVIAL_DIFF)
		m_bCurrentLineIsDiff = true;
	else
		m_bCurrentLineIsDiff = false;

	CWnd* pWnd = GetFocus();
	if (!m_bDetailView || (pWnd && pWnd->m_hWnd == this->m_hWnd))
		UpdateLocationViewPosition(m_nTopSubLine, m_nTopSubLine + GetScreenLines());
}
/**
 * @brief Select linedifference in the current line.
 *
 * Select line difference in current line. Selection type
 * is choosed by highlight type.
 */
template<bool reversed>
void CMergeEditView::OnSelectLineDiff()
{
	// Pass this to the document, to compare this file to other
	GetDocument()->Showlinediff(this, reversed);
}

/// Enable select difference menuitem if current line is inside difference.
void CMergeEditView::OnUpdateSelectLineDiff(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->IsEditedAfterRescan());
}

void CMergeEditView::OnAddToSubstitutionFilters()
{
	// Pass this to the document, to compare this file to other
	GetDocument()->AddToSubstitutionFilters(this, false);
}

void CMergeEditView::OnUpdateAddToSubstitutionFilters(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->m_nBuffers == 2 && !GetDocument()->IsEditedAfterRescan());
}

void CMergeEditView::OnAddToLineFilters()
{
	// Pass this to the document, to compare this file to other
	CMergeDoc* pDoc = GetDocument();
	auto [ptSelStart, ptSelEnd] = GetSelection();

	// Nothing selected
	if (ptSelStart == ptSelEnd)
		return;

	CString text;

	if (!m_bRectangularSelection)
	{
		CDiffTextBuffer* buffer = pDoc->m_ptBuf[m_nThisPane].get();

		String sText;
		buffer->GetTextWithoutEmptys(ptSelStart.y, ptSelStart.x,
			ptSelEnd.y, ptSelEnd.x, sText);
		text.SetString(sText.c_str(), static_cast<int>(sText.length())); // TODO: Use String instead of CString
	}
	else
		GetTextWithoutEmptysInColumnSelection(text);

	CMergeDoc* pd = GetDocument();
	pd->AddToLineFilters(text.GetString());
	pd->FlushAndRescan(true);
}

void CMergeEditView::OnUpdateAddToLineFilters(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsSelection() && GetDocument()->m_nBuffers == 2 && !GetDocument()->IsEditedAfterRescan());
}

/**
 * @brief Enable/disable Replace-menuitem
 */
void CMergeEditView::OnUpdateEditReplace(CCmdUI* pCmdUI)
{
	CMergeDoc *pd = GetDocument();
	bool bReadOnly = pd->m_ptBuf[m_nThisPane]->GetReadOnly();

	pCmdUI->Enable(!bReadOnly);
}

/**
 * @brief Offer a context menu built with scriptlet/ActiveX functions
 */
void CMergeEditView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CRect rect;
	GetClientRect(rect);
	ClientToScreen(rect);

	if (GetDocument()->m_ptBuf[m_nThisPane]->GetTableEditing())
	{
		if (rect.top <= point.y && point.y < rect.top + GetTopMarginHeight())
		{
			BCMenu menu;
			VERIFY(menu.LoadMenu(IDR_POPUP_MERGEVIEWHEADER));
			theApp.TranslateMenu(menu.m_hMenu);
			BCMenu* pSub = static_cast<BCMenu*>(menu.GetSubMenu(0));
			pSub->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				point.x, point.y, AfxGetMainWnd());
			return;
		}
	}

	// Create the menu and populate it with the available functions
	BCMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_MERGEVIEW));

	// Remove copying item copying from active side
	if (m_nThisPane == 0) // left?
	{
		if (GetDocument()->m_nBuffers < 3)
		{
			menu.RemoveMenu(ID_COPY_TO_MIDDLE_L, MF_BYCOMMAND);
			menu.RemoveMenu(ID_COPY_FROM_MIDDLE_L, MF_BYCOMMAND);
			menu.RemoveMenu(ID_COPY_LINES_TO_MIDDLE_L, MF_BYCOMMAND);
			menu.RemoveMenu(ID_COPY_LINES_FROM_MIDDLE_L, MF_BYCOMMAND);
		}
		for (UINT id = ID_COPY_TO_LEFT_M; id <= ID_COPY_FROM_LEFT_R; ++id)
			menu.RemoveMenu(id, MF_BYCOMMAND);
		for (UINT id = ID_COPY_LINES_TO_LEFT_M; id <= ID_COPY_LINES_FROM_LEFT_R; ++id)
			menu.RemoveMenu(id, MF_BYCOMMAND);
	}
	if (m_nThisPane == 1 && GetDocument()->m_nBuffers == 3)
	{
		for (UINT id = ID_COPY_TO_MIDDLE_L; id <= ID_COPY_FROM_RIGHT_L; ++id)
			menu.RemoveMenu(id, MF_BYCOMMAND);
		for (UINT id = ID_COPY_TO_MIDDLE_R; id <= ID_COPY_FROM_LEFT_R; ++id)
			menu.RemoveMenu(id, MF_BYCOMMAND);
		for (UINT id = ID_COPY_LINES_TO_MIDDLE_L; id <= ID_COPY_LINES_FROM_RIGHT_L; ++id)
			menu.RemoveMenu(id, MF_BYCOMMAND);
		for (UINT id = ID_COPY_LINES_TO_MIDDLE_R; id <= ID_COPY_LINES_FROM_LEFT_R; ++id)
			menu.RemoveMenu(id, MF_BYCOMMAND);
	}
	if (m_nThisPane == GetDocument()->m_nBuffers - 1)
	{
		if (GetDocument()->m_nBuffers < 3)
		{
			menu.RemoveMenu(ID_COPY_TO_MIDDLE_R, MF_BYCOMMAND);
			menu.RemoveMenu(ID_COPY_FROM_MIDDLE_R, MF_BYCOMMAND);
			menu.RemoveMenu(ID_COPY_LINES_TO_MIDDLE_R, MF_BYCOMMAND);
			menu.RemoveMenu(ID_COPY_LINES_FROM_MIDDLE_R, MF_BYCOMMAND);
		}
		for (UINT id = ID_COPY_TO_MIDDLE_L; id <= ID_COPY_FROM_RIGHT_M; ++id)
			menu.RemoveMenu(id, MF_BYCOMMAND);
		for (UINT id = ID_COPY_LINES_TO_MIDDLE_L; id <= ID_COPY_LINES_FROM_RIGHT_M; ++id)
			menu.RemoveMenu(id, MF_BYCOMMAND);
	}

	// Remove "Go to Moved Line Between Middle and Right" if in 2-way file comparison.
	// Remove "Go to Moved Line Between Middle and Right" if the right pane is active in 3-way file comparison.
	// Remove "Go to Moved Line Between Left and Middle" if the right pane is active in 3-way file comparison.
	int nBuffers = GetDocument()->m_nBuffers;
	if (nBuffers == 2 || (nBuffers == 3 && m_nThisPane == 0))
		menu.RemoveMenu(ID_GOTO_MOVED_LINE_MR, MF_BYCOMMAND);
	else if (nBuffers == 3 && m_nThisPane == 2)
		menu.RemoveMenu(ID_GOTO_MOVED_LINE_LM, MF_BYCOMMAND);

	VERIFY(menu.LoadToolbar(IDR_MAINFRAME, GetMainFrame()->GetToolbar()));
	theApp.TranslateMenu(menu.m_hMenu);

	BCMenu *pSub = static_cast<BCMenu *>(menu.GetSubMenu(0));
	ASSERT(pSub != nullptr);

	// Context menu opened using keyboard has no coordinates
	if (point.x == -1 && point.y == -1)
	{
		point = rect.TopLeft();
		point.Offset(5, 5);
	}

	pSub->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y, AfxGetMainWnd());

}

/**
 * @brief Update EOL mode in status bar
 */
void CMergeEditView::OnUpdateStatusEOL(CCmdUI* pCmdUI)
{
	GetGroupView(pCmdUI->m_nID - ID_STATUS_PANE0FILE_EOL)->OnUpdateIndicatorCRLF(pCmdUI);
}

/**
 * @brief Change EOL mode and unify all the lines EOL to this new mode
 */
void CMergeEditView::OnConvertEolTo(UINT nID )
{
	CRLFSTYLE nStyle = CRLFSTYLE::AUTOMATIC;;
	switch (nID)
	{
		case ID_EOL_TO_DOS:
			nStyle = CRLFSTYLE::DOS;
			break;
		case ID_EOL_TO_UNIX:
			nStyle = CRLFSTYLE::UNIX;
			break;
		case ID_EOL_TO_MAC:
			nStyle = CRLFSTYLE::MAC;
			break;
		default:
			// Catch errors
			_RPTF0(_CRT_ERROR, "Unhandled EOL type conversion!");
			break;
	}
	m_pTextBuffer->SetCRLFMode(nStyle);

	// we don't need a derived applyEOLMode for ghost lines as they have no EOL char
	if (m_pTextBuffer->applyEOLMode())
	{
		CMergeDoc *pd = GetDocument();
		ASSERT(pd != nullptr);
		pd->UpdateHeaderPath(m_nThisPane);
		pd->FlushAndRescan(true);
	}
}

/**
 * @brief allow convert to entries in file submenu
 */
void CMergeEditView::OnUpdateConvertEolTo(CCmdUI* pCmdUI)
{
	CRLFSTYLE nStyle = CRLFSTYLE::AUTOMATIC;
	switch (pCmdUI->m_nID)
	{
		case ID_EOL_TO_DOS:
			nStyle = CRLFSTYLE::DOS;
			break;
		case ID_EOL_TO_UNIX:
			nStyle = CRLFSTYLE::UNIX;
			break;
		case ID_EOL_TO_MAC:
			nStyle = CRLFSTYLE::MAC;
			break;
		default:
			// Catch errors
			_RPTF0(_CRT_ERROR, "Missing menuitem handler for EOL convert menu!");
			break;
	}

	if (GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL) ||
		GetDocument()->IsMixedEOL(m_nThisPane) ||
		nStyle != m_pTextBuffer->GetCRLFMode())
	{
		pCmdUI->SetRadio(false);

		// Don't allow selecting other EOL style for protected pane
		if (!QueryEditable())
			pCmdUI->Enable(false);
	}
	else
		pCmdUI->SetRadio(true);
}

/**
 * @brief Copy diff from left to right and advance to next diff
 */
void CMergeEditView::OnL2RNext()
{
	OnL2r();
	if (GetDocument()->m_nBuffers > 2 && IsCursorInDiff()) // for 3-way file compare
		OnNextdiff();
	OnNextdiff();
}

/**
 * @brief Update "Copy right and advance" UI item
 */
void CMergeEditView::OnUpdateL2RNext(CCmdUI* pCmdUI)
{
	OnUpdateL2r(pCmdUI);
}

/**
 * @brief Copy diff from right to left and advance to next diff
 */
void CMergeEditView::OnR2LNext()
{
	OnR2l();
	if (GetDocument()->m_nBuffers > 2 && IsCursorInDiff()) // for 3-way file compare
		OnNextdiff();
	OnNextdiff();
}

/**
 * @brief Update "Copy left and advance" UI item
 */
void CMergeEditView::OnUpdateR2LNext(CCmdUI* pCmdUI)
{
	OnUpdateR2l(pCmdUI);
}

/**
 * @brief Change active pane in MergeView.
 * Changes active pane and makes sure cursor position is kept in
 * screen. Currently we put cursor in same line than in original
 * active pane but we could be smarter too? Maybe update cursor
 * only when it is not visible in new pane?
 */
void CMergeEditView::OnChangePane(UINT nID)
{
	CSplitterWnd *pSplitterWnd = GetParentSplitter(this, false);
	CMergeEditView *pWnd = static_cast<CMergeEditView*>(pSplitterWnd->GetActivePane());
	CMergeDoc *pDoc = GetDocument();
	bool bFound = false;
	CMergeEditView *pNextActiveView = nullptr;
	std::vector<CMergeEditView *> list = pDoc->GetViewList();
	list.insert(list.end(), list.begin(), list.end());
	if (nID == ID_PREV_PANE)
		std::reverse(list.begin(), list.end());

	for (auto& pView : list)
	{
		if (bFound && pView->m_bDetailView == pWnd->m_bDetailView)
		{
			pNextActiveView = pView;
			break;
		}
		if (pWnd == pView)
			bFound = true;
	}
	GetParentFrame()->SetActiveView(pNextActiveView);
	CEPoint ptCursor = pWnd->GetCursorPos();
	ptCursor.x = 0;
	if (ptCursor.y >= pNextActiveView->GetLineCount())
		ptCursor.y = pNextActiveView->GetLineCount() - 1;
	pNextActiveView->SetCursorPos(ptCursor);
	pNextActiveView->SetAnchor(ptCursor);
	pNextActiveView->SetSelection(ptCursor, ptCursor);
}

/**
 * @brief Show "Go To" dialog and scroll views to line or diff.
 *
 * Before dialog is opened, current line and file is determined
 * and selected.
 * @note Conversions needed between apparent and real lines
 */
void CMergeEditView::OnWMGoto()
{
	WMGotoDlg dlg;
	CMergeDoc *pDoc = GetDocument();
	CEPoint pos = GetCursorPos();
	int nRealLine = 0;
	int nLastLine[3] = {};

	nRealLine = pDoc->m_ptBuf[m_nThisPane]->ComputeRealLine(pos.y);
	for (int nPane = 0; nPane < pDoc->m_nBuffers; nPane++)
	{
		int nLineCount = pDoc->m_ptBuf[nPane]->GetLineCount();
		nLastLine[nPane] = pDoc->m_ptBuf[nPane]->ComputeRealLine(nLineCount - 1);
	}

	// Set active file and current line selected in dialog
	dlg.m_strParam = strutils::to_str(nRealLine + 1);
	dlg.m_nFile = (pDoc->m_nBuffers < 3) ? (m_nThisPane == 1 ? 2 : 0) : m_nThisPane;
	dlg.m_nGotoWhat = 0;
	dlg.m_nFiles = pDoc->m_nBuffers;

	if (dlg.DoModal() == IDOK)
	{
		CMergeDoc * pDoc1 = GetDocument();

		// Get views
		CMergeEditView * pCurrentView = GetGroupView(m_nThisPane);

		int num = 0;
		try { num = std::stoi(dlg.m_strParam) - 1; } catch(...) {}

		if (dlg.m_nGotoWhat == 0)
		{
			int nPane = (pDoc1->m_nBuffers < 3) ? (dlg.m_nFile == 2 ? 1 : 0) : dlg.m_nFile;
			assert(nPane >= 0 && ((pDoc1->m_nBuffers < 3 && nPane < 2) || (pDoc1->m_nBuffers == 3 && nPane < 3)));

			int nRealLine1 = num;
			if (nRealLine1 < 0)
				nRealLine1 = 0;
			if (nRealLine1 > nLastLine[nPane])
				nRealLine1 = nLastLine[nPane];

			bool bShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
			GotoLine(nRealLine1, true, nPane, !bShift);
		}
		else
		{
			int diff = num;
			if (diff < 0)
				diff = 0;
			if (diff >= pDoc1->m_diffList.GetSize())
				diff = pDoc1->m_diffList.GetSize() - 1;

			if (pCurrentView && diff >= 0)
				pCurrentView->SelectDiff(diff, true, false);
		}
	}
}

/**
* @brief Called when "Go to Moved Line Between Left and Middle" item is selected.
* Go to moved line between the left and right panes when in 2-way file comparison.
* Go to moved line between the left and middle panes when in 3-way file comparison.
*/
void CMergeEditView::OnGotoMovedLineLM()
{
	if (!GetOptionsMgr()->GetBool(OPT_CMP_MOVED_BLOCKS))
		return;

	CMergeDoc* pDoc = GetDocument();
	CEPoint pos = GetCursorPos();

	ASSERT(m_nThisPane >= 0 && m_nThisPane < 3);
	ASSERT(pDoc != nullptr);
	ASSERT(pDoc->m_nBuffers == 2 || pDoc->m_nBuffers == 3);
	ASSERT(pos.y >= 0);

	if (m_nThisPane == 0)
	{
		int line = pDoc->RightLineInMovedBlock(m_nThisPane, pos.y);
		if (line >= 0)
			GotoLine(line, false, 1);
	}
	else if (m_nThisPane == 1)
	{
		int line = pDoc->LeftLineInMovedBlock(m_nThisPane, pos.y);
		if (line >= 0)
			GotoLine(line, false, 0);
	}
}

/**
 * @brief Called when "Go to Moved Line Between Left and Middle" item is updated.
 * @param [in] pCmdUI UI component to update.
 * @note The item label is changed to "Go to Moved Line" when 2-way file comparison.
 */
void CMergeEditView::OnUpdateGotoMovedLineLM(CCmdUI* pCmdUI)
{
	CMergeDoc* pDoc = GetDocument();
	CEPoint pos = GetCursorPos();

	ASSERT(m_nThisPane >= 0 && m_nThisPane < 3);
	ASSERT(pCmdUI != nullptr);
	ASSERT(pDoc != nullptr);
	ASSERT(pDoc->m_nBuffers == 2 || pDoc->m_nBuffers == 3);
	ASSERT(pos.y >= 0);

	if (pDoc->m_nBuffers == 2)
		pCmdUI->SetText(_("Go to Moved Line\tCtrl+Shift+G").c_str());

	if (!GetOptionsMgr()->GetBool(OPT_CMP_MOVED_BLOCKS) || m_nThisPane == 2)
	{
		pCmdUI->Enable(false);
		return;
	}

	if (m_nThisPane == 0)
	{
		bool bOn = (pDoc->RightLineInMovedBlock(m_nThisPane, pos.y) >= 0);
		pCmdUI->Enable(bOn);
	}
	else if (m_nThisPane == 1)
	{
		bool bOn = (pDoc->LeftLineInMovedBlock(m_nThisPane, pos.y) >= 0);
		pCmdUI->Enable(bOn);
	}
}

/**
* @brief Called when "Go to Moved Line Between Middle and Right" item is selected.
* Go to moved line between the middle and right panes when in 3-way file comparison.
*/
void CMergeEditView::OnGotoMovedLineMR()
{
	if (!GetOptionsMgr()->GetBool(OPT_CMP_MOVED_BLOCKS))
		return;

	CMergeDoc* pDoc = GetDocument();
	CEPoint pos = GetCursorPos();

	ASSERT(m_nThisPane >= 0 && m_nThisPane < 3);
	ASSERT(pDoc != nullptr);
	ASSERT(pDoc->m_nBuffers == 2 || pDoc->m_nBuffers == 3);
	ASSERT(pos.y >= 0);

	if (m_nThisPane == 1)
	{
		int line = pDoc->RightLineInMovedBlock(m_nThisPane, pos.y);
		if (line >= 0)
			GotoLine(line, false, 2);
	}
	else if (m_nThisPane == 2)
	{
		int line = pDoc->LeftLineInMovedBlock(m_nThisPane, pos.y);
		if (line >= 0)
			GotoLine(line, false, 1);
	}
}

/**
 * @brief Called when "Go to Moved Line Between Middle and Right" item is updated.
 * @param [in] pCmdUI UI component to update.
 */
void CMergeEditView::OnUpdateGotoMovedLineMR(CCmdUI* pCmdUI)
{
	CMergeDoc* pDoc = GetDocument();
	CEPoint pos = GetCursorPos();

	ASSERT(m_nThisPane >= 0 && m_nThisPane < 3);
	ASSERT(pCmdUI != nullptr);
	ASSERT(pDoc != nullptr);
	ASSERT(pDoc->m_nBuffers == 2 || pDoc->m_nBuffers == 3);
	ASSERT(pos.y >= 0);

	if (!GetOptionsMgr()->GetBool(OPT_CMP_MOVED_BLOCKS) || pDoc->m_nBuffers == 2 || m_nThisPane == 0)
	{
		pCmdUI->Enable(false);
		return;
	}

	if (m_nThisPane == 1)
	{
		bool bOn = (pDoc->RightLineInMovedBlock(m_nThisPane, pos.y) >= 0);
		pCmdUI->Enable(bOn);
	}
	else if (m_nThisPane == 2)
	{
		bool bOn = (pDoc->LeftLineInMovedBlock(m_nThisPane, pos.y) >= 0);
		pCmdUI->Enable(bOn);
	}
}

void CMergeEditView::OnShellMenu()
{
	CFrameWnd *pFrame = GetTopLevelFrame();
	ASSERT(pFrame != nullptr);
	BOOL bAutoMenuEnableOld = pFrame->m_bAutoMenuEnable;
	pFrame->m_bAutoMenuEnable = FALSE;

	String path = GetDocument()->m_filePaths[m_nThisPane];
	auto pContextMenu = std::make_unique<CShellContextMenu>(CShellContextMenu(0x9000, 0x9FFF));
	pContextMenu->Initialize();
	pContextMenu->AddItem(paths::GetParentPath(path), paths::FindFileName(path));
	pContextMenu->RequeryShellContextMenu();
	CPoint point;
	::GetCursorPos(&point);
	HWND hWnd = GetSafeHwnd();
	BOOL nCmd = TrackPopupMenu(pContextMenu->GetHMENU(), TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x, point.y, 0, hWnd, nullptr);
	if (nCmd)
		pContextMenu->InvokeCommand(nCmd, hWnd);
	pContextMenu->ReleaseShellContextMenu();

	pFrame->m_bAutoMenuEnable = bAutoMenuEnableOld;
}

void CMergeEditView::OnUpdateShellMenu(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->m_filePaths[m_nThisPane].empty());
}

/**
 * @brief Reload options.
 */
void CMergeEditView::RefreshOptions()
{ 
	RENDERING_MODE nRenderingMode = static_cast<RENDERING_MODE>(GetOptionsMgr()->GetInt(OPT_RENDERING_MODE));
	SetRenderingMode(nRenderingMode);

	if (GetOptionsMgr()->GetInt(OPT_TAB_TYPE) == 0)
		SetInsertTabs(true);
	else
		SetInsertTabs(false);

	SetSelectionMargin(GetOptionsMgr()->GetBool(OPT_VIEW_FILEMARGIN));
	SetTopMargin(GetOptionsMgr()->GetBool(
		GetDocument()->m_ptBuf[m_nThisPane]->GetTableEditing() ? OPT_VIEW_TOPMARGIN_TABLE : OPT_VIEW_TOPMARGIN));
	SetLineUsedAsHeaders(GetOptionsMgr()->GetInt(OPT_LINE_NUMBER_USED_AS_HEADERS));

	if (!GetOptionsMgr()->GetBool(OPT_SYNTAX_HIGHLIGHT))
		SetTextType(CrystalLineParser::SRC_PLAIN);
	else if (!GetDocument()->GetChangedSchemeManually())
	{
		// The syntax highlighting scheme should only be applied if it has not been manually changed.
		String fileName = GetDocument()->m_ptBuf[m_nThisPane]->GetTempFileName();
		String sExt;
		paths::SplitFilename(fileName, nullptr, nullptr, &sExt);
		CrystalLineParser::TextDefinition* def = CrystalLineParser::GetTextType(sExt.c_str());
		if (def != nullptr)
			SetTextType(def->type);
		else
			SetTextType(CrystalLineParser::SRC_PLAIN);
		SetDisableBSAtSOL(false);
	}

	SetWordWrapping(GetOptionsMgr()->GetBool(
		GetDocument()->m_ptBuf[0]->GetTableEditing() ?  OPT_WORDWRAP_TABLE : OPT_WORDWRAP));
	SetViewLineNumbers(GetOptionsMgr()->GetBool(OPT_VIEW_LINENUMBERS));

	SetViewTabs(GetOptionsMgr()->GetBool(OPT_VIEW_WHITESPACE));
	SetViewEols(GetOptionsMgr()->GetBool(OPT_VIEW_EOL),
		GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL) ||
		GetDocument()->IsMixedEOL(m_nThisPane));

	Options::DiffColors::Load(GetOptionsMgr(), m_cachedColors);
}

void CMergeEditView::OnScripts(UINT nID)
{
	// text is CHAR if compiled without UNICODE, WCHAR with UNICODE
	CString ctext = GetSelectedText();
	String text{ ctext, static_cast<unsigned>(ctext.GetLength()) };

	EditorScriptInfo scriptInfo(
		CMainFrame::GetPluginPipelineByMenuId(nID, FileTransform::EditorScriptEventNames, ID_SCRIPT_FIRST));
	// transform the text with a script/ActiveX function, event=EDITOR_SCRIPT
	bool bChanged = false;
	scriptInfo.TransformText(text, { GetDocument()->m_filePaths[m_nThisPane] }, bChanged);
	if (bChanged)
		// now replace the text
		ReplaceSelection(text.c_str(), text.length(), 0);
}

void CMergeEditView::OnTransformWithScript() 
{
	// let the user choose a handler
	CSelectPluginDlg dlg(_T(""),
		strutils::join(GetDocument()->m_filePaths.begin(), GetDocument()->m_filePaths.end(), _T("|")),
		CSelectPluginDlg::PluginType::EditorScript, false);
	if (dlg.DoModal() != IDOK)
		return;
	EditorScriptInfo scriptInfo(dlg.GetPluginPipeline());
	CString ctext = GetSelectedText();
	String text{ ctext, static_cast<unsigned>(ctext.GetLength()) };
	bool bChanged = false;
	scriptInfo.TransformText(text, { GetDocument()->m_filePaths[m_nThisPane] }, bChanged);
	if (bChanged)
		// now replace the text
		ReplaceSelection(text.c_str(), text.length(), 0);
}

/** 
 * @brief Goto given line.
 * @param [in] nLine Destination linenumber
 * @param [in] bRealLine if true linenumber is real line, otherwise
 * it is apparent line (including deleted lines)
 * @param [in] pane Pane index of goto target pane (0 = left, 1 = right).
 * @param [in] bMoveAnchor if true the anchor is moved to nLine
 * @param [in] nChar Destination character position
 */
void CMergeEditView::GotoLine(UINT nLine, bool bRealLine, int pane, bool bMoveAnchor, int nChar)
{
 	CMergeDoc *pDoc = GetDocument();
	CSplitterWnd *pSplitterWnd = GetParentSplitter(this, false);
	CMergeEditView *pCurrentView = nullptr;
	if (pSplitterWnd != nullptr)
		pCurrentView = static_cast<CMergeEditView*>
			(pSplitterWnd->GetActivePane());

	int nRealLine = nLine;
	int nApparentLine = nLine;

	// Compute apparent (shown linenumber) line
	if (bRealLine)
	{
		if (nRealLine > pDoc->m_ptBuf[pane]->GetLineCount() - 1)
			nRealLine = pDoc->m_ptBuf[pane]->GetLineCount() - 1;

		nApparentLine = pDoc->m_ptBuf[pane]->ComputeApparentLine(nRealLine);
	}
	CEPoint ptPos;
	ptPos.x = nChar == -1 ? 0 : nChar;
	ptPos.y = nApparentLine;

	// Scroll line to center of view
	int nScrollLine = GetSubLineIndex(nApparentLine);
	nScrollLine -= GetScreenLines() / 2;
	if (nScrollLine < 0)
		nScrollLine = 0;

	for (int nPane = 0; nPane < pDoc->m_nBuffers; nPane++)
	{
		int nGroup = m_bDetailView ? 0 : m_nThisGroup;
		CMergeEditView* pView = GetDocument()->GetView(nGroup, nPane);
		pView->ScrollToSubLine(nScrollLine);
		CEPoint pt = (ptPos.y < pView->GetLineCount()) ? ptPos : CEPoint(ptPos.x, pView->GetLineCount() - 1);
		pt.x = std::clamp(static_cast<int>(pt.x), 0, pView->GetLineLength(pt.y));
		pView->SetCursorPos(pt);
		if (bMoveAnchor || nPane != pane)
			pView->SetAnchor(pt);
		pView->SetSelection(pView->GetAnchor(), pt);
		pView->EnsureVisible(pt);
	}

	// If goto target is another view - activate another view.
	// This is done for user convenience as user probably wants to
	// work with goto target file.
	if (m_bDetailView)
		GetDocument()->GetView(0, pane)->SetActivePane();
	else if (GetGroupView(pane) != pCurrentView)
		GetGroupView(pane)->SetActivePane();
}

/**
 * @brief Check for horizontal scroll. Re-route to CSplitterEx if not from
 * a scroll bar.
 */
void CMergeEditView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	if (pScrollBar == nullptr)
	{
		// Scroll did not come frome a scroll bar
		// Find the appropriate scroll bar
		// and send the message to the splitter window instead
		// The event should eventually come back here but with a valid scrollbar
		// Along the way it will be propagated to other windows that need it
		CSplitterWnd *pSplitterWnd = GetParentSplitter(this, false);
		CScrollBar* curBar = this->GetScrollBarCtrl(SB_HORZ);
		pSplitterWnd->SendMessage(WM_HSCROLL,
			MAKELONG(nSBCode, nPos), (LPARAM)curBar->m_hWnd);
		return;
	}
	CCrystalTextView::OnHScroll (nSBCode, nPos, pScrollBar);
}

/**
 * @brief When view is scrolled using scrollbars update location pane.
 */
void CMergeEditView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	if (pScrollBar == nullptr)
	{
		// Scroll did not come frome a scroll bar
		// Find the appropriate scroll bar
		// and send the message to the splitter window instead
		// The event should eventually come back here but with a valid scrollbar
		// Along the way it will be propagated to other windows that need it
		CSplitterWnd *pSplitterWnd = GetParentSplitter(this, false);
		CScrollBar* curBar = this->GetScrollBarCtrl(SB_VERT);
		pSplitterWnd->SendMessage(WM_VSCROLL,
			MAKELONG(nSBCode, nPos), (LPARAM)curBar->m_hWnd);
		return;
	}
	CCrystalTextView::OnVScroll (nSBCode, nPos, pScrollBar);

	if (nSBCode == SB_ENDSCROLL)
		return;

	// Note we cannot use nPos because of its 16-bit nature
	SCROLLINFO si{ sizeof(si) };
	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
	VERIFY (GetScrollInfo (SB_VERT, &si));

	// Get the current position of scroll	box.
	int nCurPos =	si.nPos;
	
	UpdateLocationViewPosition(nCurPos, nCurPos + GetScreenLines());
}

/**
 * @brief Copy selected lines adding linenumbers.
 */
void CMergeEditView::OnEditCopyLineNumbers()
{
	CString strText;
	CString strLine;
	CString strNumLine;

	CMergeDoc *pDoc = GetDocument();
	auto [ptStart, ptEnd] = GetSelection();

	// Get last selected line (having widest linenumber)
	int line = pDoc->m_ptBuf[m_nThisPane]->ComputeRealLine(ptEnd.y);
	size_t nNumWidth = strutils::to_str(line + 1).length();
	
	for (int i = ptStart.y; i <= ptEnd.y; i++)
	{
		if (GetLineFlags(i) & LF_GHOST || (GetEnableHideLines() && (GetLineFlags(i) & LF_INVISIBLE)))
			continue;

		// We need to convert to real linenumbers
		line = pDoc->m_ptBuf[m_nThisPane]->ComputeRealLine(i);

		// Insert spaces to align different width linenumbers (99, 100)
		strLine = GetLineText(i);
		CString sSpaces(' ', static_cast<int>(nNumWidth - strutils::to_str(line + 1).length()));
		
		strText += sSpaces;
		strNumLine.Format(_T("%d: %s"), line + 1, (const tchar_t*)strLine);
		strText += strNumLine;
 	}
	PutToClipboard(strText, strText.GetLength(), m_bRectangularSelection);
}

void CMergeEditView::OnUpdateEditCopyLinenumbers(CCmdUI* pCmdUI)
{
	CCrystalEditViewEx::OnUpdateEditCopy(pCmdUI);
}

/**
 * @brief Open active file with associated application.
 *
 * First tries to open file using shell 'Edit' action, since that
 * action open scripts etc. to editor instead of running them. If
 * edit-action is not registered, 'Open' action is used.
 */
void CMergeEditView::OnOpenFile()
{
	CMergeDoc * pDoc = GetDocument();
	ASSERT(pDoc != nullptr);

	String sFileName = pDoc->m_filePaths[m_nThisPane];
	if (sFileName.empty())
		return;
	shell::Edit(sFileName.c_str());
}

/**
 * @brief Open active file with app selection dialog
 */
void CMergeEditView::OnOpenFileWith()
{
	CMergeDoc * pDoc = GetDocument();
	ASSERT(pDoc != nullptr);

	String sFileName = pDoc->m_filePaths[m_nThisPane];
	if (sFileName.empty())
		return;
	shell::OpenWith(sFileName.c_str());
}

/**
 * @brief Open active file with external editor
 */
void CMergeEditView::OnOpenFileWithEditor()
{
	CMergeDoc * pDoc = GetDocument();
	ASSERT(pDoc != nullptr);

	String sFileName = pDoc->m_filePaths[m_nThisPane];
	if (sFileName.empty())
		return;

	int nRealLine = ComputeRealLine(GetCursorPos().y) + 1;
	CMergeApp::OpenFileToExternalEditor(sFileName, nRealLine);
}

/**
 * @brief Open parent folder of active file
 */
void CMergeEditView::OnOpenParentFolder()
{
	CMergeDoc * pDoc = GetDocument();
	ASSERT(pDoc != nullptr);

	String sFileName = pDoc->m_filePaths[m_nThisPane];
	if (sFileName.empty())
		return;

	shell::OpenParentFolder(sFileName.c_str());
}

/**
 * @brief Force repaint of the location pane.
 */
void CMergeEditView::RepaintLocationPane()
{
	// Must force recalculation due to caching of data in location pane.
	CLocationView *pLocationView = GetDocument()->GetLocationView();
	if (pLocationView != nullptr)
		pLocationView->ForceRecalculate();
}

/**
 * @brief Enables/disables linediff (different color for diffs)
 */
void CMergeEditView::OnViewLineDiffs()
{
	bool bWordDiffHighlight = GetOptionsMgr()->GetBool(OPT_WORDDIFF_HIGHLIGHT);
	GetOptionsMgr()->SaveOption(OPT_WORDDIFF_HIGHLIGHT, !bWordDiffHighlight);

	// Call CMergeDoc RefreshOptions() to refresh *both* views
	CMergeDoc *pDoc = GetDocument();
	pDoc->RefreshOptions();
	pDoc->FlushAndRescan(true);
}

void CMergeEditView::OnUpdateViewLineDiffs(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
	pCmdUI->SetCheck(GetOptionsMgr()->GetBool(OPT_WORDDIFF_HIGHLIGHT));
}

/**
 * @brief Enables/disables line number
 */
void CMergeEditView::OnViewLineNumbers()
{
	GetOptionsMgr()->SaveOption(OPT_VIEW_LINENUMBERS, !GetViewLineNumbers());

	// Call CMergeDoc RefreshOptions() to refresh *both* views
	CMergeDoc *pDoc = GetDocument();
	pDoc->RefreshOptions();
}

void CMergeEditView::OnUpdateViewLineNumbers(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
	pCmdUI->SetCheck(GetViewLineNumbers());
}

/**
 * @brief Enables/disables word wrap
 */
void CMergeEditView::OnViewWordWrap()
{
	GetOptionsMgr()->SaveOption(
		GetDocument()->m_ptBuf[0]->GetTableEditing() ?  OPT_WORDWRAP_TABLE : OPT_WORDWRAP,
		!m_bWordWrap);

	// Call CMergeDoc RefreshOptions() to refresh *both* views
	CMergeDoc *pDoc = GetDocument();
	pDoc->RefreshOptions();
	pDoc->UpdateAllViews(this);

	UpdateCaret();
}

void CMergeEditView::OnUpdateViewWordWrap(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
	pCmdUI->SetCheck(m_bWordWrap);
	pCmdUI->SetText((GetDocument()->m_ptBuf[0]->GetTableEditing() ?
		_("W&rap Text") : _("W&rap Lines")).c_str());
}

void CMergeEditView::OnViewWhitespace() 
{
	GetOptionsMgr()->SaveOption(OPT_VIEW_WHITESPACE, !GetViewTabs());

	// Call CMergeDoc RefreshOptions() to refresh *both* views
	CMergeDoc *pDoc = GetDocument();
	pDoc->RefreshOptions();
}

void CMergeEditView::OnUpdateViewWhitespace(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GetViewTabs());
}

void CMergeEditView::OnViewEOL() 
{
	GetOptionsMgr()->SaveOption(OPT_VIEW_EOL, !GetViewEols());
	GetDocument()->RefreshOptions();
}

void CMergeEditView::OnUpdateViewEOL(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GetViewEols());
}

void CMergeEditView::OnSize(UINT nType, int cx, int cy) 
{
	if (!IsInitialized())
		return;

	CMergeDoc * pDoc = GetDocument();
	if (m_nThisPane < pDoc->m_nBuffers - 1)
	{
		// To calculate subline index correctly
		// we have to invalidate line cache in all pane before calling the function related the subline.
		for (int nPane = 0; nPane < pDoc->m_nBuffers; nPane++) 
		{
			CMergeEditView *pView = GetGroupView(nPane);
			if (pView != nullptr)
				pView->InvalidateScreenRect(false);
		}
	}
	else
	{
		for (int nPane = 0; nPane < pDoc->m_nBuffers; nPane++) 
		{
			CMergeEditView *pView = GetGroupView(nPane);
			if (pView != nullptr)
				pView->Invalidate();
		}
	}
	// recalculate m_nTopSubLine
	m_nTopSubLine = GetSubLineIndex(m_nTopLine);

	UpdateCaret();
	
	RecalcVertScrollBar (false, false);
	RecalcHorzScrollBar (false, false);
}

/**
* @brief allocates GDI resources for printing
* @param pDC [in] points to the printer device context
* @param pInfo [in] points to a CPrintInfo structure that describes the current print job
*/
void CMergeEditView::OnBeginPrinting(CDC * pDC, CPrintInfo * pInfo)
{
	GetParentFrame()->PostMessage(WM_TIMER);

	for (int pane = 0; pane < GetDocument()->m_nBuffers; pane++)
	{
		CMergeEditView *pView = GetDocument()->GetView(m_nThisGroup, pane);
		pView->m_bPrintHeader = true;
		pView->m_bPrintFooter = true;
		pView->CGhostTextView::OnBeginPrinting(pDC, pInfo);
	}
}

/**
* @brief frees GDI resources for printing
* @param pDC [in] points to the printer device context
* @param pInfo [in] points to a CPrintInfo structure that describes the current print job
*/
void CMergeEditView::OnEndPrinting(CDC * pDC, CPrintInfo * pInfo)
{
	for (int pane = 0; pane < GetDocument()->m_nBuffers; pane++)
		GetDocument()->GetView(m_nThisGroup, pane)->CGhostTextView::OnEndPrinting(pDC, pInfo);

	GetParentFrame()->PostMessage(WM_TIMER);
}

/**
* @brief Gets header text to print
* @param [in]  nPageNum the page number to print
* @param [out] header text to print
*/
void CMergeEditView::GetPrintHeaderText(int nPageNum, CString & text)
{
	text = GetDocument()->GetTitle();
}

/**
* @brief Prints header
* @param [in] nPageNum the page number to print
*/
void CMergeEditView::PrintHeader(CDC * pdc, int nPageNum)
{
	if (m_nThisPane > 0)
		return;
	int oldRight = m_rcPrintArea.right;
	m_rcPrintArea.right += m_rcPrintArea.Width() * (GetDocument()->m_nBuffers - 1);
	CGhostTextView::PrintHeader(pdc, nPageNum);
	m_rcPrintArea.right = oldRight;
}

/**
* @brief Prints footer
* @param [in] nPageNum the page number to print
*/
void CMergeEditView::PrintFooter(CDC * pdc, int nPageNum)
{
	if (m_nThisPane > 0)
		return;
	int oldRight = m_rcPrintArea.right;
	m_rcPrintArea.right += m_rcPrintArea.Width() * (GetDocument()->m_nBuffers - 1);
	CGhostTextView::PrintFooter(pdc, nPageNum);
	m_rcPrintArea.right = oldRight;
}

void CMergeEditView::RecalcPageLayouts (CDC * pDC, CPrintInfo * pInfo)
{
	for (int pane = 0; pane < GetDocument()->m_nBuffers; pane++)
		GetDocument()->GetView(m_nThisGroup, pane)->CGhostTextView::RecalcPageLayouts(pDC, pInfo);
}

/**
* @brief Prints or previews both panes.
* @param pDC [in] points to the printer device context
* @param pInfo [in] points to a CPrintInfo structure that describes the current print job
*/
void CMergeEditView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	CRect rDraw = pInfo->m_rectDraw;
	CSize sz = rDraw.Size();
	CMergeDoc *pDoc = GetDocument();

	SIZE szLeftTop{}, szRightBottom{};
	GetPrintMargins(szLeftTop.cx, szLeftTop.cy, szRightBottom.cx, szRightBottom.cy);
	pDC->HIMETRICtoLP(&szLeftTop);
	pDC->HIMETRICtoLP(&szRightBottom);
	
	int midX = (sz.cx - szLeftTop.cx - szRightBottom.cx) / pDoc->m_nBuffers;

	// print pane
	for (int pane = 0; pane < pDoc->m_nBuffers; pane++)
	{
		pInfo->m_rectDraw.left = rDraw.left + midX * pane;
		pInfo->m_rectDraw.right	= pInfo->m_rectDraw.left + midX + szLeftTop.cx + szRightBottom.cx;
		CMergeEditView* pPane = pDoc->GetView(m_nThisGroup, pane);
		pPane->CGhostTextView::OnPrint(pDC, pInfo);
	}
}

bool CMergeEditView::IsInitialized() const
{
	CMergeEditView * pThis = const_cast<CMergeEditView *>(this);
	CDiffTextBuffer * pBuffer = dynamic_cast<CDiffTextBuffer *>(pThis->LocateTextBuffer());
	return pBuffer->IsInitialized();
}

/**
 * @brief returns the number of empty lines which are added for synchronizing the line in two/three panes.
 */
int CMergeEditView::GetEmptySubLines( int nLineIndex )
{
	int	nBreaks[3] = {0};
	int nMaxBreaks = -1;
	CMergeDoc * pDoc = GetDocument();
	for (int nPane = 0; nPane < pDoc->m_nBuffers; nPane++) 
	{
		CMergeEditView *pView = GetGroupView(nPane);
		if (pView != nullptr)
		{
			if (nLineIndex >= pView->GetLineCount())
				return 0;
			pView->WrapLineCached( nLineIndex, pView->GetScreenChars(), nullptr, nBreaks[nPane] );
		}
		nMaxBreaks = (std::max)(nMaxBreaks, nBreaks[nPane]);
	}

	if (nBreaks[m_nThisPane] < nMaxBreaks)
		return nMaxBreaks - nBreaks[m_nThisPane];
	else
		return 0;
}

/**
 * @brief Invalidate sub line index cache from the specified index to the end of file.
 * @param [in] nLineIndex Index of the first line to invalidate 
 */
void CMergeEditView::InvalidateSubLineIndexCache( int nLineIndex )
{
	CMergeDoc * pDoc = GetDocument();
	ASSERT(pDoc != nullptr);

    // We have to invalidate sub line index cache on both panes.
	for (int nPane = 0; nPane < pDoc->m_nBuffers; nPane++) 
	{
		CMergeEditView *pView = GetGroupView(nPane);
		if (pView != nullptr)
			pView->CCrystalTextView::InvalidateSubLineIndexCache( nLineIndex );
	}
}

void CMergeEditView::SetWordWrapping( bool bWordWrap )
{
	for (int pane = 0; pane < GetDocument()->m_nBuffers; pane++)
		GetGroupView(pane)->m_bWordWrap = bWordWrap;
	CCrystalTextView::SetWordWrapping(bWordWrap);
}

/**
* @brief Determine if difference is visible on screen.
* @param [in] nDiff Number of diff to check.
* @return true if difference is visible.
*/
bool CMergeEditView::IsDiffVisible(int nDiff)
{
	const CMergeDoc *pd = GetDocument();

	DIFFRANGE diff;
	pd->m_diffList.GetDiff(nDiff, diff);

	return IsDiffVisible(diff);
}

/**
 * @brief Determine if difference is visible on screen.
 * @param [in] diff diff to check.
 * @param [in] nLinesBelow Allow "minimizing" the number of visible lines.
 * @return true if difference is visible, false otherwise.
 */
bool CMergeEditView::IsDiffVisible(const DIFFRANGE& diff, int nLinesBelow /*=0*/)
{
	const int nDiffStart = GetSubLineIndex(diff.dbegin);
	const int nDiffEnd = GetSubLineIndex(diff.dend);
	// Diff's height is last line - first line + last line's line count
	const int nDiffHeight = nDiffEnd - nDiffStart + GetSubLines(diff.dend) + 1;

	// If diff first line outside current view - context OR
	// if diff last line outside current view - context OR
	// if diff is bigger than screen
	if ((nDiffStart < m_nTopSubLine) ||
		(nDiffEnd >= m_nTopSubLine + GetScreenLines() - nLinesBelow) ||
		(nDiffHeight >= GetScreenLines()))
	{
		return false;
	}
	else
	{
		return true;
	}
}

/** @brief Open help from mainframe when user presses F1*/
void CMergeEditView::OnHelp()
{
	theApp.ShowHelp(MergeViewHelpLocation);
}

/**
 * @brief Called after document is loaded.
 * This function is called from CMergeDoc::OpenDocs() after documents are
 * loaded. So this is good place to set View's options etc.
 */
void CMergeEditView::DocumentsLoaded()
{
	if (GetDocument()->m_ptBuf[m_nThisPane]->GetTableEditing())
	{
		SetTopMargin(GetOptionsMgr()->GetBool(OPT_VIEW_TOPMARGIN_TABLE));
		SetLineUsedAsHeaders(GetOptionsMgr()->GetInt(OPT_LINE_NUMBER_USED_AS_HEADERS));
		if (m_nThisPane == GetDocument()->m_nBuffers - 1 && !m_bDetailView)
			AutoFitColumn();
	}
	else
	{
		SetTopMargin(GetOptionsMgr()->GetBool(OPT_VIEW_TOPMARGIN));
	}

	// SetTextType will revert to language dependent defaults for tab
	SetTabSize(GetOptionsMgr()->GetInt(OPT_TAB_SIZE));
	SetViewTabs(GetOptionsMgr()->GetBool(OPT_VIEW_WHITESPACE));
	const bool mixedEOLs = GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL) ||
		GetDocument()->IsMixedEOL(m_nThisPane);
	SetViewEols(GetOptionsMgr()->GetBool(OPT_VIEW_EOL), mixedEOLs);
	SetWordWrapping(GetOptionsMgr()->GetBool(GetDocument()->m_ptBuf[m_nThisPane]->GetTableEditing() ?
		OPT_WORDWRAP_TABLE : OPT_WORDWRAP));
	SetViewLineNumbers(GetOptionsMgr()->GetBool(OPT_VIEW_LINENUMBERS));
	SetSelectionMargin(GetOptionsMgr()->GetBool(OPT_VIEW_FILEMARGIN));

	// Enable Backspace at beginning of line
	SetDisableBSAtSOL(false);

	// Set tab type (tabs/spaces)
	bool bInsertTabs = (GetOptionsMgr()->GetInt(OPT_TAB_TYPE) == 0);
	SetInsertTabs(bInsertTabs);

	// Sometimes WinMerge doesn't update scrollbars correctly (they remain
	// disabled) after docs are open in screen. So lets make sure they are
	// really updated, even though this is unnecessary in most cases.
	InvalidateHorzScrollBar();
	InvalidateVertScrollBar();
}

/**
 * @brief Update LocationView position.
 * This function updates LocationView position to given lines.
 * Usually we want to lines in file compare view and area in
 * LocationView to match. Be extra carefull to not call non-existing
 * LocationView.
 * @param [in] nTopLine Top line of current view.
 * @param [in] nBottomLine Bottom line of current view.
 */
void CMergeEditView::UpdateLocationViewPosition(int nTopLine /*=-1*/,
		int nBottomLine /*= -1*/)
{
	CMergeDoc *pDoc = GetDocument();
	if (pDoc == nullptr)
		return;

	CLocationView *pLocationView = pDoc->GetLocationView();

	if (pLocationView != nullptr && IsWindow(pLocationView->GetSafeHwnd()))
	{
		pLocationView->UpdateVisiblePos(nTopLine, nBottomLine);
	}
}

/**
 * @brief Enable/Disable view's selection margins.
 * Selection margins show bookmarks and word-wrap symbols, so they are pretty
 * useful. But it appears many users don't use/need those features and for them
 * selection margins are just wasted screen estate.
 */
void CMergeEditView::OnViewMargin()
{
	bool bViewMargin = GetOptionsMgr()->GetBool(OPT_VIEW_FILEMARGIN);
	GetOptionsMgr()->SaveOption(OPT_VIEW_FILEMARGIN, !bViewMargin);

	SetSelectionMargin(!bViewMargin);
	CMergeDoc *pDoc = GetDocument();
	pDoc->RefreshOptions();
	pDoc->UpdateAllViews(this);
}

/**
 * @brief Update GUI for Enable/Disable view's selection margin.
 * @param [in] pCmdUI Pointer to UI item to update.
 */
void CMergeEditView::OnUpdateViewMargin(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
	pCmdUI->SetCheck(GetOptionsMgr()->GetBool(OPT_VIEW_FILEMARGIN));
}

/**
 * @brief Enable/Disable view's top margins.
 */
void CMergeEditView::OnViewTopMargin()
{
	bool bTableEditing = GetDocument()->m_ptBuf[m_nThisPane]->GetTableEditing();
	bool bViewMargin = GetOptionsMgr()->GetBool(bTableEditing ? OPT_VIEW_TOPMARGIN_TABLE : OPT_VIEW_TOPMARGIN);
	GetOptionsMgr()->SaveOption(bTableEditing ? OPT_VIEW_TOPMARGIN_TABLE : OPT_VIEW_TOPMARGIN, !bViewMargin);

	SetTopMargin(!bViewMargin);
	CMergeDoc *pDoc = GetDocument();
	pDoc->RefreshOptions();
	pDoc->UpdateAllViews(this);
}

/**
 * @brief Update GUI for Enable/Disable view's top margin.
 * @param [in] pCmdUI Pointer to UI item to update.
 */
void CMergeEditView::OnUpdateViewTopMargin(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
	pCmdUI->SetCheck(GetOptionsMgr()->GetBool(
		GetDocument()->m_ptBuf[m_nThisPane]->GetTableEditing() ? OPT_VIEW_TOPMARGIN_TABLE : OPT_VIEW_TOPMARGIN));
}

void CMergeEditView::OnUseFirstLineAsHeaders()
{
	bool bUseFirstLineAsHeaders = GetOptionsMgr()->GetInt(OPT_LINE_NUMBER_USED_AS_HEADERS) != -1;
	GetOptionsMgr()->SaveOption(OPT_LINE_NUMBER_USED_AS_HEADERS, !bUseFirstLineAsHeaders ? 0 : -1);

	SetLineUsedAsHeaders(!bUseFirstLineAsHeaders ? 0 : -1);
	CMergeDoc *pDoc = GetDocument();
	pDoc->RefreshOptions();
	pDoc->UpdateAllViews(this);
}

void CMergeEditView::OnUpdateUseFirstLineAsHeaders(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
	pCmdUI->SetCheck(GetOptionsMgr()->GetInt(OPT_LINE_NUMBER_USED_AS_HEADERS) != -1);
}

void CMergeEditView::OnAutoFitAllColumns()
{
	AutoFitColumn();
}

/**
* @brief Create the "Change Scheme" sub menu.
* @param [in] pCmdUI Pointer to UI item to update.
*/
void CMergeEditView::OnUpdateViewChangeScheme(CCmdUI *pCmdUI)
{
	// Delete the place holder menu.
	pCmdUI->m_pSubMenu->DeleteMenu(0, MF_BYPOSITION);

	const HMENU hSubMenu = pCmdUI->m_pSubMenu->m_hMenu;

	String name = theApp.LoadString(ID_COLORSCHEME_FIRST);
	AppendMenu(hSubMenu, MF_STRING, ID_COLORSCHEME_FIRST, name.c_str());

	for (int i = ID_COLORSCHEME_FIRST + 1, j = 0; i <= ID_COLORSCHEME_LAST; ++i, ++j)
	{
		name = theApp.LoadString(i);
		AppendMenu(hSubMenu, MF_STRING | (((j % 22) == 21) ? MF_MENUBREAK : 0), i, name.c_str());
	}

	pCmdUI->Enable(true);
}

/**
* @brief Change the editor's syntax highlighting scheme.
* @param [in] nID Selected color scheme sub menu id.
*/
void CMergeEditView::OnChangeScheme(UINT nID)
{
	CMergeDoc *pDoc = GetDocument();
	ASSERT(pDoc != nullptr);
	pDoc->SetTextType(nID - ID_COLORSCHEME_FIRST);
	pDoc->FlushAndRescan(true);
}

/**
* @brief Enable all color schemes sub menu items.
* @param [in] pCmdUI Pointer to UI item to update.
*/
void CMergeEditView::OnUpdateChangeScheme(CCmdUI* pCmdUI)
{
	const bool bIsCurrentScheme = (static_cast<UINT>(m_CurSourceDef->type) == (pCmdUI->m_nID - ID_COLORSCHEME_FIRST));
	pCmdUI->SetRadio(bIsCurrentScheme);
	pCmdUI->Enable(GetOptionsMgr()->GetBool(OPT_SYNTAX_HIGHLIGHT));
}

/**
 * @brief Called when mouse's wheel is scrolled.
 */
BOOL CMergeEditView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if ( nFlags == MK_CONTROL )
	{
		short amount = zDelta < 0 ? -1: 1;
		ZoomText(amount);

		// no default CCrystalTextView
		return CView::OnMouseWheel(nFlags, zDelta, pt);
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
void CMergeEditView::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
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

/**
 * @brief Change font size (zoom) in views.
 * @param [in] amount Amount of change/zoom, negative number makes
 *  font smaller, positive number bigger and 0 reset the font size.
 */
void CMergeEditView::ZoomText(short amount)
{
	LOGFONT lf = { 0 };
	GetFont(lf);

	const int nLogPixelsY = CClientDC(this).GetDeviceCaps(LOGPIXELSY);
	int nPointSize = -MulDiv(lf.lfHeight, 72, nLogPixelsY);

	int nOrgPointSize = GetOptionsMgr()->GetInt(OPT_FONT_FILECMP + OPT_FONT_POINTSIZE);
	if (nOrgPointSize ==  0)
		nOrgPointSize = -MulDiv(GetOptionsMgr()->GetInt(OPT_FONT_FILECMP + OPT_FONT_HEIGHT), 72, nLogPixelsY);

	if ( amount == 0)
		nPointSize = nOrgPointSize;

	nPointSize += amount;
	if (nPointSize < 2)
		nPointSize = 2;

	lf.lfHeight = -MulDiv(nPointSize, nLogPixelsY, 72);

	CMergeDoc *pDoc = GetDocument();
	ASSERT(pDoc != nullptr);

	if (pDoc != nullptr)
	{
		for (int nPane = 0; nPane < pDoc->m_nBuffers; nPane++) 
		{
			CMergeEditView *pView = GetGroupView(nPane);
			ASSERT(pView != nullptr);
			
			if (pView != nullptr)
			{
				pView->SetFont(lf);
			}
		}
	}

	GetOptionsMgr()->SaveOption(OPT_VIEW_ZOOM, nPointSize * 1000 / nOrgPointSize);
}

bool CMergeEditView::QueryEditable()
{
	return m_bDetailView ? false : !GetDocument()->m_ptBuf[m_nThisPane]->GetReadOnly();
}

/**
 * @brief Adjust the point to remain in the displayed diff
 *
 * @return Tells if the point has been changed
 */
bool CMergeEditView::EnsureInDiff(CEPoint& pt)
{
	int nLineCount = GetLineCount();
	if (m_lineBegin >= nLineCount)
		m_lineBegin = nLineCount - 1;
	if (m_lineEnd >= nLineCount)
		m_lineEnd = nLineCount - 1;

	int diffLength = m_lineEnd - m_lineBegin + 1;
	// first get the degenerate case out of the way
	// no diff ?
	if (diffLength == 0)
	{
		if (pt.y == m_lineBegin && pt.x == 0)
			return false;
		pt.y = m_lineBegin;
		pt.x = 0;
		return true;
	}

	// not above diff
	if (pt.y < m_lineBegin)
	{
		pt.y = m_lineBegin;
		pt.x = 0;
		return true;
	}
	// diff is defined and not below diff
	if (m_lineEnd > -1 && pt.y > m_lineEnd)
	{
		pt.y = m_lineEnd;
		pt.x = GetLineLength(pt.y);
		return true;
	}
	return false;
}

void CMergeEditView::EnsureVisible(CEPoint pt)
{
	CEPoint ptNew = pt;
	if (m_bDetailView)
	{
		// ensure we remain in diff
		if (EnsureInDiff(ptNew))
			SetCursorPos(ptNew);
	}
	CCrystalTextView::EnsureVisible(ptNew);
}

void CMergeEditView::EnsureVisible(CEPoint ptStart, CEPoint ptEnd)
{
	CCrystalTextView::EnsureVisible(ptStart, ptEnd);
}

void CMergeEditView::SetSelection(const CEPoint& ptStart, const CEPoint& ptEnd, bool bUpdateView)
{
	CEPoint ptStartNew = ptStart;
	CEPoint ptEndNew = ptEnd;
	if (m_bDetailView)
	{
		// ensure we remain in diff
		EnsureInDiff(ptStartNew);
		EnsureInDiff(ptEndNew);
	}
	CCrystalTextView::SetSelection(ptStartNew, ptEndNew, bUpdateView);
}

void CMergeEditView::ScrollToSubLine(int nNewTopLine, bool bNoSmoothScroll /*= FALSE*/, bool bTrackScrollBar /*= TRUE*/)
{
	if (m_bDetailView)
	{
		int nLineCount = GetLineCount();
		if (m_lineBegin >= nLineCount)
			m_lineBegin = nLineCount - 1;
		if (m_lineEnd >= nLineCount)
			m_lineEnd = nLineCount - 1;

		// ensure we remain in diff
		int sublineBegin = GetSubLineIndex(m_lineBegin);
		int sublineEnd = m_lineEnd < 0 ? -1 : GetSubLineIndex(m_lineEnd) + GetSubLines(m_lineEnd) - 1;
		int diffLength = sublineEnd - sublineBegin + 1;
		int displayLength = GetScreenLines();
		if (diffLength <= displayLength)
			nNewTopLine = sublineBegin;
		else
		{
			if (nNewTopLine < sublineBegin)
				nNewTopLine = sublineBegin;
			if (nNewTopLine + displayLength - 1 > sublineEnd)
				nNewTopLine = GetSubLineIndex(sublineEnd - displayLength + 1);
		}

		CEPoint pt = GetCursorPos();
		if (EnsureInDiff(pt))
			SetCursorPos(pt);

		auto [ptSelStart, ptSelEnd] = GetSelection();
		if (EnsureInDiff(ptSelStart) || EnsureInDiff(ptSelEnd))
			SetSelection(ptSelStart, ptSelEnd);
	}
	CCrystalTextView::ScrollToSubLine(nNewTopLine, bNoSmoothScroll, bTrackScrollBar);
}

void CMergeEditView::SetActivePane()
{
	auto* pwndSplitterChild = GetParentSplitter(this, false);
	if (!pwndSplitterChild)
		return;
	if (pwndSplitterChild->GetColumnCount() > 1)
		pwndSplitterChild->SetActivePane(0, m_nThisPane);
	else
		pwndSplitterChild->SetActivePane(m_nThisPane, 0);
}

/**
 * @brief Called when user selects View/Zoom In from menu.
 */
void CMergeEditView::OnViewZoomIn()
{
	ZoomText(1);
}

/**
 * @brief Called when user selects View/Zoom Out from menu.
 */
void CMergeEditView::OnViewZoomOut()
{
	ZoomText(-1);
}

/**
 * @brief Called when user selects View/Zoom Normal from menu.
 */
void CMergeEditView::OnViewZoomNormal()
{
	ZoomText(0);
}

void CMergeEditView::OnDropFiles(const std::vector<String>& tFiles)
{
	if (tFiles.size() > 1 || paths::IsDirectory(tFiles[0]))
	{
		GetMainFrame()->GetDropHandler()->GetCallback()(tFiles);
		return;
	}

	GetDocument()->ChangeFile(m_nThisPane, tFiles[0]);
}

void CMergeEditView::OnWindowSplit()
{

	auto& wndSplitter = dynamic_cast<CMergeEditFrame *>(GetParentFrame())->GetSplitter();
	CMergeDoc *pDoc = GetDocument();
	int nBuffer = m_nThisPane;
	if (pDoc->m_nGroups <= 2)
	{
		wndSplitter.SplitRow(1);
		wndSplitter.EqualizeRows();
	}
	else
	{
		wndSplitter.SetActivePane(0, 0);
		wndSplitter.DeleteRow(1);
		pDoc->GetView(0, nBuffer)->SetActivePane();
	}
}

void CMergeEditView::OnUpdateWindowSplit(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_bDetailView);
	pCmdUI->SetCheck(GetDocument()->m_nGroups > 2);
}

void CMergeEditView::OnStatusBarClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	const int pane = pNMItemActivate->iItem / 4;
	CMergeDoc* pDoc = GetDocument();
	if (pane >= pDoc->m_nBuffers || !GetParentFrame()->IsChild(CWnd::FromHandle(pNMItemActivate->hdr.hwndFrom)))
		return;

	switch (pNMItemActivate->iItem % 4)
	{
	case 0:
		pDoc->GetView(0, pane)->PostMessage(WM_COMMAND, ID_EDIT_WMGOTO);
		break;
	case 1:
		pDoc->DoFileEncodingDialog(pane);
		break;
	case 2:
	{
		CPoint point;
		::GetCursorPos(&point);

		BCMenu menu;
		VERIFY(menu.LoadMenu(IDR_POPUP_MERGEEDITFRAME_STATUSBAR_EOL));
		theApp.TranslateMenu(menu.m_hMenu);
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, GetDocument()->GetView(0, pane));
		break;
	}
	case 3:
		pDoc->m_ptBuf[pane]->SetReadOnly(!GetDocument()->m_ptBuf[pane]->GetReadOnly());
		break;
	default:
		break;
	}
}
