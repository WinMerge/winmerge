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

static bool
sd_findsyn(wdiff* pDiff, const String & str1, const String & str2, 
            bool casitive, int xwhite, 
            int &begin1, int &begin2, int &end1, int &end2, bool equal, int func,
            int &s1,int &e1,int &s2,int &e2);


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
, m_matchblock(true) // Change to false to get word to word compare
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
	BuildWordsArray(m_str1, &m_words1);
	BuildWordsArray(m_str2, &m_words2);

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
			RemoveItem(m_words1,0);
		}
		if ((int)m_words1.size() > 0)
		{
			// Remove a ending whitespace
			if (IsSpace(*m_words1[m_words1.size() - 1]))
				RemoveItem(m_words1,m_words1.size() - 1);
		}
		// Remove a leading whitespace
		if (((int)m_words2.size() > 0) && (IsSpace(*m_words2[0])))
		{
			RemoveItem(m_words2,0);
		}
		if ((int)m_words2.size() > 0)
		{
			// Remove a ending whitespace
			if (IsSpace(*m_words2[m_words2.size() - 1]))
				RemoveItem(m_words2,m_words2.size() - 1);
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
					RemoveItem(m_words1,bw1);
					lbreak = true;
				}
				if (IsSpace(*m_words2[bw2]))
				{
					RemoveItem(m_words2,bw2);
					lbreak = true;
				}
				if (lbreak)
					continue;
			}
			// Are we looking for a spacebreak, so just look for word->bBreak
			if (IsSpace(*m_words2[bw2]))
				w1 = FindNextSpaceInWords(m_words1, bw1);
			else
				w1 = FindNextMatchInWords(m_words1, *m_words2[bw2], bw1,1);
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
					w2 = FindNextSpaceInWords(m_words2, bw2);
				else
					w2 = FindNextMatchInWords(m_words2,*m_words1[bw1], bw2, 2);
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
								RemoveItem(m_words2,bw2 + 1);
								RemoveItem(m_words2,bw2 + 1);
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
								RemoveItem(m_words1,bw1 + 1);
								RemoveItem(m_words1,bw1 + 1);
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
			const int maxDistance = 6;
			const bool ok = (w2 - bw2 > maxDistance);
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
			while (bw1 > 1 && bw2 > 1)
			{
				if (AreWordsSame(*m_words1[bw1], *m_words2[bw2])
					|| (IsSpace(*m_words1[bw1]) && IsSpace(*m_words2[bw2])))
				{
					bw1--;
					bw2--;
					continue;
				}
				w1 = -2, w2 = -2;

				// Normaly we synchronise with a *word2 to a match in word1
				// If it is an Insert in word2 so look for a *word1 in word2
				if (IsInsert(*m_words2[bw2]))
					w2 = FindPreMatchInWords(m_words2, *m_words1[bw1], bw2,2);
				else
					w1 = FindPreMatchInWords(m_words1, *m_words2[bw2], bw1,1);
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
						MoveInWordsUp(m_words1, w1, bw1);
						bw1--;
						bw2--;
						continue;
					}
					else if (((k - l) == 1) && !AreWordsSame(*m_words1[bw1-l], *m_words2[bw2-l]))
					{
						MoveInWordsUp(m_words1, bw2-l, bw1);
						MoveInWordsUp(m_words1, w1, bw1-1);
						//insert a record before in words1 , after in words2
						InsertInWords(m_words2, bw2);
						InsertInWords(m_words1, w1);
						bw1--;
						bw2--;
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
						MoveInWordsUp(m_words2, w2, bw2);
						bw1--;
						bw2--;
						continue;
					}
					else if (((k - l) == 1) && !AreWordsSame(*m_words1[bw1-l], *m_words2[bw2-l]))
					{
						MoveInWordsUp(m_words2, bw1-l, bw2);
						MoveInWordsUp(m_words2, w2, bw2-1);
						//insert a record before in words2 , after in words1
						InsertInWords(m_words1, bw1);
						InsertInWords(m_words2, w2);
						bw1--;
						bw2--;
						continue;
					}
				}
				// otherwise go on
				// check for an insert on both side
				// if so move next preblock to this position
				if (IsInsert(*m_words1[bw1]))
				{
					int k = FindPreNoInsertInWords(m_words1,bw1);
					if (k == 1)
					{
						bw1--;
						bw2--;
						continue;
					}
					bool ok =false;
					if (k >=0 )
					{
						if (k > 0)
						{
							ok = !(AreWordsSame(*m_words1[k], *m_words2[k]) &&
								AreWordsSame(*m_words1[k - 1], *m_words2[k - 1]));
						}
						else
							ok = !(AreWordsSame(*m_words1[k], *m_words2[k]));
					}
					if(ok)
					{
						MoveInWordsUp(m_words1, k, bw1);
					}
				}
				if (IsInsert(*m_words2[bw2]))
				{
					int k = FindPreNoInsertInWords(m_words2,bw2);
					if (k == 1)
					{
						bw1--;
						bw2--;
						continue;
					}
					bool ok =false;
					if (k >=0 )
					{
						if (k > 0)
							ok = !(AreWordsSame(*m_words1[k], *m_words2[k]) &&
							AreWordsSame(*m_words1[k - 1], *m_words2[k - 1]));
						else
							ok = !(AreWordsSame(*m_words1[k], *m_words2[k]));
					}
					if(ok)					{
						MoveInWordsUp(m_words2, k, bw2);
					}
				}
				bw1--;
				bw2--;
				continue;
			}
		}
