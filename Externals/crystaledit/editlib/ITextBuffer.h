#pragma once

#include "utils/ctchar.h"

namespace LangServices
{

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
};

}
