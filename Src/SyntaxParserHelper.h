#pragma once

#include "ISyntaxParser.h"
#include <string>

/**
 * @brief Helper utilities for syntax parser operations.
 *
 * This class provides comment filtering and position checking functionality
 * that operates on top of ISyntaxParser implementations. These operations
 * are kept separate from the core parser interface to maintain minimal
 * parser API surface.
 */
class SyntaxParserHelper
{
public:
	/**
	 * @brief Get text with comments filtered out.
	 * @param pParser Pointer to the syntax parser.
	 * @param pTextBuffer Pointer to the text buffer.
	 * @param nStartLine Starting line index (zero-based).
	 * @param nEndLine Ending line index (zero-based, inclusive).
	 * @return The filtered text with comments removed.
	 *
	 * This method uses the parser to identify comment regions and removes them
	 * from the text. Useful for diff post-filtering to ignore comment changes.
	 */
	static std::string GetCommentsFilteredText(
		ISyntaxParser* pParser,
		ITextBuffer* pTextBuffer,
		int nStartLine,
		int nEndLine);

	/**
	 * @brief Get text with comments filtered out and per-line comment status.
	 * @param pParser Pointer to the syntax parser.
	 * @param pTextBuffer Pointer to the text buffer.
	 * @param nStartLine Starting line index (zero-based).
	 * @param nEndLine Ending line index (zero-based, inclusive).
	 * @param allTextIsComment [out] Vector indicating if each line is entirely a comment.
	 * @return The filtered text with comments removed.
	 *
	 * This overload additionally populates a vector indicating whether each line
	 * consists entirely of comments (useful for diff filtering).
	 */
	static std::string GetCommentsFilteredText(
		ISyntaxParser* pParser,
		ITextBuffer* pTextBuffer,
		int nStartLine,
		int nEndLine,
		std::vector<bool>& allTextIsComment);

	/**
	 * @brief Check if a specific position is within a comment.
	 * @param pParser Pointer to the syntax parser.
	 * @param pTextBuffer Pointer to the text buffer.
	 * @param nLineIndex Zero-based line index.
	 * @param nCharPos Zero-based character position in the line.
	 * @return true if the position is within a comment, false otherwise.
	 *
	 * This method parses the line and checks the color index at the specified
	 * position to determine if it's a comment region.
	 */
	static bool IsCommentPosition(
		ISyntaxParser* pParser,
		ITextBuffer* pTextBuffer,
		int nLineIndex,
		int nCharPos);

private:
	SyntaxParserHelper() = delete; // Static utility class
};
