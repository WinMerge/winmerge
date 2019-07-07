/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//    
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
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
#include <sys/types.h>
#include <sys/stat.h>
#include <algorithm>
#include <string>
#include <cctype>
#include <cwctype>
#include <map>
#include <cassert>
#include <exception>
#include <vector>
#include <list>
#include <Poco/Format.h>
#include <Poco/Debugger.h>
#include <Poco/StringTokenizer.h>
#include <Poco/Exception.h>
#include "DiffContext.h"
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
#include "FolderCmp.h"
#include "FilterCommentsManager.h"
#include "Environment.h"
#include "PatchHTML.h"
#include "UnicodeString.h"
#include "unicoder.h"
#include "TFile.h"
#include "Exceptions.h"
#include "MergeApp.h"

using Poco::Debugger;
using Poco::format;
using Poco::StringTokenizer;
using Poco::Exception;

extern int recursive;

static void CopyTextStats(const file_data * inf, FileTextStats * myTextStats);
static void CopyDiffutilTextStats(file_data *inf, DiffFileData * diffData);

/**
 * @brief Default constructor.
 * Initializes members and creates new FilterCommentsManager.
 */
CDiffWrapper::CDiffWrapper()
: m_pFilterCommentsManager(nullptr)
, m_bCreatePatchFile(false)
, m_bUseDiffList(false)
, m_bAddCmdLine(true)
, m_bAppendFiles(false)
, m_nDiffs(0)
, m_infoPrediffer(nullptr)
, m_pDiffList(nullptr)
, m_bPathsAreTemp(false)
, m_pFilterList(nullptr)
, m_bPluginsEnabled(false)
, m_status()
{
	// character that ends a line.  Currently this is always `\n'
	line_end_char = '\n';
}

/**
 * @brief Destructor.
 */
