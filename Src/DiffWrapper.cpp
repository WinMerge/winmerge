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
#include "coretools.h"
#include "common/unicoder.h"
#include "diffcontext.h"
#include "DiffList.h"
#include "diffwrapper.h"
#include "diff.h"
#include "FileTransform.h"
#include "LogFile.h"
#include "codepage.h"
#include "ByteComparator.h"
#include "codepage_detect.h"
#include "paths.h"
#include "IAbortable.h"
#include "CompareOptions.h"
#include "FileTextStats.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int recursive;
extern CLogFile gLog;
static int f_defcp = 0; // default codepage
static const int KILO = 1024; // Kilo(byte)

/**
 * @brief Quick contents compare's file buffer size
 */
static const int WMCMPBUFF = 32 * KILO;

static void GetComparePaths(CDiffContext * pCtxt, const DIFFITEM &di, CString & left, CString & right);
static void FreeDiffUtilsScript(struct change * & script);

//Should move FilterComments classes and global functions to it's own *.cpp & *.h file
//IngnoreComment logic developed by David Maisonave AKA (Axter)
/**
@struct FilterCommentsSet
@brief FilterCommentsSet holds search strings used to find comments in compared files.
		This data is used to find blocks that can be ignored when comparing to files.
@note
		The ignore-comment logic can only use ANSI strings, because the search buffer is
		char* type.
		Therefore, the data members should not be replaced with CString type, and should
		remain std::string, or other non-unicode type string.
*/
struct FilterCommentsSet
{
	std::string StartMarker;
	std::string EndMarker;
	std::string InlineMarker;
};


/**
@class FilterCommentsManager
@brief FilterCommentsManager reads language comment start and end marker strings from
		an INI file, and stores it in the map member variable m_FilterCommentsSetByFileType.
		Each set of comment markers have a list of file types that can be used with
		the file markers.
@note
The ignore-comment logic can only use ANSI strings, because the search buffer is
char* type.
FilterCommentsManager uses _T logic, only so-as to allow UNICODE file names to be 
used for the INI file, or INI file base directory.
After retrieving data from INI file, the data is converted to ANSI.
If no INI file exist, or the INI file is empty, then a default INI file is 
created with default values that are assoicated with most commen languages.
*/
class FilterCommentsManager
{
public:
	/**
	@brief FilterCommentsManager constructor, which reads the INI file data
			and populates the mapped member variable m_FilterCommentsSetByFileType.
	@param[in]  Optional full INI file name, to include path.
	*/
	FilterCommentsManager(const TCHAR* IniFileName = _T("")) : m_IniFileName(IniFileName)
	{
		USES_CONVERSION;

		int SectionNo = 0;
		TCHAR SectionName[99];
		TCHAR buffer[1024];
		if (m_IniFileName.IsEmpty())
		{
			m_IniFileName = GetModulePath() + _T("\\IgnoreSectionMarkers.ini");
		}
		for(SectionNo = 0;;++SectionNo) 
		{//Get each set of markers
			FilterCommentsSet filtercommentsset;
			_sntprintf(SectionName, sizeof(SectionName)/sizeof(SectionName[0]), _T("set%i"), SectionNo);
			GetPrivateProfileString(SectionName, _T("StartMarker"), _T(""), buffer,sizeof(buffer), m_IniFileName);
			filtercommentsset.StartMarker = T2CA(buffer);
			GetPrivateProfileString(SectionName, _T("EndMarker"), _T(""), buffer,sizeof(buffer), m_IniFileName);
			filtercommentsset.EndMarker = T2CA(buffer);
			GetPrivateProfileString(SectionName, _T("InlineMarker"), _T(""), buffer,sizeof(buffer), m_IniFileName);
			filtercommentsset.InlineMarker = T2CA(buffer);
			if (filtercommentsset.StartMarker.empty() && 
				filtercommentsset.EndMarker.empty() &&
				filtercommentsset.InlineMarker.empty())
			{
				break;
			}
			int FileTypeNo = 0;
			TCHAR FileTypeFieldName[99];
			for(FileTypeNo = 0;;++FileTypeNo) 
			{//Get each file type associated with current set of markers
				_sntprintf(FileTypeFieldName, sizeof(FileTypeFieldName)/sizeof(FileTypeFieldName[0]), _T("FileType%i"), FileTypeNo);
				GetPrivateProfileString(SectionName, FileTypeFieldName, _T(""), buffer,sizeof(buffer), m_IniFileName);
				CString FileTypeExtensionName = buffer;
				if (FileTypeExtensionName.IsEmpty())
					break;
				m_FilterCommentsSetByFileType[FileTypeExtensionName] = filtercommentsset;
			}
		} 

		if (!SectionNo)
		{//If no markers were found, then initialize default markers
			CreateDefaultMarkers();
		}
	}

	/**
		@brief Get comment markers that are associated with this file type.
			If there are no comment markers associated with this file type,
			then return an empty set.
		@param[in]  The file name extension. Example:("cpp", "java", "c", "h")
					Must be lower case.
	*/
	FilterCommentsSet GetSetForFileType(const CString& FileTypeName) const
	{
		std::map <CString, FilterCommentsSet> :: const_iterator pSet =
			m_FilterCommentsSetByFileType.find(FileTypeName);
		if (pSet == m_FilterCommentsSetByFileType.end())
			return FilterCommentsSet();
		return pSet->second;
	}
private:
	FilterCommentsManager(const FilterCommentsManager&); //Don't allow copy
	FilterCommentsManager& operator=(const FilterCommentsManager&);//Don't allow assignment
	/**
		@brief Create default comment marker strings
		@note
			Currently, only have C/C++/Java type markers.
	*/
	void CreateDefaultMarkers()
	{
		USES_CONVERSION;
		int SectionNo = 0;
		TCHAR SectionName[99];
		FilterCommentsSet filtercommentsset;
		filtercommentsset.StartMarker = "/*";
		filtercommentsset.EndMarker = "*/";
		filtercommentsset.InlineMarker = "//";
		TCHAR CommonFileTypes1[][9] = {_T("java"), _T("cs"), _T("cpp"), _T("c"), _T("h"), _T("cxx"), _T("cc"), _T("js"), _T("jsl"), _T("tli"), _T("tlh"), _T("rc")};
		_sntprintf(SectionName, sizeof(SectionName)/sizeof(SectionName[0]), _T("set%i"), SectionNo);
		++SectionNo;
		WritePrivateProfileString(SectionName, _T("StartMarker"), A2CT(filtercommentsset.StartMarker.c_str()), m_IniFileName);
		WritePrivateProfileString(SectionName, _T("EndMarker"), A2CT(filtercommentsset.EndMarker.c_str()), m_IniFileName);
		WritePrivateProfileString(SectionName, _T("InlineMarker"), A2CT(filtercommentsset.InlineMarker.c_str()), m_IniFileName);
		int FileTypeNo = 0;
		for(int i = 0;i < sizeof(CommonFileTypes1)/sizeof(CommonFileTypes1[0]);++i)
		{
			m_FilterCommentsSetByFileType[CommonFileTypes1[i]] = filtercommentsset;
			TCHAR FileTypeFieldName[99];
			_sntprintf(FileTypeFieldName, sizeof(FileTypeFieldName)/sizeof(FileTypeFieldName[0]), _T("FileType%i"), FileTypeNo);
			++FileTypeNo;
			WritePrivateProfileString(SectionName, FileTypeFieldName, CommonFileTypes1[i], m_IniFileName);
		}
	}

