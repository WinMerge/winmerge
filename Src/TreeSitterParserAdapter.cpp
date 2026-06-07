#include "pch.h"
#include <Windows.h>
#include "TreeSitterParserAdapter.h"

/**
 * @brief Construct a Tree-sitter parser adapter.
 */
TreeSitterParserAdapter::TreeSitterParserAdapter(const CTreeSitterLanguage* pLanguage)
	: m_pTextBuffer(nullptr)
	, m_pLanguage(pLanguage)
{
	m_parser.SetLanguage(pLanguage);
}

/**
 * @brief Set the text buffer that this parser will operate on.
 */
void TreeSitterParserAdapter::SetTextBuffer(LangServices::ITextBuffer* pTextBuffer)
{
	m_pTextBuffer = pTextBuffer;

	// Parse the entire document when buffer is set
	if (m_pTextBuffer != nullptr)
	{
		m_parser.ParseFromBuffer(m_pTextBuffer);
	}
	else
	{
		m_parser.Invalidate();
	}
}

/**
 * @brief Parse a single line and return syntax highlighting information.
 */
std::vector<LangServices::TEXTBLOCK> TreeSitterParserAdapter::ParseLine(int nLineIndex)
{
	if (m_pTextBuffer == nullptr)
		return {};

	// Ensure the document is parsed (handles lazy reparsing if dirty)
	m_parser.EnsureParsed(m_pTextBuffer);

	// Get the cached color blocks for this line
	return m_parser.GetLineBlocks(nLineIndex);
}

/**
 * @brief Notify the parser of a detailed text edit for incremental parsing.
 * This enables efficient incremental reparsing using tree-sitter's built-in
 * incremental parsing feature.
 */
void TreeSitterParserAdapter::NotifyEdit(bool bInsert, const CEPoint & ptStartPos, const CEPoint & ptEndPos, const tchar_t* pszText, size_t cchText, int nActionType)
{
	if (m_pTextBuffer == nullptr)
	{
		return;
	}

	// Delegate to the underlying Tree-sitter parser for incremental reparsing
	m_parser.NotifyEdit(bInsert, ptStartPos, ptEndPos, pszText, cchText, nActionType);
}

/**
 * @brief Get the parser type for this syntax parser.
 */
LangServices::LanguageId TreeSitterParserAdapter::GetParserType() const
{
	// Tree-sitter parsers don't map directly to the legacy LanguageId enum
	// Return Plain as a placeholder; callers should check if parser is Tree-sitter-based
	return LangServices::LanguageId::SRC_PLAIN;
}

/**
 * @brief Find the matching brace/bracket/parenthesis for the given position.
 */
bool TreeSitterParserAdapter::FindMatchingBrace(int nLineIndex, int nCharPos, int& outLineIndex, int& outCharPos) const
{
	if (m_pTextBuffer == nullptr)
	{
		return false;
	}

	// Ensure parsed
	const_cast<CTreeSitterParser&>(m_parser).EnsureParsed(m_pTextBuffer);

	// Delegate to the underlying Tree-sitter parser
	return m_parser.FindMatchingBrace(m_pTextBuffer, nLineIndex, nCharPos, outLineIndex, outCharPos);
}
