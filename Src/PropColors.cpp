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
	m_clrTrivialText = optionsMgr->GetInt(OPT_CLR_TRIVIAL_DIFF_TEXT);
	m_clrMoved = optionsMgr->GetInt(OPT_CLR_MOVEDBLOCK);
	m_clrMovedDeleted = optionsMgr->GetInt(OPT_CLR_MOVEDBLOCK_DELETED);
	m_clrMovedText = optionsMgr->GetInt(OPT_CLR_MOVEDBLOCK_TEXT);
	m_clrSelMoved = optionsMgr->GetInt(OPT_CLR_SELECTED_MOVEDBLOCK);
	m_clrSelMovedDeleted = optionsMgr->GetInt(OPT_CLR_SELECTED_MOVEDBLOCK_DELETED);
	m_clrSelMovedText = optionsMgr->GetInt(OPT_CLR_SELECTED_MOVEDBLOCK_TEXT);
	m_clrWordDiff = optionsMgr->GetInt(OPT_CLR_WORDDIFF);
	m_clrSelWordDiff = optionsMgr->GetInt(OPT_CLR_SELECTED_WORDDIFF);
	m_clrWordDiffText = optionsMgr->GetInt(OPT_CLR_WORDDIFF_TEXT);
	m_clrSelWordDiffText = optionsMgr->GetInt(OPT_CLR_SELECTED_WORDDIFF_TEXT);

	// Set colors for buttons, do NOT invalidate
	m_cDiff.SetColor(m_clrDiff, FALSE);
	m_cSelDiff.SetColor(m_clrSelDiff, FALSE);
	m_cDiffDeleted.SetColor(m_clrDiffDeleted, FALSE);
	m_cSelDiffDeleted.SetColor(m_clrSelDiffDeleted, FALSE);
	m_cDiffText.SetColor(m_clrDiffText, FALSE);
	m_cSelDiffText.SetColor(m_clrSelDiffText, FALSE);
	m_cTrivial.SetColor(m_clrTrivial, FALSE);
	m_cTrivialDeleted.SetColor(m_clrTrivialDeleted, FALSE);
	m_cTrivialText.SetColor(m_clrTrivialText, FALSE);
	m_cMoved.SetColor(m_clrMoved, FALSE);
	m_cMovedDeleted.SetColor(m_clrMovedDeleted, FALSE);
	m_cMovedText.SetColor(m_clrMovedText, FALSE);
	m_cSelMoved.SetColor(m_clrSelMoved, FALSE);
	m_cSelMovedDeleted.SetColor(m_clrSelMovedDeleted, FALSE);
	m_cSelMovedText.SetColor(m_clrSelMovedText, FALSE);
	m_cWordDiff.SetColor(m_clrWordDiff, FALSE);
	m_cSelWordDiff.SetColor(m_clrSelWordDiff, FALSE);
	m_cWordDiffText.SetColor(m_clrWordDiffText, FALSE);
	m_cSelWordDiffText.SetColor(m_clrSelWordDiffText, FALSE);

	m_pOptionsMgr = optionsMgr;
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
	DDX_Control(pDX, IDC_TRIVIAL_DIFF_TEXT_COLOR, m_cTrivialText);
	DDX_Control(pDX, IDC_MOVEDBLOCK_COLOR, m_cMoved);
	DDX_Control(pDX, IDC_MOVEDBLOCK_DELETED_COLOR, m_cMovedDeleted);
	DDX_Control(pDX, IDC_MOVEDBLOCK_TEXT_COLOR, m_cMovedText);
	DDX_Control(pDX, IDC_SEL_MOVEDBLOCK_COLOR, m_cSelMoved);
	DDX_Control(pDX, IDC_SEL_MOVEDBLOCK_DELETED_COLOR, m_cSelMovedDeleted);
	DDX_Control(pDX, IDC_SEL_MOVEDBLOCK_TEXT_COLOR, m_cSelMovedText);
	DDX_Control(pDX, IDC_SEL_WORDDIFFERENCE_TEXT_COLOR, m_cSelWordDiffText);
	DDX_Control(pDX, IDC_WORDDIFFERENCE_TEXT_COLOR, m_cWordDiffText);
	DDX_Control(pDX, IDC_SEL_WORDDIFFERENCE_COLOR, m_cSelWordDiff);
	DDX_Control(pDX, IDC_WORDDIFFERENCE_COLOR, m_cWordDiff);
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
	ON_BN_CLICKED(IDC_COLORDEFAULTS_BTN, OnDefaults)
	ON_BN_CLICKED(IDC_TRIVIAL_DIFF_TEXT_COLOR, OnTrivialDiffTextColor)
	ON_BN_CLICKED(IDC_MOVEDBLOCK_COLOR, OnMovedColor)
	ON_BN_CLICKED(IDC_MOVEDBLOCK_DELETED_COLOR, OnMovedDeletedColor)
	ON_BN_CLICKED(IDC_MOVEDBLOCK_TEXT_COLOR, OnMovedTextColor)
	ON_BN_CLICKED(IDC_SEL_MOVEDBLOCK_COLOR, OnSelMovedColor)
	ON_BN_CLICKED(IDC_SEL_MOVEDBLOCK_DELETED_COLOR, OnSelMovedDeletedColor)
	ON_BN_CLICKED(IDC_SEL_MOVEDBLOCK_TEXT_COLOR, OnSelMovedTextColor)
	ON_BN_CLICKED(IDC_WORDDIFFERENCE_COLOR, OnWordDifferenceColor)
	ON_BN_CLICKED(IDC_SEL_WORDDIFFERENCE_COLOR, OnSelWordDifferenceColor)
	ON_BN_CLICKED(IDC_WORDDIFFERENCE_TEXT_COLOR, OnWordDifferenceTextColor)
	ON_BN_CLICKED(IDC_SEL_WORDDIFFERENCE_TEXT_COLOR, OnSelWordDifferenceTextColor)
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

