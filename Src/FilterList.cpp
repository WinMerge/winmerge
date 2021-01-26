/** 
 * @file  FilterList.h
 *
 * @brief Implementation file for FilterList.
 */

#include "pch.h"
#include "FilterList.h"
#include <vector>
#include <Poco/RegularExpression.h>
#include "unicoder.h"

using Poco::RegularExpression;

/** 
 * @brief Constructor.
 */
 FilterList::FilterList()
: m_lastMatchExpression(nullptr)
{
}

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
void FilterList::AddRegExp(const std::string& regularExpression)
{
	try
	{
		m_list.push_back(filter_item_ptr(new filter_item(regularExpression, RegularExpression::RE_UTF8)));
	}
	catch (...)
	{
		// TODO:
	}
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
	bool retval = false;
	const size_t count = m_list.size();

	// convert string into UTF-8
	ucr::buffer buf(string.length() * 2);

	if (codepage != ucr::CP_UTF_8)
			ucr::convert(ucr::NONE, codepage, reinterpret_cast<const unsigned char *>(string.c_str()), 
					string.length(), ucr::UTF8, ucr::CP_UTF_8, &buf);

	unsigned i = 0;
	while (i < count && !retval)
	{
		const filter_item_ptr& item = m_list[i];
		int result = 0;
		RegularExpression::Match match;
		try
		{
			if (buf.size > 0)
				result = item->regexp.match(std::string(reinterpret_cast<const char *>(buf.ptr), buf.size), 0, match);
			else
				result = item->regexp.match(string, 0, match);
		}
		catch (...)
		{
			// TODO:
		}
		if (result > 0)
		{
			m_lastMatchExpression = &item->filterAsString;
			retval = true;
		}
		else
			++i;
	}

	return retval;
}

