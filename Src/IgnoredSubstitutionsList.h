/** 
 * @file IgnoredSubstitutionsList.h
 *
 * @brief Declaration file for IgnoredSubstitutionsList class
 */
#pragma once

#include <vector>
#include <memory>
#include "UnicodeString.h"
#include "SubstitutionList.h"

class COptionsMgr;

/**
 @brief Structure for filter.
 */
struct IgnoredSubstitution
{
	bool enabled;
	bool useRegExp;
	bool caseSensitive;
	bool matchWholeWordOnly;
	String pattern;
	String replacement;
};

/**
 @brief List of raw Ignored Substitution pairs.
 */
class IgnoredSubstitutionsList
{
public:
	IgnoredSubstitutionsList();
	~IgnoredSubstitutionsList();

	void AddFilter(const String& pattern, const String& replacement, bool useRegExp, bool caseSensitive, bool matchWholeWordOnly, bool enabled);
	size_t GetCount() const;
	void Empty();
	const IgnoredSubstitution &GetAt(size_t ind) const;
	void CloneFrom(const IgnoredSubstitutionsList *list);
	bool Compare(const IgnoredSubstitutionsList *list) const;

	void Initialize(COptionsMgr *pOptionsMgr);
	void SaveFilters();

	const SubstitutionList *MakeSubstitutionList();

private:
	std::vector<IgnoredSubstitution> m_items; /**< List for linefilter items */
	COptionsMgr * m_pOptionsMgr; /**< Options-manager for storage */
};

/**
 * @brief Returns count of items in the list.
 * @return Count of filters in the list.
 */
inline size_t IgnoredSubstitutionsList::GetCount() const
{
	return m_items.size();
}

/**
 * @brief Empties the list.
 */
inline void IgnoredSubstitutionsList::Empty()
{
	m_items.clear();
}
