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
};

class SubstitutionList
{
public:
	SubstitutionList() = default;

	void Add(const std::string& pattern, const std::string& replacement, int regexpCompileOptions);
	void Add(const std::string& pattern, const std::string& replacement, bool caseSensitive, bool matchWholeWordOnly);
	void RemoveAllFilters();
	bool HasRegExps() const { return !m_list.empty(); }
	size_t GetCount() const { return m_list.size(); }
	std::string Subst(const std::string& subject, int codepage = ucr::CP_UTF_8) const;
	const SubstitutionItem& operator[](int index) const { return m_list[index]; }

private:
	std::vector<SubstitutionItem> m_list;
};

