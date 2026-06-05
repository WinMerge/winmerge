#pragma once

#include "../Externals/crystaledit/editlib/ISyntaxParser.h"
#include "TreeSitterParser.h"

/**
 * @brief Adapter that wraps CTreeSitterParser to implement ISyntaxParser.
 *
 * This class bridges the whole-document Tree-sitter parser to the line-based
 * ISyntaxParser interface, managing incremental updates and lazy reparsing.
 */
class TreeSitterParserAdapter : public ISyntaxParser
{
public:
	/**
	 * @brief Construct a Tree-sitter parser adapter.
	 * @param pLanguage Pointer to a loaded CTreeSitterLanguage.
	 */
	explicit TreeSitterParserAdapter(const CTreeSitterLanguage* pLanguage);
	virtual ~TreeSitterParserAdapter() = default;

	// ISyntaxParser interface implementation
	void SetTextBuffer(ITextBuffer* pTextBuffer) override;
	std::vector<ISyntaxParser::TEXTBLOCK> ParseLine(int nLineIndex) override;
	void NotifyEdit(bool bInsert, const CEPoint & ptStartPos, const CEPoint & ptEndPos, const tchar_t* pszText, size_t cchText, int nActionType) override;
	ISyntaxParser::TextType GetParserType() const override;
	bool FindMatchingBrace(int nLineIndex, int nCharPos, int& outLineIndex, int& outCharPos) const override;

	/**
	 * @brief Get the underlying Tree-sitter parser for advanced operations.
	 * @return Pointer to the CTreeSitterParser instance.
	 */
	CTreeSitterParser* GetTreeSitterParser() { return &m_parser; }
	const CTreeSitterParser* GetTreeSitterParser() const { return &m_parser; }

private:
	ITextBuffer* m_pTextBuffer;           ///< Text buffer interface
	CTreeSitterParser m_parser;           ///< Underlying Tree-sitter parser
	const CTreeSitterLanguage* m_pLanguage; ///< Language definition
};
