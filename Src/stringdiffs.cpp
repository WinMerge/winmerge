/** 
 * @file  stringdiffs.cpp
 *
 * @brief Implementation file for stringdiffs_Get (q.v.)
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "stringdiffs.h"
int i;
class stuff {
	int j;
};
#include "stringdiffsi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static bool isWhitespace(TCHAR ch);

/**
 * @brief Construct our worker object and tell it to do the work
 */
void
stringdiffs_Get(const CString & str1, const CString & str2,
	bool case_sensitive, int whitespace,
	wdiffarray * pDiffs)
{
	stringdiffs sdiffs(str1, str2, case_sensitive, whitespace, pDiffs);
	// Hash all words in both lines and then compare them word by word
	// storing differences into m_wdiffs
	sdiffs.BuildWordDiffList();
	// Now copy m_wdiffs into caller-supplied m_pDiffs (coalescing adjacents if possible)
	sdiffs.PopulateDiffs();
}

/**
 * @brief stringdiffs constructor simply loads all members from arguments
 */
stringdiffs::stringdiffs(const CString & str1, const CString & str2,
	bool case_sensitive, int whitespace, 
	wdiffarray * pDiffs)
: m_str1(str1)
, m_str2(str2)
, m_case_sensitive(case_sensitive)
, m_whitespace(whitespace)
, m_pDiffs(pDiffs)
{
}

/**
 * @brief Add all different elements between lines to the wdiff list
 */
void
stringdiffs::BuildWordDiffList()
{
	BuildWordsArray(m_str1, &m_words1);
	BuildWordsArray(m_str2, &m_words2);

	int w1=0, w2=0; // next word
	int bw1, bw2; // internal temporaries

// We don't have a difference accumulated right now
insame:
	if (w1 == m_words1.GetSize() || w2 == m_words2.GetSize())
	{
		int i1 = (w1>0 ? m_words1[w1-1].end+1 : 0); // after end of word before w1
		int i2 = (w2>0 ? m_words2[w2-1].end+1 : 0); // after end of word before w2
		// Done, but handle trailing spaces
		while (i1 < m_str1.GetLength() && i2 < m_str2.GetLength()
			&& isWhitespace(m_str1[i1]) && isWhitespace(m_str2[i2]))
		{
			if (m_whitespace==0)
			{
				// Compare all whitespace
				if (!caseMatch(m_str1[i1], m_str2[i2]))
					break;
			}
			++i1;
			++i2;
		}
		if (i1 != m_str1.GetLength() || i2 != m_str2.GetLength())
		{
			wdiff wdf(i1,  m_str1.GetLength()-1, i2, m_str2.GetLength()-1);
			m_wdiffs.Add(wdf);
		}
		return;
	}
	// Check whitespace before current words for difference, if appropriate
	if (m_whitespace==0)
	{
		// Compare all whitespace
		int i1 = (w1>0 ? m_words1[w1-1].end+1 : 0); // after end of word before w1
		int i2 = (w2>0 ? m_words2[w2-1].end+1 : 0); // after end of word before w2
		while (i1<m_words1[w1].start || i2<m_words2[w2].start)
		{
			if (i1==m_words1[w1].start || i2==m_words2[w2].start
				|| m_str1[i1] != m_str2[i2])
			{
				// Found a difference
				break;
			}
			// Not difference, keep looking
			++i1;
			++i2;
		}
		if (i1<m_words1[w1].start || i2<m_words2[w2].start)
		{
			// Found a difference
			// Now backtrack from next word to find end of difference
			int e1 = m_words1[w1].start-1;
			int e2 = m_words2[w2].start-1;
			while (e1>i1 && e2>i2)
			{
				if (m_str1[e1] != m_str2[e2])
				{
					// Found a difference
					break;
				}
				// Not difference, keep looking
				--e1;
				--e2;
			}
			// Add the difference we've found
			wdiff wdf(i1, e1, i2, e2);
			m_wdiffs.Add(wdf);
		}
		
	}
	// Now check current words for difference
	if (!AreWordsSame(m_words1[w1], m_words2[w2]))
		goto startdiff;
	++w1;
	++w2;
	goto insame; // safe even if at the end of one line's words

// Just beginning a difference
startdiff:
	bw1 = w1;
	bw2 = w2;

// Currently in a difference
// Actually we don't have a label here, because we don't loop to here
// We always find the end of the difference and jump straight to it

	if (!findSync(&w1, &w2))
	{
		// Add a diff from bw1 & bw2 to end of both lines
		int s1 = m_words1[bw1].start;
		int e1 = m_words1[m_words1.GetUpperBound()].end;
		int s2 = m_words2[bw2].start;
		int e2 = m_words2[m_words2.GetUpperBound()].end;
		wdiff wdf(s1, e1, s2, e2);
		m_wdiffs.Add(wdf);
		// Now skip directly to end of last word in each line
		w1 = m_words1.GetSize();
		w2 = m_words2.GetSize();
		// go to process trailing spaces and quit
		goto insame;
	}
	else
	{
		// NB: To get into indiff, must be at least one different word
		// To reach here, must also be be a sync
		// So there is a word at the start of this diff, and a word after us
		// w1 is valid because it is the word after us
		// w1-1 >= bw1 is valid because it is the word at the start of this diff

		// Add a diff from start to just before sync word
		int s1 = m_words1[bw1].start;
		int e1 = 0; // placeholder, set below
		int s2 = m_words2[bw2].start;
		int e2 = 0; // placeholder, set below
		if (m_whitespace == 0)
		{
			// Grab all the trailing whitespace for our diff
			e1 = m_words1[w1].start-1;
			e2 = m_words2[w2].start-1;
			// Now backtrack over matching whitespace
			int pe1 = (w1 ? m_words1[w1-1].end+1 : -1);
			int pe2 = (w2 ? m_words2[w2-1].end+1 : -1);
			while (e1 > pe1
				&& e2 > pe2
				&& m_str1[e1] == m_str2[e2])
			{
				--e1;
				--e2;
			}
		}
		else
		{
			// ignore whitespace, so leave it out of diff
			e1 = (w1 ? m_words1[w1-1].end+1 : -1);
			e2 = (w2 ? m_words2[w2-1].end+1 : -1);
		}
		wdiff wdf(s1, e1, s2, e2);
		m_wdiffs.Add(wdf);
		// skip past sync words (which we already know match)
		++w1;
		++w2;
		// go process sync
		goto insame; // safe even if at the end of one line's words
	}
}

