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
#include "DiffFileData.h"

namespace CompareEngines
{

/**
 * @brief Default constructor.
 */
DiffUtils::DiffUtils()
		:  m_pDiffWrapper(new ::CDiffWrapper)
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
void DiffUtils::SetCompareOptions(const CompareOptions& options)
{
	DIFFOPTIONS doptions;
	static_cast<const DiffutilsOptions&>(options).GetAsDiffOptions(doptions);
	m_pDiffWrapper->SetOptions(&doptions, true);
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
 * @brief Compare two files (as earlier specified).
 * @return DIFFCODE as a result of compare.
 */
int DiffUtils::CompareFiles(DiffFileData* diffData)
{
	int bin_flag = 0;
	int bin_file = 0; // bitmap for binary files

	// Do the actual comparison (generating a change script)
	struct change *script = nullptr;
	bool success = m_pDiffWrapper->Diff2Files(&script, diffData, & bin_flag, & bin_file);
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
		const bool usefilters = m_pDiffWrapper->GetOptions().m_filterCommentsLines ||
			m_pDiffWrapper->GetOptions().m_bIgnoreMissingTrailingEol ||
			(m_pDiffWrapper->GetFilterList() && m_pDiffWrapper->GetFilterList()->HasRegExps()) ||
			(m_pDiffWrapper->GetSubstitutionList() && m_pDiffWrapper->GetSubstitutionList()->HasRegExps());
	
		PostFilterContext ctxt{};
		String Ext = ucr::toTString(diffData->m_inf[0].name);
		size_t PosOfDot = Ext.rfind('.');
		if (PosOfDot != String::npos)
			Ext.erase(0, PosOfDot + 1);

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
				analyze_hunk (thisob, &first0, &last0, &first1, &last1, &deletes, &inserts, diffData->m_inf);

				/* Reconnect the script so it will all be freed properly.  */
				end->link = next;

				if (deletes!=0 || inserts!=0 || thisob->trivial!=0)
				{
					OP_TYPE op = (deletes == 0 && inserts == 0) ? OP_TRIVIAL : OP_DIFF;

					if (op != OP_TRIVIAL && usefilters)
					{
						m_pDiffWrapper->PostFilter(ctxt, thisob, diffData->m_inf);
					}
				}
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
bool DiffUtils::Diff2Files(struct change ** diffs, DiffFileData *diffData,
		int * bin_status, int * bin_file) const
{
	return m_pDiffWrapper->Diff2Files(diffs, diffData, bin_status, bin_file);
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


} // namespace CompareEngines
