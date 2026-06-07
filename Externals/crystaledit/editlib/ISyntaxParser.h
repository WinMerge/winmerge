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
 *
 * TEXTBLOCK and LanguageId are defined here as the canonical location so that
 * ISyntaxParser.h does not depend on crystallineparser.h. crystallineparser.h
 * provides backward-compatible using-aliases pointing to these definitions.
 */
class ISyntaxParser
{
public:

	virtual ~ISyntaxParser() = default;

	/**
	 * @brief Set the text buffer that this parser will operate on.
	 * @param pTextBuffer Pointer to the text buffer interface.
	 */
	virtual void SetTextBuffer(LangServices::ITextBuffer* pTextBuffer) = 0;

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
		int& outLineIndex, int& outCharPos) const {
		return false;
	}
};

}