// I care about consistency and I think most apps will highlight the space
//  after the word so that would be my preference.
// to get this we need a thirt run, only look for inserts now!
		w1 = 0, w2 = 0;		// position found in array
		bw1 = 0;			// start position in m_words1
		bw2 = 0;			// start position in m_words2
			while (w1 >= 0 || w2 >= 0)
			{
				w1 = FindNextInsertInWords(m_words1,bw1);
				w2 = FindNextInsertInWords(m_words2,bw2);
				if (w1 == w2)
				{
					bw1++;
					bw2++;
				}
				// word1 is first
				else if(w1 >= 0 && (w1 < w2 || w2 == -1))
				{
					bw1 = FindNextNoInsertInWords(m_words1,w1);
					
					if (bw1 >=0 && !AreWordsSame(*m_words1[bw1], *m_words2[bw1]))
					{
						// Move block to actual position
						MoveInWordsDown(m_words1, bw1, w1);
					}
					bw1 = ++w1;
					bw2 = bw1;
				}
				else if(w2 >= 0 && (w2 < w1 || w1 == -1))
				{
					bw2 = FindNextNoInsertInWords(m_words2,w2);
					if (bw2 >=0 && !AreWordsSame(*m_words1[bw2], *m_words2[bw2]))
					{
						// Move block to actual position
						MoveInWordsDown(m_words2, bw2, w2);
					}
					bw1 = ++w2;
					bw2 = bw1;
				}		
			}

		// Remove empty records on both side
#ifdef STRINGDIFF_LOGGING
		OutputDebugString(_T("remove empty records on both side \n"));
