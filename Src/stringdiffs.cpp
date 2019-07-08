/** 
 * @file  stringdiffs.cpp
 *
 * @brief Implementation file for ComputeWordDiffs (q.v.)
 *
 */

#include "pch.h"
#include "stringdiffs.h"
#define NOMINMAX
#include <windows.h>
#include <tchar.h>
#include <cassert>
#include <mbctype.h>	// MBCS (multibyte MBCS character stuff)
#include "CompareOptions.h"
#include "stringdiffsi.h"
#include "Diff3.h"

using std::vector;

namespace strdiff
{

static bool Initialized;
static bool CustomChars;
static TCHAR *BreakChars;
static TCHAR BreakCharDefaults[] = _T(",.;:");

static bool isSafeWhitespace(TCHAR ch);
static bool isWordBreak(int breakType, const TCHAR *str, int index);

void Init()
{
	BreakChars = &BreakCharDefaults[0];
	Initialized = true;
}

void Close()
{
	if (CustomChars)
	{
		free(BreakChars);
		BreakChars = nullptr;
		CustomChars = false;
	}
	Initialized = false;
}

void SetBreakChars(const TCHAR *breakChars)
{
	assert(Initialized);

	if (CustomChars)
		free(BreakChars);

	CustomChars = true;
	BreakChars = _tcsdup(breakChars);
}

std::vector<wdiff>
ComputeWordDiffs(const String& str1, const String& str2,
	bool case_sensitive, int whitespace, int breakType, bool byte_level)
{
	String strs[3] = {str1, str2, _T("")};
	return ComputeWordDiffs(2, strs, case_sensitive, whitespace, breakType, byte_level);
}

struct Comp02Functor
{
	Comp02Functor(const String *strs, bool case_sensitive) : 
		strs_(strs), case_sensitive_(case_sensitive)
	{
	}
	bool operator()(const wdiff &wd3)
	{
		size_t wlen0 = wd3.end[0] - wd3.begin[0] + 1;
		size_t wlen2 = wd3.end[2] - wd3.begin[2] + 1;
		if (wlen0 != wlen2)
			return false;
		if (case_sensitive_)
		{
			if (memcmp(&strs_[0][wd3.begin[0]], &strs_[2][wd3.begin[2]], wlen0 * sizeof(TCHAR)) != 0)
				return false;
		}
		else
		{
			if (_tcsnicmp(&strs_[0][wd3.begin[0]], &strs_[2][wd3.begin[2]], wlen0) != 0)
				return false;
		}
		return true;
	}
	const String *strs_;
	bool case_sensitive_;
};

/**
 * @brief Construct our worker object and tell it to do the work
 */
std::vector<wdiff>
ComputeWordDiffs(int nFiles, const String str[3],
	bool case_sensitive, int whitespace, int breakType, bool byte_level)
{
	std::vector<wdiff> diffs;
	if (nFiles == 2)
	{
		stringdiffs sdiffs(str[0], str[1], case_sensitive, whitespace, breakType, &diffs);
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
			stringdiffs sdiffs(str[1], str[2], case_sensitive, whitespace, breakType, &diffs);
			sdiffs.BuildWordDiffList();
			if (byte_level)
				sdiffs.wordLevelToByteLevel();
			sdiffs.PopulateDiffs();
			for (size_t i = 0; i < diffs.size(); i++)
			{
				wdiff& diff = diffs[i];
				diff.begin[2] = diff.begin[1];
				diff.begin[1] = diff.begin[0];
				diff.begin[0] = 0;
				diff.end[2] = diff.end[1];
				diff.end[1] = diff.end[0];
				diff.end[0] = -1;
			}
		}
		else if (str[1].empty())
		{
			stringdiffs sdiffs(str[0], str[2], case_sensitive, whitespace, breakType, &diffs);
			sdiffs.BuildWordDiffList();
			if (byte_level)
				sdiffs.wordLevelToByteLevel();
			sdiffs.PopulateDiffs();
			for (size_t i = 0; i < diffs.size(); i++)
			{
				wdiff& diff = diffs[i];
				diff.begin[2] = diff.begin[1];
				//diff.begin[0] = diff.begin[0];
				diff.begin[1] = 0;
				diff.end[2] = diff.end[1];
				//diff.end[0] = diff.end[0];
				diff.end[1] = -1;
			}
		}
		else if (str[2].empty())
		{
			stringdiffs sdiffs(str[0], str[1], case_sensitive, whitespace, breakType, &diffs);
			sdiffs.BuildWordDiffList();
			if (byte_level)
				sdiffs.wordLevelToByteLevel();
			sdiffs.PopulateDiffs();
			for (size_t i = 0; i < diffs.size(); i++)
			{
				wdiff& diff = diffs[i];
				//diff.begin[1] = diff.begin[1];
				//diff.begin[0] = diff.begin[0];
				diff.begin[2] = 0;
				//diff.end[1] = diff.end[1];
				//diff.end[0] = diff.end[0];
				diff.end[2] = -1;
			}
		}
		else
		{
			std::vector<wdiff> diffs10, diffs12;
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

			Make3wayDiff(diffs, diffs10, diffs12, 
				Comp02Functor(str, case_sensitive), false);
		}
	}
	return diffs;
}

