#include "pch.h"
#include "parsers/crystallineparser.h"
#include "CrystalLineSyntaxParser.h"
#include "SyntaxColors.h"
#include <algorithm>
#include <map>

using LangServices::LanguageId;

static const std::pair<LanguageId, ParseFunc> m_ParseXDef[] =
{
{ LanguageId::SRC_PLAIN, CrystalLineParser::ParseLinePlain },
{ LanguageId::SRC_ABAP, CrystalLineParser::ParseLineAbap },
{ LanguageId::SRC_ADA, CrystalLineParser::ParseLineAda },
{ LanguageId::SRC_ASP, CrystalLineParser::ParseLineAsp },
{ LanguageId::SRC_AUTOIT, CrystalLineParser::ParseLineAutoIt },
{ LanguageId::SRC_BASIC, CrystalLineParser::ParseLineBasic },
{ LanguageId::SRC_BATCH, CrystalLineParser::ParseLineBatch },
{ LanguageId::SRC_C, CrystalLineParser::ParseLineC },
{ LanguageId::SRC_CSHARP, CrystalLineParser::ParseLineCSharp },
{ LanguageId::SRC_CSS, CrystalLineParser::ParseLineCss },
{ LanguageId::SRC_DCL, CrystalLineParser::ParseLineDcl },
{ LanguageId::SRC_DLANG, CrystalLineParser::ParseLineDlang },
{ LanguageId::SRC_FORTRAN, CrystalLineParser::ParseLineFortran },
{ LanguageId::SRC_FSHARP, CrystalLineParser::ParseLineFSharp },
{ LanguageId::SRC_GO, CrystalLineParser::ParseLineGo },
{ LanguageId::SRC_HTML, CrystalLineParser::ParseLineHtml },
{ LanguageId::SRC_INI, CrystalLineParser::ParseLineIni },
{ LanguageId::SRC_INNOSETUP, CrystalLineParser::ParseLineInnoSetup },
{ LanguageId::SRC_INSTALLSHIELD, CrystalLineParser::ParseLineIS },
{ LanguageId::SRC_JAVA, CrystalLineParser::ParseLineJava },
{ LanguageId::SRC_JAVASCRIPT, CrystalLineParser::ParseLineJavaScript },
{ LanguageId::SRC_JSON, CrystalLineParser::ParseLineJavaScript },
{ LanguageId::SRC_LISP, CrystalLineParser::ParseLineLisp },
{ LanguageId::SRC_LUA, CrystalLineParser::ParseLineLua },
{ LanguageId::SRC_MATLAB, CrystalLineParser::ParseLineMatlab },
{ LanguageId::SRC_NSIS, CrystalLineParser::ParseLineNsis },
{ LanguageId::SRC_PASCAL, CrystalLineParser::ParseLinePascal },
{ LanguageId::SRC_PERL, CrystalLineParser::ParseLinePerl },
{ LanguageId::SRC_PHP, CrystalLineParser::ParseLinePhp },
{ LanguageId::SRC_PO, CrystalLineParser::ParseLinePo },
{ LanguageId::SRC_POWERSHELL, CrystalLineParser::ParseLinePowerShell },
{ LanguageId::SRC_PYTHON, CrystalLineParser::ParseLinePython },
{ LanguageId::SRC_REXX, CrystalLineParser::ParseLineRexx },
{ LanguageId::SRC_RSRC, CrystalLineParser::ParseLineRsrc },
{ LanguageId::SRC_RUBY, CrystalLineParser::ParseLineRuby },
{ LanguageId::SRC_RUST, CrystalLineParser::ParseLineRust },
{ LanguageId::SRC_SGML, CrystalLineParser::ParseLineSgml },
{ LanguageId::SRC_SH, CrystalLineParser::ParseLineSh },
{ LanguageId::SRC_SIOD, CrystalLineParser::ParseLineSiod },
{ LanguageId::SRC_SMARTY, CrystalLineParser::ParseLineSmarty },
{ LanguageId::SRC_SQL, CrystalLineParser::ParseLineSql },
{ LanguageId::SRC_TCL, CrystalLineParser::ParseLineTcl },
{ LanguageId::SRC_TEX, CrystalLineParser::ParseLineTex },
{ LanguageId::SRC_VERILOG, CrystalLineParser::ParseLineVerilog },
{ LanguageId::SRC_VHDL, CrystalLineParser::ParseLineVhdl },
{ LanguageId::SRC_XML, CrystalLineParser::ParseLineXml },
};

bool CrystalLineSyntaxParserFactory::IsSupported(LanguageId textType) const
{
	for (const auto& pair : m_ParseXDef)
		if (textType == pair.first)
			return true;
	return false;
}

/**
 * @brief Construct a parser adapter for a specific text type.
 */
