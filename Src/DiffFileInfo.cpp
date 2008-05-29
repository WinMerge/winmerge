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
 * @file  DiffFileInfo.cpp
 *
 * @brief Implementation for DiffFileInfo routines
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "DirItem.h"
#include "DiffFileInfo.h"
#include "unicoder.h"
#include "UnicodeString.h"

/**
 * @brief Convert file flags to string presentation.
 * This function converts file flags to a string presentation that can be
 * shown in the GUI.
 * @return File flags as a string.
 */
String DiffFileFlags::ToString() const
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

	if ((coding & coding_mask) == UTF_8)
		sflags += _T("8");
	if ((coding & coding_mask) == UCS_2BE)
		sflags += _T("B");
	if ((coding & coding_mask) == UCS_2LE)
		sflags += _T("L");
	if ((coding & coding_mask) == UCS_4)
		sflags += _T("4");
	return sflags;
}

/**
 * @brief Clears FileInfo data.
 */
void DiffFileInfo::ClearPartial()
{
	DirItem::ClearPartial();
	bVersionChecked = false;
	encoding.Clear();
	m_textStats.clear();
}

/**
 * @brief Return true if file is in any Unicode encoding
 */
bool DiffFileInfo::IsEditableEncoding() const
{
	return encoding.m_bom == false;
}
