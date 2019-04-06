/** 
 * @file  MergeDocLineDiffs.cpp
 *
 * @brief Implementation file for word diff highlighting (F4) for merge edit & detail views
 *
 */

#include "StdAfx.h"
#include "MergeDoc.h"
#include <vector>
#include <memory>
#include "MergeEditView.h"
#include "DiffTextBuffer.h"
#include "stringdiffs.h"
#include "UnicodeString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
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
void CMergeDoc::Showlinediff(CMergeEditView *pView, bool bReversed)
{
	CRect rc[3];
	int nBuffer;

	Computelinediff(pView, rc, bReversed);

	if (std::all_of(rc, rc + m_nBuffers, [](auto& rc) { return rc.top == -1; }))
	{
		String caption = _("Line difference");
		String msg = _("No difference");
		MessageBox(pView->GetSafeHwnd(), msg.c_str(), caption.c_str(), MB_OK);
		return;
	}

	// Actually display selection areas on screen in both edit panels
	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		HighlightDiffRect(m_pView[pView->m_nThisGroup][nBuffer], rc[nBuffer]);
}


/**
 * @brief Returns rectangles to highlight in both views (to show differences in line specified)
 */
void CMergeDoc::Computelinediff(CMergeEditView *pView, CRect rc[], bool bReversed)
{
	int file;
	for (file = 0; file < m_nBuffers; file++)
		rc[file].top = -1;

	CPoint ptStart, ptEnd;
	pView->GetSelection(ptStart, ptEnd);

	vector<WordDiff> worddiffs = GetWordDiffArray(ptStart.y);

	if (worddiffs.empty())
		return;

	int nActivePane = pView->m_nThisPane;

	std::vector<WordDiff>::iterator it;
	for (it = worddiffs.begin(); it != worddiffs.end(); ++it)
	{
		if ((*it).beginline[nActivePane] <= ptStart.y && ptStart.y <= (*it).endline[nActivePane])
		{
			int begin = ((*it).beginline[nActivePane] < ptStart.y) ? 0 : (*it).begin[nActivePane];
			int end   = ((*it).endline[nActivePane]   > ptStart.y) ? m_ptBuf[nActivePane]->GetLineLength(ptStart.y) : (*it).end[nActivePane];
			if (begin <= ptStart.x && ptStart.x <= end)
				break;
		}
	}

	if (!bReversed)
	{
		if (it != worddiffs.end())
			++it;
	
		if (it == worddiffs.end())
			it = worddiffs.begin();
	}
	else
	{
		if (it == worddiffs.begin() || it == worddiffs.end())
			it = worddiffs.end() - 1;
		else
			--it;
	}

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

std::vector<WordDiff> CMergeDoc::GetWordDiffArrayInDiffBlock(int nDiff)
{
	DIFFRANGE cd;
	const int LineLimit = 20;
	m_diffList.GetDiff(nDiff, cd);

	bool diffPerLine = (cd.dend - cd.dbegin > LineLimit) ? true : false;
	if (!diffPerLine)
		return GetWordDiffArray(cd.dbegin);

	std::vector<WordDiff> worddiffs;
	for (int nLine = cd.dbegin; nLine <= cd.dend; ++nLine)
	{
		std::vector<WordDiff> worddiffsPerLine = GetWordDiffArray(nLine);
		worddiffs.insert(worddiffs.end(), worddiffsPerLine.begin(), worddiffsPerLine.end());
	}
	return worddiffs;
}

/**
 * @brief Return array of differences in specified line
 * This is used by algorithm for line diff coloring
 * (Line diff coloring is distinct from the selection highlight code)
 */
std::vector<WordDiff> CMergeDoc::GetWordDiffArray(int nLineIndex)
{
	int file;
	DIFFRANGE cd;
	std::vector<WordDiff> worddiffs;

	for (file = 0; file < m_nBuffers; file++)
	{
		if (nLineIndex >= m_ptBuf[file]->GetLineCount())
			return worddiffs;
	}

	int nDiff = m_diffList.LineToDiff(nLineIndex);
	if (nDiff == -1)
		return worddiffs;
	std::map<int, std::vector<WordDiff> >::iterator itmap = m_cacheWordDiffs.find(nDiff);
	if (itmap != m_cacheWordDiffs.end())
	{
		worddiffs.resize((*itmap).second.size());
		std::copy((*itmap).second.begin(), (*itmap).second.end(), worddiffs.begin());
		return worddiffs;
	}

	m_diffList.GetDiff(nDiff, cd);

	DIFFOPTIONS diffOptions = {0};
	m_diffWrapper.GetOptions(&diffOptions);
	String str[3];
	std::unique_ptr<int[]> nOffsets[3];
	const int LineLimit = 20;
	bool diffPerLine = (cd.dend - cd.dbegin > LineLimit) ? true : false;

	int nLineBegin, nLineEnd;
	if (!diffPerLine)
	{
		nLineBegin = cd.dbegin;
		nLineEnd = cd.dend;
	}
	else
	{
		nLineBegin = nLineEnd = nLineIndex;
	}

	for (file = 0; file < m_nBuffers; file++)
	{
		if (nLineEnd >= m_ptBuf[file]->GetLineCount())
			return worddiffs;
		nOffsets[file].reset(new int[nLineEnd - nLineBegin + 1]);
		CString strText;
		if (nLineBegin != nLineEnd || m_ptBuf[file]->GetLineLength(nLineEnd) > 0)
			m_ptBuf[file]->GetTextWithoutEmptys(nLineBegin, 0, nLineEnd, m_ptBuf[file]->GetLineLength(nLineEnd), strText);
		strText += m_ptBuf[file]->GetLineEol(nLineEnd);
		str[file].assign(strText, strText.GetLength());

		nOffsets[file][0] = 0;
		for (int nLine = nLineBegin; nLine < nLineEnd; nLine++)
			nOffsets[file][nLine-nLineBegin+1] = nOffsets[file][nLine-nLineBegin] + m_ptBuf[file]->GetFullLineLength(nLine);
	}

	// Options that affect comparison
	bool casitive = !diffOptions.bIgnoreCase;
	int xwhite = diffOptions.nIgnoreWhitespace;
	int breakType = GetBreakType(); // whitespace only or include punctuation
	bool byteColoring = GetByteColoringOption();

	// Make the call to stringdiffs, which does all the hard & tedious computations
	std::vector<strdiff::wdiff> wdiffs = strdiff::ComputeWordDiffs(m_nBuffers, str, casitive, xwhite, breakType, byteColoring);

	int i;
	std::vector<strdiff::wdiff>::iterator it;
	for (i = 0, it = wdiffs.begin(); it != wdiffs.end(); ++i, ++it)
	{
		WordDiff wd;
		for (file = 0; file < m_nBuffers; file++)
		{
			int nLine;
			for (nLine = nLineBegin; nLine < nLineEnd; nLine++)
			{
				if (it->begin[file] == nOffsets[file][nLine-nLineBegin] || it->begin[file] < nOffsets[file][nLine-nLineBegin+1])
					break;
			}
			wd.beginline[file] = nLine;
			wd.begin[file] = it->begin[file] - nOffsets[file][nLine-nLineBegin];
			if (m_ptBuf[file]->GetLineLength(nLine) < wd.begin[file])
				wd.begin[file] = m_ptBuf[file]->GetLineLength(nLine);

			for (; nLine < nLineEnd; nLine++)
			{
				if (it->end[file] + 1 == nOffsets[file][nLine-nLineBegin] || it->end[file] + 1 < nOffsets[file][nLine-nLineBegin+1])
					break;
			}
			wd.endline[file] = nLine;
			wd.end[file] = it->end[file]  + 1 - nOffsets[file][nLine-nLineBegin];
			if (m_ptBuf[file]->GetLineLength(nLine) < wd.end[file])
				wd.end[file] = m_ptBuf[file]->GetLineLength(nLine);
		}
		wd.op = it->op;

		worddiffs.push_back(wd);
	}

	if (!diffPerLine)
	{
		m_cacheWordDiffs[nDiff].resize(worddiffs.size());
		std::copy(worddiffs.begin(), worddiffs.end(), m_cacheWordDiffs[nDiff].begin());
	}

	return worddiffs;
}

