// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  DiffFileInfo.h
 *
 * @brief Declaration file for DiffFileInfo
 */
#pragma once

#include "DirItem.h"
#include "FileVersion.h"
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
	FileVersion version; /**< string of fixed file version, eg, 1.2.3.4 */
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
