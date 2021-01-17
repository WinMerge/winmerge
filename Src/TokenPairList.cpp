/** 
 * @file  TokenPairList.cpp
 *
 * @brief Implementation for IgnoredSubstitutionsFiltersList class.
 */

#include "pch.h"
#include "TokenPairList.h"
#include <vector>
#include <cassert>
#include "OptionsMgr.h"
#include "UnicodeString.h"

using std::vector;

/** @brief Registry key for saving Ignored Substitutions filters. */
static const TCHAR IgnoredSubstitutionsRegPath[] = _T("IgnoredSubstitutions");

/**
 * @brief Default constructor.
 */
TokenPairList::TokenPairList()
: m_pOptionsMgr(nullptr)
{
}

/**
 * @brief Destructor, empties the list.
 */
TokenPairList::~TokenPairList()
{
}

/**
 * @brief Add new filter to the list.
 * @param [in] filter Filter string to add.
 * @param [in] enabled Is filter enabled?
 */
void TokenPairList::AddFilter(const String& filter0, const String& filter1, bool useRegExp, bool enabled)
{
	std::shared_ptr<TokenPair> item(new TokenPair());
	item->enabled = enabled;
	item->useRegExp = useRegExp;
	item->filterStr0 = filter0;
	item->filterStr1 = filter1;
	m_items.push_back(item);
}

/**
 * @brief Return filter from given index.
 * @param [in] ind Index of filter.
 * @return Filter item from the index. If the index is beyond table limit,
 *  return the last item in the list.
 */
const TokenPair & TokenPairList::GetAt(size_t ind) const
{
	if (ind < m_items.size())
		return *m_items[ind];
	else
		return *m_items.back();
}

/**
 * @brief Clone filter list from another list.
 * This function clones filter list from another list. Current items in the
 * list are removed and new items added from the given list.
 * @param [in] list List to clone.
 */
void TokenPairList::CloneFrom(const TokenPairList *list)
{
	Empty();
	size_t count = list->GetCount();

	for (size_t i = 0; i < count; i++)
	{
		const TokenPair &item = list->GetAt(i);
		AddFilter(item.filterStr0, item.filterStr1, item.useRegExp, item.enabled);
	}
}

/**
 * @brief Compare filter lists.
 * @param [in] list List to compare.
 * @return true if lists are identical, false otherwise.
 */
bool TokenPairList::Compare(const TokenPairList *list) const
{
	if (list->GetCount() != GetCount())
		return false;

	for (size_t i = 0; i < GetCount(); i++)
	{
		const TokenPair &item1 = list->GetAt(i);
		const TokenPair &item2 = GetAt(i);

		if
		(
			   item1.enabled != item2.enabled
			|| item1.useRegExp != item2.useRegExp
			|| item1.filterStr0 != item2.filterStr0
			|| item1.filterStr1 != item2.filterStr1
		)
			return false;
	}
	return true;
}

/**
 * @brief Read filter list from the options system.
 * @param [in] pOptionsMgr Pointer to options system.
 */
void TokenPairList::Initialize(COptionsMgr *pOptionsMgr)
{
	assert(pOptionsMgr != nullptr);
	String valuename(IgnoredSubstitutionsRegPath);

	m_pOptionsMgr = pOptionsMgr;

	size_t count = m_items.size();
	valuename += _T("/Values");
	m_pOptionsMgr->InitOption(valuename, static_cast<int>(count));
	count = m_pOptionsMgr->GetInt(valuename);

	for (unsigned i = 0; i < count; i++)
	{
		String nameEnabled = strutils::format(_T("%s/Enabled%02u"), IgnoredSubstitutionsRegPath, i);
		m_pOptionsMgr->InitOption(nameEnabled, true);
		bool enabled = m_pOptionsMgr->GetBool(nameEnabled);

		String nameUseRegExp = strutils::format(_T("%s/UseRegExp%02u"), IgnoredSubstitutionsRegPath, i);
		m_pOptionsMgr->InitOption(nameUseRegExp, true);
		bool useRegExp = m_pOptionsMgr->GetBool(nameUseRegExp);

		String name0 = strutils::format(_T("%s/Filter%02u_0"), IgnoredSubstitutionsRegPath, i);
		m_pOptionsMgr->InitOption(name0, _T(""));
		String filterStr0 = m_pOptionsMgr->GetString(name0);

		String name1 = strutils::format(_T("%s/Filter%02u_1"), IgnoredSubstitutionsRegPath, i);
		m_pOptionsMgr->InitOption(name1, _T(""));
		String filterStr1 = m_pOptionsMgr->GetString(name1);

		AddFilter(filterStr0, filterStr1, useRegExp, enabled);
	}
}

/**
 * @brief Save Ignored Substitutions to options system.
 */
void TokenPairList::SaveFilters()
{
	assert(m_pOptionsMgr != nullptr);
	String valuename(IgnoredSubstitutionsRegPath);

	size_t count = m_items.size();
	valuename += _T("/Values");
	m_pOptionsMgr->SaveOption(valuename, static_cast<int>(count));

	for (size_t i = 0; i < count; i++)
	{
		const std::shared_ptr<TokenPair> &item = m_items[i];

		String name0 = strutils::format(_T("%s/Filter%02u_0"), IgnoredSubstitutionsRegPath, i);
		m_pOptionsMgr->InitOption(name0, _T(""));
		m_pOptionsMgr->SaveOption(name0, item->filterStr0);

		String name1 = strutils::format(_T("%s/Filter%02u_1"), IgnoredSubstitutionsRegPath, i);
		m_pOptionsMgr->InitOption(name1, _T(""));
		m_pOptionsMgr->SaveOption(name1, item->filterStr1);
	}

	// Remove options we don't need anymore
	// We could have earlier 10 pcs but now we only need 5
	String filter = strutils::format(_T("%s/Enabled%02u"), IgnoredSubstitutionsRegPath, count);
	int retval = m_pOptionsMgr->RemoveOption(filter);

	String filter0 = strutils::format(_T("%s/Filter%02u_0"), IgnoredSubstitutionsRegPath, count);
	int retval0 = m_pOptionsMgr->RemoveOption(filter0);

	String filter1 = strutils::format(_T("%s/Filter%02u_1"), IgnoredSubstitutionsRegPath, count);
	int retval1 = m_pOptionsMgr->RemoveOption(filter1);

	while (retval == COption::OPT_OK || retval0 == COption::OPT_OK || retval1 == COption::OPT_OK)
	{
		++count;
		filter = strutils::format(_T("%s/Enabled%02u"), IgnoredSubstitutionsRegPath, count);
		retval = m_pOptionsMgr->RemoveOption(filter);
		filter0 = strutils::format(_T("%s/Filter%02u_0"), IgnoredSubstitutionsRegPath, count);
		retval0 = m_pOptionsMgr->RemoveOption(filter);
		filter1 = strutils::format(_T("%s/Filter%02u_1"), IgnoredSubstitutionsRegPath, count);
		retval1 = m_pOptionsMgr->RemoveOption(filter);
	}
}
