/** 
 * @file  stringdiffs.cpp
 *
 * @brief Implementation file for sd_ComputeWordDiffs (q.v.)
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id: stringdiffs.cpp 7149 2010-05-03 17:08:20Z kimmov $

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
static bool isWordBreak(int breakType, const TCHAR *str, int index);
static int make3wayDiff(vector<wdiff*> &diff3, vector<wdiff*> &diff10, vector<wdiff*> &diff12);

void sd_Init()
{
	BreakChars = &BreakCharDefaults[0];
	Initialized = true;
}

void sd_Close()
{
	if (CustomChars)
	{
		free(BreakChars);
		BreakChars = NULL;
		CustomChars = false;
	}
	Initialized = false;
}

void sd_SetBreakChars(const TCHAR *breakChars)
{
	assert(Initialized);

	if (CustomChars)
		free(BreakChars);

	CustomChars = true;
	BreakChars = _tcsdup(breakChars);
}

void
sd_ComputeWordDiffs(const String str1, const String str2,
	bool case_sensitive, int whitespace, int breakType, bool byte_level,
	vector<wdiff*> * pDiffs)
{
	String strs[3] = {str1, str2, _T("")};
	sd_ComputeWordDiffs(2, strs, case_sensitive, whitespace, breakType, byte_level, pDiffs);
}

/**
 * @brief Construct our worker object and tell it to do the work
 */
