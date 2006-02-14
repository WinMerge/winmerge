/** 
 * @file  stringdiffs.cpp
 *
 * @brief Implementation file for sd_ComputeWordDiffs (q.v.)
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include <mbctype.h>
#include "stringdiffs.h"
#include "CompareOptions.h"
#include "stringdiffsi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static bool isSafeWhitespace(TCHAR ch);
static bool isWordBreak(int breakType, TCHAR ch);
static void wordLevelToByteLevel(wdiffarray * pDiffs, const CString& str1, const CString& str2, bool casitive, int xwhite);

/**
 * @brief Construct our worker object and tell it to do the work
 */
void
sd_ComputeWordDiffs(const CString & str1, const CString & str2,
	bool case_sensitive, int whitespace, int breakType, bool byte_level,
	wdiffarray * pDiffs)
{
	stringdiffs sdiffs(str1, str2, case_sensitive, whitespace, breakType, pDiffs);
	// Hash all words in both lines and then compare them word by word
	// storing differences into m_wdiffs
	sdiffs.BuildWordDiffList();
	// Now copy m_wdiffs into caller-supplied m_pDiffs (coalescing adjacents if possible)
	sdiffs.PopulateDiffs();
	// Adjust the range of the word diff down to byte level.
	if (byte_level)
		wordLevelToByteLevel(pDiffs, str1, str2, case_sensitive, whitespace);
}

/**
 * @brief stringdiffs constructor simply loads all members from arguments
 */
stringdiffs::stringdiffs(const CString & str1, const CString & str2,
	bool case_sensitive, int whitespace, int breakType,
	wdiffarray * pDiffs)
: m_str1(str1)
, m_str2(str2)
, m_case_sensitive(case_sensitive)
, m_whitespace(whitespace)
, m_breakType(breakType)
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
			&& isSafeWhitespace(m_str1[i1]) && isSafeWhitespace(m_str2[i2]))
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
	if (isSafeWhitespace(str[i])) 
	{
		++i;
		goto inspace;
	}
	begin = i;
	goto inword;

	// state when we are inside a word
