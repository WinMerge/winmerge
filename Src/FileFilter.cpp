/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify it
//    under the terms of the GNU General Public License as published by the
//    Free Software Foundation; either version 2 of the License, or (at your
//    option) any later version.
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
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
