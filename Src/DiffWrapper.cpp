// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  DiffWrapper.cpp
 *
 * @brief Code for DiffWrapper class
 *
 * @date  Created: 2003-08-22
 */

#include "pch.h"
#define NOMINMAX
#include "DiffWrapper.h"
#include <tuple>
#include <exception>
#include <array>
#include <Poco/Exception.h>
#include "coretools.h"
#include "DiffList.h"
#include "MovedLines.h"
#include "FilterList.h"
#include "diff.h"
#include "Diff3.h"
#include "xdiff_gnudiff_compat.h"
#include "FileTransform.h"
#include "paths.h"
#include "CompareOptions.h"
#include "FileTextStats.h"
#include "DiffFileData.h"
#include "Environment.h"
#include "PatchHTML.h"
#include "UnicodeString.h"
#include "unicoder.h"
#include "TFile.h"
#include "Exceptions.h"
#include "parsers/crystallineparser.h"
#include "SyntaxColors.h"
#include "Logger.h"
#include "MergeApp.h"
#include "SubstitutionList.h"
#include "codepage_detect.h"
#include "cio.h"

using Poco::Exception;

extern int recursive;

extern "C" int is_blank_line(char const* pch, char const* limit);

static void CopyTextStats(const file_data * inf, FileTextStats * myTextStats);
static void CopyDiffutilTextStats(file_data *inf, DiffFileData * diffData);

constexpr char* FILTERED_LINE = "!" "c0d5089f" "-" "3d91" "-" "4d69" "-" "b406" "-" "dc5a5b51a4f8";

/**
 * @brief Default constructor.
 * Initializes members.
 */
CDiffWrapper::CDiffWrapper()
: m_pFilterCommentsDef(nullptr)
, m_bCreatePatchFile(false)
, m_bUseDiffList(false)
, m_bAddCmdLine(true)
, m_bAppendFiles(false)
, m_nDiffs(0)
, m_infoPrediffer(nullptr)
, m_pDiffList(nullptr)
, m_bPathsAreTemp(false)
, m_pFilterList(nullptr)
, m_pSubstitutionList{nullptr}
, m_bPluginsEnabled(false)
, m_status()
, m_codepage(ucr::CP_UTF_8)
, m_xdlFlags(0)
{
	// character that ends a line.  Currently this is always `\n'
	line_end_char = '\n';
}

/**
 * @brief Destructor.
 */
CDiffWrapper::~CDiffWrapper() = default;

/**
 * @brief Enables/disables patch-file creation and sets filename.
 * This function enables or disables patch file creation. When
 * @p filename is empty, patch files are disabled.
 * @param [in] filename Filename for patch file, or empty string.
 */
void CDiffWrapper::SetCreatePatchFile(const String &filename)
{
	if (filename.empty())
	{
		m_bCreatePatchFile = false;
		m_sPatchFile.clear();
	}
	else
	{
		m_bCreatePatchFile = true;
		m_sPatchFile = filename;
		strutils::replace(m_sPatchFile, _T("/"), _T("\\"));
	}
}

/**
 * @brief Enables/disabled DiffList creation ands sets DiffList.
 * This function enables or disables DiffList creation. When
 * @p diffList is `nullptr`, a difflist was not created. When valid 
 * DiffList pointer is given, compare results are stored into it.
 * @param [in] diffList Pointer to DiffList getting compare results.
 */
void CDiffWrapper::SetCreateDiffList(DiffList *diffList)
{
	if (diffList == nullptr)
	{
		m_bUseDiffList = false;
		m_pDiffList = nullptr;
	}
	else
	{
		m_bUseDiffList = true;
		m_pDiffList = diffList;
	}
}

/**
 * @brief Returns current set of options used by diff-engine.
 * This function converts internally used diff-options to
 * format used outside CDiffWrapper and returns them.
 * @param [in,out] options Pointer to structure getting used options.
 */
void CDiffWrapper::GetOptions(DIFFOPTIONS *options) const
{
	assert(options != nullptr);
	DIFFOPTIONS tmpOptions = {0};
	m_options.GetAsDiffOptions(tmpOptions);
	*options = tmpOptions;
}

/**
 * @brief Set options for Diff-engine.
 * This function converts given options to format CDiffWrapper uses
 * internally and stores them.
 * @param [in] options Pointer to structure having new options.
 */
void CDiffWrapper::SetOptions(const DIFFOPTIONS *options, bool setToDiffutils)
{
	assert(options != nullptr);
	m_options.SetFromDiffOptions(*options);
	m_xdlFlags = make_xdl_flags(m_options);
	if (setToDiffutils)
		m_options.SetToDiffUtils();
}

void CDiffWrapper::SetPrediffer(const PrediffingInfo * prediffer /*= nullptr*/)
{
	// all flags are set correctly during the construction
	m_infoPrediffer.reset(new PrediffingInfo);

	if (prediffer != nullptr)
		*m_infoPrediffer = *prediffer;
}

/**
 * @brief Set options used for patch-file creation.
 * @param [in] options Pointer to structure having new options.
 */
void CDiffWrapper::SetPatchOptions(const PATCHOPTIONS *options)
{
	assert(options != nullptr);
	m_options.m_contextLines = options->nContext;

	switch (options->outputStyle)
	{
	case OUTPUT_NORMAL:
		m_options.m_outputStyle = DIFF_OUTPUT_NORMAL;
		break;
	case OUTPUT_CONTEXT:
		m_options.m_outputStyle = DIFF_OUTPUT_CONTEXT;
		break;
	case OUTPUT_UNIFIED:
		m_options.m_outputStyle = DIFF_OUTPUT_UNIFIED;
		break;
	case OUTPUT_HTML:
		m_options.m_outputStyle = DIFF_OUTPUT_HTML;
		break;
	default:
		throw "Unknown output style!";
		break;
	}

	m_bAddCmdLine = options->bAddCommandline;
}

/**
 * @brief Enables/disables moved block detection.
 * @param [in] bDetectMovedBlocks If true moved blocks are detected.
 */
void CDiffWrapper::SetDetectMovedBlocks(bool bDetectMovedBlocks)
{
	if (bDetectMovedBlocks)
	{
		if (m_pMovedLines[0] == nullptr)
		{
			m_pMovedLines[0].reset(new MovedLines);
			m_pMovedLines[1].reset(new MovedLines);
			m_pMovedLines[2].reset(new MovedLines);
		}
	}
	else
	{
		m_pMovedLines[0].reset();
		m_pMovedLines[1].reset();
		m_pMovedLines[2].reset();
	}
}

static String convertToTString(const char* start, const char* end)
{
	if (!ucr::CheckForInvalidUtf8(start, end - start))
	{
		return ucr::toTString(std::string(start, end));
	}
	else
	{
		bool lossy = false;
		String text;
		ucr::maketstring(text, start, end - start, -1, &lossy);
		return text;
	}
}

static unsigned GetLastLineCookie(unsigned dwCookie, int startLine, int endLine, const char **linbuf, CrystalLineParser::TextDefinition* enuType)
{
	if (!enuType)
		return dwCookie;
	for (int i = startLine; i <= endLine; ++i)
	{
		String text = convertToTString(linbuf[i], linbuf[i + 1]);
		int nActualItems = 0;
		std::vector<CrystalLineParser::TEXTBLOCK> blocks(text.length());
		dwCookie = enuType->ParseLineX(dwCookie, text.c_str(), static_cast<int>(text.length()), blocks.data(), nActualItems);
	}
	return dwCookie;
}

