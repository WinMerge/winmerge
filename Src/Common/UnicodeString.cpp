// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  UnicodeString.cpp
 *
 * @brief String utilities.
 */

// String formatting code originally from Paul Senzee:
// http://www.senzee5.com/2006/05/c-formatting-stdstring.html

#include "pch.h"
#include "UnicodeString.h"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <vector>

namespace strutils
{

/**
 * @brief Convert a string to lower case string.
 * @param [in] str String to convert to lower case.
 * @return Lower case string.
 */
String makelower(const String &str)
{
	String ret(str);
	String::size_type i = 0;
	for (i = 0; i < ret.length(); i++)
		ret[i] = tc::totlower(ret[i]);
	return ret;
}

/**
 * @brief Convert a string to upper case string.
 * @param [in] str String to convert to upper case.
 * @return upper case string.
 */
String makeupper(const String &str)
{
	String ret(str);
	String::size_type i = 0;
	for (i = 0; i < ret.length(); i++)
		ret[i] = tc::totupper(ret[i]);
	return ret;
}

String strip_hot_key(const String& str)
{
	String str2 = str;
	auto it = str2.find(_T("(&"));
	if (it != String::npos)
		str2.erase(it, it + 2);
	strutils::replace(str2, _T("&"), _T(""));
	return str2;
}

tchar_t from_charstr(const String& str)
{
	tchar_t ch = 0;
	String str2 = strutils::makelower(str);
	strutils::replace(str2, _T("-"), _T(""));
	if (str2 == _T("\\a") || str2 == _T("bel"))
		ch = '\a';
	else if (str2 == _T("\\b") || str2 == _T("bs"))
		ch = '\b';
	else if (str2 == _T("\\f") || str2 == _T("ff"))
		ch = '\f';
	else if (str2 == _T("\\n") || str2 == _T("lf"))
		ch = '\n';
	else if (str2 == _T("\\r") || str2 == _T("cr"))
		ch = '\r';
	else if (str2 == _T("\\t") || str2 == _T("tab"))
		ch = '\t';
	else if (str2 == _T("\\v") || str2 == _T("vt"))
		ch = '\v';
	else if (str2 == _T("\\'") || str2 == _T("sq") || str2 == _T("singlequote"))
		ch = '\'';
	else if (str2 == _T("\\\"") || str2 == _T("dq") || str2 == _T("doublequote"))
		ch = '"';
	else if (str2.find(_T("\\x"), 0) == 0 || str2.find(_T("0x"), 0) == 0)
	{
		tchar_t *pend = nullptr;
		ch = static_cast<tchar_t>(tc::tcstol(str2.substr(2).c_str(), &pend, 16));
	}
	else
		ch = str.c_str()[0];
	return ch;
}

String to_charstr(tchar_t ch)
{
	if (iscntrl(ch))
		return strutils::format(_T("\\x%02x"), ch);
	return String(1, ch);
}

String to_regex(const String& text)
{
	String ret;
	for (auto ch : text)
	{
		switch (ch)
		{
		case '\\': ret += _T("\\\\"); break;
		case '*':  ret += _T("\\*");  break;
		case '+':  ret += _T("\\+");  break;
		case '?':  ret += _T("\\?");  break;
		case '|':  ret += _T("\\|");  break;
		case '.':  ret += _T("\\.");  break;
		case '^':  ret += _T("\\^");  break;
		case '$':  ret += _T("\\$");  break;
		case '(':  ret += _T("\\(");  break;
		case ')':  ret += _T("\\)");  break;
		case '[':  ret += _T("\\[");  break;
		case ']':  ret += _T("\\]");  break;
		case '\t': ret += _T("\\t");  break;
		case '\n': ret += _T("\\n");  break;
		case '\r': ret += _T("\\r");  break;
		case '\a': ret += _T("\\a");  break;
		default:  ret += ch; break;
		}
	}
	return ret;
}

/**
 * @brief Replace a string inside a string with another string.
 * This function searches for a string inside another string an if found,
 * replaces it with another string. Function can replace several instances
 * of the string inside one string.
 * @param [in,out] target A string containing another string to replace.
 * @param [in] find A string to search and replace with another (@p replace).
 * @param [in] replace A string used to replace original (@p find).
 */
void replace(String &target, const String &find, const String &replace)
{
	const String::size_type find_len = find.length();
	const String::size_type replace_len = replace.length();
	String::size_type pos = 0;
	while ((pos = target.find(find, pos)) != String::npos)
	{
		target.replace(pos, find_len, replace);
		pos += replace_len;
	}
}

/**
 * @brief Replace the characters that matche characters specified in its arguments
 * @param [in,out] str - A string containing another string to replace.
 * @param [in] chars - characters to search for
 * @param [in] rep - String to replace
 */
void replace_chars(String& str, const tchar_t* chars, const tchar_t *rep)
{
	String::size_type pos = 0;
	size_t replen = tc::tcslen(rep);
	while ((pos = str.find_first_of(chars, pos)) != std::string::npos)
	{
		std::string::size_type posend = str.find_first_not_of(chars, pos);
		if (posend != String::npos)
			str.replace(pos, posend - pos, rep);
		else
			str.replace(pos, str.length() - pos, rep);
		pos += replen;
	}
}

/**
 * @brief Compare two strings ignoring the character casing.
 * @param [in] str1 First string to compare.
 * @param [in] str2 Second string to compare.
 * @return As strcmp(), 0 if strings match.
 */
int compare_nocase(const String &str1, const String &str2)
{
	return tc::tcsicoll(str1.c_str(), str2.c_str());
}

/**
 * @brief Compare two strings ignoring the character casing. 
 *        Digits in the strings are considered as numerical content rather than text.
 * @param [in] str1 First string to compare.
 * @param [in] str2 Second string to compare.
 * @return As strcmp(), 0 if strings match.
 */
int compare_logical(const String& str1, const String& str2)
{
	return tc::tcscmplogical(str1.c_str(), str2.c_str());
}

/**
 * @brief Trims whitespace chars from begin and end of the string.
 * @param [in] str the original string.
 * @return Trimmed string.
 */
String trim_ws(const String & str)
{
	if (str.empty())
		return str;

	String result(str);
	String::iterator it = result.begin();
	while (it != result.end() && tc::istspace(*it))
		++it;
	
	if (it != result.begin())
		result.erase(result.begin(), it);

	if (result.empty())
		return result;

	it = result.end() - 1;
	while (it != result.begin() && tc::istspace(*it))
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
String trim_ws_begin(const String & str)
{
	if (str.empty())
		return str;

	String result(str);
	String::iterator it = result.begin();
	while (it != result.end() && tc::istspace(*it))
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
String trim_ws_end(const String & str)
{
	if (str.empty())
		return str;

	String result(str);
	String::reverse_iterator it = result.rbegin();
	while (it != result.rend() && tc::istspace(*it))
		++it;

	if (it != result.rbegin())
		result.erase(it.base(), result.end());
	return result;
}

String format_arg_list(const tchar_t *fmt, va_list args)
{
	if (fmt == nullptr)
		return _T("");
	int result = -1;
	int length = 256;
	std::vector<tchar_t> buffer(length, 0);
	while (result == -1)
	{
		result = tc::vsntprintf_s(&buffer[0], length, _TRUNCATE, fmt, args);
		length *= 2;
		buffer.resize(length, 0);
	}
	String s(&buffer[0]);
	return s;
}

/**
 * @brief printf()-style formatting for STL string.
 * Use this function to format String:s in printf() style.
 */
String format_varg(const tchar_t *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	String s = format_arg_list(fmt, args);
	va_end(args);
	return s;
}

String format_strings(const String& fmt, const String *args[], size_t nargs)
{
	String str;
	str.reserve(fmt.length() * 2);
	String::const_iterator it;
	for (it = fmt.begin(); it != fmt.end(); ++it)
	{
		if (*it == '%')
		{
			++it;
			if (it == fmt.end())
				break;
			int n = *it - '0';
			if (n > 0 && static_cast<unsigned int>(n) <= nargs)
				str += *args[n - 1];
			else
				str += *it;
		}
		else
		{
			str += *it;
		}
	}
	return str;
}

String format_string1(const String& fmt, const String& arg1)
{
	const String* args[] = {&arg1};
	return format_strings(fmt, args, 1);
}

String format_string2(const String& fmt, const String& arg1, const String& arg2)
{
	const String* args[] = {&arg1, &arg2};
	return format_strings(fmt, args, 2);
}

/**
 * @brief Output the converted string according to the printf()-style formatting.
 * @param [in] fmt printf()-style formatting.
 * @param [in] arg1 Value of "%1" of fmt.
 * @param [in] arg2 Value of "%2" of fmt.
 * @param [in] arg3 Value of "%3" of fmt.
 * @return Formatted output string.
 */
String format_string3(const String& fmt, const String& arg1, const String& arg2, const String& arg3)
{
	const String* args[] = { &arg1, &arg2, &arg3 };
	return format_strings(fmt, args, 3);
}

}
