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

#ifdef _UNICODE

typedef std::basic_string<wchar_t, std::char_traits<wchar_t>,
	std::allocator<wchar_t> > String;

#else

typedef std::basic_string<char, std::char_traits<char>, std::allocator<char> >
	String;

#endif // _UNICODE

#endif // _UNICODE_STRING_
