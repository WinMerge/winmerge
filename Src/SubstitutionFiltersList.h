/** 
 * @file SubstitutionFiltersList.h
 *
 * @brief Declaration file for SubstitutionFiltersList class
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
struct SubstitutionFilter
{
	bool enabled = false;
	bool useRegExp = false;
	bool caseSensitive = false;
	bool matchWholeWordOnly = false;
	String pattern;
	String replacement;
};

/**
 @brief List of raw Ignored Substitution pairs.
 */
class SubstitutionFiltersList
{
public:
	SubstitutionFiltersList();
	~SubstitutionFiltersList();

	void Add(const String& pattern, const String& replacement, bool useRegExp, bool caseSensitive, bool matchWholeWordOnly, bool enabled);
	size_t GetCount() const;
	void Empty();
	const SubstitutionFilter &GetAt(size_t ind) const;
	void CloneFrom(const SubstitutionFiltersList *list);
	bool Compare(const SubstitutionFiltersList *list) const;
	void SetEnabled(bool enabled) { m_enabled = enabled; }
	bool GetEnabled() const { return m_enabled; }

	void Initialize(COptionsMgr *pOptionsMgr);
	void SaveFilters();

	std::shared_ptr<SubstitutionList> MakeSubstitutionList(bool throwIfInvalid = false);

private:
	bool m_enabled;
	std::vector<SubstitutionFilter> m_items; /**< List for linefilter items */
	COptionsMgr * m_pOptionsMgr; /**< Options-manager for storage */
};

/**
 * @brief Returns count of items in the list.
 * @return Count of filters in the list.
 */
inline size_t SubstitutionFiltersList::GetCount() const
{
	return m_items.size();
}

/**
 * @brief Empties the list.
 */
inline void SubstitutionFiltersList::Empty()
{
	m_items.clear();
}
