/** 
 * @file  DirViewColItems.h
 *
 * @brief Declaration file for DirColInfo
 *
 * @date  Created: 2003-08-19
 */
#pragma once

#include "UnicodeString.h"
#include <vector>
#include <sstream>

class DIFFITEM;
class CDiffContext;

// DirViewColItems typedefs
typedef String (*ColGetFncPtrType)(const CDiffContext *, const void *, int);
typedef int (*ColSortFncPtrType)(const CDiffContext *, const void *, const void *, int);


/**
 * @brief Information about one column of dirview list info
 */
struct DirColInfo
{
	enum ColAlign
	{
		ALIGN_LEFT = 0,   // LVCFMT_LEFT
		ALIGN_RIGHT = 1,  // LVCFMT_RIGHT
		ALIGN_CENTER = 2  // LVCFMT_CENTER
	};
	const tchar_t *regName; /**< Internal name used for registry entries etc */
	// localized string resources
	const char *idNameContext; 
	const char *idName; /**< Displayed name, ID of string resource */
	const char *idDesc; /**< Description, ID of string resource */
	ColGetFncPtrType getfnc; /**< Handler giving display string */
	ColSortFncPtrType sortfnc; /**< Handler for sorting this column */
	size_t offset;	/**< Offset into DIFFITEM::diffFileInfo[] */
	int physicalIndex; /**< Current physical index, -1 if not displayed */
	bool defSortUp; /**< Does column start with ascending sort (most do) */
	int alignment; /**< Column alignment */
	int opt;
	String GetDisplayName() const;
	String GetDescription() const;
};

extern const int g_ncols;
extern const int g_ncols3;

class DirViewColItems
{
public:
	explicit DirViewColItems(int nDirs, const std::vector<String>& additionalPropertyNames);
	String GetColRegValueNameBase(int col) const;
	int GetColDefaultOrder(int col) const;
	const DirColInfo * GetDirColInfo(int col) const;
	bool IsColById(int col, const char *idname) const;
	bool IsColName(int col) const;
	bool IsColLmTime(int col) const;
	bool IsColMmTime(int col) const;
	bool IsColRmTime(int col) const;
	bool IsColStatus(int col) const;
	bool IsColStatusAbbr(int col) const;
	bool IsDefaultSortAscending(int col) const;
	String GetColDisplayName(int col) const;
	String GetColDescription(int col) const;
	int	GetColCount() const { return m_numcols; };
	int GetDispColCount() const { return m_dispcols; }
	String ColGetTextToDisplay(const CDiffContext *pCtxt, int col, const DIFFITEM &di) const;
	int ColSort(const CDiffContext *pCtxt, int col, const DIFFITEM &ldi, const DIFFITEM &rdi, bool bTreeMode) const;

	int ColPhysToLog(int i) const { return m_invcolorder[i]; }
	int ColLogToPhys(int i) const { return m_colorder[i]; } /**< -1 if not displayed */
	void ClearColumnOrders();
	void MoveColumn(int psrc, int pdest);
	void ResetColumnOrdering();
	void SetColumnOrdering(const int colorder[]);
	String ResetColumnWidths(int defcolwidth);
	void LoadColumnOrders(const String& colOrders);
	String SaveColumnOrders();
	const std::vector<String>& GetAdditionalPropertyNames() const { return m_additionalPropertyNames; }
	void SetAdditionalPropertyNames(const std::vector<String>& propertyNames);

	/// Update all column widths (from registry to screen)
	// Necessary when user reorders columns
	template<class SetColumnWidthFunc>
	void LoadColumnWidths(String colwidths, SetColumnWidthFunc setcolwidth, int defcolwidth)
	{
		std::basic_istringstream<tchar_t> ss(colwidths);
		for (int i = 0; i < m_numcols; ++i)
		{
			int phy = ColLogToPhys(i);
			if (phy >= 0)
			{
				int w = defcolwidth;
				ss >> w;
				setcolwidth(phy, (std::max)(w, 10));
			}
		}
	}

	/** @brief store current column widths into registry */
	template<class GetColumnWidthFunc>
	String SaveColumnWidths(GetColumnWidthFunc getcolwidth)
	{
		String result;
		for (int i = 0; i < m_numcols; i++)
		{
			int phy = ColLogToPhys(i);
			if (phy >= 0)
			{
				if (!result.empty()) result += ' ';
				result += strutils::to_str(getcolwidth(phy));
			}
		}
		return result;
	}


private:
	void AddAdditionalPropertyName(const String& propertyName);
	void RemoveAdditionalPropertyName(const String& propertyName);

	int m_nDirs;
	int m_numcols;
	int m_dispcols;
	std::vector<int> m_colorder; /**< colorder[logical#]=physical# */
	std::vector<int> m_invcolorder; /**< invcolorder[physical]=logical# */
	std::vector<DirColInfo> m_cols;
	std::vector<String> m_additionalPropertyNames;
	std::list<String> m_strpool;
};
