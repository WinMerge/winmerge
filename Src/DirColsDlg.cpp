/** 
 * @file  DirColsDlg.cpp
 *
 * @brief Implementation file for CDirColsDlg
 *
 * @date  Created: 2003-08-19
 */
// RCS ID line follows -- this is updated by CVS
// $Id$


#include "stdafx.h"
#include "merge.h"
#include "DirColsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirColsDlg dialog


CDirColsDlg::CDirColsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDirColsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDirColsDlg)
	m_bReset = FALSE;
	//}}AFX_DATA_INIT
}


void CDirColsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDirColsDlg)
	DDX_Control(pDX, IDC_LIST_SHOW, m_list_show);
	DDX_Control(pDX, IDC_LIST_HIDE, m_list_hide);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDirColsDlg, CDialog)
	//{{AFX_MSG_MAP(CDirColsDlg)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_COLDLG_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP
	ON_LBN_SELCHANGE(IDC_LIST_SHOW, OnLbnSelchangeListShow)
	ON_LBN_SELCHANGE(IDC_LIST_HIDE, OnLbnSelchangeListHide)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirColsDlg message handlers

/**
 * @brief Dialog initialisation. Load column lists.
 */
BOOL CDirColsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	LoadLists();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Load listboxes on screen from column array
 */
void CDirColsDlg::LoadLists()
{
	for (int i=0; i<m_cols.GetSize(); ++i)
	{
		const column & c = m_cols[i];
		CListBox * list = (c.phy_col >= 0) ? &m_list_show : &m_list_hide;
		int x = list->AddString(m_cols[i].name);
		list->SetItemData(x, c.log_col);
	}
	SortArrayToLogicalOrder();
	UpdateEnables();
}

/**
 * @brief Load listboxes on screen from defaults column array
 */
void CDirColsDlg::LoadDefLists()
{
	for (int i=0; i<m_cols.GetSize(); ++i)
	{
		const column & c = m_defCols[i];
		CListBox * list = (c.phy_col >= 0) ? &m_list_show : &m_list_hide;
		int x = list->AddString(m_defCols[i].name);
		list->SetItemData(x, c.log_col);
	}
	SortArrayToLogicalOrder();
	UpdateEnables();
}

/**
 * @brief Sort m_cols so that it is in logical column order.
 */
void CDirColsDlg::SortArrayToLogicalOrder()
{
	qsort(m_cols.GetData(), m_cols.GetSize(), sizeof(m_cols[0]), &cmpcols);
}

int __cdecl CDirColsDlg::cmpcols(const void * el1, const void * el2)
{
	const column * col1 = reinterpret_cast<const column *>(el1);
	const column * col2 = reinterpret_cast<const column *>(el2);
	return col1->log_col - col2->log_col;
}

/**
 * @brief Move selected items up (in show list)
 */
void CDirColsDlg::OnUp()
{
	CListBox * list = &m_list_show;
	// find the first item not selected
	for (int i=0; i<list->GetCount(); ++i)
	{
		if (!list->GetSel(i))
			break;
	}
	// continue down, moving up all selected items 
	for (++i; i<list->GetCount(); ++i)
	{
		if (list->GetSel(i))
		{
			int data = list->GetItemData(i);
			CString str;
			list->GetText(i, str);
			list->DeleteString(i);
			int inew = i-1;
			list->InsertString(inew, str);
			list->SetItemData(inew, data);
			list->SetSel(inew);
		}
	}
}

/**
 * @brief Move selected items down (in show list)
 */
void CDirColsDlg::OnDown() 
{
	CListBox * list = &m_list_show;
	// find the lst item not selected
	for (int i=list->GetCount()-1; i>=0; --i)
	{
		if (!list->GetSel(i))
			break;
	}
	// continue down, moving up all selected items 
	for (--i; i>=0; --i)
	{
		if (list->GetSel(i))
		{
			int data = list->GetItemData(i);
			CString str;
			list->GetText(i, str);
			list->DeleteString(i);
			int inew = i+1;
			list->InsertString(inew, str);
			list->SetItemData(inew, data);
			list->SetSel(inew);
		}
	}
}

/**
 * @brief Move selected items from hide list to show list
 */
void CDirColsDlg::OnAdd() 
{
	MoveItems(&m_list_hide, &m_list_show, false);
}

/**
 * @brief Move selected items from show list to hide list
 */
void CDirColsDlg::OnRemove() 
{
	MoveItems(&m_list_show, &m_list_hide, true);
}

/**
 * @brief Move selected items from list1 to list2, putting at top if top==true
 */
void CDirColsDlg::MoveItems(CListBox * list1, CListBox * list2, bool top)
{
	for (int i=0; i<list1->GetCount(); ++i)
	{
		if (list1->GetSel(i))
		{
			int data = list1->GetItemData(i);
			CString str;
			list1->GetText(i, str);
			list1->DeleteString(i);
			--i; // new item promoted to slot#i in list1, we need to check it next
			int inew = top ? 0 : list2->GetCount();
			list2->InsertString(inew, str);
			list2->SetItemData(inew, data);
			list2->SetSel(inew);
		}
	}
	UpdateEnables();
}

/**
 * @brief Enable/disable the Add/Remove buttons appropriately
 */
void CDirColsDlg::UpdateEnables()
{
	// TODO: We could enable/disable Up/Down buttons also, but
	// we'd have to trap selection events

	GetDlgItem(IDC_ADD)->EnableWindow(m_list_hide.GetCount() > 0);
	GetDlgItem(IDC_REMOVE)->EnableWindow(m_list_show.GetCount() > 0);
}

/**
 * @brief User clicked ok, so we update m_cols and close
 */
void CDirColsDlg::OnOK() 
{
	// Update all the data in m_cols according to layout on screen
	for (int i=0; i<m_list_show.GetCount(); ++i)
	{
		column * col1 = &m_cols[m_list_show.GetItemData(i)];
		col1->phy_col = i;
	}
	for (i=0; i<m_list_hide.GetCount(); ++i)
	{
		column * col1 = &m_cols[m_list_hide.GetItemData(i)];
		col1->phy_col = -1;
	}
	
	CDialog::OnOK();
}

/**
 * @brief Empty lists and load default columns and order.
 */
void CDirColsDlg::OnDefaults()
{
	m_bReset = TRUE;
	m_list_show.ResetContent();
	m_list_hide.ResetContent();
	LoadDefLists();
}

/**
 * @brief Update description when selected item changes.
 */
void CDirColsDlg::OnLbnSelchangeListShow()
{
	for (int i=0; i<m_list_show.GetCount(); ++i)
	{
		if (m_list_show.GetSel(i))
		{
			int data = m_list_show.GetItemData(i);
			CString str;
			
			for (int j = 0; j < m_cols.GetSize(); j++)
			{
				if (m_cols.GetAt(j).log_col == data)
					break;
			}
			GetDlgItem(IDC_COLDLG_DESC)->SetWindowText(m_cols[j].desc);
			break;
		}
	}
}

/**
 * @brief Update description when selected item changes.
 */
void CDirColsDlg::OnLbnSelchangeListHide()
{
	for (int i=0; i<m_list_hide.GetCount(); ++i)
	{
		if (m_list_hide.GetSel(i))
		{
			int data = m_list_hide.GetItemData(i);
			CString str;
			
			for (int j = 0; j < m_cols.GetSize(); j++)
			{
				if (m_cols.GetAt(j).log_col == data)
					break;
			}
			GetDlgItem(IDC_COLDLG_DESC)->SetWindowText(m_cols[j].desc);
			break;
		}
	}
}
