#pragma once

#include "ITextBuffer.h"
#include "TextDefinition.h"
#include "cepoint.h"
#include <memory>
#include <vector>

namespace LangServices
{

//  Syntax coloring block — position and color index for one token run.
struct TEXTBLOCK
{
	int m_nCharPos;
	int m_nColorIndex;
	int m_nBgColorIndex;
};

/**
 * @brief Abstract interface for syntax parsers.
 *
 * This interface provides a minimal contract for syntax highlighting parsers,
 * allowing both line-based parsers (CrystalEdit) and whole-document parsers (Tree-sitter)
 * to be used interchangeably. The parser accesses text data through LangServices::ITextBuffer
 * to avoid direct MFC dependencies.
 */
struct ISyntaxParser
{
	virtual ~ISyntaxParser() = default;

	virtual void Invalidate() = 0;

	/**
	 * @brief Set the text buffer that this parser will operate on.
	 * @param pTextBuffer Pointer to the text buffer interface.
	 */
	virtual void SetTextBuffer(LangServices::ITextBuffer* pTextBuffer) = 0;

	/**
	 * @brief Get the currently set text buffer.
	 * @return Pointer to the text buffer interface, or nullptr if not set.
	 */
	virtual LangServices::ITextBuffer* GetTextBuffer() const = 0;

	/**
	 * @brief Parse a single line and return syntax highlighting information.
	 * @param nLineIndex Zero-based line index to parse.
	 * @return Vector of TEXTBLOCK describing token runs for this line.
	 */
	virtual std::vector<TEXTBLOCK> ParseLine(int nLineIndex) = 0;

	/**
	 * @brief Get the parser type for this syntax parser.
	 * @return The LanguageId enum value representing the language/format.
	 */
	virtual LanguageId GetParserType() const = 0;

	/**
	 * @brief Notify the parser of a detailed text edit for incremental parsing.
	 * @param bInsert true if inserting text, false if deleting.
	 * @param ptStartPos Start position of the edit.
	 * @param ptEndPos End position of the edit.
	 * @param pszText The inserted/deleted text.
	 * @param cchText Number of characters in pszText.
	 * @param nActionType Edit action type identifier.
	 *
	 * Implementations that support incremental reparsing (like Tree-sitter) should
	 * override this method to perform efficient updates. The default implementation
	 * does nothing.
	 */
	virtual void NotifyEdit(bool bInsert, const CEPoint& ptStartPos,
		const CEPoint& ptEndPos, const tchar_t* pszText,
		size_t cchText, int nActionType) { /* default: no-op */
	}

	/**
	 * @brief Find the matching brace/bracket/parenthesis for the given position.
	 * @param nLineIndex Zero-based line index of the starting position.
	 * @param nCharPos Zero-based character position of the starting position.
	 * @param outLineIndex [out] Line index of the matching brace (if found).
	 * @param outCharPos [out] Character position of the matching brace (if found).
	 * @return true if a matching brace was found, false otherwise.
	 *
	 * The default implementation returns false (no brace matching support).
	 */
	virtual bool FindMatchingBrace(int nLineIndex, int nCharPos,
		int& outLineIndex, int& outCharPos) {
		return false;
	}
};

/**
 * @brief Abstract factory interface for creating ISyntaxParser instances.
 *
 * Implement this interface to provide a custom syntax parser backend
 * (e.g., a Tree-sitter grammar pack, a language server, etc.).
 * Factories are registered with SyntaxParserRegistry at application startup
 * and are selected based on priority when creating parsers.
 *
 * Dependency constraints: this file MUST NOT include CCrystalTextView or
 * crystallineparser.h. Only ISyntaxParser.h is allowed.
 */
struct ISyntaxParserFactory
{
	virtual ~ISyntaxParserFactory() = default;

	/**
	 * @brief Create a parser instance for the given LanguageId.
	 * @param type The language / file-format type.
	 * @return A newly created ISyntaxParser, or nullptr if this factory
	 *         does not support the requested type.
	 */
	virtual std::shared_ptr<ISyntaxParser> Create(LanguageId type) const = 0;
};

}

