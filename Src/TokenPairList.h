/** 
 * @file IgnoredSubstitutionsFiltersList.h
 *
 * @brief Declaration file for IgnoredSubstitutionsFiltersList class
 */
#pragma once

#include <vector>
#include <memory>
#include "UnicodeString.h"

class COptionsMgr;

/**
 @brief Structure for filter.
 */
struct TokenPair
{
	String filterStr0;
	String filterStr1;
};

/**
 @brief List of raw Ignored Substitution pairs.
 */
class TokenPairList
{
public:
	TokenPairList();
	~TokenPairList();

	void AddFilter(const String& filter0, const String& filter1);
	size_t GetCount() const;
	void Empty();
	const TokenPair &GetAt(size_t ind) const;
	void CloneFrom(const TokenPairList *list);
	bool Compare(const TokenPairList *list) const;

	void Initialize(COptionsMgr *pOptionsMgr);
	void SaveFilters();

private:
	std::vector<std::shared_ptr<TokenPair>> m_items; /**< List for linefilter items */
	COptionsMgr * m_pOptionsMgr; /**< Options-manager for storage */
};

/**
 * @brief Returns count of items in the list.
 * @return Count of filters in the list.
 */
inline size_t TokenPairList::GetCount() const
{
	return m_items.size();
}

/**
 * @brief Empties the list.
 */
inline void TokenPairList::Empty()
{
	m_items.clear();
}
