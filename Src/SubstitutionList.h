/**
 * @file  SubstitutionList.h
 *
 * @brief Declaration file for SubstitutionList.
 */
#pragma once

#include <vector>
#include <memory>
#include <Poco/RegularExpression.h>
#include "unicoder.h"

struct SubstitutionItem
{
	SubstitutionItem(const std::string& pattern, const std::string& replacement,
		int regexpCompileOptions);
	SubstitutionItem(const SubstitutionItem& other);

	const std::string pattern;
	const std::string replacement;
	const int regexpCompileOptions;
	Poco::RegularExpression regexp; /**< Compiled regular expression */

	/// Processed data:
	std::string commonPrefix;	/// pattern and replacement are broken down into the three parts each:
	std::string middleParts[2];	///		pattern     = commonPrefix + middleParts[0] + commonSuffix
	std::string commonSuffix;	///		replacement = commonPrefix + middleParts[1] + commonSuffix
	size_t commonPrefixLength; /// For convenience
	size_t commonSuffixLength;
	Poco::RegularExpression changedPartCompiledRegexp[2];
};

class SubstitutionList
{
public:
	SubstitutionList() {};

	void Add(const std::string& pattern, const std::string& replacement, int regexpCompileOptions);
	void Add(const std::string& pattern, const std::string& replacement, bool caseSensitive, bool matchWholeWordOnly);
	void RemoveAllFilters();
	bool HasRegExps() const { return !m_list.empty(); }
	size_t GetCount() const { return m_list.size(); }
	std::string Subst(const std::string& subject, int codepage = CP_UTF8) const;
	bool AreAllChangesTrivial
	(
		const std::string& changedBlock0, const std::string& changedBlock1,
		const enum WhitespaceIgnoreChoices& whitespaceIgnoreChoices,
		bool ignoreCase,
		int codepage=CP_UTF8
	);

	bool MatchBoth
	(
		size_t filterIndex,
		const std::string& string0,
		const std::string& string1,
		int codepage = CP_UTF8
	) const;

	const SubstitutionItem& operator[](int index) const { return m_list[index]; }

private:
	std::vector<SubstitutionItem> m_list;
};

