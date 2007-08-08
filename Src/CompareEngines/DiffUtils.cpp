/** 
 * @file  FolderCmp.cpp
 *
 * @brief Implementation file for FolderCmp
 */
// ID line follows -- this is updated by SVN
// $Id: FolderCmp.cpp 4398 2007-08-01 19:53:59Z kimmov $


#include "stdafx.h"
#include "CompareOptions.h"
#include "FilterList.h"
#include "DiffContext.h"
#include "FileTransform.h"
#include "diff.h"
#include "DiffUtils.h"

namespace CompareEngines
{
static void CopyTextStats(const file_data * inf, FileTextStats * myTextStats);

/**
 * @brief Default constructor.
 */
DiffUtils::DiffUtils()
 : m_pOptions(NULL)
 , m_pFilterList(NULL)
 , m_inf(NULL)
{
}

/**
 * @brief Default destructor.
 */
DiffUtils::~DiffUtils()
{
	ClearCompareOptions();
	ClearFilterList();
}

/**
 * @brief Set compare options from general compare options.
 * @param [in ]options General compare options.
 * @return true if succeeded, false otherwise.
 */
bool DiffUtils::SetCompareOptions(const CompareOptions & options)
{
	if (m_pOptions != NULL)
		ClearCompareOptions();

	m_pOptions = new DiffutilsOptions(options);
	if (m_pOptions == NULL)
		return false;

	m_pOptions->SetToDiffUtils();
	return true;
}

/**
 * @brief Clear current compare options.
 */
void DiffUtils::ClearCompareOptions()
{
	delete m_pOptions;
	m_pOptions = NULL;
}

/**
 * @brief Clear current filters list.
 * Don't delete the list as it points to external list.
 */
void DiffUtils::ClearFilterList()
{
	m_pFilterList = NULL;
}

/**
 * @brief Set line filters list to use.
 * @param [in] list List of line filters.
 */
void DiffUtils::SetFilterList(FilterList * list)
{
	m_pFilterList = list;
}

/**
 * @brief Set filedata.
 * @param [in] items Count of filedata items to set.
 * @param [in] data File data.
 */
void DiffUtils::SetFileData(int items, file_data *data)
{
	// We support only two files currently!
	ASSERT(items == 2);
	m_inf = data;
}

/**
 * @brief Compare two files (as earlier specified).
 * @return DIFFCODE as a result of compare.
 */
int DiffUtils::diffutils_compare_files()
{
	int bin_flag = 0;
	int bin_file = 0; // bitmap for binary files

	// Do the actual comparison (generating a change script)
	struct change *script = NULL;
	BOOL success = Diff2Files(&script, 0, &bin_flag, FALSE, &bin_file);
	if (!success)
	{
		return DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::CMPERR;
	}
	int code = DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::SAME;

	// make sure to start counting diffs at 0
	// (usually it is -1 at this point, for unknown)
	m_ndiffs = 0;
	m_ntrivialdiffs = 0;

	if (script && m_pFilterList && m_pFilterList->HasRegExps())
	{
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

			{					
				/* Determine range of line numbers involved in each file.  */
				int first0=0, last0=0, first1=0, last1=0, deletes=0, inserts=0;
				analyze_hunk (thisob, &first0, &last0, &first1, &last1, &deletes, &inserts);
//				int op=0;
				if (deletes || inserts || thisob->trivial)
				{
/*					if (deletes && inserts)
						op = OP_DIFF;
					else if (deletes)
						op = OP_LEFTONLY;
					else if (inserts)
						op = OP_RIGHTONLY;
					else
						op = OP_TRIVIAL;
*/					
					/* Print the lines that the first file has.  */
					int trans_a0=0, trans_b0=0, trans_a1=0, trans_b1=0;
					translate_range(&m_inf[0], first0, last0, &trans_a0, &trans_b0);
					translate_range(&m_inf[1], first1, last1, &trans_a1, &trans_b1);

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
						//op = OP_TRIVIAL;
						thisob->trivial = 1;

				}
				/* Reconnect the script so it will all be freed properly.  */
				end->link = next;
			}
		}
	}


	// Free change script (which we don't want)
	if (script != NULL)
	{
		struct change *p,*e;
		for (e = script; e; e = p)
		{
			if (!e->trivial)
				++m_ndiffs;
			else
				++m_ntrivialdiffs;
			p = e->link;
			free (e);
		}
		if (m_ndiffs > 0)
			code = code & ~DIFFCODE::SAME | DIFFCODE::DIFF;
	}

	// diff_2_files set bin_flag to -1 if different binary
	// diff_2_files set bin_flag to +1 if same binary

	if (bin_flag != 0)
	{
		// Clear text-flag, set binary flag
		// We don't know diff counts for binary files
		code = code & ~DIFFCODE::TEXT;
		switch (bin_file)
		{
		case BINFILE_SIDE1: code |= DIFFCODE::BINSIDE1;
			break;
		case BINFILE_SIDE2: code |= DIFFCODE::BINSIDE2;
			break;
		case BINFILE_SIDE1 | BINFILE_SIDE2: code |= DIFFCODE::BIN;
			break;
		default:
			_RPTF1(_CRT_ERROR, "Invalid bin_file value: %d", bin_file);
			break;
		}
		m_ndiffs = CDiffContext::DIFFS_UNKNOWN;
	}

	if (bin_flag < 0)
	{
		// Clear same-flag, set diff-flag
		code = code & ~DIFFCODE::SAME | DIFFCODE::DIFF;
	}

	return code;
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
bool DiffUtils::RegExpFilter(int StartPos, int EndPos, int FileNo)
{
	if (m_pFilterList == NULL)
	{	
		_RPTF0(_CRT_ERROR, "DiffUtils::RegExpFilter() called when "
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
 * @brief Compare two files using diffutils.
 *
 * Compare two files (in DiffFileData param) using diffutils. Run diffutils
 * inside SEH so we can trap possible error and exceptions. If error or
 * execption is trapped, return compare failure.
 * @param [out] diffs Pointer to list of change structs where diffdata is stored.
 * @param [in] depth Depth in folder compare (we use 0).
 * @param [out] bin_status used to return binary status from compare.
 * @param [in] bMovedBlocks If TRUE moved blocks are analyzed.
 * @param [out] bin_file Returns which file was binary file as bitmap.
    So if first file is binary, first bit is set etc. Can be NULL if binary file
    info is not needed (faster compare since diffutils don't bother checking
    second file if first is binary).
 * @return TRUE when compare succeeds, FALSE if error happened during compare.
 */
BOOL DiffUtils::Diff2Files(struct change ** diffs, int depth,
	int * bin_status, BOOL bMovedBlocks, int * bin_file)
{
	BOOL bRet = TRUE;
	__try
	{
		*diffs = diff_2_files (m_inf, depth, bin_status, bMovedBlocks, bin_file);
//		CopyDiffutilTextStats(m_inf, &m_diffFileData);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		*diffs = NULL;
		bRet = FALSE;
	}
	return bRet;
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
 * @brief Return diff counts for last compare.
 * @param [out] diffs Count of real differences.
 * @param [out] trivialDiffs Count of ignored differences.
 */
void DiffUtils::GetDiffCounts(int & diffs, int & trivialDiffs)
{
	diffs = m_ndiffs;
	trivialDiffs = m_ntrivialdiffs;
}

/**
 * @brief Return text statistics for last compare.
 * @param [in] side For which file to return statistics.
 * @param [out] stats Stats as asked.
 */
void DiffUtils::GetTextStats(int side, FileTextStats *stats)
{
	CopyTextStats(&m_inf[side], stats);
}

} // namespace CompareEngines
