/** 
 * @file  MergeDocLineDiffs.cpp
 *
 * @brief Implementation file for word diff highlighting (F4) for merge edit & detail views
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "StdAfx.h"
#include <vector>
#include "Merge.h"
#include "MainFrm.h"

#include "MergeEditView.h"
#include "MergeDiffDetailView.h"
#include "stringdiffs.h"
#include "UnicodeString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using std::vector;

/**
 * @brief Display the line/word difference highlight in edit view
 */
static void
HighlightDiffRect(CMergeEditView * pView, const CRect & rc)
{
	if (rc.top == -1)
	{
		// Should we remove existing selection ?
	}
	else
	{
		// select the area
		// with anchor at left and caret at right
		// this seems to be conventional behavior in Windows editors
		pView->SelectArea(rc.TopLeft(), rc.BottomRight());
		pView->SetCursorPos(rc.BottomRight());
		pView->SetNewAnchor(rc.TopLeft());
		// try to ensure that selected area is visible
		pView->EnsureVisible(rc.TopLeft(), rc.BottomRight());
	}
}

/**
 * @brief Display the line/word difference highlight in detail view
 *
 * Not combined with HighlightDiffRect(CMergeEditView *.. because
 * CMergeEditView & CMergeDiffDetailView do not inherit from a common base
 * which implements SelectArea etc.
 */
static void
HighlightDiffRect(CMergeDiffDetailView * pView, const CRect & rc)
{
	if (rc.top == -1)
	{
		// Should we remove existing selection ?
	}
	else
	{
		// select the area
		// with anchor at left and caret at right
		// this seems to be conventional behavior in Windows editors
		pView->SelectArea(rc.TopLeft(), rc.BottomRight());
		pView->SetCursorPos(rc.BottomRight());
		pView->SetNewAnchor(rc.TopLeft());
		// try to ensure that selected area is visible
		pView->EnsureVisible(rc.TopLeft());
	}
}

/**
 * @brief Highlight difference in current line (left & right panes)
 */
void CMergeDoc::Showlinediff(CMergeEditView * pView, DIFFLEVEL difflvl)
{
	CRect rc1, rc2;
	Computelinediff(m_pView[0], m_pView[1], pView->GetCursorPos().y, &rc1, &rc2, difflvl);

	if (rc1.top == -1 && rc2.top == -1)
	{
		String caption = theApp.LoadString(IDS_LINEDIFF_NODIFF_CAPTION);
		String msg = theApp.LoadString(IDS_LINEDIFF_NODIFF);
		MessageBox(pView->GetSafeHwnd(), msg.c_str(), caption.c_str(), MB_OK);
		return;
	}

	// Actually display selection areas on screen in both edit panels
	HighlightDiffRect(m_pView[0], rc1);
	HighlightDiffRect(m_pView[1], rc2);
}

/**
 * @brief Highlight difference in diff bar's current line (top & bottom panes)
 */
void CMergeDoc::Showlinediff(CMergeDiffDetailView * pView, DIFFLEVEL difflvl)
{
	CRect rc1, rc2;
	Computelinediff(m_pDetailView[0], m_pDetailView[1], pView->GetCursorPos().y, &rc1, &rc2, difflvl);

	if (rc1.top == -1 && rc2.top == -1)
	{
		String caption = theApp.LoadString(IDS_LINEDIFF_NODIFF_CAPTION);
		String msg = theApp.LoadString(IDS_LINEDIFF_NODIFF);
		MessageBox(pView->GetSafeHwnd(), msg.c_str(), caption.c_str(), MB_OK);
		return;
	}

	// Actually display selection areas on screen in both detail panels
	HighlightDiffRect(m_pDetailView[0], rc1);
	HighlightDiffRect(m_pDetailView[1], rc2);
}

/**
 * @brief Ensure that i1 is no greater than w1
 */
static void
Limit(int & i1, int w1)
{
	if (i1>=w1)
		i1 = w1;
}

