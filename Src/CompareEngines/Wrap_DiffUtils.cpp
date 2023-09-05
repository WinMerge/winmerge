/**
 * @file  Wrap_DiffUtils.cpp
 *
 * @brief Implementation file for Wrap_DiffUtils class.
 */

#include "pch.h"
#include "diff.h"
#include "Wrap_DiffUtils.h"
#include <map>
#include <algorithm>
#include <cassert>
#include <sstream>
#include <vector>
#include "DiffContext.h"
#include "Exceptions.h"
#include "FilterList.h"
#include "CompareOptions.h"
#include "coretools.h"
#include "DiffList.h"
#include "DiffWrapper.h"
#include "xdiff_gnudiff_compat.h"
#include "unicoder.h"

namespace CompareEngines
{
static void CopyTextStats(const file_data * inf, FileTextStats * myTextStats);

/**
 * @brief Default constructor.
 */
DiffUtils::DiffUtils()
		: m_pOptions(nullptr)
		, m_inf(nullptr)
		, m_pDiffWrapper(new ::CDiffWrapper)
		, m_ndiffs(0)
		, m_ntrivialdiffs(0)
{
}

/**
 * @brief Default destructor.
 */
DiffUtils::~DiffUtils()
{
}

/**
 * @brief Set compare options from general compare options.
 * @param [in ]options General compare options.
 */
void DiffUtils::SetCompareOptions(const CompareOptions & options)
{
	m_pOptions.reset(new DiffutilsOptions(static_cast<const DiffutilsOptions&>(options)));
	m_pOptions->SetToDiffUtils();
}

/**
 * @brief Clear current filters list.
 * Don't delete the list as it points to external list.
 */
void DiffUtils::ClearFilterList()
{
	m_pDiffWrapper->SetFilterList(static_cast<std::shared_ptr<FilterList>>(nullptr));
}

/**
 * @brief Set line filters list to use.
 * @param [in] list List of line filters.
 */
void DiffUtils::SetFilterList(std::shared_ptr<FilterList> list)
{
	m_pDiffWrapper->SetFilterList(list);
}

void DiffUtils::SetSubstitutionList(std::shared_ptr<SubstitutionList> list)
{
	m_pDiffWrapper->SetSubstitutionList(list);
}

void DiffUtils::ClearSubstitutionList()
{
	m_pDiffWrapper->SetSubstitutionList(nullptr);
}

void DiffUtils::SetCodepage(int codepage)
{
	m_pDiffWrapper->SetCodepage(codepage);
}

/**
 * @brief Set filedata.
 * @param [in] items Count of filedata items to set.
 * @param [in] data File data.
 */
void DiffUtils::SetFileData(int items, file_data *data)
{
	// We support only two files currently!
	assert(items == 2);
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
	struct change *script = nullptr;
	bool success = Diff2Files(&script, 0, &bin_flag, false, &bin_file);
	if (!success)
	{
		return DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::CMPERR;
	}
	unsigned code = DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::SAME;

	// make sure to start counting diffs at 0
	// (usually it is -1 at this point, for unknown)
	m_ndiffs = 0;
	m_ntrivialdiffs = 0;

	if (script != nullptr)
	{
		const bool usefilters = m_pOptions->m_filterCommentsLines ||
			(m_pDiffWrapper->GetFilterList() && m_pDiffWrapper->GetFilterList()->HasRegExps()) ||
			(m_pDiffWrapper->GetSubstitutionList() && m_pDiffWrapper->GetSubstitutionList()->HasRegExps());
	
		PostFilterContext ctxt{};
		String Ext = ucr::toTString(m_inf[0].name);
		size_t PosOfDot = Ext.rfind('.');
		if (PosOfDot != String::npos)
			Ext.erase(0, PosOfDot + 1);

		DIFFOPTIONS options = {0};
		m_pOptions->GetAsDiffOptions(options);
		m_pDiffWrapper->SetOptions(&options);
		m_pDiffWrapper->SetFilterCommentsSourceDef(Ext);

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
#ifdef _DEBUG
			debug_script(thisob);
#endif

			{
				/* Determine range of line numbers involved in each file.  */
				int first0 = 0, last0 = 0, first1 = 0, last1 = 0, deletes = 0, inserts = 0;
				analyze_hunk (thisob, &first0, &last0, &first1, &last1, &deletes, &inserts, m_inf);
				if (deletes!=0 || inserts!=0 || thisob->trivial!=0)
				{
					/* Print the lines that the first file has.  */
					int trans_a0 = 0, trans_b0 = 0, trans_a1 = 0, trans_b1 = 0;
					translate_range(&m_inf[0], first0, last0, &trans_a0, &trans_b0);
					translate_range(&m_inf[1], first1, last1, &trans_a1, &trans_b1);
	
					//Determine quantity of lines in this block for both sides
					int QtyLinesLeft = (trans_b0 - trans_a0) + 1;
					int QtyLinesRight = (trans_b1 - trans_a1) + 1;
					if (usefilters)
					{
						OP_TYPE op = OP_NONE;
						if (deletes == 0 && inserts == 0)
							op = OP_TRIVIAL;
						else
							op = OP_DIFF;
						m_pDiffWrapper->PostFilter(ctxt, trans_a0 - 1, QtyLinesLeft, trans_a1 - 1, QtyLinesRight, op, m_inf);
						if(op == OP_TRIVIAL)
						{
							thisob->trivial = 1;
						}
					}
				}
				/* Reconnect the script so it will all be freed properly.  */
				end->link = next;
			}
		}
	}


