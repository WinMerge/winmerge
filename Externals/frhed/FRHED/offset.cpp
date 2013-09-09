/*
Frhed - Free hex editor
Copyright (C) 2000 Raihan Kibria

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

Last change: 2013-02-24 by Jochen Neubeck
*/
/** 
 * @file  Offset.cpp
 *
 * @brief Implementation of the offset helper functions.
 *
 */
#include "precomp.h"
#include "offset.h"

/**
 * @brief Parse offset number from string.
 * @param [in] str String to parse.
 * @param [out] offset Number to return (parsed offset)
 * @return true if parsing succeeded, false if failed.
 * @todo Find a faster way to parse all possibilities.
 */
bool offset_parse(LPCTSTR str, int &offset)
{
	bool negate = false;
	if (*str == '-')
	{
		++str;
		negate = true;
	}
	if ((_stscanf(str, _T("x%x"), &offset) <= 0) &&
		(_stscanf(str, _T("X%x"), &offset) <= 0) &&
		(_stscanf(str, _T("0x%x"), &offset) <= 0) &&
		(_stscanf(str, _T("0X%x"), &offset) <= 0) &&
		(_stscanf(str, _T("%d"), &offset) <= 0))
	{
		offset = 0;
		return false;
	}
	if (negate)
		offset = -offset;
	return true;
}

/**
 * @brief Parse 64-bit offset number from string.
 * @param [in] str String to parse.
 * @param [out] offset Number to return (parsed offset)
 * @return true if parsing succeeded, false if failed.
 * @todo Find a faster way to parse all possibilities.
 */
bool offset_parse64(LPCTSTR str, INT64 &offset)
{
	bool negate = false;
	if (*str == '-')
	{
		++str;
		negate = true;
	}
	if ((_stscanf(str, _T("x%llx"), &offset) <= 0) &&
		(_stscanf(str, _T("X%llx"), &offset) <= 0) &&
		(_stscanf(str, _T("0x%llx"), &offset) <= 0) &&
		(_stscanf(str, _T("0X%llx"), &offset) <= 0) &&
		(_stscanf(str, _T("%lld"), &offset) <= 0))
	{
		offset = 0;
		return false;
	}
	if (negate)
		offset = -offset;
	return true;
}
