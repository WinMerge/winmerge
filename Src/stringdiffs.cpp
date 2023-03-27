/** 
 * @file  stringdiffs.cpp
 *
 * @brief Implementation file for ComputeWordDiffs (q.v.)
 *
 */

#include "pch.h"
#include "stringdiffs.h"
#define NOMINMAX
#include <Windows.h>
#include <cassert>
#include <chrono>
#include "CompareOptions.h"
#include "stringdiffsi.h"
#include "Diff3.h"

using std::vector;

namespace strdiff
{

static tchar_t *BreakChars = nullptr;
static tchar_t BreakCharDefaults[] = _T(",.;:");
static const int TimeoutMilliSeconds = 500;

static bool isSafeWhitespace(tchar_t ch);
static bool isWordBreak(int breakType, const tchar_t *str, int index, bool ignore_numbers);

void Init()
{
	BreakChars = BreakCharDefaults;
}

void Close()
{
	if (BreakChars != BreakCharDefaults)
	{
		free(BreakChars);
		BreakChars = nullptr;
	}
}

void SetBreakChars(const tchar_t *breakChars)
{
	assert(BreakChars != nullptr);

	if (BreakChars != BreakCharDefaults)
		free(BreakChars);

	BreakChars = tc::tcsdup(breakChars);
}

std::vector<wdiff>
ComputeWordDiffs(const String& str1, const String& str2,
	bool case_sensitive, bool eol_sensitive, int whitespace, bool ignore_numbers, int breakType, bool byte_level)
{
	String strs[3] = {str1, str2, _T("")};
	return ComputeWordDiffs(2, strs, case_sensitive, eol_sensitive, whitespace, ignore_numbers, breakType, byte_level);
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
			if (memcmp(&strs_[0][wd3.begin[0]], &strs_[2][wd3.begin[2]], wlen0 * sizeof(tchar_t)) != 0)
				return false;
		}
		else
		{
			if (tc::tcsnicmp(&strs_[0][wd3.begin[0]], &strs_[2][wd3.begin[2]], wlen0) != 0)
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
ComputeWordDiffs(int nFiles, const String *str,
	bool case_sensitive, bool eol_sensitive, int whitespace, bool ignore_numbers, int breakType, bool byte_level)
{
	std::vector<wdiff> diffs;
	if (nFiles == 2)
	{
		stringdiffs sdiffs(str[0], str[1], case_sensitive, eol_sensitive, whitespace, ignore_numbers, breakType, &diffs);
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
			stringdiffs sdiffs(str[1], str[2], case_sensitive, eol_sensitive, whitespace, ignore_numbers, breakType, &diffs);
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
			stringdiffs sdiffs(str[0], str[2], case_sensitive, eol_sensitive, whitespace, ignore_numbers, breakType, &diffs);
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
			stringdiffs sdiffs(str[0], str[1], case_sensitive, eol_sensitive, whitespace, ignore_numbers, breakType, &diffs);
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
			stringdiffs sdiffs10(str[1], str[0], case_sensitive, eol_sensitive, 0, ignore_numbers, breakType, &diffs10);
			stringdiffs sdiffs12(str[1], str[2], case_sensitive, eol_sensitive, 0, ignore_numbers, breakType, &diffs12);
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

int Compare(const String& str1, const String& str2,
	bool case_sensitive, bool eol_sensitive, int whitespace, bool ignore_numbers)
{
	if (case_sensitive && eol_sensitive && whitespace == WHITESPACE_COMPARE_ALL && !ignore_numbers)
		return str2.compare(str1);
	String s1 = str1, s2 = str2;
	if (!case_sensitive)
	{
		s1 = strutils::makelower(s1);
		s2 = strutils::makelower(s2);
	}
	if (whitespace == WHITESPACE_IGNORE_CHANGE)
	{
		strutils::replace_chars(s1, _T(" \t"), _T(" "));
		strutils::replace_chars(s2, _T(" \t"), _T(" "));
	}
	else if (whitespace == WHITESPACE_IGNORE_ALL)
	{
		strutils::replace_chars(s1, _T(" \t"), _T(""));
		strutils::replace_chars(s2, _T(" \t"), _T(""));
	}
	if (!eol_sensitive)
	{
		strutils::replace_chars(s1, _T("\r\n"), _T("\n"));
		strutils::replace_chars(s2, _T("\r\n"), _T("\n"));
	}
	if (ignore_numbers)
	{
		strutils::replace_chars(s1, _T("0123456789"), _T(""));
		strutils::replace_chars(s2, _T("0123456789"), _T(""));
	}
	return s2.compare(s1);
}

/**
 * @brief stringdiffs constructor simply loads all members from arguments
 */
stringdiffs::stringdiffs(const String & str1, const String & str2,
	bool case_sensitive, bool eol_sensitive, int whitespace, bool ignore_numbers, int breakType,
	std::vector<wdiff> * pDiffs)
: m_str1(str1)
, m_str2(str2)
, m_whitespace(whitespace)
, m_breakType(breakType)
, m_case_sensitive(case_sensitive)
, m_eol_sensitive(eol_sensitive)
, m_ignore_numbers(ignore_numbers)
, m_pDiffs(pDiffs)
, m_matchblock(true) // Change to false to get word to word compare
{
}

/**
 * @brief Destructor.
 * The destructor frees all diffs added to the vectors.
 */
stringdiffs::~stringdiffs() = default;

#ifdef STRINGDIFF_LOGGING
void
stringdiffs::debugoutput()
{
	for (size_t i = 0; i < m_wdiffs.size(); i++)
	{
		String str1;
		String str2;
		tchar_t buf[256];
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
	if (onp(edscript) < 0)
		return false;

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
			if (m_ignore_numbers && IsNumber(m_words1[i]))
			{
				i++;
				continue;
			}

			s1 = m_words1[i].start;
			e1 = m_words1[i].end;
			s2 = m_words2[j-1].end+1;
			e2 = s2-1;
			m_wdiffs.emplace_back(s1, e1, s2, e2);
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

			if (m_ignore_numbers && IsNumber(m_words2[j]))
			{
				j++;
				continue;
			}

			s1 = m_words1[i-1].end+1;
			e1 = s1-1;
			s2 = m_words2[j].start;
			e2 = m_words2[j].end;
			m_wdiffs.emplace_back(s1, e1, s2, e2);
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
			if (m_ignore_numbers && IsNumber(m_words1[i]) && IsNumber(m_words2[j]))
			{
				i++; j++;
				continue;
			}

			s1 =  m_words1[i].start;
			e1 =  m_words1[i].end;
			s2 =  m_words2[j].start;
			e2 =  m_words2[j].end ;
			m_wdiffs.emplace_back(s1, e1, s2, e2);
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

	bool succeeded = false;
#ifdef _WIN64
	if (m_words1.size() < 20480 && m_words2.size() < 20480)
#else
	if (m_words1.size() < 2048 && m_words2.size() < 2048)
#endif
	{
		succeeded = BuildWordDiffList_DP();
	}
	if (!succeeded)
	{
		int s1 = m_words1[0].start;
		int e1 = m_words1[m_words1.size() - 1].end;
		int s2 = m_words2[0].start;
		int e2 = m_words2[m_words2.size() - 1].end;
		m_wdiffs.emplace_back(s1, e1, s2, e2);
		return;
	}

}

/**
 * @brief Break line into constituent words
 */
std::vector<stringdiffs::word>
stringdiffs::BuildWordsArray(const String & str) const
{
	std::vector<word> words = { word(0, -1, 0, 0) }; // dummy;
	int i = 0, begin = 0;
	ICUBreakIterator *pIterChar = ICUBreakIterator::getCharacterBreakIterator(reinterpret_cast<const UChar *>(str.c_str()), static_cast<int32_t>(str.length()));

	size_t sLen = str.length();
	assert(sLen < INT_MAX);
	int iLen = static_cast<int>(sLen);

	// state when we are looking for next word
inspace:
	if (isSafeWhitespace(str[i])) 
	{
		i = pIterChar->next();
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
	if (i == iLen || ((atspace = isSafeWhitespace(str[i])) != 0) || isWordBreak(m_breakType, str.c_str(), i, m_ignore_numbers))
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
			int break_type = (m_ignore_numbers && tc::istdigit(str[i]))
				? dlnumber
				: dlbreak;

			int inext = pIterChar->next();
			words.push_back(word(i, inext - 1, break_type, Hash(str, i, inext - 1, 0)));
			i = inext;
			begin = i;
			goto inword;
		}
	}
	i = pIterChar->next();
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
	auto IsEOLorEmpty = [](const String& text, size_t begin, size_t end) -> bool {
		if (end - begin + 1 > 2)
			return false;
		String str = text.substr(begin, end - begin + 1);
		return (str.empty() || str == _T("\r\n") || str == _T("\n") || str == _T("\r"));
	};
	
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
		else
		{
			if (!m_eol_sensitive &&
				IsEOLorEmpty(m_str1, m_wdiffs[i].begin[0], m_wdiffs[i].end[0]) &&
				IsEOLorEmpty(m_str2, m_wdiffs[i].begin[1], m_wdiffs[i].end[1]))
				skipIt = true;
		}
		if (!skipIt)
		{
			// Should never have a pair where both are missing
			assert(m_wdiffs[i].begin[0]>=0 || m_wdiffs[i].begin[1]>=0);

			// Store the diff[i] in the caller list (m_pDiffs)
			m_pDiffs->emplace_back(m_wdiffs[i]);
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
	if (m_case_sensitive)
	{
		for (int i = begin; i <= end; ++i)
		{
			tchar_t ch = static_cast<unsigned>(str[i]);
			h += HASH(h, ch);
		}
	}
	else
	{
		for (int i = begin; i <= end; ++i)
		{
			tchar_t ch = static_cast<unsigned>(str[i]);
			ch = static_cast<unsigned>(tc::totlower(ch));
			h += HASH(h, ch);
		}
	}

	return h;
}

/**
 * @brief Compare two words (by reference to original strings)
 */
bool
stringdiffs::AreWordsSame(const word& word1, const word& word2) const
{
	if (this->m_whitespace != WHITESPACE_COMPARE_ALL)
	{
		if (IsSpace(word1) && IsSpace(word2))
			return true;
	}
	if (m_ignore_numbers)
	{
		if (tc::istdigit(m_str1[word1.start]) && tc::istdigit(m_str2[word2.start]))
			return true;
	}

	if (word1.hash != word2.hash)
		return false;
	
	int length = word1.length();
	if (length != word2.length())
		return false;
	
	if (m_case_sensitive)
	{
		for (int i = 0; i < length; ++i)
		{
			if (m_str1[word1.start + i] != m_str2[word2.start + i])
				return false;
		}
	}
	else
	{
		for (int i = 0; i < length; ++i)
		{
			tchar_t ch1 = m_str1[word1.start + i];
			tchar_t ch2 = m_str2[word2.start + i];

			if (tc::totlower(ch1) != tc::totlower(ch2))
				return false;
		}
	}
	return true;
}

/**
 * @ brief An O(NP) Sequence Comparison Algorithm. Sun Wu, Udi Manber, Gene Myers
 */
int
stringdiffs::onp(std::vector<char> &edscript)
{
	auto start = std::chrono::system_clock::now();

	int M = static_cast<int>(m_words1.size() - 1);
	int N = static_cast<int>(m_words2.size() - 1);
	const bool exchanged = (M > N);
	if (exchanged)
		std::swap(M, N);

	int *fp = (new int[(M+1) + 1 + (N+1)]) + (M+1);
	struct EditScriptElem { int op; int neq; int pk; int pi; };
	std::vector<EditScriptElem> *es = (new std::vector<EditScriptElem>[(M+1) + 1 + (N+1)]) + (M+1);
	int DELTA = N - M;
	
	auto addEditScriptElem = [&es, &fp](int k) {
		EditScriptElem ese;
		if (fp[k - 1] + 1 > fp[k + 1])
		{
			ese.op = '+';
			ese.neq = fp[k] - (fp[k - 1] + 1);
			ese.pk = k - 1;
		}
		else
		{
			ese.op = '-';
			ese.neq = fp[k] - fp[k + 1];
			ese.pk = k + 1;
		}
		ese.pi = static_cast<int>(es[ese.pk].size() - 1);
		es[k].push_back(ese);
	};

	const int COUNTMAX = 100000;
	int count = 0;
	int k;
	for (k = -(M+1); k <= (N+1); k++)
		fp[k] = -1; 
	int p = -1;
	do
	{
		p++;
		for (k = -p; k <= DELTA-1; k++)
		{
			fp[k] = snake(k, std::max(fp[k-1] + 1, fp[k+1]), M, N, exchanged);
			addEditScriptElem(k);
			count++;
		}
		for (k = DELTA + p; k >= DELTA+1; k--)
		{
			fp[k] = snake(k, std::max(fp[k-1] + 1, fp[k+1]), M, N, exchanged);
			addEditScriptElem(k);
			count++;
		}
		k = DELTA;
		fp[k] = snake(k, std::max(fp[k-1] + 1, fp[k+1]), M, N, exchanged);
		addEditScriptElem(k);
		count++;

		if (count > COUNTMAX)
		{
			count = 0;
			auto end = std::chrono::system_clock::now();
			auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			if (msec > TimeoutMilliSeconds)
			{
				delete [] (es - (M+1));
				delete [] (fp - (M+1));
				return -1;
			}
		}
	} while (fp[k] != N);

	edscript.clear();

	std::vector<char> ses;
	int i;
	for (k = DELTA, i = static_cast<int>(es[DELTA].size() - 1); i >= 0;)
	{
		EditScriptElem& esi = es[k][i];
		for (int j = 0; j < esi.neq; ++j)
			ses.push_back('=');
		ses.push_back(static_cast<char>(esi.op));
		i = esi.pi;
		k = esi.pk;
	}
	std::reverse(ses.begin(), ses.end());

	int D = 0;
	for (size_t n = 1, cnt = ses.size(); n < cnt; n++)
	{
		char c = '!';
		int ch = ses[n];
		bool is_plus = (ch == '+');
		if (is_plus || ch == '-')
		{
			if (n != (cnt - 1) && ses[n + 1] == "+-"[is_plus])
				n++;
			else
				c = "+-"[exchanged == is_plus]; //('+' : exchanged ? '-' : '+'); ('-' : exchanged ? '+' : '-')
			D++;
		}
		else
			c = '=';
		edscript.push_back(c);
	}
		
	delete [] (es - (M+1));
	delete [] (fp - (M+1));

	return D;
}

int
stringdiffs::snake(int k, int y, int M, int N, bool exchanged) const
{
	int x = y - k;
	if (exchanged)
	{
		while (x < M && y < N && AreWordsSame(m_words1[y + 1], m_words2[x + 1])) {
			x++; y++;
		}
	}
	else
	{
		while (x < M && y < N && AreWordsSame(m_words1[x + 1], m_words2[y + 1])) {
			x++; y++;
		}
	}
	return y;
}

/**
 * @brief Return true if chars match
 *
 * Caller must not call this for lead bytes
 */
static inline bool
matchchar(const tchar_t *ch1, const tchar_t *ch2, size_t len, bool casitive)
{
	if (casitive)
		return memcmp(ch1, ch2, len * sizeof(tchar_t)) == 0;
	for (size_t i = 0; i < len; ++i)
	{
		if (tc::totlower(ch1[i]) != tc::totlower(ch2[i]))
			return false;
	}
	return true;
}


/** Does character introduce a multicharacter character? */
static inline bool IsLeadByte(tchar_t ch)
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
isSafeWhitespace(tchar_t ch)
{
	return tc::istspace((unsigned)ch) && !IsLeadByte(ch);
}

/**
 * @brief Is it a non-whitespace wordbreak character (ie, punctuation)?
 */
static bool
isWordBreak(int breakType, const tchar_t *str, int index, bool ignore_numbers)
{
	tchar_t ch = str[index];
	if (ignore_numbers && tc::istdigit(ch))
		return true;
	// breakType==1 means break also on punctuation
	if ((ch & 0xff00) == 0)
	{
//		tchar_t nextCh = str[index + 1];
		// breakType==0 means whitespace only
		if (breakType==0)
			return false;
		return tc::tcschr(BreakChars, ch) != nullptr;
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
//		tchar_t nextCh = str[index + 1];
//		GetStringTypeW(CT_CTYPE3, &nextCh, 1, &wCharTypeNext);
//		return (wCharType != wCharTypeNext);
//		
		WORD wCharType = 0;
		GetStringTypeW(CT_CTYPE1, &ch, 1, &wCharType);
		return !(wCharType & (C1_UPPER | C1_LOWER | C1_DIGIT));
	}
}


/**
 * @brief advance current pointer over whitespace, until not whitespace or beyond end
 * @param pcurrent [in,out] current location (to be advanced)
 * @param end [in] last valid position (only go one beyond this)
 */
static void
AdvanceOverWhitespace(const tchar_t **pcurrent, const tchar_t *end)
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
stringdiffs::ComputeByteDiff(const String & str1, const String & str2, 
		   bool casitive, int xwhite, 
		   int begin[2], int end[2], bool equal)
{
	// Set to sane values
	// Also this way can distinguish if we set begin[0] to -1 for no diff in line
	begin[0] = begin[1] = end[0] = end[1] = 0;

	int len1 = static_cast<int>(str1.length());
	int len2 = static_cast<int>(str2.length());

	const tchar_t *pbeg1 = str1.c_str();
	const tchar_t *pbeg2 = str2.c_str();

	ICUBreakIterator *pIterCharBegin1 = ICUBreakIterator::getCharacterBreakIterator(reinterpret_cast<const UChar *>(pbeg1), static_cast<int32_t>(len1));
	ICUBreakIterator *pIterCharBegin2 = ICUBreakIterator::getCharacterBreakIterator<2>(reinterpret_cast<const UChar *>(pbeg2), static_cast<int32_t>(len2));
	ICUBreakIterator *pIterCharEnd1 = ICUBreakIterator::getCharacterBreakIterator<3>(reinterpret_cast<const UChar *>(pbeg1), static_cast<int32_t>(len1));
	ICUBreakIterator *pIterCharEnd2 = ICUBreakIterator::getCharacterBreakIterator<4>(reinterpret_cast<const UChar *>(pbeg2), static_cast<int32_t>(len2));
	
	if (len1 == 0 || len2 == 0)
	{
		if (len1 == len2)
		{
			begin[0] = begin[1] = end[0] = end[1] = -1;
		}
		else
		{
			end[0] = len1 - 1;
			end[1] = len2 - 1;
		}
		return;
	}

	// cursors from front, which we advance to beginning of difference
	const tchar_t *py1 = pbeg1;
	const tchar_t *py2 = pbeg2;

	// pen1,pen2 point to the last valid character (broken multibyte lead chars don't count)
	const tchar_t *pen1 = pbeg1 + (len1 > 0 ? pIterCharEnd1->preceding(len1) : 0);
	const tchar_t *pen2 = pbeg2 + (len2 > 0 ? pIterCharEnd2->preceding(len2) : 0);
	size_t glyphlenz1 = pbeg1 + len1 - pen1;
	size_t glyphlenz2 = pbeg2 + len2 - pen2;

	if (xwhite != WHITESPACE_COMPARE_ALL)
	{
		// Ignore leading and trailing whitespace
		// by advancing py1 and py2
		// and retreating pen1 and pen2
		while (py1 < pen1 && isSafeWhitespace(*py1))
			py1 = pbeg1 + pIterCharBegin1->next();
		while (py2 < pen2 && isSafeWhitespace(*py2))
			py2 = pbeg2 + pIterCharBegin2->next();
		if ((pen1 < pbeg1 + len1 - 1 || pen2 < pbeg2 + len2 -1)
			&& (pbeg1[len1] != pbeg2[len2]))
		{
			// mismatched broken multibyte ends
		}
		else
		{
			while (pen1 > py1 && isSafeWhitespace(*pen1))
				pen1 = pbeg1 + pIterCharEnd1->previous();
			while (pen2 > py2 && isSafeWhitespace(*pen2))
				pen2 = pbeg2 + pIterCharEnd2->previous();
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

		const tchar_t* py1next = pbeg1 + pIterCharBegin1->next();
		const tchar_t* py2next = pbeg2 + pIterCharBegin2->next();
		size_t glyphleny1 = py1next - py1;
		size_t glyphleny2 = py2next - py2;
		if (glyphleny1 != glyphleny2 || !matchchar(py1, py2, glyphleny1, casitive))
			break; // done with forward search
		py1 = py1next;
		py2 = py2next;
	}

	// Potential difference extends from py1 to pen1 and py2 to pen2

	// Store results of advance into return variables (begin[0] & begin[1])
	// -1 in a begin variable means no visible diff area
	begin[0] = static_cast<int>(py1 - pbeg1);
	begin[1] = static_cast<int>(py2 - pbeg2);

	const tchar_t *pz1 = pen1;
	const tchar_t *pz2 = pen2;

	// Retreat over matching ends of lines
	// Retreat pz1 & pz2 from end until find difference or beginning
	while (true)
	{
		// Check if either side finished
		if (pz1 < py1 && pz2 < py2)
		{
			begin[0] = begin[1] = end[0] = end[1] = -1;
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
				pz1 = pbeg1 + pIterCharEnd1->previous();
			while (pz2 > py2 && isSafeWhitespace(*pz2))
				pz2 = pbeg2 + pIterCharEnd2->previous();
			continue;

		}
		if (xwhite && pz2 > py2 && isSafeWhitespace(*pz2))
		{
			if (xwhite==1)
				break; // done with reverse search
			while (pz2 > py2 && isSafeWhitespace(*pz2))
				pz2 = pbeg2 + pIterCharEnd2->previous();
			continue;
		}

		if (glyphlenz1 != glyphlenz2 || !matchchar(pz1, pz2, glyphlenz1, casitive))
			break; // done with forward search
		const tchar_t* pz1next = pz1;
		const tchar_t* pz2next = pz2;
		pz1 = (pz1 > pbeg1) ? pbeg1 + pIterCharEnd1->preceding(static_cast<int32_t>(pz1 - pbeg1)) : pz1 - 1;
		pz2 = (pz2 > pbeg2) ? pbeg2 + pIterCharEnd2->preceding(static_cast<int32_t>(pz2 - pbeg2)) : pz2 - 1;
		glyphlenz1 = pz1next - pz1;
		glyphlenz2 = pz2next - pz2;
		// Now do real character match
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
	end[0] = static_cast<int>(pz1 - pbeg1 + glyphlenz1 - 1);
	end[1] = static_cast<int>(pz2 - pbeg2 + glyphlenz2 - 1);

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
		String str1_2 = m_str1.substr(diff.begin[0], diff.end[0] - diff.begin[0] + 1);
		String str2_2 = m_str2.substr(diff.begin[1], diff.end[1] - diff.begin[1] + 1);
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