	// Free change script (which we don't want)
	if (script != nullptr)
	{
		struct change *p, *e;
		for (e = script; e; e = p)
		{
			if (!e->trivial)
				++m_ndiffs;
			else
				++m_ntrivialdiffs;
			p = e->link;
			free(e);
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
		case BINFILE_SIDE1:
			code |= DIFFCODE::BIN | DIFFCODE::BINSIDE1;
			break;
		case BINFILE_SIDE2:
			code |= DIFFCODE::BIN | DIFFCODE::BINSIDE2;
			break;
		case BINFILE_SIDE1 | BINFILE_SIDE2:
			code |= DIFFCODE::BIN | DIFFCODE::BINSIDE1 | DIFFCODE::BINSIDE2;
			break;
		default:
			std::ostringstream ss;
			ss << "Invalid bin_file value: " << bin_file;
			throw ss.str();
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
 * @brief Compare two files using diffutils.
 *
 * Compare two files (in DiffFileData param) using diffutils. Run diffutils
 * inside SEH so we can trap possible error and exceptions. If error or
 * execption is trapped, return compare failure.
 * @param [out] diffs Pointer to list of change structs where diffdata is stored.
 * @param [in] depth Depth in folder compare (we use 0).
 * @param [out] bin_status used to return binary status from compare.
 * @param [in] bMovedBlocks If `true` moved blocks are analyzed.
 * @param [out] bin_file Returns which file was binary file as bitmap.
    So if first file is binary, first bit is set etc. Can be `nullptr` if binary file
    info is not needed (faster compare since diffutils don't bother checking
    second file if first is binary).
 * @return `true` when compare succeeds, `false` if error happened during compare.
 */
bool DiffUtils::Diff2Files(struct change ** diffs, int depth,
		int * bin_status, bool bMovedBlocks, int * bin_file) const
{
	bool bRet = true;
	SE_Handler seh;
	try
	{
		if (m_pOptions->m_diffAlgorithm != DIFF_ALGORITHM_DEFAULT)
		{
			const unsigned xdl_flags = make_xdl_flags(*m_pOptions);
			*diffs = diff_2_files_xdiff(m_inf, bin_status, bMovedBlocks, bin_file, xdl_flags);
			files[0] = m_inf[0];
			files[1] = m_inf[1];
		}
		else
		{
			*diffs = diff_2_files(m_inf, depth, bin_status, bMovedBlocks, bin_file);
		}
	}
	catch (SE_Exception&)
	{
		*diffs = nullptr;
		bRet = false;
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
	myTextStats->nzeros = inf->count_zeros;
}

/**
 * @brief Return diff counts for last compare.
 * @param [out] diffs Count of real differences.
 * @param [out] trivialDiffs Count of ignored differences.
 */
void DiffUtils::GetDiffCounts(int & diffs, int & trivialDiffs) const
{
	diffs = m_ndiffs;
	trivialDiffs = m_ntrivialdiffs;
}

/**
 * @brief Return text statistics for last compare.
 * @param [in] side For which file to return statistics.
 * @param [out] stats Stats as asked.
 */
void DiffUtils::GetTextStats(int side, FileTextStats *stats) const
{
	CopyTextStats(&m_inf[side], stats);
}

} // namespace CompareEngines
