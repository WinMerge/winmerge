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
 * @file  StringTable.h
 *
 * @brief String table for translated texts.
 *
 */
#ifndef _STRING_TABLE_H_
#define _STRING_TABLE_H_

/**
 * @brief A struct holding resource strings (translated).
 * This struct contains strings loaded from the resource file (English) or
 * from selected translation's PO file.
 */
template <class T>
struct StringTable
{
	operator T *() { return reinterpret_cast<T *>(this); }
#	define DECLARE(X) T m_##X;
#	include "StringTable.inl"
#	undef DECLARE
};

extern StringTable<LPTSTR> S;
extern StringTable<WORD> IDS;

#define GetLangString(id) ::S.m_##id

#endif // _STRING_TABLE_H_