static std::tuple<std::string, unsigned, std::vector<bool>> GetCommentsFilteredText(unsigned dwCookie, int startLine, int endLine, const char **linbuf, CrystalLineParser::TextDefinition* enuType)
{
	String filteredT;
	std::vector<bool> allTextIsComment(endLine - startLine + 1);
	for (int i = startLine; i <= endLine; ++i)
	{
		String text = convertToTString(linbuf[i], linbuf[i + 1]);
		unsigned textlen = static_cast<unsigned>(text.size());
		if (!enuType)
		{
			filteredT += text;
		}
		else
		{
			int nActualItems = 0;
			std::vector<CrystalLineParser::TEXTBLOCK> blocks(textlen);
			dwCookie = enuType->ParseLineX(dwCookie, text.c_str(), textlen, blocks.data(), nActualItems);

			if (nActualItems == 0)
			{
				filteredT += text;
			}
			else
			{
				allTextIsComment[i - startLine] =
					(nActualItems > 0 && blocks[0].m_nColorIndex == COLORINDEX_COMMENT);
				for (int j = 0; j < nActualItems; ++j)
				{
					CrystalLineParser::TEXTBLOCK& block = blocks[j];
					if (block.m_nColorIndex != COLORINDEX_COMMENT)
					{
						unsigned blocklen = (j < nActualItems - 1) ? (blocks[j + 1].m_nCharPos - block.m_nCharPos) : textlen - block.m_nCharPos;
						filteredT.append(text.c_str() + block.m_nCharPos, blocklen);
						tchar_t c = (blocklen == 0) ? 0 : *(text.c_str() + block.m_nCharPos);
						if (c != '\r' && c != '\n')
							allTextIsComment[i - startLine] = false;
					}
				}

				if (blocks[nActualItems - 1].m_nColorIndex == COLORINDEX_COMMENT)
				{
					// If there is an inline comment, the EOL for that line will be deleted, so add the EOL.
					size_t fullLen = linbuf[i + 1] - linbuf[i];
					size_t len = linelen(linbuf[i], fullLen);
					for (size_t j = len; j < fullLen; ++j)
						filteredT += linbuf[i][j];
				}
			}
		}
	}

	return { ucr::toUTF8(filteredT), dwCookie, allTextIsComment };
}

/**
 * @brief Replace a string inside a string with another string.
 * This function searches for a string inside another string an if found,
 * replaces it with another string. Function can replace several instances
 * of the string inside one string.
 * @param [in,out] target A string containing another string to replace.
 * @param [in] find A string to search and replace with another (@p replace).
 * @param [in] replace A string used to replace original (@p find).
 */
void Replace(std::string &target, const std::string &find, const std::string &replace)
{
	const std::string::size_type find_len = find.length();
	const std::string::size_type replace_len = replace.length();
	std::string::size_type pos = 0;
	while ((pos = target.find(find, pos)) != std::string::npos)
	{
		target.replace(pos, find_len, replace);
		pos += replace_len;
	}
}

/**
 * @brief Replace the characters that matche characters specified in its arguments
 * @param [in,out] str - A string containing another string to replace.
 * @param [in] chars - characters to search for
 * @param [in] rep - String to replace
 */
static void ReplaceChars(std::string & str, const char* chars, const char *rep)
{
	std::string::size_type pos = 0;
	size_t replen = strlen(rep);
	while ((pos = str.find_first_of(chars, pos)) != std::string::npos)
	{
		std::string::size_type posend = str.find_first_not_of(chars, pos);
		if (posend != String::npos)
			str.replace(pos, posend - pos, rep);
		else
			str.replace(pos, str.length() - pos, rep);
		pos += replen;
	}
}

/**
 * @brief Get the end-of-line (EOL) characters (LF, CR, or CRLF) from the end of a string.
 * @param [in] str - A string from which the EOL characters will be identified.
 */
static std::string GetEOL(const std::string& str)
{
	if (str.empty())
		return "";
	if (str.size() >= 2 && str[str.size() - 2] == '\r' && str[str.size() - 1] == '\n')
		return "\r\n";
	if (str.back() == '\r')
		return "\r";
	if (str.back() == '\n')
		return "\n";
	return "";
}

/**
 * @brief The main entry for post filtering.  Performs post-filtering, by setting comment blocks to trivial
 * @param [in, out]  thisob	Current change
 * @return Number of trivial diffs inserted
 */
