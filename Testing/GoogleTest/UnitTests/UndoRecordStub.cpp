/**
 * @file  UndoRecordStub.cpp
 *
 * @brief Stub implementation of UndoRecord methods for test projects.
 *
 * This file provides minimal implementations of UndoRecord methods
 * to satisfy linker requirements when TreeSitterParser.cpp is compiled
 * without the full editlib library.
 */

#include "../../../Externals/crystaledit/editlib/UndoRecord.h"

/**
 * @brief Minimal stub implementation of Clone.
 *
 * Test projects don't need actual undo functionality.
 * This stub allows TreeSitterParser to link without errors.
 */
void UndoRecord::Clone(const UndoRecord &src)
{
	m_dwFlags = src.m_dwFlags;
	m_ptStartPos = src.m_ptStartPos;
	m_ptEndPos = src.m_ptEndPos;
	m_nAction = src.m_nAction;
	m_paSavedRevisionNumbers = nullptr;
	// Don't copy text - test projects don't need it
	m_pszText = nullptr;
}

/**
 * @brief Minimal stub implementation of FreeText.
 *
 * Test projects don't allocate text, so nothing to free.
 */
void UndoRecord::FreeText()
{
	// No-op for stub implementation
	m_pszText = nullptr;
}
