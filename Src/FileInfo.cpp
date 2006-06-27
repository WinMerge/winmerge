/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//    
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  FileInfo.cpp
 *
 * @brief Implementation for FileInfo routines
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "FileInfo.h"

/**
 * @brief Convert a FILETIME to a long (standard time)
 */
static __int64 FileTimeToInt64(FILETIME & ft)
{
	return CTime(ft).GetTime();
}

/**
 * @brief Update fileinfo from given file
 * @param [in] sFilePath Full path to file/directory to update
 */
void FileInfo::Update(CString sFilePath)
{
	// CFileFind doesn't expose the attributes
	// CFileStatus doesn't expose 64 bit size

	WIN32_FIND_DATA wfd;
	HANDLE h = FindFirstFile(sFilePath, &wfd);
	__int64 mtime64 = 0;
	size = -1;
	flags.reset();
	mtime = 0;
	if (h != INVALID_HANDLE_VALUE)
	{
		mtime64 = FileTimeToInt64(wfd.ftLastWriteTime);
		flags.attributes = wfd.dwFileAttributes;

		// No size for directory ( size remains as -1)$
		if ((flags.attributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			size = FileInfo::GetSizeFromFindData(wfd);
		FindClose(h);
	}
	mtime = mtime64;
}

/**
 * @brief Clears FileInfo data.
 */
void FileInfo::Clear()
{
	ctime = 0;
	mtime = 0;
	size = -1;
	version.Empty();
	flags.reset();
}

/**
 * @brief Returns 64-bit filesize from Windows Find Data.
 * This function calculates 64-bit filesize from given find-data where
 * filesize is in two variables.
 * @param [in] findData Find-data to get filesize.
 * @return 64-bit filesize.
 */
__int64 FileInfo::GetSizeFromFindData(const WIN32_FIND_DATA & findData)
{
	__int64 tmpSize = 0;
	// Get file's size. If more than 31 bits is needed then we need to
	// calculate full 64 bits. Since size is signed variable.
	if (findData.nFileSizeHigh > 0 || findData.nFileSizeLow > INT_MAX)
	{
		tmpSize = findData.nFileSizeHigh;
		tmpSize = tmpSize << 32;
		tmpSize += findData.nFileSizeLow;
	}
	else
	{
		tmpSize = findData.nFileSizeLow;
	}
	return tmpSize;
}
