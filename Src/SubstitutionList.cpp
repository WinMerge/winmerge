/** 
 * @file  SubstitutionList.cpp
 *
 * @brief Implementation file for SubstitutionList.
 */

#include "pch.h"
#include "SubstitutionList.h"
#include <vector>
#include <Poco/RegularExpression.h>
#include "unicoder.h"
#include "CompareOptions.h"
#include "stringdiffs.h"


static std::string ExtractCommonPrefix(const std::string& str0, const std::string& str1)
{
	size_t strlen0 = str0.length();
	size_t strlen1 = str1.length();
	size_t minStrlen = strlen0 < strlen1 ? strlen0 : strlen1;
	for (size_t c = 0; c < minStrlen; c++)
	{
		if (str0[c] != str1[c])
			return str0.substr(0, c);
	}
	return strlen0 < strlen1 ? str0 : str1;
}

static std::string ExtractCommonSuffix(const std::string& str0, const std::string& str1)
{
	size_t strlen0 = str0.length();
	size_t strlen1 = str1.length();
	size_t minStrlen = strlen0 < strlen1 ? strlen0 : strlen1;
	for (size_t c = 0; c < minStrlen; c++)
	{
		if (str0[strlen0 - 1 - c] != str1[strlen1 - 1 - c])
			return str0.substr(strlen0 - c, c);
	}
	return strlen0 < strlen1 ? str0 : str1;
}

static std::string ExtractMiddleChangedPart(const std::string& str0, const std::string& str1, int stringIndex)
{
	size_t strlen[2]{ str0.length(), str1.length() };
	size_t minStrlen = strlen[0] < strlen[1] ? strlen[0] : strlen[1];
	size_t diffStart = 0;
	while (diffStart < minStrlen && str0[diffStart] == str1[diffStart])
		diffStart++;
	size_t diffEnd = 0;
	while (diffEnd < minStrlen && str0[strlen[0] - 1 - diffEnd] == str1[strlen[1] - 1 - diffEnd])
		diffEnd++;

	const std::string* strs[2]{ &str0, &str1 };
	return strs[stringIndex]->substr(diffStart, strlen[stringIndex] - diffStart - diffEnd);
}

SubstitutionItem::SubstitutionItem(const std::string& pattern,
	const std::string& replacement, int regexpCompileOptions)
	: pattern(pattern)
	, replacement(replacement)
	, regexpCompileOptions(regexpCompileOptions)
	, regexp(pattern, regexpCompileOptions)
	, commonPrefix(ExtractCommonPrefix(pattern, replacement))
	, commonPrefixLength(commonPrefix.length())
	, middleParts
	{
		ExtractMiddleChangedPart(pattern, replacement, 0),
		ExtractMiddleChangedPart(pattern, replacement, 1)
	}
	, commonSuffix(ExtractCommonSuffix(pattern, replacement))
	, commonSuffixLength(commonSuffix.length())
	, changedPartCompiledRegexp
	{
		Poco::RegularExpression(middleParts[0], regexpCompileOptions),
		Poco::RegularExpression(middleParts[1], regexpCompileOptions)
	}
{
}

SubstitutionItem::SubstitutionItem(const SubstitutionItem& other)
	: pattern(other.pattern)
	, replacement(other.replacement)
	, regexpCompileOptions(other.regexpCompileOptions)
	, regexp(other.pattern, other.regexpCompileOptions)
	, commonPrefix (other.commonPrefix)
	, middleParts { other.middleParts[0], other.middleParts[1] }
	, commonSuffix (other.commonSuffix)
	, commonPrefixLength (other.commonPrefixLength)
	, commonSuffixLength (other.commonSuffixLength)
	, changedPartCompiledRegexp
	{
		Poco::RegularExpression(other.middleParts[0], regexpCompileOptions),
		Poco::RegularExpression(other.middleParts[0], regexpCompileOptions),
	}
{
}

void SubstitutionList::Add(const std::string& pattern, const std::string& replacement, int regexpCompileOptions)
{
	m_list.emplace_back(pattern, replacement, regexpCompileOptions);
}

void SubstitutionList::Add(
	const std::string& pattern, const std::string& replacement,
	bool caseSensitive, bool matchWholeWordOnly)
{
	int regexpCompileOptions = 
		caseSensitive ? 0 : Poco::RegularExpression::RE_CASELESS;
	std::string rePattern;
	for (auto c: pattern)
	{
		switch (c)
		{
		case '\\': case '.': case '^': case '$': case '|':
		case '[':  case ']': case '(': case ')': case '!':
		case '?': case '*':  case '+': case '{': case '}':
			rePattern.push_back('\\');
			break;
		default:
			break;
		}
		rePattern.push_back(c);
	}
	if (matchWholeWordOnly)
		rePattern = "\b" + rePattern + "\b";
	m_list.emplace_back(rePattern, replacement, regexpCompileOptions);
}