void CPropColors::OnTrivialDiffTextColor()
{
	CColorDialog dialog(m_clrTrivialText);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;
	
	if (dialog.DoModal() == IDOK)
	{
		m_clrTrivialText = dialog.GetColor();
		m_cTrivialText.SetColor(m_clrTrivialText);
	}
	SaveCustomColors();
}

void CPropColors::OnMovedColor()
{
	CColorDialog dialog(m_clrMoved);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;
	
	if (dialog.DoModal() == IDOK)
	{
		m_clrMoved = dialog.GetColor();
		m_cMoved.SetColor(m_clrMoved);
	}
	SaveCustomColors();
}

void CPropColors::OnMovedDeletedColor()
{
	CColorDialog dialog(m_clrMovedDeleted);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;
	
	if (dialog.DoModal() == IDOK)
	{
		m_clrMovedDeleted = dialog.GetColor();
		m_cMovedDeleted.SetColor(m_clrMovedDeleted);
	}
	SaveCustomColors();
}

void CPropColors::OnMovedTextColor()
{
	CColorDialog dialog(m_clrMovedText);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;
	
	if (dialog.DoModal() == IDOK)
	{
		m_clrMovedText = dialog.GetColor();
		m_cMovedText.SetColor(m_clrMovedText);
	}
	SaveCustomColors();
}

void CPropColors::OnSelMovedColor()
{
	CColorDialog dialog(m_clrSelMoved);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;
	
	if (dialog.DoModal() == IDOK)
	{
		m_clrSelMoved = dialog.GetColor();
		m_cSelMoved.SetColor(m_clrSelMoved);
	}
	SaveCustomColors();
}

void CPropColors::OnSelMovedDeletedColor()
{
	CColorDialog dialog(m_clrSelMovedDeleted);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;
	
	if (dialog.DoModal() == IDOK)
	{
		m_clrSelMovedDeleted = dialog.GetColor();
		m_cSelMovedDeleted.SetColor(m_clrSelMovedDeleted);
	}
	SaveCustomColors();
}

void CPropColors::OnSelMovedTextColor()
{
	CColorDialog dialog(m_clrSelMovedText);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;
	
	if (dialog.DoModal() == IDOK)
	{
		m_clrSelMovedText = dialog.GetColor();
		m_cSelMovedText.SetColor(m_clrSelMovedText);
	}
	SaveCustomColors();
}

/** 
 * @brief User wants to change word difference color
 */
void CPropColors::OnWordDifferenceColor() 
{
	CColorDialog dialog(m_clrWordDiff);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;
	
	if (dialog.DoModal() == IDOK)
	{
		m_clrWordDiff = dialog.GetColor();
		m_cWordDiff.SetColor(m_clrWordDiff);
	}
	SaveCustomColors();
}

/** 
 * @brief User wants to change selected word difference color
 */
void CPropColors::OnSelWordDifferenceColor() 
{
	CColorDialog dialog(m_clrSelWordDiff);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;
	
	if (dialog.DoModal() == IDOK)
	{
		m_clrSelWordDiff = dialog.GetColor();
		m_cSelWordDiff.SetColor(m_clrSelWordDiff);
	}
	SaveCustomColors();
}

/** 
 * @brief User wants to change word difference text color
 */
