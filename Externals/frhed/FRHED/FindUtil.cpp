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
 * @file FindUtil.cpp
 *
 * @brief Find functions.
 *
 */
#include "precomp.h"
#include "FindUtil.h"

static BYTE equal(BYTE c);
static BYTE lower_case(BYTE c);
static int find_bytes(BYTE* ps, int ls, BYTE* pb, int lb, int mode, BYTE (*cmp)(BYTE));

/**
 * @brief Find bytes in buffer.
 * @param [in] ps Start position.
 * @param [in] ls Length of src array.
 * @param [in] pb Start of searchstring.
 * @param [in] lb Length searchstring.
 * @param [in] mode Search mode:
 *   -1 : backwards search
 *    1 : forward search
 * @param [in] case_sensitive If true, performs case-sensitive search.
 * @return Position of found string or -1 if not there.
*/
int findutils_FindBytes(BYTE* ps, int ls, BYTE* pb, int lb, int mode, bool case_sensitive)
{
	BYTE (*cmp)(BYTE) = case_sensitive ? equal : lower_case;
	return find_bytes(ps, ls, pb, lb, mode, cmp);
}

//--------------------------------------------------------------------------------------------
// Required for the find function.
BYTE equal(BYTE c)
{
	return c;
}

BYTE lower_case(BYTE c)
{
	if (c >= 'A' && c <= 'Z')
		c = (BYTE)('a' + c - 'A');
	return c;
}

/**
 * @brief Find bytes in buffer.
 * @param [in] ps Start position.
 * @param [in] ls Length of src array.
 * @param [in] pb Start of searchstring.
 * @param [in] lb Length searchstring.
 * @param [in] mode Search mode:
 *   -1 : backwards search
 *    1 : forward search
 * @param [in] cmp pointer to function that is applied to data before comparing.
 * @return Position of found string or -1 if not there.
*/
int find_bytes(BYTE* ps, int ls, BYTE* pb, int lb, int mode, BYTE (*cmp)(BYTE))
{
	int start, inc, end, i;
	if (mode == 1)
	{
		start = 0;
		inc = 1;
		end = ls - lb + 1;
	}
	else
	{
		start = ls - lb;
		inc = -1;
		end = 1;
	}

	for (; mode * start < end; start += inc)
	{
		for (i = start; i < start + lb; i++)
		{
			if (cmp (ps[i]) != cmp (pb[i - start]))
				break;
		}
		if (i == start + lb)
			return start;
	}

	return -1;
}