#endif
		int i = 0; 
		while ((i < (int)m_words1.size()) && (i < (int)m_words2.size()))
		{
#ifdef STRINGDIFF_LOGGING
			wsprintf(buf, _T("left=%d, op=%d, right=%d, op=%d\n"),
				m_words1[i]->hash, m_words1[i]->bBreak, m_words2[i]->hash, m_words2[i]->bBreak);
			OutputDebugString(buf);
#endif

			if (IsInsert(*m_words1[i]) && AreWordsSame(*m_words1[i], *m_words2[i]))
			{
				RemoveItem(m_words1,i);
				RemoveItem(m_words2,i);
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
				RemoveItem(m_words1,i);
				RemoveItem(m_words2,i + 1);
				continue;
			}
			if (IsInsert(*m_words1[i + 1]) && IsInsert(*m_words2[i]))
			{
				RemoveItem(m_words1,i + 1);
				RemoveItem(m_words2,i);
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
				RemoveItem(m_words1,i);
				RemoveItem(m_words2,i);
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
					RemoveItem(m_words1,i);
					RemoveItem(m_words2,i);
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
					RemoveItem(m_words1,i + 1);
					RemoveItem(m_words2,i + 1);
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
					RemoveItem(m_words1,i + 1);
					RemoveItem(m_words2,i + 1);
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

#ifdef STRINGDIFF_LOGGING
				int len1 = e1 - s1 + 1;
				int len2 = e2 - s2 + 1;

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
 * @brief Insert a new block in words)
 */
void
stringdiffs::InsertInWords(std::vector<word*> &words, int bw)
{
	// Remember last start end
	int end, start;
	if (bw)
	{
		end = words[bw - 1]->end;
	}
	else
	{
		end = -1;
	}
	start = end + 1;
	vector<word*>::iterator iter = words.begin() + bw;
	word *wd  = new word(start, end, dlinsert, 0);
	words.insert(iter, wd);

}
/**
 * @brief Find pre word in m_words2 (starting at bw2) that matches needword1 (in m_words1)
 */
int 
stringdiffs::FindPreMatchInWords(const std::vector<word*> &words, const word & needword, int bw, int side) const
{
	while (bw >= 0)
	{
		if (side == 1)
		{
			if (AreWordsSame(*words[bw], needword))
				return bw;
		}
		else
		{
			if (AreWordsSame(needword, *words[bw]))
				return bw;
		}		--bw;
	}
	return -1;
}
/**
 * @brief Find next word in words (starting at bw) that matches needword1 (in m_words1)
 */
int 
stringdiffs::FindNextMatchInWords(const std::vector<word*> &words, const word & needword, int bw, int side) const
{
	const int iSize = (int) words.size();
	while (bw < iSize)
	{
		if (side == 1)
		{
			if (AreWordsSame(*words[bw], needword))
				return bw;
		}
		else
		{
			if (AreWordsSame(needword, *words[bw]))
				return bw;
		}
		++bw;
	}
	return -1;
}
/**
 * @brief Find pre space in m_words (starting at bw)
 */
int 
stringdiffs::FindPreSpaceInWords(const std::vector<word*> &words, int bw) const
{
	while (bw >= 0)
	{
		if (IsSpace(*words[bw]))
			return bw;
		--bw;
	}
	return -1;
}

/**
 * @brief Find next space in m_words (starting at bw)
 */
int 
stringdiffs::FindNextSpaceInWords(const std::vector<word*> &words, int bw) const
{
	const int iSize = (int) words.size();
	while (bw < iSize)
	{
		if (IsSpace(*words[bw]))
			return bw;
		++bw;
	}
	return -1;
}
/**
 * @brief Find next pre noinsert in words (starting at bw)
 */
int 
stringdiffs::FindPreNoInsertInWords(const std::vector<word*> &words, int bw) const
{
	while (bw >= 0)
	{
		if (!IsInsert(*words[bw]))
			return bw;
		--bw;
	}
	return -1;
}
/**
 * @brief Find next insert in m_words (starting at bw)
 */
int 
stringdiffs::FindNextInsertInWords(const std::vector<word*> &words, int bw) const
{
	const int iSize = (int) words.size();
	while (bw < iSize)
	{
		if (IsInsert(*words[bw]))
			return bw;
		++bw;
	}
	return -1;
}
/**
 * @brief Find next noinsert in m_words (starting at bw)
 */
int 
stringdiffs::FindNextNoInsertInWords(const std::vector<word*> &words, int bw) const
{
	const int iSize = (int) words.size();
	while (bw < iSize)
	{
		if (!IsInsert(*words[bw]))
			return bw;
		++bw;
	}
	return -1;
}
/**
 * @brief Move word to new position (starting at bw)
 */
void 
stringdiffs::MoveInWordsUp(std::vector<word*> &words, int source, int target) const
{
	word *wd  = new word(words[source]->start, words[source]->end, words[source]->bBreak, words[source]->hash);
	RemoveItem(words, source);
	vector<word*>::iterator iter = words.begin() + target;
	words.insert(iter, wd);
	// Correct the start-end pointer
	const int istart = words[target]->start;
	const int iend =istart - 1;
	for (; source < target ; source++)
	{
		words[source]->start = istart;
		words[source]->end = iend;	
	}
}
/**
 * @brief Move word to new position (starting at bw)
 */
void 
stringdiffs::MoveInWordsDown(std::vector<word*> &words, int source, int target) const
{
	word *wd  = new word(words[source]->start, words[source]->end, words[source]->bBreak, words[source]->hash);
	RemoveItem(words, source);
	vector<word*>::iterator iter = words.begin() + target;
	words.insert(iter, wd);
	// Correct the start-end pointer
	const int istart = words[target]->end + 1;
	const int iend =istart - 1;
	target++;
	for (; target < source + 1; target++)
	{
		words[target]->start = istart;
		words[target]->end = iend;	
	}
}
/**
 * @brief erase an item in words
 */
bool
stringdiffs::RemoveItem(std::vector<word*> &words,int bw) const
{
	if ((int)words.size()== bw + 1)
	{
		delete words.back();
		words.pop_back();
	}
	else
	{
		vector<word*>::iterator iter = words.begin() + bw ;
		delete *iter;
		*words.erase(iter);
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
	if (i == str.length() || ((atspace = isSafeWhitespace(str[i])) != 0) || isWordBreak(m_breakType, str[i]))
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
	if (m_whitespace == WHITESPACE_IGNORE_ALL)
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
 * @param [in] casitive true for case-sensitive, false for case-insensitive
 * @param [in] xwhite This governs whether we handle whitespace specially
 * (see WHITESPACE_COMPARE_ALL, WHITESPACE_IGNORE_CHANGE, WHITESPACE_IGNORE_ALL)
 * @param [out] begin1 return -1 if not found or pos of equal
 * @param [out] begin2 return -1 if not found or pos of equal
 * @param [out] end1 return -1 if not found or pos of equal valid if begin1 >=0
 * @param [out] end2 return -1 if not found or pos of equal valid if begin2 >=0
 * @param [in] equal false surch for a diff, true surch for equal
 *
 * Assumes whitespace is never leadbyte or trailbyte!
 */
void
sd_ComputeByteDiff(String & str1, String & str2, 
		   bool casitive, int xwhite, 
		   int &begin1, int &begin2, int &end1, int &end2, bool equal)
{
	// Set to sane values
	// Also this way can distinguish if we set begin1 to -1 for no diff in line
	begin1 = end1 = begin2 = end2 = 0;

	int len1 = str1.length();
	int len2 = str2.length();

	LPCTSTR pbeg1 = str1.c_str();
	LPCTSTR pbeg2 = str2.c_str();
	// cursors from front, which we advance to beginning of difference
	LPCTSTR py1 = 0;
	LPCTSTR py2 = 0;
	// cursors from front, which we advance to ending of difference
	LPCTSTR pz1 = 0;
	LPCTSTR pz2 = 0;

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
	py1 = pbeg1;
	py2 = pbeg2;

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
		begin1 = 0;
		begin2 = 0;
		end1 =len1 - 1;
		end2 =len2 - 1;
		return;
	}
	bool alldone = false;
	bool found = false;

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
			if (!equal)
			{
				if (!(py1[0] == py2[0] && py1[1] == py2[1]))
					break; // done with forward search
			}
			else
			{
				if ((py1[0] == py2[0] && py1[1] == py2[1]))
				{
					// check at least two chars are identical
					if (!found)
					{
						found = true;
					}
					else
					{
						break; // done with forward search
					}
				}
				else
				{
					found = false;
				}
			}
			py1 += 2; // DBCS specific
			py2 += 2; // DBCS specific
		}
		else
		{
			if (IsLeadByte(*py2))
				break; // done with forward search
			if (!equal)
			{
				if (!matchchar(py1[0], py2[0], casitive))
					break; // done with forward search
			}
			else 
			{
				if (matchchar(py1[0], py2[0], casitive))
				{
					// check at least two chars are identical
					if (!found)
					{
						found = true;
					}
					else
					{
						py1 = CharPrev(pbeg1, py1);
						py2 = CharPrev(pbeg2, py2);
						break; // done with forward search
					}
				}
				else
				{
					found = false;
				}
			}
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
		pz1 = pen1;
		pz2 = pen2;

		// Retreat over matching ends of lines
		// Retreat pz1 & pz2 from end until find difference or beginning
		found = false;
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
				found = true;
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
				if (((pz1 == py1) && isSafeWhitespace(*pz1)) || ((pz2 == py2) && isSafeWhitespace(*pz2)))
				{
					break;
				}
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
				if (((pz1 == py1) && isSafeWhitespace(*pz1)) || ((pz2 == py2) && isSafeWhitespace(*pz2)))
				{
					break;
				}
				continue;
			}

			// Now do real character match
			if (IsLeadByte(*pz1))
			{
				if (!IsLeadByte(*pz2))
					break; // done with forward search
				// DBCS (we assume if a lead byte, then character is 2-byte)
				if (!equal)
				{
					if (!(pz1[0] == pz2[0] && pz1[1] == pz2[1]))
					{
						found = true;
						break; // done with forward search
					}
				}
				else
				{
					if ((pz1[0] == pz2[0] && pz1[1] == pz2[1]))
					{
						// check at least two chars are identical
						if (!found)
						{
							found = true;
						}
						else
						{
							break; // done with forward search
						}
					}
					else
					{
						found = false;
					}
                }
			}
			else
			{
				if (IsLeadByte(*pz2))
					break; // done with forward search
				if (!equal)
				{
					if (!matchchar(pz1[0], pz2[0], casitive))
					{
						found = true;
						break; // done with forward search
					}
				}
				else 
				{
					if (matchchar(pz1[0], pz2[0], casitive))
					{
						// check at least two chars are identical
						if (!found)
						{
							found = true;
						}
						else
						{
							pz1++;
							pz2++;
							break; // done with forward search
						}
					}
					else
					{
						found = false;
					}
				}
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
		// return -1 for Not found, otherwise distance from begin
		if (found && pz1 == pbeg1 ) 
		{
			// Found on begin
			end1 = 0;
		}
		else if (pz1 <= pbeg1 || pz1 < py1) 
		{
			// No visible diff in line 1
			end1 = -1; 
		}
		else
		{
			// Found on distance line 1
			end1 = pz1 - pbeg1;
		}
		if (found && pz2 == pbeg2 ) 
		{
			// Found on begin
			end2 = 0;
		}
		else if (pz2 <= pbeg2 || pz2 < py2)
		{
			// No visible diff in line 2
			end2 = -1; 
		}
		else
		{
			// Found on distance line 2
			end2 = pz2 - pbeg2;
		}
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
	bool bRepeat = true;
	String str1_2, str2_2;
	int s1 = 0,e1 = 0,s2 = 0,e2 = 0; 

#ifdef STRINGDIFF_LOGGING
	TCHAR buf[256];
#endif

	for (int i = 0; i < (int)pDiffs->size(); i++)
	{
		int begin1, begin2, end1, end2;
		wdiff *pDiff = (*pDiffs)[i];
		bRepeat = true;

		// Something to differ?
		if (pDiff->start[0] > pDiff->end[0] || pDiff->start[1] > pDiff->end[1])
			continue;
#ifdef STRINGDIFF_LOGGING
		wsprintf(buf, _T("actuell \n left=  %d,%d\n right=  %d,%d \n"),
			pDiff->start[0], pDiff->end[0],pDiff->start[1], pDiff->end[1]);
		OutputDebugString(buf);
#endif	

		// Check for first and last difference in word
		str1_2 = str1.substr(pDiff->start[0], pDiff->end[0] - pDiff->start[0] + 1);
		str2_2 = str2.substr(pDiff->start[1], pDiff->end[1] - pDiff->start[1] + 1);
		sd_ComputeByteDiff(str1_2, str2_2, casitive, xwhite, begin1, begin2, end1, end2, false);
		if (begin1 == -1)
		{
			// no visible diff on side1
			pDiff->end[0] = pDiff->start[0] - 1;
			bRepeat = false;
		}
		else
		{
			if (end1 == -1)
			{
				pDiff->start[0] += begin1;
				pDiff->end[0] = pDiff->start[0] + end1;
			}
			else
			{
				pDiff->end[0] = pDiff->start[0] + end1;
				pDiff->start[0] += begin1;
			}
		}
		if (begin2 == -1)
		{
			// no visible diff on side2
			pDiff->end[1] = pDiff->start[1] - 1;
			bRepeat = false;
		}
		else
		{
			if (end2 == -1)
			{
				pDiff->start[1] += begin2;
				pDiff->end[1] = pDiff->start[1] + end2;
			}
			else
			{
				pDiff->end[1] = pDiff->start[1] + end2;
				pDiff->start[1] += begin2;
			}
		}
#ifdef STRINGDIFF_LOGGING
		wsprintf(buf, _T("changed \n left=  %d,%d\n right=  %d,%d \n"),
			pDiff->start[0], pDiff->end[0],pDiff->start[1], pDiff->end[1]);
		OutputDebugString(buf);
#endif	

		// Nothing to display, remove item
		if (pDiff->start[0] > pDiff->end[0] && pDiff->start[1] > pDiff->end[1])
		{
			vector<wdiff*>::iterator iter = pDiffs->begin() + i;
			delete *iter;
			pDiffs->erase(iter);
			i--;
			continue;
		}
		// Nothing more todo
		if (((pDiff->end[0] - pDiff->start[0])< 3) ||
			((pDiff->end[1] - pDiff->start[1])< 3))
		{
			continue;
		}
		// Now check if possiblity to get more details
		// We have to check if there is again a synchronisation inside the pDiff
		if (bRepeat && pDiff->start[0] < pDiff->end[0] && pDiff->start[1] < pDiff->end[1])
		{
			// define offset to zero
			s1 = 0,e1 = 0,s2 = 0,e2 = 0; 
			bool bsynchron = false;
			// Try to synchron side1 from begin
			bsynchron = sd_findsyn(pDiff, str1, str2, casitive, xwhite,
				begin1, begin2, end1, end2,
				true, synbegin1, s1, e1, s2, e2);

			if (!bsynchron)
			{
				// Try to synchron side2 from begin
				s1 = 0;
				bsynchron = sd_findsyn(pDiff, str1, str2, casitive, xwhite,
					begin1, begin2, end1, end2,
					true, synbegin2, s1, e1, s2, e2);
			}
			if (bsynchron)
			{
				// Try to synchron side1 from end
				bsynchron = sd_findsyn(pDiff, str1, str2, casitive, xwhite,
					begin1, begin2, end1, end2,
					true, synend1, s1, e1, s2, e2);
				if (!bsynchron)
				{
					// Try to synchron side1 from end
					e1 = 0;
					bsynchron = sd_findsyn(pDiff, str1, str2, casitive, xwhite,
						begin1, begin2, end1, end2,
						true, synend2, s1, e1, s2, e2);
				}
			}

			// Do we have more details?
			if ((begin1 == -1) && (begin2 == -1) || (!bsynchron))
			{
				// No visible synchcron on side1 and side2
				bRepeat = false;
			}
			else if ((begin1 >= 0) && (begin2 >= 0) && (end1 >= 0) && (end2 >= 0))
			{
				// Visible sync on side1 and side2
				// Now split in two diff
				vector<wdiff*>::iterator iter = pDiffs->begin() + i + 1;
				// New behind actual diff
				// wdf->start diff->start + offset + end + 1 (zerobased)
				wdiff *wdf = new wdiff(pDiff->start[0] + s1 + end1 + 1, pDiff->end[0],
					pDiff->start[1] + s2 + end2 + 1, pDiff->end[1]);
				pDiffs->insert(iter, wdf);
#ifdef STRINGDIFF_LOGGING
				wsprintf(buf, _T("org\n left=  %d,%d\n right=  %d,%d \n"),
					pDiff->start[0], pDiff->end[0],pDiff->start[1], pDiff->end[1]);
				OutputDebugString(buf);
				wsprintf(buf, _T("insert\n left=  %d,%d\n right=  %d,%d \n"),
					wdf->start[0], wdf->end[0],wdf->start[1], wdf->end[1]);
				OutputDebugString(buf);
#endif
				// change end of actual diff
				pDiff->end[0] = pDiff->start[0] + s1 + begin1 - 1;
				pDiff->end[1] = pDiff->start[1] + s2 + begin2 - 1;
#ifdef STRINGDIFF_LOGGING
				wsprintf(buf, _T("changed\n left=  %d,%d\n right=  %d,%d \n"),
					pDiff->start[0], pDiff->end[0],pDiff->start[1], pDiff->end[1]);
				OutputDebugString(buf);
#endif			
				// visible sync on side1 and side2
				// new in middle with diff
				wdiff *wdfm = new wdiff(pDiff->end[0]  + 1, wdf->start[0] - 1,
					pDiff->end[1] + 1, wdf->start[1] - 1);
				iter = pDiffs->begin() + i + 1;
				pDiffs->insert(iter, wdfm);
#ifdef STRINGDIFF_LOGGING
				wsprintf(buf, _T("insert\n left=  %d,%d\n right=  %d,%d \n"),
					wdf->start[0], wdf->end[0],wdf->start[1], wdf->end[1]);
				OutputDebugString(buf);
#endif
			}
			else if ((begin1 >= 0) && (begin1 < end1))
			{
				// insert side2
				// Visible sync on side1 and side2
				// Now split in two diff
				vector<wdiff*>::iterator iter = pDiffs->begin() + i + 1;
				// New behind actual diff
				wdiff *wdf = new wdiff(pDiff->start[0] + s1 + e1 + end1, pDiff->end[0],
					pDiff->start[1] + s2 + e2 , pDiff->end[1]);
				pDiffs->insert(iter, wdf);
#ifdef STRINGDIFF_LOGGING
				wsprintf(buf, _T("org\n left=  %d,%d\n right=  %d,%d \n"),
					pDiff->start[0], pDiff->end[0],pDiff->start[1], pDiff->end[1]);
				OutputDebugString(buf);
				wsprintf(buf, _T("insert\n left=  %d,%d\n right=  %d,%d \n"),
					wdf->start[0], wdf->end[0],wdf->start[1], wdf->end[1]);
				OutputDebugString(buf);
#endif
				// change end of actual diff
				pDiff->end[0] = pDiff->start[0] + s1 + begin1;
				pDiff->end[1] = pDiff->start[1] + s2 + begin2;
#ifdef STRINGDIFF_LOGGING
				wsprintf(buf, _T("changed\n left=  %d,%d\n right=  %d,%d \n"),
					pDiff->start[0], pDiff->end[0],pDiff->start[1], pDiff->end[1]);
				OutputDebugString(buf);
#endif	
			}
			else if ((begin2 >= 0) && (begin2 < end2))
			{
				// insert side1
				// Visible sync on side1 and side2
				// Now split in two diff
				vector<wdiff*>::iterator iter = pDiffs->begin() + i + 1;
				// New behind actual diff
				wdiff *wdf = new wdiff(pDiff->start[0] + s1 + e1, pDiff->end[0],
					pDiff->start[1] + s2 + e2 + end2, pDiff->end[1]);
				pDiffs->insert(iter, wdf);
#ifdef STRINGDIFF_LOGGING
				wsprintf(buf, _T("org\n left=  %d,%d\n right=  %d,%d \n"),
					pDiff->start[0], pDiff->end[0],pDiff->start[1], pDiff->end[1]);
				OutputDebugString(buf);
				wsprintf(buf, _T("insert\n left=  %d,%d\n right=  %d,%d \n"),
					wdf->start[0], wdf->end[0],wdf->start[1], wdf->end[1]);
				OutputDebugString(buf);
#endif	
				// change end of actual diff
				pDiff->end[0] = pDiff->start[0] + s1 + begin1;
				pDiff->end[1] = pDiff->start[1] + s2 + begin2;
#ifdef STRINGDIFF_LOGGING
				wsprintf(buf, _T("changed\n left=  %d,%d\n right=  %d,%d \n"),
					pDiff->start[0], pDiff->end[0],pDiff->start[1], pDiff->end[1]);
				OutputDebugString(buf);
#endif	
			}
		}
	}
}

/**
 * @brief Compute begin1,begin2,end1,end2 to display byte difference between strings str1 & str2
 *
 * @param [in] casitive true for case-sensitive, false for case-insensitive
 * @param [in] xwhite This governs whether we handle whitespace specially
 * (see WHITESPACE_COMPARE_ALL, WHITESPACE_IGNORE_CHANGE, WHITESPACE_IGNORE_ALL)
 * @param [out] begin1 return -1 if not found or pos of equal
 * @param [out] begin2 return -1 if not found or pos of equal
 * @param [out] end1 return -1 if not found or pos of equal valid if begin1 >=0
 * @param [out] end2 return -1 if not found or pos of equal valid if begin2 >=0
 * @param [in] equal false surch for a diff, true surch for equal
 * @param [in] func 0-3 how to synchron,0,2 str1, 1,3 str2
 * @param [in,out] s1 offset str1 begin
 * @param [in,out] e1 offset str1 end
 * @param [in,out] s2 offset str2 begin
 * @param [in,out] e2 offset str2 end
 *
 * Assumes whitespace is never leadbyte or trailbyte!
 */
bool
sd_findsyn(wdiff*  pDiff, const String & str1, const String & str2, 
		   bool casitive, int xwhite, 
		   int &begin1, int &begin2, int &end1, int &end2, bool equal, int func,
		   int &s1,int &e1,int &s2,int &e2)
{
	String str1_2, str2_2;
	int max1 = pDiff->end[0] - pDiff->start[0];
	int max2 = pDiff->end[1] - pDiff->start[1];
	while((s1 + e1) < max1 && (s2 + e2) < max2)
	{
		// Get substrings for both sides
		str1_2 = str1.substr(pDiff->start[0] + s1,
			pDiff->end[0] - e1 - (pDiff->start[0] + s1) + 1);
		str2_2 = str2.substr(pDiff->start[1] + s2,
			pDiff->end[1] - e2 - (pDiff->start[1] + s2) + 1);
		sd_ComputeByteDiff(str1_2, str2_2, casitive, xwhite, begin1, begin2, end1, end2, equal);

		if (func == synbegin1)
        {
			if ((begin1 == -1) || (begin2 == -1))
			{
				s1++;
			}
			else
			{
				// Found a synchronisation
				return true;
			}
        }
		else if (func == synbegin2)
        {
			if ((begin1 == -1) || (begin2 == -1))
			{
				s2++;
			}
			else
			{
				// Found a synchronisation
				return true;
			}
        }
		else if (func == synend1)
        {
			if ((begin1 == -1) || (begin2 == -1))
			{
				// Found no synchronisation
				return false;
			}
			else
			{
				if ((end1 == -1) || (end2 == -1))
				{
					e1++;
				}
				else
				{
					// Found a synchronisation
					return true;
				}
			}
        }
		else if (func == synend2)
        {
			if ((begin1 == -1) || (begin2 == -1))
			{
				// Found no synchronisation
				return false;
			}
			else
			{
				if ((end1 == -1) || (end2 == -1))
				{
					e2++;
				}
				else
				{
					// Found a synchronisation
					return true;
				}
			}
			if ((begin1 > -1) && (begin2 > -1) && (0 <= end1) && (0 <= end2))
			{
				// Found a synchronisation
				return true;
			}
        }
	}
	// No synchronisation found
	return false;
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