int CDiffWrapper::PostFilter(PostFilterContext& ctxt, change* thisob, const file_data *file_data_ary) const
{
	const int first0 = thisob->line0;
	const int first1 = thisob->line1;
	const int last0 = first0 + thisob->deleted - 1;
	const int last1 = first1 + thisob->inserted - 1;
	int trans_a0 = 0, trans_b0 = 0, trans_a1 = 0, trans_b1 = 0;
	translate_range(&file_data_ary[0], first0, last0, &trans_a0, &trans_b0);
	translate_range(&file_data_ary[1], first1, last1, &trans_a1, &trans_b1);
	const int qtyLinesLeft = (trans_b0 - trans_a0) + 1; //Determine quantity of lines in this block for left side
	const int qtyLinesRight = (trans_b1 - trans_a1) + 1;//Determine quantity of lines in this block for right side
	const int lineNumberLeft = trans_a0 - 1;
	const int lineNumberRight = trans_a1 - 1;
	
	std::string lineDataLeft, lineDataRight;
	std::vector<bool> allTextIsCommentLeft(qtyLinesLeft), allTextIsCommentRight(qtyLinesRight);

	if (m_options.m_filterCommentsLines)
	{
		ctxt.dwCookieLeft = GetLastLineCookie(ctxt.dwCookieLeft,
			ctxt.nParsedLineEndLeft + 1, lineNumberLeft - 1, file_data_ary[0].linbuf + file_data_ary[0].linbuf_base, m_pFilterCommentsDef);
		ctxt.dwCookieRight = GetLastLineCookie(ctxt.dwCookieRight,
			ctxt.nParsedLineEndRight + 1, lineNumberRight - 1, file_data_ary[1].linbuf + file_data_ary[1].linbuf_base, m_pFilterCommentsDef);

		ctxt.nParsedLineEndLeft = lineNumberLeft + qtyLinesLeft - 1;
		ctxt.nParsedLineEndRight = lineNumberRight + qtyLinesRight - 1;;

		auto resultLeft = GetCommentsFilteredText(ctxt.dwCookieLeft,
			lineNumberLeft, ctxt.nParsedLineEndLeft, file_data_ary[0].linbuf + file_data_ary[0].linbuf_base, m_pFilterCommentsDef);
		lineDataLeft = std::move(std::get<0>(resultLeft));
		ctxt.dwCookieLeft = std::get<1>(resultLeft);
		allTextIsCommentLeft = std::move(std::get<2>(resultLeft));

		auto resultRight = GetCommentsFilteredText(ctxt.dwCookieRight,
			lineNumberRight, ctxt.nParsedLineEndRight, file_data_ary[1].linbuf + file_data_ary[1].linbuf_base, m_pFilterCommentsDef);
		lineDataRight = std::move(std::get<0>(resultRight));
		ctxt.dwCookieRight = std::get<1>(resultRight);
		allTextIsCommentRight = std::move(std::get<2>(resultRight));
	}
	else
	{
		lineDataLeft.assign(file_data_ary[0].linbuf[lineNumberLeft + file_data_ary[0].linbuf_base],
			file_data_ary[0].linbuf[lineNumberLeft + qtyLinesLeft + file_data_ary[0].linbuf_base]
			- file_data_ary[0].linbuf[lineNumberLeft + file_data_ary[0].linbuf_base]);
		lineDataRight.assign(file_data_ary[1].linbuf[lineNumberRight + file_data_ary[1].linbuf_base],
			file_data_ary[1].linbuf[lineNumberRight + qtyLinesRight + file_data_ary[1].linbuf_base]
			- file_data_ary[1].linbuf[lineNumberRight + file_data_ary[1].linbuf_base]);
	}

	if (m_pFilterList != nullptr && m_pFilterList->HasRegExps())
	{
		// Match lines against regular expression filters
		// Our strategy is that every line in both sides must
		// match regexp before we mark difference as ignored.
		bool match1 = RegExpFilter(lineDataLeft);
		bool match2 = RegExpFilter(lineDataRight);
		if (match1 && match2)
		{
			thisob->trivial = 1;
			return 0;
		}
	}

	if (m_pSubstitutionList)
	{
		lineDataLeft = m_pSubstitutionList->Subst(lineDataLeft, m_codepage);
		lineDataRight = m_pSubstitutionList->Subst(lineDataRight, m_codepage);
	}

	if (m_options.m_ignoreWhitespace == WHITESPACE_IGNORE_ALL)
	{
		//Ignore character case
		ReplaceChars(lineDataLeft, " \t", "");
		ReplaceChars(lineDataRight, " \t", "");
	}
	else if (m_options.m_ignoreWhitespace == WHITESPACE_IGNORE_CHANGE)
	{
		//Ignore change in whitespace char count
		ReplaceChars(lineDataLeft, " \t", " ");
		ReplaceChars(lineDataRight, " \t", " ");
	}

	if (m_options.m_bIgnoreNumbers)
	{
		//Ignore number character case
		ReplaceChars(lineDataLeft, "0123456789", "");
		ReplaceChars(lineDataRight, "0123456789", "");
	}
	if (m_options.m_bIgnoreCase)
	{
		//ignore case
		for (std::string::iterator pb = lineDataLeft.begin(), pe = lineDataLeft.end(); pb != pe; ++pb) 
			*pb = static_cast<char>(::toupper(*pb));
		for (std::string::iterator pb = lineDataRight.begin(), pe = lineDataRight.end(); pb != pe; ++pb) 
			*pb = static_cast<char>(::toupper(*pb));
	}
	if (m_options.m_bIgnoreEOLDifference)
	{
		Replace(lineDataLeft, "\r\n", "\n");
		Replace(lineDataLeft, "\r", "\n");
		Replace(lineDataRight, "\r\n", "\n");
		Replace(lineDataRight, "\r", "\n");
	}
	if (thisob->link == nullptr && m_options.m_bIgnoreMissingTrailingEol && (file_data_ary[0].missing_newline != file_data_ary[1].missing_newline))
	{
		if (file_data_ary[0].missing_newline && !file_data_ary[1].missing_newline)
			lineDataLeft += GetEOL(lineDataRight);
		else if (!file_data_ary[0].missing_newline && file_data_ary[1].missing_newline)
			lineDataRight += GetEOL(lineDataLeft);
	}

	// If both match after filtering, mark this diff hunk as trivial and return.
	if (lineDataLeft == lineDataRight)
	{
		//only difference is trivial
		thisob->trivial = 1;
		return 0;
	}

	auto SplitLines = [](const std::string& lines, int nlines) -> std::vector<std::string_view>
		{
			std::vector<std::string_view> result;
			const char* line = lines.c_str();
			for (size_t i = 0; i < lines.length(); ++i)
			{
				char c = lines[i];
				if (c == '\r')
				{
					if (i + 1 < lines.length() && lines[i + 1] == '\n')
						i++;
					result.emplace_back(line, lines.c_str() + i + 1 - line);
					line = lines.c_str() + i + 1;
				}
				else if (c == '\n')
				{
					result.emplace_back(line, lines.c_str() + i + 1 - line);
					line = lines.c_str() + i + 1;
				}
			}
			if (!lines.empty() && (lines.back() != '\r' && lines.back() != '\n'))
				result.emplace_back(line, lines.c_str() + lines.length() - line);
			if (result.size() < nlines)
				result.emplace_back("", 0);
			return result; 
		};

	std::vector<std::string_view> leftLines = SplitLines(lineDataLeft, qtyLinesLeft);
	std::vector<std::string_view> rightLines = SplitLines(lineDataRight, qtyLinesRight);

	if (qtyLinesLeft != leftLines.size() || qtyLinesRight != rightLines.size())
		return 0;

	// If both do not match as a result of filtering, some lines may match,
	// so diff calculation is performed again using the filtered lines.
	change* script = diff_2_buffers_xdiff(
		lineDataLeft.c_str(), lineDataLeft.length(),
		lineDataRight.c_str(), lineDataRight.length(), m_xdlFlags);
	if (!script)
		return 0;

	auto TranslateLineNumbers = [](change* thisob, change* script)
		{
			assert(thisob && script);
			for (change* cur = script; cur; cur = cur->link)
			{
				cur->line0 += thisob->line0;
				cur->line1 += thisob->line1;
			}
		};

	// Insert lines with no differences as trivial diffs after filtering
	auto InsertTrivialChanges = [](change* thisob, change* script) -> int
		{
			assert(thisob && script);
			int l0 = thisob->line0;
			int l1 = thisob->line1;
			change* first = script;
			change* prev = nullptr;
			int nTrivialInserts = 0;
			for (change* cur = script; cur; cur = cur->link)
			{
				if (l0 < cur->line0 || l1 < cur->line1)
				{
					nTrivialInserts++;
					change *newob = (change *)xmalloc(sizeof (change));
					newob->line0 = l0;
					newob->line1 = l1;
					newob->deleted = cur->line0 - l0;
					newob->inserted = cur->line1 - l1;
					newob->trivial = 1;
					newob->match0 = -1;
					newob->match1 = -1;
					if (cur == first)
					{
						std::swap(newob->line0, cur->line0);
						std::swap(newob->line1, cur->line1);
						std::swap(newob->deleted, cur->deleted);
						std::swap(newob->inserted, cur->inserted);
						std::swap(newob->trivial, cur->trivial);
						std::swap(newob->match0, cur->match0);
						std::swap(newob->match1, cur->match1);
						newob->link = cur->link;
						cur->link = newob;
					}
					else
					{
						prev->link = newob;
						newob->link = cur;
					}
				}
				l0 = cur->line0 + cur->deleted;
				l1 = cur->line1 + cur->inserted;
				prev = cur;
			}
			if (l0 < thisob->line0 + thisob->deleted || l1 < thisob->line1 + thisob->inserted)
			{
				nTrivialInserts++;
				change *newob = (change *)xmalloc(sizeof (change));
				prev->link = newob;
				newob->line0 = l0;
				newob->line1 = l1;
				newob->deleted = thisob->line0 + thisob->deleted - l0;
				newob->inserted = thisob->line1 + thisob->inserted - l1;
				newob->trivial = 1;
				newob->match0 = -1;
				newob->match1 = -1;
				newob->link = nullptr;
			}
			return nTrivialInserts;
		};

	// Insert blank lines or filtered lines that are only on one side as trivial diffs. 
	auto InsertTrivialChanges2 =
		[](change* thisob, change* script, bool ignoreBlankLines,
		   const std::vector<std::string_view>& leftLines,
		   const std::vector<std::string_view>& rightLines,
		   const std::vector<bool>& linesFilteredLeft,
		   const std::vector<bool>& linesFilteredRight) -> int
		{
			assert(thisob && script);
			auto IsBlankLine = [](const std::string_view& line)
				{
					for (char c : line)
					{
						if (!std::isspace(static_cast<unsigned char>(c)))
							return false;
					}
					return true;
				};
			int nTrivialInserts = 0;
			for (change* cur = script; cur; cur = cur->link)
			{
				if (!cur->trivial && cur->deleted != cur->inserted)
				{
					bool ignorable = true;
					if (cur->deleted > cur->inserted)
					{
						for (int i = cur->line0 + cur->inserted - thisob->line0; i < cur->line0 + cur->deleted - thisob->line0; ++i)
						{
							if (!(ignoreBlankLines && IsBlankLine(leftLines[i])) && !linesFilteredLeft[i] && leftLines[i] != FILTERED_LINE)
								ignorable = false;
						}
						if (ignorable)
						{
							if (cur->inserted == 0)
							{
								cur->trivial = 1;
							}
							else
							{
								nTrivialInserts++;
								change* newob = (change*)xmalloc(sizeof(change));
								newob->line0 = cur->line0 + cur->inserted;
								newob->line1 = cur->line1 + cur->inserted;
								newob->deleted = cur->deleted - cur->inserted;
								newob->inserted = 0;
								newob->trivial = 1;
								newob->match0 = -1;
								newob->match1 = -1;
								newob->link = cur->link;
								cur->link = newob;
								cur->deleted = cur->inserted;
							}
						}
					}
					else
					{
						for (int i = cur->line1 + cur->deleted - thisob->line1; i < cur->line1 + cur->inserted - thisob->line1; ++i)
						{
							if (!(ignoreBlankLines && IsBlankLine(rightLines[i])) && !linesFilteredRight[i] && rightLines[i] != FILTERED_LINE)
								ignorable = false;
						}
						if (ignorable)
						{
							if (cur->deleted == 0)
							{
								cur->trivial = 1;
							}
							else
							{
								nTrivialInserts++;
								change* newob = (change*)xmalloc(sizeof(change));
								newob->line0 = cur->line0 + cur->deleted;
								newob->line1 = cur->line1 + cur->deleted;
								newob->deleted = 0;
								newob->inserted = cur->inserted - cur->deleted;
								newob->trivial = 1;
								newob->match0 = -1;
								newob->match1 = -1;
								newob->link = cur->link;
								cur->link = newob;
								cur->inserted = cur->deleted;
							}
						}
					}
				}
			}
			return nTrivialInserts;
		};

	auto ReplaceChanges = [](change* thisob, change* script)
		{
			assert(thisob && script);
			change* last = script;
			for (change* cur = script; cur; cur = cur->link)
				last = cur;
			last->link = thisob->link;
			thisob->link = script->link;
			thisob->line0 = script->line0;
			thisob->line1 = script->line1;
			thisob->deleted = script->deleted;
			thisob->inserted = script->inserted;
			thisob->trivial = script->trivial;
			thisob->match0 = script->match0;
			thisob->match1 = script->match1;
			free(script);
		};

	TranslateLineNumbers(thisob, script);
	int nTrivialInserts = InsertTrivialChanges(thisob, script);
	nTrivialInserts += InsertTrivialChanges2(thisob, script, m_options.m_bIgnoreBlankLines, leftLines, rightLines, allTextIsCommentLeft, allTextIsCommentRight);
	ReplaceChanges(thisob, script);
	return nTrivialInserts;
}

