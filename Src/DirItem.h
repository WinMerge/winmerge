// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  DirItem.h
 *
 * @brief Declaration file for DirItem
 */
#pragma once

#define POCO_NO_UNWINDOWS 1
#include <Poco/File.h>
#include <Poco/Timestamp.h>
#if !defined(__cppcheck__)
#include <boost/flyweight.hpp>
#endif
#include "FileFlags.h"
#include "UnicodeString.h"

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
	FileFlags flags; /**< file attributes */
	
	enum : uint64_t { FILE_SIZE_NONE = UINT64_MAX };
	DirItem() : ctime(Poco::Timestamp::TIMEVAL_MIN), mtime(Poco::Timestamp::TIMEVAL_MIN), size(DirItem::FILE_SIZE_NONE) { }
	void SetFile(const String &fullPath);
	String GetFile() const;
	bool Update(const String &sFilePath);
	bool UpdateFileName(const String& sFilePath);
	void ClearPartial();
	bool IsDirectory() const;
};
