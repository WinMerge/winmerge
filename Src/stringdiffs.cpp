/** 
 * @file  stringdiffs.cpp
 *
 * @brief Implementation file for sd_ComputeWordDiffs (q.v.)
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <mbctype.h>
#include "string_util.h"
#include "stringdiffs.h"
#include "CompareOptions.h"
#include "stringdiffsi.h"

using std::vector;

// Uncomment this to see stringdiff log messages
// We don't use _DEBUG since stringdiff logging is verbose and slows down WinMerge
// #define STRINGDIFF_LOGGING

static bool Initialized;
static bool CustomChars;
static TCHAR *BreakChars;
static TCHAR BreakCharDefaults[] = _T(",.;:");

static bool isSafeWhitespace(TCHAR ch);
static bool isWordBreak(int breakType, TCHAR ch);
static void wordLevelToByteLevel(vector<wdiff*> * pDiffs, const String& str1,
		const String& str2, bool casitive, int xwhite);

void sd_Init()
{
	BreakChars = &BreakCharDefaults[0];
	Initialized = true;
}

void sd_Close()
{
	if (CustomChars)
	{
		delete [] BreakChars;
		BreakChars = NULL;
		CustomChars = false;
	}
	Initialized = false;
}

void sd_SetBreakChars(const TCHAR *breakChars)
{
	assert(Initialized);

	if (CustomChars)
		delete [] BreakChars;

	CustomChars = true;
	BreakChars = _tcsdup(breakChars);
}

/**
 * @brief Construct our worker object and tell it to do the work
 */
void
sd_ComputeWordDiffs(const String & str1, const String & str2,
	bool case_sensitive, int whitespace, int breakType, bool byte_level,
	vector<wdiff*> * pDiffs)
{
	assert(Initialized);

	stringdiffs sdiffs(str1, str2, case_sensitive, whitespace, breakType, pDiffs);
	// Hash all words in both lines and then compare them word by word
	// storing differences into m_wdiffs
	sdiffs.BuildWordDiffList();
	// Now copy m_wdiffs into caller-supplied m_pDiffs (coalescing adjacents if possible)
	sdiffs.PopulateDiffs();
	// Adjust the range of the word diff down to byte (char) level.
	if (byte_level)
		wordLevelToByteLevel(pDiffs, str1, str2, case_sensitive, whitespace);
}

/**
 * @brief stringdiffs constructor simply loads all members from arguments
 */
stringdiffs::stringdiffs(const String & str1, const String & str2,
	bool case_sensitive, int whitespace, int breakType,
	vector<wdiff*> * pDiffs)
: m_str1(str1)
, m_str2(str2)
, m_case_sensitive(case_sensitive)
, m_whitespace(whitespace)
, m_breakType(breakType)
, m_pDiffs(pDiffs)
{
}

/**
 * @brief Destructor.
 * The destructor frees all diffs added to the vectors.
 */
stringdiffs::~stringdiffs()
{
	while (!m_words1.empty())
	{
		delete m_words1.back();
		m_words1.pop_back();
	}

	while (!m_words2.empty())
	{
		delete m_words2.back();
		m_words2.pop_back();
	}

	while (!m_wdiffs.empty())
	{
		delete m_wdiffs.back();
		m_wdiffs.pop_back();
	}
}

/**
 * @brief Add all different elements between lines to the wdiff list
 */