/**
 * @brief stringdiffs constructor simply loads all members from arguments
 */
stringdiffs::stringdiffs(const String & str1, const String & str2,
	bool case_sensitive, int whitespace, int breakType,
	std::vector<wdiff> * pDiffs)
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
}

#ifdef STRINGDIFF_LOGGING
void
stringdiffs::debugoutput()
{
	for (size_t i = 0; i < m_wdiffs.size(); i++)
	{
		String str1;
		String str2;
		TCHAR buf[256];
		int s1 = m_wdiffs[i].begin[0];
		int e1 = m_wdiffs[i].end[0];
		int s2 = m_wdiffs[i].begin[1];
		int e2 = m_wdiffs[i].end[1];

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

	int i = 1, j = 1;
	for (size_t k = 0; k < edscript.size(); k++)
	{
		int s1, e1, s2, e2;
		if (edscript[k] == '-')
		{
			if (m_whitespace == WHITESPACE_IGNORE_ALL)
			{
				if (IsSpace(m_words1[i]))
				{
					i++;
					continue;
				}
			}
				
			s1 = m_words1[i].start;
			e1 = m_words1[i].end;
			s2 = m_words2[j-1].end+1;
			e2 = s2-1;
			m_wdiffs.push_back(wdiff(s1, e1, s2, e2));
			i++;
		}
		else if (edscript[k] == '+')
		{
			if (m_whitespace == WHITESPACE_IGNORE_ALL)
			{
				if (IsSpace(m_words2[j]))
				{
					j++;
					continue;
				}
			}

			s1 = m_words1[i-1].end+1;
			e1 = s1-1;
			s2 = m_words2[j].start;
			e2 = m_words2[j].end;
			m_wdiffs.push_back(wdiff(s1, e1, s2, e2));
			j++;
		}
		else if (edscript[k] == '!')
		{
			if (m_whitespace == WHITESPACE_IGNORE_CHANGE || m_whitespace == WHITESPACE_IGNORE_ALL)
			{
				if (IsSpace(m_words1[i]) && IsSpace(m_words2[j]))
				{
					i++; j++;
					continue;
				}
			}
				
			s1 =  m_words1[i].start;
			e1 =  m_words1[i].end;
			s2 =  m_words2[j].start;
			e2 =  m_words2[j].end ;
			m_wdiffs.push_back(wdiff(s1, e1, s2, e2));
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
	m_words1 = BuildWordsArray(m_str1);
	m_words2 = BuildWordsArray(m_str2);

#ifdef _WIN64
	if (m_words1.size() > 20480 || m_words2.size() > 20480)
#else
	if (m_words1.size() > 2048 || m_words2.size() > 2048)
#endif
	{
		int s1 = m_words1[0].start;
		int e1 = m_words1[m_words1.size() - 1].end;
		int s2 = m_words2[0].start;
		int e2 = m_words2[m_words2.size() - 1].end;
		m_wdiffs.push_back(wdiff(s1, e1, s2, e2));		
		return;
	}

	BuildWordDiffList_DP();
}

/**
 * @brief Break line into constituent words
 */
std::vector<stringdiffs::word>
stringdiffs::BuildWordsArray(const String & str)
{
	std::vector<word> words;
	int i=0, begin=0;

	size_t sLen = str.length();
	assert(sLen < INT_MAX);
	int iLen = static_cast<int>(sLen);

	// dummy;
	words.push_back(word(0, -1, 0, 0));

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

		words.push_back(word(begin, e, dlspace, Hash(str, begin, e, 0)));
	}
	if (i == iLen)
		return words;
	begin = i;
	goto inword;

	// state when we are inside a word
inword:
	bool atspace=false;
	if (i == iLen || ((atspace = isSafeWhitespace(str[i])) != 0) || isWordBreak(m_breakType, str.c_str(), i))
	{
		if (begin<i)
		{
			// just finished a word
			// e is first non-word character (space or at end)
			int e = i-1;
			
			words.push_back(word(begin, e, dlword, Hash(str, begin, e, 0)));
		}
		if (i == iLen)
		{
			return words;
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
			words.push_back(word(i, i, dlbreak, Hash(str, i, i, 0)));
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
			if (m_wdiffs[i].end[0] + 1 == m_wdiffs[i+1].begin[0]
				&& m_wdiffs[i].end[1] + 1 == m_wdiffs[i+1].begin[1])
			{
				// diff[i] and diff[i+1] are contiguous
				// so combine them into diff[i+1] and ignore diff[i]
				m_wdiffs[i+1].begin[0] = m_wdiffs[i].begin[0];
				m_wdiffs[i+1].begin[1] = m_wdiffs[i].begin[1];
				skipIt = true;
			}
		}
		if (!skipIt)
		{
			// Should never have a pair where both are missing
			assert(m_wdiffs[i].begin[0]>=0 || m_wdiffs[i].begin[1]>=0);

			// Store the diff[i] in the caller list (m_pDiffs)
			m_pDiffs->push_back(wdiff(m_wdiffs[i]));
		}
	}
}

// diffutils hash

/* Rotate a value n bits to the left. */
#define UINT_BIT (sizeof (unsigned) * CHAR_BIT)
#define ROL(v, n) ((v) << (n) | (v) >> (UINT_BIT - (n)))
/* Given a hash value and a new character, return a new hash value. */
#define HASH(h, c) ((c) + ROL (h, 7))

unsigned
stringdiffs::Hash(const String & str, int begin, int end, unsigned h) const
{
	for (int i = begin; i <= end; ++i)
	{
		TCHAR ch = static_cast<unsigned>(str[i]);
		if (m_case_sensitive)
		{
			h += HASH(h, ch);
		}
		else
		{
			ch = static_cast<unsigned>(_totupper(ch));
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
 * @ brief An O(NP) Sequence Comparison Algorithm. Sun Wu, Udi Manber, Gene Myers
 */
int
stringdiffs::onp(std::vector<char> &edscript)
{
	int M = static_cast<int>(m_words1.size() - 1);
	int N = static_cast<int>(m_words2.size() - 1);
	bool exchanged = false;
	if (M > N)
	{
		M = static_cast<int>(m_words2.size() - 1);
		N = static_cast<int>(m_words1.size() - 1);
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
			fp[k] = snake(k, std::max(fp[k-1] + 1, fp[k+1]), exchanged);
	
			es[k] = fp[k-1] + 1 > fp[k+1] ? es[k-1] : es[k+1];
			es[k].push_back(fp[k-1] + 1 > fp[k+1] ? '+' : '-');
			es[k].resize(es[k].size() + fp[k] - std::max(fp[k-1] + 1, fp[k+1]), '=');
		}
		for (k = DELTA + p; k >= DELTA+1; k--)
		{
			fp[k] = snake(k, std::max(fp[k-1] + 1, fp[k+1]), exchanged);
	
			es[k] = fp[k-1] + 1 > fp[k+1] ? es[k-1] : es[k+1];
			es[k].push_back(fp[k-1] + 1 > fp[k+1] ? '+' : '-');
			es[k].resize(es[k].size() + fp[k] - std::max(fp[k-1] + 1, fp[k+1]), '=');
		}
		k = DELTA;
		fp[k] = snake(k, std::max(fp[k-1] + 1, fp[k+1]), exchanged);
	
		es[k] = fp[k-1] + 1 > fp[k+1] ? es[k-1] : es[k+1];
		es[k].push_back(fp[k-1] + 1 > fp[k+1] ? '+' : '-');
		es[k].resize(es[k].size() + fp[k] - std::max(fp[k-1] + 1, fp[k+1]), '=');
	} while (fp[k] != N);

	std::vector<char> &ses = es[DELTA]; // Shortest edit script
	edscript.clear();

	int D = 0;
	for (size_t i = 1; i < ses.size(); i++)
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
	int M = static_cast<int>(exchanged ? m_words2.size() - 1 : m_words1.size() - 1);
	int N = static_cast<int>(exchanged ? m_words1.size() - 1 : m_words2.size() - 1);
	int x = y - k;
	while (x < M && y < N && (exchanged ? AreWordsSame(m_words1[y + 1], m_words2[x + 1]) : AreWordsSame(m_words1[x + 1], m_words2[y + 1]))) {
		x = x + 1; y = y + 1;
	}
	return y;
}

/**
 * @brief Return true if chars match
 *
 * Caller must not call this for lead bytes
 */
static inline bool
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
	return _istspace((unsigned)ch) && !IsLeadByte(ch);
}

/**
 * @brief Is it a non-whitespace wordbreak character (ie, punctuation)?
 */
static bool
isWordBreak(int breakType, const TCHAR *str, int index)
{
	TCHAR ch = str[index];
	// breakType==1 means break also on punctuation
	if ((ch & 0xff00) == 0)
	{
//		TCHAR nextCh = str[index + 1];
		// breakType==0 means whitespace only
		if (breakType==0)
			return false;
		return _tcschr(BreakChars, ch) != nullptr;
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
 * @brief Compute begin1,begin2,end1,end2 to display byte difference between strings str1 & str2
 * @param casitive [in] true for case-sensitive, false for case-insensitive
 * @param xwhite [in] This governs whether we handle whitespace specially (see WHITESPACE_COMPARE_ALL, WHITESPACE_IGNORE_CHANGE, WHITESPACE_IGNORE_ALL)
 * @param [out] begin return -1 if not found or pos of equal
 * @param [out] end return -1 if not found or pos of equal valid if begin1 >=0
 * @param [in] equal false surch for a diff, true surch for equal
 *
 *
 * Assumes whitespace is never leadbyte or trailbyte!
 */
void
ComputeByteDiff(String & str1, String & str2, 
		   bool casitive, int xwhite, 
		   int begin[2], int end[2], bool equal)
{
	// Set to sane values
	// Also this way can distinguish if we set begin[0] to -1 for no diff in line
	begin[0] = end[0] = begin[1] = end[1] = 0;

	int len1 = static_cast<int>(str1.length());
	int len2 = static_cast<int>(str2.length());

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
	while (true)
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
	begin[0] = static_cast<int>(py1 - pbeg1);
	begin[1] = static_cast<int>(py2 - pbeg2);

	LPCTSTR pz1 = pen1;
	LPCTSTR pz2 = pen2;

	// Retreat over matching ends of lines
	// Retreat pz1 & pz2 from end until find difference or beginning
	while (true)
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
	end[0] = static_cast<int>(pz1 - pbeg1);
	end[1] = static_cast<int>(pz2 - pbeg2);

	// Check if difference region was empty
	if (begin[0] == end[0] + 1 && begin[1] == end[1] + 1)
		begin[0] = -1; // no diff
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
	for (size_t i = 0; i < m_wdiffs.size(); i++)
	{
		int begin[3], end[3];
		wdiff& diff = m_wdiffs[i];
		String str1_2, str2_2;
		str1_2 = m_str1.substr(diff.begin[0], diff.end[0] - diff.begin[0] + 1);
		str2_2 = m_str2.substr(diff.begin[1], diff.end[1] - diff.begin[1] + 1);
		ComputeByteDiff(str1_2, str2_2, m_case_sensitive, m_whitespace, begin, end, false);
		if (begin[0] == -1)
		{
			// no visible diff on side1
			diff.end[0] = diff.begin[0] - 1;
		}
		else
		{
			diff.end[0] = diff.begin[0] + end[0];
			diff.begin[0] += begin[0];
		}
		if (begin[1] == -1)
		{
			// no visible diff on side2
			diff.end[1] = diff.begin[1] - 1;
		}
		else
		{
			diff.end[1] = diff.begin[1] + end[1];
			diff.begin[1] += begin[1];
		}
	}
}

}