void
sd_ComputeWordDiffs(int nFiles, const String str[3],
	bool case_sensitive, int whitespace, int breakType, bool byte_level,
	vector<wdiff*> * pDiffs)
{
	if (nFiles == 2)
	{
		stringdiffs sdiffs(str[0], str[1], case_sensitive, whitespace, breakType, pDiffs);
		// Hash all words in both lines and then compare them word by word
		// storing differences into m_wdiffs
		sdiffs.BuildWordDiffList();

		if (byte_level)
			sdiffs.wordLevelToByteLevel();

		// Now copy m_wdiffs into caller-supplied m_pDiffs (coalescing adjacents if possible)
		sdiffs.PopulateDiffs();

	}
	else
	{
		if (str[0].empty())
		{
			stringdiffs sdiffs(str[1], str[2], case_sensitive, whitespace, breakType, pDiffs);
			sdiffs.BuildWordDiffList();
			if (byte_level)
				sdiffs.wordLevelToByteLevel();
			sdiffs.PopulateDiffs();
			for (int i = 0; i < pDiffs->size(); i++)
			{
				wdiff *pDiff = (*pDiffs)[i];
				pDiff->begin[2] = pDiff->begin[1];
				pDiff->begin[1] = pDiff->begin[0];
				pDiff->begin[0] = 0;
				pDiff->end[2] = pDiff->end[1];
				pDiff->end[1] = pDiff->end[0];
				pDiff->end[0] = -1;
			}
		}
		else if (str[1].empty())
		{
			stringdiffs sdiffs(str[0], str[2], case_sensitive, whitespace, breakType, pDiffs);
			sdiffs.BuildWordDiffList();
			if (byte_level)
				sdiffs.wordLevelToByteLevel();
			sdiffs.PopulateDiffs();
			for (int i = 0; i < pDiffs->size(); i++)
			{
				wdiff *pDiff = (*pDiffs)[i];
				pDiff->begin[2] = pDiff->begin[1];
				pDiff->begin[0] = pDiff->begin[0];
				pDiff->begin[1] = 0;
				pDiff->end[2] = pDiff->end[1];
				pDiff->end[0] = pDiff->end[0];
				pDiff->end[1] = -1;
			}
		}
		else if (str[2].empty())
		{
			stringdiffs sdiffs(str[0], str[1], case_sensitive, whitespace, breakType, pDiffs);
			sdiffs.BuildWordDiffList();
			if (byte_level)
				sdiffs.wordLevelToByteLevel();
			sdiffs.PopulateDiffs();
			for (int i = 0; i < pDiffs->size(); i++)
			{
				wdiff *pDiff = (*pDiffs)[i];
				pDiff->begin[1] = pDiff->begin[1];
				pDiff->begin[0] = pDiff->begin[0];
				pDiff->begin[2] = 0;
				pDiff->end[1] = pDiff->end[1];
				pDiff->end[0] = pDiff->end[0];
				pDiff->end[2] = -1;
			}
		}
		else
		{
			vector<wdiff*> diffs10, diffs12, diffs02;
			stringdiffs sdiffs10(str[1], str[0], case_sensitive, whitespace, breakType, &diffs10);
			stringdiffs sdiffs12(str[1], str[2], case_sensitive, whitespace, breakType, &diffs12);
			// Hash all words in both lines and then compare them word by word
			// storing differences into m_wdiffs
			sdiffs10.BuildWordDiffList();
			sdiffs12.BuildWordDiffList();
			if (byte_level)
			{
				sdiffs10.wordLevelToByteLevel();
				sdiffs12.wordLevelToByteLevel();
			}
			// Now copy m_wdiffs into caller-supplied m_pDiffs (coalescing adjacents if possible)
			sdiffs10.PopulateDiffs();
			sdiffs12.PopulateDiffs();

			make3wayDiff(*pDiffs, diffs10, diffs12);
			while (!diffs10.empty())
			{
				delete diffs10.back();
				diffs10.pop_back();
			}
			while (!diffs12.empty())
			{
				delete diffs12.back();
				diffs12.pop_back();
			}
		}
	}
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

#ifdef STRINGDIFF_LOGGING
void
stringdiffs::debugoutput()
{
	for (int i = 0; i < m_wdiffs.size(); i++)
	{
		String str1;
		String str2;
		TCHAR buf[256];
		int s1 = m_wdiffs[i]->begin[0];
		int e1 = m_wdiffs[i]->end[0];
		int s2 = m_wdiffs[i]->begin[1];
		int e2 = m_wdiffs[i]->end[1];

		int len1 = e1 - s1 + 1;
		int len2 = e2 - s2 + 1;

		if (len1 < 50)
			str1 = m_str1.substr(s1 ,e1 - s1 + 1);
		else
			str1 = m_str1.substr(s1, 50);

		if (len2 < 50)
			str2 = m_str2.substr(s2, e2- s2 + 1);
		else
			str2 = m_str2.substr(s2, 50);

		wsprintf(buf, _T("left=  %s,   %d,%d,\nright= %s,   %d,%d \n"),
			str1.c_str(), s1, e1, str2.c_str(), s2, e2);
		OutputDebugString(buf);
	}
}
#endif

bool
stringdiffs::BuildWordDiffList_DP()
{
	std::vector<char> edscript;

	//if (dp(edscript) <= 0)
	//	return false;
	onp(edscript);

	const int iSize1 = (int)m_words1.size();
	const int iSize2 = (int)m_words2.size();
	int i = 1, j = 1;
	for (int k = 0; k < edscript.size(); k++)
	{
		int s1, e1, s2, e2;
		if (edscript[k] == '-')
		{
			if (m_whitespace == WHITESPACE_IGNORE_ALL)
			{
				if (IsSpace(*m_words1[i]))
				{
					i++;
					continue;
				}
			}
				
			s1 = m_words1[i]->start;
			e1 = m_words1[i]->end;
			s2 = m_words2[j-1]->end+1;
			e2 = s2-1;
			wdiff *wdf = new wdiff(s1, e1, s2, e2);
			m_wdiffs.push_back(wdf);
			i++;
		}
		else if (edscript[k] == '+')
		{
			if (m_whitespace == WHITESPACE_IGNORE_ALL)
			{
				if (IsSpace(*m_words2[j]))
				{
					j++;
					continue;
				}
			}

			s1 = m_words1[i-1]->end+1;
			e1 = s1-1;
			s2 = m_words2[j]->start;
			e2 = m_words2[j]->end;
			wdiff *wdf = new wdiff(s1, e1, s2, e2);
			m_wdiffs.push_back(wdf);
			j++;
		}
		else if (edscript[k] == '!')
		{
			if (m_whitespace == WHITESPACE_IGNORE_CHANGE || m_whitespace == WHITESPACE_IGNORE_ALL)
			{
				if (IsSpace(*m_words1[i]) && IsSpace(*m_words2[j]))
				{
					i++; j++;
					continue;
				}
			}
				
			s1 =  m_words1[i]->start;
			e1 =  m_words1[i]->end;
			s2 =  m_words2[j]->start;
			e2 =  m_words2[j]->end ;
			wdiff *wdf = new wdiff(s1, e1, s2, e2);
			m_wdiffs.push_back(wdf);
			i++; j++;
		}
		else
		{
			i++; j++;
		}
	}
#ifdef STRINGDIFF_LOGGING
	debugoutput();
#endif
	return true;
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

	if (m_words1.size() > 2048 || m_words2.size() > 2048)
	{
		int s1 = m_words1[0]->start;
		int e1 = m_words1[m_words1.size() - 1]->end;
		int s2 = m_words2[0]->start;
		int e2 = m_words2[m_words2.size() - 1]->end;
		wdiff *wdf = new wdiff(s1, e1, s2, e2);
		m_wdiffs.push_back(wdf);		
		return;
	}

	if (BuildWordDiffList_DP())
		return;

	int i = 0; // Number of fixed records of word2
	int start = 0;
	int end = 0;
#ifdef STRINGDIFF_LOGGING
	String str1;
	String str2;
	TCHAR buf[256];
#endif
	// If we have to ignore all whitespace change,
	// just remove leading and ending if one is there
	if (m_whitespace == WHITESPACE_IGNORE_ALL && m_matchblock)
	{
		// Remove a leading whitespace
		if (((int) m_words1.size() > 0) && (IsSpace(*m_words1[0])))
		{
			RemoveItem1(0);
		}
		if ((int)m_words1.size() > 0)
		{
			// Remove a ending whitespace
			if (IsSpace(*m_words1[m_words1.size() - 1]))
				RemoveItem1(m_words1.size() - 1);
		}
		// Remove a leading whitespace
		if (((int)m_words2.size() > 0) && (IsSpace(*m_words2[0])))
		{
			RemoveItem2(0);
		}
		if ((int)m_words2.size() > 0)
		{
			// Remove a ending whitespace
			if (IsSpace(*m_words2[m_words2.size() - 1]))
				RemoveItem2(m_words2.size() - 1);
		}
	}
	// Look for a match of word2 in word1
	// not found put an empty record at beginn of word1
	// if distance to far, crosscheck match word1 in word2
	// whatever is shorter, it's result.
	int w1 = 0, w2 = 0;		// position found in array
	int bw1 = 0;			// start position in m_words1
	int bw2 = 0;			// start position in m_words2
	bool lbreak = false;	// repeat
	if (m_matchblock && (int)m_words1.size() > 0 && (int)m_words2.size() > 0)
	{
		bw1 = 0;
		bw2 = 0;
		while (bw2 < (int)m_words2.size())
		{
			w2 = 0;
			lbreak = false;
			if (bw1 >= (int)m_words1.size())
				break;
			if (m_whitespace == WHITESPACE_IGNORE_ALL)
			{
				if (IsSpace(*m_words1[bw1]))
				{
					RemoveItem1(bw1);
					lbreak = true;
				}
				if (IsSpace(*m_words2[bw2]))
				{
					RemoveItem2(bw2);
					lbreak = true;
				}
				if (lbreak)
					continue;
			}
			// Are we looking for a spacebreak, so just look for word->bBreak
			if (IsSpace(*m_words2[bw2]))
				w1 = FindNextSpaceInWords1(bw1);
			else
				w1 = FindNextMatchInWords1(*m_words2[bw2], bw1);
			// Found at same position, so go on with next word 
			if (w1 == bw1)
			{
				bw1++;
				bw2++;
				continue;
			}
			w2 = w1;
			// Not found, not same, check whitch distance is shorter
			if (w1 == -1 || (w1 - bw1) > 0)
			{
				// Are we looking for a spacebreak, so just look for word->bBreak
				if (IsSpace(*m_words1[bw1]))
					w2 = FindNextSpaceInWords2(bw2);
				else
					w2 = FindNextMatchInWords2(*m_words1[bw1], bw2);
				// Execption both are not found in other string
				// so all between keep as a differ
				if (w1 == -1 && w2 == -1)
				{
					// check if it would be same if we remove the spaceblock
					// we must only ckeck on the side with the shortes word
					if ((m_words1[bw1]->end - m_words1[bw1]->start) > (m_words2[bw2]->end - m_words2[bw2]->start))
					{
						// first we check for size word2 and next word as space
						if ((bw2 < (int)m_words2.size() - 2) && IsSpace(*m_words2[bw2 + 1]))
						{
							// Are the contents same
							if (m_words1[bw1]->hash ==
								Hash(m_str2, m_words2[bw2 + 2]->start, m_words2[bw2 + 2]->end, m_words2[bw2]->hash))
							{
								m_words2[bw2]->end = m_words2[bw2 + 2]->end;
								m_words2[bw1]->hash = m_words1[bw1]->hash;
								// Now remove the detected blocks on side2.
								RemoveItem2(bw2 + 1);
								RemoveItem2(bw2 + 1);
								bw1++;
								bw2++;
								continue;
							}
						}
					}
					else if ((m_words1[bw1]->end - m_words1[bw1]->start) < (m_words2[bw2]->end - m_words2[bw2]->start))
					{
						// first we check for size  word1 and next word as space
						if ((bw1 < (int)m_words1.size() - 2) && IsSpace(*m_words1[bw1 + 1]))
						{
							// Are the contents same
							if (m_words2[bw2]->hash ==
								Hash(m_str1, m_words1[bw1 + 2]->start, m_words1[bw1 + 2]->end, m_words1[bw1]->hash))
							{
								m_words1[bw1]->end = m_words1[bw1 + 2]->end;
								m_words1[bw1]->hash = m_words2[bw1]->hash;
								// Now remove the detected blocks on side2.
								RemoveItem1(bw1 + 1);
								RemoveItem1(bw1 + 1);
								bw1++;
								bw2++;
								continue;
							}
						}
					}
					// Otherwise keep as diff
					bw1++;
					bw2++;
					continue;
				}
				// Not found on one side, so check if we are synchron again on
				// next words (try not to check breaks)
				if ((w1 == -1) || (w2 == -1))
				{
					// In relation to words.size()
					// Check if in distance 2 and (3 or 4) is equal again
					if (bw1 + 4 < (int)m_words1.size() && (bw2 + 4 < (int)m_words2.size()))
					{
						if (AreWordsSame(*m_words1[bw1 + 2], *m_words2[bw2 + 2]))
						{
							if (AreWordsSame(*m_words1[bw1 + 3], *m_words2[bw2 + 3]) || AreWordsSame(*m_words1[bw1 + 4], *m_words2[bw2 + 4]))
							{
								// Ok than keep it as a differ
								bw1 = bw1 + 2;
								bw2 = bw2 + 2;
								continue;
							}
						}
					}
					// Check if in distance 2 is equal again
					else if (bw1 + 2 < (int)m_words1.size() && (bw2 + 2 < (int)m_words2.size()))
					{
						if (AreWordsSame(*m_words1[bw1 + 1], *m_words2[bw2 + 1]))
						{
							// Ok than keep it as a differ
							bw1 = bw1 + 2;
							bw2 = bw2 + 2;
							continue;
						}
					}
				}
			}
			// distance from word1 to word2 is too far away on both side
			// or too far away and not found on the other side
			const int maxDistance = 4;
			if (((w1 - bw1 > maxDistance) && (w2 - bw2 > maxDistance))
				|| ((w1 - bw1 > maxDistance) && (w2 == -1))
				|| ((w2 - bw2 > maxDistance) && (w1 == -1)))
			{
				// keep as diff
				bw1++;
				bw2++;
				continue;
			}
			else if ((w2 > bw2) && ((w1 == -1 || (w2 - bw2) < (w1 - bw1))))
			{
				// Numbers of inserts in word1
				int k = w2 - bw2;
				for (int l = 0; l < k; l++)
				{
					// Remember last start end
					if (bw1)
					{
						end = m_words1[bw1 - 1]->end;
					}
					else
					{
						end = -1;
					}
					start = end + 1;
					vector<word*>::iterator iter = m_words1.begin() + bw1;
					word *wd  = new word(start, end, dlinsert, 0);
					m_words1.insert(iter, wd);
				}
				bw1 = bw1 + k + 1;
				bw2 = bw2 + k + 1; // Next record
				continue;
			}
			else if (w1 > bw2)
			{
				// Numbers of inserts in word2			
				int k = w1 - bw2;
				for (int l = 0; l < k; l++)
				{
					// Remember last start end
					if (bw2)
					{
						end = m_words2[bw2 - 1]->end;
					}
					else
					{
						end = -1;
					}
					start = end + 1;
					vector<word*>::iterator iter = m_words2.begin() + bw2;
					word *wd  = new word(start, end, dlinsert, 0);
					m_words2.insert(iter, wd);
				}
				bw1 = bw1 + k + 1;
				bw2 = bw2 + k + 1; // Next record
				continue;
			}
			else if (w1 == bw2)
			{
				bw1++;
				bw2++;
			}
		}
	}

	// Make both array to same length
	if ((int) m_words1.size() > 0 || (int) m_words2.size() > 0)
	{
		if ((int)m_words1.size() != (int)m_words2.size())
		{
			i = 0;
			int length1 = (int)m_words1.size() - 1;
			int length2 = (int)m_words2.size() - 1;

			if (length1 < length2)
			{
				// Numbers of inserts in word1			
				int k = length2 - length1;
				// Remember last start end
				if (length1 == -1)
				{
					end = -1;
				}
				else
				{
					end = m_words1[length1]->end - 1;
				}
				start = end + 1;
				for (int l = 0; l < k; l++)
				{
					word *wd  = new word(start, end, dlinsert, 0);
					m_words1.push_back(wd);
				}
			}
			else if (length1 > length2)
			{
				// Numbers of inserts in word2			
				int k = length1 - length2;
				// Remember last start end
				if (length2 == -1)
				{
					end = -1;
				}
				else
				{
					end = m_words2[length2]->end - 1;
				}
				start = end + 1;
				for (int l = 0; l < k; l++)
				{
					word *wd  = new word(start, end, dlinsert, 0);
					m_words2.push_back(wd);
				}
			}
		}

		// Look for a match of word2 in word1
		// We do it from back to get more acurated matches
		w1 = 0, w2 = 0;		// position found in array
		bw1 = (int)m_words1.size() - 1;			// start position in m_words1
		bw2 = (int)m_words2.size() - 1;			// start position in m_words2
		if (m_matchblock && bw1 > 0 && bw2 > 0)
		{
			while (bw1 > 0 && bw2 > 0)
			{
				if (AreWordsSame(*m_words1[bw1], *m_words2[bw2])
					|| IsSpace(*m_words1[bw1]) || IsSpace(*m_words2[bw2]))
				{
					bw1--;
					bw2--;
					continue;
				}
				w1 = -2, w2 = -2;

				// Normaly we synchronise with a *word2 to a match in word1
				// If it is an Insert in word2 so look for a *word1 in word2
				if (IsInsert(*m_words2[bw2]))
					w2 = FindPreMatchInWords2(*m_words1[bw1], bw2);
				else
					w1 = FindPreMatchInWords1(*m_words2[bw2], bw1);
				// check all between are inserts
				// if so, so exchange the position
				if (w1 >= 0)
				{
					// Numbers of inserts in word1			
					const int k = bw1 - w1;
					int l = 0;
					for (l = 0; l < k; l++)
					{
						if (!IsInsert(*m_words1[bw1 - l]))
							break;
					}
					// all are inserts so k==0
					if ((k - l) == 0)
					{
						word *wd  = new word(m_words1[w1]->start, m_words1[w1]->end, m_words1[w1]->bBreak, m_words1[w1]->hash);
						RemoveItem1(w1);
						vector<word*>::iterator iter = m_words1.begin() + bw1;
						m_words1.insert(iter, wd);
						// Correct the start-end pointer
						const int istart = m_words1[bw1]->start;
						const int iend =istart - 1;
						bw1--;
						bw2--;
						for (l = 0; l < k ; l++)
						{
							m_words1[bw1 - l]->start = istart;
							m_words1[bw1 - l]->end = iend;	
						}
						continue;
					}
				}
				if (w2 >= 0)
				{
					// Numbers of inserts in word2			
					const int k = bw2 - w2;
					int l = 0;
					for (l = 0; l < k; l++)
					{
						if (!IsInsert(*m_words2[bw2 - l]))
							break;
					}
					// all are inserts so k==0
					if ((k - l) == 0)
					{
						word *wd  = new word(m_words2[w2]->start, m_words2[w2]->end, m_words2[w2]->bBreak, m_words2[w2]->hash);
						RemoveItem2(w2);
						vector<word*>::iterator iter = m_words2.begin() + bw2;
						m_words2.insert(iter, wd);
						// Correct the start-end pointer
						const int istart = m_words2[bw1]->start;
						const int iend = istart - 1;
						bw1--;
						bw2--;
						for (l = 0; l < k ; l++)
						{
							m_words2[bw2 - l]->start = istart;
							m_words2[bw2 - l]->end = iend;	
						}
						continue;
					}
				}
				// otherwise go on
				bw1--;
				bw2--;
				continue;
			}
		}
		// Remove empty records on both side
#ifdef STRINGDIFF_LOGGING
		OutputDebugString(_T("remove empty records on both side \n"));
#endif
		i = 0; 
		while ((i < (int)m_words1.size()) && (i < (int)m_words2.size()))
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
		while ((i < ((int)m_words1.size() - 2)) && (i < ((int)m_words2.size() - 2)))
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
		while ((i < (int) m_words1.size() - 1) && (i < (int) m_words2.size() - 1))
		{
			if  (AreWordsSame(*m_words1[i], *m_words2[i]))
			{
				RemoveItem1(i);
				RemoveItem2(i);
				continue;
			}
			i++;
		}

		// ignore all diff in whitespace
		// so remove same records
		if (m_whitespace != WHITESPACE_COMPARE_ALL)
		{
			i = 0; 
			while ((i < (int)m_words1.size()) && (i < (int)m_words2.size()))
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
		while ((int)m_words1.size() > i + 1)
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
		while ((int)m_words2.size() > i + 1)
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
		// Check last insert to be at string.length
		// protect to put last insert behind string for editor, otherwise
		// it shows last char double
		i = (int)m_words1.size() - 1;
		if (i >= 1)
		{
			if (IsInsert(*m_words1[i]) &&
				((m_words1[i]->start == m_str1.length() - 1)|| (m_words1[i]->start ==m_words1[i-1]->end)))
			{
				m_words1[i]->start = m_str1.length();
				m_words1[i]->end = m_words1[i]->start - 1;
			}
			else
			{
				if (IsInsert(*m_words2[i]) &&
				((m_words2[i]->start == m_str2.length() - 1)|| (m_words2[i]->start ==m_words2[i-1]->end)))
				{
					m_words2[i]->start = m_str2.length();
					m_words2[i]->end = m_words2[i]->start - 1;
				}
			}
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
		const int iSize1 = (int)m_words1.size();
		const int iSize2 = (int)m_words2.size();
		while ((i < iSize1) && (i < iSize2) && ((int)m_wdiffs.size() <= imaxcount))
		{
			if (!AreWordsSame(*m_words1[i], *m_words2[i]))
			{
				int s1 =  m_words1[i]->start;
				int e1 =  m_words1[i]->end;
				int s2 =  m_words2[i]->start;
				int e2 =  m_words2[i]->end ;

				wdiff *wdf = new wdiff(s1, e1, s2, e2);
				m_wdiffs.push_back(wdf);
			}
			i++;
		}
	}
#ifdef STRINGDIFF_LOGGING
	debugoutput();
#endif
}

/**
 * @brief Find pre word in m_words1 (starting at bw1) that matches needword2 (in m_words2)
 */
int 
stringdiffs::FindPreMatchInWords1(const word & needword2, int bw1) const
{
	while (bw1 >= 0)
	{
		if (AreWordsSame(*m_words1[bw1], needword2))
			return bw1;
		--bw1;
	}
	return -1;
}
/**
 * @brief Find next word in m_words1 (starting at bw1) that matches needword2 (in m_words2)
 */
int 
stringdiffs::FindNextMatchInWords1(const word & needword2, int bw1) const
{
	const int iSize = (int) m_words1.size();
	while (bw1 < iSize)
	{
		if (AreWordsSame(*m_words1[bw1], needword2))
			return bw1;
		++bw1;
	}
	return -1;
}

/**
 * @brief Find pre word in m_words2 (starting at bw2) that matches needword1 (in m_words1)
 */
int 
stringdiffs::FindPreMatchInWords2(const word & needword1, int bw2) const
{
	while (bw2 >= 0)
	{
		if (AreWordsSame(needword1, *m_words2[bw2]))
			return bw2;
		--bw2;
	}
	return -1;
}
/**
 * @brief Find next word in m_words2 (starting at bw2) that matches needword1 (in m_words1)
 */
int 
stringdiffs::FindNextMatchInWords2(const word & needword1, int bw2) const
{
	const int iSize = (int) m_words2.size();
	while (bw2 < iSize)
	{
		if (AreWordsSame(needword1, *m_words2[bw2]))
			return bw2;
		++bw2;
	}
	return -1;
}
/**
 * @brief Find pre space in m_words1 (starting at bw1)
 */
int 
stringdiffs::FindPreSpaceInWords1(int bw1) const
{
	while (bw1 >= 0)
	{
		if (IsSpace(*m_words1[bw1]))
			return bw1;
		--bw1;
	}
	return -1;
}

/**
 * @brief Find next space in m_words1 (starting at bw1)
 */
int 
stringdiffs::FindNextSpaceInWords1(int bw1) const
{
	const int iSize = (int) m_words1.size();
	while (bw1 < iSize)
	{
		if (IsSpace(*m_words1[bw1]))
			return bw1;
		++bw1;
	}
	return -1;
}
/**
 * @brief Find next space in m_words2 (starting at bw2)
 */
int 
stringdiffs::FindNextSpaceInWords2(int bw2) const
{
	const int iSize = (int) m_words2.size();
	while (bw2 < iSize)
	{
		if (IsSpace(*m_words2[bw2]))
			return bw2;
		++bw2;
	}
	return -1;
}
/**
 * @brief erase an item in m_words1
 */
bool
stringdiffs::RemoveItem1(int bw1)
{
	if ((int)m_words1.size()== bw1 + 1)
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
stringdiffs::RemoveItem2(int bw2)
{
	if ((int)m_words2.size()== bw2 + 1)
	{
		delete m_words2.back();
		m_words2.pop_back();
	}
	else
	{
		vector<word*>::iterator iter = m_words2.begin() + bw2 ;
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

	// dummy;
	words->push_back(new word(0, -1, 0, 0));

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

		word *wd  = new word(begin, e, dlspace, Hash(str, begin, e, 0));
			words->push_back(wd);
		}
	if (i == str.length())
		return;
	begin = i;
	goto inword;

	// state when we are inside a word
inword:
	bool atspace=false;
	if (i == str.length() || ((atspace = isSafeWhitespace(str[i])) != 0) || isWordBreak(m_breakType, str.c_str(), i))
	{
		if (begin<i)
		{
			// just finished a word
			// e is first non-word character (space or at end)
			int e = i-1;
			
			word *wd  = new word(begin, e, dlword, Hash(str, begin, e, 0));
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
			word *wd  = new word(i, i, dlbreak, Hash(str, i, i, 0));
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
	for (int i=0; i< (int)m_wdiffs.size(); ++i)
	{
		bool skipIt = false;
		// combine it with next ?
		if (i+1< (int)m_wdiffs.size())
		{
			if (m_wdiffs[i]->end[0] + 1 == m_wdiffs[i+1]->begin[0]
				&& m_wdiffs[i]->end[1] + 1 == m_wdiffs[i+1]->begin[1])
			{
				// diff[i] and diff[i+1] are contiguous
				// so combine them into diff[i+1] and ignore diff[i]
				m_wdiffs[i+1]->begin[0] = m_wdiffs[i]->begin[0];
				m_wdiffs[i+1]->begin[1] = m_wdiffs[i]->begin[1];
				skipIt = true;
			}
		}
		if (!skipIt)
		{
			// Should never have a pair where both are missing
			assert(m_wdiffs[i]->begin[0]>=0 || m_wdiffs[i]->begin[1]>=0);

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
	if (this->m_whitespace != WHITESPACE_COMPARE_ALL)
	{
		if (IsSpace(word1) && IsSpace(word2))
			return true;
	}
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
 * @brief Is this block a word one?
 */
bool
stringdiffs::IsWord(const word & word1) const
{
	return (word1.bBreak == dlword);
}

/**
 * @brief Is this block an space or whitespace one?
 */
bool
stringdiffs::IsSpace(const word & word1) const
{
	return (word1.bBreak == dlspace);
}
/**
 * @brief Is this block a break?
 */
bool
stringdiffs::IsBreak(const word & word1) const
{
	return (word1.bBreak == dlbreak || word1.bBreak == dlspace);
}

/**
 * @brief Is this block an empty (insert) one?
 */
bool
stringdiffs::IsInsert(const word & word1) const
{
	return (word1.bBreak == dlinsert);
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

int
stringdiffs::dp(std::vector<char> & edscript)
{
	unsigned char C[128][128];
	int i, j;
	int m = m_words1.size() - 1;
	int n = m_words2.size() - 1;

	if (m > 127 || n > 127)
		return -1;
	for (i = 0; i <= m; i++)
		C[i][0] = i;
	for (j = 0; j <= n; j++)
		C[0][j] = j;
	for (i = 1; i <= m; i++)
	{
		for (j = 1; j <= n; j++)
		{
			C[i][j] = min(
				min(
					C[i-1][j] + 1,
					C[i][j-1] + 1),
					C[i-1][j-1] + (AreWordsSame(*m_words1[i], *m_words2[j]) ? 0 : 1)	
				);
		}
	}

	std::vector<char> edscriptr;

	i = m; j = n;
	while (i >= 1 && j >= 1)
	{
		if (AreWordsSame(*m_words1[i], *m_words2[j]))
		{
			edscriptr.push_back('=');
			i--; j--;
		}
		else if (C[i][j] == C[i-1][j] + 1)
		{
			edscriptr.push_back('-');
			i--;
		}
		else if (C[i][j] == C[i-1][j-1] + 1)
		{
			edscriptr.push_back('!');
			i--; j--;
		}
		else
		{
			edscriptr.push_back('+');
			j--;
		}
	}
	if (i > 0)
	{
		while (i >= 1)
		{
			edscriptr.push_back('-');
			i--;
		}
	}
	if (j > 0)
	{
		while (j >= 1)
		{
			edscriptr.push_back('+');
			j--;
		}
	}
	edscript = std::vector<char>(edscriptr.rbegin(), edscriptr.rend());

	return C[m][n];
}

/**
 * @ brief An O(NP) Sequence Comparison Algorithm. Sun Wu, Udi Manber, Gene Myers
 */
int
stringdiffs::onp(std::vector<char> &edscript)
{
	int M = m_words1.size() - 1;
	int N = m_words2.size() - 1;
	bool exchanged = false;
	if (M > N)
	{
		M = m_words2.size() - 1;
		N = m_words1.size() - 1;
		exchanged = true;
	}
    int *fp = (new int[(M+1) + 1 + (N+1)]) + (M+1);
	std::vector<char> *es = (new std::vector<char>[(M+1) + 1 + (N+1)]) + (M+1);
    int DELTA = N - M;

    int k;
	for (k = -(M+1); k <= (N+1); k++)
		fp[k] = -1; 
	int p = -1;
    do
	{
		p = p + 1;
        for (k = -p; k <= DELTA-1; k++)
		{
			fp[k] = snake(k, max(fp[k-1] + 1, fp[k+1]), exchanged);

			es[k] = fp[k-1] + 1 > fp[k+1] ? es[k-1] : es[k+1];
			es[k].push_back(fp[k-1] + 1 > fp[k+1] ? '+' : '-');
			es[k].resize(es[k].size() + fp[k] - max(fp[k-1] + 1, fp[k+1]), '=');
		}
        for (k = DELTA + p; k >= DELTA+1; k--)
		{
			fp[k] = snake(k, max(fp[k-1] + 1, fp[k+1]), exchanged);

			es[k] = fp[k-1] + 1 > fp[k+1] ? es[k-1] : es[k+1];
			es[k].push_back(fp[k-1] + 1 > fp[k+1] ? '+' : '-');
			es[k].resize(es[k].size() + fp[k] - max(fp[k-1] + 1, fp[k+1]), '=');
		}
		k = DELTA;
		fp[k] = snake(k, max(fp[k-1] + 1, fp[k+1]), exchanged);

		es[k] = fp[k-1] + 1 > fp[k+1] ? es[k-1] : es[k+1];
		es[k].push_back(fp[k-1] + 1 > fp[k+1] ? '+' : '-');
		es[k].resize(es[k].size() + fp[k] - max(fp[k-1] + 1, fp[k+1]), '=');
    } while (fp[k] != N);

	std::vector<char> &ses = es[DELTA]; // Shortest edit script
	edscript.clear();

	int D = 0;
	for (int i = 1; i < ses.size(); i++)
	{
		switch (ses[i])
		{
		case '+':
			if (i + 1 < ses.size() && ses[i + 1] == '-')
			{
				edscript.push_back('!');
				i++;
				D++;
			}
			else
			{
				edscript.push_back(exchanged ? '-' : '+');
				D++;
			}
			break;
		case '-':
			if (i + 1 < ses.size() && ses[i + 1] == '+')
			{
				edscript.push_back('!');
				i++;
				D++;
			}
			else
			{
				edscript.push_back(exchanged ? '+' : '-');
				D++;
			}
			break;
		default:
			edscript.push_back('=');
		}
	}
		
	delete [] (es - (M+1));
	delete [] (fp - (M+1));

	return D;
}

int
stringdiffs::snake(int k, int y, bool exchanged)
{

	int M = exchanged ? m_words2.size() - 1 : m_words1.size() - 1;
	int N = exchanged ? m_words1.size() - 1 : m_words2.size() - 1;
	int x = y - k;
	while (x < M && y < N && (exchanged ? AreWordsSame(*m_words1[y + 1], *m_words2[x + 1]) : AreWordsSame(*m_words1[x + 1], *m_words2[y + 1]))) {
        x = x + 1; y = y + 1;
    }
    return y;
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
static inline bool
isSafeWhitespace(TCHAR ch)
{
	return xisspace(ch) && !IsLeadByte(ch);
}

/**
 * @brief Is it a non-whitespace wordbreak character (ie, punctuation)?
 */
static bool
isWordBreak(int breakType, const TCHAR *str, int index)
{
	TCHAR ch = str[index];
	// breakType==1 means break also on punctuation
#ifdef _UNICODE
	if ((ch & 0xff00) == 0)
	{
		TCHAR nextCh = str[index + 1];
		// breakType==0 means whitespace only
		if (!breakType)
			return false;
		return _tcschr(BreakChars, ch) != 0;
	}
	else 
	{
//		if (
//			ch==0xff0c/* Fullwidth Full Stop */ || 
//			ch==0xff0e/* Fullwidth Comma */ ||
//			ch==0xff1b/* Fullwidth Semicolon */ ||
//			ch==0xff1a/* Fullwidth Colon */ ||
//			ch==0x3002/* Ideographic Full Stop */ || 
//			ch==0x3001/* Ideographic Comma */
//			)
//			return true;
//		WORD wCharType, wCharTypeNext;
//		GetStringTypeW(CT_CTYPE3, &ch, 1, &wCharType);
//		TCHAR nextCh = str[index + 1];
//		GetStringTypeW(CT_CTYPE3, &nextCh, 1, &wCharTypeNext);
//		return (wCharType != wCharTypeNext);
//		
		return true;
	}
#else
	// breakType==0 means whitespace only
	if (!breakType)
		return false;
	return _tcschr(BreakChars, ch) != 0;
#endif
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
		   int begin[2], int end[2], bool equal)
{
	// Set to sane values
	// Also this way can distinguish if we set begin[0] to -1 for no diff in line
	begin[0] = end[0] = begin[1] = end[1] = 0;

	int len1 = str1.length();
	int len2 = str2.length();

	LPCTSTR pbeg1 = str1.c_str();
	LPCTSTR pbeg2 = str2.c_str();

	if (len1 == 0 || len2 == 0)
	{
		if (len1 == len2)
		{
			begin[0] = -1;
			begin[1] = -1;
		}
		end[0] = len1 - 1;
		end[1] = len2 - 1;
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
	//check for excaption of empty string on one side
	//In that case display all as a diff
	if (!equal && (((py1 == pen1) && isSafeWhitespace(*pen1)) ||
		((py2 == pen2) && isSafeWhitespace(*pen2))))
	{
		begin[0] = 0;
		begin[1] = 0;
		end[0] = len1 - 1;
		end[1] = len2 - 1;
		return;
	}
	// Advance over matching beginnings of lines
	// Advance py1 & py2 from beginning until find difference or end
	while (1)
	{
		// Potential difference extends from py1 to pen1 and py2 to pen2

		// Check if either side finished
		if (py1 > pen1 && py2 > pen2)
		{
			begin[0] = end[0] = begin[1] = end[1] = -1;
			break;
		}
		if (py1 > pen1 || py2 > pen2)
		{
			break;
		}

		// handle all the whitespace logic (due to WinMerge whitespace settings)
		if (xwhite && py1 < pen1 && isSafeWhitespace(*py1))
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
		if (xwhite && py2 < pen2 && isSafeWhitespace(*py2))
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

	// Store results of advance into return variables (begin[0] & begin[1])
	// -1 in a begin variable means no visible diff area
	begin[0] = py1 - pbeg1;
	begin[1] = py2 - pbeg2;

	LPCTSTR pz1 = pen1;
	LPCTSTR pz2 = pen2;

	// Retreat over matching ends of lines
	// Retreat pz1 & pz2 from end until find difference or beginning
	while (1)
	{
		// Check if either side finished
		if (pz1 < py1 && pz2 < py2)
		{
			begin[0] = end[0] = begin[1] = end[1] = -1;
			break;
		}
		if (pz1 < py1 || pz2 < py2)
		{
			break;
		}

		// handle all the whitespace logic (due to WinMerge whitespace settings)
		if (xwhite && pz1 > py1 && isSafeWhitespace(*pz1))
		{
			if (xwhite==1 && !isSafeWhitespace(*pz2))
				break; // done with reverse search
			// gobble up all whitespace in current area
			while (pz1 > py1 && isSafeWhitespace(*pz1))
				pz1 = CharPrev(py1, pz1);
			while (pz2 > py2 && isSafeWhitespace(*pz2))
				pz2 = CharPrev(py2, pz2);
			continue;

		}
		if (xwhite && pz2 > py2 && isSafeWhitespace(*pz2))
		{
			if (xwhite==1)
				break; // done with reverse search
			while (pz2 > py2 && isSafeWhitespace(*pz2))
				pz2 = CharPrev(py2, pz2);
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
		pz1 = (pz1 > pbeg1) ? CharPrev(pbeg1, pz1) : pz1 - 1;
		pz2 = (pz2 > pbeg2) ? CharPrev(pbeg2, pz2) : pz2 - 1;
    }

/*	if (*pz1 == '\r' && *(pz1+1) == '\n')
	{
		pz1++;
		pz2++;
	}
	else if (*pz2 == '\r' && *(pz2+1) == '\n')
	{
		pz2++;
		pz1++;
	}
	if (*(pbeg1-1) == '\r' && *pbeg1 == '\n')
	{
		pbeg1--;
		pbeg2--;
	}
	else if (*(pbeg2-1) == '\r' && *pbeg2 == '\n')
	{
		pbeg2--;
		pbeg1--;
	}*/

	// Store results of advance into return variables (end[0] & end[1])
	end[0] = pz1 - pbeg1;
	end[1] = pz2 - pbeg2;

	// Check if difference region was empty
	if (begin[0] == end[0] + 1 && begin[1] == end[1] + 1)
		begin[0] = -1; // no diff
}

/* diff3 algorithm. It is almost the same as GNU diff3's algorithm */
static int make3wayDiff(vector<wdiff*> &diff3, vector<wdiff*> &diff10, vector<wdiff*> &diff12)
{
	int diff10count = diff10.size();
	int diff12count = diff12.size();

	int diff10i = 0;
	int diff12i = 0;
	int diff3i = 0;

	int diff10itmp;
	int diff12itmp;

	bool lastDiffBlockIsDiff12;
	bool firstDiffBlockIsDiff12;

	wdiff *dr3, dr10, dr12, dr10first, dr10last, dr12first, dr12last;
	vector<wdiff*> diff3tmp;

	int linelast0 = 0;
	int linelast1 = 0;
	int linelast2 = 0;

	for (;;)
	{
		if (diff10i >= diff10count && diff12i >= diff12count)
			break;

		/* 
		 * merge overlapped diff blocks
		 * diff10 is diff blocks between file1 and file0.
		 * diff12 is diff blocks between file1 and file2.
		 *
		 *                      diff12
		 *                 diff10            diff3
		 *                 |~~~|             |~~~|
		 * firstDiffBlock  |   |             |   |
		 *                 |   | |~~~|       |   |
		 *                 |___| |   |       |   |
		 *                       |   |   ->  |   |
		 *                 |~~~| |___|       |   |
		 * lastDiffBlock   |   |             |   |
		 *                 |___|             |___|
		 */

		if (diff10i >= diff10count && diff12i < diff12count)
		{
			dr12first = *diff12.at(diff12i);
			dr12last = dr12first;
			firstDiffBlockIsDiff12 = true;
		}
		else if (diff10i < diff10count && diff12i >= diff12count)
		{
			dr10first = *diff10.at(diff10i);
			dr10last = dr10first;
			firstDiffBlockIsDiff12 = false;
		}
		else
		{
			dr10first = *diff10.at(diff10i);	
			dr12first = *diff12.at(diff12i);	
			dr10last = dr10first;
			dr12last = dr12first;

			if (dr12first.begin[0] <= dr10first.begin[0])
				firstDiffBlockIsDiff12 = true;
			else
				firstDiffBlockIsDiff12 = false;
		}
		lastDiffBlockIsDiff12 = firstDiffBlockIsDiff12;

		diff10itmp = diff10i;
		diff12itmp = diff12i;
		for (;;)
		{
			if (diff10itmp >= diff10count || diff12itmp >= diff12count)
				break;

			dr10 = *diff10.at(diff10itmp);
			dr12 = *diff12.at(diff12itmp);

			if (dr10.end[0] == dr12.end[0])
			{
				diff10itmp++;
				lastDiffBlockIsDiff12 = true;

				dr10last = dr10;
				dr12last = dr12;
				break;
			}

			if (lastDiffBlockIsDiff12)
			{
				if (dr12.end[0] + 1 < dr10.begin[0])
					break;
			}
			else
			{
				if (dr10.end[0] + 1 < dr12.begin[0])
					break;
			}

			if (dr12.end[0] > dr10.end[0])
			{
				diff10itmp++;
				lastDiffBlockIsDiff12 = true;
			}
			else
			{
				diff12itmp++;
				lastDiffBlockIsDiff12 = false;
			}

			dr10last = dr10;
			dr12last = dr12;
		}

		if (lastDiffBlockIsDiff12)
			diff12itmp++;
		else
			diff10itmp++;

		dr3 = new wdiff();
		if (firstDiffBlockIsDiff12)
		{
			dr3->begin[1] = dr12first.begin[0];
			dr3->begin[2] = dr12first.begin[1];
			if (diff10itmp == diff10i)
				dr3->begin[0] = dr3->begin[1] - linelast1 + linelast0;
			else
				dr3->begin[0] = dr3->begin[1] - dr10first.begin[0] + dr10first.begin[1];
		}
		else
		{
			dr3->begin[0] = dr10first.begin[1];
			dr3->begin[1] = dr10first.begin[0];
			if (diff12itmp == diff12i)
				dr3->begin[2] = dr3->begin[1] - linelast1 + linelast2;
			else
				dr3->begin[2] = dr3->begin[1] - dr12first.begin[0] + dr12first.begin[1];
		}

		if (lastDiffBlockIsDiff12)
		{
			dr3->end[1] = dr12last.end[0];
			dr3->end[2] = dr12last.end[1];
			if (diff10itmp == diff10i)
				dr3->end[0] = dr3->end[1] - linelast1 + linelast0;
			else
				dr3->end[0] = dr3->end[1] - dr10last.end[0] + dr10last.end[1];
		}
		else
		{
			dr3->end[0] = dr10last.end[1];
			dr3->end[1] = dr10last.end[0];
			if (diff12itmp == diff12i)
				dr3->end[2] = dr3->end[1] - linelast1 + linelast2;
			else
				dr3->end[2] = dr3->end[1] - dr12last.end[0] + dr12last.end[1];
		}

		linelast0 = dr3->end[0] + 1;
		linelast1 = dr3->end[1] + 1;
		linelast2 = dr3->end[2] + 1;

		diff3tmp.push_back(dr3);

//		TRACE(_T("left=%d,%d middle=%d,%d right=%d,%d\n"),
//			dr3->begin[0], dr3->end[0], dr3->begin[1], dr3->end[1], dr3->begin[2], dr3->end[2]);

		diff3i++;
		diff10i = diff10itmp;
		diff12i = diff12itmp;
	}

	for (int i = 0; i < diff3i; i++)
	{
		dr3 = diff3tmp.at(i);
		if (i < diff3i - 1)
		{
			wdiff dr3next = *diff3tmp.at(i + 1);
			for (int j = 0; j < 3; j++)
			{
				if (dr3->end[j] >= dr3next.begin[j])
					dr3->end[j] = dr3next.begin[j] - 1;
			}
		}
		diff3.push_back(dr3);
	}
	return diff3i;
}

/**
 * @brief adjust the range of the specified word diffs down to byte(char) level.
 * @param str1, str2 [in] line to be compared
 * @param casitive [in] true for case-sensitive, false for case-insensitive
 * @param xwhite [in] This governs whether we handle whitespace specially
 *  (see WHITESPACE_COMPARE_ALL, WHITESPACE_IGNORE_CHANGE, WHITESPACE_IGNORE_ALL)
 */
void stringdiffs::wordLevelToByteLevel()
{
	for (int i = 0; i < m_wdiffs.size(); i++)
	{
		int begin[3], end[3];
		wdiff *pDiff = m_wdiffs[i];
		String str1_2, str2_2;
		str1_2 = m_str1.substr(pDiff->begin[0], pDiff->end[0] - pDiff->begin[0] + 1);
		str2_2 = m_str2.substr(pDiff->begin[1], pDiff->end[1] - pDiff->begin[1] + 1);
		sd_ComputeByteDiff(str1_2, str2_2, m_case_sensitive, m_whitespace, begin, end, false);
		if (begin[0] == -1)
		{
			// no visible diff on side1
			pDiff->end[0] = pDiff->begin[0] - 1;
		}
		else
		{
			pDiff->end[0] = pDiff->begin[0] + end[0];
			pDiff->begin[0] += begin[0];
		}
		if (begin[1] == -1)
		{
			// no visible diff on side2
			pDiff->end[1] = pDiff->begin[1] - 1;
		}
		else
		{
			pDiff->end[1] = pDiff->begin[1] + end[1];
			pDiff->begin[1] += begin[1];
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
	return (worddiffs[0]->end[0] == -1 && worddiffs[0]->begin[1] == 0 &&
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
	return (worddiffs[0]->end[1] == -1  && worddiffs[0]->begin[0] == 0 &&
			worddiffs[0]->end[0] + 1 == nLineLength);
}
