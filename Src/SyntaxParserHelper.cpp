#include "pch.h"
#include <Windows.h>
#include "SyntaxParserHelper.h"
#include "SyntaxColors.h"
#include "utils/ctchar.h"
#include <vector>

/**
 * @brief Get text with comments filtered out.
 */
std::string SyntaxParserHelper::GetCommentsFilteredText(
	ISyntaxParser* pParser,
	ITextBuffer* pTextBuffer,
	int nStartLine,
	int nEndLine)
{
	if (pParser == nullptr || pTextBuffer == nullptr)
		return std::string();

	std::string result;

	// Process each line in the range
	for (int nLine = nStartLine; nLine <= nEndLine; nLine++)
	{
		if (nLine < 0 || nLine >= pTextBuffer->GetLineCount())
			continue;

		const tchar_t* pszChars = pTextBuffer->GetLineChars(nLine);
		int nLength = pTextBuffer->GetLineLength(nLine);

		if (nLength == 0)
		{
			result += '\n';
			continue;
		}

		// Parse the line to get color blocks
		std::vector<CrystalLineParser::TEXTBLOCK> blocks(nLength + 1);
		int nActualItems = 0;
		pParser->ParseLine(nLine, blocks.data(), nActualItems);

		// Extract non-comment text
		std::string lineText;
		int nCurrentPos = 0;

		for (int i = 0; i < nActualItems; i++)
		{
			int nBlockStart = blocks[i].m_nCharPos;
			int nBlockEnd = (i + 1 < nActualItems) ? blocks[i + 1].m_nCharPos : nLength;
			int nColorIndex = blocks[i].m_nColorIndex;

			// Skip comment blocks
			if (nColorIndex == COLORINDEX_COMMENT)
			{
				nCurrentPos = nBlockEnd;
				continue;
			}

			// Append non-comment text
			if (nBlockEnd > nBlockStart)
			{
#ifdef _UNICODE
				// Convert wide char to UTF-8
				int nBlockLen = nBlockEnd - nBlockStart;
				int nUtf8Len = WideCharToMultiByte(CP_UTF8, 0, pszChars + nBlockStart, nBlockLen, nullptr, 0, nullptr, nullptr);
				if (nUtf8Len > 0)
				{
					std::vector<char> utf8Buffer(nUtf8Len);
					WideCharToMultiByte(CP_UTF8, 0, pszChars + nBlockStart, nBlockLen, utf8Buffer.data(), nUtf8Len, nullptr, nullptr);
					lineText.append(utf8Buffer.data(), nUtf8Len);
				}
#else
				lineText.append(pszChars + nBlockStart, nBlockEnd - nBlockStart);
#endif
			}

			nCurrentPos = nBlockEnd;
		}

		result += lineText;
		result += '\n';
	}

	return result;
}

/**
 * @brief Get text with comments filtered out and per-line comment status.
 */
std::string SyntaxParserHelper::GetCommentsFilteredText(
	ISyntaxParser* pParser,
	ITextBuffer* pTextBuffer,
	int nStartLine,
	int nEndLine,
	std::vector<bool>& allTextIsComment)
{
	if (pParser == nullptr || pTextBuffer == nullptr)
	{
		allTextIsComment.clear();
		return std::string();
	}

	std::string result;
	int nLineCount = nEndLine - nStartLine + 1;
	allTextIsComment.resize(nLineCount, true);

	// Process each line in the range
	for (int nLine = nStartLine; nLine <= nEndLine; nLine++)
	{
		int lineIndex = nLine - nStartLine;

		if (nLine < 0 || nLine >= pTextBuffer->GetLineCount())
		{
			allTextIsComment[lineIndex] = false;
			continue;
		}

		const tchar_t* pszChars = pTextBuffer->GetLineChars(nLine);
		int nLength = pTextBuffer->GetLineLength(nLine);

		if (nLength == 0)
		{
			result += '\n';
			allTextIsComment[lineIndex] = false;
			continue;
		}

		// Parse the line to get color blocks
		std::vector<CrystalLineParser::TEXTBLOCK> blocks(nLength + 1);
		int nActualItems = 0;
		pParser->ParseLine(nLine, blocks.data(), nActualItems);

		// Check if entire line is a comment
		bool hasNonComment = false;
		std::string lineText;
		int nCurrentPos = 0;

		for (int i = 0; i < nActualItems; i++)
		{
			int nBlockStart = blocks[i].m_nCharPos;
			int nBlockEnd = (i + 1 < nActualItems) ? blocks[i + 1].m_nCharPos : nLength;
			int nColorIndex = blocks[i].m_nColorIndex;

			// Skip comment blocks
			if (nColorIndex == COLORINDEX_COMMENT)
			{
				nCurrentPos = nBlockEnd;
				continue;
			}

			// Found non-comment content
			hasNonComment = true;

			// Append non-comment text
			if (nBlockEnd > nBlockStart)
			{
#ifdef _UNICODE
				// Convert wide char to UTF-8
				int nBlockLen = nBlockEnd - nBlockStart;
				int nUtf8Len = WideCharToMultiByte(CP_UTF8, 0, pszChars + nBlockStart, nBlockLen, nullptr, 0, nullptr, nullptr);
				if (nUtf8Len > 0)
				{
					std::vector<char> utf8Buffer(nUtf8Len);
					WideCharToMultiByte(CP_UTF8, 0, pszChars + nBlockStart, nBlockLen, utf8Buffer.data(), nUtf8Len, nullptr, nullptr);
					lineText.append(utf8Buffer.data(), nUtf8Len);
				}
#else
				lineText.append(pszChars + nBlockStart, nBlockEnd - nBlockStart);
#endif
			}

			nCurrentPos = nBlockEnd;
		}

		// Update allTextIsComment flag
		allTextIsComment[lineIndex] = (nActualItems > 0 && !hasNonComment);

		result += lineText;
		result += '\n';
	}

	return result;
}

/**
 * @brief Check if a specific position is within a comment.
 */
bool SyntaxParserHelper::IsCommentPosition(
	ISyntaxParser* pParser,
	ITextBuffer* pTextBuffer,
	int nLineIndex,
	int nCharPos)
{
	if (pParser == nullptr || pTextBuffer == nullptr)
		return false;

	if (nLineIndex < 0 || nLineIndex >= pTextBuffer->GetLineCount())
		return false;

	int nLength = pTextBuffer->GetLineLength(nLineIndex);
	if (nCharPos < 0 || nCharPos >= nLength)
		return false;

	// Parse the line to get color blocks
	std::vector<CrystalLineParser::TEXTBLOCK> blocks(nLength + 1);
	int nActualItems = 0;
	pParser->ParseLine(nLineIndex, blocks.data(), nActualItems);

	// Find the block containing nCharPos
	for (int i = 0; i < nActualItems; i++)
	{
		int nBlockStart = blocks[i].m_nCharPos;
		int nBlockEnd = (i + 1 < nActualItems) ? blocks[i + 1].m_nCharPos : nLength;

		if (nCharPos >= nBlockStart && nCharPos < nBlockEnd)
		{
			return (blocks[i].m_nColorIndex == COLORINDEX_COMMENT);
		}
	}

	return false;
}
