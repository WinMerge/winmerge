/**
 *  @file PropLineFilter.cpp
 *
 *  @brief Implementation of Line Filter dialog
 */ 
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "merge.h"
#include "PropLineFilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropLineFilter property page

IMPLEMENT_DYNAMIC(CPropLineFilter, CPropertyPage)

CPropLineFilter::CPropLineFilter()
: CPropertyPage(CPropLineFilter::IDD)
{
	//{{AFX_DATA_INIT(CPropLineFilter)
	m_bIgnoreRegExp = FALSE;
	m_sPattern = _T("");
	//}}AFX_DATA_INIT
}

CPropLineFilter::~CPropLineFilter()
{
}

void CPropLineFilter::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropLineFilter)
	DDX_Control(pDX, IDC_EDITPATTERN, m_cPattern);
	DDX_Check(pDX, IDC_IGNOREREGEXP, m_bIgnoreRegExp);
	DDX_Text(pDX, IDC_EDITPATTERN, m_sPattern);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropLineFilter, CPropertyPage)
	//{{AFX_MSG_MAP(CPropLineFilter)
	ON_BN_CLICKED(IDC_IGNOREREGEXP, OnIgnoreregexp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropLineFilter message handlers

BOOL CPropLineFilter::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_cPattern.EnableWindow(m_bIgnoreRegExp);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/// User clicked the ignore checkbox
void CPropLineFilter::OnIgnoreregexp() 
{
	UpdateData();
	// enable or disable the edit box according to
	// the value of the check box
	m_cPattern.EnableWindow(m_bIgnoreRegExp);
	if (m_bIgnoreRegExp)
		m_cPattern.SetFocus();
}
