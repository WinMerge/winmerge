/** 
 * @file  FilterList.h
 *
 * @brief Implementation file for FilterList.
 */

#include "stdafx.h"
#include <vector>
#include "FilterList.h"
#include "pcre.h"

using namespace std;

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
	free(m_lastMatchExpression);
}

/** 
 * @brief Add new regular expression to the list.
 * This function adds new regular expression to the list of expressions.
 * The regular expression is compiled and studied for better performance.
 * @param [in] regularExpression Regular expression string.
 * @param [in] encoding Expression encoding.
 */
void FilterList::AddRegExp(const char * regularExpression, EncodingType encoding)
{
	filter_item item;
	item.filterAsString = strdup(regularExpression);

	const char * errormsg = NULL;
	int erroroffset = 0;
	int pcre_flags = 0;

	if (encoding == ENC_UTF8)
		pcre_flags |= PCRE_UTF8;
	else if (encoding != ENC_ANSI)
		_RPTF0(_CRT_ERROR, "Unregognized regexp encoding!");

	pcre *regexp = pcre_compile(regularExpression, pcre_flags, &errormsg,
		&erroroffset, NULL);
	if (regexp)
	{
		errormsg = NULL;
		pcre_extra *pe = pcre_study(regexp, 0, &errormsg);
		
		item.pRegExp = regexp;
		if (pe != NULL && errormsg != NULL)
			item.pRegExpExtra = pe;
		else
			item.pRegExpExtra = NULL;

		m_list.push_back(item);
	}
}

/** 
 * @brief Removes all expressions from the list.
 */
void FilterList::RemoveAllFilters()
{
	while (!m_list.empty())
	{
		filter_item & item = m_list.back();
		free(item.filterAsString);
		pcre_free(item.pRegExp);
		pcre_free(item.pRegExpExtra);
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
 * @param [in] string UTF-8 string to match.
 * @return true if any of the expressions did match the string.
 */
bool FilterList::Match(const char *string)
{
	bool retval = false;
	unsigned int count = m_list.size();

	unsigned int i = 0;
	while (i < count && retval == false)
	{
		filter_item item = m_list[i];
		int ovector[30];
		pcre * regexp = item.pRegExp;
		pcre_extra * extra = item.pRegExpExtra;
		int result = pcre_exec(regexp, extra, string, strlen(string),
			0, 0, ovector, 30);

		if (result >= 0)
		{
			if (m_lastMatchExpression != NULL)
				free(m_lastMatchExpression);
			m_lastMatchExpression = strdup(item.filterAsString);
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
	return m_lastMatchExpression;
}
