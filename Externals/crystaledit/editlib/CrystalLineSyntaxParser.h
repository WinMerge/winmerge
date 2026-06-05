#pragma once

#include "ISyntaxParser.h"
#include "parsers/crystallineparser.h"
#include <vector>
#include <memory>

/**
 * @brief Adapter that wraps legacy CrystalEdit line-based parsers.
 *
 * This class adapts the traditional ParseLineX function pointer style
 * to the ISyntaxParser interface, managing per-line cookie state internally.
 */
class CrystalLineSyntaxParser : public ISyntaxParser
{
public:
	/**
	 * @brief Construct a parser adapter for a specific text type.
	 * @param textType The language/format type to parse.
	 */
	explicit CrystalLineSyntaxParser(ISyntaxParser::TextType textType);
	virtual ~CrystalLineSyntaxParser() = default;

	// ISyntaxParser interface implementation
	void SetTextBuffer(ITextBuffer* pTextBuffer) override;
	std::vector<ISyntaxParser::TEXTBLOCK> ParseLine(int nLineIndex) override;
	void NotifyEdit(bool bInsert, const CEPoint & ptStartPos, const CEPoint & ptEndPos, const tchar_t* pszText, size_t cchText, int nActionType) override;
	ISyntaxParser::TextType GetParserType() const override;
	bool FindMatchingBrace(int nLineIndex, int nCharPos, int& outLineIndex, int& outCharPos) const override;

private:
	/**
	 * @brief Get or compute the parser cookie for a specific line.
	 * @param nLineIndex Zero-based line index.
	 * @return The cookie value for this line.
	 */
	unsigned GetLineCookie(int nLineIndex);

	/**
	 * @brief Invalidate cached parse state from a specific line onward.
	 * @param nStartLine Zero-based index of the first line to invalidate.
	 */
	void InvalidateFromLine(int nStartLine);

	ITextBuffer* m_pTextBuffer;                    ///< Text buffer interface
	ISyntaxParser::TextType m_textType;            ///< Parser type
	CrystalLineParser::TextDefinition* m_pTextDef; ///< Parser definition
	std::vector<unsigned> m_ParseCookies;          ///< Per-line parser state cookies
};