std::string SubstitutionList::Subst(const std::string& subject, int codepage/*=CP_UTF8*/) const
{
	std::string replaced;

	if (codepage != ucr::CP_UTF_8)
	{
		// convert string into UTF-8
		ucr::buffer buf(subject.length() * 2);

		ucr::convert(ucr::NONE, codepage, reinterpret_cast<const unsigned char*>(subject.c_str()),
			subject.length(), ucr::UTF8, ucr::CP_UTF_8, &buf);

		replaced.assign(reinterpret_cast<const char *>(buf.ptr), buf.size);
	}
	else
	{
		replaced = subject;
	}

	for (const auto& item : m_list)
	{
		try
		{
			item.regexp.subst(replaced, item.replacement, Poco::RegularExpression::RE_GLOBAL);
		}
		catch (...)
		{
			// TODO:
		}
	}

	return replaced;
}

static void ReplaceSpaces(std::string & str, const char *rep)
{
	std::string::size_type pos = 0;
	size_t replen = strlen(rep);
	while ((pos = str.find_first_of(" \t", pos)) != std::string::npos)
	{
		std::string::size_type posend = str.find_first_not_of(" \t", pos);
		if (posend != String::npos)
			str.replace(pos, posend - pos, rep);
		else
			str.replace(pos, 1, rep);
		pos += replen;
	}
}

bool SubstitutionList::MatchBoth
(
	size_t filterIndex,
	const std::string& string0,
	const std::string& string1,
	int codepage/*=CP_UTF8*/
) const
{
	bool retval = false;
	const size_t count = m_list.size();

	if (filterIndex >= count)
		return false;

	// convert string into UTF-8
	ucr::buffer buf0(string0.length() * 2);
	ucr::buffer buf1(string1.length() * 2);

	if (codepage != ucr::CP_UTF_8)
	{
		ucr::convert(ucr::NONE, codepage, reinterpret_cast<const unsigned char*>(string0.c_str()),
			string0.length(), ucr::UTF8, ucr::CP_UTF_8, &buf0);
		ucr::convert(ucr::NONE, codepage, reinterpret_cast<const unsigned char*>(string1.c_str()),
			string1.length(), ucr::UTF8, ucr::CP_UTF_8, &buf1);
	}

	const SubstitutionItem &item = m_list[filterIndex];
	int result = 0;
	Poco::RegularExpression::Match match;
	try
	{
		if (buf0.size > 0 && buf1.size > 0)
		{
			result =
				   item.changedPartCompiledRegexp[0].match(std::string(reinterpret_cast<const char*>(buf0.ptr), buf0.size), 0, match)
				&& item.changedPartCompiledRegexp[1].match(std::string(reinterpret_cast<const char*>(buf1.ptr), buf1.size), 0, match);
		}
		else
		{
			result =
				   item.changedPartCompiledRegexp[0].match(string0, 0, match)
				&& item.changedPartCompiledRegexp[1].match(string1, 0, match);
		}
	}
	catch (...)
	{
		// TODO:
	}
	if (result > 0)
	{
		retval = true;
	}

	return retval;
}

