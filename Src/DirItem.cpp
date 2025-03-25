// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  DirItem.cpp
 *
 * @brief Implementation for DirItem routines
 */

#include "pch.h"
#include "DirItem.h"
#include "UnicodeString.h"
#include "paths.h"
#include "TFile.h"
#include "Logger.h"
#include "DebugNew.h"
#include <filesystem>

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

	if (!sFilePath.empty() && !paths::IsURL(sFilePath))
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
		catch (Poco::Exception& e)
		{
			RootLogger::Warn(e.displayText());
		}
	}
	return retVal;
}

/**
 * @brief Update filename from given file.
 * @param [in] sFilePath Full path to file/directory to update
 * @return true if information was updated (item was found).
 */
bool DirItem::UpdateFileName(const String& sFilePath)
{
	bool retVal = false;
	if (!sFilePath.empty())
	{
		try
		{
			std::filesystem::path canonicalPath = std::filesystem::canonical(sFilePath);
			filename = canonicalPath.filename();
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
	flags.reset();
}

/**
 * @brief Return whether the item is a directory.
 * @return true if the item is a directory.
 */
bool DirItem::IsDirectory() const
{
	return ((flags.attributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
}