	//Use CString instead of std::string, so as to allow UNICODE file extensions
	std::map<CString, FilterCommentsSet> m_FilterCommentsSetByFileType;
	CString m_IniFileName;
};

/**
 * @brief Default constructor
 */
CDiffWrapper::CDiffWrapper():m_FilterCommentsManager(new FilterCommentsManager)
{
	ZeroMemory(&m_settings, sizeof(DIFFSETTINGS));
	ZeroMemory(&m_globalSettings, sizeof(DIFFSETTINGS));
	ZeroMemory(&m_status, sizeof(DIFFSTATUS));
	m_bCreatePatchFile = FALSE;
	m_bUseDiffList = FALSE;
	m_bDetectMovedBlocks = FALSE;
	m_bAddCmdLine = TRUE;
	m_bAppendFiles = FALSE;
	m_nDiffs = 0;
	m_infoPrediffer = NULL;
	m_pDiffList = NULL;

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
	delete m_infoPrediffer;
	delete m_FilterCommentsManager;
}

/**
 * @brief Sets filename of produced patch-file.
 * @param [in] file Filename of patch file.
 */
void CDiffWrapper::SetPatchFile(const CString &file)
{
	m_sPatchFile = file;
	m_sPatchFile.Replace('/', '\\');
}

/**
 * @brief Sets pointer to DiffList getting compare results.
 * CDiffWrapper adds compare results to DiffList. This function
 * sets external DiffList which gets compare results.
 * @param [in] difflist Pointer to DiffList getting compare results.
 */