/**
 * @brief Set source paths for diffing two files.
 * Sets full paths to two files we are diffing. Paths can be actual user files
 * or temporary copies of user files. Parameter @p tempPaths tells if paths
 * are temporary paths that can be deleted.
 * @param [in] files Files to compare
 * @param [in] tempPaths Are given paths temporary (can be deleted)?.
 */
void CDiffWrapper::SetPaths(const PathContext &tFiles,
		bool tempPaths)
{
	m_files = tFiles;
	m_bPathsAreTemp = tempPaths;
}

/**
 * @brief Runs diff-engine.
 */
bool CDiffWrapper::RunFileDiff()
{
	PathContext aFiles = m_files;
	int file;
	for (file = 0; file < m_files.GetSize(); file++)
		aFiles[file] = paths::ToWindowsPath(aFiles[file]);

	bool bRet = true;
	String strFileTemp[3];
	std::copy(m_files.begin(), m_files.end(), strFileTemp);
	
	m_options.SetToDiffUtils();

	if (m_bUseDiffList)
		m_nDiffs = m_pDiffList->GetSize();

	for (file = 0; file < aFiles.GetSize(); file++)
	{
		if (m_bPluginsEnabled)
		{
			// Do the preprocessing now, overwrite the temp files
			// NOTE: FileTransform_UCS2ToUTF8() may create new temp
			// files and return new names, those created temp files
			// are deleted in end of function.

			// this can only fail if the data can not be saved back (no more
			// place on disk ???) What to do then ??
			if (m_infoPrediffer && !m_infoPrediffer->Prediffing(file, strFileTemp[file], m_sToFindPrediffer, m_bPathsAreTemp, { strFileTemp[file] }))
			{
				// display a message box
				String sError = strutils::format_string2(
					_("Error prediffing '%1' with '%2'. Prediffing disabled."),
					strFileTemp[file].c_str(),
					m_infoPrediffer->GetPluginPipeline().c_str());
				AppErrorMessageBox(sError);
				// don't use any more this prediffer
				m_infoPrediffer->ClearPluginPipeline();
			}
		}
	}

	struct change *script = nullptr;
	struct change *script10 = nullptr;
	struct change *script12 = nullptr;
	struct change *script02 = nullptr;
	DiffFileData diffdata, diffdata10, diffdata12, diffdata02;
	int bin_flag = 0, bin_flag10 = 0, bin_flag12 = 0, bin_flag02 = 0;

	if (aFiles.GetSize() == 2)
	{
		diffdata.SetDisplayFilepaths(aFiles[0], aFiles[1]); // store true names for diff utils patch file
		// This opens & fstats both files (if it succeeds)
		if (!diffdata.OpenFiles(strFileTemp[0], strFileTemp[1]))
		{
			return false;
		}

		// Compare the files, if no error was found.
		// Last param (bin_file) is `nullptr` since we don't
		// (yet) need info about binary sides.
		bRet = Diff2Files(&script, &diffdata, &bin_flag, nullptr);

		// We don't anymore create diff-files for every rescan.
		// User can create patch-file whenever one wants to.
		// We don't need to waste time. But lets keep this as
		// debugging aid. Sometimes it is very useful to see
		// what differences diff-engine sees!
#ifdef _DEBUG
		// throw the diff into a temp file
		String sTempPath = env::GetTemporaryPath(); // get path to Temp folder
		String path = paths::ConcatPath(sTempPath, _T("Diff.txt"));

		if (cio::tfopen_s(&outfile, path, _T("w+")) == 0)
		{
			print_normal_script(script);
			fclose(outfile);
			outfile = nullptr;
		}
#endif
	}
	else
	{
		diffdata10.SetDisplayFilepaths(aFiles[1], aFiles[0]); // store true names for diff utils patch file
		diffdata12.SetDisplayFilepaths(aFiles[1], aFiles[2]); // store true names for diff utils patch file
		diffdata02.SetDisplayFilepaths(aFiles[0], aFiles[2]); // store true names for diff utils patch file

		if (!diffdata10.OpenFiles(strFileTemp[1], strFileTemp[0]))
		{
			return false;
		}

		bRet = Diff2Files(&script10, &diffdata10, &bin_flag10, nullptr);

		if (!diffdata12.OpenFiles(strFileTemp[1], strFileTemp[2]))
		{
			return false;
		}

		bRet = Diff2Files(&script12, &diffdata12, &bin_flag12, nullptr);

		if (!diffdata02.OpenFiles(strFileTemp[0], strFileTemp[2]))
		{
			return false;
		}

		bRet = Diff2Files(&script02, &diffdata02, &bin_flag02, nullptr);
	}

	// First determine what happened during comparison
	// If there were errors or files were binaries, don't bother
	// creating diff-lists or patches
	
	// diff_2_files set bin_flag to -1 if different binary
	// diff_2_files set bin_flag to +1 if same binary

	file_data * inf = diffdata.m_inf;
	file_data * inf10 = diffdata10.m_inf;
	file_data * inf12 = diffdata12.m_inf;
	file_data * inf02 = diffdata02.m_inf;

	if (aFiles.GetSize() == 2)
	{
		if (bin_flag != 0)
			m_status.bBinaries = true;
	}
	else
	{
		m_status.bBinaries = (bin_flag10 != 0 || bin_flag12 != 0);
	}

	// Create patch file
	if (!m_status.bBinaries && m_bCreatePatchFile && aFiles.GetSize() == 2)
	{
		WritePatchFile(script, &inf[0]);
	}
	
	// Go through diffs adding them to WinMerge's diff list
	// This is done on every WinMerge's doc rescan!
	if (!m_status.bBinaries && m_bUseDiffList)
	{
		if (aFiles.GetSize() == 2)
			LoadWinMergeDiffsFromDiffUtilsScript(script, diffdata.m_inf);
		else
			LoadWinMergeDiffsFromDiffUtilsScript3(
				script10, script12, script02,
				diffdata10.m_inf, diffdata12.m_inf, diffdata02.m_inf);
	}			

	if (aFiles.GetSize() == 2)
	{
		if (bin_flag != 0)
		{
			if (bin_flag != -1)
				m_status.Identical = IDENTLEVEL::ALL;
			else
				m_status.Identical = IDENTLEVEL::NONE;
		}
		else
		{ // text files according to diffutils, so change script exists
			m_status.Identical = (script == 0) ? IDENTLEVEL::ALL : IDENTLEVEL::NONE;
		}
		m_status.bMissingNL[0] = !!inf[0].missing_newline;
		m_status.bMissingNL[1] = !!inf[1].missing_newline;
	}
	else
	{
		m_status.Identical = IDENTLEVEL::NONE;
		if (bin_flag10 != 0 || bin_flag12 != 0)
		{
			if (bin_flag10 != -1 && bin_flag12 != -1)
				m_status.Identical = IDENTLEVEL::ALL;
			else if (bin_flag10 != -1)
				m_status.Identical = IDENTLEVEL::EXCEPTRIGHT;
			else if (bin_flag12 != -1)
				m_status.Identical = IDENTLEVEL::EXCEPTLEFT;
			else if (bin_flag12 != -1)
				m_status.Identical = IDENTLEVEL::EXCEPTMIDDLE;
		}
		else
		{ // text files according to diffutils, so change script exists
			if (IsIdenticalOrIgnorable(script10) && IsIdenticalOrIgnorable(script12))
				m_status.Identical = IDENTLEVEL::ALL;
			else if (IsIdenticalOrIgnorable(script10))
				m_status.Identical = IDENTLEVEL::EXCEPTRIGHT;
			else if (IsIdenticalOrIgnorable(script12))
				m_status.Identical = IDENTLEVEL::EXCEPTLEFT;
			else if (IsIdenticalOrIgnorable(script02))
				m_status.Identical = IDENTLEVEL::EXCEPTMIDDLE;
		}
		m_status.bMissingNL[0] = !!inf10[1].missing_newline;
		m_status.bMissingNL[1] = !!inf12[0].missing_newline;
		m_status.bMissingNL[2] = !!inf02[1].missing_newline;
	}

	// cleanup the script
	if (aFiles.GetSize() == 2)
		FreeDiffUtilsScript(script);
	else
	{
		FreeDiffUtilsScript(script10);
		FreeDiffUtilsScript(script12);
		FreeDiffUtilsScript(script02);
	}

	// Done with diffutils filedata
	if (aFiles.GetSize() == 2)
	{
		diffdata.Close();
	}
	else
	{
		diffdata10.Close();
		diffdata12.Close();
		diffdata02.Close();
	}

	if (m_bPluginsEnabled)
	{
		// Delete temp files transformation functions possibly created
		for (file = 0; file < aFiles.GetSize(); file++)
		{
			if (strutils::compare_nocase(aFiles[file], strFileTemp[file]) != 0)
			{
				try
				{
					TFile(strFileTemp[file]).remove();
				}
				catch (Exception& e)
				{
					RootLogger::Error(e.displayText());
				}
				strFileTemp[file].erase();
			}
		}
	}
	return bRet;
}

