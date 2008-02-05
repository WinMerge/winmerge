/** 
 * @file  LineFiltersList.cpp
 *
 * @brief Implementation for LineFiltersList class.
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "LineFiltersList.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

/** @brief Registry key for saving linefilters. */
static const TCHAR FiltersRegPath[] =_T("LineFilters");

/**
 * @brief Default constructor.
 */
LineFiltersList::LineFiltersList()
: m_pOptionsMgr(NULL)
{
}

/**
 * @brief Destructor, empties the list.
 */
LineFiltersList::~LineFiltersList()
{
	Empty();
}

/**
 * @brief Add new filter to the list.
 * @param [in] filter Filter string to add.
 * @param [in] enabled Is filter enabled?
 */
void LineFiltersList::AddFilter(LPCTSTR filter, BOOL enabled)
{
	LineFilterItem item;
	item.enabled = enabled;
	item.filterStr = filter;
	m_items.AddTail(item);
}

/**
 * @brief Returns count of items in the list.
 * @return Count of filters in the list.
 */
int LineFiltersList::GetCount() const
{
	return m_items.GetCount();
}

/**
 * @brief Empties the list.
 */
void LineFiltersList::Empty()
{
	while (!m_items.IsEmpty())
	{
		m_items.RemoveHead();
	}
}

/**
 * @brief Returns the filter list as one filter string.
 * This function returns the list of filters as one string that can be
 * given to regular expression engine as filter. Filter strings in
 * the list are separated by "|".
 * @return Filter string.
 */
String LineFiltersList::GetAsString() const
{
	String filter;
	POSITION pos = m_items.GetHeadPosition();

	while (pos)
	{
		LineFilterItem item = m_items.GetNext(pos);
		if (item.enabled)
		{
			if (!filter.empty())
				filter += _T("|");
			filter += item.filterStr;
		}
	}
	return filter;	
}

/**
 * @brief Return filter from given index.
 * @param [in] ind Index of filter.
 * @return Filter item from the index.
 */
LineFilterItem LineFiltersList::GetAt(int ind)
{
	LineFilterItem item;

	if (ind < m_items.GetCount())
	{
		POSITION pos = m_items.FindIndex(ind);
		if (pos)
		{
			return m_items.GetAt(pos);
		}
	}
	return item;
}

/**
 * @brief Clone filter list from another list.
 * This function clones filter list from another list. Current items in the
 * list are removed and new items added from the given list.
 * @param [in] list List to clone.
 */
void LineFiltersList::CloneFrom(LineFiltersList *list)
{
	Empty();
	int count = list->GetCount();

	for (int i = 0; i < count; i++)
	{
		LineFilterItem item = list->GetAt(i);
		AddFilter(item.filterStr.c_str(), item.enabled);
	}
}

/**
 * @brief Read filter list from the options system.
 * @param [in] pOptionsMgr Pointer to options system.
 */
void LineFiltersList::Initialize(COptionsMgr *pOptionsMgr)
{
	ASSERT(pOptionsMgr);
	String valuename(FiltersRegPath);

	m_pOptionsMgr = pOptionsMgr;

	int count = m_items.GetCount();
	valuename += _T("/Values");
	m_pOptionsMgr->InitOption(valuename.c_str(), count);
	count = m_pOptionsMgr->GetInt(valuename.c_str());

	for (unsigned int i = 0; i < count; i++)
	{
		TCHAR name[100] = {0};
		_sntprintf(name, 99, _T("%s/Filter%02u"), FiltersRegPath, i);
		m_pOptionsMgr->InitOption(name, _T(""));
		String filter = m_pOptionsMgr->GetString(name);

		_sntprintf(name, 99, _T("%s/Enabled%02u"), FiltersRegPath, i);
		m_pOptionsMgr->InitOption(name, (int)TRUE);
		int enabled = m_pOptionsMgr->GetInt(name);
		BOOL bEnabled = enabled ? TRUE : FALSE;

		AddFilter(filter.c_str(), bEnabled);
	}
}

/**
 * @brief Save linefilters to options system.
 */
void LineFiltersList::SaveFilters()
{
	ASSERT(m_pOptionsMgr);
	String valuename(FiltersRegPath);

	int count = m_items.GetCount();
	valuename += _T("/Values");
	m_pOptionsMgr->SetInt(valuename.c_str(), count);

	for (unsigned int i = 0; i < count; i++)
	{
		POSITION pos = m_items.FindIndex(i);
		LineFilterItem item = m_items.GetAt(pos);

		TCHAR name[100] = {0};
		_sntprintf(name, 99, _T("%s/Filter%02u"), FiltersRegPath, i);
		m_pOptionsMgr->InitOption(name, _T(""));
		m_pOptionsMgr->SaveOption(name, item.filterStr.c_str());

		_sntprintf(name, 99, _T("%s/Enabled%02u"), FiltersRegPath, i);
		m_pOptionsMgr->InitOption(name, 0);
		m_pOptionsMgr->SaveOption(name, (int)item.enabled);
	}

	// Remove options we don't need anymore
	// We could have earlier 10 pcs but now we only need 5
	TCHAR filter[100] = {0};
	_sntprintf(filter, 99, _T("%s/Filter%02u"), FiltersRegPath, count);
	int retval1 = m_pOptionsMgr->RemoveOption(filter);

	_sntprintf(filter, 99, _T("%s/Enabled%02u"), FiltersRegPath, count);
	int retval2 = m_pOptionsMgr->RemoveOption(filter);
	
	while (retval1 == OPT_OK || retval2 == OPT_OK)
	{
		++count;
		_sntprintf(filter, 99, _T("%s/Filter%02u"), FiltersRegPath, count);
		retval1 = m_pOptionsMgr->RemoveOption(filter);
		_sntprintf(filter, 99, _T("%s/Enabled%02u"), FiltersRegPath, count);
		retval2 = m_pOptionsMgr->RemoveOption(filter);
	}
}

/**
 * @brief Import old-style filter string into new system.
 * This function imports old-style (2.6.x and earlier) line filters
 * to new linefilter system. Earlier linefilters were saved as one
 * string to registry.
 * @param [in] filters String containing line filters in old-style.
 */
void LineFiltersList::Import(LPCTSTR filters)
{
	const TCHAR sep[] = _T("\r\n");
	TCHAR *p_filters = (TCHAR *)&filters[0];
	TCHAR *token;
	
	if (filters != NULL && _tcslen(filters) > 0)
	{
		// find each regular expression and add to list
		token = _tcstok(p_filters, sep);
		while (token)
		{
			AddFilter(token, TRUE);
			token = _tcstok(NULL, sep);
		}
		SaveFilters();
	}
}
