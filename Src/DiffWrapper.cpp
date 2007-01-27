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
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include <string>
#include <map>
#include <shlwapi.h>
#include "Ucs2Utf8.h"
#include "coretools.h"
#include "diffcontext.h"
#include "DiffList.h"
#include "MovedLines.h"
#include "FilterList.h"
#include "diffwrapper.h"
#include "diff.h"
#include "FileTransform.h"
#include "LogFile.h"
#include "paths.h"
#include "CompareOptions.h"
#include "FileTextStats.h"
//#include "DiffFileData.h"
#include "FolderCmp.h"
#include "FilterCommentsManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int recursive;
extern CLogFile gLog;

static void FreeDiffUtilsScript(struct change * & script);
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
, m_infoPrediffer(NULL)
, m_pDiffList(NULL)
, m_bPathsAreTemp(FALSE)
, m_pMovedLines(NULL)
, m_pFilterList(NULL)
{
	ZeroMemory(&m_settings, sizeof(DIFFSETTINGS));
	ZeroMemory(&m_globalSettings, sizeof(DIFFSETTINGS));
	ZeroMemory(&m_status, sizeof(DIFFSTATUS));
	m_settings.heuristic = 1;
	m_settings.outputStyle = OUTPUT_NORMAL;
	m_settings.context = -1;

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
	delete m_pMovedLines;
}

/**
 * @brief Enables/disables patch-file creation and sets filename.
 * This function enables or disables patch file creation. When
 * @p filename is empty, patch files are disabled.
 * @param [in] filename Filename for patch file, or empty string.
 */
