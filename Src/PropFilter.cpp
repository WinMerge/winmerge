// PropFilter.cpp : implementation file
//

#include "stdafx.h"
#include "merge.h"
#include "PropFilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropFilter property page

IMPLEMENT_DYNAMIC(CPropFilter, CPropertyPage)

CPropFilter::CPropFilter(const StringPairArray * fileFilters, CString & selected)
: CPropertyPage(CPropFilter::IDD)
, m_Filters(fileFilters)
{
	//{{AFX_DATA_INIT(CPropFilter)
	m_bIgnoreRegExp = FALSE;
	m_sPattern = _T("");
	//}}AFX_DATA_INIT
	m_sFileFilterPath = selected;
}

CPropFilter::~CPropFilter()
{
}

void CPropFilter::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropFilter)
	DDX_Control(pDX, IDC_EDIT_FILE_FILTER, m_btnEditFileFilter);
	DDX_Control(pDX, IDC_FILE_FILTER, m_cboFileFilter);
	DDX_Control(pDX, IDC_EDITPATTERN, m_cPattern);
	DDX_Check(pDX, IDC_IGNOREREGEXP, m_bIgnoreRegExp);
	DDX_Text(pDX, IDC_EDITPATTERN, m_sPattern);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropFilter, CPropertyPage)
	//{{AFX_MSG_MAP(CPropFilter)
	ON_BN_CLICKED(IDC_IGNOREREGEXP, OnIgnoreregexp)
	ON_CBN_SELCHANGE(IDC_FILE_FILTER, OnSelchangeFileFilter)
	ON_BN_CLICKED(IDC_EDIT_FILE_FILTER, OnEditFileFilter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropFilter message handlers

BOOL CPropFilter::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	AddFilter(-1);
	int sel = 0;
	if (m_Filters)
	{
		for (int i=0; i<m_Filters->GetSize(); ++i)
		{
			int index = AddFilter(i);
			if (m_sFileFilterPath == m_Filters->GetAt(i).first)
				sel = index;
		}
	}
	m_cboFileFilter.SetCurSel(sel);
	m_btnEditFileFilter.EnableWindow(sel!=0);
	
	m_cPattern.EnableWindow(m_bIgnoreRegExp);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Add a filter to the filter display combo box
 */
int CPropFilter::AddFilter(int i)
{
	CString name;
	if (i>=0)
		name = m_Filters->GetAt(i).second;
	else
		name = _T("<None>"); // TODO: Globalize
	int index = m_cboFileFilter.AddString(name);
	m_cboFileFilter.SetItemData(index, i);
	return index;
}

// User clicked the ignore checkbox
void CPropFilter::OnIgnoreregexp() 
{
	UpdateData();
	// enable or disable the edit box according to
	// the value of the check box
	m_cPattern.EnableWindow(m_bIgnoreRegExp);
	if (m_bIgnoreRegExp)
		m_cPattern.SetFocus();
}

// User changed file filter names
void CPropFilter::OnSelchangeFileFilter() 
{
	int index = m_cboFileFilter.GetCurSel();
	if (index<=0) // Cannot edit #0 ("<None>")
	{
		m_btnEditFileFilter.EnableWindow(FALSE);
		m_sFileFilterPath = _T("");
	}
	else
	{
		m_btnEditFileFilter.EnableWindow(TRUE);
		int nfilter = m_cboFileFilter.GetItemData(index);
		m_sFileFilterPath = m_Filters->GetAt(nfilter).first;
	}
}

void CPropFilter::OnEditFileFilter() 
{
	int index = m_cboFileFilter.GetCurSel();
	if (index<=0) return; // Cannot edit #0 ("<None>")

	int nfilter = m_cboFileFilter.GetItemData(index);
	CString filterpath = m_Filters->GetAt(nfilter).first;
	theApp.EditFileFilter(filterpath);
}
