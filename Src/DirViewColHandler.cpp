/** 
 * @file  DirViewColHandler.cpp
 *
 * @brief Methods of CDirView dealing with the listview (of file results)
 *
 * @date  Created: 2003-08-19
 */
// RCS ID line follows -- this is updated by CVS
// $Id$


#include "stdafx.h"
#include "Merge.h"
#include "DirView.h"
#include "DirDoc.h"
#include "MainFrm.h"
#include "resource.h"
#include "coretools.h"
#include "dllver.h"
#include "DirViewColItems.h"
#include "DirColsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////



/**
 * @brief Get text for specified column (forwards to specific column handler)
 */
static CString ColGet(int col, const DIFFITEM & di)
{
	return (*g_cols[col].getfnc)(di);
}

/**
 * @brief Sort two items on specified column (forwards to specific column handler)
 */
static int ColSort(int col, const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	return (*g_cols[col].sortfnc)(ldi, rdi);
}

/**
 * @brief return whether column normally sorts ascending (dates do not)
 */
bool CDirView::IsDefaultSortAscending(int col) const
{
	return g_cols[col].defSortUp;
}

/// Assign column name, using string resource & current column ordering
void CDirView::NameColumn(int id, int subitem)
{
	int phys = ColLogToPhys(subitem);
	if (phys>=0)
	{
		CString s;
		VERIFY(s.LoadString(id));
		LV_COLUMN lvc;
		lvc.mask = LVCF_TEXT;
		lvc.pszText = (LPTSTR)((LPCTSTR)s);
		m_pList->SetColumn(m_colorder[subitem], &lvc);
	}
}

/// Load column names from string table
void CDirView::UpdateColumnNames()
{
	for (int i=0; i<g_ncols; ++i)
	{
		const DirColInfo & col = g_cols[i];
		NameColumn(col.idName, i);
	}
}

/// Compare two specified rows during a sort operation (windows callback)
int CALLBACK CDirView::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	// initialize structures to obtain required information
	CDirView* pView = reinterpret_cast<CDirView*>(lParamSort);
	POSITION diffposl = pView->GetItemKeyFromData(lParam1);
	POSITION diffposr = pView->GetItemKeyFromData(lParam2);
	DIFFITEM ldi = pView->GetDiffContext()->GetDiffAt(diffposl);
	DIFFITEM rdi = pView->GetDiffContext()->GetDiffAt(diffposr);

	// compare 'left' and 'right' parameters as appropriate
	int retVal = ColSort(pView->m_sortColumn, ldi, rdi);

	// return compare result, considering sort direction
	return (pView->m_bSortAscending)?retVal:-retVal;
}

// Add new item to list view
int CDirView::AddNewItem(int i)
{
	LV_ITEM lvItem;
	memset(&lvItem, 0, sizeof(lvItem));
	lvItem.iItem = i;
	return GetListCtrl().InsertItem(&lvItem);
  
}

// Set a subitem on an existing item
void CDirView::SetSubitem(int item, int phy, LPCTSTR sz)
{
	LV_ITEM lvItem;
	memset(&lvItem, 0, sizeof(lvItem));
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = item;
	lvItem.iSubItem = phy;
	lvItem.pszText = const_cast<LPTSTR>(sz);
	GetListCtrl().SetItem(&lvItem);
}


// Add a new diff item to dir view
int CDirView::AddDiffItem(int index, const DIFFITEM & di, LPCTSTR szPath, POSITION curdiffpos)
{
	int i = AddNewItem(index);
	SetItemKey(i, curdiffpos);
	SetImage(i, FILE_ERROR);
	return i;
}

/**
 * @brief Return image index appropriate for this row
 */
static int GetColImage(const DIFFITEM & di)
{
	switch (di.code)
	{
	case FILE_DIFF: return FILE_DIFF;
	case FILE_BINDIFF: return FILE_BINDIFF;
	case FILE_BINSAME: return FILE_BINSAME;
	case FILE_LUNIQUE:
	case FILE_LDIRUNIQUE:
		return di.code;
	case FILE_RUNIQUE:
	case FILE_RDIRUNIQUE:
		return di.code;
		break;
	case FILE_SAME: return FILE_SAME;
	default: return FILE_ERROR;
	}
}

// Update listview display of details for specified row
void CDirView::UpdateDiffItemStatus(UINT nIdx, const DIFFITEM & di)
{
	for (int i=0; i<g_ncols; ++i)
	{
		const DirColInfo & col = g_cols[i];
		int phy = ColLogToPhys(i);
		if (phy>=0)
			SetSubitem(nIdx, phy, ColGet(i, di));
	}
	SetImage(nIdx, GetColImage(di));
}

/// store current column orders into registry
void CDirView::SaveColumnOrders()
{
	ASSERT(m_colorder.GetSize() == m_numcols);
	ASSERT(m_invcolorder.GetSize() == m_numcols);
	int cols = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (int i=0; i < m_numcols; i++)
	{
		CString RegName = GetColRegValueNameBase(i) + _T("_Order");
		int ord = m_colorder[i];
		theApp.WriteProfileInt(_T("DirView"), RegName, ord);
	}
}


/**
 * @brief Load column orders from registry
 */
