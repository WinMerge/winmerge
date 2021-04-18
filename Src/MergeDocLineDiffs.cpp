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
#include "SubstitutionFiltersList.h"
#include "Merge.h"

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

	Computelinediff(pView, rc, bReversed);

	if (std::all_of(rc, rc + m_nBuffers, [](auto& rc) { return rc.top == -1; }))
	{
		String caption = _("Line difference");
		String msg = _("No differences to select found");
		MessageBox(pView->GetSafeHwnd(), msg.c_str(), caption.c_str(), MB_OK);
		return;
	}

	// Actually display selection areas on screen in both edit panels
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		HighlightDiffRect(m_pView[pView->m_nThisGroup][nBuffer], rc[nBuffer]);
}

void CMergeDoc::AddToSubstitutionFilters(CMergeEditView* pView, bool bReversed)
{
	if (m_nBuffers != 2)
		return; /// Not clear what to do for a 3-way merge

	CRect rc[3];

	Computelinediff(pView, rc, bReversed);

	if (std::all_of(rc, rc + m_nBuffers, [](auto& rc) { return rc.top == -1; }))
	{
		String caption = _("Line difference");
		String msg = _("No differences found to add as substitution filter");
		MessageBox(pView->GetSafeHwnd(), msg.c_str(), caption.c_str(), MB_OK);
		return;
	}

	// Actually display selection areas on screen in both edit panels
	String selectedText[3];
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		HighlightDiffRect(m_pView[pView->m_nThisGroup][nBuffer], rc[nBuffer]);
		selectedText[nBuffer] = String(m_pView[pView->m_nThisGroup][nBuffer]->GetSelectedText());
	}

	if (selectedText[0].empty())
	{
		return;
	}


	/// Check whether the pair is already registered with Substitution Filters
	SubstitutionFiltersList &SubstitutionFiltersList = *theApp.m_pSubstitutionFiltersList.get();
	for (int f = 0; f < SubstitutionFiltersList.GetCount(); f++)
	{
		String str0 = SubstitutionFiltersList.GetAt(f).pattern;
		String str1 = SubstitutionFiltersList.GetAt(f).replacement;
		if ( str0 == selectedText[0] && str1 == selectedText[1])
		{
			String caption = _("The pair is already present in the list of Substitution Filters");
			String msg = strutils::format(_T("\"%s\" <-> \"%s\""), selectedText[0], selectedText[1]);
			MessageBox(pView->GetSafeHwnd(), msg.c_str(), caption.c_str(), MB_OK);
			return; /// The substitution pair is already registered
		}
	}

	String caption = _("Add this change to Substitution Filters?");
	String msg = strutils::format(_T("\"%s\" <-> \"%s\""), selectedText[0], selectedText[1]);
	if (MessageBox(pView->GetSafeHwnd(), msg.c_str(), caption.c_str(), MB_YESNO) == IDYES)
	{
		SubstitutionFiltersList.Add(selectedText[0], selectedText[1], false, true, false, true);
		SubstitutionFiltersList.SetEnabled(true);
		SubstitutionFiltersList.SaveFilters();
		FlushAndRescan(true);
		//Rescan();
	}
	return;
}

static inline bool IsDiffPerLine(bool bTableEditing, const DIFFRANGE& cd)
{
	const int LineLimit = 20;
	return (bTableEditing || (cd.dend - cd.dbegin > LineLimit)) ? true : false;
}

/**
 * @brief Returns rectangles to highlight in both views (to show differences in line specified)
 */
