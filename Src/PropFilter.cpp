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

CPropFilter::CPropFilter(const CStringList & fileFilters, CString & selected)
: CPropertyPage(CPropFilter::IDD)
{
	//{{AFX_DATA_INIT(CPropFilter)
	m_bIgnoreRegExp = FALSE;
	m_sPattern = _T("");
	//}}AFX_DATA_INIT
	for (POSITION pos = fileFilters.GetHeadPosition(); pos; )
	{
		CString name = fileFilters.GetNext(pos);
		m_FilterNames.AddTail(name);
	}
	m_sFileFilterName = selected;
}

CPropFilter::~CPropFilter()
{
}

void CPropFilter::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropFilter)
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropFilter message handlers

BOOL CPropFilter::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_cboFileFilter.AddString(_T("<None>"));
	int sel = 0;
	for (POSITION pos = m_FilterNames.GetHeadPosition(); pos; )
	{
		CString name = m_FilterNames.GetNext(pos);
		if (name == m_sFileFilterName)
			sel = m_cboFileFilter.GetCount();
		m_cboFileFilter.AddString(name);
	}
	m_cboFileFilter.SetCurSel(sel);
	
	m_cPattern.EnableWindow(m_bIgnoreRegExp);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
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
	m_cboFileFilter.GetWindowText(m_sFileFilterName);
	if (m_sFileFilterName == _T("<None>"))
		m_sFileFilterName = _T("");
}
