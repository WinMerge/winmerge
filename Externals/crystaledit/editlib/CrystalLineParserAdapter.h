#pragma once

#include "ISyntaxParser.h"
#include <vector>
#include <memory>

/**
 * @brief Adapter that wraps legacy CrystalEdit line-based parsers.
 *
 * This class adapts the traditional ParseLineX function pointer style
 * to the ISyntaxParser interface, managing per-line cookie state internally.
 */
class CrystalLineParserAdapter : public ISyntaxParser
{
public:
	/**
	 * @brief Construct a parser adapter for a specific text type.
	 * @param textType The language/format type to parse.
	 */
	explicit CrystalLineParserAdapter(CrystalLineParser::TextType textType);
	virtual ~CrystalLineParserAdapter() = default;

	// ISyntaxParser interface implementation
	void SetTextBuffer(ITextBuffer* pTextBuffer) override;
	unsigned ParseLine(int nLineIndex, CrystalLineParser::TEXTBLOCK* pBuf, int& nActualItems) override;
	void OnTextChanged(int nStartLine, int nEndLine) override;
	void NotifyEdit(const TextEdit& textEdit) override;
	CrystalLineParser::TextType GetParserType() const override;
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
	CrystalLineParser::TextType m_textType;        ///< Parser type
	CrystalLineParser::TextDefinition* m_pTextDef; ///< Parser definition
	std::vector<unsigned> m_ParseCookies;          ///< Per-line parser state cookies
};
