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

SubstitutionItem::SubstitutionItem(const std::string& pattern,
	const std::string& replacement, int regexpCompileOptions)
	: pattern(pattern)
	, replacement(replacement)
	, regexpCompileOptions(regexpCompileOptions)
	, regexp(pattern, regexpCompileOptions)
{
}

SubstitutionItem::SubstitutionItem(const SubstitutionItem& other)
	: pattern(other.pattern)
	, replacement(other.replacement)
	, regexpCompileOptions(other.regexpCompileOptions)
	, regexp(other.pattern, other.regexpCompileOptions)
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
		rePattern = "\\b" + rePattern + "\\b";
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

void SubstitutionList::RemoveAllFilters()
{
	m_list.clear();
}

