// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file UnicodeString.h
 *
 * @brief Unicode string based on std::wstring.
 *
 */
#pragma once

#include <string>
#include <string_view>
#include <tchar.h>

#ifdef _UNICODE
#define std_tchar(type) std::w##type
#else
#define std_tchar(type) std::type
#endif // _UNICODE

typedef std_tchar(string) String;
typedef std_tchar(string_view) StringView;

namespace strutils
{

String makelower(const String &str);
String makeupper(const String &str);

void replace(String &target, const String &find, const String &replace);

// Comparing
int compare_nocase(const String &str1, const String &str2);

// Trimming
String trim_ws(const String & str);
String trim_ws_begin(const String & str);
String trim_ws_end(const String & str);

// Formatting
String format_arg_list(const TCHAR *fmt, va_list args);
String format_varg(const TCHAR *fmt, ...);
namespace detail
{
	template <typename T> inline T arg(T value) { return value; }
	template <typename T> inline T const * arg(std::basic_string<T> const & value) { return value.c_str(); }
}
template <typename ... Args>
inline String format(TCHAR const * const fmt, Args const & ... args)
{
	return format_varg(fmt, detail::arg(args) ...);
}
template <typename ... Args>
inline String format(String const & fmt, Args const & ... args)
{
	return format_varg(fmt.c_str(), detail::arg(args) ...);
}
String format_strings(const String& fmt, const String *args[], size_t nargs);
String format_string1(const String& fmt, const String& arg1);
String format_string2(const String& fmt, const String& arg1, const String& arg2);

template <class InputIterator>
String join(const InputIterator& begin, const InputIterator& end, const String& delim)
{
	size_t sum = 0, delim_len = delim.length();
	for (InputIterator it = begin; it != end; ++it)
	{
		if (sum != 0) sum += delim_len;
		sum += (*it).length();
	}
	String result;
	result.reserve(sum);
	for (InputIterator it = begin; it != end; ++it)
	{
		if (it != begin) result.append(delim);
		result += *it;
	}
	return result;
}

template <class Formatter, class InputIterator>
String join(const InputIterator& begin, const InputIterator& end, const String& delim, Formatter func)
{
	String result;
	for (InputIterator it = begin; it != end; ++it)
	{
		if (it != begin) result.append(delim);
		result += func(*it);
	}
	return result;
}

std::vector<StringView> split(StringView str, TCHAR delim);

inline String to_str(int val) { return strutils::format(_T("%d"), val); }
inline String to_str(unsigned val) { return strutils::format(_T("%u"), val); }
inline String to_str(long val) { return strutils::format(_T("%ld"), val); }
inline String to_str(unsigned long val) { return strutils::format(_T("%lu"), val); }
inline String to_str(long long val) { return strutils::format(_T("%I64d"), val); }
inline String to_str(unsigned long long val) { return strutils::format(_T("%I64u"), val); }
inline String to_str(float val) { return strutils::format(_T("%f"), val); }
inline String to_str(double val) { return strutils::format(_T("%f"), val); }

}