void CDirView::LoadColumnOrders()
{
	ASSERT(m_numcols == -1);
	m_numcols = GetColLogCount();
	ClearColumnOrders();
	m_dispcols = 0;

	// Load column orders
	// Break out if one is missing
	// Break out & mark failure (m_dispcols == -1) if one is invalid
	for (int i=0; i<m_numcols; ++i)
	{
		CString RegName = GetColRegValueNameBase(i) + _T("_Order");
		int ord = theApp.GetProfileInt(_T("DirView"), RegName, -2);
		if (ord<-1 || ord >= m_numcols)
			break;
		m_colorder[i] = ord;
		if (ord>=0)
		{
			++m_dispcols;
			if (m_invcolorder[ord] != -1)
			{
				m_dispcols = -1;
				break;
			}
			m_invcolorder[ord] = i;
		}
	}
	// Check that a contiguous range was set
	for (i=0; i<m_dispcols; ++i)
	{
		if (m_invcolorder[i] < 0)
		{
			m_dispcols = -1;
			break;
		}
	}
	// Must have at least one column
	if (m_dispcols<=1)
	{
		ResetColumnOrdering();
	}

	ValidateColumnOrdering();
}

/**
 * @brief Sanity check column ordering
 */
void CDirView::ValidateColumnOrdering()
{
TRACE(" <%s>\n", COleDateTime::GetCurrentTime().Format());
for (int k=0; k<m_invcolorder.GetSize();++k)
	TRACE("invcolorder[%d]=%d\n", k, m_invcolorder[k]);
for (k=0; k<m_colorder.GetSize();++k)
	TRACE("colorder[%d]=%d\n", k, m_colorder[k]);

#if _DEBUG
	ASSERT(m_invcolorder[0]>=0);
	ASSERT(m_numcols == GetColLogCount());
	// Check that any logical->physical mapping is reversible
	for (int i=0; i<m_numcols; ++i)
	{
		int phy = m_colorder[i];
		if (phy >= 0)
		{
			int log = m_invcolorder[phy];
			ASSERT(i == log);
		}
	}
	// Bail out if header doesn't exist yet
	int hdrcnt = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	if (hdrcnt)
	{
		ASSERT(hdrcnt == m_dispcols);
	}
		return;
	// Check that all physical columns map to logical columns
	for (i=0; i<m_dispcols; ++i)
	{
		ASSERT(m_invcolorder[i]>=0);
	}
	// Check that no columns beyond end have physical mappings
	for ( /* existing i */; i<m_numcols; ++i)
	{
		ASSERT(m_invcolorder[i]==-1);
	}
#endif
}

/**
 * @brief Set column ordering to default initial order
 */
void CDirView::ResetColumnOrdering()
{
	ClearColumnOrders();
	m_dispcols = 0;
	for (int i=0; i<m_numcols; ++i)
	{
		int phy = GetColDefaultOrder(i);
		m_colorder[i] = phy;
		if (phy>=0)
		{
			m_invcolorder[phy] = i;
			++m_dispcols;
		}
	}
	ValidateColumnOrdering();
}

/**
 * @brief Reset all current column ordering information
 */
void CDirView::ClearColumnOrders()
{
	m_colorder.SetSize(m_numcols);
	m_invcolorder.SetSize(m_numcols);
	for (int i=0; i<m_numcols; ++i)
	{
		m_colorder[i] = -1;
		m_invcolorder[i] = -1;
	}
}

/**
 * @brief Return display name of column
 */
CString CDirView::GetColDisplayName(int col) const
{
	const DirColInfo & colinfo = g_cols[col];
	CString s;
	s.LoadString(colinfo.idName);
	return s;
}

/**
 * @brief Return total number of known columns
 */
int CDirView::GetColLogCount() const
{
	return g_ncols;
}

/**
 * @brief Remove any windows reordering of columns (params are physical columns)
 */
void CDirView::MoveColumn(int psrc, int pdest)
{
	// actually moved column
	m_colorder[m_invcolorder[psrc]] = pdest;
	// shift all other affected columns
	int dir = psrc > pdest ? +1 : -1;
	for (int i=pdest; i!=psrc; i += dir)
	{
		m_colorder[m_invcolorder[i]] = i+dir;
	}
	// fix inverse mapping
	for (i=0; i<m_numcols; ++i)
	{
		if (m_colorder[i] >= 0)
			m_invcolorder[m_colorder[i]] = i;
	}
	ValidateColumnOrdering();
	InitiateSort();
	ValidateColumnOrdering();
}

/**
 * @brief User examines & edits which columns are displayed in dirview, and in which order
 */
void CDirView::OnEditColumns()
{
ToDoDeleteThisValidateColumnOrdering();
	CDirColsDlg dlg;
	// List all the currently displayed columns
	for (int col=0; col<GetListCtrl().GetHeaderCtrl()->GetItemCount(); ++col)
	{
		int l = ColPhysToLog(col);
		dlg.AddColumn(GetColDisplayName(l), l, col);
	}
	// Now add all the columns not currently displayed
	for (int l=0; l<GetColLogCount(); ++l)
	{
		if (ColLogToPhys(l)==-1)
		{
			dlg.AddColumn(GetColDisplayName(l), l);
		}
	}
	if (dlg.DoModal() != IDOK)
		return;

	// Reset our data to reflect the new data from the dialog
	const CDirColsDlg::ColumnArray & cols = dlg.GetColumns();
	ClearColumnOrders();
	m_dispcols = 0;
	for (int i=0; i<cols.GetSize(); ++i)
	{
		int log = cols[i].log_col;
		int phy = cols[i].phy_col;
		m_colorder[log] = phy;
		if (phy>=0)
		{
			++m_dispcols;
			m_invcolorder[phy] = log;
		}
	}
	if (m_dispcols < 1)
	{
		// Ignore them if they didn't leave a column showing
		ResetColumnOrdering();
	}
	else
	{
		ReloadColumns();
		GetDocument()->Redisplay();
	}
	ValidateColumnOrdering();
}