void CDiffWrapper::SetCreatePatchFile(const CString &filename)
{
	if (filename.IsEmpty())
	{
		m_bCreatePatchFile = FALSE;
		m_sPatchFile.Empty();
	}
	else
	{
		m_bCreatePatchFile = TRUE;
		m_sPatchFile = filename;
		m_sPatchFile.Replace('/', '\\');
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
void CDiffWrapper::GetOptions(DIFFOPTIONS *options) const
{
	ASSERT(options);
	InternalGetOptions(options);
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
	InternalSetOptions(options);
}

/**
 * @brief Set text tested to find the prediffer automatically.
 * Most probably a concatenated string of both filenames.
 */
void CDiffWrapper::SetTextForAutomaticPrediff(const CString &text)
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
	m_settings.context = options->nContext;
	m_settings.outputStyle = options->outputStyle;
	m_bAddCmdLine = options->bAddCommandline;
}

/**
 * @brief Enables/disables moved block detection.
 * @param [in] bDetectMovedBlocks If TRUE moved blocks are detected.
 */
void CDiffWrapper::SetDetectMovedBlocks(BOOL bDetectMovedBlocks)
{
	if (bDetectMovedBlocks)
	{
		if (m_pMovedLines == NULL)
			m_pMovedLines = new MovedLines;
	}
	else
	{
		delete m_pMovedLines;
		m_pMovedLines = NULL;
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
	@param[in]  StartPos			- First line number to read
	@param[in]  EndPos				- The line number PASS the last line number to read
	@param[in]  QtyLinesInBlock		- Number of lines in diff block.  Not needed in backward direction.
	@param[in]  Direction			- This should be 1 or -1, to indicate which direction to read (backward or forward)
	@param[in/out]  Op				- This variable is set to trivial if block should be ignored.
	@param[in]  FileNo				- Should be 0 or 1, to indicate left or right file.
	@param[in]  filtercommentsset	- Comment marker set used to indicate comment blocks.
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
		std::string LineData(files[FileNo].linbuf[i]);
		size_t EolPos = LineData.find_first_of(EolIndicators);
		if (EolPos != std::string::npos)
		{
			LineData.erase(EolPos);
		}

		int Len = LineData.size();
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
					std::string LineDataTrvCk(files[FileNo].linbuf[TrivLinePos]);
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
@param[in]  LineStr				- Line of string to check that must be NULL terminated.
@param[in/out]  Op				- This variable is set to trivial if block should be ignored.
@param[in]  filtercommentsset	- Comment marker set used to indicate comment blocks.
@param[in]  PartOfMultiLineCheck- Set to true, if this block is a multiple line block
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
@param[in]  LineNumberLeft		- First line number to read from left file
@param[in]  QtyLinesLeft		- Number of lines in the block for left file
@param[in]  LineNumberRight		- First line number to read from right file
@param[in]  QtyLinesRight		- Number of lines in the block for right file
@param[in/out]  Op				- This variable is set to trivial if block should be ignored.
@param[in]  filtercommentsset	- Comment marker set used to indicate comment blocks.
@param[in]  FileNameExt			- The file name extension.  Needs to be lower case string ("cpp", "java", "c")
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

	if (Op == OP_LEFTONLY)
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
	else if (Op == OP_RIGHTONLY)
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
 * @param [in] filepath1 First file to compare "original file".
 * @param [in] filepath2 Second file to compare "changed file".
 * @param [in] tempPaths Are given paths temporary (can be deleted)?.
 */
void CDiffWrapper::SetPaths(const CString &filepath1, const CString &filepath2,
		BOOL tempPaths)
{
	m_s1File = filepath1;
	m_s2File = filepath2;
	m_bPathsAreTemp = tempPaths;
}

/**
 * @brief Set source paths for original (NON-TEMP) diffing two files.
 * Sets full paths to two (NON-TEMP) files we are diffing.
 * @param [in] OriginalFile1 First file to compare "(NON-TEMP) file".
 * @param [in] OriginalFile2 Second file to compare "(NON-TEMP) file".
 */
void CDiffWrapper::SetCompareFiles(const CString &OriginalFile1, const CString &OriginalFile2)
{
	m_sOriginalFile1 = OriginalFile1;
	m_sOriginalFile2 = OriginalFile2;
}

/**
 * @brief Set alternative paths for compared files.
 * Sets alternative paths for diff'ed files. These alternative paths might not
 * be real paths. For example when creating a patch file from folder compare
 * we want to use relative paths.
 * @param [in] altPath1 Alternative file path of first file.
 * @param [in] altPath2 Alternative file path of second file.
 */
void CDiffWrapper::SetAlternativePaths(const CString &altPath1, const CString &altPath2)
{
	m_s1AlternativePath = altPath1;
	m_s2AlternativePath = altPath2;
}

/**
 * @brief Runs diff-engine.
 */
BOOL CDiffWrapper::RunFileDiff()
{
	CString filepath1(m_s1File);
	CString filepath2(m_s2File);
	filepath1.Replace('/', '\\');
	filepath2.Replace('/', '\\');

	BOOL bRet = TRUE;
	USES_CONVERSION;
	CString strFile1Temp = filepath1;
	CString strFile2Temp = filepath2;
	SwapToInternalSettings();

	if (m_bUseDiffList)
		m_nDiffs = m_pDiffList->GetSize();

	// Do the preprocessing now, overwrite the temp files
	// NOTE: FileTransform_UCS2ToUTF8() may create new temp
	// files and return new names, those created temp files
	// are deleted in end of function.
	if (m_infoPrediffer->bToBeScanned)
	{
		// this can only fail if the data can not be saved back (no more
		// place on disk ???) What to do then ??
		FileTransform_Prediffing(strFile1Temp, m_sToFindPrediffer, m_infoPrediffer,
			m_bPathsAreTemp);
	}
	else
	{
		// This can fail if the prediffer has a problem
		if (FileTransform_Prediffing(strFile1Temp, *m_infoPrediffer,
			m_bPathsAreTemp) == FALSE)
		{
			// display a message box
			CString sError;
			AfxFormatString2(sError, IDS_PREDIFFER_ERROR, strFile1Temp,
				m_infoPrediffer->pluginName);
			AfxMessageBox(sError, MB_OK | MB_ICONSTOP);
			// don't use any more this prediffer
			m_infoPrediffer->bToBeScanned = FALSE;
			m_infoPrediffer->pluginName.Empty();
		}
	}

	FileTransform_UCS2ToUTF8(strFile1Temp, m_bPathsAreTemp);
	// We use the same plugin for both files, so it must be defined before
	// second file
	ASSERT(m_infoPrediffer->bToBeScanned == FALSE);
	if (FileTransform_Prediffing(strFile2Temp, *m_infoPrediffer,
		m_bPathsAreTemp) == FALSE)
	{
		// display a message box
		CString sError;
		AfxFormatString2(sError, IDS_PREDIFFER_ERROR, strFile2Temp,
			m_infoPrediffer->pluginName);
		AfxMessageBox(sError, MB_OK | MB_ICONSTOP);
		// don't use any more this prediffer
		m_infoPrediffer->bToBeScanned = FALSE;
		m_infoPrediffer->pluginName = _T("");
	}
	FileTransform_UCS2ToUTF8(strFile2Temp, m_bPathsAreTemp);

	DiffFileData diffdata;
	diffdata.SetDisplayFilepaths(filepath1, filepath2); // store true names for diff utils patch file
	// This opens & fstats both files (if it succeeds)
	if (!diffdata.OpenFiles(strFile1Temp, strFile2Temp))
	{
		return FALSE;
	}

	// Compare the files, if no error was found.
	// Last param (bin_file) is NULL since we don't
	// (yet) need info about binary sides.
	int bin_flag = 0;
	struct change *script = NULL;
	bRet = Diff2Files(&script, &diffdata, &bin_flag, NULL);

	// We don't anymore create diff-files for every rescan.
	// User can create patch-file whenever one wants to.
	// We don't need to waste time. But lets keep this as
	// debugging aid. Sometimes it is very useful to see
	// what differences diff-engine sees!
#ifdef _DEBUG
	// throw the diff into a temp file
	CString sTempPath = paths_GetTempPath(); // get path to Temp folder
	CString path = paths_ConcatPath(sTempPath, _T("Diff.txt"));

	outfile = _tfopen(path, _T("w+"));
	if (outfile != NULL)
	{
		print_normal_script(script);
		fclose(outfile);
		outfile = NULL;
	}
#endif

	// First determine what happened during comparison
	// If there were errors or files were binaries, don't bother
	// creating diff-lists or patches
	
	// diff_2_files set bin_flag to -1 if different binary
	// diff_2_files set bin_flag to +1 if same binary
	if (bin_flag != 0)
	{
		m_status.bBinaries = TRUE;
		if (bin_flag == -1)
			m_status.bIdentical = FALSE;
		else
			m_status.bIdentical = TRUE;
	}
	else
	{ // text files according to diffutils, so change script exists
		m_status.bIdentical = (script == 0);
		m_status.bBinaries = FALSE;
	}
	file_data * inf = diffdata.m_inf;
	m_status.bLeftMissingNL = inf[0].missing_newline;
	m_status.bRightMissingNL = inf[1].missing_newline;


	// Create patch file
	if (!m_status.bBinaries && m_bCreatePatchFile)
	{
		WritePatchFile(script, &inf[0]);
	}
	
	// Go through diffs adding them to WinMerge's diff list
	// This is done on every WinMerge's doc rescan!
	if (!m_status.bBinaries && m_bUseDiffList)
	{
		LoadWinMergeDiffsFromDiffUtilsScript(script, diffdata.m_inf);
	}			

	FreeDiffUtilsScript(script);

	// Done with diffutils filedata
	diffdata.Close();

	// Delete temp files transformation functions possibly created
	if (filepath1.CompareNoCase(strFile1Temp) != 0)
	{
		if (!::DeleteFile(strFile1Temp))
		{
			LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
				strFile1Temp, GetSysError(GetLastError())));
		}
		strFile1Temp.Empty();
	}
	if (filepath2.CompareNoCase(strFile2Temp) != 0)
	{
		if (!::DeleteFile(strFile2Temp))
		{
			LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
				strFile2Temp, GetSysError(GetLastError())));
		}
		strFile2Temp.Empty();
	}

	SwapToGlobalSettings();
	return bRet;
}

