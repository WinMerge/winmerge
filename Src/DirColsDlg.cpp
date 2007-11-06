/** 
 * @file  DirColsDlg.cpp
 *
 * @brief Implementation file for CDirColsDlg
 *
 * @date  Created: 2003-08-19
 */
// ID line follows -- this is updated by SVN
// $Id$


#include "stdafx.h"
#include "merge.h"
#include "DirColsDlg.h"
#include "dllver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirColsDlg dialog

/**
 * @brief Default dialog constructor.
 * @param [in] pParent Dialog's parent component (window).
 */
CDirColsDlg::CDirColsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDirColsDlg::IDD, pParent)
	, m_bReset(FALSE)
{
}

/**
 * @brief Handle dialog data exchange between controls and variables.
 */
void CDirColsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDirColsDlg)
	DDX_Control(pDX, IDC_COLDLG_LIST, m_listColumns);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDirColsDlg, CDialog)
	//{{AFX_MSG_MAP(CDirColsDlg)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_COLDLG_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_COLDLG_LIST, OnLvnItemchangedColdlgList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirColsDlg message handlers

/**
 * @brief Initialize listcontrol in dialog.
 */
void CDirColsDlg::InitList()
{
	// Show selection across entire row.
	DWORD newstyle = LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT;
	// Also enable infotips if they have new enough version for our
	// custom draw code
	// LPNMLVCUSTOMDRAW->iSubItem not supported before comctl32 4.71
	if (GetDllVersion(_T("comctl32.dll")) >= PACKVERSION(4,71))
		newstyle |= LVS_EX_INFOTIP;
	m_listColumns.SetExtendedStyle(m_listColumns.GetExtendedStyle() | newstyle);
	m_listColumns.InsertColumn(0, _T(""), LVCFMT_LEFT, 150);
}

/**
 * @brief Dialog initialisation. Load column lists.
 */
