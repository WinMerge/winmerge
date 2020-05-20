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
	EmptyFilterList(&dirfilters);
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