void
stringdiffs::BuildWordDiffList()
{
	bool m_matchblock (true); // Change to false to get word to word compare
	BuildWordsArray(m_str1, &m_words1);
	BuildWordsArray(m_str2, &m_words2);

	int i = 0; // Number of fixed records of word2
	int start = 0;
	int end = 0;
	String str1;
	String str2;
	TCHAR buf[256];
	// If we have to ignore all whitespace change,
	// just remove leading and ending if one is there
	if (m_whitespace == 2 && m_matchblock)
	{
		// Remove a leading whitespace
		if ((m_words1.size() > 0) && (IsSpace(*m_words1[0])))
		{
			RemoveItem1(0);
		}
		if (m_words1.size() > 0)
		{
			// Remove a ending whitespace
			if (IsSpace(*m_words1[m_words1.size() - 1]))
				RemoveItem1(m_words1.size() - 1);
		}
		// Remove a leading whitespace
		if ((m_words2.size() > 0) && (IsSpace(*m_words2[0])))
		{
			RemoveItem2(0);
			if	(m_words2.size() > 0)
			{
				// Remove a ending whitespace
				if (IsSpace(*m_words2[ m_words2.size() - 1]))
					RemoveItem2(m_words2.size() - 1);
			}
		}
		// Now lets remove all space inbetween and put the words to 
		// one síngle word as it was break by space
		// doit for word1
		i = 0;
		while (m_words1.size() > i + 2)
		{
			if (IsSpace(*m_words1[i + 1]) && m_words1[i + 2]->bBreak == 0)
			{
				m_words1[i]->hash =
					Hash(m_str1, m_words1[i + 2]->start, m_words1[i+2]->end, m_words1[i]->hash);
				m_words1[i]->end = m_words1[i + 2]->end;
				RemoveItem1(i + 1);
				RemoveItem1(i + 1);
				continue;
			}
			i++;
		}
		// Doit for word2
		i = 0;
		while (m_words2.size() > i + 2)
		{
			if (IsSpace(*m_words2[i + 1]) && m_words2[i + 2]->bBreak == 0)
			{
				m_words2[i]->hash =
					Hash(m_str2, m_words2[i + 2]->start, m_words2[i + 2]->end, m_words2[i]->hash);
				m_words2[i]->end = m_words2[i + 2]->end;
				RemoveItem2(i + 1);
				RemoveItem2(i + 1);
				continue;
			}
			i++;
		}
	}
	// Look for a match of word2 in word1
	// not found put an empty record at beginn of word1
	// if distance to far, crosscheck match word1 in word2
	// whatever is shorter, it's result.
	int w1 = 0, w2 = 0;		// position found in array
	int bw1 = 0;			// start position in array

	if (m_matchblock && m_words1.size() > 0 && m_words2.size() > 0)
	{
		i = 0;
		bw1 = 0;
		while (i < m_words2.size())
		{
			w2 = 0;
			if (bw1 >= m_words1.size())
				break;
			// Are we looking for a spacebreak, so just look for word->bBreak
			if (IsSpace(*m_words2[i]))
				w1 = FindNextSpaceInWords1(bw1);
			else
				w1 = FindNextMatchInWords1(*m_words2[i], bw1);
			// Found at same position, so go on wuth next word 
			if (w1 == bw1)
			{
				bw1++;
				i++;
				continue;
			}
			w2 = w1;
			// Not found, not same, check whitch distance is shorter
			if (w1 == -1 || (w1 - bw1) > 0)
			{
				w2 = FindNextMatchInWords2(*m_words1[i], bw1);
				// Execption both are not found in other string
				// so all between keep as a differ
				if (w1 == -1 && w2 == -1)
				{
					bw1++;
					i++;
					continue;
				}
				// Check if in distance 2 and 3 is equal again
				if (w2 == -1)
				{
					if (i + 2 < m_words1.size() && (i + 2 < m_words2.size()))
					{
						if (AreWordsSame(*m_words1[i + 1], *m_words2[i + 1]))
						{
							if (AreWordsSame(*m_words1[i + 2], *m_words2[i + 2]))
							{
								// Ok than keep it as a differ
								bw1 = bw1 + 2;
								i = i + 2;
								continue;
							}
						}
					}
				}
			}

			// Not found in word1, or distance from word1 to word2 is shorter
			// or too far away from both
			if ((w1 == -1) || (w1 < w2) || (w2 - bw1 > 4))
			{
				// Not found in word1 or to far away
				// so insert an empty record in word1 
				// at actual position
				// remember last start end
				if (bw1)
				{
					// Last word is a insert? So use same positions
					if (m_words1[bw1 - 1]->bBreak == -2)
					{
						end = m_words1[bw1 - 1]->end;
						start =  m_words1[bw1 - 1]->start;
					}
					else
					{
						// First insert, use endpos as start
						start = m_words1[bw1 - 1]->end + 1;
						end = start - 1;
					}
				}
				else
				{
					// At first position allways start at 0
					end = -1;
					start = 0;
				}

				vector<word*>::iterator iter = m_words1.begin() + bw1;
				word *wd  = new word(start, end, -2, 0);
				m_words1.insert(iter, wd);
				i = i + 1; // Next record
				bw1 = bw1 + 1; 
				continue;
			}
			else if (w1 < i)
			{
				int k = i- w1;
				for (int l = 0; l < k; l++)
				{
					// Remember last start end
					if (w1)
					{
						end = m_words1[w1 - 1]->end;
					}
					else
					{
						end = -1;
					}
					start = end + 1;
					vector<word*>::iterator iter = m_words1.begin() + w1;
					word *wd  = new word(start, end, -2, 0);
					m_words1.insert(iter, wd);
				}
				i =i + k; // Next record
				bw1 = w1 + k;
				continue;
			}
			else if (w1 > i)
			{
				int k = w1 - i;

				for (int l = 0; l < k; l++)
				{
					// Remember last start end
					if (i)
					{
						end = m_words2[i - 1]->end;
					}
					else
					{
						end = -1;
					}
					start = end + 1;
					vector<word*>::iterator iter = m_words2.begin() + i;
					word *wd  = new word(start, end, -2, 0);
					m_words2.insert(iter, wd);
				}
				bw1 = w1 + 1;
				i = i + k + 1; // Next record
				continue;
			}
			else if (w1 == i)
			{
				bw1++;
				i++;
			}
		}
	}

	// If needed, this is the place to merge from back again
	// to get more acurated matches
	if (m_words1.size() > 0 || m_words2.size() > 0)
	{
		// Make both array to same length
		i = 0; 
		int length1 = m_words1.size();
		int length2 = m_words2.size();

		if (length1 == 0 || length2 == 0)
			i = i;
		if (length1 < length2)
		{
			end =  m_str1.length() - 1;
			start = end + 1;
			word *wd  = new word(start, end, -2, 0);
			m_words1.push_back(wd);
			m_words2[length1]->end = m_words2[length2 - 1]->end;
			while ((length1 + 1) < m_words2.size())
			{
				delete m_words2.back();
				m_words2.pop_back();
			}
		}
		if (length1 > length2)
		{
			end =  m_str2.length() - 1;
			start = end + 1;
			word *wd  = new word(start, end, -2, 0);
			m_words2.push_back(wd);
			m_words1[length2]->end = m_words1[length1 - 1]->end;
			while ((length2 + 1) < m_words1.size())
			{
				delete m_words1.back();
				m_words1.pop_back();
			}
		}
		// Remove empty records on both side
#ifdef STRINGDIFF_LOGGING
		OutputDebugString(_T("remove empty records on both side \n"));
#endif
		i = 0; 
		while ((i < m_words1.size()) && (i < m_words2.size()))
		{
#ifdef STRINGDIFF_LOGGING
			wsprintf(buf, _T("left=%d, op=%d, right=%d, op=%d\n"),
				m_words1[i]->hash, m_words1[i]->bBreak, m_words2[i]->hash, m_words2[i]->bBreak);
			OutputDebugString(buf);
#endif

			if (IsInsert(*m_words1[i]) && AreWordsSame(*m_words1[i], *m_words2[i]))
			{
				RemoveItem1(i);
				RemoveItem2(i);
				continue;
			}
			i++;
		}

		// Remove empty records on both side
		// in case on the otherside +1 is also an empty one
		i = 0;  
		while ((i < m_words1.size() - 1) && (i < m_words2.size() - 1))
		{
			if (IsInsert(*m_words1[i]) && IsInsert(*m_words2[i + 1]))
			{
				RemoveItem1(i);
				RemoveItem2(i + 1);
				continue;
			}
			if (IsInsert(*m_words1[i + 1]) && IsInsert(*m_words2[i]))
			{
				RemoveItem1(i + 1);
				RemoveItem2(i);
				continue;
			}
			i++;
		}	

		// remove equal records on both side
		// even diff is close together
		i = 0; 
		while ((i < m_words1.size() - 1) && (i < m_words2.size() - 1))
		{
			if  (AreWordsSame(*m_words1[i], *m_words2[i]))
			{
				RemoveItem1(i);
				RemoveItem2(i);
				continue;
			}
			i++;
		}

		// inore all diff in whitespace
		// so remove same records
		if (m_whitespace != 0)
		{
			i = 0; 
			while ((i < m_words1.size()) && (i < m_words2.size()))
			{
				if (IsSpace(*m_words1[i]) && IsSpace(*m_words2[i]) )
				{
					RemoveItem1(i);
					RemoveItem2(i);
					continue;
				}
				i++;
			}
		}

		// Now lets connect inserts to one if same words 
		// are also words at a block
		// doit for word1
		i = 0;
		while (m_words1.size() > i + 2 )
		{
			if (IsInsert(*m_words1[i]) && IsInsert(*m_words1[i+1]))
			{
				if ((m_words2[i]->end +1 ) == (m_words2[i + 1]->start))
				{
					m_words2[i]->end = m_words2[i + 1]->end;
					RemoveItem1(i + 1);
					RemoveItem2(i + 1);
					continue;
				}
			}
			i++;
		}
		// Doit for word2
		i = 0;
		while (m_words2.size() > i + 2 )
		{
			if (IsInsert(*m_words2[i]) && IsInsert(*m_words2[i+1]))
			{
				if ((m_words1[i]->end +1 ) == (m_words1[i + 1]->start))
				{
					m_words1[i]->end = m_words1[i + 1]->end;
					RemoveItem1(i + 1);
					RemoveItem2(i + 1);
					continue;
				}
			}
			i++;
		}

		// Final run create diff
#ifdef STRINGDIFF_LOGGING
		OutputDebugString(_T("final run create diff \n"));
		wsprintf(buf, _T("left=  %d,   right=  %d \n"),
					m_str1.length(), m_str2.length());
		OutputDebugString(buf);		
#endif
		// Be aware, do not create more wdiffs as shortest line has chars!
		int imaxcount = min(m_str1.length(), m_str2.length());
		i = 0;
		while ((i < m_words1.size()) && (i < m_words2.size()) && (m_wdiffs.size() <= imaxcount))
		{
			if (!AreWordsSame(*m_words1[i], *m_words2[i]))
			{
				int s1 =  m_words1[i]->start;
				int e1 =  m_words1[i]->end;
				int len1 = e1 - s1 + 1;
				int s2 =  m_words2[i]->start;
				int e2 =  m_words2[i]->end ;
				int len2 = e2 - s2 + 1;

#ifdef STRINGDIFF_LOGGING
				if (IsInsert(*m_words1[i]))
					str1.clear();
				else
				{
					if (len1 < 50)
						str1 = m_str1.substr(s1 ,e1 - s1 + 1);
					else
						str1 = m_str1.substr(s1, 50);
				}

				if (IsInsert(*m_words2[i]))
					str2.clear();
				else
				{
					if (len2 < 50)
						str2 = m_str2.substr(s2, e2- s2 + 1);
					else
						str2 = m_str2.substr(s2, 50);
				}

				wsprintf(buf, _T("left=  %s,   %d,%d \n, right=  %s,   %d,%d \n"),
					str1.c_str(), s1, e1, str2.c_str(), s2, e2);
				OutputDebugString(buf);

				wsprintf(buf, _T("left=%d , op=%d, right=%d, op=%d\n"),
					m_words1[i]->hash, m_words1[i]->bBreak, m_words2[i]->hash, m_words2[i]->bBreak);
				OutputDebugString(buf);

#endif
				wdiff *wdf = new wdiff(s1, e1, s2, e2);
				m_wdiffs.push_back(wdf);
			}
			i++;
		}
	}
}
/**
 * @brief Find next word in m_words2 (starting at bw2) that matches needword1 (in m_words1)
 */
