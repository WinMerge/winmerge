/** 
 * @file  IgnoredSubstitutionsList.cpp
 *
 * @brief Implementation for IgnoredSubstitutionsFiltersList class.
 */

#include "pch.h"
#include "IgnoredSubstitutionsList.h"
#include "SubstitutionList.h"
#include <vector>
#include <cassert>
#include <Poco/Exception.h>
#include "OptionsMgr.h"
#include "UnicodeString.h"

/** @brief Registry key for saving Ignored Substitutions filters. */
static const TCHAR IgnoredSubstitutionsRegPath[] = _T("IgnoredSubstitutions");

/**
 * @brief Default constructor.
 */
IgnoredSubstitutionsList::IgnoredSubstitutionsList()
: m_pOptionsMgr(nullptr)
{
}

/**
 * @brief Destructor, empties the list.
 */
IgnoredSubstitutionsList::~IgnoredSubstitutionsList()
{
}

/**
 * @brief Add new filter to the list.
 * @param [in] filter Filter string to add.
 * @param [in] enabled Is filter enabled?
 */
void IgnoredSubstitutionsList::Add(const String& filter0, const String& filter1,
	bool useRegExp, bool caseSensitive, bool matchWholeWordOnly, bool enabled)
{
	IgnoredSubstitution item;
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
const IgnoredSubstitution& IgnoredSubstitutionsList::GetAt(size_t ind) const
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
void IgnoredSubstitutionsList::CloneFrom(const IgnoredSubstitutionsList *list)
{
	Empty();
	size_t count = list->GetCount();

	for (size_t i = 0; i < count; i++)
	{
		const IgnoredSubstitution &item = list->GetAt(i);
		Add(item.pattern, item.replacement, item.useRegExp,
			item.caseSensitive, item.matchWholeWordOnly, item.enabled);
	}
}

/**
 * @brief Compare filter lists.
 * @param [in] list List to compare.
 * @return true if lists are identical, false otherwise.
 */
bool IgnoredSubstitutionsList::Compare(const IgnoredSubstitutionsList *list) const
{
	if (list->GetCount() != GetCount())
		return false;

	for (size_t i = 0; i < GetCount(); i++)
	{
		const IgnoredSubstitution &item1 = list->GetAt(i);
		const IgnoredSubstitution &item2 = GetAt(i);

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
void IgnoredSubstitutionsList::Initialize(COptionsMgr *pOptionsMgr)
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
		m_pOptionsMgr->InitOption(nameUseRegExp, false);
		bool useRegExp = m_pOptionsMgr->GetBool(nameUseRegExp);

		String nameCaseSensitive = strutils::format(_T("%s/CaseSensitive%02u"), IgnoredSubstitutionsRegPath, i);
		m_pOptionsMgr->InitOption(nameCaseSensitive, false);
		bool caseSensitive = m_pOptionsMgr->GetBool(nameCaseSensitive);

		String nameMatchWholeWordOnly = strutils::format(_T("%s/MatchWholeWordOnly%02u"), IgnoredSubstitutionsRegPath, i);
		m_pOptionsMgr->InitOption(nameMatchWholeWordOnly, false);
		bool matchWholeWordOnly = m_pOptionsMgr->GetBool(nameMatchWholeWordOnly);

		String name0 = strutils::format(_T("%s/Pattern%02u"), IgnoredSubstitutionsRegPath, i);
		m_pOptionsMgr->InitOption(name0, _T(""));
		String pattern = m_pOptionsMgr->GetString(name0);

		String name1 = strutils::format(_T("%s/Replacement%02u"), IgnoredSubstitutionsRegPath, i);
		m_pOptionsMgr->InitOption(name1, _T(""));
		String replacement = m_pOptionsMgr->GetString(name1);

		Add(pattern, replacement, useRegExp, caseSensitive, matchWholeWordOnly, enabled);
	}
}

/**
 * @brief Save Ignored Substitutions to options system.
 */
void IgnoredSubstitutionsList::SaveFilters()
{
	assert(m_pOptionsMgr != nullptr);
	String valuename(IgnoredSubstitutionsRegPath);

	size_t count = m_items.size();
	valuename += _T("/Values");
	m_pOptionsMgr->SaveOption(valuename, static_cast<int>(count));

	for (size_t i = 0; i < count; i++)
	{
		const IgnoredSubstitution& item = m_items[i];

		String nameEnabled = strutils::format(_T("%s/Enabled%02u"), IgnoredSubstitutionsRegPath, i);
		m_pOptionsMgr->InitOption(nameEnabled, true);
		m_pOptionsMgr->SaveOption(nameEnabled, item.enabled);

		String nameUseRegExp = strutils::format(_T("%s/UseRegExp%02u"), IgnoredSubstitutionsRegPath, i);
		m_pOptionsMgr->InitOption(nameUseRegExp, false);
		m_pOptionsMgr->SaveOption(nameUseRegExp, item.useRegExp);

		String nameCaseSensitive = strutils::format(_T("%s/CaseSensitive%02u"), IgnoredSubstitutionsRegPath, i);
		m_pOptionsMgr->InitOption(nameCaseSensitive, false);
		m_pOptionsMgr->SaveOption(nameCaseSensitive, item.caseSensitive);

		String nameMatchWholeWordOnly = strutils::format(_T("%s/MatchWholeWordOnly%02u"), IgnoredSubstitutionsRegPath, i);
		m_pOptionsMgr->InitOption(nameMatchWholeWordOnly, false);
		m_pOptionsMgr->SaveOption(nameMatchWholeWordOnly, item.matchWholeWordOnly);

		String name0 = strutils::format(_T("%s/Pattern%02u"), IgnoredSubstitutionsRegPath, i);
		m_pOptionsMgr->InitOption(name0, _T(""));
		m_pOptionsMgr->SaveOption(name0, item.pattern);

		String name1 = strutils::format(_T("%s/Replacement%02u"), IgnoredSubstitutionsRegPath, i);
		m_pOptionsMgr->InitOption(name1, _T(""));
		m_pOptionsMgr->SaveOption(name1, item.replacement);
	}

	// Remove options we don't need anymore
	// We could have earlier 10 pcs but now we only need 5
	String filterEnabled = strutils::format(_T("%s/Enabled%02u"), IgnoredSubstitutionsRegPath, count);
	int retvalEnabled = m_pOptionsMgr->RemoveOption(filterEnabled);

	String filterUseRegExp = strutils::format(_T("%s/UseRegExp%02u"), IgnoredSubstitutionsRegPath, count);
	int retvalUseRegExp = m_pOptionsMgr->RemoveOption(filterUseRegExp);

	String filterCaseSensitive = strutils::format(_T("%s/CaseSensitive%02u"), IgnoredSubstitutionsRegPath, count);
	int retvalCaseSensitive = m_pOptionsMgr->RemoveOption(filterCaseSensitive);

	String filterMatchWholeWordOnly = strutils::format(_T("%s/MatchWholeWordOnly%02u"), IgnoredSubstitutionsRegPath, count);
	int retvalMatchWholeWordOnly = m_pOptionsMgr->RemoveOption(filterMatchWholeWordOnly);

	String filter0 = strutils::format(_T("%s/Pattern%02u"), IgnoredSubstitutionsRegPath, count);
	int retval0 = m_pOptionsMgr->RemoveOption(filter0);

	String filter1 = strutils::format(_T("%s/Replacement%02u"), IgnoredSubstitutionsRegPath, count);
	int retval1 = m_pOptionsMgr->RemoveOption(filter1);

	while (retvalEnabled == COption::OPT_OK || retvalUseRegExp == COption::OPT_OK ||
		retvalCaseSensitive == COption::OPT_OK || retvalMatchWholeWordOnly == COption::OPT_OK ||
		retval0 == COption::OPT_OK || retval1 == COption::OPT_OK)
	{
		++count;
		filterEnabled = strutils::format(_T("%s/Enabled%02u"), IgnoredSubstitutionsRegPath, count);
		retvalEnabled = m_pOptionsMgr->RemoveOption(filterEnabled);
		filterUseRegExp = strutils::format(_T("%s/UseRegExp%02u"), IgnoredSubstitutionsRegPath, count);
		retvalUseRegExp = m_pOptionsMgr->RemoveOption(filterUseRegExp);
		filterCaseSensitive = strutils::format(_T("%s/CaseSensitive%02u"), IgnoredSubstitutionsRegPath, count);
		retvalCaseSensitive = m_pOptionsMgr->RemoveOption(filterCaseSensitive);
		filterMatchWholeWordOnly = strutils::format(_T("%s/MatchWholeWordOnly%02u"), IgnoredSubstitutionsRegPath, count);
		retvalMatchWholeWordOnly = m_pOptionsMgr->RemoveOption(filterMatchWholeWordOnly);
		filter0 = strutils::format(_T("%s/Pattern%02u"), IgnoredSubstitutionsRegPath, count);
		retval0 = m_pOptionsMgr->RemoveOption(filter0);
		filter1 = strutils::format(_T("%s/Replacement%02u"), IgnoredSubstitutionsRegPath, count);
		retval1 = m_pOptionsMgr->RemoveOption(filter1);
	}
}

const SubstitutionList* IgnoredSubstitutionsList::MakeSubstitutionList(bool throwIfInvalid)
{
	int i = 0;
	SubstitutionList *plist = new SubstitutionList();
	for (auto& item : m_items)
	{
		if (item.enabled)
		{
			try
			{
				if (item.useRegExp)
				{
					plist->Add(
						ucr::toUTF8(item.pattern),
						ucr::toUTF8(item.replacement),
						item.caseSensitive ? 0 : Poco::RegularExpression::RE_CASELESS);
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
					delete plist;
					char msg[512];
					_snprintf_s(msg, _TRUNCATE, "#%d: %s", i + 1, e.message().c_str());
					throw Poco::RegularExpressionException(msg, e.code());
				}
			}
		}
		i++;
	}
	return plist;
}