CDiffWrapper::~CDiffWrapper()
{
}

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
void CDiffWrapper::SetOptions(const DIFFOPTIONS *options)
{
	assert(options != nullptr);
	m_options.SetFromDiffOptions(*options);
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

/**
 * @brief Test for trivial only characters in string
 * @param [in] Start				- Start position in string
 * @param [in] End					- One character pass the end position of the string
 * @param [in] filtercommentsset	- For future use to determine trivial bytes
 * @return Returns true if all characters are trivial
 */
bool CDiffWrapper::IsTrivialBytes(const char* Start, const char* End,
	const FilterCommentsSet& filtercommentsset) const
{
	std::string testdata(Start, End);
	//@TODO: Need to replace the following trivial string with a user specified string
	size_t pos = testdata.find_first_not_of(" \t\r\n");
	return (pos == std::string::npos);
}

/**
 * @brief Test for a line of trivial data
 * @param [in] Line					- String to test for
 * @param [in] StartOfComment		- 
 * @param [in] EndOfComment			- 
 * @param [in] InLineComment		- 
 * @param [in] filtercommentsset	- Comment marker set used to indicate comment blocks.
 * @return Returns true if entire line is trivial
 */
bool CDiffWrapper::IsTrivialLine(const std::string &Line, 
				   const char * StartOfComment,	
				   const char * EndOfComment,	
				   const char * InLineComment,	
				   const FilterCommentsSet& filtercommentsset) const
{
	//Do easy test first
	if ((StartOfComment == nullptr || EndOfComment == nullptr) && InLineComment == nullptr)
		return false;//In no Start and End pair, and no single in-line set, then it's not trivial

	if (StartOfComment == Line.c_str() &&
		static_cast<size_t>((EndOfComment + filtercommentsset.EndMarker.size()) - StartOfComment) == Line.size())
	{//If entire line is blocked by End and Start markers, then entire line is trivial
		return true;
	}

	if (InLineComment != nullptr && InLineComment < StartOfComment)
	{
		if (InLineComment == Line.c_str())
			return true;//If line starts with InLineComment marker, then entire line is trivial

		//Other wise, check if data before InLineComment marker is trivial
		return IsTrivialBytes(Line.c_str(), InLineComment, filtercommentsset);
	}

	//Done with easy test, so now do more complex test
	if (StartOfComment != nullptr && 
		EndOfComment != nullptr && 
		StartOfComment < EndOfComment &&
		IsTrivialBytes(Line.c_str(), StartOfComment, filtercommentsset) &&
		IsTrivialBytes(EndOfComment + filtercommentsset.EndMarker.size(),
			Line.c_str()+Line.size(), filtercommentsset))
	{
		return true;
	}

	return false;
}

/**
 * @brief Find comment marker in string, excluding portions enclosed in quotation marks or apostrophes
 * @param [in] target				- string to search
 * @param [in] marker				- marker to search for
 * @return Returns position of marker, or `nullptr` if none is present
 */
static const char *FindCommentMarker(const char *target, const char *marker)
{
	char prev = '\0';
	char quote = '\0';
	size_t marker_len = strlen(marker);
	while (char c = *target)
	{
		if (quote == '\0' && strncmp(target, marker, marker_len) == 0)
			return target;
		if ((prev != '\\') &&
			(c == '"' || c == '\'') &&
			(quote == '\0' || quote == c))
		{
			quote ^= c;
		}
		prev = c;
		++target;
	}
	return nullptr;
}

/**
 * @brief Replace spaces in a string
 * @param [in] str - String to search
 * @param [in] rep - String to replace
 */
static void ReplaceSpaces(std::string & str, const char *rep)
{
	std::string::size_type pos = 0;
	size_t replen = strlen(rep);
	while ((pos = str.find_first_of(" \t", pos)) != std::string::npos)
	{
		std::string::size_type posend = str.find_first_not_of(" \t", pos);
		if (posend != std::string::npos)
			str.replace(pos, posend - pos, rep);
		else
			str.replace(pos, 1, rep);
		pos += replen;
	}
}

/**
	@brief Performs post-filtering, by setting comment blocks to trivial
	@param [in]  StartPos			- First line number to read
	@param [in]  EndPos				- The line number PASS the last line number to read
	@param [in]  QtyLinesInBlock		- Number of lines in diff block.  Not needed in backward direction.
	@param [in]  Direction			- This should be 1 or -1, to indicate which direction to read (backward or forward)
	@param [in,out]  Op				- This variable is set to trivial if block should be ignored.
	@param [in]  FileNo				- Should be 0 or 1, to indicate left or right file.
	@param [in]  filtercommentsset	- Comment marker set used to indicate comment blocks.
	@return		Always returns true in reverse direction.
				In forward direction, returns false if none trivial data is found within QtyLinesInBlock
*/
bool CDiffWrapper::PostFilter(int StartPos, int EndPos, int Direction,
	int QtyLinesInBlock, OP_TYPE &Op, int FileNo,
	FilterCommentsSet& filtercommentsset) const
{
	if (Op == OP_TRIVIAL) //If already set to trivial, then exit.
		return true;
	bool OpShouldBeTrivial = false;
	int QtyTrivialLines = 0;
	for(int i = StartPos + ((Direction == -1)?-1:0); i != EndPos;i += Direction)
	{
		if ((i - StartPos) == QtyLinesInBlock && 
			QtyLinesInBlock == QtyTrivialLines)
		{
			OpShouldBeTrivial = true;
			break;
		}
		size_t len = files[FileNo].linbuf[i + 1] - files[FileNo].linbuf[i];
		const char *LineStr = files[FileNo].linbuf[i];
		std::string LineData(LineStr, linelen(LineStr, len));

		const char * StartOfComment		= FindCommentMarker(LineData.c_str(), filtercommentsset.StartMarker.c_str());
		const char * EndOfComment		= FindCommentMarker(LineData.c_str(), filtercommentsset.EndMarker.c_str());
		const char * InLineComment		= FindCommentMarker(LineData.c_str(), filtercommentsset.InlineMarker.c_str());
		//The following logic determines if the entire block is a comment block, and only marks it as trivial
		//if all the changes are within a comment block.
		if (Direction == -1)
		{
			if (!StartOfComment && EndOfComment)
				break;
			
			if (StartOfComment && (!EndOfComment || EndOfComment < StartOfComment) && (!InLineComment || InLineComment > StartOfComment))
			{
				OpShouldBeTrivial = true;
				break;
			}
		}
		else if (Direction == 1)
		{
			if (IsTrivialBytes(LineData.c_str(), LineData.c_str()+LineData.size(), filtercommentsset) || 
				IsTrivialLine(LineData, StartOfComment,	EndOfComment, InLineComment, filtercommentsset))
			{
				++QtyTrivialLines;
			}

			if (!EndOfComment && StartOfComment)
			{
				if (i == (StartPos + QtyTrivialLines) )
				{
					if (StartOfComment == LineData.c_str())
					{//If this is at the beginning of the first line, then lets continue
						continue;
					}
					if (IsTrivialBytes(LineData.c_str(), StartOfComment, filtercommentsset))
					{//If only trivial bytes before comment marker, then continue
						continue;
					}
					break;
				}
				//If this is not the first line, then assume
				//previous lines are non-trivial, and return true.
				return false;
			}

			if (EndOfComment != nullptr && 
				(StartOfComment == nullptr || StartOfComment > EndOfComment) && 
				(InLineComment == nullptr || InLineComment > EndOfComment) )
			{
				if (!IsTrivialBytes(EndOfComment+filtercommentsset.EndMarker.size(), LineData.c_str()+LineData.size(), filtercommentsset))
				{
					return false;
				}

				if ((i - StartPos) >=  (QtyLinesInBlock-1))
				{
					OpShouldBeTrivial = true;
					break;
				}

				//Lets check if the remaining lines only contain trivial data
				bool AllRemainingLinesContainTrivialData = true;
				int TrivLinePos = i+1;
				for(; TrivLinePos != (StartPos + QtyLinesInBlock);++TrivLinePos)
				{
					size_t len1 = files[FileNo].linbuf[TrivLinePos + 1] - files[FileNo].linbuf[TrivLinePos];
					const char *LineStrTrvCk = files[FileNo].linbuf[TrivLinePos];
					std::string LineDataTrvCk(LineStrTrvCk, linelen(LineStrTrvCk, len1));
					if (LineDataTrvCk.size() &&
						!IsTrivialBytes(LineDataTrvCk.c_str(), LineDataTrvCk.c_str() + LineDataTrvCk.size(), filtercommentsset))
					{
						AllRemainingLinesContainTrivialData = false;
						break;
					}
				}
				if (AllRemainingLinesContainTrivialData)
				{
					OpShouldBeTrivial = true;
					break;
				}
				if (TrivLinePos != (StartPos + QtyLinesInBlock) )
				{
					return PostFilter(TrivLinePos, EndPos, Direction, QtyLinesInBlock - (TrivLinePos - StartPos), Op, FileNo, filtercommentsset);
				}
			}
		}
	}
	if (OpShouldBeTrivial)
	{
		Op = OP_TRIVIAL;
	}
	return true;
}

/**
@brief The main entry for post filtering.  Performs post-filtering, by setting comment blocks to trivial
@param [in]  LineNumberLeft		- First line number to read from left file
@param [in]  QtyLinesLeft		- Number of lines in the block for left file
@param [in]  LineNumberRight		- First line number to read from right file
@param [in]  QtyLinesRight		- Number of lines in the block for right file
@param [in,out]  Op				- This variable is set to trivial if block should be ignored.
@param [in]  FileNameExt			- The file name extension.  Needs to be lower case string ("cpp", "java", "c")
*/
void CDiffWrapper::PostFilter(int LineNumberLeft, int QtyLinesLeft, int LineNumberRight,
	int QtyLinesRight, OP_TYPE &Op, const String& FileNameExt) const
{
	if (Op == OP_TRIVIAL || m_pFilterCommentsManager == nullptr)
		return;
	
	//First we need to get lowercase file name extension
	FilterCommentsSet filtercommentsset = m_pFilterCommentsManager->GetSetForFileType(FileNameExt);
	if (filtercommentsset.StartMarker.empty() && 
		filtercommentsset.EndMarker.empty() &&
		filtercommentsset.InlineMarker.empty())
	{
		return;
	}

	OP_TYPE LeftOp = OP_NONE;
	OP_TYPE RightOp = OP_NONE;

	if (QtyLinesRight == 0)
	{	//Only check left side
		if (PostFilter(LineNumberLeft, files[0].valid_lines, 1, QtyLinesLeft, LeftOp, 0, filtercommentsset))
			PostFilter(LineNumberLeft, -1, -1, QtyLinesLeft, LeftOp, 0, filtercommentsset);
	}
	else if (QtyLinesLeft == 0)
	{	//Only check right side
		if (PostFilter(LineNumberRight, files[1].valid_lines, 1, QtyLinesRight, RightOp, 1, filtercommentsset))
			PostFilter(LineNumberRight, -1, -1, QtyLinesRight, RightOp, 1, filtercommentsset);
	}
	else
	{
		if (PostFilter(LineNumberLeft, files[0].valid_lines, 1, QtyLinesLeft, LeftOp, 0, filtercommentsset))
			PostFilter(LineNumberLeft, -1, -1, QtyLinesLeft, LeftOp, 0, filtercommentsset);

		if (PostFilter(LineNumberRight, files[1].valid_lines, 1, QtyLinesRight, RightOp, 1, filtercommentsset))
			PostFilter(LineNumberRight, -1, -1, QtyLinesRight, RightOp, 1, filtercommentsset);
	}

	std::list<std::string> LeftLines, RightLines;
	for (int i = 0; (i < QtyLinesLeft) || (i < QtyLinesRight); i++)
	{
		//Lets test  all lines if only a comment is different.
		const char *	LineStrLeft = "";
		const char *	EndLineLeft = LineStrLeft;
		const char *	LineStrRight = "";
		const char *	EndLineRight = LineStrRight;
		if(i < QtyLinesLeft)
		{
			LineStrLeft = files[0].linbuf[LineNumberLeft + i];
			EndLineLeft = files[0].linbuf[LineNumberLeft + i + 1];
		}
		if(i < QtyLinesRight)
		{
			LineStrRight = files[1].linbuf[LineNumberRight + i];
			EndLineRight = files[1].linbuf[LineNumberRight + i + 1];
		}
			
		if (EndLineLeft != nullptr && EndLineRight != nullptr)
		{	
			std::string LineDataLeft(LineStrLeft, EndLineLeft);
			std::string LineDataRight(LineStrRight, EndLineRight);

			if (!filtercommentsset.StartMarker.empty() && !filtercommentsset.EndMarker.empty())
			{
				const char * CommentStrLeftStart;
				const char * CommentStrLeftEnd;
				const char * CommentStrRightStart;
				const char * CommentStrRightEnd;

				bool bFirstLoop = true;
				do {
					//Lets remove block comments, and see if lines are equal
					CommentStrLeftStart = FindCommentMarker(LineDataLeft.c_str(), filtercommentsset.StartMarker.c_str());
					CommentStrLeftEnd = FindCommentMarker(LineDataLeft.c_str(), filtercommentsset.EndMarker.c_str());
					CommentStrRightStart = FindCommentMarker(LineDataRight.c_str(), filtercommentsset.StartMarker.c_str());
					CommentStrRightEnd = FindCommentMarker(LineDataRight.c_str(), filtercommentsset.EndMarker.c_str());
					
					if (CommentStrLeftStart != nullptr && CommentStrLeftEnd != nullptr && CommentStrLeftStart < CommentStrLeftEnd)
						LineDataLeft.erase(CommentStrLeftStart - LineDataLeft.c_str(), CommentStrLeftEnd + filtercommentsset.EndMarker.size() - CommentStrLeftStart);
					else if (CommentStrLeftEnd != nullptr)
						LineDataLeft.erase(0, CommentStrLeftEnd + filtercommentsset.EndMarker.size() - LineDataLeft.c_str());
					else if (CommentStrLeftStart != nullptr)
						LineDataLeft.erase(CommentStrLeftStart - LineDataLeft.c_str());
					else if(LeftOp == OP_TRIVIAL && bFirstLoop)
						LineDataLeft.erase(0);  //This line is all in block comments

					if (CommentStrRightStart != nullptr && CommentStrRightEnd != nullptr && CommentStrRightStart < CommentStrRightEnd)
						LineDataRight.erase(CommentStrRightStart - LineDataRight.c_str(), CommentStrRightEnd + filtercommentsset.EndMarker.size() - CommentStrRightStart);
					else if (CommentStrRightEnd != nullptr)
						LineDataRight.erase(0, CommentStrRightEnd + filtercommentsset.EndMarker.size() - LineDataRight.c_str());
					else if (CommentStrRightStart != nullptr)
						LineDataRight.erase(CommentStrRightStart - LineDataRight.c_str());
					else if(RightOp == OP_TRIVIAL && bFirstLoop)
						LineDataRight.erase(0);  //This line is all in block comments

					bFirstLoop = false;

				} while (CommentStrLeftStart != nullptr || CommentStrLeftEnd != nullptr
					|| CommentStrRightStart != nullptr || CommentStrRightEnd != nullptr); //Loops until all blockcomments are lost
			}

			if (!filtercommentsset.InlineMarker.empty())
			{
				//Lets remove line comments
				const char * CommentStrLeft = FindCommentMarker(LineDataLeft.c_str(), filtercommentsset.InlineMarker.c_str());
				const char * CommentStrRight = FindCommentMarker(LineDataRight.c_str(), filtercommentsset.InlineMarker.c_str());

				if (CommentStrLeft != nullptr)
					LineDataLeft.erase(CommentStrLeft - LineDataLeft.c_str());
				if (CommentStrRight != nullptr)
					LineDataRight.erase(CommentStrRight - LineDataRight.c_str());
			}

		if (m_options.m_ignoreWhitespace == WHITESPACE_IGNORE_ALL)
			{
				//Ignore character case
				ReplaceSpaces(LineDataLeft, "");
				ReplaceSpaces(LineDataRight, "");
			}
			else if (m_options.m_ignoreWhitespace == WHITESPACE_IGNORE_CHANGE)
			{
				//Ignore change in whitespace char count
				ReplaceSpaces(LineDataLeft, " ");
				ReplaceSpaces(LineDataRight, " ");
			}

			if (m_options.m_bIgnoreCase)
			{
				//ignore case
				// std::transform(LineDataLeft.begin(),  LineDataLeft.end(),  LineDataLeft.begin(),  ::toupper);
				for (std::basic_string<char>::iterator pb = LineDataLeft.begin(), pe = LineDataLeft.end(); pb != pe; ++pb) 
					*pb = static_cast<char>(::toupper(*pb)); 
				// std::transform(LineDataRight.begin(), LineDataRight.end(), LineDataRight.begin(), ::toupper);
				for (std::basic_string<char>::iterator pb = LineDataRight.begin(), pe = LineDataRight.end(); pb != pe; ++pb) 
					*pb = static_cast<char>(::toupper(*pb)); 
			}

			if (!LineDataLeft.empty())
				LeftLines.push_back(LineDataLeft);
			if (!LineDataRight.empty())
				RightLines.push_back(LineDataRight);
		}
	}
	if (LeftLines != RightLines)
		return;
	//only difference is trival
	Op = OP_TRIVIAL;
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
			if (!FileTransform::Prediffing(m_infoPrediffer.get(), strFileTemp[file], m_sToFindPrediffer, m_bPathsAreTemp))
			{
				// display a message box
				String sError = strutils::format(
					_T("An error occurred while prediffing the file '%s' with the plugin '%s'. The prediffing is not applied any more."),
					strFileTemp[file].c_str(),
					m_infoPrediffer->m_PluginName.c_str());
				AppErrorMessageBox(sError);
				// don't use any more this prediffer
				m_infoPrediffer->m_PluginOrPredifferMode = PLUGIN_MANUAL;
				m_infoPrediffer->m_PluginName.erase();
			}

			// We use the same plugin for both files, so it must be defined before
			// second file
			assert(m_infoPrediffer->m_PluginOrPredifferMode == PLUGIN_MANUAL);
		}
	}

	struct change *script = nullptr;
	struct change *script10 = nullptr;
	struct change *script12 = nullptr;
	DiffFileData diffdata, diffdata10, diffdata12;
	int bin_flag = 0, bin_flag10 = 0, bin_flag12 = 0;

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

		if (_tfopen_s(&outfile, path.c_str(), _T("w+")) == 0)
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
	}

	// First determine what happened during comparison
	// If there were errors or files were binaries, don't bother
	// creating diff-lists or patches
	
	// diff_2_files set bin_flag to -1 if different binary
	// diff_2_files set bin_flag to +1 if same binary

	file_data * inf = diffdata.m_inf;
	file_data * inf10 = diffdata10.m_inf;
	file_data * inf12 = diffdata12.m_inf;

	if (aFiles.GetSize() == 2)
	{
		if (bin_flag != 0)
		{
			m_status.bBinaries = true;
			if (bin_flag != -1)
				m_status.Identical = IDENTLEVEL_ALL;
			else
				m_status.Identical = IDENTLEVEL_NONE;
		}
		else
		{ // text files according to diffutils, so change script exists
			m_status.Identical = (script == 0) ? IDENTLEVEL_ALL : IDENTLEVEL_NONE;
			m_status.bBinaries = false;
		}
		m_status.bMissingNL[0] = !!inf[0].missing_newline;
		m_status.bMissingNL[1] = !!inf[1].missing_newline;
	}
	else
	{
		m_status.Identical = IDENTLEVEL_NONE;
		if (bin_flag10 != 0 || bin_flag12 != 0)
		{
			m_status.bBinaries = true;
			if (bin_flag10 != -1 && bin_flag12 != -1)
				m_status.Identical = IDENTLEVEL_ALL;
			else if (bin_flag10 != -1)
				m_status.Identical = IDENTLEVEL_EXCEPTRIGHT;
			else if (bin_flag12 != -1)
				m_status.Identical = IDENTLEVEL_EXCEPTLEFT;
			else
				m_status.Identical = IDENTLEVEL_EXCEPTMIDDLE;
		}
		else
		{ // text files according to diffutils, so change script exists
			m_status.bBinaries = false;
			if (script10 == nullptr && script12 == nullptr)
				m_status.Identical = IDENTLEVEL_ALL;
			else if (script10 == nullptr)
				m_status.Identical = IDENTLEVEL_EXCEPTRIGHT;
			else if (script12 == nullptr)
				m_status.Identical = IDENTLEVEL_EXCEPTLEFT;
			else
				m_status.Identical = IDENTLEVEL_EXCEPTMIDDLE;
		}
		m_status.bMissingNL[0] = !!inf10[1].missing_newline;
		m_status.bMissingNL[1] = !!inf12[0].missing_newline;
		m_status.bMissingNL[2] = !!inf12[1].missing_newline;
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
				script10, script12,
				diffdata10.m_inf, diffdata12.m_inf);
	}			

	// cleanup the script
	if (aFiles.GetSize() == 2)
		FreeDiffUtilsScript(script);
	else
	{
		FreeDiffUtilsScript(script10);
		FreeDiffUtilsScript(script12);
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
					LogErrorStringUTF8(e.displayText());
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
		AppErrorMessageBox(ucr::toTString(e.what()));
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
		AppErrorMessageBox(ucr::toTString(e.what()));
	}
}

