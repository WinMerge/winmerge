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

#include "UnicodeString.h"

void string_replace(String &target, const String &find, const String &replace)
{
	const size_t replace_len = find.length();
	size_t prevPos = 0;
	size_t pos = target.find(find, prevPos);
	while (pos >= 0)
	{
		target.replace(pos, replace_len, replace);
		prevPos = pos;
		pos = target.find(find, prevPos);
	}
}
