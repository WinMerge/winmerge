/** 
 * @file  MergeDocLineDiffs.cpp
 *
 * @brief Implementation file for word diff highlighting (F4) for merge edit & detail views
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "Merge.h"
#include "MainFrm.h"

#include "MergeEditView.h"
#include "MergeDiffDetailView.h"
#include "stringdiffs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


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
		pView->SelectArea(rc.TopLeft(), rc.BottomRight());
		pView->SetCursorPos(rc.TopLeft());
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
		pView->SelectArea(rc.TopLeft(), rc.BottomRight());
		pView->SetCursorPos(rc.TopLeft());
		pView->EnsureVisible(rc.TopLeft());
	}
}

/**
 * @brief Highlight difference in current line (left & right panes)
 */
void CMergeDoc::Showlinediff(CMergeEditView * pView, DIFFLEVEL difflvl)
{
	CRect rc1, rc2;
	Computelinediff(m_pLeftView, m_pRightView, pView->GetCursorPos().y, &rc1, &rc2, difflvl);

	if (rc1.top == -1 && rc2.top == -1)
	{
		CString caption;
		CString msg;
		VERIFY(caption.LoadString(IDS_LINEDIFF_NODIFF_CAPTION));
		VERIFY(msg.LoadString(IDS_LINEDIFF_NODIFF));
		MessageBox(pView->GetSafeHwnd(), msg, caption, MB_OK);
		return;
	}

	// Actually display selection areas on screen in both edit panels
	HighlightDiffRect(m_pLeftView, rc1);
	HighlightDiffRect(m_pRightView, rc2);
}

/**
 * @brief Highlight difference in diff bar's current line (top & bottom panes)
 */
void CMergeDoc::Showlinediff(CMergeDiffDetailView * pView, DIFFLEVEL difflvl)
{
	CRect rc1, rc2;
	Computelinediff(m_pLeftDetailView, m_pRightDetailView, pView->GetCursorPos().y, &rc1, &rc2, difflvl);

	if (rc1.top == -1 && rc2.top == -1)
	{
		CString caption;
		CString msg;
		VERIFY(caption.LoadString(IDS_LINEDIFF_NODIFF_CAPTION));
		VERIFY(msg.LoadString(IDS_LINEDIFF_NODIFF));
		MessageBox(pView->GetSafeHwnd(), msg, caption, MB_OK);
		return;
	}

	// Actually display selection areas on screen in both detail panels
	HighlightDiffRect(m_pLeftDetailView, rc1);
	HighlightDiffRect(m_pRightDetailView, rc2);
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
ComputeHighlightRects(const wdiffarray & worddiffs, int whichdiff, int line, int width1, int width2, CRect * rc1, CRect * rc2)
{
	ASSERT(whichdiff >= 0 && whichdiff < worddiffs.GetSize());
	const wdiff & diff = worddiffs[whichdiff];

	int begin1 = diff.start[0];
	int end1 = diff.end[0];
	int begin2 = diff.start[1];
	int end2 = diff.end[1];

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

	// Only remember place in cycle if same line and same view (and not doing bytelevel)
	if (lastView != pView1 || lastLine != line || difflvl==BYTEDIFF)
	{
		lastView = pView1;
		lastLine = line;
		whichdiff = -2; // reset to not in cycle
	}

	DIFFOPTIONS diffOptions = {0};
	m_diffWrapper.GetOptions(&diffOptions);

	CString str1 = pView1->GetLineChars(line);
	CString str2 = pView2->GetLineChars(line);

	if (!diffOptions.bEolSensitive)
	{
		/* Commented out code because GetLineActualLength is buggy
		// Chop of eol (end of line) characters
		int len1 = pView1->GetLineActualLength(line);
		str1 = str1.Left(len1);
		int len2 = pView2->GetLineActualLength(line);
		str2 = str2.Left(len2);
		*/
		int i = str1.GetLength()-1;
		while (i>=0 && (str1[i]=='\r' || str1[i]=='\n'))
			--i;
		if (i+1 < str1.GetLength())
			str1 = str1.Left(i+1);
		i = str2.GetLength()-1;
		while (i>=0 && (str2[i]=='\r' || str2[i]=='\n'))
			--i;
		if (i+1 < str2.GetLength())
			str2 = str2.Left(i+1);
	}

	// We truncate diffs to remain inside line (ie, to not flag eol characters)
	int width1 = pView1->GetLineLength(line);
	int width2 = pView2->GetLineLength(line);

	// Options that affect comparison
	bool casitive = !diffOptions.bIgnoreCase;
	int xwhite = diffOptions.nIgnoreWhitespace;

	if (difflvl==BYTEDIFF)
	{
		int begin1=-1, end1=-1, begin2=-1, end2=-1;
		sd_ComputeByteDiff(str1, str2, casitive, xwhite, begin1, begin2, end1, end2);
		SetLineHighlightRect(begin1, end1, line, width1, rc1);
		SetLineHighlightRect(begin2, end2, line, width2, rc2);
		return;
	}

	// Make the call to stringdiffs, which does all the hard & tedious computations
	wdiffarray worddiffs;
	bool breakType = GetBreakType();
	stringdiffs_Get(str1, str2, casitive, xwhite, breakType, &worddiffs);

	if (!worddiffs.GetSize())
	{
		// signal to caller that there was no diff
		rc1->top = -1;
		rc2->top = -1;
		return;
	}

	// Are we continuing a cycle from the same place ?
	if (whichdiff >= worddiffs.GetSize())
		whichdiff = -2; // Clearly not continuing the same cycle, reset to not in cycle
	
	// After last diff, reset to get full line again
	if (whichdiff == worddiffs.GetUpperBound())
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
		int i;
		for (i=0; i<worddiffs.GetSize(); ++i)
		{
			const wdiff & diff = worddiffs[i];
			if (begin1 == -1 && diff.start[0] != -1)
				begin1 = diff.start[0];
			if (begin2 == -1 && diff.start[1] != -1)
				begin2 = diff.start[1];
			if (begin1 != -1 && begin2 != -1)
				break; // found both
		}
		// Find ending locations for both sides
		// Have to look for last valid starting location for each side
		for (i=worddiffs.GetUpperBound(); i>=0; --i)
		{
			const wdiff & diff = worddiffs[i];
			if (end1 == -1 && diff.end[0] != -1)
				end1 = diff.end[0];
			if (end2 == -1 && diff.end[1] != -1)
				end2 = diff.end[1];
			if (end1 != -1 && end2 != -1)
				break; // found both
		}
		SetLineHighlightRect(begin1, end1, line, width1, rc1);
		SetLineHighlightRect(begin2, end2, line, width2, rc2);
		whichdiff = -1;
	}
	else
	{
		// Advance to next diff (and earlier we checked for running off the end)
		++whichdiff;
		ASSERT(whichdiff < worddiffs.GetSize());

		// highlight one particular diff
		ComputeHighlightRects(worddiffs, whichdiff, line, width1, width2, rc1, rc2);
		lastRc1 = rc1;
		lastRc2 = rc2;
	}
}