/**
 * @brief Add diff to external diff-list
 */
void CDiffWrapper::AddDiffRange(DiffList *pDiffList, unsigned begin0, unsigned end0, unsigned begin1, unsigned end1, OP_TYPE op)
{
	try
	{
		DIFFRANGE dr;
		dr.begin[0] = begin0;
		dr.end[0] = end0;
		dr.begin[1] = begin1;
		dr.end[1] = end1;
		dr.begin[2] = -1;
		dr.end[2] = -1;
		dr.op = op;
		dr.blank[0] = dr.blank[1] = dr.blank[2] = -1;
		pDiffList->AddDiff(dr);
	}
	catch (std::exception& e)
	{
		RootLogger::Error(e.what());
	}
}

void CDiffWrapper::AddDiffRange(DiffList *pDiffList, DIFFRANGE &dr)
{
	try
	{
		pDiffList->AddDiff(dr);
	}
	catch (std::exception& e)
	{
		RootLogger::Error(e.what());
	}
}

/**
 * @brief Expand last DIFFRANGE of file by one line to contain last line after EOL.
 * @param [in] leftBufferLines size of array pane left
 * @param [in] rightBufferLines size of array pane right
 * @param [in] left on which side we have to insert
 * @param [in] bIgnoreBlankLines, if true we always add a new diff and mark as trivial
 */
void CDiffWrapper::FixLastDiffRange(int nFiles, int bufferLines[], bool bMissingNL[], bool bIgnoreBlankLines)
{
	DIFFRANGE dr;
	const int count = m_pDiffList->GetSize();
	if (count > 0)
	{
		m_pDiffList->GetDiff(count - 1, dr);

		for (int file = 0; file < nFiles; file++)
		{
			if (!bMissingNL[file])
				dr.end[file]++;
		}

		m_pDiffList->SetDiff(count - 1, dr);
	}
	else 
	{
		// we have to create the DIFF
		for (int file = 0; file < nFiles; file++)
		{
			dr.end[file] = bufferLines[file] - 1;
			if (bMissingNL[file])
				dr.begin[file] = dr.end[file];
			else
				dr.begin[file] = dr.end[file] + 1;
			dr.op = OP_DIFF;
			assert(dr.begin[0] == dr.begin[file]);
		}
		if (bIgnoreBlankLines)
			dr.op = OP_TRIVIAL;

		AddDiffRange(m_pDiffList, dr); 
	}
}

/**
 * @brief Returns status-data from diff-engine last run
 */
void CDiffWrapper::GetDiffStatus(DIFFSTATUS *status) const
{
	std::memcpy(status, &m_status, sizeof(DIFFSTATUS));
}

/**
 * @brief Formats command-line for diff-engine last run (like it was called from command-line)
 */
String CDiffWrapper::FormatSwitchString() const
{
	String switches;
	
	switch (m_options.m_outputStyle)
	{
	case DIFF_OUTPUT_NORMAL:
		switches = _T(" ");
		break;
	case DIFF_OUTPUT_CONTEXT:
		switches = (m_options.m_contextLines > 0) ? _T(" -C ") : _T(" -c");
		break;
	case DIFF_OUTPUT_UNIFIED:
		switches = (m_options.m_contextLines > 0) ? _T(" -U ") : _T(" -u");
		break;
#if 0
	case DIFF_OUTPUT_ED:
		switches = _T(" e");
		break;
	case DIFF_OUTPUT_FORWARD_ED:
		switches = _T(" f");
		break;
	case DIFF_OUTPUT_RCS:
		switches = _T(" n");
		break;
	case DIFF_OUTPUT_IFDEF:
		switches = _T(" D");
		break;
	case DIFF_OUTPUT_SDIFF:
		switches = _T(" y");
		break;
#endif
	}

	if ((m_options.m_outputStyle == DIFF_OUTPUT_CONTEXT || m_options.m_outputStyle == DIFF_OUTPUT_UNIFIED) &&
		m_options.m_contextLines > 0)
		switches += strutils::to_str(m_options.m_contextLines);

	if (ignore_all_space_flag > 0)
		switches += _T(" -w");

	if (ignore_blank_lines_flag > 0)
		switches += _T(" -B");

	if (ignore_case_flag > 0)
		switches += _T(" -i");

	if (ignore_space_change_flag > 0)
		switches += _T(" -b");

	return switches;
}

/**
 * @brief Enables/disables patch-file appending.
 * If the file for patch already exists then the patch will be appended to
 * existing file.
 * @param [in] bAppendFiles If true patch will be appended to existing file.
 */
void CDiffWrapper::SetAppendFiles(bool bAppendFiles)
{
	m_bAppendFiles = bAppendFiles;
}

/**
 * @brief Compare two files using diffutils.
 *
 * Compare two files (in DiffFileData param) using diffutils. Run diffutils
 * inside SEH so we can trap possible error and exceptions. If error or
 * exception is trapped, return compare failure.
 * @param [out] diffs Pointer to list of change structs where diffdata is stored.
 * @param [in] diffData files to compare.
 * @param [out] bin_status used to return binary status from compare.
 * @param [out] bin_file Returns which file was binary file as bitmap.
    So if first file is binary, first bit is set etc. Can be `nullptr` if binary file
    info is not needed (faster compare since diffutils don't bother checking
    second file if first is binary).
 * @return true when compare succeeds, false if error happened during compare.
 * @note This function is used in file compare, not folder compare. Similar
 * folder compare function is in DiffFileData.cpp.
 */
bool CDiffWrapper::Diff2Files(struct change ** diffs, DiffFileData *diffData,
	int * bin_status, int * bin_file) const
{
	bool bRet = true;
	SE_Handler seh;
	try
	{
		if (m_options.m_diffAlgorithm != DIFF_ALGORITHM_DEFAULT)
		{
			const unsigned xdl_flags = make_xdl_flags(m_options);
			*diffs = diff_2_files_xdiff(diffData->m_inf, bin_status,
				(m_pMovedLines[0] != nullptr), bin_file, xdl_flags);
			files[0] = diffData->m_inf[0];
			files[1] = diffData->m_inf[1];
		}
		else
		{
			// Diff files. depth is zero because we are not comparing dirs
			*diffs = diff_2_files(diffData->m_inf, 0, bin_status,
				(m_pMovedLines[0] != nullptr), bin_file);
		}
		CopyDiffutilTextStats(diffData->m_inf, diffData);
	}
	catch (SE_Exception&)
	{
		*diffs = nullptr;
		bRet = false;
	}
	return bRet;
}

bool CDiffWrapper::IsIdenticalOrIgnorable(struct change* script)
{
	bool diff = false;
	struct change *e=nullptr, *p=nullptr;
	for (e = script; e != nullptr; e = p)
	{
		if (!e->trivial)
			diff = true;
		p = e->link;
	}
	return !diff;
}

