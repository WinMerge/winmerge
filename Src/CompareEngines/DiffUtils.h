/** 
 * @file  DiffUtils.h
 *
 * @brief Declaration of DiffUtils class.
 */
// ID line follows -- this is updated by SVN
// $Id$


#ifndef _DIFF_UTILS_H_
#define _DIFF_UTILS_H_

class CompareOptions;
class FilterList;
class DiffutilsOptions;
struct file_data;
struct FileTextStats;

namespace CompareEngines
{

/**
 * @brief A class wrapping diffutils as compare engine.
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
	void ClearFilterList();
	void SetFileData(int items, file_data *data);
	int diffutils_compare_files();
	bool RegExpFilter(int StartPos, int EndPos, int FileNo);
	void GetDiffCounts(int & diffs, int & trivialDiffs);
	void GetTextStats(int side, FileTextStats *stats);
	BOOL Diff2Files(struct change ** diffs, int depth,
		int * bin_status, BOOL bMovedBlocks, int * bin_file);

protected:
	void ClearCompareOptions();

private:
	DiffutilsOptions *m_pOptions; /**< Compare options for diffutils. */
	FilterList * m_pFilterList; /**< Filter list for line filters. */
	file_data * m_inf; /**< Compared files data (for diffutils). */
	int m_ndiffs; /**< Real diffs found. */
	int m_ntrivialdiffs; /**< Ignored diffs found. */
};


} // namespace CompareEngines

#endif // _DIFF_UTILS_H_