BOOL CDirColsDlg::OnInitDialog() 
{
	theApp.TranslateDialog(m_hWnd);
	CDialog::OnInitDialog();
	InitList();
	LoadLists();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Load listboxes on screen from column array.
 */
void CDirColsDlg::LoadLists()
{
	for (int i = 0; i < m_cols.GetSize(); i++)
	{
		const column & c = m_cols[i];
		int x = m_listColumns.InsertItem(m_listColumns.GetItemCount(),
			m_cols[i].name);
		m_listColumns.SetItemData(x, c.log_col);
		if (c.phy_col >= 0)
			m_listColumns.SetCheck(x, TRUE);
	}
	SortArrayToLogicalOrder();
	
	// Set first item to selected state
	SelectItem(0);
}

/**
 * @brief Select item in list.
 * @param [in] index Index of item to select.
 */
void CDirColsDlg::SelectItem(int index)
{
	m_listColumns.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
}

/**
 * @brief Load listboxes on screen from defaults column array
 */
void CDirColsDlg::LoadDefLists()
{
	for (int i=0; i<m_cols.GetSize(); ++i)
	{
		const column & c = m_defCols[i];
		int x = m_listColumns.InsertItem(m_listColumns.GetItemCount(),
			m_cols[i].name);
		m_listColumns.SetItemData(x, c.log_col);
		if (c.phy_col >= 0)
			m_listColumns.SetCheck(x, TRUE);
	}
	SortArrayToLogicalOrder();
}
/**
 * @brief Sort m_cols so that it is in logical column order.
 */
void CDirColsDlg::SortArrayToLogicalOrder()
{
	qsort(m_cols.GetData(), m_cols.GetSize(), sizeof(m_cols[0]), &cmpcols);
}

/**
 * @brief Compare column order of two columns.
 * @param [in] el1 First column to compare.
 * @param [in] el2 Second column to compare.
 * @return Column order.
 */
int __cdecl CDirColsDlg::cmpcols(const void * el1, const void * el2)
{
	const column * col1 = reinterpret_cast<const column *>(el1);
	const column * col2 = reinterpret_cast<const column *>(el2);
	return col1->log_col - col2->log_col;
}

/**
 * @brief Move column name in the list.
 * @param [in] index Index of column to move.
 * @param [in] newIndex New index for the column to move.
 */
void CDirColsDlg::MoveItem(int index, int newIndex)
{
	// Get current column data
	CString text = m_listColumns.GetItemText(index, 0);
	BOOL checked = m_listColumns.GetCheck(index);
	UINT state = m_listColumns.GetItemState(index, LVIS_SELECTED);
	DWORD_PTR data = m_listColumns.GetItemData(index);

	// Delete column
	m_listColumns.DeleteItem(index);
	
	// Insert column in new index
	m_listColumns.InsertItem(newIndex, text);
	m_listColumns.SetItemState(newIndex, state, LVIS_SELECTED);
	m_listColumns.SetItemData(newIndex, data);
	if (checked)
		m_listColumns.SetCheck(newIndex);
}

/**
 * @brief Move selected items one position up or down.
 * @param [in] bUp If TRUE items are moved up,
 *  if FALSE items are moved down.
 */
void CDirColsDlg::MoveSelectedItems(BOOL bUp)
{
	int firstInd = -1;
	POSITION pos = m_listColumns.GetFirstSelectedItemPosition();

	while (pos)
	{
		int ind = m_listColumns.GetNextSelectedItem(pos);
		int newInd = bUp ? ind - 1: ind + 1;
		
		// Remember first item
		if (firstInd == -1)
			firstInd = newInd;
		MoveItem(ind, newInd);
	}
	m_listColumns.EnsureVisible(firstInd, FALSE);
}

/**
 * @brief Move selected items up in list.
 */
void CDirColsDlg::OnUp()
{
	MoveSelectedItems(TRUE);
}

/**
 * @brief Move selected items down (in show list)
 */
void CDirColsDlg::OnDown() 
{
	MoveSelectedItems(FALSE);
}

/**
 * @brief Move hidden columns as last items in the list.
 */
void CDirColsDlg::SanitizeOrder()
{
	// Find last visible column.
	int i = m_listColumns.GetItemCount() - 1;
	for ( ; i >= 0; i--)
	{
		if (m_listColumns.GetCheck(i))
			break;
	}

	// Move all hidden columns below last visible column.
	for (int j = i; j >= 0; j--)
	{
		if (!m_listColumns.GetCheck(j))
		{
			MoveItem(j, i);
			i--;
		}
	}
}

/**
 * @brief User clicked ok, so we update m_cols and close.
 */
void CDirColsDlg::OnOK() 
{
	SanitizeOrder();

	for (int i = 0; i < m_listColumns.GetItemCount(); i++)
	{
		BOOL checked = m_listColumns.GetCheck(i);
		DWORD_PTR data = m_listColumns.GetItemData(i);
		column * col1 = &m_cols[data];
		if (checked)
			col1->phy_col = i;
		else
			col1->phy_col = -1;
	}

	CDialog::OnOK();
}

/**
 * @brief Empty lists and load default columns and order.
 */
void CDirColsDlg::OnDefaults()
{
	m_listColumns.DeleteAllItems();
	m_bReset = TRUE;
	LoadDefLists();
}

/**
 * @brief Update description when selected item changes.
 */
void CDirColsDlg::OnLvnItemchangedColdlgList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	POSITION pos = m_listColumns.GetFirstSelectedItemPosition();

	if (pos)
	{
		int ind = m_listColumns.GetNextSelectedItem(pos);
		DWORD_PTR data = m_listColumns.GetItemData(ind);

		int j = 0;
		for (j = 0; j < m_cols.GetSize(); j++)
		{
			if (m_cols.GetAt(j).log_col == data)
				break;
		}
		GetDlgItem(IDC_COLDLG_DESC)->SetWindowText(m_cols[j].desc);

		// Disable Up/Down -buttons when first/last items are selected.
		if (ind == 0)
			GetDlgItem(IDC_UP)->EnableWindow(FALSE);
		else
			GetDlgItem(IDC_UP)->EnableWindow(TRUE);

		if (ind == m_listColumns.GetItemCount() - m_listColumns.GetSelectedCount())
			GetDlgItem(IDC_DOWN)->EnableWindow(FALSE);
		else
			GetDlgItem(IDC_DOWN)->EnableWindow(TRUE);

	}
	*pResult = 0;
}