/**
 * @brief Free script (the diffutils linked list of differences)
 */
void
CDiffWrapper::FreeDiffUtilsScript(struct change * & script)
{
	if (script == nullptr) return;
	struct change *e=nullptr, *p=nullptr;
	// cleanup the script
	for (e = script; e != nullptr; e = p)
	{
		p = e->link;
		free(e);
	}
	script = nullptr;
}

/**
 * @brief Match regular expression list against given difference.
 * This function matches the regular expression list against the difference
 * (given as start line and end line). Matching the diff requires that all
 * lines in difference match.
 * @param [in] StartPos First line of the difference.
 * @param [in] endPos Last line of the difference.
 * @param [in] FileNo File to match.
 * return true if any of the expressions matches.
 */
bool CDiffWrapper::RegExpFilter(std::string& lines) const
{
	if (m_pFilterList == nullptr)
	{	
		throw "CDiffWrapper::RegExpFilter() called when "
			"filterlist doesn't exist (=nullptr)";
	}

	bool linesMatch = true; // set to false when non-matching line is found.

	std::string replaced;
	replaced.reserve(lines.length());
	size_t pos = 0;
	while (pos < lines.length())
	{
		const char* string = lines.c_str() + pos;
		while (pos < lines.length() && (lines[pos] != '\r' && lines[pos] != '\n'))
			pos++;
		size_t stringlen = lines.c_str() + pos - string;
		std::string line = std::string(string, stringlen);
		if (!m_pFilterList->Match(line, m_codepage))
		{
			linesMatch = false;
			replaced += line;
		}
		else
		{
			replaced += FILTERED_LINE;
		}
		std::string eol;
		while (pos < lines.length() && (lines[pos] == '\r' || lines[pos] == '\n'))
			eol += lines[pos++];
		replaced += eol;
	}
	lines = replaced;
	return linesMatch;
}

/**
 * @brief Walk the diff utils change script, building the WinMerge list of diff blocks
 */
void
CDiffWrapper::LoadWinMergeDiffsFromDiffUtilsScript(struct change * script, const file_data * file_data_ary)
{
	//Logic needed for Ignore comment option
	PostFilterContext ctxt;

	struct change *next = script;

	const bool usefilters = m_options.m_filterCommentsLines ||
		m_options.m_bIgnoreMissingTrailingEol ||
		(m_pFilterList && m_pFilterList->HasRegExps()) ||
		(m_pSubstitutionList && m_pSubstitutionList->HasRegExps());
	
	while (next != nullptr)
	{
		/* Find a set of changes that belong together.  */
		struct change *thisob = next;
		struct change *end = find_change(next);
		
		/* Disconnect them from the rest of the changes,
		making them a hunk, and remember the rest for next iteration.  */
		next = end->link;
		end->link = nullptr;
#ifdef DEBUG
		debug_script(thisob);
#endif

		/* Print thisob hunk.  */
		//(*printfun) (thisob);
		{					
			/* Determine range of line numbers involved in each file.  */
			int first0=0, last0=0, first1=0, last1=0, deletes=0, inserts=0;
			analyze_hunk (thisob, &first0, &last0, &first1, &last1, &deletes, &inserts, file_data_ary);
		
			/* Reconnect the script so it will all be freed properly.  */
			end->link = next;

			if (deletes || inserts || thisob->trivial)
			{
				OP_TYPE op = OP_NONE;
				if (deletes && inserts)
					op = OP_DIFF;
				else if (deletes || inserts)
					op = OP_DIFF;
				else
					op = OP_TRIVIAL;
				
				/* Print the lines that the first file has.  */
				int trans_a0=0, trans_b0=0, trans_a1=0, trans_b1=0;
				translate_range(&file_data_ary[0], first0, last0, &trans_a0, &trans_b0);
				translate_range(&file_data_ary[1], first1, last1, &trans_a1, &trans_b1);

				// Store information about these blocks in moved line info
				if (GetDetectMovedBlocks())
				{
					if (thisob->match0>=0)
					{
						assert(thisob->inserted > 0);
						for (int i=0; i<thisob->inserted; ++i)
						{
							int line0 = i+thisob->match0 + (trans_a0-first0-1);
							int line1 = i+thisob->line1 + (trans_a1-first1-1);
							GetMovedLines(1)->Add(MovedLines::SIDE::LEFT, line1, line0);
						}
					}
					if (thisob->match1>=0)
					{
						assert(thisob->deleted > 0);
						for (int i=0; i<thisob->deleted; ++i)
						{
							int line0 = i+thisob->line0 + (trans_a0-first0-1);
							int line1 = i+thisob->match1 + (trans_a1-first1-1);
							GetMovedLines(0)->Add(MovedLines::SIDE::RIGHT, line0, line1);
						}
					}
				}
				int nTrivialInserts = 0;
				if (op != OP_TRIVIAL && usefilters)
					nTrivialInserts = PostFilter(ctxt, thisob, file_data_ary);
				if (nTrivialInserts > 0)
				{
					while (thisob != next)
					{
						op = (thisob->trivial) ? OP_TRIVIAL : OP_DIFF;
						first0 = thisob->line0;
						first1 = thisob->line1;
						last0 = first0 + thisob->deleted - 1;
						last1 = first1 + thisob->inserted - 1;
						translate_range (&file_data_ary[0], first0, last0, &trans_a0, &trans_b0);
						translate_range (&file_data_ary[1], first1, last1, &trans_a1, &trans_b1);
						const int qtyLinesLeft = (trans_b0 - trans_a0) + 1
							- ((!thisob->link && file_data_ary[0].missing_newline) ? 1 : 0); //Determine quantity of lines in this block for left side
						const int qtyLinesRight = (trans_b1 - trans_a1) + 1
							- ((!thisob->link && file_data_ary[1].missing_newline) ? 1 : 0); //Determine quantity of lines in this block for right side

						if (op == OP_TRIVIAL && m_options.m_bCompletelyBlankOutIgnoredDiffereneces)
						{
							if (qtyLinesLeft == qtyLinesRight)
							{
								op = OP_NONE;
							}
							else
							{
								trans_a0 += qtyLinesLeft < qtyLinesRight ? qtyLinesLeft : qtyLinesRight;
								trans_a1 += qtyLinesLeft < qtyLinesRight ? qtyLinesLeft : qtyLinesRight;
							}
						}
						if (op != OP_NONE)
							AddDiffRange(m_pDiffList, trans_a0-1, trans_b0-1, trans_a1-1, trans_b1-1, op);

						thisob = thisob->link;
					}
				}
				else
				{
					if (thisob->trivial)
						op = OP_TRIVIAL;
					const int qtyLinesLeft = (trans_b0 - trans_a0) + 1
						- ((!thisob->link && file_data_ary[0].missing_newline) ? 1 : 0); //Determine quantity of lines in this block for left side
					const int qtyLinesRight = (trans_b1 - trans_a1) + 1
						- ((!thisob->link && file_data_ary[1].missing_newline) ? 1 : 0); //Determine quantity of lines in this block for right side
					if (op == OP_TRIVIAL && m_options.m_bCompletelyBlankOutIgnoredDiffereneces)
					{
						if (qtyLinesLeft == qtyLinesRight)
						{
							op = OP_NONE;
						}
						else
						{
							trans_a0 += qtyLinesLeft < qtyLinesRight ? qtyLinesLeft : qtyLinesRight;
							trans_a1 += qtyLinesLeft < qtyLinesRight ? qtyLinesLeft : qtyLinesRight;
						}
					}
					if (op != OP_NONE)
						AddDiffRange(m_pDiffList, trans_a0-1, trans_b0-1, trans_a1-1, trans_b1-1, op);
				}
			}
		}
	}
}

struct Comp02Functor
{
	Comp02Functor(const file_data * inf10, const file_data * inf12) :
		inf10_(inf10), inf12_(inf12)
	{
	}
	bool operator()(const DiffRangeInfo &dr3)
	{
		int line0 = dr3.begin[0];
		int line2 = dr3.begin[2];
		int line0end = dr3.end[0];
		int line2end = dr3.end[2];
		if (line0end - line0 != line2end - line2)
			return false;
		const char **linbuf0 = inf10_[1].linbuf + inf10_[1].linbuf_base;
		const char **linbuf2 = inf12_[1].linbuf + inf12_[1].linbuf_base;
		for (int i = 0; i < line0end - line0 + 1; ++i)
		{
			const size_t line0len = linbuf0[line0 + i + 1] - linbuf0[line0 + i];
			const size_t line2len = linbuf2[line2 + i + 1] - linbuf2[line2 + i];
			if (line_cmp(linbuf0[line0 + i], line0len, linbuf2[line2 + i], line2len) != 0)
				return false;
		}
		return true;
	}
	const file_data *inf10_;
	const file_data *inf12_;
};

