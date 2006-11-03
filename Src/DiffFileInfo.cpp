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
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "FileInfo.h"
#include "DiffFileInfo.h"
#include "unicoder.h"

/**
 * @brief Clears FileInfo data.
 */
void DiffFileInfo::Clear()
{
	FileInfo::Clear();
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