int 
stringdiffs::FindNextMatchInWords2(const word & needword1, int bw2) const
{
	while (bw2 < m_words2.size())
	{
		if (AreWordsSame(needword1, *m_words2[bw2]))
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
	while (bw1 < m_words1.size())
	{
		if (AreWordsSame(*m_words1[bw1], needword2))
			return bw1;
		++bw1;
	}
	return -1;
}
/**
 * @brief Find next space in m_words1 (starting at bw1)
 */
int 
stringdiffs::FindNextSpaceInWords1(int bw1) const
{
	while (bw1 < (int) m_words1.size())
	{
		if (IsSpace(*m_words1[bw1]))
			return bw1;
		++bw1;
	}
	return -1;
}
/**
 * @brief erase an item in m_words1
 */
bool
stringdiffs::RemoveItem1(int bw1)
{
	if (m_words1.size()== bw1 + 1)
	{
		delete m_words1.back();
		m_words1.pop_back();
	}
	else
	{
		vector<word*>::iterator iter = m_words1.begin() + bw1 ;
		delete *iter;
		*m_words1.erase(iter);
	}
	return true;
}
/**
 * @brief erase an item in m_words2
 */
bool
stringdiffs::RemoveItem2(int bw1)
{
	if (m_words2.size()== bw1 + 1)
	{
		delete m_words2.back();
		m_words2.pop_back();
	}
	else
	{
		vector<word*>::iterator iter = m_words2.begin() + bw1 ;
		delete *iter;
		*m_words2.erase(iter);
	}
	return true;
}
/**
 * @brief Break line into constituent words
 */
void
stringdiffs::BuildWordsArray(const String & str, vector<word*> * words)
{
	int i=0, begin=0;

	// state when we are looking for next word
inspace:
	if (isSafeWhitespace(str[i])) 
	{
		++i;
		goto inspace;
	}
	if (begin < i)
	{
		// just finished a word
		// e is first word character (space or at end)
		int e = i - 1;

		word *wd  = new word(begin, e, -1, Hash(str, begin, e, 0));
		words->push_back(wd);
	}
	if (i == str.length())
		return;
	begin = i;
	goto inword;

	// state when we are inside a word
inword:
	bool atspace=false;
	if (i == str.length() || ((atspace = isSafeWhitespace(str[i])) != 0) || isWordBreak(m_breakType, str[i]))
	{
		if (begin<i)
		{
			// just finished a word
			// e is first non-word character (space or at end)
			int e = i-1;
			
			word *wd  = new word(begin, e, 0, Hash(str, begin, e, 0));
			words->push_back(wd);
		}
		if (i == str.length())
		{
			return;
		}
		else if (atspace)
		{
			begin = i;
			goto inspace;
		}
		else
		{
			// start a new word because we hit a non-whitespace word break (eg, a comma)
			// but, we have to put each word break character into its own word
			word *wd  = new word(i, i, 1, Hash(str, i, i, 0));
			words->push_back(wd);
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
	for (int i=0; i<m_wdiffs.size(); ++i)
	{
		bool skipIt = false;
		// combine it with next ?
		if (i+1<m_wdiffs.size())
		{
			if (m_wdiffs[i]->end[0] == m_wdiffs[i+1]->start[0]
				&& m_wdiffs[i]->end[1] == m_wdiffs[i+1]->start[1])
			{
				// diff[i] and diff[i+1] are contiguous
				// so combine them into diff[i+1] and ignore diff[i]
				m_wdiffs[i+1]->start[0] = m_wdiffs[i]->start[0];
				m_wdiffs[i+1]->start[1] = m_wdiffs[i]->start[1];
				skipIt = true;
			}
		}
		if (!skipIt)
		{
			// Should never have a pair where both are missing
			assert(m_wdiffs[i]->start[0]>=0 || m_wdiffs[i]->start[1]>=0);

			// Store the diff[i] in the caller list (m_pDiffs)
			wdiff *dr = new wdiff(*m_wdiffs[i]);
			m_pDiffs->push_back(dr);
		}
	}
}

// diffutils hash

/* Rotate a value n bits to the left. */
#define UINT_BIT (sizeof (unsigned) * CHAR_BIT)
#define ROL(v, n) ((v) << (n) | (v) >> (UINT_BIT - (n)))
/* Given a hash value and a new character, return a new hash value. */
#define HASH(h, c) ((c) + ROL (h, 7))

UINT
stringdiffs::Hash(const String & str, int begin, int end, UINT h) const
{
	for (int i = begin; i <= end; ++i)
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
	if (m_whitespace == 2)
		if (word1.hash == word2.hash)
			return true;
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
 * @brief Is this block an space or whitespace one
 */
bool
stringdiffs::IsSpace(const word & word1) const
{
	return (word1.bBreak == -1);
}
/**
 * @brief Is this block an empty (insert) one
 */
bool
stringdiffs::IsInsert(const word & word1) const
{
	return (word1.bBreak == -2);
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
	if (!breakType)
		return false;
	// breakType==1 means break also on punctuation
	return _tcschr(BreakChars, ch) != 0;
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
		if (prev == psz)
			psz++;
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
sd_ComputeByteDiff(String & str1, String & str2, 
		   bool casitive, int xwhite, 
		   int &begin1, int &begin2, int &end1, int &end2)
{
	// Set to sane values
	// Also this way can distinguish if we set begin1 to -1 for no diff in line
	begin1 = end1 = begin2 = end2 = 0;

	int len1 = str1.length();
	int len2 = str2.length();

	LPCTSTR pbeg1 = str1.c_str();
	LPCTSTR pbeg2 = str2.c_str();

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
 * @brief adjust the range of the specified word diffs down to byte(char) level.
 * @param str1, str2 [in] line to be compared
 * @param casitive [in] true for case-sensitive, false for case-insensitive
 * @param xwhite [in] This governs whether we handle whitespace specially
 *  (see WHITESPACE_COMPARE_ALL, WHITESPACE_IGNORE_CHANGE, WHITESPACE_IGNORE_ALL)
 */
static void wordLevelToByteLevel(vector<wdiff*> * pDiffs, const String& str1,
		const String& str2, bool casitive, int xwhite)
{
	for (int i = 0; i < pDiffs->size(); i++)
	{
		int begin1, begin2, end1, end2;
		wdiff *pDiff = (*pDiffs)[i];
		String str1_2, str2_2;
		str1_2 = str1.substr(pDiff->start[0], pDiff->end[0] - pDiff->start[0] + 1);
		str2_2 = str2.substr(pDiff->start[1], pDiff->end[1] - pDiff->start[1] + 1);
		sd_ComputeByteDiff(str1_2, str2_2, casitive, xwhite, begin1, begin2, end1, end2);
		if (begin1 == -1)
		{
			// no visible diff on side1
			pDiff->end[0] = pDiff->start[0] - 1;
		}
		else
		{
			pDiff->end[0] = pDiff->start[0] + end1;
			pDiff->start[0] += begin1;
		}
		if (begin2 == -1)
		{
			// no visible diff on side2
			pDiff->end[1] = pDiff->start[1] - 1;
		}
		else
		{
			pDiff->end[1] = pDiff->start[1] + end2;
			pDiff->start[1] += begin2;
		}
	}
}

/**
 * @brief Check if first side is identical.
 * @param [in] worddiffs Diffs from sd_ComputeWordDiffs.
 * @param [in] nLineLength Length of the string.
 * @return true if first side is identical.
 */
bool IsSide0Empty(vector<wdiff*> worddiffs, int nLineLength)
{
	return (worddiffs[0]->end[0] == -1 && worddiffs[0]->start[1] == 0 &&
			worddiffs[0]->end[1] + 1 == nLineLength);
}
/**
 * @brief Check if second side is identical.
 * @param [in] worddiffs Diffs from sd_ComputeWordDiffs.
 * @param [in] nLineLength Length of the string.
 * @return true if second side is identical.
 */
bool IsSide1Empty(vector<wdiff*> worddiffs, int nLineLength)
{
	return (worddiffs[0]->end[1] == -1  && worddiffs[0]->start[0] == 0 &&
			worddiffs[0]->end[0] + 1 == nLineLength);
}