/**
 * @brief Expand last DIFFRANGE of file by one line to contain last line after EOL.
 * @param [in] leftBufferLines size of array pane left
 * @param [in] rightBufferLines size of array pane right
 * @param [in] left on whitch side we have to insert
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
		switches = _T(" C");
		break;
	case DIFF_OUTPUT_UNIFIED:
		switches = _T(" U");
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

	if (m_options.m_contextLines > 0)
	{
		TCHAR tmpNum[5] = {0};
		_itot_s(m_options.m_contextLines, tmpNum, 10);
		switches += tmpNum;
	}

	if (ignore_all_space_flag > 0)
		switches += _T("w");

	if (ignore_blank_lines_flag > 0)
		switches += _T("B");

	if (ignore_case_flag > 0)
		switches += _T("i");

	if (ignore_space_change_flag > 0)
		switches += _T("b");

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
 * execption is trapped, return compare failure.
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
			unsigned xdl_flags = make_xdl_flags(m_options);
			*diffs = diff_2_files_xdiff(diffData->m_inf, (m_pMovedLines[0] != nullptr), xdl_flags);
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
bool CDiffWrapper::RegExpFilter(int StartPos, int EndPos, int FileNo) const
{
	if (m_pFilterList == nullptr)
	{	
		throw "CDiffWrapper::RegExpFilter() called when "
			"filterlist doesn't exist (=nullptr)";
	}

	bool linesMatch = true; // set to false when non-matching line is found.
	int line = StartPos;

	while (line <= EndPos && linesMatch)
	{
		size_t len = files[FileNo].linbuf[line + 1] - files[FileNo].linbuf[line];
		const char *string = files[FileNo].linbuf[line];
		size_t stringlen = linelen(string, len);
		if (!m_pFilterList->Match(std::string(string, stringlen)))

		{
			linesMatch = false;
		}
		++line;
	}
	return linesMatch;
}

/**
 * @brief Walk the diff utils change script, building the WinMerge list of diff blocks
 */
