// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  ConflictFileParser.h
 *
 * @brief Declaration ConflictFileParser.
 */
// Conflict file parsing method modified from original code got from:
// TortoiseCVS - a Windows shell extension for easy version control
// Copyright (C) 2000 - Francis Irving
// <francis@flourish.org> - January 2001

#pragma once

#include "UnicodeString.h"

bool IsConflictFile(const String& conflictFileName);

bool ParseConflictFile(const String& conflictFileName,
		const String& workingCopyFileName, const String& newRevisionFileName, const String& baseRevisionFileName,
		int iGuessEncodingType, bool &nestedConflicts, bool &b3way);
