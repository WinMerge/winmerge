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
 * @file  StringTable.h
 *
 * @brief String table for translated texts.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: StringTable.h 173 2008-12-03 17:29:30Z kimmov $

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
	T DiffListItemFormat;
	T AboutFrhed;
};

extern StringTable<LPTSTR> S;
extern StringTable<WORD> IDS;

#endif // _STRING_TABLE_H_
