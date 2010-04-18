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
// ID line follows -- this is updated by SVN
// $Id: DiffWrapper.cpp 7091 2010-01-11 20:27:43Z kimmov $

#include "StdAfx.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <map>
#include <shlwapi.h>
#include "Ucs2Utf8.h"
#include "coretools.h"
#include "DiffContext.h"
#include "DiffList.h"
#include "MovedLines.h"
#include "FilterList.h"
#include "DiffWrapper.h"
#include "DIFF.H"
#include "FileTransform.h"
#include "LogFile.h"
#include "paths.h"
#include "CompareOptions.h"
#include "FileTextStats.h"
#include "FolderCmp.h"
#include "FilterCommentsManager.h"
#include "Environment.h"
#include "PatchHTML.h"
#include "AnsiConvert.h"
#include "UnicodeString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int recursive;

static void FreeDiffUtilsScript(struct change * & script);
static void FreeDiffUtilsScript3(struct change * & script10, struct change * & script12, struct change * & script02);
static void CopyTextStats(const file_data * inf, FileTextStats * myTextStats);
static void CopyDiffutilTextStats(file_data *inf, DiffFileData * diffData);

// Postfiltering
static bool IsTrivialBytes(const char* Start, const char* End,
	const FilterCommentsSet& filtercommentsset);
static bool IsTrivialLine(const std::string &Line, const char * StartOfComment,
   const char * EndOfComment, const char * InLineComment,
   const FilterCommentsSet& filtercommentsset);
static bool PostFilter(int StartPos, int EndPos, int Direction,
	int QtyLinesInBlock, int &Op, int FileNo,
	const FilterCommentsSet& filtercommentsset);
static void PostFilterSingleLine(const char* LineStr, int &Op,
	const FilterCommentsSet& filtercommentsset, bool PartOfMultiLineCheck);
static void PostFilter(int LineNumberLeft, int QtyLinesLeft, int LineNumberRight,
	int QtyLinesRight, int &Op, const FilterCommentsManager &filtercommentsmanager,
	const TCHAR *FileNameExt);


/**
 * @brief Default constructor.
 * Initializes members and creates new FilterCommentsManager.
 */
CDiffWrapper::CDiffWrapper()
: m_FilterCommentsManager(new FilterCommentsManager)
, m_bCreatePatchFile(FALSE)
, m_bUseDiffList(FALSE)
, m_bAddCmdLine(TRUE)
, m_bAppendFiles(FALSE)
, m_nDiffs(0)
, m_codepage(GetACP())
, m_infoPrediffer(NULL)
, m_pDiffList(NULL)
, m_bPathsAreTemp(FALSE)
, m_pFilterList(NULL)
, m_bPluginsEnabled(false)
{
	ZeroMemory(&m_status, sizeof(DIFFSTATUS));

	m_pMovedLines[0] = NULL;
	m_pMovedLines[1] = NULL;
	m_pMovedLines[2] = NULL;

	// character that ends a line.  Currently this is always `\n'
	line_end_char = '\n';
}

/**
 * @brief Destructor.
 */
CDiffWrapper::~CDiffWrapper()
{
	delete m_pFilterList;
	delete m_infoPrediffer;
	delete m_FilterCommentsManager;
	delete m_pMovedLines[0];
	delete m_pMovedLines[1];
	delete m_pMovedLines[2];
}

/**
 * @brief Set plugins enabled/disabled.
 * @param [in] enable if true plugins are enabled.
 */
void CDiffWrapper::EnablePlugins(bool enable)
{
	m_bPluginsEnabled = enable;
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
		m_bCreatePatchFile = FALSE;
		m_sPatchFile.clear();
	}
	else
	{
		m_bCreatePatchFile = TRUE;
		m_sPatchFile = filename;
		string_replace(m_sPatchFile, _T("/"), _T("\\"));
	}
}

/**
 * @brief Enables/disabled DiffList creation ands sets DiffList.
 * This function enables or disables DiffList creation. When
 * @p diffList is NULL difflist is not created. When valid DiffList
 * pointer is given, compare results are stored into it.
 * @param [in] diffList Pointer to DiffList getting compare results.
 */
void CDiffWrapper::SetCreateDiffList(DiffList *diffList)
{
	if (diffList == NULL)
	{
		m_bUseDiffList = FALSE;
		m_pDiffList = NULL;
	}
	else
	{
		m_bUseDiffList = TRUE;
		m_pDiffList = diffList;
	}
}

/**
 * @brief Returns current set of options used by diff-engine.
 * This function converts internally used diff-options to
 * format used outside CDiffWrapper and returns them.
 * @param [in,out] options Pointer to structure getting used options.
 */
void CDiffWrapper::GetOptions(DIFFOPTIONS *options)
{
	ASSERT(options);
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
	ASSERT(options);
	m_options.SetFromDiffOptions(*options);
}

/**
 * @brief Set text tested to find the prediffer automatically.
 * Most probably a concatenated string of both filenames.
 */
void CDiffWrapper::SetTextForAutomaticPrediff(const String &text)
{
	m_sToFindPrediffer = text;
}
void CDiffWrapper::SetPrediffer(PrediffingInfo * prediffer /*=NULL*/)
{
	if (m_infoPrediffer)
		delete m_infoPrediffer;

	// all flags are set correctly during the construction
	m_infoPrediffer = new PrediffingInfo;

	if (prediffer)
		*m_infoPrediffer = *prediffer;
}
void CDiffWrapper::GetPrediffer(PrediffingInfo * prediffer)
{
	*prediffer = *m_infoPrediffer;
}

/**
 * @brief Set options used for patch-file creation.
 * @param [in] options Pointer to structure having new options.
 */
void CDiffWrapper::SetPatchOptions(const PATCHOPTIONS *options)
{
	ASSERT(options);
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
		_RPTF0(_CRT_ERROR, "Unknown output style!");
		break;
	}

	m_bAddCmdLine = options->bAddCommandline;
}

/**
 * @brief Enables/disables moved block detection.
 * @param [in] bDetectMovedBlocks If TRUE moved blocks are detected.
 */
