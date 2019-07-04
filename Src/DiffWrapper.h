/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  DiffWrapper.h
 *
 * @brief Declaration file for CDiffWrapper.
 *
 * @date  Created: 2003-08-22
 */
#pragma once

#include <memory>
#include "diff.h"
#include "FileLocation.h"
#include "PathContext.h"
#include "CompareOptions.h"
#include "DiffList.h"
#include "UnicodeString.h"
#include "FileTransform.h"

class CDiffContext;
class PrediffingInfo;
struct DiffFileData;
class PathContext;
struct file_data;
class FilterCommentsManager;
struct FilterCommentsSet;
class MovedLines;
class FilterList;

/** @enum COMPARE_TYPE
 * @brief Different foldercompare methods.
 * These values are the foldercompare methods WinMerge supports.
 */

/** @var CMP_CONTENT
 * @brief Normal by content compare.
 * This compare type is first, default and all-seeing compare type.
 * diffutils is used for producing compare results. So all limitations
 * of diffutils (like buffering) apply to this compare method. But this
 * is also currently only compare method that produces difference lists
 * we can use in file compare.
 */

/** @var CMP_QUICK_CONTENT
 * @brief Faster byte per byte -compare.
 * This type of compare was a response for needing faster compare results
 * in folder compare. It independent from diffutils, and fully customised
 * for WinMerge. It basically does byte-per-byte compare, still implementing
 * different whitespace ignore options.
 *
 * Optionally this compare type can be stopped when first difference is found.
 * Which gets compare as fast as possible. But misses sometimes binary files
 * if zero bytes aren't found before first difference. Also difference counts
 * are not useful with that option.
 */

/** @var CMP_DATE
 * @brief Compare by modified date.
 * This compare type was added after requests and realization that in some
 * situations difference in file's timestamps is enough to judge them
 * different. E.g. when modifying files in local machine, file timestamps
 * are certainly different after modifying them. This method doesn't even
 * open files for reading them. It only reads file's infos for timestamps
 * and compares them.
 *
 * This is no doubt fastest way to compare files.
 */

/** @var CMP_DATE_SIZE
 * @brief Compare by date and then by size.
 * This method is basically same than CMP_DATE, but it adds check for file
 * sizes if timestamps are identical. This is because there are situations
 * timestamps can't be trusted alone, especially with network shares. Adding
 * checking for file sizes adds some more reliability for results with
 * minimal increase in compare time.
 */

/** @var CMP_SIZE
 * @brief Compare by file size.
 * This compare method compares file sizes. This isn't quite accurate method,
 * other than it can detect files that certainly differ. But it can show lot of
 * different files as identical too. Advantage is in some use cases where different
 * size always means files are different. E.g. automatically created logs - when
 * more data is added size increases.
 */
enum COMPARE_TYPE
{
	CMP_CONTENT = 0,
	CMP_QUICK_CONTENT,
	CMP_BINARY_CONTENT,
	CMP_DATE,
	CMP_DATE_SIZE,
	CMP_SIZE,
};

/**
 * @brief Additional options for creating patch files
 */
struct PATCHOPTIONS
{
	enum output_style outputStyle; /**< Patch file style. */
	int nContext; /**< Number of context lines. */
	bool bAddCommandline; /**< Add diff-style commandline to patch file. */
};

typedef enum {
	IDENTLEVEL_ALL,
	IDENTLEVEL_NONE,
	IDENTLEVEL_EXCEPTLEFT,
	IDENTLEVEL_EXCEPTMIDDLE,
	IDENTLEVEL_EXCEPTRIGHT,
} IDENTLEVEL;
/**
 * @brief Diffutils returns this statusdata about files compared
 */
struct DIFFSTATUS
{
	bool bMissingNL[3] {}; /**< file is missing EOL before EOF */
	bool bBinaries = false; /**< Files are binaries */
	IDENTLEVEL Identical = IDENTLEVEL_NONE; /**< diffutils said files are identical */
	bool bPatchFileFailed = false; /**< Creating patch file failed */

	DIFFSTATUS() {}
	void MergeStatus(const DIFFSTATUS& other)
	{
		if (Identical == IDENTLEVEL_ALL)
			Identical = other.Identical;
		else if (
			 (Identical == IDENTLEVEL_EXCEPTLEFT   && other.Identical != IDENTLEVEL_EXCEPTLEFT) ||
			 (Identical == IDENTLEVEL_EXCEPTRIGHT  && other.Identical != IDENTLEVEL_EXCEPTRIGHT) ||
			 (Identical == IDENTLEVEL_EXCEPTMIDDLE && other.Identical != IDENTLEVEL_EXCEPTMIDDLE))
			Identical = IDENTLEVEL_NONE;
		if (other.bPatchFileFailed)
			bPatchFileFailed = true;
		if (other.bBinaries)
			bBinaries = true;
		std::copy_n(other.bMissingNL, 3, bMissingNL);
	}
};

class FilterCommentsManager;

/**
 * @brief Wrapper class for diffengine (diffutils and ByteComparator).
 * Diffwappre class is used to run selected diffengine. For folder compare
 * there are several methods (COMPARE_TYPE), but for file compare diffutils
 * is used always. For file compare diffutils can output results to external
 * DiffList or to patch file. Output type must be selected with member
 * functions SetCreatePatchFile() and SetCreateDiffList().
 */
