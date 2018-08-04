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
#pragma once

#define POCO_NO_UNWINDOWS 1
#include <Poco/File.h>
#include <Poco/Timestamp.h>
#include <boost/flyweight.hpp>
#include "UnicodeString.h"
#include "FileVersion.h"

/**
 * @brief Class for fileflags.
 */
struct FileFlags
{
	unsigned attributes; /**< Fileattributes for item */
	FileFlags() : attributes(0) { }
	void reset() { attributes = 0; } /// Reset fileattributes
	String ToString() const;
};

/**
 * @brief Information for file.
 * This class stores basic information from a file or folder.
 * Information consists of item name, times, size and attributes.
 * Also version info can be get for files supporting it.
 *
 * @note times in are seconds since January 1, 1970.
 * See Dirscan.cpp/fentry and Dirscan.cpp/LoadFiles()
 */
struct DirItem
{
	Poco::Timestamp ctime; /**< time of creation */
	Poco::Timestamp mtime; /**< time of last modify */
	Poco::File::FileSize size; /**< file size in bytes, FILE_SIZE_NONE (== -1) means file does not exist*/
	boost::flyweight<String> filename; /**< filename for this item */
	boost::flyweight<String> path; /**< full path (excluding filename) for the item */
	FileVersion version; /**< string of fixed file version, eg, 1.2.3.4 */
	FileFlags flags; /**< file attributes */
	
	enum : uint64_t { FILE_SIZE_NONE = UINT64_MAX };
	DirItem() : ctime(0), mtime(0), size(DirItem::FILE_SIZE_NONE) { }
	void SetFile(const String &fullPath);
	String GetFile() const;
	bool Update(const String &sFilePath);
	void ClearPartial();
};
