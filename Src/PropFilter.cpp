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

IMPLEMENT_DYNCREATE(CPropFilter, CPropertyPage)

CPropFilter::CPropFilter() : CPropertyPage(CPropFilter::IDD)
{
	//{{AFX_DATA_INIT(CPropFilter)
	m_bIgnoreRegExp = FALSE;
	m_sPattern = _T("");
	//}}AFX_DATA_INIT
}

CPropFilter::~CPropFilter()
{
}

void CPropFilter::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropFilter)
	DDX_Control(pDX, IDC_EDITPATTERN, m_cPattern);
	DDX_Check(pDX, IDC_IGNOREREGEXP, m_bIgnoreRegExp);
	DDX_Text(pDX, IDC_EDITPATTERN, m_sPattern);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropFilter, CPropertyPage)
	//{{AFX_MSG_MAP(CPropFilter)
	ON_BN_CLICKED(IDC_IGNOREREGEXP, OnIgnoreregexp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropFilter message handlers

BOOL CPropFilter::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_cPattern.EnableWindow(m_bIgnoreRegExp);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropFilter::OnIgnoreregexp() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	// enable or disable the edit box according to
	// the value of the check box
	m_cPattern.EnableWindow(m_bIgnoreRegExp);
	if (m_bIgnoreRegExp)
		m_cPattern.SetFocus();
}