/**
 * @brief Return current diffutils options
 */
void CDiffWrapper::InternalGetOptions(DIFFOPTIONS *options) const
{
	int nIgnoreWhitespace = 0;

	if (m_settings.ignoreAllSpace)
		nIgnoreWhitespace = WHITESPACE_IGNORE_ALL;
	else if (m_settings.ignoreSpaceChange)
		nIgnoreWhitespace = WHITESPACE_IGNORE_CHANGE;

	options->nIgnoreWhitespace = nIgnoreWhitespace;
	options->bIgnoreBlankLines = m_settings.ignoreBlankLines;
	options->bFilterCommentsLines = m_settings.filterCommentsLines;
	options->bIgnoreCase = m_settings.ignoreCase;
	options->bIgnoreEol = m_settings.ignoreEOLDiff;

}

/**
 * @brief Set diffutils options
 */
void CDiffWrapper::InternalSetOptions(const DIFFOPTIONS *options)
{
	m_settings.ignoreAllSpace = (options->nIgnoreWhitespace == WHITESPACE_IGNORE_ALL);
	m_settings.ignoreSpaceChange = (options->nIgnoreWhitespace == WHITESPACE_IGNORE_CHANGE);
	m_settings.ignoreBlankLines = options->bIgnoreBlankLines;
	m_settings.filterCommentsLines = options->bFilterCommentsLines;
	m_settings.ignoreEOLDiff = options->bIgnoreEol;
	m_settings.ignoreCase = options->bIgnoreCase;
	m_settings.ignoreSomeChanges = (options->nIgnoreWhitespace != WHITESPACE_COMPARE_ALL) ||
		options->bIgnoreCase || options->bIgnoreBlankLines ||
		options->bIgnoreEol;
	m_settings.lengthVaries = (options->nIgnoreWhitespace != WHITESPACE_COMPARE_ALL);
}

