/** 
 * @file  MergeDocLineDiffs.cpp
 *
 * @brief Implementation file for word diff highlighting (F4) for merge edit & detail views
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id: MergeDocLineDiffs.cpp 7067 2009-12-29 14:22:46Z kimmov $

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
	CRect rc[3];
	int pane;
	CCrystalTextView *pCrystalTextView[3] = {m_pView[0], m_pView[1], m_pView[2]};

	Computelinediff(pCrystalTextView, pView->GetCursorPos().y, rc, difflvl);

	IF_IS_TRUE_ALL ((rc[pane].top == -1), pane, m_nBuffers)
	{
		String caption = theApp.LoadString(IDS_LINEDIFF_NODIFF_CAPTION);
		String msg = theApp.LoadString(IDS_LINEDIFF_NODIFF);
		MessageBox(pView->GetSafeHwnd(), msg.c_str(), caption.c_str(), MB_OK);
		return;
	}

	// Actually display selection areas on screen in both edit panels
	for (pane = 0; pane < m_nBuffers; pane++)
		HighlightDiffRect(m_pView[pane], rc[pane]);
}

/**
 * @brief Highlight difference in diff bar's current line (top & bottom panes)
 */
void CMergeDoc::Showlinediff(CMergeDiffDetailView * pView, DIFFLEVEL difflvl)
{
	CRect rc[3];
	int pane;
	CCrystalTextView *pCrystalTextView[3] = {m_pView[0], m_pView[1], m_pView[2]};

	Computelinediff(pCrystalTextView, pView->GetCursorPos().y, rc, difflvl);

	IF_IS_TRUE_ALL ((rc[pane].top == -1), pane, m_nBuffers)
	{
		String caption = theApp.LoadString(IDS_LINEDIFF_NODIFF_CAPTION);
		String msg = theApp.LoadString(IDS_LINEDIFF_NODIFF);
		MessageBox(pView->GetSafeHwnd(), msg.c_str(), caption.c_str(), MB_OK);
		return;
	}

	// Actually display selection areas on screen in both detail panels
	for (pane = 0; pane < m_nBuffers; pane++)
		HighlightDiffRect(m_pDetailView[pane], rc[pane]);
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
ComputeHighlightRects(int nFiles, const vector<wdiff*> & worddiffs, int whichdiff, int line, int width[3], CRect rc[3])
{
	ASSERT(whichdiff >= 0 && whichdiff < worddiffs.size());
	const wdiff * diff = worddiffs[whichdiff];

	for (int file = 0; file < nFiles; file++)
		SetLineHighlightRect(diff->begin[file], diff->end[file], line, width[file], &rc[file]);
	
}

/**
 * @brief Returns rectangles to highlight in both views (to show differences in line specified)
 */
void CMergeDoc::Computelinediff(CCrystalTextView * pView[], int line, CRect rc[], DIFFLEVEL difflvl)
{
	// Local statics are used so we can cycle through diffs in one line
	// We store previous state, both to find next state, and to verify
	// that nothing has changed (else, we reset the cycle)
	static CCrystalTextView * lastView = 0;
	static int lastLine = -1;
	static CRect lastRc[3];
	static int whichdiff=-2; // last diff highlighted (-2==none, -1=whole line)
	int i[3] = {0};
	int file;

	// Only remember place in cycle if same line and same view
	if (lastView != pView[0] || lastLine != line)
	{
		lastView = pView[0];
		lastLine = line;
		whichdiff = -2; // reset to not in cycle
	}

	DIFFOPTIONS diffOptions = {0};
	m_diffWrapper.GetOptions(&diffOptions);

	String str[3];	
	for (file = 0; file < m_nBuffers; file++)
	{
		i[file] = m_pView[file]->GetLineLength(line);
		str[file] = String(m_pView[file]->GetLineChars(line), i[file]);
	}

	// We truncate diffs to remain inside line (ie, to not flag eol characters)
	int width[3];
	for (file = 0; file < m_nBuffers; file++)
		width[file] = pView[file]->GetLineLength(line);

	// Options that affect comparison
	bool casitive = !diffOptions.bIgnoreCase;
	int xwhite = diffOptions.nIgnoreWhitespace;

	// Make the call to stringdiffs, which does all the hard & tedious computations
	vector<wdiff*> worddiffs;
	bool breakType = GetBreakType();
	sd_ComputeWordDiffs(m_nBuffers, str, casitive, xwhite, breakType, difflvl == BYTEDIFF, &worddiffs);
	//Add a diff in case of EOL difference
	if (!diffOptions.bIgnoreEol)
	{
		if (pView[0]->GetTextBufferEol(line) != pView[0]->GetTextBufferEol(line))
		{
			wdiff *wdf = new wdiff(i[0], i[0], i[1], i[1]);
			worddiffs.push_back(wdf);
		}
	}
	if (worddiffs.empty())
	{
		// signal to caller that there was no diff
		for (file = 0; file < m_nBuffers; file++)
			rc[file].top = -1;
		
		return;
	}

	// Are we continuing a cycle from the same place ?
	if (whichdiff >= (int)worddiffs.size())
		whichdiff = -2; // Clearly not continuing the same cycle, reset to not in cycle
	
	// After last diff, reset to get full line again
	if (whichdiff == (int)worddiffs.size() - 1)
		whichdiff = -2;

	// Check if current line has changed enough to reset cycle
	if (whichdiff >= 0)
	{
		// Recompute previous highlight rectangle
		CRect rcx[3];
		ComputeHighlightRects(m_nBuffers, worddiffs, whichdiff, line, width, rcx);
		IF_IS_TRUE_ALL (rcx[file] == lastRc[file], file, m_nBuffers) {}
		else
		{
			// Something has changed, reset cycle
			whichdiff = -2;
		}
	}

	int begin[3] = {-1, -1, -1}, end[3] = {-1, -1, -1};

	if (whichdiff == -2)
	{
		// Find starting locations for both sides
		// Have to look for first valid starting location for each side
		vector<wdiff*>::const_iterator it = worddiffs.begin();
		while (it != worddiffs.end())
		{
			//const wdiff * diff = *it;
			for (file = 0; file < m_nBuffers; file++)
			{
				if (begin[file] == -1 && (*it)->begin[file] != -1)
					begin[file] = (*it)->begin[file];
			}
			IF_IS_TRUE_ALL (begin[file] != -1, file, m_nBuffers)
			{
				break; // found both
			}
			it++;
		}
		// Find ending locations for both sides
		// Have to look for last valid starting location for each side
		if (worddiffs.size() >= 1)
		{
			vector<wdiff*>::const_iterator it = worddiffs.end();
			do
			{
				it--;
				for (file = 0; file < m_nBuffers; file++)
				{
					if (end[file] == -1 && (*it)->end[file] != -1)
						end[file] = (*it)->end[file];
				}
				IF_IS_TRUE_ALL (end[file] != -1, file, m_nBuffers)
				{
					break; // found both
				}
			} while (it != worddiffs.begin());
		}
		for (file = 0; file < m_nBuffers; file++)
			SetLineHighlightRect(begin[file], end[file], line, width[file], &rc[file]);
		whichdiff = -1;
	}
	else
	{
		// Advance to next diff (and earlier we checked for running off the end)
		++whichdiff;
		ASSERT(whichdiff < worddiffs.size());

		// highlight one particular diff
		ComputeHighlightRects(m_nBuffers, worddiffs, whichdiff, line, width, rc);
		for (file = 0; file < m_nBuffers; file++)
			lastRc[file] = rc[file];
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
	int icrlf[3] = {0};
	int iend[3] = {0};
	int file;
	for (file = 0; file < m_nBuffers; file++)
	{
		if (nLineIndex >= m_pView[file]->GetLineCount()) return;
	}

	DIFFOPTIONS diffOptions = {0};
	m_diffWrapper.GetOptions(&diffOptions);
	String str[3];
	for (file = 0; file < m_nBuffers; file++)
	{
		iend[file] = str[file].length() - 1;
		icrlf[file] = m_pView[file]->GetLineLength(nLineIndex);
		str[file] = String(m_pView[file]->GetLineChars(nLineIndex), icrlf[file]);
	}
	// Options that affect comparison
	bool casitive = !diffOptions.bIgnoreCase;
	int xwhite = diffOptions.nIgnoreWhitespace;
	int breakType = GetBreakType(); // whitespace only or include punctuation
	bool byteColoring = GetByteColoringOption();

	// Make the call to stringdiffs, which does all the hard & tedious computations
	sd_ComputeWordDiffs(m_nBuffers, str, casitive, xwhite, breakType, byteColoring, pworddiffs);
	//Add a diff in case of EOL difference
	if (!diffOptions.bIgnoreEol)
	{
		if (m_pView[0]->GetTextBufferEol(nLineIndex) != m_pView[1]->GetTextBufferEol(nLineIndex) || 
		    (m_nBuffers == 3 && m_pView[1]->GetTextBufferEol(nLineIndex) != m_pView[2]->GetTextBufferEol(nLineIndex)))
		{
			wdiff *wdf = new wdiff(icrlf[0], iend[0], icrlf[1], iend[1], icrlf[2], iend[2]);
			pworddiffs->push_back(wdf);
		}
	}
	return;
}

