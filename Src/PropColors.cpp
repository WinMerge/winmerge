// PropColors.cpp : implementation file
//

#include "stdafx.h"
#include "merge.h"
#include "PropColors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropColors dialog


CPropColors::CPropColors( COLORREF clrDiff, COLORREF clrSelDiff )
	: CPropertyPage(CPropColors::IDD), m_clrDiff(clrDiff), m_clrSelDiff(clrSelDiff),
		m_cDiff(clrDiff), m_cSelDiff(clrSelDiff)
{
	//{{AFX_DATA_INIT(CPropColors)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPropColors::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropColors)
	DDX_Control(pDX, IDC_SEL_DIFFERENCE_COLOR, m_cSelDiff);
	DDX_Control(pDX, IDC_DIFFERENCE_COLOR, m_cDiff);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropColors, CDialog)
	//{{AFX_MSG_MAP(CPropColors)
	ON_BN_CLICKED(IDC_DIFFERENCE_COLOR, OnDifferenceColor)
	ON_BN_CLICKED(IDC_SEL_DIFFERENCE_COLOR, OnSelDifferenceColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropColors message handlers

void CPropColors::OnDifferenceColor() 
{
	CColorDialog	dialog( m_clrDiff );
	
	if ( dialog.DoModal() == IDOK ) {

		m_clrDiff = dialog.GetColor();

		m_cDiff.SetColor( m_clrDiff );
	}
}

void CPropColors::OnSelDifferenceColor() 
{
	CColorDialog	dialog( m_clrSelDiff );
	
	if ( dialog.DoModal() == IDOK ) {

		m_clrSelDiff = dialog.GetColor();

		m_cSelDiff.SetColor( m_clrSelDiff );
	}
}