void
CDiffWrapper::LoadWinMergeDiffsFromDiffUtilsScript(struct change * script, const file_data * inf)
{
	//Logic needed for Ignore comment option
	DIFFOPTIONS options;
	GetOptions(&options);
	String asLwrCaseExt;
	if (options.bFilterCommentsLines)
	{
		String LowerCaseExt = m_originalFile.GetLeft();
		String::size_type PosOfDot = LowerCaseExt.rfind('.');
		if (PosOfDot != String::npos)
		{
			LowerCaseExt.erase(0, PosOfDot + 1);
			std::transform(LowerCaseExt.begin(), LowerCaseExt.end(), LowerCaseExt.begin(), ::towlower);
			asLwrCaseExt = LowerCaseExt;
		}
	}

	struct change *next = script;
	
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
			analyze_hunk (thisob, &first0, &last0, &first1, &last1, &deletes, &inserts, inf);
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
				translate_range(&inf[0], first0, last0, &trans_a0, &trans_b0);
				translate_range(&inf[1], first1, last1, &trans_a1, &trans_b1);

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
							GetMovedLines(1)->Add(MovedLines::SIDE_LEFT, line1, line0);
						}
					}
					if (thisob->match1>=0)
					{
						assert(thisob->deleted > 0);
						for (int i=0; i<thisob->deleted; ++i)
						{
							int line0 = i+thisob->line0 + (trans_a0-first0-1);
							int line1 = i+thisob->match1 + (trans_a1-first1-1);
							GetMovedLines(0)->Add(MovedLines::SIDE_RIGHT, line0, line1);
						}
					}
				}

				if (options.bFilterCommentsLines)
				{
					int QtyLinesLeft = (trans_b0 - trans_a0) + 1; //Determine quantity of lines in this block for left side
					int QtyLinesRight = (trans_b1 - trans_a1) + 1;//Determine quantity of lines in this block for right side
					PostFilter(thisob->line0, QtyLinesLeft, thisob->line1, QtyLinesRight, op, asLwrCaseExt);
				}

				if (m_pFilterList != nullptr && m_pFilterList->HasRegExps())
				{
					 //Determine quantity of lines in this block for both sides
					int QtyLinesLeft = (trans_b0 - trans_a0);
					int QtyLinesRight = (trans_b1 - trans_a1);
					
					// Match lines against regular expression filters
					// Our strategy is that every line in both sides must
					// match regexp before we mark difference as ignored.
					bool match2 = false;
					bool match1 = RegExpFilter(thisob->line0, thisob->line0 + QtyLinesLeft, 0);
					if (match1)
						match2 = RegExpFilter(thisob->line1, thisob->line1 + QtyLinesRight, 1);
					if (match1 && match2)
						op = OP_TRIVIAL;
				}

				AddDiffRange(m_pDiffList, trans_a0-1, trans_b0-1, trans_a1-1, trans_b1-1, op);
			}
		}
		
		/* Reconnect the script so it will all be freed properly.  */
		end->link = next;
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
	const file_data * inf10, 
	const file_data * inf12)
{
	DiffList diff10, diff12;
	diff10.Clear();
	diff12.Clear();

	for (int file = 0; file < 2; file++)
	{
		struct change *next = nullptr;
		int trans_a0, trans_b0, trans_a1, trans_b1;
		int first0, last0, first1, last1, deletes, inserts;
		OP_TYPE op;
		const file_data *pinf = nullptr;
		DiffList *pdiff = nullptr;

		switch (file)
		{
		case 0: next = script10; pdiff = &diff10; pinf = inf10; break;
		case 1: next = script12; pdiff = &diff12; pinf = inf12; break;
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
						MovedLines::ML_SIDE side1 = MovedLines::SIDE_RIGHT;
						MovedLines::ML_SIDE side2 = MovedLines::SIDE_LEFT;
						if (file == 1 /* diff12 */)
						{
							index1 = 2;
							index2 = 1;
							side1 = MovedLines::SIDE_LEFT;
							side2 = MovedLines::SIDE_RIGHT;
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

					AddDiffRange(pdiff, trans_a0-1, trans_b0-1, trans_a1-1, trans_b1-1, op);
				}
			}
			
			/* Reconnect the script so it will all be freed properly.  */
			end->link = next;
		}
	}

	Make3wayDiff(m_pDiffList->GetDiffRangeInfoVector(), diff10.GetDiffRangeInfoVector(), diff12.GetDiffRangeInfoVector(), 
		Comp02Functor(inf10, inf12), (m_pFilterList != nullptr && m_pFilterList->HasRegExps()));
}

