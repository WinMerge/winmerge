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
#include <sys/types.h>
#include <sys/stat.h>
#include "FileInfo.h"

/**
 * @brief Update fileinfo from given file
 * @param [in] sFilePath Full path to file/directory to update
 * @return TRUE if information was updated (item was found).
 */
BOOL FileInfo::Update(CString sFilePath)
{
	struct _stati64 fstats;
	__int64 mtime64 = 0;
	BOOL retVal = FALSE;

	size = -1;
	flags.reset();
	mtime = 0;

	if (_tstati64(sFilePath, &fstats) == 0)
	{
		// There can be files without modification date.
		// Then we must use creation date. Of course we assume
		// creation date then exists...
		if (fstats.st_mtime == 0)
			mtime64 = fstats.st_ctime;
		else
			mtime64 = fstats.st_mtime;

		flags.attributes = GetFileAttributes(sFilePath);

		// No size for directory ( size remains as -1)
		if ((flags.attributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			size = fstats.st_size;

		retVal = TRUE;
	}
	mtime = mtime64;
	return retVal;
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
