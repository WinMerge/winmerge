// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  DiffFileInfo.cpp
 *
 * @brief Implementation for DiffFileInfo routines
 */

#include "pch.h"
#include "DiffFileInfo.h"
#include "DebugNew.h"

/**
 * @brief Clears FileInfo data.
 */
void DiffFileInfo::ClearPartial()
{
	DirItem::ClearPartial();
	version.Clear();
	encoding.Clear();
	m_textStats.clear();
}
