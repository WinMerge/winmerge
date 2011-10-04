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
 * @brief Highlight difference in current line (left & right panes)
 */
void CMergeDoc::Showlinediff(CMergeEditView *pView)
{
	CRect rc[3];
	int pane;

	Computelinediff(pView, rc);

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
 * @brief Returns rectangles to highlight in both views (to show differences in line specified)
 */
void CMergeDoc::Computelinediff(CMergeEditView *pView, CRect rc[])
{
	int file;
	for (file = 0; file < m_nBuffers; file++)
		rc[file].top = -1;

	CPoint ptStart, ptEnd;
	pView->GetSelection(ptStart, ptEnd);

	vector<WordDiff> worddiffs;
	int nDiff = m_diffList.LineToDiff(ptStart.y);

	GetWordDiffArray(nDiff, &worddiffs);

	if (worddiffs.empty())
		return;

	int nActivePane = pView->m_nThisPane;

	std::vector<WordDiff>::iterator it;
	for (it = worddiffs.begin(); it != worddiffs.end(); it++)
	{
		if ((*it).beginline[nActivePane] <= ptStart.y && ptStart.y <= (*it).endline[nActivePane])
		{
			int begin = ((*it).beginline[nActivePane] < ptStart.y) ? 0 : (*it).begin[nActivePane];
			int end   = ((*it).endline[nActivePane]   > ptStart.y) ? m_ptBuf[nActivePane]->GetLineLength(ptStart.y) : (*it).end[nActivePane];
			if (begin <= ptStart.x && ptStart.x <= end)
				break;
		}
	}

	if (it != worddiffs.end())
		it++;
	
	if (it == worddiffs.end())
		it = worddiffs.begin();

	for (file = 0; file < m_nBuffers; file++)
	{
		rc[file].left = (*it).begin[file];
		rc[file].top = (*it).beginline[file];
		rc[file].right =(*it).end[file];
		rc[file].bottom = (*it).endline[file];
	}
}

void CMergeDoc::ClearWordDiffCache(int nDiff/* = -1 */)
{
	if (nDiff == -1)
	{
		m_cacheWordDiffs.clear();
	}
	else
	{
		std::map<int, std::vector<WordDiff> >::iterator it = m_cacheWordDiffs.find(nDiff);
		if (it != m_cacheWordDiffs.end())
			m_cacheWordDiffs.erase(it);
	}
}

/**
 * @brief Return array of differences in specified line
 * This is used by algorithm for line diff coloring
 * (Line diff coloring is distinct from the selection highlight code)
 */
void CMergeDoc::GetWordDiffArray(int nDiff, vector<WordDiff> *pWordDiffs)
{
	int file;
	DIFFRANGE cd;
	std::map<int, std::vector<WordDiff> >::iterator itmap = m_cacheWordDiffs.find(nDiff);
	if (itmap != m_cacheWordDiffs.end())
	{
		pWordDiffs->resize((*itmap).second.size());
		std::copy((*itmap).second.begin(), (*itmap).second.end(), pWordDiffs->begin());
		return;
	}

	if (nDiff != -1)
		m_diffList.GetDiff(nDiff, cd);

	DIFFOPTIONS diffOptions = {0};
	m_diffWrapper.GetOptions(&diffOptions);
	String str[3];
	int *nOffsets[3];
	for (file = 0; file < m_nBuffers; file++)
	{
		nOffsets[file] = new int[cd.dend[file] - cd.dbegin[file] + 1];
		CString strText;
		int nLineBegin = cd.dbegin[file];
		int nLineEnd = cd.dend[file];
		if (nLineBegin != nLineEnd || m_ptBuf[file]->GetLineLength(nLineEnd) > 0)
			m_ptBuf[file]->GetTextWithoutEmptys(nLineBegin, 0, nLineEnd, m_ptBuf[file]->GetLineLength(nLineEnd), strText);
		strText += m_ptBuf[file]->GetLineEol(nLineEnd);
		str[file] = strText;

		nOffsets[file][0] = 0;
		for (int nLine = nLineBegin; nLine < nLineEnd; nLine++)
			nOffsets[file][nLine-nLineBegin+1] = nOffsets[file][nLine-nLineBegin] + m_ptBuf[file]->GetFullLineLength(nLine);
	}

	// Options that affect comparison
	bool casitive = !diffOptions.bIgnoreCase;
	int xwhite = diffOptions.nIgnoreWhitespace;
	int breakType = GetBreakType(); // whitespace only or include punctuation
	bool byteColoring = GetByteColoringOption();

	std::vector<wdiff> worddiffs;
	// Make the call to stringdiffs, which does all the hard & tedious computations
	sd_ComputeWordDiffs(m_nBuffers, str, casitive, xwhite, breakType, byteColoring, &worddiffs);

	pWordDiffs->resize(worddiffs.size());

	int i;
	std::vector<wdiff>::iterator it;
	for (i = 0, it = worddiffs.begin(); it != worddiffs.end(); i++, it++)
	{
		for (file = 0; file < m_nBuffers; file++)
		{
			int nLine;
			int nLineBegin = cd.dbegin[file];
			int nLineEnd = cd.dend[file];
			int begin, end;
			for (nLine = nLineBegin; nLine < nLineEnd; nLine++)
			{
				if (it->begin[file] == nOffsets[file][nLine-nLineBegin] || it->begin[file] < nOffsets[file][nLine-nLineBegin+1])
					break;
			}
			(*pWordDiffs)[i].beginline[file] = nLine;
			(*pWordDiffs)[i].begin[file] = begin = it->begin[file] - nOffsets[file][nLine-nLineBegin];
			if (m_ptBuf[file]->GetLineLength(nLine) < (*pWordDiffs)[i].begin[file])
				(*pWordDiffs)[i].begin[file] = m_ptBuf[file]->GetLineLength(nLine);
			for (nLine = nLineBegin; nLine < nLineEnd; nLine++)
			{
				if (it->end[file] + 1 == nOffsets[file][nLine-nLineBegin] || it->end[file] + 1 < nOffsets[file][nLine-nLineBegin+1])
					break;
			}
			(*pWordDiffs)[i].endline[file] = nLine;
			(*pWordDiffs)[i].end[file] = end = it->end[file]  + 1 - nOffsets[file][nLine-nLineBegin];
			if (m_ptBuf[file]->GetLineLength(nLine) < (*pWordDiffs)[i].end[file])
				(*pWordDiffs)[i].end[file] = m_ptBuf[file]->GetLineLength(nLine);
		}
	}

	delete nOffsets[0], nOffsets[1], nOffsets[2];

	m_cacheWordDiffs[nDiff].resize(pWordDiffs->size());
	std::copy(pWordDiffs->begin(), pWordDiffs->end(), m_cacheWordDiffs[nDiff].begin());

	return;
}

