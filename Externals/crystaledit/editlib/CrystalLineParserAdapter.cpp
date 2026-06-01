#include "pch.h"
#include <Windows.h>
#include "CrystalLineParserAdapter.h"
#include "SyntaxColors.h"
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
 * @brief Check if a specific position is inside a comment.
 */
bool CrystalLineParserAdapter::IsCommentPosition(int nLineIndex, int nCharPos) const
{
	if (m_pTextBuffer == nullptr || nLineIndex < 0 || nLineIndex >= m_pTextBuffer->GetLineCount())
	{
		return false;
	}

	// Parse the line to get syntax blocks
	const int MAX_BLOCKS = 256;
	CrystalLineParser::TEXTBLOCK blocks[MAX_BLOCKS];
	int nActualItems = 0;

	// We need to call ParseLine, but it's non-const and modifies cache
	// Cast away const - this is safe because ParseLine only updates internal cache
	const_cast<CrystalLineParserAdapter*>(this)->ParseLine(nLineIndex, blocks, nActualItems);

	// Find the block containing nCharPos
	for (int i = 0; i < nActualItems; i++)
	{
		int nBlockStart = blocks[i].m_nCharPos;
		int nBlockEnd = (i + 1 < nActualItems) ? blocks[i + 1].m_nCharPos : m_pTextBuffer->GetLineLength(nLineIndex);

		if (nCharPos >= nBlockStart && nCharPos < nBlockEnd)
		{
			// Check if this block is a comment
			return blocks[i].m_nColorIndex == COLORINDEX_COMMENT;
		}
	}

	return false;
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

namespace {
	/**
	 * @brief Helper to identify brace type: returns 1-8 for {}()[]<>, 0 otherwise.
	 */
	int bracetype(tchar_t c)
	{
		static const tchar_t* braces = _T("{}()[]<>");
		const tchar_t* pos = tc::tcschr(braces, c);
		return pos != nullptr ? static_cast<int>(pos - braces) + 1 : 0;
	}
}

/**
 * @brief Find the matching brace/bracket/parenthesis for the given position.
 */
bool CrystalLineParserAdapter::FindMatchingBrace(int nLineIndex, int nCharPos, int& outLineIndex, int& outCharPos) const
{
	if (m_pTextBuffer == nullptr || m_pTextDef == nullptr)
	{
		return false;
	}

	int nLineCount = m_pTextBuffer->GetLineCount();
	if (nLineIndex < 0 || nLineIndex >= nLineCount)
	{
		return false;
	}

	int nLength = m_pTextBuffer->GetLineLength(nLineIndex);
	const tchar_t* pszText = m_pTextBuffer->GetLineChars(nLineIndex);
	const tchar_t* pszEnd = pszText + nCharPos;

	// Determine which brace we're on and search direction
	bool bAfter = false;
	int nType = 0;

	if (nCharPos < nLength)
	{
		nType = bracetype(*pszEnd);
		if (nType)
		{
			bAfter = false;
		}
		else if (nCharPos > 0)
		{
			nType = bracetype(pszEnd[-1]);
			bAfter = true;
		}
	}
	else if (nCharPos > 0)
	{
		nType = bracetype(pszEnd[-1]);
		bAfter = true;
	}

	if (!nType)
	{
		return false; // Not on a brace
	}

	// Calculate matching brace type and initial position
	int nOther = ((nType - 1) ^ 1) + 1;
	int nCount = 0;
	int nComment = 0;

	if (bAfter)
	{
		if (nOther & 1)
			pszEnd--;
	}
	else
	{
		if (!(nOther & 1))
			pszEnd++;
	}

	// Get comment delimiters
	const tchar_t* pszOpenComment = m_pTextDef->opencomment;
	const tchar_t* pszCloseComment = m_pTextDef->closecomment;
	const tchar_t* pszCommentLine = m_pTextDef->commentline;

	int nOpenComment = static_cast<int>(tc::tcslen(pszOpenComment));
	int nCloseComment = static_cast<int>(tc::tcslen(pszCloseComment));
	int nCommentLine = static_cast<int>(tc::tcslen(pszCommentLine));

	int ptY = nLineIndex;
	int ptX = nCharPos;

	// Search backward or forward
	if (nOther & 1)
	{
		// Search backward
		for (;;)
		{
			while (--pszEnd >= pszText)
			{
				const tchar_t* pszTest = pszEnd - nOpenComment + 1;
				if (pszTest >= pszText && nOpenComment > 0 && !tc::tcsnicmp(pszTest, pszOpenComment, nOpenComment))
				{
					nComment--;
					pszEnd = pszTest;
					if (--pszEnd < pszText)
						break;
				}
				pszTest = pszEnd - nCloseComment + 1;
				if (pszTest >= pszText && nCloseComment > 0 && !tc::tcsnicmp(pszTest, pszCloseComment, nCloseComment))
				{
					nComment++;
					pszEnd = pszTest;
					if (--pszEnd < pszText)
						break;
				}
				if (!nComment)
				{
					// Check if in comment
					pszTest = pszEnd - nCommentLine + 1;
					if (pszTest >= pszText && nCommentLine > 0 && !tc::tcsnicmp(pszTest, pszCommentLine, nCommentLine))
						break;

					if (bracetype(*pszEnd) == nType)
					{
						nCount++;
					}
					else if (bracetype(*pszEnd) == nOther)
					{
						if (!nCount--)
						{
							outLineIndex = ptY;
							outCharPos = static_cast<int>(pszEnd - pszText);
							if (bAfter)
								outCharPos++;
							return true;
						}
					}
				}
			}
			if (ptY > 0)
			{
				ptY--;
				ptX = m_pTextBuffer->GetLineLength(ptY);
				pszText = m_pTextBuffer->GetLineChars(ptY);
				pszEnd = pszText + ptX;
			}
			else
				break;
		}
	}
	else
	{
		// Search forward
		const tchar_t* pszBegin = pszText;
		pszText = pszEnd;
		pszEnd = pszBegin + nLength;

		for (;;)
		{
			while (pszText < pszEnd)
			{
				const tchar_t* pszTest = pszText + nCloseComment;
				if (pszTest <= pszEnd && nCloseComment > 0 && !tc::tcsnicmp(pszText, pszCloseComment, nCloseComment))
				{
					nComment--;
					pszText = pszTest;
					if (pszText > pszEnd)
						break;
				}
				pszTest = pszText + nOpenComment;
				if (pszTest <= pszEnd && nOpenComment > 0 && !tc::tcsnicmp(pszText, pszOpenComment, nOpenComment))
				{
					nComment++;
					pszText = pszTest;
					if (pszText > pszEnd)
						break;
				}
				if (!nComment)
				{
					// Check if in comment
					pszTest = pszText + nCommentLine;
					if (pszTest <= pszEnd && nCommentLine > 0 && !tc::tcsnicmp(pszText, pszCommentLine, nCommentLine))
						break;

					if (bracetype(*pszText) == nType)
					{
						nCount++;
					}
					else if (bracetype(*pszText) == nOther)
					{
						if (!nCount--)
						{
							outLineIndex = ptY;
							outCharPos = static_cast<int>(pszText - pszBegin);
							if (bAfter)
								outCharPos++;
							return true;
						}
					}
				}
				pszText++;
			}
			if (ptY < nLineCount - 1)
			{
				ptY++;
				ptX = 0;
				nLength = m_pTextBuffer->GetLineLength(ptY);
				pszBegin = pszText = m_pTextBuffer->GetLineChars(ptY);
				pszEnd = pszBegin + nLength;
			}
			else
				break;
		}
	}

	return false;
}
