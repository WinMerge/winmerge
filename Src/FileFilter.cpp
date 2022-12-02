// SPDX-License-Identifier: GPL-2.0-or-later
/**
 *  @file FileFilter.cpp
 *
 *  @brief Implementation of FileFilter.
 */ 

#include "pch.h"
#include "FileFilter.h"
#include <vector>

using std::vector;

/**
 * @brief Destructor, frees created filter lists.
 */
FileFilter::~FileFilter()
{
	EmptyFilterList(&filefilters);
	EmptyFilterList(&filefiltersExclude);
	EmptyFilterList(&dirfilters);
	EmptyFilterList(&dirfiltersExclude);
}

/**
 * @brief Deletes items from filter list.
 *
 * @param [in] filterList List to empty.
 */
void FileFilter::EmptyFilterList(vector<FileFilterElementPtr> *filterList)
{
	filterList->clear();
}

/**
 * @brief Clone file filter from another filter.
 * This function clones file filter from another filter.
 * Current contents in the filter are removed and new contents added from the given filter.
 * @param [in] filter File filter to clone.
 */
void FileFilter::CloneFrom(const FileFilter* filter)
{
	if (!filter)
		return;

	default_include = filter->default_include;
	name = filter->name;
	description = filter->description;
	fullpath = filter->fullpath;

	filefilters.clear();
	size_t count = filter->filefilters.size();
	for (size_t i = 0; i < count; i++)
	{
		filefilters.emplace_back(std::make_shared<FileFilterElement>(filter->filefilters[i].get()));
	}

	dirfilters.clear();
	count = filter->dirfilters.size();
	for (size_t i = 0; i < count; i++)
	{
		dirfilters.emplace_back(std::make_shared<FileFilterElement>(filter->dirfilters[i].get()));
	}
	filefiltersExclude.clear();
	count = filter->filefiltersExclude.size();
	for (size_t i = 0; i < count; i++)
	{
		filefiltersExclude.emplace_back(std::make_shared<FileFilterElement>(filter->filefiltersExclude[i].get()));
	}

	dirfiltersExclude.clear();
	count = filter->dirfiltersExclude.size();
	for (size_t i = 0; i < count; i++)
	{
		dirfiltersExclude.emplace_back(std::make_shared<FileFilterElement>(filter->dirfiltersExclude[i].get()));
	}
}
