#include "pch.h"
#include <Windows.h>
#include "CrystalLineParserAdapter.h"
#include <algorithm>

/**
 * @brief Construct a parser adapter for a specific text type.
 */
CrystalLineParserAdapter::CrystalLineParserAdapter(CrystalLineParser::TextType textType)
	: m_pTextBuffer(nullptr)
	, m_textType(textType)
	, m_pTextDef(nullptr)
{
	// Find the text definition for this type
	if (textType >= 0 && textType < CrystalLineParser::SRC_MAX_ENTRY)
	{
		m_pTextDef = &CrystalLineParser::m_SourceDefs[textType];
	}
}

/**
 * @brief Set the text buffer that this parser will operate on.
 */
void CrystalLineParserAdapter::SetTextBuffer(ITextBuffer* pTextBuffer)
{
	m_pTextBuffer = pTextBuffer;

	// Reset parse state when buffer changes
	m_ParseCookies.clear();
	if (m_pTextBuffer != nullptr)
	{
		int nLineCount = m_pTextBuffer->GetLineCount();
		m_ParseCookies.resize(nLineCount + 1, 0);
	}
}

/**
 * @brief Parse a single line and return syntax highlighting information.
 */
unsigned CrystalLineParserAdapter::ParseLine(int nLineIndex, CrystalLineParser::TEXTBLOCK* pBuf, int& nActualItems)
{
	if (m_pTextBuffer == nullptr || m_pTextDef == nullptr || m_pTextDef->ParseLineX == nullptr)
	{
		nActualItems = 0;
		return 0;
	}

	// Ensure parse cookie cache is large enough
	int nLineCount = m_pTextBuffer->GetLineCount();
	if (static_cast<int>(m_ParseCookies.size()) < nLineCount + 1)
	{
		m_ParseCookies.resize(nLineCount + 1, 0);
	}

	// Get the line text
	const tchar_t* pszChars = m_pTextBuffer->GetLineChars(nLineIndex);
	int nLength = m_pTextBuffer->GetLineLength(nLineIndex);

	// Get the cookie from the previous line
	unsigned dwCookie = GetLineCookie(nLineIndex);

	// Call the legacy parser function
	unsigned dwNewCookie = m_pTextDef->ParseLineX(dwCookie, nLineIndex, pszChars, nLength, pBuf, nActualItems, nullptr);

	// Cache the result cookie for the next line
	if (nLineIndex < nLineCount)
	{
		m_ParseCookies[nLineIndex + 1] = dwNewCookie;
	}

	return dwNewCookie;
}

/**
 * @brief Notify the parser that the text buffer has been modified.
 */
void CrystalLineParserAdapter::OnTextChanged(int nStartLine, int nEndLine)
{
	// Invalidate parse state from the first modified line
	InvalidateFromLine(nStartLine);
}

/**
 * @brief Get the parser type for this syntax parser.
 */
CrystalLineParser::TextType CrystalLineParserAdapter::GetParserType() const
{
	return m_textType;
}

/**
 * @brief Get or compute the parser cookie for a specific line.
 */
unsigned CrystalLineParserAdapter::GetLineCookie(int nLineIndex)
{
	if (nLineIndex <= 0 || m_pTextBuffer == nullptr)
	{
		return 0; // First line always starts with cookie 0
	}

	// Ensure the cookie cache is large enough
	int nLineCount = m_pTextBuffer->GetLineCount();
	if (static_cast<int>(m_ParseCookies.size()) < nLineCount + 1)
	{
		m_ParseCookies.resize(nLineCount + 1, 0);
	}

	// If we already have a valid cookie, return it
	if (m_ParseCookies[nLineIndex] != 0 || nLineIndex == 0)
	{
		return m_ParseCookies[nLineIndex];
	}

	// Otherwise, parse previous lines until we find a valid cookie
	int nParseLine = nLineIndex - 1;
	while (nParseLine > 0 && m_ParseCookies[nParseLine] == 0)
	{
		nParseLine--;
	}

	// Parse forward from the last valid cookie to compute the cookie for nLineIndex
	for (int i = nParseLine; i < nLineIndex; i++)
	{
		int nActualItems = 0;
		ParseLine(i, nullptr, nActualItems);
	}

	return m_ParseCookies[nLineIndex];
}

/**
 * @brief Invalidate cached parse state from a specific line onward.
 */
void CrystalLineParserAdapter::InvalidateFromLine(int nStartLine)
{
	if (nStartLine < 0 || m_pTextBuffer == nullptr)
	{
		return;
	}

	int nLineCount = m_pTextBuffer->GetLineCount();
	if (static_cast<int>(m_ParseCookies.size()) < nLineCount + 1)
	{
		m_ParseCookies.resize(nLineCount + 1, 0);
	}

	// Clear cookies from nStartLine+1 onward (the start line's cookie is still valid from previous line)
	for (size_t i = nStartLine + 1; i < m_ParseCookies.size(); i++)
	{
		m_ParseCookies[i] = 0;
	}
}
