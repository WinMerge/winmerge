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
 * @file  DirItem.cpp
 *
 * @brief Implementation for DirItem routines
 */

#include "pch.h"
#include "DirItem.h"
#include <windows.h>
#include "UnicodeString.h"
#include "paths.h"
#include "TFile.h"
#include "DebugNew.h"

/**
	* @brief Convert file flags to string presentation.
	* This function converts file flags to a string presentation that can be
	* shown in the GUI.
	* @return File flags as a string.
	*/
String FileFlags::ToString() const
{
	String sflags;
	if (attributes & FILE_ATTRIBUTE_READONLY)
		sflags += _T("R");
	if (attributes & FILE_ATTRIBUTE_HIDDEN)
		sflags += _T("H");
	if (attributes & FILE_ATTRIBUTE_SYSTEM)
		sflags += _T("S");
	if (attributes & FILE_ATTRIBUTE_ARCHIVE)
		sflags += _T("A");
	return sflags;
}

/**
 * @brief Set filename and path for the item.
 * @param [in] fullpath Full path to file to set to item.
 */
void DirItem::SetFile(const String &fullPath)
{
	String ext, filename2, path2;
	paths::SplitFilename(fullPath, &path2, &filename2, &ext);
	filename2 += _T(".");
	filename2 += ext;
	filename = filename2;
	path = path2;
}

/**
 * @brief Get the full path of the item.
 * @return fullpath
 */
String DirItem::GetFile() const
{
	return paths::ConcatPath(path.get(), filename.get());
}

/**
 * @brief Update fileinfo from given file.
 * This function updates file's information from given item. Function
 * does not set filename and path.
 * @param [in] sFilePath Full path to file/directory to update
 * @return true if information was updated (item was found).
 */
bool DirItem::Update(const String &sFilePath)
{
	bool retVal = false;

	size = DirItem::FILE_SIZE_NONE;
	flags.reset();
	mtime = 0;

	if (!sFilePath.empty())
	{
		try
		{
			TFile file(sFilePath);

			mtime = file.getLastModified();
			// There can be files without modification date.
			// Then we must use creation date. Of course we assume
			// creation date then exists...
			if (mtime == 0)
				mtime = file.created();

			// No size for directory ( size remains as -1)
			if (!file.isDirectory())
				size = file.getSize();

			flags.attributes = GetFileAttributes(file.wpath().c_str());

			retVal = true;
		}
		catch (...)
		{
		}
	}
	return retVal;
}

/**
 * @brief Clears FileInfo data.
 */
/*void DirItem::Clear()
{
	ClearPartial();
	filename.erase();
	path.erase();
}*/

/**
 * @brief Clears FileInfo data except path/filename.
 */
void DirItem::ClearPartial()
{
	ctime = 0;
	mtime = 0;
	size = DirItem::FILE_SIZE_NONE;
	version.Clear();
	flags.reset();
}