/**
 * @brief Find closest matching word
 */
bool
stringdiffs::findSync(int *w1, int *w2) const
{
	// Look among remaining words in m_words2 for a word that matches w1
	int cw2 = -1;
	while (*w1 < m_words1.GetSize())
	{
		cw2 = FindNextMatchInWords2(m_words1[*w1], *w2);
		if (cw2>=0)
			break;
		// No word matches w1
		++(*w1);
	}
	// Look among remaining words in m_words1 for a word that matches w2
	int cw1 = -1;
	while (*w2 < m_words2.GetSize())
	{
		cw1 = FindNextMatchInWords1(m_words2[*w2], *w1);
		if (cw1>=0)
			break;
		// No word matches w2
		++(*w2);
	}
	if (cw1 == -1)
	{
		if (cw2 ==-1)
			return false;
		*w2 = cw2;
	}
	else
	{
		if (cw2 ==-1)
		{
			*w1 = cw1;
		}
		else
		{
			// We have candidates advancing along either string
			// Pick closer
			int len1 = m_words1[cw1].end - m_words1[*w1].start;
			int len2 = m_words2[cw2].end - m_words2[*w2].start;
			if (len1 < len2)
				*w1 = cw1;
			else
				*w2 = cw2;
		}
	}
	return true;
}

/**
 * @brief Find next word in m_words2 (starting at bw2) that matches needword1 (in m_words1)
 */
int 
stringdiffs::FindNextMatchInWords2(const word & needword1, int bw2) const
{
	while (bw2 < m_words2.GetSize())
	{
		if (AreWordsSame(needword1, m_words2[bw2]))
			return bw2;
		++bw2;
	}
	return -1;
}

/**
 * @brief Find next word in m_words1 (starting at bw1) that matches needword2 (in m_words2)
 */