/**
 * @brief Walk the diff utils change script, building the WinMerge list of diff blocks
 */
void
CDiffWrapper::LoadWinMergeDiffsFromDiffUtilsScript3(
	struct change * script10, 
	struct change * script12,  
	struct change * script02,  
	const file_data * inf10, 
	const file_data * inf12,
	const file_data * inf02)
{
	DiffList diff10, diff12, diff02;
	diff10.Clear();
	diff12.Clear();
	diff02.Clear();

	const bool usefilters = m_options.m_filterCommentsLines ||
		m_options.m_bIgnoreMissingTrailingEol ||
		(m_pFilterList && m_pFilterList->HasRegExps()) ||
		(m_pSubstitutionList && m_pSubstitutionList->HasRegExps());
	
	for (int file = 0; file < 3; file++)
	{
		struct change *next = nullptr;
		int trans_a0, trans_b0, trans_a1, trans_b1;
		int first0, last0, first1, last1, deletes, inserts;
		OP_TYPE op;
		const file_data *pinf = nullptr;
		DiffList *pdiff = nullptr;
		PostFilterContext ctxt;

		switch (file)
		{
		case 0: next = script10; pdiff = &diff10; pinf = inf10; break;
		case 1: next = script12; pdiff = &diff12; pinf = inf12; break;
		case 2: next = script02; pdiff = &diff02; pinf = inf02; break;
		}

		while (next != nullptr)
		{
			/* Find a set of changes that belong together.  */
			struct change *thisob = next;
			struct change *end = find_change(next);
			
			/* Disconnect them from the rest of the changes,
			making them a hunk, and remember the rest for next iteration.  */
			next = end->link;
			end->link = nullptr;
#ifdef DEBUG
			debug_script(thisob);
#endif

			/* Print thisob hunk.  */
			//(*printfun) (thisob);
			{					
				/* Determine range of line numbers involved in each file.  */
				analyze_hunk (thisob, &first0, &last0, &first1, &last1, &deletes, &inserts, pinf);
			
				/* Reconnect the script so it will all be freed properly.  */
				end->link = next;

				if (deletes || inserts || thisob->trivial)
				{
					if (deletes && inserts)
						op = OP_DIFF;
					else if (deletes || inserts)
						op = OP_DIFF;
					else
						op = OP_TRIVIAL;
					
					/* Print the lines that the first file has.  */
					translate_range (&pinf[0], first0, last0, &trans_a0, &trans_b0);
					translate_range (&pinf[1], first1, last1, &trans_a1, &trans_b1);

					// Store information about these blocks in moved line info
					if (GetDetectMovedBlocks())
					{
						int index1 = 0;  // defaults for (file == 0 /* diff10 */)
						int index2 = 1;
						MovedLines::SIDE side1 = MovedLines::SIDE::RIGHT;
						MovedLines::SIDE side2 = MovedLines::SIDE::LEFT;
						if (file == 1 /* diff12 */)
						{
							index1 = 2;
							index2 = 1;
							side1 = MovedLines::SIDE::LEFT;
							side2 = MovedLines::SIDE::RIGHT;
						}
						if (index1 != -1 && index2 != -1)
						{
							if (thisob->match0>=0)
							{
								assert(thisob->inserted > 0);
								for (int i=0; i<thisob->inserted; ++i)
								{
									int line0 = i+thisob->match0 + (trans_a0-first0-1);
									int line1 = i+thisob->line1 + (trans_a1-first1-1);
									GetMovedLines(index1)->Add(side1, line1, line0);
								}
							}
							if (thisob->match1>=0)
							{
								assert(thisob->deleted > 0);
								for (int i=0; i<thisob->deleted; ++i)
								{
									int line0 = i+thisob->line0 + (trans_a0-first0-1);
									int line1 = i+thisob->match1 + (trans_a1-first1-1);
									GetMovedLines(index2)->Add(side2, line0, line1);
								}
							}
						}
					}

					int nTrivialInserts = 0;
					if (op != OP_TRIVIAL && usefilters)
						nTrivialInserts = PostFilter(ctxt, thisob, pinf);
					if (nTrivialInserts)
					{
						while (thisob != next)
						{
							op = (thisob->trivial) ? OP_TRIVIAL : OP_DIFF;
							first0 = thisob->line0;
							first1 = thisob->line1;
							last0 = first0 + thisob->deleted - 1;
							last1 = first1 + thisob->inserted - 1;
							translate_range (&pinf[0], first0, last0, &trans_a0, &trans_b0);
							translate_range (&pinf[1], first1, last1, &trans_a1, &trans_b1);

							AddDiffRange(pdiff, trans_a0-1, trans_b0-1, trans_a1-1, trans_b1-1, op);
							thisob = thisob->link;
						}
					}
					else
					{
						if (thisob->trivial)
							op = OP_TRIVIAL;
						AddDiffRange(pdiff, trans_a0-1, trans_b0-1, trans_a1-1, trans_b1-1, op);
					}
				}
			}
		}
	}

	Make3wayDiff(m_pDiffList->GetDiffRangeInfoVector(), diff10.GetDiffRangeInfoVector(), diff12.GetDiffRangeInfoVector(), 
		Comp02Functor(inf10, inf12), 
		(usefilters || m_options.m_bIgnoreBlankLines));

	if (GetDetectMovedBlocks())
		m_pDiffList->GetDiffRangeInfoVector() = InsertMovedBlocks3Way();
}

std::vector<DiffRangeInfo> CDiffWrapper::InsertMovedBlocks3Way()
{
	std::vector<DiffRangeInfo> result;
	const MovedLines* pMovedLines0 = GetMovedLines(0);
	const MovedLines* pMovedLines1 = GetMovedLines(1);
	const MovedLines* pMovedLines2 = GetMovedLines(2);
	const std::vector<DiffRangeInfo> diffRangeList = m_pDiffList->GetDiffRangeInfoVector();
	for (const auto& diffInfo : diffRangeList)
	{
		const std::array<int, 3> elms = { diffInfo.end[0] + 1 - diffInfo.begin[0], diffInfo.end[1] + 1 - diffInfo.begin[1], diffInfo.end[2] + 1 - diffInfo.begin[2] };
		const int maxlines = *std::max_element(elms.begin(), elms.end());
		DiffRangeInfo diffInfo2 = diffInfo;
		int prevMovedLineI = -1;
		for (int i = 0; i < maxlines; ++i)
		{
			bool movedLine = false;
			if (diffInfo.begin[0] + i <= diffInfo.end[0])
				movedLine |= pMovedLines0->LineInBlock(diffInfo.begin[0] + i, MovedLines::SIDE::RIGHT) != -1;
			if (diffInfo.begin[1] + i <= diffInfo.end[1])
			{
				movedLine |= pMovedLines1->LineInBlock(diffInfo.begin[1] + i, MovedLines::SIDE::LEFT) != -1;
				movedLine |= pMovedLines1->LineInBlock(diffInfo.begin[1] + i, MovedLines::SIDE::RIGHT) != -1;
			}
			if (diffInfo.begin[2] + i <= diffInfo.end[2])
				movedLine |= pMovedLines2->LineInBlock(diffInfo.begin[2] + i, MovedLines::SIDE::LEFT) != -1;

			if (movedLine)
			{
				if (prevMovedLineI + 1 < i)
				{
					DiffRangeInfo diffInfoT = diffInfo2;
					for (int pane = 0; pane < 3; ++pane)
						diffInfoT.end[pane] = std::clamp(diffInfo.begin[pane] + i - 1, -1, diffInfo.end[pane]);
					result.push_back(diffInfoT);
				}

				DiffRangeInfo diffInfoM = diffInfo2;
				for (int pane = 0; pane < 3; ++pane)
				{
					diffInfoM.begin[pane] = std::clamp(diffInfo.begin[pane] + i, 0, diffInfo.end[pane] + 1);
					diffInfoM.end[pane] = std::clamp(diffInfo.begin[pane] + i, -1, diffInfo.end[pane]);
				}
				result.push_back(diffInfoM);

				for (int pane = 0; pane < 3; ++pane)
					diffInfo2.begin[pane] = std::clamp(diffInfo.begin[pane] + i + 1, 0, diffInfo.end[pane] + 1);
				prevMovedLineI = i;
			}
		}
		if (prevMovedLineI < maxlines - 1)
			result.push_back(diffInfo2);
	}
	return result;
}