void CMergeDoc::GetWordDiffArray(int nLineIndex, wdiffarray *pworddiffs)
{
	if (nLineIndex >= m_pLeftView->GetLineCount()) return;
	if (nLineIndex >= m_pRightView->GetLineCount()) return;

	DIFFOPTIONS diffOptions = {0};
	m_diffWrapper.GetOptions(&diffOptions);

	CString str1 = m_pLeftView->GetLineChars(nLineIndex);
	CString str2 = m_pRightView->GetLineChars(nLineIndex);

	if (!diffOptions.bEolSensitive)
	{
		/* Commented out code because GetLineActualLength is buggy
		// Chop of eol (end of line) characters
		int len1 = pView1->GetLineActualLength(line);
		str1 = str1.Left(len1);
		int len2 = pView2->GetLineActualLength(line);
		str2 = str2.Left(len2);
		*/
		int i = str1.GetLength()-1;
		while (i>=0 && (str1[i]=='\r' || str1[i]=='\n'))
			--i;
		if (i+1 < str1.GetLength())
			str1 = str1.Left(i+1);
		i = str2.GetLength()-1;
		while (i>=0 && (str2[i]=='\r' || str2[i]=='\n'))
			--i;
		if (i+1 < str2.GetLength())
			str2 = str2.Left(i+1);
	}

	// We truncate diffs to remain inside line (ie, to not flag eol characters)
	int width1 = m_pLeftView->GetLineLength(nLineIndex);
	int width2 = m_pRightView->GetLineLength(nLineIndex);

	// Options that affect comparison
	bool casitive = !diffOptions.bIgnoreCase;
	int xwhite = diffOptions.nIgnoreWhitespace;
	int breakType = GetBreakType();

	// Make the call to stringdiffs, which does all the hard & tedious computations
	stringdiffs_Get(str1, str2, casitive, xwhite, breakType, pworddiffs);

	return;
}

