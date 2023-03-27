/** 
 * @file  SubstitutionFiltersList.cpp
 *
 * @brief Implementation for SubstitutionFiltersList class.
 */

#include "pch.h"
#include "SubstitutionFiltersList.h"
#include "SubstitutionList.h"
#include <vector>
#include <cassert>
#include <Poco/Exception.h>
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "UnicodeString.h"

/** @brief Registry key for saving Substitution filters. */
static const tchar_t SubstitutionFiltersRegPath[] = _T("SubstitutionFilters");

/**
 * @brief Default constructor.
 */
SubstitutionFiltersList::SubstitutionFiltersList()
: m_pOptionsMgr(nullptr)
{
}

/**
 * @brief Destructor, empties the list.
 */
SubstitutionFiltersList::~SubstitutionFiltersList() = default;

/**
 * @brief Add new filter to the list.
 * @param [in] filter Filter string to add.
 * @param [in] enabled Is filter enabled?
 */
void SubstitutionFiltersList::Add(const String& filter0, const String& filter1,
	bool useRegExp, bool caseSensitive, bool matchWholeWordOnly, bool enabled)
{
	SubstitutionFilter item;
	item.useRegExp = useRegExp;
	item.caseSensitive = caseSensitive;
	item.matchWholeWordOnly = matchWholeWordOnly;
	item.pattern = filter0;
	item.replacement = filter1;
	item.enabled = enabled;
	m_items.emplace_back(item);
}

/**
 * @brief Return filter from given index.
 * @param [in] ind Index of filter.
 * @return Filter item from the index. If the index is beyond table limit,
 *  return the last item in the list.
 */
const SubstitutionFilter& SubstitutionFiltersList::GetAt(size_t ind) const
{
	if (ind < m_items.size())
		return m_items[ind];
	else
		return m_items.back();
}

/**
 * @brief Clone filter list from another list.
 * This function clones filter list from another list. Current items in the
 * list are removed and new items added from the given list.
 * @param [in] list List to clone.
 */
void SubstitutionFiltersList::CloneFrom(const SubstitutionFiltersList *list)
{
	Empty();
	size_t count = list->GetCount();
	m_enabled = list->m_enabled;
	for (size_t i = 0; i < count; i++)
	{
		const SubstitutionFilter &item = list->GetAt(i);
		Add(item.pattern, item.replacement, item.useRegExp,
			item.caseSensitive, item.matchWholeWordOnly, item.enabled);
	}
}

/**
 * @brief Compare filter lists.
 * @param [in] list List to compare.
 * @return true if lists are identical, false otherwise.
 */
bool SubstitutionFiltersList::Compare(const SubstitutionFiltersList *list) const
{
	if (list->GetCount() != GetCount())
		return false;
	if (list->GetEnabled() != GetEnabled())
		return false;

	for (size_t i = 0; i < GetCount(); i++)
	{
		const SubstitutionFilter &item1 = list->GetAt(i);
		const SubstitutionFilter &item2 = GetAt(i);

		if
		(
			   item1.enabled != item2.enabled
			|| item1.useRegExp != item2.useRegExp
			|| item1.caseSensitive != item2.caseSensitive
			|| item1.matchWholeWordOnly != item2.matchWholeWordOnly
			|| item1.pattern != item2.pattern
			|| item1.replacement != item2.replacement
		)
			return false;
	}
	return true;
}

/**
 * @brief Read filter list from the options system.
 * @param [in] pOptionsMgr Pointer to options system.
 */
