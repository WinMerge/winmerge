/** 
 * @file  FilterList.h
 *
 * @brief Implementation file for FilterList.
 */

#include "pch.h"
#include "FilterList.h"
#include "FileFilter.h"
#include <vector>
#include <Poco/RegularExpression.h>
#include <Poco/Exception.h>
#include "unicoder.h"

using Poco::RegularExpression;

/** 
 * @brief Constructor.
 */
FilterList::FilterList() = default;

/** 
 * @brief Destructor.
 */
FilterList::~FilterList()
{
	RemoveAllFilters();
}

/** 
 * @brief Add new regular expression to the list.
 * This function adds new regular expression to the list of expressions.
 * The regular expression is compiled and studied for better performance.
 * @param [in] regularExpression Regular expression string.
 * @param [in] encoding Expression encoding.
 */
void FilterList::AddRegExp(const std::string& regularExpression, bool throwIfInvalid)
{
	try
	{
		m_list.push_back(filter_item_ptr(new filter_item(regularExpression, RegularExpression::RE_UTF8)));
	}
	catch (Poco::RegularExpressionException& e)
	{
		if (throwIfInvalid)
			throw std::runtime_error(e.message().c_str());
	}
}

static bool match(const std::vector <filter_item_ptr>& list, const std::string& string)
{
	if (list.empty())
		return false;
	bool retval = false;
	for (const auto& item : list)
	{
		int result = 0;
		RegularExpression::Match match;
		try
		{
			result = item->regexp.match(string, 0, match);
		}
		catch (...)
		{
		}
		if (result > 0)
		{
			retval = true;
			break;
		}
	}
	return retval;
}

/** 
 * @brief Match string against list of expressions.
 * This function matches given @p string against the list of regular
 * expressions. The matching ends when first match is found, so all
 * expressions may not be matched against.
 * @param [in] string string to match.
 * @param [in] codepage codepage of string.
 * @return true if any of the expressions did match the string.
 */
bool FilterList::Match(const std::string& string, int codepage/*=CP_UTF8*/)
{
	// convert string into UTF-8
	ucr::buffer buf(string.length() * 2);

	if (codepage != ucr::CP_UTF_8)
			ucr::convert(ucr::NONE, codepage, reinterpret_cast<const unsigned char *>(string.c_str()), 
					string.length(), ucr::UTF8, ucr::CP_UTF_8, &buf);

	return match(m_list, (buf.size > 0) ? std::string(reinterpret_cast<const char*>(buf.ptr), buf.size) : string);
}

/**
 * @brief Clone filter list from another list.
 * This function clones filter list from another list. Current items in the
 * list are removed and new items added from the given list.
 * @param [in] filterList File list to clone.
 */
void FilterList::CloneFrom(const FilterList* filterList)
{
	if (!filterList)
		return;

	m_list.clear();

	size_t count = filterList->m_list.size();
	m_list.reserve(count);
	for (size_t i = 0; i < count; i++)
	{
		m_list.emplace_back(std::make_shared<filter_item>(filterList->m_list[i].get()));
	}
}
