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
* @brief Add all different elements between lines to the m_pDiffs list.
*/
void stringdiffs::BuildWordDiffList()
{
	BuildWordsArray(m_str1, &m_words1);
	BuildWordsArray(m_str2, &m_words2);

	INT_PTR nWordsCount = min(m_words1.GetSize(), m_words2.GetSize());
	INT_PTR nDiffStartAtWord = -1;
	INT_PTR nWord;
	bool bInWordDiff = false;

	// Compare each word against its matching word on the second list. Each
	// difference starts from the first non-matching word and ends in the
	// predecessor of the first matching word.
	for (nWord = 0; nWord < nWordsCount; ++nWord)
	{
		if (!bInWordDiff)
		{
			if (!AreWordsSame(m_words1[nWord], m_words2[nWord]))
			{
				nDiffStartAtWord = nWord;
				bInWordDiff = true;
			}
		}
		else
		{
			if (AreWordsSame(m_words1[nWord], m_words2[nWord]))
			{
				INT_PTR nDiffEndAtWord = nWord - 1;
				AddWordDiff(nDiffStartAtWord, nWord - 1);
				bInWordDiff = false;
			}
		}
	}

	// Ops, we left an open word difference, better close it. This could
	// happen when there is only one difference. Either a line with a single
	// word or reaching end of line without finding matching word.
	if (bInWordDiff)
	{
		INT_PTR nDiffEndAtWord = nWord - 1;
		AddWordDiff(nDiffStartAtWord, nWord - 1);
	}

	// Handle the case where word lists are not in the same size.
	if (nWordsCount < m_words1.GetSize())
	{
		// Left side has more words.
		AddDiff(m_words1[nWordsCount].start,
			m_words1[m_words1.GetUpperBound()].end,
			m_str2.GetLength(),	-1);
	}
	else if (nWordsCount < m_words2.GetSize())
	{
		// Right side has more words.
		AddDiff(m_str1.GetLength(), -1,
			m_words2[nWordsCount].start,
			m_words2[m_words2.GetUpperBound()].end);
	}
}

/**
* @brief Add a difference using offsets.
*/
void stringdiffs::AddDiff(int s1, int e1, int s2, int e2)
{
	wdiff wdf(s1, e1, s2, e2);
	m_pDiffs->Add(wdf);
}

/**
* @brief Add a difference using words range.
*/
void stringdiffs::AddWordDiff(int start, int end)
{
	AddDiff(m_words1[start].start, m_words1[end].end,
		m_words2[start].start, m_words2[end].end);
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
