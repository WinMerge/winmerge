/** 
 * @file  FilterList.h
 *
 * @brief Implementation file for FilterList.
 */
// ID line follows -- this is updated by SVN
// $Id: FilterList.cpp 7164 2010-05-15 13:57:32Z jtuc $

#include "FilterList.h"
#include <vector>
#include <Poco/RegularExpression.h>
#include "unicoder.h"

using Poco::RegularExpression;

/** 
 * @brief Constructor.
 */
 FilterList::FilterList()
: m_lastMatchExpression(NULL)
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
		m_list.push_back(new filter_item(regularExpression, RegularExpression::RE_UTF8));
	}
	catch (...)
	{
		// TODO:
	}
}

/** 
 * @brief Removes all expressions from the list.
 */
void FilterList::RemoveAllFilters()
{
	while (!m_list.empty())
	{
		filter_item *item = m_list.back();
		delete item;
		m_list.pop_back();
	}
}

/** 
 * @brief Returns if list has any expressions.
 * @return true if list contains one or more expressions.
 */
bool FilterList::HasRegExps()
{
	return !m_list.empty();
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

	if (codepage != CP_UTF8)
			ucr::convert(ucr::NONE, codepage, reinterpret_cast<const unsigned char *>(string.c_str()), 
					string.length(), ucr::UTF8, CP_UTF8, &buf);

	unsigned i = 0;
	while (i < count && retval == false)
	{
		const filter_item* item = m_list[i];
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

/** 
 * @brief Returns the last matched expression (if any).
 * This function returns the regular expression string that matched last.
 * @return Last matched expression, or NULL in case no matches yet.
 */
const char * FilterList::GetLastMatchExpression()
{
	return m_lastMatchExpression->c_str();
}