void CDiffWrapper::WritePatchFileHeader(enum output_style tOutput_style, bool bAppendFiles)
{
	outfile = nullptr;
	if (!m_sPatchFile.empty())
	{
		const tchar_t *mode = (bAppendFiles ? _T("a+") : _T("w+"));
		if (cio::tfopen_s(&outfile, m_sPatchFile, mode) != 0)
			outfile = nullptr;
	}

	if (outfile == nullptr)
	{
		m_status.bPatchFileFailed = true;
		return;
	}

	// Output patchfile
	switch (tOutput_style)
	{
	case OUTPUT_NORMAL:
	case OUTPUT_CONTEXT:
	case OUTPUT_UNIFIED:
#if 0
	case OUTPUT_ED:
	case OUTPUT_FORWARD_ED:
	case OUTPUT_RCS:
	case OUTPUT_IFDEF:
	case OUTPUT_SDIFF:
#endif
		break;
	case OUTPUT_HTML:
		print_html_header();
		break;
	}
	
	fclose(outfile);
	outfile = nullptr;
}

void CDiffWrapper::WritePatchFileTerminator(enum output_style tOutput_style)
{
	outfile = nullptr;
	if (!m_sPatchFile.empty())
	{
		if (cio::tfopen_s(&outfile, m_sPatchFile, _T("a+")) != 0)
			outfile = nullptr;
	}

	if (outfile == nullptr)
	{
		m_status.bPatchFileFailed = true;
		return;
	}

	// Output patchfile
	switch (tOutput_style)
	{
	case OUTPUT_NORMAL:
	case OUTPUT_CONTEXT:
	case OUTPUT_UNIFIED:
#if 0
	case OUTPUT_ED:
	case OUTPUT_FORWARD_ED:
	case OUTPUT_RCS:
	case OUTPUT_IFDEF:
	case OUTPUT_SDIFF:
#endif
		break;
	case OUTPUT_HTML:
		print_html_terminator();
		break;
	}
	
	fclose(outfile);
	outfile = nullptr;
}

/**
 * @brief Write out a patch file.
 * Writes patch file using already computed diffutils script. Converts path
 * delimiters from \ to / since we want to keep compatibility with patch-tools.
 * @param [in] script list of changes.
 * @param [in] inf file_data table containing filenames
 */
void CDiffWrapper::WritePatchFile(struct change * script, file_data * inf)
{
	file_data inf_patch[2] = { inf[0], inf[1] };

	// Get paths, primarily use alternative paths, only if they are empty
	// use full filepaths
	String path1(m_alternativePaths[0]);
	String path2(m_alternativePaths[1]);
	if (path1.empty())
		path1 = m_files[0];
	if (path2.empty())
		path2 = m_files[1];
	path1 = paths::ToUnixPath(path1);
	path2 = paths::ToUnixPath(path2);
	auto strdupPath = [](const String& path, const void *buffer, size_t buffered_chars) -> char*
	{
		FileTextEncoding encoding = codepage_detect::Guess(_T(""), buffer, buffered_chars, 1);
		if (encoding.m_unicoding != ucr::NONE)
			encoding.SetUnicoding(ucr::UTF8);
		ucr::buffer buf(256);
		ucr::convert(ucr::CP_TCHAR, reinterpret_cast<const unsigned char *>(path.c_str()), static_cast<int>(path.size() * sizeof(tchar_t)), encoding.m_codepage, &buf);
		return strdup(reinterpret_cast<const char *>(buf.ptr));
	};
	inf_patch[0].name = strdupPath(path1, inf_patch[0].buffer, inf_patch[0].buffered_chars);
	inf_patch[1].name = strdupPath(path2, inf_patch[1].buffer, inf_patch[1].buffered_chars);

	// If paths in m_s1File and m_s2File point to original files, then we can use
	// them to fix potentially meaningless stats from potentially temporary files,
	// resulting from whatever transforms may have taken place.
	// If not, then we can't help it, and hence assert that this won't happen.
	if (!m_bPathsAreTemp)
	{
		mywstat(m_files[0].c_str(), &inf_patch[0].stat);
		mywstat(m_files[1].c_str(), &inf_patch[1].stat);
	}
	else
	{
		assert(false);
	}

	outfile = nullptr;
	if (!m_sPatchFile.empty())
	{
		const tchar_t *mode = (m_bAppendFiles ? _T("a+") : _T("w+"));
		if (cio::tfopen_s(&outfile, m_sPatchFile, mode) != 0)
			outfile = nullptr;
	}

	if (outfile == nullptr)
	{
		m_status.bPatchFileFailed = true;
		return;
	}

	if (paths::IsNullDeviceName(ucr::toTString(inf[0].name)))
	{
		free((void *)inf_patch[0].name);
		inf_patch[0].name = strdup("/dev/null");
	}
	if (paths::IsNullDeviceName(ucr::toTString(inf[1].name)))
	{
		free((void *)inf_patch[1].name);
		inf_patch[1].name = strdup("/dev/null");
	}

	// Print "command line"
	if (m_bAddCmdLine && output_style != OUTPUT_HTML)
	{
		String switches = FormatSwitchString();
		fprintf(outfile, "diff%S %s %s\n",
			switches.c_str(), inf_patch[0].name, inf_patch[1].name);
	}

	// Output patchfile
	switch (output_style)
	{
	case OUTPUT_NORMAL:
		print_normal_script(script);
		break;
	case OUTPUT_CONTEXT:
		print_context_header(inf_patch, 0);
		print_context_script(script, 0);
		break;
	case OUTPUT_UNIFIED:
		print_context_header(inf_patch, 1);
		print_context_script(script, 1);
		break;
#if 0
	case OUTPUT_ED:
		print_ed_script(script);
		break;
	case OUTPUT_FORWARD_ED:
		pr_forward_ed_script(script);
		break;
	case OUTPUT_RCS:
		print_rcs_script(script);
		break;
	case OUTPUT_IFDEF:
		print_ifdef_script(script);
		break;
	case OUTPUT_SDIFF:
		print_sdiff_script(script);
		break;
#endif
	case OUTPUT_HTML:
		print_html_diff_header(inf_patch);
		print_html_script(script);
		print_html_diff_terminator();
	}
	
	fclose(outfile);
	outfile = nullptr;

	free((void *)inf_patch[0].name);
	free((void *)inf_patch[1].name);
}

const FilterList* CDiffWrapper::GetFilterList() const
{
	return m_pFilterList.get();
}

void CDiffWrapper::SetFilterList(std::shared_ptr<FilterList> pFilterList)
{
	m_pFilterList = std::move(pFilterList);
}

const SubstitutionList* CDiffWrapper::GetSubstitutionList() const
{
	return m_pSubstitutionList.get();
}

void CDiffWrapper::SetSubstitutionList(std::shared_ptr<SubstitutionList> pSubstitutionList)
{
	m_pSubstitutionList = std::move(pSubstitutionList);
}

void CDiffWrapper::SetFilterCommentsSourceDef(const String& ext)
{
	m_pFilterCommentsDef = CrystalLineParser::GetTextType(ext.c_str());
}

/**
 * @brief Copy text stat results from diffutils back into the FileTextStats structure
 */
void CopyTextStats(const file_data * inf, FileTextStats * myTextStats)
{
	myTextStats->ncrlfs = inf->count_crlfs;
	myTextStats->ncrs = inf->count_crs;
	myTextStats->nlfs = inf->count_lfs;
	myTextStats->nzeros = inf->count_zeros;
}

/**
 * @brief Copy both left & right text stats results back into the DiffFileData text stats
 */
void CopyDiffutilTextStats(file_data *inf, DiffFileData * diffData)
{
	CopyTextStats(&inf[0], &diffData->m_textStats[0]);
	CopyTextStats(&inf[1], &diffData->m_textStats[1]);
}
