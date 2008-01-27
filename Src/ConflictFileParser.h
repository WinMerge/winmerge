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
 * @file  ConflictFileParser.h
 *
 * @brief Declaration ConflictFileParser.
 */
// ID line follows -- this is updated by SVN
// $Id$

// Conflict file parsing method modified from original code got from:
// TortoiseCVS - a Windows shell extension for easy version control
// Copyright (C) 2000 - Francis Irving
// <francis@flourish.org> - January 2001

#ifndef _CONFLICT_FILE_PARSER_H_
#define _CONFLICT_FILE_PARSER_H_

#include "UnicodeString.h"

bool IsConflictFile(const String &conflictFileName);

bool ParseConflictFile(const String &conflictFileName,
		const String &workingCopyFileName, const String &newRevisionFileName,
		bool &nestedConflicts);

#endif // _CONFLICT_FILE_PARSER_H_