class CDiffWrapper
{
public:
	CDiffWrapper();
	~CDiffWrapper();
	void SetCreatePatchFile(const String &filename);
	void SetCreateDiffList(DiffList *diffList);
	void GetOptions(DIFFOPTIONS *options) const;
	void SetOptions(const DIFFOPTIONS *options);
	void SetTextForAutomaticPrediff(const String &text);
	void SetPrediffer(const PrediffingInfo * prediffer = nullptr);
	void GetPrediffer(PrediffingInfo * prediffer) const;
	void SetPatchOptions(const PATCHOPTIONS *options);
	void SetDetectMovedBlocks(bool bDetectMovedBlocks);
	bool GetDetectMovedBlocks() const { return (m_pMovedLines[0] != nullptr); }
	void SetAppendFiles(bool bAppendFiles);
	void SetPaths(const PathContext &files, bool tempPaths);
	void SetAlternativePaths(const PathContext &altPaths);
	bool RunFileDiff();
	void GetDiffStatus(DIFFSTATUS *status) const;
	void AddDiffRange(DiffList *pDiffList, unsigned begin0, unsigned end0, unsigned begin1, unsigned end1, OP_TYPE op);
	void AddDiffRange(DiffList *pDiffList, DIFFRANGE &dr);
	void FixLastDiffRange(int nFiles, int bufferLines[], bool bMissingNL[], bool bIgnoreBlankLines);
	MovedLines * GetMovedLines(int index) { return m_pMovedLines[index].get(); }
	void SetCompareFiles(const PathContext &originalFile);
	void WritePatchFileHeader(enum output_style output_style, bool bAppendFiles);
	void WritePatchFileTerminator(enum output_style output_style);
	void SetFilterList(const String& filterStr);
	void SetFilterCommentsManager(const FilterCommentsManager *pFilterCommentsManager) { m_pFilterCommentsManager = pFilterCommentsManager; };
	void EnablePlugins(bool enable);
	bool IsTrivialBytes(const char* Start, const char* End,
		const FilterCommentsSet& filtercommentsset) const;
	bool IsTrivialLine(const std::string &Line, const char * StartOfComment,
	   const char * EndOfComment, const char * InLineComment,
	   const FilterCommentsSet& filtercommentsset) const;
	bool PostFilter(int StartPos, int EndPos, int Direction,
		int QtyLinesInBlock, OP_TYPE &Op, int FileNo,
		FilterCommentsSet& filtercommentsset) const;
	void PostFilter(int LineNumberLeft, int QtyLinesLeft, int LineNumberRight,
		int QtyLinesRight, OP_TYPE &Op, const String& FileNameExt) const;

protected:
	String FormatSwitchString() const;
	bool Diff2Files(struct change ** diffs, DiffFileData *diffData,
		int * bin_status, int * bin_file) const;
	void LoadWinMergeDiffsFromDiffUtilsScript(struct change * script, const file_data * inf);
	void WritePatchFile(struct change * script, file_data * inf);
public:
	void LoadWinMergeDiffsFromDiffUtilsScript3(
		struct change * script10, struct change * script12,
		const file_data * inf10, const file_data * inf12);
	static void FreeDiffUtilsScript(struct change * & script);
	bool RegExpFilter(int StartPos, int EndPos, int FileNo) const;

private:
	DiffutilsOptions m_options;
	DIFFSTATUS m_status; /**< Status of last compare */
	std::unique_ptr<FilterList> m_pFilterList; /**< List of linefilters. */
	PathContext m_files; /**< Full path to diff'ed file. */
	PathContext m_alternativePaths; /**< file's alternative path (may be relative). */
	PathContext m_originalFile; /**< file's original (NON-TEMP) path. */

	String m_sPatchFile; /**< Full path to created patch file. */
	bool m_bPathsAreTemp; /**< Are compared paths temporary? */
	/// prediffer info are stored only for MergeDoc
	std::unique_ptr<PrediffingInfo> m_infoPrediffer;
	/// prediffer info are stored only for MergeDoc
	String m_sToFindPrediffer;
	bool m_bUseDiffList; /**< Are results returned in difflist? */
	bool m_bCreatePatchFile; /**< Do we create a patch file? */
	bool m_bAddCmdLine; /**< Do we add commandline to patch file? */
	bool m_bAppendFiles; /**< Do we append to existing patch file? */
	int m_nDiffs; /**< Difference count */
	DiffList *m_pDiffList; /**< Pointer to external DiffList */
	std::unique_ptr<MovedLines> m_pMovedLines[3];
	const FilterCommentsManager* m_pFilterCommentsManager; /**< Comments filtering manager */
	bool m_bPluginsEnabled; /**< Are plugins enabled? */
};

/**
 * @brief Set text tested to find the prediffer automatically.
 * Most probably a concatenated string of both filenames.
 */
inline void CDiffWrapper::SetTextForAutomaticPrediff(const String &text)
{
	m_sToFindPrediffer = text;
}

inline void CDiffWrapper::GetPrediffer(PrediffingInfo * prediffer) const
{
	*prediffer = *m_infoPrediffer;
}

/**
 * @brief Set plugins enabled/disabled.
 * @param [in] enable if true plugins are enabled.
 */
inline void CDiffWrapper::EnablePlugins(bool enable)
{
	m_bPluginsEnabled = enable;
}

/**
 * @brief Set source paths for original (NON-TEMP) diffing two files.
 * Sets full paths to two (NON-TEMP) files we are diffing.
 * @param [in] OriginalFile1 First file to compare "(NON-TEMP) file".
 * @param [in] OriginalFile2 Second file to compare "(NON-TEMP) file".
 */
inline void CDiffWrapper::SetCompareFiles(const PathContext &originalFile)
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
inline void CDiffWrapper::SetAlternativePaths(const PathContext &altPaths)
{
	m_alternativePaths = altPaths;
}

