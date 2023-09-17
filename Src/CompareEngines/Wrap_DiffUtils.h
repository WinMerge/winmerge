/**
 * @file  Wrap_DiffUtils.h
 *
 * @brief Declaration of Wrap_DiffUtils class.
 */
#pragma once

#include <memory>

class FilterList;
class SubstitutionList;
class CompareOptions;
struct FileTextStats;
class CDiffWrapper;
struct DiffFileData;

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

	void SetCodepage(int codepage);
	void SetCompareOptions(const CompareOptions& options);
	void SetFilterList(std::shared_ptr<FilterList> plist);
	void ClearFilterList();
	void SetSubstitutionList(std::shared_ptr<SubstitutionList> plist);
	void ClearSubstitutionList();

	int CompareFiles(DiffFileData* diffData);
	bool Diff2Files(struct change ** diffs, DiffFileData *diffData,
			int * bin_status, int * bin_file) const;

	void GetDiffCounts(int & diffs, int & trivialDiffs) const;

private:
	int m_ndiffs; /**< Real diffs found. */
	int m_ntrivialdiffs; /**< Ignored diffs found. */
	std::unique_ptr<CDiffWrapper> m_pDiffWrapper;
};


} // namespace CompareEngines