void CDiffWrapper::WritePatchFileHeader(enum output_style tOutput_style, bool bAppendFiles)
{
	outfile = nullptr;
	if (!m_sPatchFile.empty())
	{
		const TCHAR *mode = (bAppendFiles ? _T("a+") : _T("w+"));
		if (_tfopen_s(&outfile, m_sPatchFile.c_str(), mode) != 0)
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
		if (_tfopen_s(&outfile, m_sPatchFile.c_str(), _T("a+")) != 0)
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
	if (ucr::CheckForInvalidUtf8(inf_patch[0].linbuf[inf_patch[0].linbuf_base], inf_patch[0].buffered_chars))
	{
		inf_patch[0].name = _strdup(ucr::toThreadCP(path1).c_str());
		inf_patch[1].name = _strdup(ucr::toThreadCP(path2).c_str());
	}
	else
	{
		inf_patch[0].name = _strdup(ucr::toUTF8(path1).c_str());
		inf_patch[1].name = _strdup(ucr::toUTF8(path2).c_str());
	}

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
		const TCHAR *mode = (m_bAppendFiles ? _T("a+") : _T("w+"));
		if (_tfopen_s(&outfile, m_sPatchFile.c_str(), mode) != 0)
			outfile = nullptr;
	}

	if (outfile == nullptr)
	{
		m_status.bPatchFileFailed = true;
		return;
	}

	// Print "command line"
	if (m_bAddCmdLine && output_style != OUTPUT_HTML)
	{
		String switches = FormatSwitchString();
		_ftprintf(outfile, _T("diff%s %s %s\n"),
			switches.c_str(), 
			path1 == _T("NUL") ? _T("/dev/null") : path1.c_str(),
			path2 == _T("NUL") ? _T("/dev/null") : path2.c_str());
	}

	if (strcmp(inf[0].name, "NUL") == 0)
	{
		free((void *)inf[0].name);
		inf[0].name = _strdup("/dev/null");
	}
	if (strcmp(inf[1].name, "NUL") == 0)
	{
		free((void *)inf[1].name);
		inf[1].name = _strdup("/dev/null");
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

/**
 * @brief Set line filters, given as one string.
 * @param [in] filterStr Filters.
 */
void CDiffWrapper::SetFilterList(const String& filterStr)
{
	// Remove filterlist if new filter is empty
	if (filterStr.empty())
	{
		m_pFilterList.reset();
		return;
	}

	// Adding new filter without previous filter
	if (m_pFilterList == nullptr)
	{
		m_pFilterList.reset(new FilterList);
	}

	m_pFilterList->RemoveAllFilters();

	std::string regexp_str = ucr::toUTF8(filterStr);

	// Add every "line" of regexps to regexp list
	StringTokenizer tokens(regexp_str, "\r\n");
	for (StringTokenizer::Iterator it = tokens.begin(); it != tokens.end(); ++it)
		m_pFilterList->AddRegExp(*it);
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
