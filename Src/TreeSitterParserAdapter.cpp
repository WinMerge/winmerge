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
void TreeSitterParserAdapter::SetTextBuffer(ITextBuffer* pTextBuffer)
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
unsigned TreeSitterParserAdapter::ParseLine(int nLineIndex, CrystalLineParser::TEXTBLOCK* pBuf, int& nActualItems)
{
	if (m_pTextBuffer == nullptr)
	{
		nActualItems = 0;
		return 0;
	}

	// Ensure the document is parsed (handles lazy reparsing if dirty)
	m_parser.EnsureParsed(m_pTextBuffer);

	// Get the cached color blocks for this line
	m_parser.GetLineBlocks(nLineIndex, pBuf, nActualItems);

	// Tree-sitter doesn't use cookies (returns 0)
	return 0;
}

/**
 * @brief Notify the parser that the text buffer has been modified.
 */
void TreeSitterParserAdapter::OnTextChanged(int nStartLine, int nEndLine)
{
	// Mark the parser as dirty; it will reparse on the next ParseLine call
	m_parser.MarkDirty();
}

/**
 * @brief Get the parser type for this syntax parser.
 */
CrystalLineParser::TextType TreeSitterParserAdapter::GetParserType() const
{
	// Tree-sitter parsers don't map directly to the legacy TextType enum
	// Return SRC_PLAIN as a placeholder; callers should check if parser is Tree-sitter-based
	return CrystalLineParser::SRC_PLAIN;
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
