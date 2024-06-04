// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  FileFlags.cpp
 *
 * @brief Implementation for FileFlags routines
 */

#include "pch.h"
#include <windows.h>
#include "FileFlags.h"
#include "UnicodeString.h"
#include "DebugNew.h"

/**
	* @brief Convert file flags to string presentation.
	* This function converts file flags to a string presentation that can be
	* shown in the GUI.
	* @return File flags as a string.
	*/
String FileFlags::ToString() const
{
	tchar_t sflags[5], *p = sflags;
	if (attributes & FILE_ATTRIBUTE_READONLY)
		*p++ = 'R';
	if (attributes & FILE_ATTRIBUTE_HIDDEN)
		*p++ = 'H';
	if (attributes & FILE_ATTRIBUTE_SYSTEM)
		*p++ = 'S';
	if (attributes & FILE_ATTRIBUTE_ARCHIVE)
		*p++ = 'A';
	return String{ sflags, p };
}

