/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////

/** 
 * @file  UnicodeString.cpp
 *
 * @brief String utilities.
 */
// ID line follows -- this is updated by SVN
// $Id$

#include <tchar.h>
#include "UnicodeString.h"

/**
 * @brief Convert a string to lower case string.
 * @param [in] str String to convert to lower case.
 * @return Lower case string.
 */
String string_makelower(const String &str)
{
	String ret(str);
	String::size_type i = 0;
	for (i = 0; i < ret.length(); i++)
		ret[i] = _totlower(ret[i]);
	return ret;
}

/**
 * @brief Replace a string inside a string with another string.
 * This function searches for a string inside another string an if found,
 * replaces it with another string. Function can replace several instances
 * of the string inside one string.
 * @param [in] target A string containing another string to replace.
 * @param [in] find A string to search and replace with another (@p replace).
 * @param [in] replace A string used to replace original (@p find).
 */
void string_replace(String &target, const String &find, const String &replace)
{
	const std::string::size_type replace_len = find.length();
	std::string::size_type pos = 0;
	while ((pos = target.find(find, pos)) != String::npos)
	{
		target.replace(pos, replace_len, replace);
		pos += replace_len;
	}
}

/**
 * @brief Compare two strings ignoring the character casing.
 * @param [in] str1 First string to compare.
 * @param [in] str2 Second string to compare.
 * @return As strcmp(), 0 if strings match.
 */
int string_compare_nocase(const String &str1, const String &str2)
{
	String s1(str1);
	String s2(str2);
	String::size_type i = 0;
	for (i = 0; i < s1.length(); i++)
		s1[i] = _totlower(s1[i]);
	for (i = 0; i < s2.length(); i++)
		s2[i] = _totlower(s2[i]);

	return s1.compare(s2);
}

/**
 * @brief Trims whitespace chars from begin and end of the string.
 * @param [in] str the original string.
 * @return Trimmed string.
 */
String string_trim_ws(const String & str)
{
	if (str.empty())
		return str;

	String result(str);
	String::iterator it = result.begin();
	while (_istspace(*it))
		++it;
	
	if (it != result.begin())
		result.erase(result.begin(), it);

	it = result.end() - 1;
	while (_istspace(*it))
		--it;

	if (it != result.end() - 1)
		result.erase(it + 1, result.end());
	return result;
}

/**
 * @brief Trims whitespace chars from begin of the string.
 * @param [in] str the original string.
 * @return Trimmed string.
 */
String string_trim_ws_begin(const String & str)
{
	if (str.empty())
		return str;

	String result(str);
	String::iterator it = result.begin();
	while (_istspace(*it))
		++it;
	
	if (it != result.begin())
		result.erase(result.begin(), it);
	return result;
}

/**
 * @brief Trims whitespace chars from end of the string.
 * @param [in] str the original string.
 * @return Trimmed string.
 */
String string_trim_ws_end(const String & str)
{
	if (str.empty())
		return str;

	String result(str);
	String::iterator it = result.end() - 1;
	while (_istspace(*it))
		--it;

	if (it != result.end() - 1)
		result.erase(it + 1, result.end());
	return result;
}
