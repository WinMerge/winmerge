/**
 * @file  Wrap_DiffUtils.h
 *
 * @brief Declaration of Wrap_DiffUtils class.
 */
#pragma once

#include <memory>

class CompareOptions;
class FilterList;
class DiffutilsOptions;
struct file_data;
struct FileTextStats;
class FilterCommentsManager;
class CDiffWrapper;

namespace CompareEngines
{

/**
 * @brief A class wrapping GNU diffutils as compare engine.
 *
 * This class needs to have all its data as local copies, not as pointers
 * outside. Lifetime can vary certainly be different from unrelated classes.
 * Filters list being an exception - pcre structs are too complex to easily
 * copy so we'll only keep a pointer to external list.
 */
class DiffUtils
{
public:
	DiffUtils();
	~DiffUtils();
	bool SetCompareOptions(const CompareOptions & options);
	void SetFilterList(FilterList * list);
	void SetFilterCommentsManager(const FilterCommentsManager *pFilterCommentsManager);
	void ClearFilterList();
	void SetFileData(int items, file_data *data);
	int diffutils_compare_files();
	bool RegExpFilter(int StartPos, int EndPos, int FileNo) const;
	void GetDiffCounts(int & diffs, int & trivialDiffs) const;
	void GetTextStats(int side, FileTextStats *stats) const;
	bool Diff2Files(struct change ** diffs, int depth,
			int * bin_status, bool bMovedBlocks, int * bin_file) const;
	void SetCodepage(int codepage) { m_codepage = codepage; }

private:
	std::unique_ptr<DiffutilsOptions> m_pOptions; /**< Compare options for diffutils. */
	FilterList * m_pFilterList; /**< Filter list for line filters. */
	file_data * m_inf; /**< Compared files data (for diffutils). */
	int m_ndiffs; /**< Real diffs found. */
	int m_ntrivialdiffs; /**< Ignored diffs found. */
	int m_codepage; /**< Codepage used in line filter */
	std::unique_ptr<CDiffWrapper> m_pDiffWrapper;
};


} // namespace CompareEngines