inword:
	bool atspace=false;
	if (i==str.GetLength() || (atspace=isSafeWhitespace(str[i])) || isWordBreak(m_breakType, str[i]))
	{
		if (begin<i)
		{
			// just finished a word
			// e is first non-word character (space or at end)
			int e = i-1;
			word wd(begin, e, hash(str, begin, e));
			words->Add(wd);
		}
		if (i == str.GetLength())
		{
			return;
		}
		else if (atspace)
		{
			goto inspace;
		}
		else
		{
			// start a new word because we hit a non-whitespace word break (eg, a comma)
			// but, we have to put each word break character into its own word
			word wd(i, i, hash(str, i, i));
			words->Add(wd);
			++i;
			begin = i;
			goto inword;
		}
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
 * @brief Return true if chars match
 *
 * Caller must not call this for lead bytes
 */
static bool
matchchar(TCHAR ch1, TCHAR ch2, bool casitive)
{
	if (casitive)
		return ch1==ch2;
	else 
		return _totupper(ch1)==_totupper(ch2);
}


/** Does character introduce a multicharacter character? */
static inline bool IsLeadByte(TCHAR ch)
{
#ifdef UNICODE
	return false;
#else
	return _getmbcp() && IsDBCSLeadByte(ch);
#endif
}

/**
 * @brief Is it whitespace (excludes all lead & trail bytes)?
 */
static bool
isSafeWhitespace(TCHAR ch)
{
	return xisspace(ch) && !IsLeadByte(ch);
}

/**
 * @brief Is it a non-whitespace wordbreak character (ie, punctuation)?
 */
static bool
isWordBreak(int breakType, TCHAR ch)
{
	// breakType==0 means whitespace only
	if (!breakType) return false;
	// breakType==1 means break also on punctuation
	return ch==',' || ch==';' || ch==':' || ch=='.';
}


/**
 * @brief Return pointer to last character of specified string (handle MBCS)
 *
 * If the last byte is a broken multibyte (ie, a solo lead byte), this returns previous char
 */
static LPCTSTR
LastChar(LPCTSTR psz, int len)
{
	if (!len) return psz;

	if (!_getmbcp()) return psz+len-1;

	LPCTSTR lastValid = psz+len-1;

	LPCTSTR prev=psz;
	while (psz<lastValid)
	{
		prev = psz;
		psz = CharNext(psz);
	}
	if (psz==lastValid && !IsLeadByte(*psz))
		return psz;
	else // last character was multibyte or broken multibyte
		return prev;
}

/**
 * @brief advance current pointer over whitespace, until not whitespace or beyond end
 * @param pcurrent [in,out] current location (to be advanced)
 * @param end [in] last valid position (only go one beyond this)
 */
static void
AdvanceOverWhitespace(LPCTSTR * pcurrent, LPCTSTR end)
{
	// advance over whitespace
	while (*pcurrent <= end && isSafeWhitespace(**pcurrent))
		++(*pcurrent); // DBCS safe because of isSafeWhitespace above
}

/**
 * @brief back current pointer over whitespace, until not whitespace or at start
 * @param pcurrent [in,out] current location (to be backed up)
 * @param start [in] first valid position (do not go before this)
 *
 * NB: Unlike AdvanceOverWhitespace, this will not go over the start
 * This because WinMerge doesn't need to, and also CharPrev cannot easily do so
 */
static void
RetreatOverWhitespace(LPCTSTR * pcurrent, LPCTSTR start)
{
	// back over whitespace
	while (*pcurrent > start && isSafeWhitespace(**pcurrent))
		*pcurrent = CharPrev(start, *pcurrent); // DBCS safe because of isSafeWhitespace above
}

/**
 * @brief Compute begin1,begin2,end1,end2 to display byte difference between strings str1 & str2
 * @param casitive [in] true for case-sensitive, false for case-insensitive
 * @param xwhite [in] This governs whether we handle whitespace specially (see WHITESPACE_COMPARE_ALL, WHITESPACE_IGNORE_CHANGE, WHITESPACE_IGNORE_ALL)
 *
 * Assumes whitespace is never leadbyte or trailbyte!
 */
void
sd_ComputeByteDiff(CString & str1, CString & str2, 
		   bool casitive, int xwhite, 
		   int &begin1, int &begin2, int &end1, int &end2)
{
	// Set to sane values
	// Also this way can distinguish if we set begin1 to -1 for no diff in line
	begin1 = end1 = begin2 = end2 = 0;

	int len1 = str1.GetLength();
	int len2 = str2.GetLength();

	LPCTSTR pbeg1 = (LPCTSTR)str1;
	LPCTSTR pbeg2 = (LPCTSTR)str2;

	if (len1 == 0 || len2 == 0)
	{
		if (len1 == len2)
		{
			begin1 = -1;
			begin2 = -1;
		}
		end1 = len1 - 1;
		end2 = len2 - 1;
		return;
	}

	// cursors from front, which we advance to beginning of difference
	LPCTSTR py1 = pbeg1;
	LPCTSTR py2 = pbeg2;

	// pen1,pen2 point to the last valid character (broken multibyte lead chars don't count)
	LPCTSTR pen1 = LastChar(py1, len1);
	LPCTSTR pen2 = LastChar(py2, len2);

	if (xwhite != WHITESPACE_COMPARE_ALL)
	{
		// Ignore leading and trailing whitespace
		// by advancing py1 and py2
		// and retreating pen1 and pen2
		while (py1 < pen1 && isSafeWhitespace(*py1))
			++py1; // DBCS safe because of isSafeWhitespace above
		while (py2 < pen2 && isSafeWhitespace(*py2))
			++py2; // DBCS safe because of isSafeWhitespace above
		if ((pen1 < pbeg1 + len1 - 1 || pen2 < pbeg2 + len2 -1)
			&& (!len1 || !len2 || pbeg1[len1] != pbeg2[len2]))
		{
			// mismatched broken multibyte ends
		}
		else
		{
			while (pen1 > py1 && isSafeWhitespace(*pen1))
				pen1 = CharPrev(py1, pen1);
			while (pen2 > py2 && isSafeWhitespace(*pen2))
				pen2 = CharPrev(py2, pen2);
		}
	}

	bool alldone = false;
	// Advance over matching beginnings of lines
	// Advance py1 & py2 from beginning until find difference or end
	while (1)
	{
		// Potential difference extends from py1 to pen1 and py2 to pen2

		// Check if either side finished
		if (py1 > pen1 && py2 > pen2)
		{
			begin1 = end1 = begin2 = end2 = -1;
			alldone = true;
			break;
		}
		if (py1 > pen1 || py2 > pen2)
		{
			alldone = true;
			break;
		}

		// handle all the whitespace logic (due to WinMerge whitespace settings)
		if (xwhite!=WHITESPACE_COMPARE_ALL && isSafeWhitespace(*py1))
		{
			if (xwhite==WHITESPACE_IGNORE_CHANGE && !isSafeWhitespace(*py2))
			{
				// py1 is white but py2 is not
				// in WHITESPACE_IGNORE_CHANGE mode,
				// this doesn't qualify as skippable whitespace
				break; // done with forward search
			}
			// gobble up all whitespace in current area
			AdvanceOverWhitespace(&py1, pen1); // will go beyond end
			AdvanceOverWhitespace(&py2, pen2); // will go beyond end
			continue;

		}
		if (xwhite!=WHITESPACE_COMPARE_ALL && isSafeWhitespace(*py2))
		{
			if (xwhite==WHITESPACE_IGNORE_CHANGE && !isSafeWhitespace(*py1))
			{
				// py2 is white but py1 is not
				// in WHITESPACE_IGNORE_CHANGE mode,
				// this doesn't qualify as skippable whitespace
				break; // done with forward search
			}
			// gobble up all whitespace in current area
			AdvanceOverWhitespace(&py1, pen1); // will go beyond end
			AdvanceOverWhitespace(&py2, pen2); // will go beyond end
			continue;
		}

		// Now do real character match
		if (IsLeadByte(*py1))
		{
			if (!IsLeadByte(*py2))
				break; // done with forward search
			// DBCS (we assume if a lead byte, then character is 2-byte)
			if (!(py1[0] == py2[0] && py1[1] == py2[1]))
				break; // done with forward search
			py1 += 2; // DBCS specific
			py2 += 2; // DBCS specific
		}
		else
		{
			if (IsLeadByte(*py2))
				break; // done with forward search
			if (!matchchar(py1[0], py2[0], casitive))
				break; // done with forward search
			++py1; // DBCS safe b/c we checked above
			++py2; // DBCS safe b/c we checked above
		}
	}

	// Potential difference extends from py1 to pen1 and py2 to pen2

	// Store results of advance into return variables (begin1 & begin2)
	// -1 in a begin variable means no visible diff area
	begin1 = (py1 > pen1) ? -1 : (py1 - pbeg1);
	begin2 = (py2 > pen2) ? -1 : (py2 - pbeg2);

	if (alldone)
	{
		end1 = pen1 - pbeg1;
		end2 = pen2 - pbeg2;
	}
	else
	{
		LPCTSTR pz1 = pen1;
		LPCTSTR pz2 = pen2;

		// Retreat over matching ends of lines
		// Retreat pz1 & pz2 from end until find difference or beginning
		while (1)
		{
			// Potential difference extends from py1 to pz1 and from py2 to pz2

			// Check if either side finished
			if (pz1 < py1 && pz2 < py2)
			{
				begin1 = end1 = begin2 = end2 = -1;
				break;
			}
			if (pz1 < py1 || pz2 < py2)
			{
				break;
			}

			// handle all the whitespace logic (due to WinMerge whitespace settings)
			if (xwhite!=WHITESPACE_COMPARE_ALL && isSafeWhitespace(*pz1))
			{
				if (xwhite==WHITESPACE_IGNORE_CHANGE && !isSafeWhitespace(*pz2))
				{
					// pz1 is white but pz2 is not
					// in WHITESPACE_IGNORE_CHANGE mode,
					// this doesn't qualify as skippable whitespace
					break; // done with reverse search
				}
				// gobble up all whitespace in current area
				RetreatOverWhitespace(&pz1, py1); // will not go over beginning
				RetreatOverWhitespace(&pz2, py2); // will not go over beginning
				continue;

			}
			if (xwhite!=WHITESPACE_COMPARE_ALL && isSafeWhitespace(*pz2))
			{
				if (xwhite==WHITESPACE_IGNORE_CHANGE && !isSafeWhitespace(*pz1))
				{
					// pz2 is white but pz1 is not
					// in WHITESPACE_IGNORE_CHANGE mode,
					// this doesn't qualify as skippable whitespace
					break; // done with reverse search
				}
				// gobble up all whitespace in current area
				RetreatOverWhitespace(&pz1, py1); // will not go over beginning
				RetreatOverWhitespace(&pz2, py2); // will not go over beginning
				continue;
			}

			// Now do real character match
			if (IsLeadByte(*pz1))
			{
				if (!IsLeadByte(*pz2))
					break; // done with forward search
				// DBCS (we assume if a lead byte, then character is 2-byte)
				if (!(pz1[0] == pz2[0] && pz1[1] == pz2[1]))
					break; // done with forward search
			}
			else
			{
				if (IsLeadByte(*pz2))
					break; // done with forward search
				if (!matchchar(pz1[0], pz2[0], casitive))
					break; // done with forward search
			}
			// decrement pz1 and pz2
			if (pz1 == pbeg1)
				pz1 = pbeg1 - 1; // earlier than pbeg1 signifies no difference
			else
				pz1 = CharPrev(pbeg1, pz1);
			if (pz2 == pbeg2)
				pz2 = pbeg2 - 1; // earlier than pbeg1 signifies no difference
			else
				pz2 = CharPrev(pbeg2, pz2);
		}

		// Store results of advance into return variables (end1 & end2)
		if (pz1 < pbeg1)
			begin1 = -1; // no visible diff in line 1
		else
			end1 = pz1 - pbeg1;
		if (pz2 < pbeg2)
			begin2 = -1; // no visible diff in line 2
		else
			end2 = pz2 - pbeg2;
		
	}
}

/**
 * @brief adjust the range of the specified word diffs down to byte level.
 * @param str1, str2 [in] line to be compared
 * @param casitive [in] true for case-sensitive, false for case-insensitive
 * @param xwhite [in] This governs whether we handle whitespace specially (see WHITESPACE_COMPARE_ALL, WHITESPACE_IGNORE_CHANGE, WHITESPACE_IGNORE_ALL)
 */
static void wordLevelToByteLevel(wdiffarray * pDiffs, const CString& str1, const CString& str2, bool casitive, int xwhite)
{
	for (int i = 0; i < pDiffs->GetSize(); i++)
	{
		int begin1, begin2, end1, end2;
		wdiff *pDiff = &(*pDiffs)[i];
		CString str1_2, str2_2;
		str1_2 = str1.Mid(pDiff->start[0], pDiff->end[0] - pDiff->start[0] + 1);
		str2_2 = str2.Mid(pDiff->start[1], pDiff->end[1] - pDiff->start[1] + 1);
		sd_ComputeByteDiff(str1_2, str2_2, casitive, xwhite, begin1, begin2, end1, end2);
		if (begin1 == -1)
		{
			// no visible diff on side1
			pDiff->end[0] = pDiff->start[0];
		}
		else
		{
			pDiff->end[0] = pDiff->start[0] + end1;
			pDiff->start[0] += begin1;
		}
		if (begin2 == -1)
		{
			// no visible diff on side2
			pDiff->end[1] = pDiff->start[1];
		}
		else
		{
			pDiff->end[1] = pDiff->start[1] + end2;
			pDiff->start[1] += begin2;
		}
	}
}
