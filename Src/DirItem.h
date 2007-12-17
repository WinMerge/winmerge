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
 * @file  DirItem.h
 *
 * @brief Declaration file for DirItem
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _DIR_ITEM_H_INCLUDED
#define _DIR_ITEM_H_INCLUDED

#include "UnicodeString.h"
#include "FileVersion.h"

/**
 * @brief Class for fileflags.
 */
struct FileFlags
{
	DWORD attributes; /**< Fileattributes for item */
	FileFlags() : attributes(0) { }
	void reset() { attributes = 0; } /// Reset fileattributes
};

/**
 * @brief Information for file.
 * This class stores basic information from a file or folder.
 * Information consists of item name, times, size and attributes.
 * Also version info can be get for files supporting it.
 */
struct DirItem
{
	__int64 ctime; /**< time of creation */
	__int64 mtime; /**< time of last modify */
	__int64 size; /**< file size in bytes, -1 means file does not exist*/
	String filename; /**< filename for this item */
	String path; /**< full path (excluding filename) for the item */
	bool bIsDir; /**< is this a directory item or file item? */
	FileVersion version; /**< string of fixed file version, eg, 1.2.3.4 */
	FileFlags flags; /**< file attributes */
	DirItem() { Clear(); }

	void SetFile(String fullPath);
	BOOL Update(const String &sFilePath);
	void Clear();
};

#endif // _DIR_ITEM_H_INCLUDED
