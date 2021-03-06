// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  FileFlags.h
 *
 * @brief Declaration file for FileFlags
 */
#pragma once

#include "UnicodeString.h"

/**
 * @brief Class for fileflags.
 */
struct FileFlags
{
	unsigned attributes; /**< Fileattributes for item */
	FileFlags() : attributes(0) { }
	void reset() { attributes = 0; } /// Reset fileattributes
	String ToString() const;
};

