/** 
 * @file  PropColors.cpp
 *
 * @brief Implementation of CPropColors propertysheet
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "merge.h"
#include "PropColors.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const TCHAR Section[] = _T("Custom Colors");

/////////////////////////////////////////////////////////////////////////////
// CPropColors dialog

CPropColors::CPropColors(COptionsMgr *optionsMgr) : CPropertyPage(CPropColors::IDD)
{
	//{{AFX_DATA_INIT(CPropColors)
	m_clrDiff = optionsMgr->GetInt(OPT_CLR_DIFF);
	m_clrSelDiff = optionsMgr->GetInt(OPT_CLR_SELECTED_DIFF);
	m_clrDiffDeleted = optionsMgr->GetInt(OPT_CLR_DIFF_DELETED);
	m_clrSelDiffDeleted = optionsMgr->GetInt(OPT_CLR_SELECTED_DIFF_DELETED);
	m_clrDiffText = optionsMgr->GetInt(OPT_CLR_DIFF_TEXT);
	m_clrSelDiffText = optionsMgr->GetInt(OPT_CLR_SELECTED_DIFF_TEXT);
	m_clrTrivial = optionsMgr->GetInt(OPT_CLR_TRIVIAL_DIFF);
	m_clrTrivialDeleted = optionsMgr->GetInt(OPT_CLR_TRIVIAL_DIFF_DELETED);

	// Set colors for buttons, do NOT invalidate
	m_cDiff.SetColor(m_clrDiff, FALSE);
	m_cSelDiff.SetColor(m_clrSelDiff, FALSE);
	m_cDiffDeleted.SetColor(m_clrDiffDeleted, FALSE);
	m_cSelDiffDeleted.SetColor(m_clrSelDiffDeleted, FALSE);
	m_cDiffText.SetColor(m_clrDiffText, FALSE);
	m_cSelDiffText.SetColor(m_clrSelDiffText, FALSE);
	m_cTrivial.SetColor(m_clrTrivial, FALSE);
	m_cTrivialDeleted.SetColor(m_clrTrivialDeleted, FALSE);
	//}}AFX_DATA_INIT
}

void CPropColors::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropColors)
	DDX_Control(pDX, IDC_TRIVIAL_DIFF_DELETED_COLOR, m_cTrivialDeleted);
	DDX_Control(pDX, IDC_TRIVIAL_DIFF_COLOR, m_cTrivial);
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
	ON_BN_CLICKED(IDC_TRIVIAL_DIFF_COLOR, OnTrivialDiffColor)
	ON_BN_CLICKED(IDC_TRIVIAL_DIFF_DELETED_COLOR, OnTrivialDiffDeletedColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief User wants to change difference color
 */
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

/** 
 * @brief User wants to change selected difference color
 */
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

/** 
 * @brief User wants to change deleted difference color
 */
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

/** 
 * @brief User wants to change selected & deleted difference color
 */
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

/** 
 * @brief User wants to change difference text color
 */
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

/** 
 * @brief User wants to change selected difference text color
 */
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

/** 
 * @brief User wants to change trivial difference color
 */
void CPropColors::OnTrivialDiffColor() 
{
	CColorDialog dialog(m_clrTrivial);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;
	
	if (dialog.DoModal() == IDOK)
	{
		m_clrTrivial = dialog.GetColor();
		m_cTrivial.SetColor(m_clrTrivial);
	}
	SaveCustomColors();	
}

/** 
 * @brief User wants to change deleted trivial difference color
 */
void CPropColors::OnTrivialDiffDeletedColor() 
{
	CColorDialog dialog(m_clrTrivialDeleted);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;
	
	if (dialog.DoModal() == IDOK)
	{
		m_clrTrivialDeleted = dialog.GetColor();
		m_cTrivialDeleted.SetColor(m_clrTrivialDeleted);
	}
	SaveCustomColors();
}

/** 
 * @brief Loads color selection dialog's custom colors from registry
 */
void CPropColors::LoadCustomColors()
{
	for (int i = 0; i < CustomColorsAmount; i++)
	{
		CString sEntry;
		sEntry.Format(_T("%d"), i);
		m_cCustColors[i] = ::AfxGetApp()->GetProfileInt(Section,
			sEntry, RGB(255, 255, 255));
	}
}

/** 
 * @brief Saves color selection dialog's custom colors to registry
 */
void CPropColors::SaveCustomColors()
{
	for (int i = 0; i < CustomColorsAmount; i++)
	{
		CString sEntry;
		sEntry.Format(_T("%d"), i);
		if (m_cCustColors[i] == RGB(255, 255, 255))
			::AfxGetApp()->WriteProfileString(Section, sEntry, NULL);
		else 
			::AfxGetApp()->WriteProfileInt(Section, sEntry, m_cCustColors[i]);
	}
}