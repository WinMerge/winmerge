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
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _UNICODE_STRING_
#define _UNICODE_STRING_

#include <string>
#include <cstdarg>

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

String string_makelower(const String &str);
String string_makeupper(const String &str);

void string_replace(String &target, const String &find, const String &replace);

// Comparing
int string_compare_nocase(const String &str1, const String &str2);

// Trimming
String string_trim_ws(const String & str);
String string_trim_ws_begin(const String & str);
String string_trim_ws_end(const String & str);

// Formatting
String string_format_arg_list(const TCHAR *fmt, va_list args);
String string_format(const TCHAR *fmt, ...);
String string_format_strings(const String& fmt, const String *args[], size_t nargs);
String string_format_string1(const String& fmt, const String& arg1);
String string_format_string2(const String& fmt, const String& arg1, const String& arg2);

#endif // _UNICODE_STRING_