int 
stringdiffs::FindNextMatchInWords1(const word & needword2, int bw1) const
{
	while (bw1 < m_words1.GetSize())
	{
		if (AreWordsSame(m_words1[bw1], needword2))
			return bw1;
		++bw1;
	}
	return -1;
}

/**
 * @brief Break line into constituent words
 */
void
stringdiffs::BuildWordsArray(const CString & str, wordarray * words)
{
	int i=0, begin=0;

	// state when we are looking for next word
inspace:
	if (i==str.GetLength())
		return;
	if (isWhitespace(str[i])) 
	{
		++i;
		goto inspace;
	}
	begin = i;
	goto inword;

	// state when we are inside a word
inword:
	if (i==str.GetLength() || isWhitespace(str[i]))
	{
		if (begin<i)
		{
			// just finished a word
			// e is first non-word character (space or at end)
			int e = i-1;
			word wd(begin, e, hash(str, begin, e));
			words->Add(wd);
		}
		goto inspace; // safe even if we're at the end
	}
	++i;
	goto inword; // safe even if we're at the end or no longer in a word
}

/**
 * @brief Populate m_pDiffs from m_wdiffs (combining adjacent diffs)
 *
 * Doing the combining of adjacent diffs here keeps some complexity out of BuildWordsArray.
 */
void
stringdiffs::PopulateDiffs()
{
	for (int i=0; i<m_wdiffs.GetSize(); ++i)
	{
		wdiff & wd = m_wdiffs[i];
		bool skipIt = false;
		// combine it with next ?
		if (i+1<m_wdiffs.GetSize())
		{
			if (m_wdiffs[i].end[0] == m_wdiffs[i+1].start[0]
				&& m_wdiffs[i].end[1] == m_wdiffs[i+1].start[1])
			{
				// diff[i] and diff[i+1] are contiguous
				// so combine them into diff[i+1] and ignore diff[i]
				m_wdiffs[i+1].start[0] = m_wdiffs[i].start[0];
				m_wdiffs[i+1].start[1] = m_wdiffs[i].start[1];
				skipIt = true;
			}
		}
		if (!skipIt)
		{
			// Should never have a pair where both are missing
			ASSERT(m_wdiffs[i].start[0]>=0 || m_wdiffs[i].start[1]>=0);

			// Store the diff[i] in the caller list (m_pDiffs)
			wdiff dr(m_wdiffs[i]);
			m_pDiffs->Add(dr);
		}
	}
}

// diffutils hash

/* Rotate a value n bits to the left. */
#define UINT_BIT (sizeof (unsigned) * CHAR_BIT)
#define ROL(v, n) ((v) << (n) | (v) >> (UINT_BIT - (n)))
/* Given a hash value and a new character, return a new hash value. */
#define HASH(h, c) ((c) + ROL (h, 7))

int
stringdiffs::hash(const CString & str, int begin, int end) const
{
	UINT h = 0;
	for (int i=begin; i<end; ++i)
	{
		UINT ch = (UINT)str[i];
		if (m_case_sensitive)
		{
			h += HASH(h, ch);
		}
		else
		{
			ch = (UINT)_totupper(ch);
			h += HASH(h, ch);
		}
	}

	return h;
}

/**
 * @brief Compare two words (by reference to original strings)
 */
bool
stringdiffs::AreWordsSame(const word & word1, const word & word2) const
{
	if (word1.hash != word2.hash)
		return false;
	if (word1.length() != word2.length())
		return false;
	for (int i=0; i<word1.length(); ++i)
	{
		if (!caseMatch(m_str1[word1.start+i], m_str2[word2.start+i]))
			return false;
	}
	return true;
}

/**
 * @brief Return true if characters match
 */
bool
stringdiffs::caseMatch(TCHAR ch1, TCHAR ch2) const
{
	if (m_case_sensitive) 
		return ch1==ch2;
	else 
		return _totupper(ch1)==_totupper(ch2);
}

/**
 * @brief Return true if character meets our definition for whitespace
 */
static bool
isWhitespace(TCHAR ch)
{
	// Actually, we just use the C run time library :)
	return !!_istspace(ch); 
}