/**
 * @brief Replaces global options used by diff-engine with options in diff-wrapper
 */
void CDiffWrapper::SwapToInternalSettings()
{
	// Save current settings to temp variables
	m_globalSettings.outputStyle = output_style;
	output_style = m_settings.outputStyle;
	
	m_globalSettings.context = context;
	context = m_settings.context;
	
	m_globalSettings.alwaysText = always_text_flag;
	always_text_flag = m_settings.alwaysText;

	m_globalSettings.horizLines = horizon_lines;
	horizon_lines = m_settings.horizLines;

	m_globalSettings.ignoreSpaceChange = ignore_space_change_flag;
	ignore_space_change_flag = m_settings.ignoreSpaceChange;

	m_globalSettings.ignoreAllSpace = ignore_all_space_flag;
	ignore_all_space_flag = m_settings.ignoreAllSpace;

	m_globalSettings.ignoreBlankLines = ignore_blank_lines_flag;
	ignore_blank_lines_flag = m_settings.ignoreBlankLines;

	m_globalSettings.ignoreCase = ignore_case_flag;
	ignore_case_flag = m_settings.ignoreCase;

	m_globalSettings.ignoreEOLDiff = ignore_eol_diff;
	ignore_eol_diff = m_settings.ignoreEOLDiff;

	m_globalSettings.ignoreSomeChanges = ignore_some_changes;
	ignore_some_changes = m_settings.ignoreSomeChanges;

	m_globalSettings.lengthVaries = length_varies;
	length_varies = m_settings.lengthVaries;

	m_globalSettings.heuristic = heuristic;
	heuristic = m_settings.heuristic;

	m_globalSettings.recursive = recursive;
	recursive = m_settings.recursive;
}

/**
 * @brief Resumes global options as they were before calling SwapToInternalOptions()
 */