/**
 * @brief Set highlight rectangle for a given difference (begin->end in line)
 */
static void
SetLineHighlightRect(int begin, int end, int line, int width, CRect * rc)
{
	if (begin == -1)
	{
		begin = end = 0;
	}
	else
	{
		++end; // MergeDoc needs to point past end
	}
	// Chop off any reference to eol characters
	// TODO: Are we dropping the last non-eol character,
	// because MergeDoc points past the end ?
	Limit(begin, width);
	Limit(end, width);
	CPoint ptBegin(begin,line), ptEnd(end,line);
	*rc = CRect(ptBegin, ptEnd);
}

/**
 * @brief Construct the highlight rectangles for diff # whichdiff
 */
static void
ComputeHighlightRects(const vector<wdiff*> & worddiffs, int whichdiff, int line, int width1, int width2, CRect * rc1, CRect * rc2)
{
	ASSERT(whichdiff >= 0 && whichdiff < worddiffs.size());
	const wdiff * diff = worddiffs[whichdiff];

	int begin1 = diff->start[0];
	int end1 = diff->end[0];
	int begin2 = diff->start[1];
	int end2 = diff->end[1];

	SetLineHighlightRect(begin1, end1, line, width1, rc1);
	SetLineHighlightRect(begin2, end2, line, width2, rc2);
	
}

/**
 * @brief Returns rectangles to highlight in both views (to show differences in line specified)
 */
void CMergeDoc::Computelinediff(CCrystalTextView * pView1, CCrystalTextView * pView2, int line, CRect * rc1, CRect * rc2, DIFFLEVEL difflvl)
{
	// Local statics are used so we can cycle through diffs in one line
	// We store previous state, both to find next state, and to verify
	// that nothing has changed (else, we reset the cycle)
	static CCrystalTextView * lastView = 0;
	static int lastLine = -1;
	static CRect lastRc1, lastRc2;
	static int whichdiff=-2; // last diff highlighted (-2==none, -1=whole line)
	int i1 = 0, i2 = 0;

	// Only remember place in cycle if same line and same view
	if (lastView != pView1 || lastLine != line)
	{
		lastView = pView1;
		lastLine = line;
		whichdiff = -2; // reset to not in cycle
	}

	DIFFOPTIONS diffOptions = {0};
	m_diffWrapper.GetOptions(&diffOptions);
	i1 = m_pView[0]->GetLineLength(line);
	i2 = m_pView[1]->GetLineLength(line);
	String str1(m_pView[0]->GetLineChars(line), i1);
	String str2(m_pView[1]->GetLineChars(line), i2);

	// We truncate diffs to remain inside line (ie, to not flag eol characters)
	int width1 = i1;
	int width2 = i2;

	// Options that affect comparison
	bool casitive = !diffOptions.bIgnoreCase;
	int xwhite = diffOptions.nIgnoreWhitespace;

	// Make the call to stringdiffs, which does all the hard & tedious computations
	vector<wdiff*> worddiffs;
	bool breakType = GetBreakType();
	sd_ComputeWordDiffs(str1, str2, casitive, xwhite, breakType, difflvl == BYTEDIFF, &worddiffs);
	//Add a diff in case of EOL difference
	if (!diffOptions.bIgnoreEol)
	{
		if (pView1->GetTextBufferEol(line) != pView1->GetTextBufferEol(line))
		{
			wdiff *wdf = new wdiff(i1, i1, i2, i2);
			worddiffs.push_back(wdf);
		}
	}
	if (worddiffs.empty())
	{
		// signal to caller that there was no diff
		rc1->top = -1;
		rc2->top = -1;
		return;
	}

	// Are we continuing a cycle from the same place ?
	if (whichdiff >= (int)worddiffs.size())
		whichdiff = -2; // Clearly not continuing the same cycle, reset to not in cycle
	
	// After last diff, reset to get full line again
	if (whichdiff == worddiffs.size() - 1)
		whichdiff = -2;

	// Check if current line has changed enough to reset cycle
	if (whichdiff >= 0)
	{
		// Recompute previous highlight rectangle
		CRect rc1x, rc2x;
		ComputeHighlightRects(worddiffs, whichdiff, line, width1, width2, &rc1x, &rc2x);
		if (rc1x != lastRc1 || rc2x != lastRc2)
		{
			// Something has changed, reset cycle
			whichdiff = -2;
		}
	}

	int begin1=-1, end1=-1, begin2=-1, end2=-1;

	if (whichdiff == -2)
	{
		// Find starting locations for both sides
		// Have to look for first valid starting location for each side
		vector<wdiff*>::const_iterator it = worddiffs.begin();
		while (it != worddiffs.end())
		{
			//const wdiff * diff = *it;
			if (begin1 == -1 && (*it)->start[0] != -1)
				begin1 = (*it)->start[0];
			if (begin2 == -1 && (*it)->start[1] != -1)
				begin2 = (*it)->start[1];
			if (begin1 != -1 && begin2 != -1)
				break; // found both
			it++;
		}
		// Find ending locations for both sides
		// Have to look for last valid starting location for each side
		if (worddiffs.size() > 1)
		{
			vector<wdiff*>::const_iterator it = worddiffs.end();
			do
			{
				it--;
				if (end1 == -1 && (*it)->end[0] != -1)
					end1 = (*it)->end[0];
				if (end2 == -1 && (*it)->end[1] != -1)
					end2 = (*it)->end[1];
				if (end1 != -1 && end2 != -1)
					break; // found both
			} while (it != worddiffs.begin());
		}
		SetLineHighlightRect(begin1, end1, line, width1, rc1);
		SetLineHighlightRect(begin2, end2, line, width2, rc2);
		whichdiff = -1;
	}
	else
	{
		// Advance to next diff (and earlier we checked for running off the end)
		++whichdiff;
		ASSERT(whichdiff < worddiffs.size());

		// highlight one particular diff
		ComputeHighlightRects(worddiffs, whichdiff, line, width1, width2, rc1, rc2);
		lastRc1 = rc1;
		lastRc2 = rc2;
	}

	while (!worddiffs.empty())
	{
		delete worddiffs.back();
		worddiffs.pop_back();
	}
}

