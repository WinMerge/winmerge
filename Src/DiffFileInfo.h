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
 * @file  DiffFileInfo.h
 *
 * @brief Declaration file for DiffFileInfo
 */
#pragma once

#include "DirItem.h"
#include "FileTextEncoding.h"
#include "FileTextStats.h"

/**
 * @brief Information for file.
 * This class expands DirItem class with encoding information and
 * text stats information.
 * @sa DirItem.
 */
struct DiffFileInfo : public DirItem
{
// data
	FileTextEncoding encoding; /**< unicode or codepage info */
	FileTextStats m_textStats; /**< EOL, zero-byte etc counts */

	// We could stash a pointer here to the parent DIFFITEM
	// but, I ran into trouble with, I think, the DIFFITEM copy constructor

// methods

	DiffFileInfo() { }
	//void Clear();
	void ClearPartial();
	bool IsEditableEncoding() const;
};

/**
 * @brief Return true if file is in any Unicode encoding
 */
inline bool DiffFileInfo::IsEditableEncoding() const
{
	return !encoding.m_bom;
}
