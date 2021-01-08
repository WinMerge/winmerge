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

static std::string ExtractCommonPrefix(const std::string& str0, const std::string& str1)
{
	size_t strlen0 = str0.length();
	size_t strlen1 = str1.length();
	size_t minStrlen = strlen0 < strlen1 ? strlen0 : strlen1;
	for (size_t c = 0; c < minStrlen; c++)
	{
		if (str0[c] != str1[c])
			return str0.substr(0, c);
	}
	return strlen0 < strlen1 ? str0 : str1;
}

static std::string ExtractCommonSuffix(const std::string& str0, const std::string& str1)
{
	size_t strlen0 = str0.length();
	size_t strlen1 = str1.length();
	size_t minStrlen = strlen0 < strlen1 ? strlen0 : strlen1;
	for (size_t c = 0; c < minStrlen; c++)
	{
		if (str0[strlen0 - 1 - c] != str1[strlen1 - 1 - c])
			return str0.substr(strlen0 - c, c);
	}
	return strlen0 < strlen1 ? str0 : str1;
}

template<int index>
static std::string ExtractMiddleChangedPart(const std::string& str0, const std::string& str1)
{
	size_t strlen[2]{ str0.length(), str1.length() };
	size_t minStrlen = strlen[0] < strlen[1] ? strlen[0] : strlen[1];
	size_t diffStart = 0;
	while (diffStart < minStrlen && str0[diffStart] == str1[diffStart])
		diffStart++;
	size_t diffEnd = minStrlen - 1;
	while (diffEnd < minStrlen && str0[strlen[0] - 1 - diffEnd] == str1[strlen[1] - 1 - diffEnd])
		diffEnd++;

	const std::string* strs[2]{ &str0, &str1 };
	return strs[index]->substr(diffStart, strlen[index] - diffEnd - 1);
}

IgnoredSusbstitutionItem::IgnoredSusbstitutionItem
(
	const std::string& filter0, const std::string& filter1,
	int regexpCompileOptions, bool extractCommonSufixAndPrefix
)
	: CommonPrefix(extractCommonSufixAndPrefix ? ExtractCommonPrefix(filter0, filter1) : "")
	, CommonPrefixLength(CommonPrefix.length())
	, ChangedPart
	{
		extractCommonSufixAndPrefix ? ExtractMiddleChangedPart<0>(filter0, filter1) : filter0,
		extractCommonSufixAndPrefix ? ExtractMiddleChangedPart<1>(filter0, filter1) : filter1
	}
	, CommonSuffix(extractCommonSufixAndPrefix ? ExtractCommonSuffix(filter0, filter1) : "")
	, CommonSuffixLength(CommonSuffix.length())
	, ChangedPartRegexp
	{
		Poco::RegularExpression(ChangedPart[0], regexpCompileOptions),
		Poco::RegularExpression(ChangedPart[1], regexpCompileOptions)
	}
{
}

IgnoredSubstitutionsFilterList::IgnoredSubstitutionsFilterList()
{
}

IgnoredSubstitutionsFilterList::~IgnoredSubstitutionsFilterList()
{
	RemoveAllFilters();
}

void IgnoredSubstitutionsFilterList::Add(const std::string& change0, const std::string& change1, bool extractCommonSufixAndPrefix)
{
	try
	{
		m_list.push_back
		(
			std::shared_ptr<IgnoredSusbstitutionItem>(
				new IgnoredSusbstitutionItem(change0, change1, RegularExpression::RE_UTF8, extractCommonSufixAndPrefix)
		));
	}
	catch (...)
	{
		// TODO:
	}
}

bool IgnoredSubstitutionsFilterList::MatchBoth
(
	size_t filterIndex,
	const std::string& string0,
	const std::string& string1,
	int codepage/*=CP_UTF8*/
) const
{
	bool retval = false;
	const size_t count = m_list.size();

	if (filterIndex >= count)
		return false;

	// convert string into UTF-8
	ucr::buffer buf0(string0.length() * 2);
	ucr::buffer buf1(string1.length() * 2);

	if (codepage != ucr::CP_UTF_8)
	{
		ucr::convert(ucr::NONE, codepage, reinterpret_cast<const unsigned char*>(string0.c_str()),
			string0.length(), ucr::UTF8, ucr::CP_UTF_8, &buf0);
		ucr::convert(ucr::NONE, codepage, reinterpret_cast<const unsigned char*>(string1.c_str()),
			string1.length(), ucr::UTF8, ucr::CP_UTF_8, &buf1);
	}

	const IgnoredSusbstitutionItem &item = *m_list[filterIndex];
	int result = 0;
	RegularExpression::Match match;
	try
	{
		if (buf0.size > 0 && buf1.size > 0)
		{
			result =
			   item.ChangedPartRegexp[0].match(std::string(reinterpret_cast<const char*>(buf0.ptr), buf0.size), 0, match)
			&& item.ChangedPartRegexp[1].match(std::string(reinterpret_cast<const char*>(buf1.ptr), buf1.size), 0, match);
		}
		else
		{
			result =
			   item.ChangedPartRegexp[0].match(string0, 0, match)
			&& item.ChangedPartRegexp[1].match(string1, 0, match);
		}
	}
	catch (...)
	{
		// TODO:
	}
	if (result > 0)
	{
		retval = true;
	}

	return retval;
}

void IgnoredSubstitutionsFilterList::RemoveAllFilters()
{
	m_list.clear();
}

bool IgnoredSubstitutionsFilterList::HasRegExps() const
{
	return !m_list.empty();
}

const IgnoredSusbstitutionItem &IgnoredSubstitutionsFilterList::operator[](int index) const
{
	return *m_list[index];
}