/**
 * @brief Return array of differences in specified line
 * This is used by algorithm for line diff coloring
 * (Line diff coloring is distinct from the selection highlight code)
 */
void CMergeDoc::GetWordDiffArray(int nLineIndex, vector<wdiff*> *pworddiffs)
{
	int i1 = 0, i2 = 0;
	if (nLineIndex >= m_pView[0]->GetLineCount()) return;
	if (nLineIndex >= m_pView[1]->GetLineCount()) return;

	DIFFOPTIONS diffOptions = {0};
	m_diffWrapper.GetOptions(&diffOptions);
	i1 = m_pView[0]->GetLineLength(nLineIndex);
	i2 = m_pView[1]->GetLineLength(nLineIndex);
	String str1(m_pView[0]->GetLineChars(nLineIndex), i1);
	String str2(m_pView[1]->GetLineChars(nLineIndex), i2);

	// Options that affect comparison
	bool casitive = !diffOptions.bIgnoreCase;
	int xwhite = diffOptions.nIgnoreWhitespace;
	int breakType = GetBreakType(); // whitespace only or include punctuation
	bool byteColoring = GetByteColoringOption();

	// Make the call to stringdiffs, which does all the hard & tedious computations
	sd_ComputeWordDiffs(str1, str2, casitive, xwhite, breakType, byteColoring, pworddiffs);
	//Add a diff in case of EOL difference
	if (!diffOptions.bIgnoreEol)
	{
		if (m_pView[0]->GetTextBufferEol(nLineIndex) != m_pView[1]->GetTextBufferEol(nLineIndex))
		{
			wdiff *wdf = new wdiff(i1, i1, i2, i2);
			pworddiffs->push_back(wdf);
		}
	}
	return;
}