bool SubstitutionList::AreAllChangesTrivial
(
	const std::string& changeBlock0, const std::string& changeBlock1,
	const WhitespaceIgnoreChoices& whitespaceIgnoreChoices,
	bool ignoreCase,
	int codepage/*=CP_UTF8*/
)
{
	static constexpr bool optMatchBothWays = false;
	static constexpr bool optUseRegexpsForIgnoredSubstitutions = false;

	std::string c0 = changeBlock0;
	std::string c1 = changeBlock1;

	if (whitespaceIgnoreChoices == WHITESPACE_IGNORE_ALL)
	{
		//Ignore character case
		ReplaceSpaces(c0, "");
		ReplaceSpaces(c1, "");
	}
	else if (whitespaceIgnoreChoices == WHITESPACE_IGNORE_CHANGE)
	{
		//Ignore change in whitespace char count
		ReplaceSpaces(c0, " ");
		ReplaceSpaces(c1, " ");
	}

	String strs[3] = { ucr::toTString(c0.c_str()), ucr::toTString(c1.c_str()), _T("") };

	bool eol_sensitive = false;
	int whitespace = whitespaceIgnoreChoices;
	int breakType = 1;// breakType==1 means break also on punctuation
	bool byte_level = true;
	std::vector<strdiff::wdiff> worddiffs =
		strdiff::ComputeWordDiffs(2, strs, ignoreCase, eol_sensitive, whitespace, breakType, byte_level);

	if (worddiffs.empty())
		return false;

	for (const strdiff::wdiff &diff : worddiffs)
	{
		int changeStartPos[2] = { diff.begin[0], diff.begin[1] };
		int changeEndPos[2] = { diff.end[0], diff.end[1] };
	
		if (!optUseRegexpsForIgnoredSubstitutions)
		{
			/// The passed in changes can still have a commonality between them on the ends. Shrink changeStartPos and changeEndPos,
			/// so that in terms of the first and the last symbols change0 and change1 are different.
			/// The ignored substitution tokens are broken down similarly.
			while
			(
				   changeStartPos[0] < changeEndPos[0]
				&& changeStartPos[1] < changeEndPos[1]
				&& c0[changeStartPos[0]] == c1[changeStartPos[1]]
			)
			{
				changeStartPos[0]++;
				changeStartPos[1]++;
			}

			while
			(
				   changeStartPos[0] < changeEndPos[0]
				&& changeStartPos[1] < changeEndPos[1]
				&& c0[changeEndPos[0]] == c1[changeEndPos[1]]
			)
			{
				changeEndPos[0]--;
				changeEndPos[1]--;
			}
		}

		int changeLen0 = changeEndPos[0] - changeStartPos[0] + 1;
		int changeLen1 = changeEndPos[1] - changeStartPos[1] + 1;
		std::string change0 = std::string(c0.c_str() + changeStartPos[0], changeLen0);
		std::string change1 = std::string(c1.c_str() + changeStartPos[1], changeLen1);

		size_t numIgnoredSubstitutions = GetCount();

		for (int f = 0; f < numIgnoredSubstitutions; f++)
		{
			const SubstitutionItem& filter = operator[](f);

			/// Check if the common prefix and suffix fit into the line around the change
			if
			(
				   changeStartPos[0] < filter.commonPrefixLength
				|| changeStartPos[1] < filter.commonPrefixLength
				|| changeEndPos[0] >= c0.length() - filter.commonSuffixLength
				|| changeEndPos[1] >= c1.length() - filter.commonSuffixLength
			)
				continue; /// This filter does not fit into the change block together with its suffix and prefix

			/// Check if the common prefix matches (if the the filter is a regexp the prefix will be empty)
			bool continueWithNextFilter = false;
			for (int p = 1; p <= filter.commonPrefixLength; p++)
			{
				char preffixChar0 = c0[changeStartPos[0] - p];
				char preffixChar1 = c1[changeStartPos[1] - p];
				char preffixCharInFilter = filter.commonPrefix[filter.commonPrefixLength - p];
				if (preffixChar0 != preffixCharInFilter || preffixChar1 != preffixCharInFilter)
				{
					continueWithNextFilter = true;
					break;
				}
			}

			if (continueWithNextFilter)
				continue;

			/// Check if the common suffix matches (if the the filter is a regexp the suffix will be empty)
			for (int s = 1; s <= filter.commonSuffixLength; s++)
			{
				char suffixChar0 = c0[changeEndPos[0] + s];
				char suffixChar1 = c1[changeEndPos[1] + s];
				char suffixCharInFilter = filter.commonSuffix[s - 1];
				if (suffixChar0 != suffixCharInFilter || suffixChar1 != suffixCharInFilter)
				{
					continueWithNextFilter = true;
					break;
				}
			}

			if (continueWithNextFilter)
				continue;

			/// Check is the middle part matches
			if(optUseRegexpsForIgnoredSubstitutions)
			{
				if
				(
					   MatchBoth(f, change0, change1)
					||
						 optMatchBothWays
					&& MatchBoth(f, change1, change0)
				)
				{
					return true; /// a match found
				}
			}
			else
			{
				if
				(
					   filter.middleParts[0].compare(change0) == 0
					&& filter.middleParts[1].compare(change1) == 0
					||
					   optMatchBothWays
					&& filter.middleParts[0].compare(change1) == 0
					&& filter.middleParts[1].compare(change0) == 0
				)
				{
					return true; /// a match found
				}
			}
		}

		return false; /// n0 match found
	}

	return true;
}

void SubstitutionList::RemoveAllFilters()
{
	m_list.clear();
}