void CPropColors::OnWordDifferenceTextColor() 
{
	CColorDialog dialog(m_clrWordDiffText);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;

	if (dialog.DoModal() == IDOK)
	{
		m_clrWordDiffText = dialog.GetColor();
		m_cWordDiffText.SetColor(m_clrWordDiffText);
	}
	SaveCustomColors();
}

/** 
 * @brief User wants to change selected word difference text color
 */
void CPropColors::OnSelWordDifferenceTextColor() 
{
	CColorDialog dialog(m_clrSelWordDiffText);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;
	
	if (dialog.DoModal() == IDOK)
	{
		m_clrSelWordDiffText = dialog.GetColor();
		m_cSelWordDiffText.SetColor(m_clrSelWordDiffText);
	}
	SaveCustomColors();
}

/** 
 * @brief Resets colors to defaults
 */
void CPropColors::OnDefaults()
{
	m_pOptionsMgr->GetDefault(OPT_CLR_DIFF, m_clrDiff);
	m_pOptionsMgr->GetDefault(OPT_CLR_SELECTED_DIFF, m_clrSelDiff);
	m_pOptionsMgr->GetDefault(OPT_CLR_DIFF_DELETED, m_clrDiffDeleted);
	m_pOptionsMgr->GetDefault(OPT_CLR_SELECTED_DIFF_DELETED, m_clrSelDiffDeleted);
	m_pOptionsMgr->GetDefault(OPT_CLR_DIFF_TEXT, m_clrDiffText);
	m_pOptionsMgr->GetDefault(OPT_CLR_SELECTED_DIFF_TEXT, m_clrSelDiffText);
	m_pOptionsMgr->GetDefault(OPT_CLR_TRIVIAL_DIFF, m_clrTrivial);
	m_pOptionsMgr->GetDefault(OPT_CLR_TRIVIAL_DIFF_DELETED, m_clrTrivialDeleted);
	m_pOptionsMgr->GetDefault(OPT_CLR_TRIVIAL_DIFF_TEXT, m_clrTrivialText);
	m_pOptionsMgr->GetDefault(OPT_CLR_MOVEDBLOCK, m_clrMoved);
	m_pOptionsMgr->GetDefault(OPT_CLR_MOVEDBLOCK_DELETED, m_clrMovedDeleted);
	m_pOptionsMgr->GetDefault(OPT_CLR_MOVEDBLOCK_TEXT, m_clrMovedText);
	m_pOptionsMgr->GetDefault(OPT_CLR_SELECTED_MOVEDBLOCK, m_clrSelMoved);
	m_pOptionsMgr->GetDefault(OPT_CLR_SELECTED_MOVEDBLOCK_DELETED, m_clrSelMovedDeleted);
	m_pOptionsMgr->GetDefault(OPT_CLR_SELECTED_MOVEDBLOCK_TEXT, m_clrSelMovedText);
	m_pOptionsMgr->GetDefault(OPT_CLR_WORDDIFF, m_clrWordDiff);
	m_pOptionsMgr->GetDefault(OPT_CLR_SELECTED_WORDDIFF, m_clrSelWordDiff);
	m_pOptionsMgr->GetDefault(OPT_CLR_WORDDIFF_TEXT, m_clrWordDiffText);
	m_pOptionsMgr->GetDefault(OPT_CLR_SELECTED_WORDDIFF_TEXT, m_clrSelWordDiffText);

	m_cDiff.SetColor(m_clrDiff);
	m_cSelDiff.SetColor(m_clrSelDiff);
	m_cDiffDeleted.SetColor(m_clrDiffDeleted);
	m_cSelDiffDeleted.SetColor(m_clrSelDiffDeleted);
	m_cDiffText.SetColor(m_clrDiffText);
	m_cSelDiffText.SetColor(m_clrSelDiffText);
	m_cTrivial.SetColor(m_clrTrivial);
	m_cTrivialDeleted.SetColor(m_clrTrivialDeleted);
	m_cTrivialText.SetColor(m_clrTrivialText);
	m_cMoved.SetColor(m_clrMoved);
	m_cMovedDeleted.SetColor(m_clrMovedDeleted);
	m_cMovedText.SetColor(m_clrMovedText);
	m_cSelMoved.SetColor(m_clrSelMoved);
	m_cSelMovedDeleted.SetColor(m_clrSelMovedDeleted);
	m_cSelMovedText.SetColor(m_clrSelMovedText);
	m_cWordDiff.SetColor(m_clrWordDiff);
	m_cSelWordDiff.SetColor(m_clrSelWordDiff);
	m_cWordDiffText.SetColor(m_clrWordDiffText);
	m_cSelWordDiffText.SetColor(m_clrSelWordDiffText);
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