void SubstitutionFiltersList::Initialize(COptionsMgr *pOptionsMgr)
{
	assert(pOptionsMgr != nullptr);
	String valuename(SubstitutionFiltersRegPath);

	m_pOptionsMgr = pOptionsMgr;

	m_enabled = m_pOptionsMgr->GetBool(OPT_SUBSTITUTION_FILTERS_ENABLED);

	Empty();

	valuename += _T("/Values");
	m_pOptionsMgr->InitOption(valuename, 0);
	size_t count = m_pOptionsMgr->GetInt(valuename);

	for (unsigned i = 0; i < count; i++)
	{
		String nameEnabled = strutils::format(_T("%s/Enabled%02u"), SubstitutionFiltersRegPath, i);
		m_pOptionsMgr->InitOption(nameEnabled, true);
		bool enabled = m_pOptionsMgr->GetBool(nameEnabled);

		String nameUseRegExp = strutils::format(_T("%s/UseRegExp%02u"), SubstitutionFiltersRegPath, i);
		m_pOptionsMgr->InitOption(nameUseRegExp, false);
		bool useRegExp = m_pOptionsMgr->GetBool(nameUseRegExp);

		String nameCaseSensitive = strutils::format(_T("%s/CaseSensitive%02u"), SubstitutionFiltersRegPath, i);
		m_pOptionsMgr->InitOption(nameCaseSensitive, false);
		bool caseSensitive = m_pOptionsMgr->GetBool(nameCaseSensitive);

		String nameMatchWholeWordOnly = strutils::format(_T("%s/MatchWholeWordOnly%02u"), SubstitutionFiltersRegPath, i);
		m_pOptionsMgr->InitOption(nameMatchWholeWordOnly, false);
		bool matchWholeWordOnly = m_pOptionsMgr->GetBool(nameMatchWholeWordOnly);

		String name0 = strutils::format(_T("%s/Pattern%02u"), SubstitutionFiltersRegPath, i);
		m_pOptionsMgr->InitOption(name0, _T(""));
		String pattern = m_pOptionsMgr->GetString(name0);

		String name1 = strutils::format(_T("%s/Replacement%02u"), SubstitutionFiltersRegPath, i);
		m_pOptionsMgr->InitOption(name1, _T(""));
		String replacement = m_pOptionsMgr->GetString(name1);

		Add(pattern, replacement, useRegExp, caseSensitive, matchWholeWordOnly, enabled);
	}
}

/**
 * @brief Save Substitution Filters to options system.
 */