void CDiffWrapper::SetDetectMovedBlocks(bool bDetectMovedBlocks)
{
	if (bDetectMovedBlocks)
	{
		if (m_pMovedLines[0] == NULL)
		{
			m_pMovedLines[0] = new MovedLines;
			m_pMovedLines[1] = new MovedLines;
			m_pMovedLines[2] = new MovedLines;
		}
	}
	else
	{
		delete m_pMovedLines[0];
		delete m_pMovedLines[1];
		delete m_pMovedLines[2];
		m_pMovedLines[0] = NULL;
		m_pMovedLines[1] = NULL;
		m_pMovedLines[2] = NULL;
	}
}

/**
 * @brief Test for trivial only characters in string
 * @param [in] Start				- Start position in string
 * @param [in] End					- One character pass the end position of the string
 * @param [in] filtercommentsset	- For future use to determine trivial bytes
 * @return Returns true if all characters are trivial
 */
static bool IsTrivialBytes(const char* Start, const char* End,
	const FilterCommentsSet& filtercommentsset)
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
static bool IsTrivialLine(const std::string &Line, 
				   const char * StartOfComment,	
				   const char * EndOfComment,	
				   const char * InLineComment,	
				   const FilterCommentsSet& filtercommentsset)
{
	//Do easy test first
	if ((!StartOfComment || !EndOfComment) && !InLineComment)
		return false;//In no Start and End pair, and no single in-line set, then it's not trivial

	if (StartOfComment == Line.c_str() &&
		((EndOfComment + filtercommentsset.EndMarker.size()) - StartOfComment) == Line.size())
	{//If entire line is blocked by End and Start markers, then entire line is trivial
		return true;
	}

	if (InLineComment && InLineComment < StartOfComment)
	{
		if (InLineComment == Line.c_str())
			return true;//If line starts with InLineComment marker, then entire line is trivial

		//Other wise, check if data before InLineComment marker is trivial
		return IsTrivialBytes(Line.c_str(), InLineComment, filtercommentsset);
	}

	//Done with easy test, so now do more complex test
	if (StartOfComment && 
		EndOfComment && 
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
static bool PostFilter(int StartPos, int EndPos, int Direction,
	int QtyLinesInBlock, int &Op, int FileNo,
	const FilterCommentsSet& filtercommentsset)
{
	const char* EolIndicators = "\r\n"; //List of characters used as EOL
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
		std::string LineData(files[FileNo].linbuf[i], len);
		size_t EolPos = LineData.find_first_of(EolIndicators);
		if (EolPos != std::string::npos)
		{
			LineData.erase(EolPos);
		}

		const char * StartOfComment		= strstr(LineData.c_str(), filtercommentsset.StartMarker.c_str());
		const char * EndOfComment		= strstr(LineData.c_str(), filtercommentsset.EndMarker.c_str());
		const char * InLineComment		= strstr(LineData.c_str(), filtercommentsset.InlineMarker.c_str());
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

			if (EndOfComment && 
				(!StartOfComment || StartOfComment > EndOfComment) && 
				(!InLineComment || InLineComment > EndOfComment) )
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
					size_t len = files[FileNo].linbuf[TrivLinePos + 1] - files[FileNo].linbuf[TrivLinePos];
					std::string LineDataTrvCk(files[FileNo].linbuf[TrivLinePos], len);
					size_t EolPos = LineDataTrvCk.find_first_of(EolIndicators);
					if (EolPos != std::string::npos)
					{
						LineDataTrvCk.erase(EolPos);
					}
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
@brief Performs post-filtering on single line comments, by setting comment blocks to trivial
@param [in]  LineStr				- Line of string to check that must be NULL terminated.
@param [in,out]  Op				- This variable is set to trivial if block should be ignored.
@param [in]  filtercommentsset	- Comment marker set used to indicate comment blocks.
@param [in]  PartOfMultiLineCheck- Set to true, if this block is a multiple line block
*/
static void PostFilterSingleLine(const char* LineStr, int &Op,
	const FilterCommentsSet& filtercommentsset, bool PartOfMultiLineCheck)
{
	if (Op == OP_TRIVIAL)
		return;
	if (filtercommentsset.InlineMarker.empty())
	{//If filtercommentsset.InlineMarker is empty, then no support for single line comment
		return;
	}
	const char *	EndLine = strchr(LineStr, '\0');
	if (EndLine)
	{
		std::string LineData(LineStr, EndLine);
		if (LineData.empty() && PartOfMultiLineCheck)
		{
			Op = OP_TRIVIAL;
			return;
		}

		size_t CommentStr = LineData.find(filtercommentsset.InlineMarker);
		if (CommentStr == std::string::npos)
			return;
		if (!CommentStr)
		{//If it begins with comment string, then this is a trivial difference
			Op = OP_TRIVIAL;
			return;
		}
		//Consider adding code here to check if there's any significant code before the comment string
	}

}

/**
@brief The main entry for post filtering.  Performs post-filtering, by setting comment blocks to trivial
@param [in]  LineNumberLeft		- First line number to read from left file
@param [in]  QtyLinesLeft		- Number of lines in the block for left file
@param [in]  LineNumberRight		- First line number to read from right file
@param [in]  QtyLinesRight		- Number of lines in the block for right file
@param [in,out]  Op				- This variable is set to trivial if block should be ignored.
@param [in]  filtercommentsset	- Comment marker set used to indicate comment blocks.
@param [in]  FileNameExt			- The file name extension.  Needs to be lower case string ("cpp", "java", "c")
*/
static void PostFilter(int LineNumberLeft, int QtyLinesLeft, int LineNumberRight,
	int QtyLinesRight, int &Op, const FilterCommentsManager &filtercommentsmanager,
	const TCHAR *FileNameExt)
{
	if (Op == OP_TRIVIAL)
		return;
	
	//First we need to get lowercase file name extension
	FilterCommentsSet filtercommentsset = filtercommentsmanager.GetSetForFileType(FileNameExt);
	if (filtercommentsset.StartMarker.empty() && 
		filtercommentsset.EndMarker.empty() &&
		filtercommentsset.InlineMarker.empty())
	{
		return;
	}

	if (QtyLinesRight == 0)
	{//Only check left side
		if (PostFilter(LineNumberLeft, files[0].valid_lines, 1, QtyLinesLeft, Op, 0, filtercommentsset))
		{
			PostFilter(LineNumberLeft, -1, -1, QtyLinesLeft, Op, 0, filtercommentsset);
		}
		
		if (Op != OP_TRIVIAL && !filtercommentsset.InlineMarker.empty())
		{
			bool AllLinesAreComments = true;
			for(int i = LineNumberLeft;i < LineNumberLeft + QtyLinesLeft;++i)
			{
				int TestOp = 0;
				PostFilterSingleLine(files[0].linbuf[i], TestOp, filtercommentsset, QtyLinesLeft > 1);
				if (TestOp != OP_TRIVIAL)
				{
					AllLinesAreComments = false;
					break;
				}
			}

			if (AllLinesAreComments)
				Op = OP_TRIVIAL;
		}
	}
	else if (QtyLinesLeft == 0)
	{//Only check right side
		if (PostFilter(LineNumberRight, files[1].valid_lines, 1, QtyLinesRight, Op, 1, filtercommentsset))
		{
			PostFilter(LineNumberRight, -1, -1, QtyLinesRight, Op, 1, filtercommentsset);
		}

		if (Op != OP_TRIVIAL && !filtercommentsset.InlineMarker.empty())
		{
			bool AllLinesAreComments = true;
			for(int i = LineNumberRight;i < LineNumberRight + QtyLinesRight;++i)
			{
				int TestOp = 0;
				PostFilterSingleLine(files[1].linbuf[i], TestOp, filtercommentsset, QtyLinesRight > 1);
				if (TestOp != OP_TRIVIAL)
				{
					AllLinesAreComments = false;
					break;
				}
			}

			if (AllLinesAreComments)
				Op = OP_TRIVIAL;
		}
	}
	else
	{
		int LeftOp = 0;
		if (PostFilter(LineNumberLeft, files[0].valid_lines, 1, QtyLinesLeft, LeftOp, 0, filtercommentsset))
			PostFilter(LineNumberLeft, -1, -1, QtyLinesLeft, LeftOp, 0, filtercommentsset);

		int RightOp = 0;
		if (PostFilter(LineNumberRight, files[1].valid_lines, 1, QtyLinesRight, RightOp, 1, filtercommentsset))
			PostFilter(LineNumberRight, -1, -1, QtyLinesRight, RightOp, 1, filtercommentsset);

		if (LeftOp == OP_TRIVIAL && RightOp == OP_TRIVIAL)
			Op = OP_TRIVIAL;
		else if (!filtercommentsset.InlineMarker.empty() && QtyLinesLeft == 1 && QtyLinesRight == 1)
		{
			//Lets test if only a post line comment is different.
			const char *	LineStrLeft = files[0].linbuf[LineNumberLeft];
			const char *	EndLineLeft = strchr(LineStrLeft, '\0');
			const char *	LineStrRight = files[1].linbuf[LineNumberRight];
			const char *	EndLineRight = strchr(LineStrRight, '\0');
			if (EndLineLeft && EndLineRight)
			{
				std::string LineDataLeft(LineStrLeft, EndLineLeft);
				std::string LineDataRight(LineStrRight, EndLineRight);
				size_t CommentStrLeft = LineDataLeft.find(filtercommentsset.InlineMarker);
				size_t CommentStrRight = LineDataRight.find(filtercommentsset.InlineMarker);
				//If neither side has comment string, then lets assume significant difference, and return
				if (CommentStrLeft == std::string::npos && CommentStrRight == std::string::npos)
				{
					return;
				}
				//Do a quick test to see if both sides begin with comment character
				if (!CommentStrLeft && !CommentStrRight)
				{//If both sides begin with comment character, then this is a trivial difference
					Op = OP_TRIVIAL;
					return;
				}

				//Lets remove comments, and see if lines are equal
				if (CommentStrLeft != std::string::npos)
					LineDataLeft.erase(CommentStrLeft);
				if (CommentStrRight != std::string::npos)
					LineDataRight.erase(CommentStrRight);
				if (LineDataLeft == LineDataRight)
				{//If they're equal now, then only difference is comments, and that's a trivial difference
					Op = OP_TRIVIAL;
					return;
				}
			}
		}
	}
}

/**
 * @brief Set source paths for diffing two files.
 * Sets full paths to two files we are diffing. Paths can be actual user files
 * or temporary copies of user files. Parameter @p tempPaths tells if paths
 * are temporary paths that can be deleted.
 * @param [in] files Files to compare
 * @param [in] tempPaths Are given paths temporary (can be deleted)?.
 */
void CDiffWrapper::SetPaths(const PathContext &files,
		BOOL tempPaths)
{
	m_files = files;
	m_bPathsAreTemp = tempPaths;
}

/**
 * @brief Set source paths for original (NON-TEMP) diffing two files.
 * Sets full paths to two (NON-TEMP) files we are diffing.
 * @param [in] OriginalFile1 First file to compare "(NON-TEMP) file".
 * @param [in] OriginalFile2 Second file to compare "(NON-TEMP) file".
 */
void CDiffWrapper::SetCompareFiles(const PathContext &originalFile)
{
	m_originalFile = originalFile;
}

/**
 * @brief Set alternative paths for compared files.
 * Sets alternative paths for diff'ed files. These alternative paths might not
 * be real paths. For example when creating a patch file from folder compare
 * we want to use relative paths.
 * @param [in] altPaths Alternative file paths.
 */
void CDiffWrapper::SetAlternativePaths(const PathContext &altPaths)
{
	m_alternativePaths = altPaths;
}

/**
 * @brief Runs diff-engine.
 */
BOOL CDiffWrapper::RunFileDiff()
{
	PathContext files = m_files;
	int file;
	for (file = 0; file < m_files.GetSize(); file++)
		replace_char(&*files[file].begin(), '/', '\\');

	BOOL bRet = TRUE;
	String strFileTemp[3];
	for (file = 0; file < m_files.GetSize(); file++)
		strFileTemp[file] = files[file];

	
	m_options.SetToDiffUtils();

	if (m_bUseDiffList)
		m_nDiffs = m_pDiffList->GetSize();

	for (file = 0; file < files.GetSize(); file++)
	{
		if (m_bPluginsEnabled)
		{
			// Do the preprocessing now, overwrite the temp files
			// NOTE: FileTransform_UCS2ToUTF8() may create new temp
			// files and return new names, those created temp files
			// are deleted in end of function.
			if (m_infoPrediffer->bToBeScanned)
			{
				// this can only fail if the data can not be saved back (no more
				// place on disk ???) What to do then ??
				FileTransform_Prediffing(strFileTemp[file], m_sToFindPrediffer.c_str(), m_infoPrediffer,
					m_bPathsAreTemp);
			}
			else
			{
				// This can fail if the prediffer has a problem
				if (FileTransform_Prediffing(strFileTemp[file], *m_infoPrediffer,
					m_bPathsAreTemp) == FALSE)
				{
					// display a message box
					CString sError;
					LangFormatString2(sError, IDS_PREDIFFER_ERROR, strFileTemp[file].c_str(),
						m_infoPrediffer->pluginName.c_str());
					AfxMessageBox(sError, MB_OK | MB_ICONSTOP);
					// don't use any more this prediffer
					m_infoPrediffer->bToBeScanned = FALSE;
					m_infoPrediffer->pluginName.erase();
				}
			}
		}

		FileTransform_UCS2ToUTF8(strFileTemp[file], m_bPathsAreTemp);
		// We use the same plugin for both files, so it must be defined before
		// second file
		ASSERT(m_infoPrediffer->bToBeScanned == FALSE);
	}

	struct change *script = NULL;
	struct change *script10 = NULL;
	struct change *script12 = NULL;
	struct change *script02 = NULL;
	DiffFileData diffdata, diffdata10, diffdata12, diffdata02;
	int bin_flag = 0, bin_flag10 = 0, bin_flag12 = 0, bin_flag02 = 0;

	if (files.GetSize() == 2)
	{
		diffdata.SetDisplayFilepaths(files[0].c_str(), files[1].c_str()); // store true names for diff utils patch file
		// This opens & fstats both files (if it succeeds)
		if (!diffdata.OpenFiles(strFileTemp[0].c_str(), strFileTemp[1].c_str()))
		{
			return FALSE;
		}

		// Compare the files, if no error was found.
		// Last param (bin_file) is NULL since we don't
		// (yet) need info about binary sides.
		bRet = Diff2Files(&script, &diffdata, &bin_flag, NULL);

		// We don't anymore create diff-files for every rescan.
		// User can create patch-file whenever one wants to.
		// We don't need to waste time. But lets keep this as
		// debugging aid. Sometimes it is very useful to see
		// what differences diff-engine sees!
#ifdef _DEBUG
		// throw the diff into a temp file
		String sTempPath = env_GetTempPath(); // get path to Temp folder
		String path = paths_ConcatPath(sTempPath, _T("Diff.txt"));

		outfile = _tfopen(path.c_str(), _T("w+"));
		if (outfile != NULL)
		{
			print_normal_script(script);
			fclose(outfile);
			outfile = NULL;
		}
#endif
	}
	else
	{
		diffdata10.SetDisplayFilepaths(files[1].c_str(), files[0].c_str()); // store true names for diff utils patch file
		diffdata02.SetDisplayFilepaths(files[0].c_str(), files[2].c_str()); // store true names for diff utils patch file
		diffdata12.SetDisplayFilepaths(files[1].c_str(), files[2].c_str()); // store true names for diff utils patch file

		if (!diffdata10.OpenFiles(strFileTemp[1].c_str(), strFileTemp[0].c_str()))
		{
			return FALSE;
		}

		bRet = Diff2Files(&script10, &diffdata10, &bin_flag10, NULL);

		if (!diffdata12.OpenFiles(strFileTemp[1].c_str(), strFileTemp[2].c_str()))
		{
			return FALSE;
		}

		bRet = Diff2Files(&script12, &diffdata12, &bin_flag12, NULL);

		if (!diffdata02.OpenFiles(strFileTemp[0].c_str(), strFileTemp[2].c_str()))
		{
			return FALSE;
		}

		bRet = Diff2Files(&script02, &diffdata02, &bin_flag02, NULL);
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

	if (files.GetSize() == 2)
	{
		if (bin_flag != 0)
		{
			m_status.bBinaries = TRUE;
			if (bin_flag != -1)
				m_status.Identical = IDENTLEVEL_ALL;
			else
				m_status.Identical = IDENTLEVEL_NONE;
		}
		else
		{ // text files according to diffutils, so change script exists
			m_status.Identical = (script == 0) ? IDENTLEVEL_ALL : IDENTLEVEL_NONE;
			m_status.bBinaries = FALSE;
		}
		m_status.bMissingNL[0] = inf[0].missing_newline;
		m_status.bMissingNL[1] = inf[1].missing_newline;
	}
	else
	{
		m_status.Identical = IDENTLEVEL_NONE;
		if (bin_flag10 != 0 || bin_flag12 != 0 || bin_flag02 != 0)
		{
			m_status.bBinaries = TRUE;
			if (bin_flag10 != -1 && bin_flag12 != -1)
				m_status.Identical = IDENTLEVEL_ALL;
			else if (bin_flag10 != -1)
				m_status.Identical = IDENTLEVEL_EXCEPTRIGHT;
			else if (bin_flag12 != -1)
				m_status.Identical = IDENTLEVEL_EXCEPTLEFT;
			else if (bin_flag02 != -1)
				m_status.Identical = IDENTLEVEL_EXCEPTMIDDLE;
		}
		else
		{ // text files according to diffutils, so change script exists
			m_status.bBinaries = FALSE;
			if (script10 == 0 && script12 == 0)
				m_status.Identical = IDENTLEVEL_ALL;
			else if (script10 == 0)
				m_status.Identical = IDENTLEVEL_EXCEPTRIGHT;
			else if (script12 == 0)
				m_status.Identical = IDENTLEVEL_EXCEPTLEFT;
			else if (script02 == 0)
				m_status.Identical = IDENTLEVEL_EXCEPTMIDDLE;
		}
		m_status.bMissingNL[0] = inf02[0].missing_newline;
		m_status.bMissingNL[1] = inf12[0].missing_newline;
		m_status.bMissingNL[2] = inf12[1].missing_newline;
	}


	// Create patch file
	if (!m_status.bBinaries && m_bCreatePatchFile && files.GetSize() == 2)
	{
		WritePatchFile(script, &inf[0]);
	}
	
	// Go through diffs adding them to WinMerge's diff list
	// This is done on every WinMerge's doc rescan!
	if (!m_status.bBinaries && m_bUseDiffList)
	{
		if (files.GetSize() == 2)
			LoadWinMergeDiffsFromDiffUtilsScript(script, diffdata.m_inf);
		else
			LoadWinMergeDiffsFromDiffUtilsScript3(
				script10, script12, script02,
				diffdata10.m_inf, diffdata12.m_inf, diffdata02.m_inf);
	}			

	// cleanup the script
	if (files.GetSize() == 2)
		FreeDiffUtilsScript(script);
	else
		FreeDiffUtilsScript3(script10, script12, script02);

	// Done with diffutils filedata
	if (files.GetSize() == 2)
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
		for (file = 0; file < files.GetSize(); file++)
		{
			if (lstrcmpi(files[file].c_str(), strFileTemp[file].c_str()) != 0)
			{
				if (!::DeleteFile(strFileTemp[file].c_str()))
				{
					LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
						strFileTemp[file], GetSysError(GetLastError()).c_str()));
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
void CDiffWrapper::AddDiffRange(DiffList *pDiffList, UINT begin0, UINT end0, UINT begin1, UINT end1, BYTE op)
{
	TRY {
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
	CATCH_ALL(e)
	{
		TCHAR msg[1024] = {0};
		e->GetErrorMessage(msg, 1024);
		AfxMessageBox(msg, MB_ICONSTOP);
	}
	END_CATCH_ALL;
}

void CDiffWrapper::AddDiffRange(DiffList *pDiffList, DIFFRANGE &dr)
{
	TRY {
		pDiffList->AddDiff(dr);
	}
	CATCH_ALL(e)
	{
		TCHAR msg[1024] = {0};
		e->GetErrorMessage(msg, 1024);
		AfxMessageBox(msg, MB_ICONSTOP);
	}
	END_CATCH_ALL;
}

/**
 * @brief Expand last DIFFRANGE of file by one line to contain last line after EOL.
 * @param [in] leftBufferLines size of array pane left
 * @param [in] rightBufferLines size of array pane right
 * @param [in] left on whitch side we have to insert
 * @param [in] bIgnoreBlankLines, if true we allways add a new diff and make as trivial
 */
void CDiffWrapper::FixLastDiffRange(int nFiles, int bufferLines[], int bMissingNL[], bool bIgnoreBlankLines)
{
	DIFFRANGE dr;
	const int count = m_pDiffList->GetSize();
	if (count > 0 && !bIgnoreBlankLines)
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
			ASSERT(dr.begin[0] == dr.begin[file]);
		}
		if (bIgnoreBlankLines)
			dr.op = OP_TRIVIAL;

		AddDiffRange(m_pDiffList, dr); 
	}
}

/**
 * @brief Returns status-data from diff-engine last run
 */
void CDiffWrapper::GetDiffStatus(DIFFSTATUS *status)
{
	CopyMemory(status, &m_status, sizeof(DIFFSTATUS));
}

/**
 * @brief Formats command-line for diff-engine last run (like it was called from command-line)
 */
String CDiffWrapper::FormatSwitchString()
{
	String switches;
	TCHAR tmpNum[5] = {0};
	
	switch (m_options.m_outputStyle)
	{
	case OUTPUT_CONTEXT:
		switches = _T(" C");
		break;
	case OUTPUT_UNIFIED:
		switches = _T(" U");
		break;
	case OUTPUT_ED:
		switches = _T(" e");
		break;
	case OUTPUT_FORWARD_ED:
		switches = _T(" f");
		break;
	case OUTPUT_RCS:
		switches = _T(" n");
		break;
	case OUTPUT_NORMAL:
		switches = _T(" ");
		break;
	case OUTPUT_IFDEF:
		switches = _T(" D");
		break;
	case OUTPUT_SDIFF:
		switches = _T(" y");
		break;
	}

	if (m_options.m_contextLines > 0)
	{
		_itot(m_options.m_contextLines, tmpNum, 10);
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
 * @param [in] bAppendFiles If TRUE patch will be appended to existing file.
 */
void CDiffWrapper::SetAppendFiles(BOOL bAppendFiles)
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
    So if first file is binary, first bit is set etc. Can be NULL if binary file
    info is not needed (faster compare since diffutils don't bother checking
    second file if first is binary).
 * @return TRUE when compare succeeds, FALSE if error happened during compare.
 * @note This function is used in file compare, not folder compare. Similar
 * folder compare function is in DiffFileData.cpp.
 */
BOOL CDiffWrapper::Diff2Files(struct change ** diffs, DiffFileData *diffData,
	int * bin_status, int * bin_file)
{
	BOOL bRet = TRUE;
	__try
	{
		// Diff files. depth is zero because we are not 6comparing dirs
		*diffs = diff_2_files (diffData->m_inf, 0, bin_status,
				(m_pMovedLines[0] != NULL), bin_file);
		CopyDiffutilTextStats(diffData->m_inf, diffData);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		*diffs = NULL;
		bRet = FALSE;
	}
	return bRet;
}

/**
 * @brief Free script (the diffutils linked list of differences)
 */
static void
FreeDiffUtilsScript(struct change * & script)
{
	if (!script) return;
	struct change *e=0, *p=0;
	// cleanup the script
	for (e = script; e; e = p)
	{
		p = e->link;
		free(e);
	}
	script = 0;
}

/**
 * @brief Free script (the diffutils linked list of differences)
 */
static void
FreeDiffUtilsScript3(struct change * & script10, struct change * & script12, struct change * & script02)
{
	struct change *e=0, *p=0;
	for (e = script10; e; e = p)
	{
		p = e->link;
		free (e);
	}
	for (e = script12; e; e = p)
	{
		p = e->link;
		free (e);
	}
	for (e = script02; e; e = p)
	{
		p = e->link;
		free (e);
	}
}

void
CDiffWrapper::FreeDiffUtilsScript3(struct change * & script10, struct change * & script12, struct change * & script02)
{
	::FreeDiffUtilsScript3(script10, script12, script02);
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
bool CDiffWrapper::RegExpFilter(int StartPos, int EndPos, int FileNo)
{
	if (m_pFilterList == NULL)
	{	
		_RPTF0(_CRT_ERROR, "CDiffWrapper::RegExpFilter() called when "
			"filterlist doesn't exist (=NULL)");
		return false;
	}

	const char EolIndicators[] = "\r\n"; //List of characters used as EOL
	bool linesMatch = true; // set to false when non-matching line is found.
	int line = StartPos;

	while (line <= EndPos && linesMatch == true)
	{
		size_t len = files[FileNo].linbuf[line + 1] - files[FileNo].linbuf[line];
		std::string LineData(files[FileNo].linbuf[line], len);
		size_t EolPos = LineData.find_first_of(EolIndicators);
		if (EolPos != std::string::npos)
		{
			LineData.erase(EolPos);
		}

		if (!m_pFilterList->Match(LineData.c_str(), m_codepage))
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
		int PosOfDot = LowerCaseExt.rfind('.');
		if (PosOfDot != -1)
		{
			LowerCaseExt.erase(0, PosOfDot + 1);
			CharLower(&*LowerCaseExt.begin());
			asLwrCaseExt = LowerCaseExt;
		}
	}

	struct change *next = script;
	struct change *thisob=0, *end=0;
	
	while (next)
	{
		/* Find a set of changes that belong together.  */
		thisob = next;
		end = find_change(next);
		
		/* Disconnect them from the rest of the changes,
		making them a hunk, and remember the rest for next iteration.  */
		next = end->link;
		end->link = 0;
#ifdef DEBUG
		debug_script(thisob);
#endif

		/* Print thisob hunk.  */
		//(*printfun) (thisob);
		{					
			/* Determine range of line numbers involved in each file.  */
			int first0=0, last0=0, first1=0, last1=0, deletes=0, inserts=0;
			analyze_hunk (thisob, &first0, &last0, &first1, &last1, &deletes, &inserts, inf);
			int op=0;
			if (deletes || inserts || thisob->trivial)
			{
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
						ASSERT(thisob->inserted);
						for (int i=0; i<thisob->inserted; ++i)
						{
							int line0 = i+thisob->match0 + (trans_a0-first0-1);
							int line1 = i+thisob->line1 + (trans_a1-first1-1);
							GetMovedLines(1)->Add(MovedLines::SIDE_LEFT, line1, line0);
						}
					}
					if (thisob->match1>=0)
					{
						ASSERT(thisob->deleted);
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
					PostFilter(thisob->line0, QtyLinesLeft, thisob->line1, QtyLinesRight, op, *m_FilterCommentsManager, asLwrCaseExt.c_str());
				}

				if (m_pFilterList && m_pFilterList->HasRegExps())
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

				AddDiffRange(m_pDiffList, trans_a0-1, trans_b0-1, trans_a1-1, trans_b1-1, (BYTE)op);
				TRACE(_T("left=%d,%d   right=%d,%d   op=%d\n"),
					trans_a0-1, trans_b0-1, trans_a1-1, trans_b1-1, op);
			}
		}
		
		/* Reconnect the script so it will all be freed properly.  */
		end->link = next;
	}
}

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
	DiffList diff10, diff12, diff02, *pdiff;
	diff10.Clear();
	diff12.Clear();
	diff02.Clear();

	for (int file = 0; file < 3; file++)
	{
		struct change *next;
		int trans_a0, trans_b0, trans_a1, trans_b1;
		int first0, last0, first1, last1, deletes, inserts, op;
		struct change *thisob, *end;
		const file_data *pinf;

		switch (file)
		{
		case 0: next = script10; pdiff = &diff10; pinf = inf10; break;
		case 1: next = script12; pdiff = &diff12; pinf = inf12; break;
		case 2: next = script02; pdiff = &diff02; pinf = inf02; break;
		}

		while (next)
		{
			/* Find a set of changes that belong together.  */
			thisob = next;
			end = find_change(next);
			
			/* Disconnect them from the rest of the changes,
			making them a hunk, and remember the rest for next iteration.  */
			next = end->link;
			end->link = 0;
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
						int index1 = -1;
						int index2 = -1;
						MovedLines::ML_SIDE side1;
						MovedLines::ML_SIDE side2;
						if (file == 0 /* diff10 */)
						{
							index1 = 0;
							index2 = 1;
							side1 = MovedLines::SIDE_RIGHT;
							side2 = MovedLines::SIDE_LEFT;
						}
						else if (file == 1 /* diff12 */)
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
								ASSERT(thisob->inserted);
								for (int i=0; i<thisob->inserted; ++i)
								{
									int line0 = i+thisob->match0 + (trans_a0-first0-1);
									int line1 = i+thisob->line1 + (trans_a1-first1-1);
									GetMovedLines(index1)->Add(side1, line1, line0);
								}
							}
							if (thisob->match1>=0)
							{
								ASSERT(thisob->deleted);
								for (int i=0; i<thisob->deleted; ++i)
								{
									int line0 = i+thisob->line0 + (trans_a0-first0-1);
									int line1 = i+thisob->match1 + (trans_a1-first1-1);
									GetMovedLines(index2)->Add(side2, line0, line1);
								}
							}
						}
					}

					AddDiffRange(pdiff, trans_a0-1, trans_b0-1, trans_a1-1, trans_b1-1, (BYTE)op);
					TRACE(_T("left=%d,%d   right=%d,%d   op=%d\n"),
						trans_a0-1, trans_b0-1, trans_a1-1, trans_b1-1, op);
				}
			}
			
			/* Reconnect the script so it will all be freed properly.  */
			end->link = next;
		}
	}

	Make3wayDiff(*m_pDiffList, diff10, diff12, diff02);
}

void CDiffWrapper::WritePatchFileHeader(enum output_style output_style, BOOL bAppendFiles)
{
	outfile = NULL;
	if (!m_sPatchFile.empty())
	{
		LPCTSTR mode = (bAppendFiles ? _T("a+") : _T("w+"));
		outfile = _tfopen(m_sPatchFile.c_str(), mode);
	}

	if (!outfile)
	{
		m_status.bPatchFileFailed = TRUE;
		return;
	}

	// Output patchfile
	switch (output_style)
	{
	case OUTPUT_CONTEXT:
	case OUTPUT_UNIFIED:
	case OUTPUT_ED:
	case OUTPUT_FORWARD_ED:
	case OUTPUT_RCS:
	case OUTPUT_NORMAL:
	case OUTPUT_IFDEF:
	case OUTPUT_SDIFF:
		break;
	case OUTPUT_HTML:
		print_html_header();
		break;
	}
	
	fclose(outfile);
	outfile = NULL;
}

void CDiffWrapper::WritePatchFileTerminator(enum output_style output_style)
{
	outfile = NULL;
	if (!m_sPatchFile.empty())
	{
		outfile = _tfopen(m_sPatchFile.c_str(), _T("a+"));
	}

	if (!outfile)
	{
		m_status.bPatchFileFailed = TRUE;
		return;
	}

	// Output patchfile
	switch (output_style)
	{
	case OUTPUT_CONTEXT:
	case OUTPUT_UNIFIED:
	case OUTPUT_ED:
	case OUTPUT_FORWARD_ED:
	case OUTPUT_RCS:
	case OUTPUT_NORMAL:
	case OUTPUT_IFDEF:
	case OUTPUT_SDIFF:
		break;
	case OUTPUT_HTML:
		print_html_terminator();
		break;
	}
	
	fclose(outfile);
	outfile = NULL;
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
	file_data inf_patch[2] = {0};
	CopyMemory(&inf_patch, inf, sizeof(file_data) * 2);
	
	// Get paths, primarily use alternative paths, only if they are empty
	// use full filepaths
	String path1(m_alternativePaths[0]);
	String path2(m_alternativePaths[1]);
	if (path1.empty())
		path1 = m_files[0];
	if (path2.empty())
		path2 = m_files[1];
	replace_char(&*path1.begin(), '\\', '/');
	replace_char(&*path2.begin(), '\\', '/');
	inf_patch[0].name = ansiconvert_SystemCP(path1.c_str());
	inf_patch[1].name = ansiconvert_SystemCP(path2.c_str());

	// Fix timestamps for generated patch files
	// If there are translations needed (e.g. when comparing UTF-16 files)
	// then the stats in 'inf' are read from temp files. If the original
	// file's and read timestamps differ, use original file's timestamps.
	// See also sf.net bug item #2791506.
	struct __stat64 st;
	_tstat64(path1.c_str(), &st);
	if (st.st_mtime != inf_patch[0].stat.st_mtime)
		inf_patch[0].stat.st_mtime = st.st_mtime;
	_tstat64(path2.c_str(), &st);
	if (st.st_mtime != inf_patch[1].stat.st_mtime)
		inf_patch[1].stat.st_mtime = st.st_mtime;

	outfile = NULL;
	if (!m_sPatchFile.empty())
	{
		LPCTSTR mode = (m_bAppendFiles ? _T("a+") : _T("w+"));
		outfile = _tfopen(m_sPatchFile.c_str(), mode);
	}

	if (!outfile)
	{
		m_status.bPatchFileFailed = TRUE;
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
		inf[0].name = strdup("/dev/null");
	}
	if (strcmp(inf[1].name, "NUL") == 0)
	{
		free((void *)inf[1].name);
		inf[1].name = strdup("/dev/null");
	}

	// Output patchfile
	switch (output_style)
	{
	case OUTPUT_CONTEXT:
		print_context_header(inf_patch, 0);
		print_context_script(script, 0);
		break;
	case OUTPUT_UNIFIED:
		print_context_header(inf_patch, 1);
		print_context_script(script, 1);
		break;
	case OUTPUT_ED:
		print_ed_script(script);
		break;
	case OUTPUT_FORWARD_ED:
		pr_forward_ed_script(script);
		break;
	case OUTPUT_RCS:
		print_rcs_script(script);
		break;
	case OUTPUT_NORMAL:
		print_normal_script(script);
		break;
	case OUTPUT_IFDEF:
		print_ifdef_script(script);
		break;
	case OUTPUT_SDIFF:
		print_sdiff_script(script);
		break;
	case OUTPUT_HTML:
		print_html_diff_header(inf_patch);
		print_html_script(script);
		print_html_diff_terminator();
	}
	
	fclose(outfile);
	outfile = NULL;

	free((void *)inf_patch[0].name);
	free((void *)inf_patch[1].name);
}

/**
 * @brief Set line filters, given as one string.
 * @param [in] filterStr Filters.
 */
void CDiffWrapper::SetFilterList(LPCTSTR filterStr)
{
	// Remove filterlist if new filter is empty
	if (*filterStr == '\0')
	{
		delete m_pFilterList;
		m_pFilterList = NULL;
		return;
	}

	// Adding new filter without previous filter
	if (m_pFilterList == NULL)
	{
		m_pFilterList = new FilterList;
	}

	m_pFilterList->RemoveAllFilters();

	char * regexp_str;
	FilterList::EncodingType type;
	
#ifdef UNICODE
	regexp_str = UCS2UTF8_ConvertToUtf8(filterStr);
	type = FilterList::ENC_UTF8;
#else
	CString tmp_str(filterStr);
	regexp_str = tmp_str.LockBuffer();
	type = FilterList::ENC_ANSI;
#endif

	// Add every "line" of regexps to regexp list
	char * token;
	const char sep[] = "\r\n";
	token = strtok(regexp_str, sep);
	while (token)
	{
		m_pFilterList->AddRegExp(token, type);
		token = strtok(NULL, sep);
	}
#ifdef UNICODE
	UCS2UTF8_Dealloc(regexp_str);
#else
	tmp_str.UnlockBuffer();
#endif
}

/**
 * @brief Copy text stat results from diffutils back into the FileTextStats structure
 */
void CopyTextStats(const file_data * inf, FileTextStats * myTextStats)
{
	myTextStats->ncrlfs = inf->count_crlfs;
	myTextStats->ncrs = inf->count_crs;
	myTextStats->nlfs = inf->count_lfs;
}

/**
 * @brief Copy both left & right text stats results back into the DiffFileData text stats
 */
void CopyDiffutilTextStats(file_data *inf, DiffFileData * diffData)
{
	CopyTextStats(&inf[0], &diffData->m_textStats[0]);
	CopyTextStats(&inf[1], &diffData->m_textStats[1]);
}

/* diff3 algorithm. It is almost the same as GNU diff3's algorithm */
int CDiffWrapper::Make3wayDiff(DiffList& diff3, DiffList& diff10, DiffList& diff12, DiffList& diff02)
{
	int diff10count = diff10.GetSize();
	int diff12count = diff12.GetSize();
	int diff02count = diff02.GetSize();

	int diff10i = 0;
	int diff12i = 0;
	int diff02i = 0;
	int diff3i = 0;

	int diff10itmp;
	int diff12itmp;
	int diff02itmp = 0;

	bool lastDiffBlockIsDiff12;
	bool firstDiffBlockIsDiff12;

	DIFFRANGE dr3, dr10, dr12, dr02, dr10first, dr10last, dr12first, dr12last;
	dr3.blank[0] = dr3.blank[1] = dr3.blank[2] = -1;

	int linelast0 = 0;
	int linelast1 = 0;
	int linelast2 = 0;

	for (;;)
	{
		if (diff10i >= diff10count && diff12i >= diff12count)
			break;

		/* 
		 * merge overlapped diff blocks
		 * diff10 is diff blocks between file1 and file0.
		 * diff12 is diff blocks between file1 and file2.
		 *
		 *                      diff12
		 *                 diff10            diff3
		 *                 |~~~|             |~~~|
		 * firstDiffBlock  |   |             |   |
		 *                 |   | |~~~|       |   |
		 *                 |___| |   |       |   |
		 *                       |   |   ->  |   |
		 *                 |~~~| |___|       |   |
		 * lastDiffBlock   |   |             |   |
		 *                 |___|             |___|
		 */

		if (diff10i >= diff10count && diff12i < diff12count)
		{
			diff12.GetDiff(diff12i, dr12first);
			dr12last = dr12first;
			firstDiffBlockIsDiff12 = true;
		}
		else if (diff10i < diff10count && diff12i >= diff12count)
		{
			diff10.GetDiff(diff10i, dr10first);	
			dr10last = dr10first;
			firstDiffBlockIsDiff12 = false;
		}
		else
		{
			diff10.GetDiff(diff10i, dr10first);	
			diff12.GetDiff(diff12i, dr12first);
			dr10last = dr10first;
			dr12last = dr12first;
			if (dr12first.begin[0] <= dr10first.begin[0])
				firstDiffBlockIsDiff12 = true;
			else
				firstDiffBlockIsDiff12 = false;
		}
		lastDiffBlockIsDiff12 = firstDiffBlockIsDiff12;

		diff10itmp = diff10i;
		diff12itmp = diff12i;
		for (;;)
		{
			if (diff10itmp >= diff10count || diff12itmp >= diff12count)
				break;

			diff10.GetDiff(diff10itmp, dr10);
			diff12.GetDiff(diff12itmp, dr12);

			if (dr10.end[0] == dr12.end[0])
			{
				diff10itmp++;
				lastDiffBlockIsDiff12 = true;

				dr10last = dr10;
				dr12last = dr12;
				break;
			}

			if (lastDiffBlockIsDiff12)
			{
				if (dr12.end[0] + 1 < dr10.begin[0])
					break;
			}
			else
			{
				if (dr10.end[0] + 1 < dr12.begin[0])
					break;
			}

			if (dr12.end[0] > dr10.end[0])
			{
				diff10itmp++;
				lastDiffBlockIsDiff12 = true;
			}
			else
			{
				diff12itmp++;
				lastDiffBlockIsDiff12 = false;
			}

			dr10last = dr10;
			dr12last = dr12;
		}

		if (lastDiffBlockIsDiff12)
			diff12itmp++;
		else
			diff10itmp++;

		if (firstDiffBlockIsDiff12)
		{
			dr3.begin[1] = dr12first.begin[0];
			dr3.begin[2] = dr12first.begin[1];
			if (diff10itmp == diff10i)
				dr3.begin[0] = dr3.begin[1] - linelast1 + linelast0;
			else
				dr3.begin[0] = dr3.begin[1] - dr10first.begin[0] + dr10first.begin[1];
		}
		else
		{
			dr3.begin[0] = dr10first.begin[1];
			dr3.begin[1] = dr10first.begin[0];
			if (diff12itmp == diff12i)
				dr3.begin[2] = dr3.begin[1] - linelast1 + linelast2;
			else
				dr3.begin[2] = dr3.begin[1] - dr12first.begin[0] + dr12first.begin[1];
		}

		if (lastDiffBlockIsDiff12)
		{
			dr3.end[1] = dr12last.end[0];
			dr3.end[2] = dr12last.end[1];
			if (diff10itmp == diff10i)
				dr3.end[0] = dr3.end[1] - linelast1 + linelast0;
			else
				dr3.end[0] = dr3.end[1] - dr10last.end[0] + dr10last.end[1];
		}
		else
		{
			dr3.end[0] = dr10last.end[1];
			dr3.end[1] = dr10last.end[0];
			if (diff12itmp == diff12i)
				dr3.end[2] = dr3.end[1] - linelast1 + linelast2;
			else
				dr3.end[2] = dr3.end[1] - dr12last.end[0] + dr12last.end[1];
		}

		linelast0 = dr3.end[0] + 1;
		linelast1 = dr3.end[1] + 1;
		linelast2 = dr3.end[2] + 1;

		if (diff10i == diff10itmp)
			dr3.op = OP_3RDONLY;
		else if (diff12i == diff12itmp)
			dr3.op = OP_1STONLY;
		else 
		{
			dr3.op = OP_2NDONLY;
			for (diff02itmp = diff02i; diff02itmp < diff02count; diff02itmp++)
			{
				diff02.GetDiff(diff02itmp, dr02);
				if (dr02.end[1] < dr3.begin[2])
					continue;
				
				if (dr02.begin[1] <= dr3.end[2])
					dr3.op = OP_DIFF;
				break;
			}
		}

		if (ignore_regexp_list)
		{
			bool bTrivialDiff10 = true;
			bool bTrivialDiff12 = true;
			int i;

			for (i = diff10i; i < diff10itmp; i++)
			{
				diff10.GetDiff(i, dr10);
				if (dr10.op != OP_TRIVIAL)
				{
					bTrivialDiff10 = false;
					break;
				}
			}

			for (i = diff12i; i < diff12itmp; i++)
			{
				diff12.GetDiff(i, dr12);
				if (dr12.op != OP_TRIVIAL)
				{
					bTrivialDiff12 = false;
					break;
				}
			}

			if (bTrivialDiff10 && bTrivialDiff12)
				dr3.op = OP_TRIVIAL;
		}

		diff3.AddDiff(dr3);

		TRACE(_T("left=%d,%d middle=%d,%d right=%d,%d   op=%d\n"),
			dr3.begin[0], dr3.end[0], dr3.begin[1], dr3.end[1], dr3.begin[2], dr3.end[2], dr3.op);

		diff3i++;
		diff10i = diff10itmp;
		diff12i = diff12itmp;
		diff02i = diff02itmp;
	}
	return diff3i;
}