CrystalLineSyntaxParser::CrystalLineSyntaxParser(LanguageId textType)
	: m_pTextBuffer(nullptr)
	, m_textType(textType)
	, m_pTextDef(nullptr)
	, m_ParseLineX(nullptr)
{
	// Find the text definition for this type
	for (const auto& pair : m_ParseXDef)
	{
		if (textType == pair.first)
		{
			m_pTextDef = LangServices::GetTextType(textType);
			m_ParseLineX = pair.second;
		}
	}
}

/**
 * @brief Set the text buffer that this parser will operate on.
 */
void CrystalLineSyntaxParser::SetTextBuffer(LangServices::ITextBuffer* pTextBuffer)
{
	m_pTextBuffer = pTextBuffer;

	// Reset parse state when buffer changes
	m_ParseCookies.clear();
}

/**
 * @brief Parse a single line and return syntax highlighting information.
 */
std::vector<LangServices::TEXTBLOCK> CrystalLineSyntaxParser::ParseLine(int nLineIndex)
{
	std::vector<LangServices::TEXTBLOCK> blocks;

	if (m_pTextBuffer == nullptr || m_ParseLineX == nullptr)
		return blocks;

	// Ensure parse cookie cache is large enough
	int nLineCount = m_pTextBuffer->GetLineCount();
	if (static_cast<int>(m_ParseCookies.size()) < nLineCount + 1)
		m_ParseCookies.resize(nLineCount + 1, static_cast<unsigned>(-1));

	// Get the line text
	const tchar_t* pszChars = m_pTextBuffer->GetLineChars(nLineIndex);
	int nLength = m_pTextBuffer->GetLineLength(nLineIndex);

	// Get the cookie from the previous line
	unsigned dwCookie = GetLineCookie(nLineIndex);

	// Call the legacy parser function
	unsigned dwNewCookie = m_ParseLineX(dwCookie, pszChars, nLength, &blocks);

	// Cache the result cookie for the next line
	if (nLineIndex < nLineCount)
		m_ParseCookies[nLineIndex + 1] = dwNewCookie;

	return blocks;
}

/**
 * @brief Notify the parser of a detailed text edit.
 * Legacy line-based parsers don't use detailed edit information;
 * they just invalidate state from the start line.
 */
void CrystalLineSyntaxParser::NotifyEdit(bool bInsert, const CEPoint & ptStartPos, const CEPoint & ptEndPos, const tchar_t* pszText, size_t cchText, int nActionType)
{
	// Invalidate from the start line of the edit
	InvalidateFromLine(ptStartPos.y);
}

/**
 * @brief Get the parser type for this syntax parser.
 */
LanguageId CrystalLineSyntaxParser::GetParserType() const
{
	return m_textType;
}

/**
 * @brief Get or compute the parser cookie for a specific line.
 */
unsigned CrystalLineSyntaxParser::GetLineCookie(int nLineIndex)
{
	if (nLineIndex <= 0 || m_pTextBuffer == nullptr)
		return 0; // First line always starts with cookie 0

	// Ensure the cookie cache is large enough
	int nLineCount = m_pTextBuffer->GetLineCount();
	if (static_cast<int>(m_ParseCookies.size()) < nLineCount + 1)
		m_ParseCookies.resize(nLineCount + 1, static_cast<unsigned>(-1));

	// If we already have a valid cookie, return it
	if (m_ParseCookies[nLineIndex] != -1)
		return m_ParseCookies[nLineIndex];

	// Special case: if requesting cookie for line 0, it's always 0
	if (nLineIndex == 0)
		return 0;

	if (m_pTextDef != nullptr && m_ParseLineX != nullptr)
	{
		int start = nLineIndex - 1;
		while (start > 0 && m_ParseCookies[start] == -1)
			--start;

		unsigned cookie = (start == 0) ? 0 : m_ParseCookies[start];
		for (int i = start; i < nLineIndex; ++i)
		{
			const tchar_t* pszChars = m_pTextBuffer->GetLineChars(i);
			int nLength = m_pTextBuffer->GetLineLength(i);
			m_ParseCookies[i + 1] = m_ParseLineX(cookie, pszChars, nLength, nullptr);
		}
	}

	return m_ParseCookies[nLineIndex];
}

/**
 * @brief Invalidate cached parse state from a specific line onward.
 */
void CrystalLineSyntaxParser::InvalidateFromLine(int nStartLine)
{
	if (nStartLine < 0 || m_pTextBuffer == nullptr)
		return;

	int nLineCount = m_pTextBuffer->GetLineCount();
	if (static_cast<int>(m_ParseCookies.size()) < nLineCount + 1)
		m_ParseCookies.resize(nLineCount + 1, static_cast<unsigned>(-1));

	// Clear cookies from nStartLine+1 onward (the start line's cookie is still valid from previous line)
	for (size_t i = nStartLine + 1; i < m_ParseCookies.size(); i++)
		m_ParseCookies[i] = static_cast<unsigned>(-1);
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
bool CrystalLineSyntaxParser::FindMatchingBrace(int nLineIndex, int nCharPos, int& outLineIndex, int& outCharPos) const
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