void SubstitutionFiltersList::SaveFilters()
{
	assert(m_pOptionsMgr != nullptr);
	String valuename(SubstitutionFiltersRegPath);

	m_pOptionsMgr->SaveOption(OPT_SUBSTITUTION_FILTERS_ENABLED, m_enabled);

	size_t count = m_items.size();
	valuename += _T("/Values");
	m_pOptionsMgr->SaveOption(valuename, static_cast<int>(count));

	for (size_t i = 0; i < count; i++)
	{
		const SubstitutionFilter& item = m_items[i];

		String nameEnabled = strutils::format(_T("%s/Enabled%02u"), SubstitutionFiltersRegPath, i);
		m_pOptionsMgr->InitOption(nameEnabled, true);
		m_pOptionsMgr->SaveOption(nameEnabled, item.enabled);

		String nameUseRegExp = strutils::format(_T("%s/UseRegExp%02u"), SubstitutionFiltersRegPath, i);
		m_pOptionsMgr->InitOption(nameUseRegExp, false);
		m_pOptionsMgr->SaveOption(nameUseRegExp, item.useRegExp);

		String nameCaseSensitive = strutils::format(_T("%s/CaseSensitive%02u"), SubstitutionFiltersRegPath, i);
		m_pOptionsMgr->InitOption(nameCaseSensitive, false);
		m_pOptionsMgr->SaveOption(nameCaseSensitive, item.caseSensitive);

		String nameMatchWholeWordOnly = strutils::format(_T("%s/MatchWholeWordOnly%02u"), SubstitutionFiltersRegPath, i);
		m_pOptionsMgr->InitOption(nameMatchWholeWordOnly, false);
		m_pOptionsMgr->SaveOption(nameMatchWholeWordOnly, item.matchWholeWordOnly);

		String name0 = strutils::format(_T("%s/Pattern%02u"), SubstitutionFiltersRegPath, i);
		m_pOptionsMgr->InitOption(name0, _T(""));
		m_pOptionsMgr->SaveOption(name0, item.pattern);

		String name1 = strutils::format(_T("%s/Replacement%02u"), SubstitutionFiltersRegPath, i);
		m_pOptionsMgr->InitOption(name1, _T(""));
		m_pOptionsMgr->SaveOption(name1, item.replacement);
	}

	// Remove options we don't need anymore
	// We could have earlier 10 pcs but now we only need 5
	String filterEnabled = strutils::format(_T("%s/Enabled%02u"), SubstitutionFiltersRegPath, count);
	int retvalEnabled = m_pOptionsMgr->RemoveOption(filterEnabled);

	String filterUseRegExp = strutils::format(_T("%s/UseRegExp%02u"), SubstitutionFiltersRegPath, count);
	int retvalUseRegExp = m_pOptionsMgr->RemoveOption(filterUseRegExp);

	String filterCaseSensitive = strutils::format(_T("%s/CaseSensitive%02u"), SubstitutionFiltersRegPath, count);
	int retvalCaseSensitive = m_pOptionsMgr->RemoveOption(filterCaseSensitive);

	String filterMatchWholeWordOnly = strutils::format(_T("%s/MatchWholeWordOnly%02u"), SubstitutionFiltersRegPath, count);
	int retvalMatchWholeWordOnly = m_pOptionsMgr->RemoveOption(filterMatchWholeWordOnly);

	String filter0 = strutils::format(_T("%s/Pattern%02u"), SubstitutionFiltersRegPath, count);
	int retval0 = m_pOptionsMgr->RemoveOption(filter0);

	String filter1 = strutils::format(_T("%s/Replacement%02u"), SubstitutionFiltersRegPath, count);
	int retval1 = m_pOptionsMgr->RemoveOption(filter1);

	while (retvalEnabled == COption::OPT_OK || retvalUseRegExp == COption::OPT_OK ||
		retvalCaseSensitive == COption::OPT_OK || retvalMatchWholeWordOnly == COption::OPT_OK ||
		retval0 == COption::OPT_OK || retval1 == COption::OPT_OK)
	{
		++count;
		filterEnabled = strutils::format(_T("%s/Enabled%02u"), SubstitutionFiltersRegPath, count);
		retvalEnabled = m_pOptionsMgr->RemoveOption(filterEnabled);
		filterUseRegExp = strutils::format(_T("%s/UseRegExp%02u"), SubstitutionFiltersRegPath, count);
		retvalUseRegExp = m_pOptionsMgr->RemoveOption(filterUseRegExp);
		filterCaseSensitive = strutils::format(_T("%s/CaseSensitive%02u"), SubstitutionFiltersRegPath, count);
		retvalCaseSensitive = m_pOptionsMgr->RemoveOption(filterCaseSensitive);
		filterMatchWholeWordOnly = strutils::format(_T("%s/MatchWholeWordOnly%02u"), SubstitutionFiltersRegPath, count);
		retvalMatchWholeWordOnly = m_pOptionsMgr->RemoveOption(filterMatchWholeWordOnly);
		filter0 = strutils::format(_T("%s/Pattern%02u"), SubstitutionFiltersRegPath, count);
		retval0 = m_pOptionsMgr->RemoveOption(filter0);
		filter1 = strutils::format(_T("%s/Replacement%02u"), SubstitutionFiltersRegPath, count);
		retval1 = m_pOptionsMgr->RemoveOption(filter1);
	}
}

std::shared_ptr<SubstitutionList> SubstitutionFiltersList::MakeSubstitutionList(bool throwIfInvalid)
{
	int i = 0;
	std::shared_ptr<SubstitutionList> plist(new SubstitutionList);
	for (auto& item : m_items)
	{
		if (item.enabled && !item.pattern.empty())
		{
			try
			{
				if (item.useRegExp)
				{
					plist->Add(
						ucr::toUTF8(item.pattern),
						ucr::toUTF8(item.replacement),
						(item.caseSensitive ? 0 : Poco::RegularExpression::RE_CASELESS) |
						 Poco::RegularExpression::RE_MULTILINE);
				}
				else
				{
					plist->Add(
						ucr::toUTF8(item.pattern),
						ucr::toUTF8(item.replacement),
						item.caseSensitive, item.matchWholeWordOnly);
				}
			}
			catch (const Poco::RegularExpressionException& e)
			{
				if (throwIfInvalid)
				{
					plist.reset();
					const String msg = strutils::format(_T("#%d: %s"), i + 1, e.message().c_str());
					throw Poco::RegularExpressionException(ucr::toUTF8(msg).c_str(), e.code());
				}
			}
		}
		i++;
	}
	return plist;
}
