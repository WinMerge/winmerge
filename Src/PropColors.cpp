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

static const TCHAR Section[] = _T("Custom Colors");

/////////////////////////////////////////////////////////////////////////////
// CPropColors dialog


CPropColors::CPropColors( COLORREF clrDiff, COLORREF clrSelDiff, COLORREF clrDiffDeleted, COLORREF clrSelDiffDeleted, COLORREF clrDiffText, COLORREF clrSelDiffText)
	: CPropertyPage(CPropColors::IDD),
		m_clrDiff(clrDiff), m_clrSelDiff(clrSelDiff), m_clrDiffDeleted(clrDiffDeleted), m_clrSelDiffDeleted(clrSelDiffDeleted), m_clrDiffText(clrDiffText), m_clrSelDiffText(clrSelDiffText),
		m_cDiff(clrDiff), m_cSelDiff(clrSelDiff), m_cDiffDeleted(clrDiffDeleted), m_cSelDiffDeleted(clrSelDiffDeleted), m_cDiffText(clrDiffText), m_cSelDiffText(clrSelDiffText)
{
	//{{AFX_DATA_INIT(CPropColors)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPropColors::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropColors)
	DDX_Control(pDX, IDC_SEL_DIFFERENCE_TEXT_COLOR, m_cSelDiffText);
	DDX_Control(pDX, IDC_DIFFERENCE_TEXT_COLOR, m_cDiffText);
	DDX_Control(pDX, IDC_SEL_DIFFERENCE_DELETED_COLOR, m_cSelDiffDeleted);
	DDX_Control(pDX, IDC_DIFFERENCE_DELETED_COLOR, m_cDiffDeleted);
	DDX_Control(pDX, IDC_SEL_DIFFERENCE_COLOR, m_cSelDiff);
	DDX_Control(pDX, IDC_DIFFERENCE_COLOR, m_cDiff);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropColors, CDialog)
	//{{AFX_MSG_MAP(CPropColors)
	ON_BN_CLICKED(IDC_DIFFERENCE_COLOR, OnDifferenceColor)
	ON_BN_CLICKED(IDC_DIFFERENCE_DELETED_COLOR, OnDifferenceDeletedColor)
	ON_BN_CLICKED(IDC_SEL_DIFFERENCE_DELETED_COLOR, OnSelDifferenceDeletedColor)
	ON_BN_CLICKED(IDC_SEL_DIFFERENCE_COLOR, OnSelDifferenceColor)
	ON_BN_CLICKED(IDC_DIFFERENCE_TEXT_COLOR, OnDifferenceTextColor)
	ON_BN_CLICKED(IDC_SEL_DIFFERENCE_TEXT_COLOR, OnSelDifferenceTextColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropColors message handlers

void CPropColors::OnDifferenceColor() 
{
	CColorDialog dialog(m_clrDiff);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;
	
	if (dialog.DoModal() == IDOK)
	{
		m_clrDiff = dialog.GetColor();
		m_cDiff.SetColor(m_clrDiff);
	}
	SaveCustomColors();
}

void CPropColors::OnSelDifferenceColor() 
{
	CColorDialog dialog(m_clrSelDiff);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;
	
	if (dialog.DoModal() == IDOK)
	{
		m_clrSelDiff = dialog.GetColor();
		m_cSelDiff.SetColor(m_clrSelDiff);
	}
	SaveCustomColors();
}

void CPropColors::OnDifferenceDeletedColor() 
{
	CColorDialog dialog(m_clrDiffDeleted);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;
	
	if (dialog.DoModal() == IDOK)
	{
		m_clrDiffDeleted = dialog.GetColor();
		m_cDiffDeleted.SetColor(m_clrDiffDeleted);
	}
	SaveCustomColors();
}

void CPropColors::OnSelDifferenceDeletedColor() 
{
	CColorDialog dialog(m_clrSelDiffDeleted);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;
	
	if (dialog.DoModal() == IDOK)
	{
		m_clrSelDiffDeleted = dialog.GetColor();
		m_cSelDiffDeleted.SetColor(m_clrSelDiffDeleted);
	}
	SaveCustomColors();
}

void CPropColors::OnDifferenceTextColor() 
{
	CColorDialog dialog(m_clrDiffText);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;

	if (dialog.DoModal() == IDOK)
	{
		m_clrDiffText = dialog.GetColor();
		m_cDiffText.SetColor(m_clrDiffText);
	}
	SaveCustomColors();
}

void CPropColors::OnSelDifferenceTextColor() 
{
	CColorDialog dialog(m_clrSelDiffText);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;
	
	if (dialog.DoModal() == IDOK)
	{
		m_clrSelDiffText = dialog.GetColor();
		m_cSelDiffText.SetColor(m_clrSelDiffText);
	}
	SaveCustomColors();
}

void CPropColors::LoadCustomColors()
{
	for (int i = 0; i < CustomColorsAmount; i++)
	{
		CString sEntry;
		sEntry.Format("%d", i);
		m_cCustColors[i] = ::AfxGetApp()->GetProfileInt(Section,
			sEntry, RGB(255, 255, 255));
	}
}

void CPropColors::SaveCustomColors()
{
	for (int i = 0; i < CustomColorsAmount; i++)
	{
		CString sEntry;
		sEntry.Format("%d", i);
		if (m_cCustColors[i] == RGB(255, 255, 255))
			::AfxGetApp()->WriteProfileString(Section, sEntry, NULL);
		else 
			::AfxGetApp()->WriteProfileInt(Section, sEntry, m_cCustColors[i]);
	}
}