void CDiffWrapper::SwapToGlobalSettings()
{
	// Resume values
	output_style = m_globalSettings.outputStyle;
	context = m_globalSettings.context;
	always_text_flag = m_globalSettings.alwaysText;
	horizon_lines = m_globalSettings.horizLines;
	ignore_space_change_flag = m_globalSettings.ignoreSpaceChange;
	ignore_all_space_flag = m_globalSettings.ignoreAllSpace;
	ignore_blank_lines_flag = m_globalSettings.ignoreBlankLines;
	ignore_case_flag = m_globalSettings.ignoreCase;
	ignore_eol_diff = m_globalSettings.ignoreEOLDiff;
	ignore_some_changes = m_globalSettings.ignoreSomeChanges;
	length_varies = m_globalSettings.lengthVaries;
	heuristic = m_globalSettings.heuristic;
	recursive = m_globalSettings.recursive;
}

/**
 * @brief Add diff to external diff-list
 */
void CDiffWrapper::AddDiffRange(UINT begin0, UINT end0, UINT begin1, UINT end1, BYTE op)
{
	TRY {
		DIFFRANGE dr;
		dr.begin0 = begin0;
		dr.end0 = end0;
		dr.begin1 = begin1;
		dr.end1 = end1;
		dr.op = op;
		dr.blank0 = dr.blank1 = -1;
		m_pDiffList->AddDiff(dr);
		m_nDiffs++;
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
 */
void CDiffWrapper::FixLastDiffRange(int leftBufferLines, int rightBufferLines, BOOL left)
{
	DIFFRANGE dr;
	const int count = m_pDiffList->GetSize();
	if (count > 0)
	{
		m_pDiffList->GetDiff(count - 1, dr);

		if (left)
		{
			if (dr.op == OP_RIGHTONLY)
				dr.op = OP_DIFF;
			dr.end0++;
		}
		else
		{
			if (dr.op == OP_LEFTONLY)
				dr.op = OP_DIFF;
			dr.end1++;
		}
		m_pDiffList->SetDiff(count - 1, dr);
	}
	else 
	{
		// we have to create the DIFF
		dr.end0 = leftBufferLines - 1;
		dr.end1 = rightBufferLines - 1;
		if (left)
		{
			dr.begin0 = dr.end0;
			dr.begin1 = dr.end1 + 1;
			dr.op = OP_LEFTONLY;
		}
		else
		{
			dr.begin0 = dr.end0 + 1;
			dr.begin1 = dr.end1;
			dr.op = OP_RIGHTONLY;
		}
		ASSERT(dr.begin0 == dr.begin1);

		AddDiffRange(dr.begin0, dr.end0, dr.begin1, dr.end1, dr.op); 
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
CString CDiffWrapper::FormatSwitchString()
{
	CString switches;
	TCHAR tmpNum[5] = {0};
	
	switch (m_settings.outputStyle)
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

	if (m_settings.context > 0)
	{
		_itot(m_settings.context, tmpNum, 10);
		switches += tmpNum;
	}

	if (m_settings.ignoreAllSpace > 0)
		switches += _T("w");

	if (m_settings.ignoreBlankLines > 0)
		switches += _T("B");

	if (m_settings.ignoreCase > 0)
		switches += _T("i");

	if (m_settings.ignoreSpaceChange > 0)
		switches += _T("b");

	return switches;
}

/**
 * @brief Enables/disables patch-file appending (files with same filename are appended)
 */
BOOL CDiffWrapper::SetAppendFiles(BOOL bAppendFiles)
{
	BOOL temp = m_bAppendFiles;
	m_bAppendFiles = bAppendFiles;
	return temp;
}

/**
 * @brief Sets options for directory compare
 */
void CDiffWrapper::StartDirectoryDiff()
{
	SwapToInternalSettings();
}

/**
 * @brief resumes options after directory compare
 */
void CDiffWrapper::EndDirectoryDiff()
{
	SwapToGlobalSettings();
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
		// Diff files. depth is zero because we are not comparing dirs
		*diffs = diff_2_files (diffData->m_inf, 0, bin_status,
				(m_pMovedLines != NULL), bin_file);
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
		std::string LineData(files[FileNo].linbuf[line]);
		size_t EolPos = LineData.find_first_of(EolIndicators);
		if (EolPos != std::string::npos)
		{
			LineData.erase(EolPos);
		}

		if (!m_pFilterList->Match(LineData.c_str()))
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
	CString asLwrCaseExt;
	if (options.bFilterCommentsLines)
	{
		CString LowerCaseExt = m_sOriginalFile1;
		int PosOfDot = LowerCaseExt.ReverseFind('.');
		if (PosOfDot != -1)
		{
			LowerCaseExt = LowerCaseExt.Mid(PosOfDot+1);
			LowerCaseExt.MakeLower();
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
			analyze_hunk (thisob, &first0, &last0, &first1, &last1, &deletes, &inserts);
			int op=0;
			if (deletes || inserts || thisob->trivial)
			{
				if (deletes && inserts)
					op = OP_DIFF;
				else if (deletes)
					op = OP_LEFTONLY;
				else if (inserts)
					op = OP_RIGHTONLY;
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
							GetMovedLines()->Add(MovedLines::SIDE_RIGHT, line1, line0);
						}
					}
					if (thisob->match1>=0)
					{
						ASSERT(thisob->deleted);
						for (int i=0; i<thisob->deleted; ++i)
						{
							int line0 = i+thisob->line0 + (trans_a0-first0-1);
							int line1 = i+thisob->match1 + (trans_a1-first1-1);
							GetMovedLines()->Add(MovedLines::SIDE_LEFT, line0, line1);
						}
					}
				}

				if (options.bFilterCommentsLines)
				{
					int QtyLinesLeft = (trans_b0 - trans_a0) + 1; //Determine quantity of lines in this block for left side
					int QtyLinesRight = (trans_b1 - trans_a1) + 1;//Determine quantity of lines in this block for right side
					PostFilter(thisob->line0, QtyLinesLeft, thisob->line1, QtyLinesRight, op, *m_FilterCommentsManager, asLwrCaseExt);
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

				AddDiffRange(trans_a0-1, trans_b0-1, trans_a1-1, trans_b1-1, (BYTE)op);
				TRACE(_T("left=%d,%d   right=%d,%d   op=%d\n"),
					trans_a0-1, trans_b0-1, trans_a1-1, trans_b1-1, op);
			}
		}
		
		/* Reconnect the script so it will all be freed properly.  */
		end->link = next;
	}
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
	USES_CONVERSION;
	file_data inf_patch[2] = {0};
	CopyMemory(&inf_patch, inf, sizeof(file_data) * 2);
	
	// Get paths, primarily use alternative paths, only if they are empty
	// use full filepaths
	CString path1(m_s1AlternativePath);
	CString path2(m_s2AlternativePath);
	if (path1.IsEmpty())
		path1 = m_s1File;
	if (path2.IsEmpty())
		path2 = m_s2File;
	path1.Replace('\\', '/');
	path2.Replace('\\', '/');
	inf_patch[0].name = strdup(T2CA(path1));
	inf_patch[1].name = strdup(T2CA(path2));

	outfile = NULL;
	if (!m_sPatchFile.IsEmpty())
	{
		LPCTSTR mode = (m_bAppendFiles ? _T("a+") : _T("w+"));
		outfile = _tfopen(m_sPatchFile, mode);
	}

	if (!outfile)
	{
		m_status.bPatchFileFailed = TRUE;
		return;
	}

	// Print "command line"
	if (m_bAddCmdLine)
	{
		CString switches = FormatSwitchString();
		_ftprintf(outfile, _T("diff%s %s %s\n"),
			switches, path1, path2);
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
void CDiffWrapper::SetFilterList(const CString &filterStr)
{
	// Remove filterlist if new filter is empty
	if (filterStr.IsEmpty())
	{
		delete m_pFilterList;
		m_pFilterList = NULL;
		return;
	}

	// Adding new filter without previous filter
	if (!filterStr.IsEmpty() && m_pFilterList == NULL)
	{
		m_pFilterList = new FilterList;
	}

	m_pFilterList->RemoveAllFilters();
	if (!filterStr.IsEmpty())
	{
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
	CopyTextStats(&inf[0], &diffData->m_textStats0);
	CopyTextStats(&inf[1], &diffData->m_textStats1);
}
