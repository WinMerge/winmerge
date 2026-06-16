#include "pch.h"
#include "SyntaxParserHelper.h"
#include "unicoder.h"
#include "SyntaxColors.h"
#include "utils/ctchar.h"
#include <vector>

/**
 * @brief Get text with comments filtered out and per-line comment status.
 */
std::string SyntaxParserHelper::GetCommentsFilteredText(
	LangServices::ISyntaxParser* pParser,
	int nStartLine,
	int nEndLine,
	std::vector<bool>& allTextIsComment)
{
	if (pParser == nullptr || !pParser->GetTextBuffer() || nEndLine < nStartLine)
	{
		allTextIsComment.clear();
		return std::string();
	}

	auto* pTextBuffer = pParser->GetTextBuffer();

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
		int nFullLength = pTextBuffer->GetFullLineLength(nLine);

		// Parse the line to get color blocks
		std::vector<LangServices::TEXTBLOCK> blocks = pParser->ParseLine(nLine);
		if (blocks.empty())
		{
			allTextIsComment[lineIndex] = false;
			result.append(ucr::toUTF8(pszChars, nFullLength));
			continue;
		}
		
		// Check if entire line is a comment
		bool hasNonComment = false;
		std::string lineText;
		int nActualItems = static_cast<int>(blocks.size());

		for (int i = 0; i < nActualItems; i++)
		{
			int nBlockStart = blocks[i].m_nCharPos;
			int nBlockEnd = (i + 1 < nActualItems) ? blocks[i + 1].m_nCharPos : nFullLength;
			int nColorIndex = blocks[i].m_nColorIndex;

			// Skip comment blocks
			if (nColorIndex == COLORINDEX_COMMENT)
				continue;

			if (nBlockStart < 0 || nBlockStart >= nFullLength)
				continue;

			tchar_t c = pszChars[nBlockStart];
			if (c != '\r' && c != '\n')
			{
				// Found non-comment content
				hasNonComment = true;
			}

			// Append non-comment text
			if (nBlockEnd > nBlockStart)
				lineText.append(ucr::toUTF8(pszChars + nBlockStart, nBlockEnd - nBlockStart));
		}

		// Update allTextIsComment flag
		allTextIsComment[lineIndex] = (nActualItems > 0 && !hasNonComment);

		result += lineText;
		if (nActualItems > 0 && blocks[nActualItems - 1].m_nColorIndex == COLORINDEX_COMMENT)
		{
			// If there is an inline comment, the EOL for that line will be deleted, so add the EOL.
			int nLength = pTextBuffer->GetLineLength(nLine);
			result += ucr::toUTF8(pszChars + nLength, nFullLength - nLength);
		}
	}

	return result;
}
