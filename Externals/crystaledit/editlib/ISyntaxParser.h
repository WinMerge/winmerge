#pragma once

#include "ITextBuffer.h"
#include "parsers/crystallineparser.h"

/**
 * @brief Abstract interface for syntax parsers.
 *
 * This interface provides a minimal contract for syntax highlighting parsers,
 * allowing both line-based parsers (CrystalEdit) and whole-document parsers (Tree-sitter)
 * to be used interchangeably. The parser accesses text data through ITextBuffer
 * to avoid direct MFC dependencies.
 */
class ISyntaxParser
{
public:
	virtual ~ISyntaxParser() = default;

	/**
	 * @brief Set the text buffer that this parser will operate on.
	 * @param pTextBuffer Pointer to the text buffer interface.
	 */
	virtual void SetTextBuffer(ITextBuffer* pTextBuffer) = 0;

	/**
	 * @brief Parse a single line and return syntax highlighting information.
	 * @param nLineIndex Zero-based line index to parse.
	 * @param pBuf Output buffer to receive TEXTBLOCK array (can be nullptr for size query).
	 * @param nActualItems [in/out] Maximum items on input, actual items written on output.
	 * @return Parser cookie/state for this line (used for multi-line state tracking).
	 */
	virtual unsigned ParseLine(int nLineIndex, CrystalLineParser::TEXTBLOCK* pBuf, int& nActualItems) = 0;

	/**
	 * @brief Notify the parser that the text buffer has been modified.
	 * @param nStartLine Zero-based index of the first modified line.
	 * @param nEndLine Zero-based index of the last modified line (inclusive).
	 * 
	 * This allows whole-document parsers like Tree-sitter to perform incremental updates.
	 * Line-based parsers may ignore this notification.
	 */
	virtual void OnTextChanged(int nStartLine, int nEndLine) = 0;

	/**
	 * @brief Get the parser type for this syntax parser.
	 * @return The TextType enum value representing the language/format.
	 */
	virtual CrystalLineParser::TextType GetParserType() const = 0;

	/**
	 * @brief Check if a specific position is inside a comment.
	 * @param nLineIndex Zero-based line index.
	 * @param nCharPos Zero-based character position in the line.
	 * @return true if the position is inside a comment (line or block), false otherwise.
	 * 
	 * Used by bracket matching and other features that need to skip comments.
	 * The default implementation returns false (no comment detection).
	 */
	virtual bool IsCommentPosition(int nLineIndex, int nCharPos) const { return false; }

	/**
	 * @brief Find the matching brace/bracket/parenthesis for the given position.
	 * @param nLineIndex Zero-based line index of the starting position.
	 * @param nCharPos Zero-based character position of the starting position.
	 * @param outLineIndex [out] Line index of the matching brace (if found).
	 * @param outCharPos [out] Character position of the matching brace (if found).
	 * @return true if a matching brace was found, false otherwise.
	 * 
	 * The default implementation returns false (no brace matching support).
	 * Implementations should handle (), [], {}, <> pairs and skip comments.
	 */
	virtual bool FindMatchingBrace(int nLineIndex, int nCharPos, int& outLineIndex, int& outCharPos) const { return false; }
};