void CMergeDoc::Computelinediff(CMergeEditView *pView, CRect rc[], bool bReversed)
{
	int file;
	for (file = 0; file < m_nBuffers; file++)
		rc[file].top = -1;

	const int nActivePane = pView->m_nThisPane;
	if (m_diffList.GetSize() == 0 || IsEditedAfterRescan())
		return;

	auto [ptStart, ptEnd] = pView->GetSelection();

	const int nLineCount = m_ptBuf[nActivePane]->GetLineCount();
	size_t nWordDiff = 0;
	DIFFRANGE di;
	vector<WordDiff> worddiffs;
	int nDiff = m_diffList.LineToDiff(ptStart.y);
	if (nDiff != -1)
		worddiffs = GetWordDiffArrayInDiffBlock(nDiff);

	if (!worddiffs.empty())
	{
		bool bGoToNextWordDiff = true;
		if (EqualCurrentWordDiff(nActivePane, ptStart, ptEnd))
		{
			nWordDiff = m_CurWordDiff.nWordDiff;
		}
		else
		{
			if (!bReversed)
			{
				for (nWordDiff = 0; nWordDiff < worddiffs.size(); ++nWordDiff)
				{
					auto& worddiff = worddiffs[nWordDiff];
					if (worddiff.beginline[nActivePane] <= ptStart.y && ptStart.y <= worddiff.endline[nActivePane])
					{
						int begin = (worddiff.beginline[nActivePane] < ptStart.y) ? 0 : worddiff.begin[nActivePane];
						int end = (worddiff.endline[nActivePane] > ptStart.y) ? m_ptBuf[nActivePane]->GetLineLength(ptStart.y) : worddiff.end[nActivePane];
						if (ptStart.x <= begin || (begin <= ptStart.x && ptStart.x <= end))
						{
							bGoToNextWordDiff = false;
							break;
						}
					}
				}
			}
			else
			{
				for (nWordDiff = worddiffs.size() - 1; nWordDiff != static_cast<size_t>(-1); --nWordDiff)
				{
					auto& worddiff = worddiffs[nWordDiff];
					if (worddiff.beginline[nActivePane] <= ptStart.y && ptStart.y <= worddiff.endline[nActivePane])
					{
						int begin = (worddiff.beginline[nActivePane] < ptStart.y) ? 0 : worddiff.begin[nActivePane];
						int end = (worddiff.endline[nActivePane] > ptStart.y) ? m_ptBuf[nActivePane]->GetLineLength(ptStart.y) : worddiff.end[nActivePane];
						if (ptStart.x >= end || (begin <= ptStart.x && ptStart.x <= end))
						{
							bGoToNextWordDiff = false;
							break;
						}
					}
				}
			}
		}
		if (bGoToNextWordDiff)
		{
			if (!bReversed)
			{
				if (nWordDiff < worddiffs.size())
					++nWordDiff;

				if (nWordDiff == worddiffs.size())
				{
					m_diffList.GetDiff(nDiff, di);
					ptStart.y = (di.dend + 1) % nLineCount;
					worddiffs.clear();
				}
			}
			else
			{
				if (nWordDiff == 0)
				{
					m_diffList.GetDiff(nDiff, di);
					ptStart.y = (di.dbegin - 1) % nLineCount;
					worddiffs.clear();
				}
				else
					--nWordDiff;
			}
		}
	}
	if (worddiffs.empty())
	{
		if (!bReversed)
		{
			nDiff = m_diffList.LineToDiff(ptStart.y);
			if (nDiff == -1)
			{
				m_diffList.GetNextDiff(ptStart.y, nDiff);
				if (nDiff == -1)
					nDiff = 0;
			}
			else
			{
				nDiff = (nDiff + 1) % m_diffList.GetSize();
			}
			worddiffs = GetWordDiffArrayInDiffBlock(nDiff);
			nWordDiff = 0;
		}
		else
		{
			nDiff = m_diffList.LineToDiff(ptStart.y);
			if (nDiff == -1)
			{
				m_diffList.GetPrevDiff(ptStart.y, nDiff);
				if (nDiff == -1)
					nDiff = m_diffList.GetSize() - 1;
			}
			else
			{
				nDiff = (nDiff - 1) % m_diffList.GetSize();
			}
			worddiffs = GetWordDiffArrayInDiffBlock(nDiff);
			nWordDiff = worddiffs.size() - 1;
		}
		if (worddiffs.empty())
		{
			if (nDiff == -1)
				return;
			for (file = 0; file < m_nBuffers; file++)
			{
				rc[file].left = 0;
				rc[file].top = di.dbegin;
				rc[file].right = 0;
				if (di.dbegin < nLineCount - 1)
					rc[file].bottom = di.dbegin + 1;
				else
					rc[file].bottom = di.dbegin;
			}
			nWordDiff = static_cast<size_t>(-1);
		}
	}

	if (nWordDiff != static_cast<size_t>(-1))
	{
		auto& worddiff = worddiffs[nWordDiff];
		for (file = 0; file < m_nBuffers; file++)
		{
			rc[file].left = worddiff.begin[file];
			rc[file].top = worddiff.beginline[file];
			rc[file].right = worddiff.end[file];
			rc[file].bottom = worddiff.endline[file];
		}
	}

	m_CurWordDiff.nPane = nActivePane;
	m_CurWordDiff.ptStart.x = rc[nActivePane].left;
	m_CurWordDiff.ptStart.y = rc[nActivePane].top;
	m_CurWordDiff.ptEnd.x = rc[nActivePane].right;
	m_CurWordDiff.ptEnd.y = rc[nActivePane].bottom;
	m_CurWordDiff.nDiff = nDiff;
	m_CurWordDiff.nWordDiff = nWordDiff;
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
	m_diffList.GetDiff(nDiff, cd);

	bool diffPerLine = IsDiffPerLine(m_ptBuf[0]->GetTableEditing(), cd);
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
	bool diffPerLine = IsDiffPerLine(m_ptBuf[0]->GetTableEditing(), cd);

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
	bool eolSensitive = !diffOptions.bIgnoreEol;
	int xwhite = diffOptions.nIgnoreWhitespace;
	int breakType = GetBreakType(); // whitespace only or include punctuation
	bool byteColoring = GetByteColoringOption();

	// Make the call to stringdiffs, which does all the hard & tedious computations
	std::vector<strdiff::wdiff> wdiffs = strdiff::ComputeWordDiffs(m_nBuffers, str, casitive, eolSensitive, xwhite, breakType, byteColoring);

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
			{
				if (wd.beginline[file] < m_ptBuf[file]->GetLineCount() - 1)
				{
					wd.begin[file] = 0;
					wd.beginline[file]++;
				}
				else
				{
					wd.begin[file] = m_ptBuf[file]->GetLineLength(nLine);
				}
			}

			for (; nLine < nLineEnd; nLine++)
			{
				if (it->end[file] + 1 == nOffsets[file][nLine-nLineBegin] || it->end[file] + 1 < nOffsets[file][nLine-nLineBegin+1])
					break;
			}
			wd.endline[file] = nLine;
			wd.end[file] = it->end[file]  + 1 - nOffsets[file][nLine-nLineBegin];
			if (m_ptBuf[file]->GetLineLength(nLine) < wd.end[file])
			{
				if (wd.endline[file] < m_ptBuf[file]->GetLineCount() - 1)
				{
					wd.end[file] = 0;
					wd.endline[file]++;
				}
				else
				{
					wd.end[file] = m_ptBuf[file]->GetLineLength(nLine);
				}
			}
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

