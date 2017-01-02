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
 * @file UnicodeString.h
 *
 * @brief Unicode string based on std::wstring.
 *
 */
#pragma once

#include <string>
#include <cstdarg>
#include <cstdint>

#ifdef _WIN32
#  include <tchar.h>
#else
#  ifndef _T
#    ifdef _UNICODE
#      define _T(x) L ## x
#    else
#      define _T(x) x
#    endif
#  endif
#  ifndef _TCHAR_DEFINED
#    ifdef _UNICODE
typedef wchar_t TCHAR;
#    else
typedef char    TCHAR;
#    endif
#  endif
#  define _TCHAR_DEFINED
#endif

#ifdef _UNICODE
#define std_tchar(type) std::w##type
#else
#define std_tchar(type) std::type
#endif // _UNICODE

typedef std_tchar(string) String;

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
String format(const TCHAR *fmt, ...);
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
		if (!result.empty()) result.append(delim);
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
		if (!result.empty()) result.append(delim);
		result += func(*it);
	}
	return result;
}

inline String to_str(int val) { return strutils::format(_T("%d"), val); }
inline String to_str(unsigned val) { return strutils::format(_T("%u"), val); }
inline String to_str(long val) { return strutils::format(_T("%ld"), val); }
inline String to_str(unsigned long val) { return strutils::format(_T("%lu"), val); }
inline String to_str(long long val) { return strutils::format(_T("%I64d"), val); }
inline String to_str(unsigned long long val) { return strutils::format(_T("%I64u"), val); }
inline String to_str(float val) { return strutils::format(_T("%f"), val); }
inline String to_str(double val) { return strutils::format(_T("%f"), val); }

}
