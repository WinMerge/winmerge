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
	unsigned dwNewCookie = m_pTextDef->ParseLineX(dwCookie, pszChars, nLength, pBuf, nActualItems, nullptr);

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
	if (m_ParseCookies[nLineIndex] != 0)
	{
		return m_ParseCookies[nLineIndex];
	}

	// Special case: if requesting cookie for line 0, it's always 0
	if (nLineIndex == 0)
	{
		return 0;
	}

	// Otherwise, get cookie from previous line and parse current line
	// This is more efficient than parsing all previous lines
	unsigned dwPrevCookie = GetLineCookie(nLineIndex - 1);

	// Now parse line nLineIndex-1 to compute cookie for nLineIndex
	const tchar_t* pszChars = m_pTextBuffer->GetLineChars(nLineIndex - 1);
	int nLength = m_pTextBuffer->GetLineLength(nLineIndex - 1);
	int nActualItems = 0;

	if (m_pTextDef != nullptr && m_pTextDef->ParseLineX != nullptr)
	{
		unsigned dwNewCookie = m_pTextDef->ParseLineX(dwPrevCookie, pszChars, nLength, nullptr, nActualItems, nullptr);
		m_ParseCookies[nLineIndex] = dwNewCookie;
		return dwNewCookie;
	}

	return 0;
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