void CDiffWrapper::SetDiffList(DiffList *diffList)
{
	ASSERT(diffList);
	m_pDiffList = diffList;
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
 * @brief Returns current set of options used for patch-file creation.
 * @param [in, out] options Pointer to structure where options are stored.
 */
void CDiffWrapper::GetPatchOptions(PATCHOPTIONS *options) const
{
	ASSERT(options);
	options->nContext = m_settings.context;
	options->outputStyle = m_settings.outputStyle;
	options->bAddCommandline = m_bAddCmdLine;
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
 * @brief Determines if external results-list is used.
 * When diff'ing files results are stored to external DiffList,
 * set by SetDiffList(). This function determines if we are currently
 * using that external list.
 * @return TRUE if results are added to external DiffList.
 */
BOOL CDiffWrapper::GetUseDiffList() const
{
	return m_bUseDiffList;
}

/**
 * @brief Enables/disables external result-list.
 * CDiffWrapper uses external DiffList to return compare results.
 * This function enables/disables usage of that external DiffList.
 * @param [in] bUseDifList TRUE if external DiffList is used.
 * @return Old value of the setting.
 */
BOOL CDiffWrapper::SetUseDiffList(BOOL bUseDiffList)
{
	BOOL temp = m_bUseDiffList;
	m_bUseDiffList = bUseDiffList;
	return temp;
}

/**
 * @brief Determines if patch-file is created.
 * @return TRUE if patch file will be created.
 */
BOOL CDiffWrapper::GetCreatePatchFile() const 
{
	return m_bCreatePatchFile;
}

/**
 * @brief Enables/disables creation of patch-file.
 * @param [in] bCreatePatchFile If TRUE patch file will be created.
 * @return Previous value for setting.
 */
BOOL CDiffWrapper::SetCreatePatchFile(BOOL bCreatePatchFile)
{
	BOOL temp = m_bCreatePatchFile;
	m_bCreatePatchFile = bCreatePatchFile;
	return temp;
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
bool PostFilter(int StartPos, int EndPos, int Direction, int QtyLinesInBlock, int &Op, int FileNo, const FilterCommentsSet& filtercommentsset)
{
	const char* EolIndicators = "\r\n"; //List of characters used as EOL
	if (Op == OP_TRIVIAL) //If already set to trivial, then exit.
		return true;
	bool OpShouldBeTrivial = false;
	for(int i = StartPos; i != EndPos;i += Direction)
	{
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
			if (!EndOfComment && StartOfComment)
			{
				if (i == StartPos)
					break;
				//If this is not the first line, then assume
				//previous lines are non-trivial, and return true.
				return false;
			}

			if (EndOfComment && 
				(!StartOfComment || StartOfComment > EndOfComment) && 
				(!InLineComment || InLineComment > EndOfComment) &&
				(i - StartPos) >=  QtyLinesInBlock )
			{
				OpShouldBeTrivial = true;
				break;
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
void PostFilterSingleLine(const char* LineStr, int &Op, const FilterCommentsSet& filtercommentsset, bool PartOfMultiLineCheck)
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
void PostFilter(int LineNumberLeft, int QtyLinesLeft, int LineNumberRight, int QtyLinesRight, int &Op, const FilterCommentsManager &filtercommentsmanager, const TCHAR *FileNameExt)
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
 * Sets full paths to two files we are diffing.
 * @param [in] filepath1 First file to compare "original file".
 * @param [in] filepath2 Second file to compare "changed file".
 */
void CDiffWrapper::SetPaths(const CString &filepath1, const CString &filepath2)
{
	m_s1File = filepath1;
	m_s2File = filepath2;
}

/**
 * @brief Set source paths for original (NON-TEMP) diffing two files.
 * Sets full paths to two (NON-TEMP) files we are diffing.
 * @param [in] filepath1 First file to compare "(NON-TEMP) file".
 * @param [in] filepath2 Second file to compare "(NON-TEMP) file".
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
BOOL CDiffWrapper::RunFileDiff(ARETEMPFILES areTempFiles)
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

	// Are our working files overwritable (temp)?
	BOOL bMayOverwrite = (areTempFiles == YESTEMPFILES);

	// Do the preprocessing now, overwrite the temp files
	// NOTE: FileTransform_UCS2ToUTF8() may create new temp
	// files and return new names, those created temp files
	// are deleted in end of function.
	if (m_infoPrediffer->bToBeScanned)
	{
		// this can only fail if the data can not be saved back (no more place on disk ???)
		// what to do then ??
		FileTransform_Prediffing(strFile1Temp, m_sToFindPrediffer, m_infoPrediffer, bMayOverwrite);
	}
	else
	{
		// this can failed if the prediffer has a problem
		if (FileTransform_Prediffing(strFile1Temp, *m_infoPrediffer, bMayOverwrite) == FALSE)
		{
			// display a message box
			CString sError;
			AfxFormatString2(sError, IDS_PREDIFFER_ERROR, strFile1Temp, m_infoPrediffer->pluginName);
			AfxMessageBox(sError, MB_OK | MB_ICONSTOP);
			// don't use any more this prediffer
			m_infoPrediffer->bToBeScanned = FALSE;
			m_infoPrediffer->pluginName = _T("");
		}
	}

	FileTransform_UCS2ToUTF8(strFile1Temp, bMayOverwrite);
	// we use the same plugin for both files, so it must be defined before second file
	ASSERT(m_infoPrediffer->bToBeScanned == FALSE);
	if (FileTransform_Prediffing(strFile2Temp, *m_infoPrediffer, bMayOverwrite) == FALSE)
	{
		// display a message box
		CString sError;
		AfxFormatString2(sError, IDS_PREDIFFER_ERROR, strFile2Temp, m_infoPrediffer->pluginName);
		AfxMessageBox(sError, MB_OK | MB_ICONSTOP);
		// don't use any more this prediffer
		m_infoPrediffer->bToBeScanned = FALSE;
		m_infoPrediffer->pluginName = _T("");
	}
	FileTransform_UCS2ToUTF8(strFile2Temp, bMayOverwrite);

	DiffFileData diffdata;
	diffdata.SetDisplayFilepaths(filepath1, filepath2); // store true names for diff utils patch file
	// This opens & fstats both files (if it succeeds)
	if (!diffdata.OpenFiles(strFile1Temp, strFile2Temp))
	{
		return FALSE;
	}

	/* Compare the files, if no error was found.  */
	int bin_flag = 0;
	struct change *script = NULL;
	bRet = Diff2Files(&script, &diffdata, &bin_flag);

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
 * @brief Determines if patch-files are appended (not overwritten)
 */
BOOL CDiffWrapper::GetAppendFiles() const
{
	return m_bAppendFiles;
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
 * @brief clear the lists (left & right) of moved blocks before RunFileDiff
 */
void CDiffWrapper::ClearMovedLists() 
{ 
	m_moved0.RemoveAll(); 
	m_moved1.RemoveAll(); 
}

/**
 * @brief Get left->right info for a moved line (real line number)
 */
int CDiffWrapper::RightLineInMovedBlock(int leftLine)
{
	int rightLine;
	if (m_moved0.Lookup(leftLine, rightLine))
		return rightLine;
	else
		return -1;
}

/**
 * @brief Get right->left info for a moved line (real line number)
 */
int CDiffWrapper::LeftLineInMovedBlock(int rightLine)
{
	int leftLine;
	if (m_moved1.Lookup(rightLine, leftLine))
		return leftLine;
	else
		return -1;
}

/** @brief Allow caller to specify codepage to assume for all unknown files */
void // static
DiffFileData::SetDefaultCodepage(int defcp)
{
	f_defcp = defcp;
}

/**
 * @brief Simple initialization of DiffFileData
 * @note Diffcounts are initialized to invalid values, not zeros.
 */
DiffFileData::DiffFileData()
{
	m_inf = new file_data[2];
	int i=0;
	for (i=0; i<2; ++i)
		memset(&m_inf[i], 0, sizeof(m_inf[i]));
	m_used = false;
	m_ndiffs = DiffFileData::DIFFS_UNKNOWN;
	m_ntrivialdiffs = DiffFileData::DIFFS_UNKNOWN;
	Reset();
	// Set default codepages
	for (i=0; i<sizeof(m_FileLocation)/sizeof(m_FileLocation[0]); ++i)
	{
		m_FileLocation[i].encoding.SetCodepage(f_defcp);
	}
}

/** @brief deallocate member data */
DiffFileData::~DiffFileData()
{
	Reset();
	delete [] m_inf;
}

/** @brief Open file descriptors in the inf structure (return false if failure) */
bool DiffFileData::OpenFiles(LPCTSTR szFilepath1, LPCTSTR szFilepath2)
{
	m_FileLocation[0].setPath(szFilepath1);
	m_FileLocation[1].setPath(szFilepath2);
	bool b = DoOpenFiles();
	if (!b)
		Reset();
	return b;
}

/** @brief stash away true names for display, before opening files */
void DiffFileData::SetDisplayFilepaths(LPCTSTR szTrueFilepath1, LPCTSTR szTrueFilepath2)
{
	m_sDisplayFilepath[0] = szTrueFilepath1;
	m_sDisplayFilepath[1] = szTrueFilepath2;
}


/** @brief Open file descriptors in the inf structure (return false if failure) */
bool DiffFileData::DoOpenFiles()
{
	Reset();

	for (int i=0; i<2; ++i)
	{
		// Fill in 8-bit versions of names for diffutils (WinMerge doesn't use these)
		USES_CONVERSION;
		// Actual paths are m_FileLocation[i].filepath
		// but these are often temporary files
		// Displayable (original) paths are m_sDisplayFilepath[i]
		m_inf[i].name = strdup(T2CA(m_sDisplayFilepath[i]));
		if (m_inf[i].name == NULL)
			return false;

		// Open up file descriptors
		// Always use O_BINARY mode, to avoid terminating file read on ctrl-Z (DOS EOF)
		// Also, WinMerge-modified diffutils handles all three major eol styles
		if (m_inf[i].desc == 0)
		{
			m_inf[i].desc = _topen(m_FileLocation[i].filepath, O_RDONLY|O_BINARY, _S_IREAD);
		}
		if (m_inf[i].desc < 0)
			return false;

		// Get file stats (diffutils uses these)
		if (fstat(m_inf[i].desc, &m_inf[i].stat) != 0)
		{
			return false;
		}
		if (0 == m_FileLocation[1].filepath.CompareNoCase(m_FileLocation[0].filepath))
		{
			m_inf[1].desc = m_inf[0].desc;
		}
	}

	m_used = true;
	return true;
}

/** @brief Clear inf structure to pristine */
void DiffFileData::Reset()
{
	ASSERT(m_inf);
	// If diffutils put data in, have it cleanup
	if (m_used)
	{
		cleanup_file_buffers(m_inf);
		m_used = false;
	}
	// clean up any open file handles, and zero stuff out
	// open file handles might be leftover from a failure in DiffFileData::OpenFiles
	for (int i=0; i<2; ++i)
	{
		if (m_inf[1].desc == m_inf[0].desc)
		{
			m_inf[1].desc = 0;
		}
		free((void *)m_inf[i].name);
		m_inf[i].name = NULL;

		if (m_inf[i].desc > 0)
		{
			close(m_inf[i].desc);
		}
		m_inf[i].desc = 0;
		memset(&m_inf[i], 0, sizeof(m_inf[i]));
	}
}

/**
 * @brief Try to deduce encoding for this file (given copy in memory)
 */
void DiffFileData::GuessEncoding_from_buffer(FileLocation & fpenc, const char **data, int count)
{
	if (!fpenc.encoding.m_bom)
	{
		CString sExt = PathFindExtension(fpenc.filepath);
		GuessEncoding_from_bytes(sExt, data, count, &fpenc.encoding);
	}
}

/** @brief Guess encoding for one file (in DiffContext memory buffer) */
void DiffFileData::GuessEncoding_from_buffer_in_DiffContext(int side, CDiffContext * pCtxt)
{
	GuessEncoding_from_buffer(m_FileLocation[side], m_inf[side].linbuf + m_inf[side].linbuf_base, 
	                                m_inf[side].valid_lines - m_inf[side].linbuf_base);
}

/** @brief Guess encoding for one file (in DiffContext memory buffer) */
void DiffFileData::GuessEncoding_from_FileLocation(FileLocation & fpenc)
{
	if (!fpenc.encoding.m_bom)
	{
		BOOL bGuess = TRUE;
		GuessCodepageEncoding(fpenc.filepath, &fpenc.encoding, bGuess);
	}
}

/**
 * @brief Compare two specified files.
 *
 * @param [in] Current directory depth.
 * @return Compare result as DIFFCODE.
 */
int DiffFileData::diffutils_compare_files(int depth)
{
	int bin_flag = 0;

	// Do the actual comparison (generating a change script)
	struct change *script = NULL;
	BOOL success = Diff2Files(&script, depth, &bin_flag, FALSE);
	if (!success)
	{
		return DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::CMPERR;
	}
	int code = DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::SAME;

	// make sure to start counting diffs at 0
	// (usually it is -1 at this point, for unknown)
	m_ndiffs = 0;
	m_ntrivialdiffs = 0;

	// Free change script (which we don't want)
	if (script != NULL)
	{
		struct change *p,*e;
		for (e = script; e; e = p)
		{
			++m_ndiffs;
			if (!e->trivial)
				code = code & ~DIFFCODE::SAME | DIFFCODE::DIFF;
			else
				++m_ntrivialdiffs;
			p = e->link;
			free (e);
		}
	}

	// diff_2_files set bin_flag to -1 if different binary
	// diff_2_files set bin_flag to +1 if same binary

	if (bin_flag != 0)
	{
		// Clear text-flag, set binary flag
		// We don't know diff counts for binary files
		code = code & ~DIFFCODE::TEXT | DIFFCODE::BIN;
		m_ndiffs = DiffFileData::DIFFS_UNKNOWN;
	}

	if (bin_flag < 0)
	{
		// Clear same-flag, set diff-flag
		code = code & ~DIFFCODE::SAME | DIFFCODE::DIFF;
	}

	return code;
}

/** @brief detect unicode file and quess encoding */
DiffFileData::UniFileBom::UniFileBom(int fd)
{
	size = 0;
	unicoding = ucr::NONE;
	if (fd == -1) 
		return;
	long tmp = _lseek(fd, 0, SEEK_SET);
	switch (_read(fd, buffer, 3))
	{
		case 3:
			if (buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF)
			{
				size = 3;
				unicoding = ucr::UTF8;
				break;
			}
			// fall through & try the 2-byte BOMs
		case 2:
			if (buffer[0] == 0xFF && buffer[1] == 0xFE)
			{
				size = 2;
				unicoding = ucr::UCS2LE;
				break;
			}
			if (buffer[0] == 0xFE && buffer[1] == 0xFF)
			{
				size = 2;
				unicoding = ucr::UCS2BE;
				break;
			}
		default:
			size = 0;
			unicoding = ucr::NONE;
	}
	_lseek(fd, tmp, SEEK_SET);
}

/** @brief Create int array with size elements, and set initial entries to initvalue */
#if 0
static int * NewIntArray(int size, int initvalue)
{
	int * arr = new int[size];
	for (int i=0; i<size; ++i)
		arr[i] = initvalue;
	return arr;
}
#endif

class IntSet
{
public:
	void Add(int val) { m_map.SetAt(val, 1); }
	void Remove(int val) { m_map.RemoveKey(val); }
	int count() const { return m_map.GetCount(); }
	bool isPresent(int val) const { int parm; return !!m_map.Lookup(val, parm); }
	int getSingle() const 
	{
		int val, parm;
		POSITION pos = m_map.GetStartPosition();
		m_map.GetNextAssoc(pos, val, parm); 
		return val; 
	}

private:
	CMap<int, int, int, int> m_map;
};

/** 
 * @brief  Set of equivalent lines
 * This uses diffutils line numbers, which are counted from the prefix
 */
struct EqGroup
{
	IntSet m_lines0; // equivalent lines on side#0
	IntSet m_lines1; // equivalent lines on side#1

	bool isPerfectMatch() const { return m_lines0.count()==1 && m_lines1.count()==1; }
};


/** @brief  Maps equivalency code to equivalency group */
class CodeToGroupMap : public CTypedPtrMap<CMapPtrToPtr, void*, EqGroup *>
{
public:
	/** @brief Add a line to the appropriate equivalency group */
	void Add(int lineno, int eqcode, int nside)
	{
		EqGroup * pgroup = 0;
		if (!Lookup((void *)eqcode, pgroup))
		{
			pgroup = new EqGroup;
			SetAt((void *)eqcode, pgroup);
		}
		if (nside)
			pgroup->m_lines1.Add(lineno);
		else
			pgroup->m_lines0.Add(lineno);
	}

	/** @brief Return the appropriate equivalency group */
	EqGroup * find(int eqcode)
	{
		EqGroup * pgroup=0;
		Lookup((void *)eqcode, pgroup);
		return pgroup;
	}

	~CodeToGroupMap()
	{
		for (POSITION pos = GetStartPosition(); pos; )
		{
			void * v=0;
			EqGroup * pgroup=0;
			GetNextAssoc(pos, v, pgroup);
			delete pgroup;
		}
	}
};

/*
 WinMerge moved block code
 This is called by diffutils code, by diff_2_files routine
 read_files earlier computed the hash chains ("equivs" file variable) and freed them,
 but the equivs numerics are still available in each line

 match1 set by scan from line0 to deleted
 match0 set by scan from line1 to inserted

*/
extern "C" void moved_block_analysis(struct change ** pscript, struct file_data fd[])
{
	// Hash all altered lines
	CodeToGroupMap map;

	struct change * script = *pscript;
	struct change *p,*e;
	for (e = script; e; e = p)
	{
		p = e->link;
		int i=0;
		for (i=e->line0; i-(e->line0) < (e->deleted); ++i)
			map.Add(i, fd[0].equivs[i], 0);
		for (i=e->line1; i-(e->line1) < (e->inserted); ++i)
			map.Add(i, fd[1].equivs[i], 1);
	}


	// Scan through diff blocks, finding moved sections from left side
	// and splitting them out
	// That is, we actually fragment diff blocks as we find moved sections
	for (e = script; e; e = p)
	{
		// scan down block for a match
		p = e->link;
		EqGroup * pgroup = 0;
		int i=0;
		for (i=e->line0; i-(e->line0) < (e->deleted); ++i)
		{
			EqGroup * tempgroup = map.find(fd[0].equivs[i]);
			if (tempgroup->isPerfectMatch())
			{
				pgroup = tempgroup;
				break;
			}
		}

		// if no match, go to next diff block
		if (!pgroup)
			continue;

		// found a match
		int j = pgroup->m_lines1.getSingle();
		// Ok, now our moved block is the single line i,j

		// extend moved block upward as far as possible
		int i1 = i-1;
		int j1 = j-1;
		for ( ; i1>=e->line0; --i1, --j1)
		{
			EqGroup * pgroup0 = map.find(fd[0].equivs[i1]);
			EqGroup * pgroup1 = map.find(fd[1].equivs[j1]);
			if (pgroup0 != pgroup1)
				break;
			pgroup0->m_lines0.Remove(i1);
			pgroup1->m_lines1.Remove(j1);
		}
		++i1;
		++j1;
		// Ok, now our moved block is i1->i, j1->j

		// extend moved block downward as far as possible
		int i2 = i+1;
		int j2 = j+1;
		for ( ; i2-(e->line0) < (e->deleted); ++i2,++j2)
		{
			EqGroup * pgroup0 = map.find(fd[0].equivs[i2]);
			EqGroup * pgroup1 = map.find(fd[1].equivs[j2]);
			if (pgroup0 != pgroup1)
				break;
			pgroup0->m_lines0.Remove(i2);
			pgroup1->m_lines1.Remove(j2);
		}
		--i2;
		--j2;
		// Ok, now our moved block is i1->i2,j1->j2

		ASSERT(i2-i1 >= 0);
		ASSERT(i2-i1 == j2-j1);

		int prefix = i1 - (e->line0);
		if (prefix)
		{
			// break e (current change) into two pieces
			// first part is the prefix, before the moved part
			// that stays in e
			// second part is the moved part & anything after it
			// that goes in newob
			// leave the right side (e->inserted) on e
			// so no right side on newob
			// newob will be the moved part only, later after we split off any suffix from it
			struct change *newob = (struct change *) xmalloc (sizeof (struct change));
			memset(newob, 0, sizeof(*newob));

			newob->line0 = i1;
			newob->line1 = e->line1 + e->inserted;
			newob->inserted = 0;
			newob->deleted = e->deleted - prefix;
			newob->link = e->link;
			newob->match0 = -1;
			newob->match1 = -1;

			e->deleted = prefix;
			e->link = newob;

			// now make e point to the moved part (& any suffix)
			e = newob;
		}
		// now e points to a moved diff chunk with no prefix, but maybe a suffix

		e->match1 = j1;

		int suffix = (e->deleted) - (i2-(e->line0)) - 1;
		if (suffix)
		{
			// break off any suffix from e
			// newob will be the suffix, and will get all the right side
			struct change *newob = (struct change *) xmalloc (sizeof (struct change));
			memset(newob, 0, sizeof(*newob));

			newob->line0 = i2+1;
			newob->line1 = e->line1;
			newob->inserted = e->inserted;
			newob->deleted = suffix;
			newob->link = e->link;
			newob->match0 = -1;
			newob->match1 = -1;

			e->inserted = 0;
			e->deleted -= suffix;
			e->link = newob;

			p = newob; // next block to scan
		}
	}

	// Scan through diff blocks, finding moved sections from right side
	// and splitting them out
	// That is, we actually fragment diff blocks as we find moved sections
	for (e = script; e; e = p)
	{
		// scan down block for a match
		p = e->link;
		EqGroup * pgroup = 0;
		int j=0;
		for (j=e->line1; j-(e->line1) < (e->inserted); ++j)
		{
			EqGroup * tempgroup = map.find(fd[1].equivs[j]);
			if (tempgroup->isPerfectMatch())
			{
				pgroup = tempgroup;
				break;
			}
		}

		// if no match, go to next diff block
		if (!pgroup)
			continue;

		// found a match
		int i = pgroup->m_lines0.getSingle();
		// Ok, now our moved block is the single line i,j

		// extend moved block upward as far as possible
		int i1 = i-1;
		int j1 = j-1;
		for ( ; j1>=e->line1; --i1, --j1)
		{
			EqGroup * pgroup0 = map.find(fd[0].equivs[i1]);
			EqGroup * pgroup1 = map.find(fd[1].equivs[j1]);
			if (pgroup0 != pgroup1)
				break;
			pgroup0->m_lines0.Remove(i1);
			pgroup1->m_lines1.Remove(j1);
		}
		++i1;
		++j1;
		// Ok, now our moved block is i1->i, j1->j

		// extend moved block downward as far as possible
		int i2 = i+1;
		int j2 = j+1;
		for ( ; j2-(e->line1) < (e->inserted); ++i2,++j2)
		{
			EqGroup * pgroup0 = map.find(fd[0].equivs[i2]);
			EqGroup * pgroup1 = map.find(fd[1].equivs[j2]);
			if (pgroup0 != pgroup1)
				break;
			pgroup0->m_lines0.Remove(i2);
			pgroup1->m_lines1.Remove(j2);
		}
		--i2;
		--j2;
		// Ok, now our moved block is i1->i2,j1->j2

		ASSERT(i2-i1 >= 0);
		ASSERT(i2-i1 == j2-j1);

		int prefix = j1 - (e->line1);
		if (prefix)
		{
			// break e (current change) into two pieces
			// first part is the prefix, before the moved part
			// that stays in e
			// second part is the moved part & anything after it
			// that goes in newob
			// leave the left side (e->deleted) on e
			// so no right side on newob
			// newob will be the moved part only, later after we split off any suffix from it
			struct change *newob = (struct change *) xmalloc (sizeof (struct change));
			memset(newob, 0, sizeof(*newob));

			newob->line0 = e->line0 + e->deleted;
			newob->line1 = j1;
			newob->inserted = e->inserted - prefix;
			newob->deleted = 0;
			newob->link = e->link;
			newob->match0 = -1;
			newob->match1 = -1;

			e->inserted = prefix;
			e->link = newob;

			// now make e point to the moved part (& any suffix)
			e = newob;
		}
		// now e points to a moved diff chunk with no prefix, but maybe a suffix

		e->match0 = i1;

		int suffix = (e->inserted) - (j2-(e->line1)) - 1;
		if (suffix)
		{
			// break off any suffix from e
			// newob will be the suffix, and will get all the left side
			struct change *newob = (struct change *) xmalloc (sizeof (struct change));
			memset(newob, 0, sizeof(*newob));

			newob->line0 = e->line0;
			newob->line1 = j2+1;
			newob->inserted = suffix;
			newob->deleted = e->deleted;
			newob->link = e->link;
			newob->match0 = -1;
			newob->match1 = e->match1;

			e->inserted -= suffix;
			e->deleted = 0;
			e->match1 = -1;
			e->link = newob;

			p = newob; // next block to scan
		}
	}

}

/**
 * @brief Invoke appropriate plugins for unpacking
 * return false if anything fails
 * caller has to DeleteFile filepathTransformed, if it differs from filepath
 */
static bool Unpack(CString & filepathTransformed,
	const CString & filteredFilenames, PackingInfo * infoUnpacker)
{
	// first step : unpack (plugins)
	if (infoUnpacker->bToBeScanned)
	{
		if (!FileTransform_Unpacking(filepathTransformed, filteredFilenames, infoUnpacker, &infoUnpacker->subcode))
			return false;
	}
	else
	{
		if (!FileTransform_Unpacking(filepathTransformed, infoUnpacker, &infoUnpacker->subcode))
			return false;
	}
	return true;
}

/**
 * @brief Get actual compared paths from DIFFITEM.
 * @note If item is unique, same path is returned for both.
 */
void GetComparePaths(CDiffContext * pCtxt, const DIFFITEM &di, CString & left, CString & right)
{
	static const TCHAR backslash[] = _T("\\");

	if (!di.isSideRight())
	{
		// Compare file to itself to detect encoding
		left = pCtxt->GetNormalizedLeft();
		if (!paths_EndsWithSlash(left))
			left += backslash;
		if (!di.sLeftSubdir.IsEmpty())
			left += di.sLeftSubdir + backslash;
		left += di.sLeftFilename;
		if (di.isSideLeft())
			right = left;
	}
	if (!di.isSideLeft())
	{
		// Compare file to itself to detect encoding
		right = pCtxt->GetNormalizedRight();
		if (!paths_EndsWithSlash(right))
			right += backslash;
		if (!di.sRightSubdir.IsEmpty())
			right += di.sRightSubdir + backslash;
		right += di.sRightFilename;
		if (di.isSideRight())
			left = right;
	}
}


/**
 * @brief Invoke appropriate plugins for prediffing
 * return false if anything fails
 * caller has to DeleteFile filepathTransformed, if it differs from filepath
 */
bool DiffFileData::Filepath_Transform(FileLocation & fpenc, const CString & filepath, CString & filepathTransformed,
	const CString & filteredFilenames, PrediffingInfo * infoPrediffer, int fd)
{
	BOOL bMayOverwrite = FALSE; // temp variable set each time it is used

	// Read BOM to check for Unicode
	UniFileBom bom = fd;
	if (bom.unicoding)
		fpenc.encoding.SetUnicoding(bom.unicoding);

	if (fpenc.encoding.m_unicoding && fpenc.encoding.m_unicoding != ucr::UCS2LE)
	{
		// second step : normalize Unicode to OLECHAR (most of time, do nothing) (OLECHAR = UCS-2LE in Windows)
		bMayOverwrite = (filepathTransformed != filepath); // may overwrite if we've already copied to temp file
		if (!FileTransform_NormalizeUnicode(filepathTransformed, bMayOverwrite))
			return false;
	}

	// Note: filepathTransformed may be in UCS-2 (if toUtf8), or it may be raw encoding (if !Utf8)
	// prediff plugins must handle both

	// third step : prediff (plugins)
	bMayOverwrite = (filepathTransformed != filepath); // may overwrite if we've already copied to temp file
	if (infoPrediffer->bToBeScanned)
	{
		// FileTransform_Prediffing tries each prediffer for the pointed out filteredFilenames
		// if a prediffer fails, we consider it is not the good one, that's all
		// FileTransform_Prediffing returns FALSE only if the prediffer works, 
		// but the data can not be saved to disk (no more place ??)
		if (FileTransform_Prediffing(filepathTransformed, filteredFilenames, infoPrediffer, bMayOverwrite) 
				== FALSE)
			return false;
	}
	else
	{
		// this can failed if the pointed out prediffer has a problem
		if (FileTransform_Prediffing(filepathTransformed, *infoPrediffer, bMayOverwrite) 
				== FALSE)
			return false;
	}

	if (fpenc.encoding.m_unicoding)
	{
		// fourth step : prepare for diffing
		// may overwrite if we've already copied to temp file
		BOOL bMayOverwrite = (0 != filepathTransformed.CompareNoCase(filepath));
		if (!FileTransform_UCS2ToUTF8(filepathTransformed, bMayOverwrite))
			return false;
	}
	return true;
}

/**
 * @brief Prepare files (run plugins) & compare them, and return diffcode
 */
int DiffFileData::prepAndCompareTwoFiles(CDiffContext * pCtxt, DIFFITEM &di)
{
	int nCompMethod = pCtxt->m_nCompMethod;
	CString filepath1;
	CString filepath2;
	GetComparePaths(pCtxt, di, filepath1, filepath2);

	// Reset text stats
	m_textStats0.clear();
	m_textStats1.clear();

	int code = DIFFCODE::FILE | DIFFCODE::CMPERR;
	// For user chosen plugins, define bAutomaticUnpacker as false and use the chosen infoHandler
	// but how can we receive the infoHandler ? DirScan actually only 
	// returns info, but can not use file dependent information.

	// Transformation happens here
	// text used for automatic mode : plugin filter must match it
	CString filteredFilenames = filepath1 + "|" + filepath2;

	PackingInfo * infoUnpacker=0;
	PrediffingInfo * infoPrediffer=0;

	// Get existing or new plugin infos
	pCtxt->FetchPluginInfos(filteredFilenames, &infoUnpacker, &infoPrediffer);

	// plugin may alter filepaths to temp copies (which we delete before returning in all cases)
	CString filepathUnpacked1 = filepath1;
	CString filepathUnpacked2 = filepath2;

	CString filepathTransformed1;
	CString filepathTransformed2;

	//DiffFileData diffdata; //(filepathTransformed1, filepathTransformed2);
	// Invoke unpacking plugins
	if (!Unpack(filepathUnpacked1, filteredFilenames, infoUnpacker))
	{
		di.errorDesc = _T("Unpack Error Side 1");
		goto exitPrepAndCompare;
	}

	// we use the same plugins for both files, so they must be defined before second file
	ASSERT(infoUnpacker->bToBeScanned == FALSE);

	if (!Unpack(filepathUnpacked2, filteredFilenames, infoUnpacker))
	{
		di.errorDesc = _T("Unpack Error Side 2");
		goto exitPrepAndCompare;
	}

	// As we keep handles open on unpacked files, Transform() may not delete them.
	// Unpacked files will be deleted at end of this function.
	filepathTransformed1 = filepathUnpacked1;
	filepathTransformed2 = filepathUnpacked2;
	SetDisplayFilepaths(filepath1, filepath2); // store true names for diff utils patch file
	if (!OpenFiles(filepathTransformed1, filepathTransformed2))
	{
		di.errorDesc = _T("OpenFiles Error (before tranform)");
		goto exitPrepAndCompare;
	}

	// Invoke prediff'ing plugins
	if (!Filepath_Transform(m_FileLocation[0], filepathUnpacked1, filepathTransformed1, filteredFilenames, infoPrediffer, m_inf[0].desc))
	{
		di.errorDesc = _T("Transform Error Side 1");
		goto exitPrepAndCompare;
	}

	// we use the same plugins for both files, so they must be defined before second file
	ASSERT(infoPrediffer->bToBeScanned == FALSE);

	if (!Filepath_Transform(m_FileLocation[1], filepathUnpacked2, filepathTransformed2, filteredFilenames, infoPrediffer, m_inf[1].desc))
	{
		di.errorDesc = _T("Transform Error Side 2");
		goto exitPrepAndCompare;
	}

	// If options are binary equivalent, we could check for filesize
	// difference here, and bail out if files are clearly different
	// But, then we don't know if file is ascii or binary, and this
	// affects behavior (also, we don't have an icon for unknown type)

	// Actually compare the files
	// diffutils_compare_files is a fairly thin front-end to diffutils
	if (filepathTransformed1 != filepathUnpacked1 || filepathTransformed2 != filepathUnpacked2)
	{
		//diffdata.m_sFilepath[0] = filepathTransformed1;
		//diffdata.m_sFilepath[1] = filepathTransformed2;
		if (!OpenFiles(filepathTransformed1, filepathTransformed2))
		{
			di.errorDesc = _T("OpenFiles Error (after tranform)");
			goto exitPrepAndCompare;
		}
	}

	// If either file is larger than limit compare files by quick contents
	// This allows us to (faster) compare big binary files
	if (pCtxt->m_nCompMethod == CMP_CONTENT && 
		(di.left.size > pCtxt->m_nQuickCompareLimit ||
		di.right.size > pCtxt->m_nQuickCompareLimit))
	{
		nCompMethod = CMP_QUICK_CONTENT;
	}

	if (nCompMethod == CMP_CONTENT)
	{
		// use diffutils
		code = diffutils_compare_files(0);
		// If unique item, it was being compared to itself to determine encoding
		// and the #diffs is invalid
		if (di.isSideRight() || di.isSideLeft())
		{
			m_ndiffs = DiffFileData::DIFFS_UNKNOWN;
			m_ntrivialdiffs = DiffFileData::DIFFS_UNKNOWN;
		}
		if (DIFFCODE::isResultError(code))
			di.errorDesc = _T("DiffUtils Error");

		if (!DIFFCODE::isResultError(code) && pCtxt->m_bGuessEncoding)
		{
			// entire file is in memory in the diffutils buffers
			// inside the diff context, so may as well use in-memory copy
			GuessEncoding_from_buffer_in_DiffContext(0, pCtxt);
			GuessEncoding_from_buffer_in_DiffContext(1, pCtxt);
		}
	}
	else if (nCompMethod == CMP_QUICK_CONTENT)
	{
		// use our own byte-by-byte compare
		code = byte_compare_files(pCtxt->m_bStopAfterFirstDiff, pCtxt->GetAbortable());
		// Quick contents doesn't know about diff counts
		// Set to special value to indicate invalid
		m_ndiffs = DIFFS_UNKNOWN_QUICKCOMPARE;
		m_ntrivialdiffs = DIFFS_UNKNOWN_QUICKCOMPARE;

		if (!DIFFCODE::isResultError(code) && pCtxt->m_bGuessEncoding)
		{
			GuessEncoding_from_FileLocation(m_FileLocation[0]);
			GuessEncoding_from_FileLocation(m_FileLocation[1]);
		}
	}
	else
	{
		// Print error since we should have handled by date compare earlier
		_RPTF0(_CRT_ERROR, "Invalid compare type, DiffFileData can't handle it");
		di.errorDesc = _T("Bad compare type");
		goto exitPrepAndCompare;
	}


exitPrepAndCompare:
	Reset();
	// delete the temp files after comparison
	if (filepathTransformed1 != filepathUnpacked1)
		VERIFY(::DeleteFile(filepathTransformed1) || gLog::DeleteFileFailed(filepathTransformed1));
	if (filepathTransformed2 != filepathUnpacked2)
		VERIFY(::DeleteFile(filepathTransformed2) || gLog::DeleteFileFailed(filepathTransformed2));
	if (filepathUnpacked1 != filepath1)
		VERIFY(::DeleteFile(filepathUnpacked1) || gLog::DeleteFileFailed(filepathUnpacked1));
	if (filepathUnpacked2 != filepath2)
		VERIFY(::DeleteFile(filepathUnpacked2) || gLog::DeleteFileFailed(filepathUnpacked2));
	return code;
}

/**
 * @brief Copy text stat results from diffutils back into the FileTextStats structure
 */
static void CopyTextStats(const file_data * inf, FileTextStats * myTextStats)
{
	myTextStats->ncrlfs = inf->count_crlfs;
	myTextStats->ncrs = inf->count_crs;
	myTextStats->nlfs = inf->count_lfs;
}

/**
 * @brief Copy both left & right text stats results back into the DiffFileData text stats
 */
static void CopyDiffutilTextStats(file_data *inf, DiffFileData * diffData)
{
	CopyTextStats(&inf[0], &diffData->m_textStats0);
	CopyTextStats(&inf[1], &diffData->m_textStats1);
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
 * @return TRUE when compare succeeds, FALSE if error happened during compare.
 */
BOOL CDiffWrapper::Diff2Files(struct change ** diffs, DiffFileData *diffData,
	int * bin_status)
{
	BOOL bRet = TRUE;
	__try
	{
		// Diff files. depth is zero because we are not comparing dirs
		*diffs = diff_2_files (diffData->m_inf, 0, bin_status, m_bDetectMovedBlocks);
		CopyDiffutilTextStats(diffData->m_inf, diffData);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		*diffs = NULL;
		bRet = FALSE;
	}
	return bRet;
}

BOOL DiffFileData::Diff2Files(struct change ** diffs, int depth,
	int * bin_status, BOOL bMovedBlocks)
{
	BOOL bRet = TRUE;
	__try
	{
		*diffs = diff_2_files (m_inf, depth, bin_status, bMovedBlocks);
		CopyDiffutilTextStats(m_inf, this);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		*diffs = NULL;
		bRet = FALSE;
	}
	return bRet;
}

struct FileHandle
{
	FileHandle() : m_fp(0) { }
	void Assign(FILE * fp) { Close(); m_fp = fp; }
	void Close() { if (m_fp) { fclose(m_fp); m_fp = 0; } }
	~FileHandle() { Close(); }
	FILE * m_fp;
};

/** 
 * @brief Compare two specified files, byte-by-byte
 * @param [in] bStopAfterFirstDiff Stop compare after we find first difference?
 * @param [in] piAbortable Interface allowing to abort compare
 * @return DIFFCODE
 */
int DiffFileData::byte_compare_files(BOOL bStopAfterFirstDiff, const IAbortable * piAbortable)
{
	// Close any descriptors open for diffutils
	Reset();

	// TODO
	// Right now, we assume files are in 8-bit encoding
	// because transform code converted any UCS-2 files to UTF-8
	// We could compare directly in UCS-2LE here, as an optimization, in that case
	char buff[2][WMCMPBUFF]; // buffered access to files
	FILE * fp[2]; // for files to compare
	FileHandle fhd[2]; // to ensure file handles fp get closed
	int i;
	int diffcode = 0;

	// Open both files
	for (i=0; i<2; ++i)
	{
		fp[i] = _tfopen(m_FileLocation[i].filepath, _T("rb"));
		if (!fp[i])
			return DIFFCODE::CMPERR;
		fhd[i].Assign(fp[i]);
	}

	// area of buffer currently holding data
	int bfstart[2]; // offset into buff[i] where current data resides
	int bfend[2]; // past-the-end pointer into buff[i], giving end of current data
	// buff[0] has bytes to process from buff[0][bfstart[0]] to buff[0][bfend[0]-1]

	bool eof[2]; // if we've finished file

	// initialize our buffer pointers and end of file flags
	for (i=0; i<2; ++i)
	{
		bfstart[i] = bfend[i] = 0;
		eof[i] = false;
	}

	ByteComparator comparator(ignore_case_flag, ignore_space_change_flag
		, ignore_all_space_flag, ignore_eol_diff, ignore_blank_lines_flag);

	// Begin loop
	// we handle the files in WMCMPBUFF sized buffers (variable buff[][])
	// That is, we do one buffer full at a time
	// or even less, as we process until one side buffer is empty, then reload that one
	// and continue
	while (!eof[0] || !eof[1])
	{
		if (piAbortable && piAbortable->ShouldAbort())
			return DIFFCODE::CMPABORT;

		// load or update buffers as appropriate
		for (i=0; i<2; ++i)
		{
			if (!eof[i] && bfstart[i]==countof(buff[i]))
			{
				bfstart[i]=bfend[i] = 0;
			}
			if (!eof[i] && bfend[i]<countof(buff[i])-1)
			{
				int space = countof(buff[i]) - bfend[i];
				int rtn = fread(&buff[i][bfend[i]], 1, space, fp[i]);
				if (ferror(fp[i]))
					return DIFFCODE::CMPERR;
				if (feof(fp[i]))
					eof[i] = true;
				bfend[i] += rtn;
			}
		}

		// where to start comparing right now
		LPCSTR ptr0 = &buff[0][bfstart[0]];
		LPCSTR ptr1 = &buff[1][bfstart[1]];

		// remember where we started
		LPCSTR orig0 = ptr0, orig1 = ptr1;

		// how far can we go right now?
		LPCSTR end0 = &buff[0][bfend[0]];
		LPCSTR end1 = &buff[1][bfend[1]];

		int offset0 = (ptr0 - &buff[0][0]);
		int offset1 = (ptr1 - &buff[1][0]);

		// are these two buffers the same?
		if (!comparator.CompareBuffers(m_textStats0, m_textStats1, 
			ptr0, ptr1, end0, end1, eof[0], eof[1], offset0, offset1))
		{
			if (bStopAfterFirstDiff)
			{
				// By bailing out here
				// we leave our text statistics incomplete
				return diffcode | DIFFCODE::DIFF;
			}
			else
			{
				diffcode |= DIFFCODE::DIFF;
				ptr0 = end0;
				ptr1 = end1;
			}
		}
		else
		{
			ptr0 = end0;
			ptr1 = end1;
		}


		// did we finish both files?
		if (eof[0] && eof[1])
		{

			BOOL bBin0 = (m_textStats0.nzeros>0);
			BOOL bBin1 = (m_textStats1.nzeros>0);
			if (bBin0 || bBin1)
			{
				diffcode |= DIFFCODE::BIN;
			}

			// If either unfinished, they differ
			if (ptr0 != end0 || ptr1 != end1)
				diffcode = (diffcode & DIFFCODE::DIFF);
			
			if (diffcode & DIFFCODE::DIFF)
				return diffcode | DIFFCODE::DIFF;
			else
				return diffcode | DIFFCODE::SAME;
		}

		// move our current pointers over what we just compared
		ASSERT(ptr0 >= orig0);
		ASSERT(ptr1 >= orig1);
		bfstart[0] += ptr0-orig0;
		bfstart[1] += ptr1-orig1;
	}
	return diffcode;
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
				if (thisob->match0>=0)
				{
					ASSERT(thisob->inserted);
					for (int i=0; i<thisob->inserted; ++i)
					{
						int line0 = i+thisob->match0 + (trans_a0-first0-1);
						int line1 = i+thisob->line1 + (trans_a1-first1-1);
						m_moved1[line1]=line0;
					}
				}
				if (thisob->match1>=0)
				{
					ASSERT(thisob->deleted);
					for (int i=0; i<thisob->deleted; ++i)
					{
						int line0 = i+thisob->line0 + (trans_a0-first0-1);
						int line1 = i+thisob->match1 + (trans_a1-first1-1);
						m_moved0[line0]=line1;
					}
				}

				if (options.bFilterCommentsLines)
				{
					int QtyLinesLeft = (trans_b0 - trans_a0) + 1; //Determine quantity of lines in this block for left side
					int QtyLinesRight = (trans_b1 - trans_a1) + 1;//Determine quantity of lines in this block for right side
					PostFilter(thisob->line0, QtyLinesLeft, thisob->line1, QtyLinesRight, op, *m_FilterCommentsManager, asLwrCaseExt);
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
 * @todo filepath1 and filepath2 aren't really needed, paths are already in inf.
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
