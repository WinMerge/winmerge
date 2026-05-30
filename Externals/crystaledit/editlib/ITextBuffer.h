////////////////////////////////////////////////////////////////////////////
//  File:       ITextBuffer.h
//  Version:    1.0.0.0
//  Created:    2026-01-XX
//
//  Interface for text buffer access - MFC-independent abstraction
//  for TreeSitterParser and other components that need basic text
//  buffer operations without MFC dependencies.
//
//  SPDX-License-Identifier: GPL-2.0-or-later
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "utils/ctchar.h"
#include "UndoRecord.h"

/**
 * @brief Abstract interface for text buffer access.
 *
 * This interface provides a minimal set of methods needed by TreeSitterParser
 * and other components to access text buffer content and undo information
 * without depending on MFC or CCrystalTextBuffer directly.
 */
class ITextBuffer
{
public:
	virtual ~ITextBuffer() = default;

	// Line access methods
	/**
	 * @brief Get the number of lines in the buffer.
	 * @return Line count.
	 */
	virtual int GetLineCount() const = 0;

	/**
	 * @brief Get a pointer to the characters of a specific line.
	 * @param nLineIndex Zero-based line index.
	 * @return Pointer to the line's character data (may not be null-terminated).
	 */
	virtual const tchar_t* GetLineChars(int nLineIndex) const = 0;

	/**
	 * @brief Get the length of a specific line.
	 * @param nLineIndex Zero-based line index.
	 * @return Number of characters in the line (excluding line ending).
	 */
	virtual int GetLineLength(int nLineIndex) const = 0;

	// Undo-related methods
	/**
	 * @brief Check if undo is available.
	 * @return true if undo operations are available.
	 */
	virtual bool CanUndo() const = 0;

	/**
	 * @brief Get the current undo position.
	 * @return Current position in the undo stack.
	 */
	virtual int GetUndoPosition() const = 0;

	/**
	 * @brief Get an undo record at a specific position.
	 * @param nUndoPos Position in the undo stack.
	 * @return The undo record at that position.
	 */
	virtual UndoRecord GetUndoRecord(int nUndoPos) const = 0;
};
