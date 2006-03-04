/** 
 * @file  PropColors.cpp
 *
 * @brief Implementation of CPropMergeColors propertysheet
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
// CPropMergeColors dialog

/** 
 * @brief Default constructor.
 */
CPropMergeColors::CPropMergeColors(COptionsMgr *optionsMgr)
 : CPropertyPage(CPropMergeColors::IDD)
, m_pOptionsMgr(optionsMgr)
{
}

void CPropMergeColors::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropMergeColors)
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


BEGIN_MESSAGE_MAP(CPropMergeColors, CDialog)
	//{{AFX_MSG_MAP(CPropMergeColors)
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
 * @brief Reads options values from storage to UI.
 * (Property sheet calls this before displaying all property pages)
 */
void CPropMergeColors::ReadOptions()
{
	SerializeColors(READ_OPTIONS);
}

/** 
 * @brief Writes options values from UI to storage.
 * (Property sheet calls this after displaying all property pages)
 */
void CPropMergeColors::WriteOptions()
{
	SerializeColors(WRITE_OPTIONS);
}

/** 
 * @brief Let user browse common color dialog, and select a color
 */
void CPropMergeColors::BrowseColor(CColorButton & colorButton, COLORREF & currentColor)
{
	CColorDialog dialog(currentColor);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;
	
	if (dialog.DoModal() == IDOK)
	{
		currentColor = dialog.GetColor();
		colorButton.SetColor(currentColor);
	}
	SaveCustomColors();
}

/** 
 * @brief User wants to change difference color
 */
void CPropMergeColors::OnDifferenceColor() 
{
	BrowseColor(m_cDiff, m_clrDiff);
}

/** 
 * @brief User wants to change selected difference color
 */
void CPropMergeColors::OnSelDifferenceColor() 
{
	BrowseColor(m_cSelDiff, m_clrSelDiff);
}

/** 
 * @brief User wants to change deleted difference color
 */
void CPropMergeColors::OnDifferenceDeletedColor() 
{
	BrowseColor(m_cDiffDeleted, m_clrDiffDeleted);
}

/** 
 * @brief User wants to change selected & deleted difference color
 */
void CPropMergeColors::OnSelDifferenceDeletedColor() 
{
	BrowseColor(m_cSelDiffDeleted, m_clrSelDiffDeleted);
}

/** 
 * @brief User wants to change difference text color
 */
void CPropMergeColors::OnDifferenceTextColor() 
{
	BrowseColor(m_cDiffText, m_clrDiffText);
}

/** 
 * @brief User wants to change selected difference text color
 */
void CPropMergeColors::OnSelDifferenceTextColor() 
{
	BrowseColor(m_cSelDiffText, m_clrSelDiffText);
}

/** 
 * @brief User wants to change trivial difference color
 */
void CPropMergeColors::OnTrivialDiffColor() 
{
	BrowseColor(m_cTrivial, m_clrTrivial);
}

/** 
 * @brief User wants to change deleted trivial difference color
 */
void CPropMergeColors::OnTrivialDiffDeletedColor() 
{
	BrowseColor(m_cTrivialDeleted, m_clrTrivialDeleted);
}

void CPropMergeColors::OnTrivialDiffTextColor()
{
	BrowseColor(m_cTrivialText, m_clrTrivialText);
}

void CPropMergeColors::OnMovedColor()
{
	BrowseColor(m_cMoved, m_clrMoved);
}

void CPropMergeColors::OnMovedDeletedColor()
{
	BrowseColor(m_cMovedDeleted, m_clrMovedDeleted);
}

void CPropMergeColors::OnMovedTextColor()
{
	BrowseColor(m_cMovedText, m_clrMovedText);
}

void CPropMergeColors::OnSelMovedColor()
{
	BrowseColor(m_cSelMoved, m_clrSelMoved);
}

void CPropMergeColors::OnSelMovedDeletedColor()
{
	BrowseColor(m_cSelMovedDeleted, m_clrSelMovedDeleted);
}

void CPropMergeColors::OnSelMovedTextColor()
{
	BrowseColor(m_cSelMovedText, m_clrSelMovedText);
}

/** 
 * @brief User wants to change word difference color
 */
void CPropMergeColors::OnWordDifferenceColor() 
{
	BrowseColor(m_cWordDiff, m_clrWordDiff);
}

/** 
 * @brief User wants to change selected word difference color
 */
void CPropMergeColors::OnSelWordDifferenceColor() 
{
	BrowseColor(m_cSelWordDiff, m_clrSelWordDiff);
}

/** 
 * @brief User wants to change word difference text color
 */
void CPropMergeColors::OnWordDifferenceTextColor() 
{
	BrowseColor(m_cWordDiffText, m_clrWordDiffText);
}

/** 
 * @brief User wants to change selected word difference text color
 */
void CPropMergeColors::OnSelWordDifferenceTextColor() 
{
	BrowseColor(m_cSelWordDiffText, m_clrSelWordDiffText);
}

void CPropMergeColors::SerializeColors(OPERATION op)
{
	SerializeColor(op, m_cDiff, OPT_CLR_DIFF, m_clrDiff);
	SerializeColor(op, m_cDiffDeleted, OPT_CLR_DIFF_DELETED, m_clrDiffDeleted);
	SerializeColor(op, m_cSelDiff, OPT_CLR_SELECTED_DIFF, m_clrSelDiff);

	SerializeColor(op, m_cDiffText, OPT_CLR_DIFF_TEXT, m_clrDiffText);
	SerializeColor(op, m_cSelDiffDeleted, OPT_CLR_SELECTED_DIFF_DELETED, m_clrSelDiffDeleted);
	SerializeColor(op, m_cSelDiffText, OPT_CLR_SELECTED_DIFF_TEXT, m_clrSelDiffText);

	SerializeColor(op, m_cTrivial, OPT_CLR_TRIVIAL_DIFF, m_clrTrivial);
	SerializeColor(op, m_cTrivialDeleted, OPT_CLR_TRIVIAL_DIFF_DELETED, m_clrTrivialDeleted);
	SerializeColor(op, m_cTrivialText, OPT_CLR_TRIVIAL_DIFF_TEXT, m_clrTrivialText);
	
	SerializeColor(op, m_cMoved, OPT_CLR_MOVEDBLOCK, m_clrMoved);
	SerializeColor(op, m_cMovedDeleted, OPT_CLR_MOVEDBLOCK_DELETED, m_clrMovedDeleted);
	SerializeColor(op, m_cMovedText, OPT_CLR_MOVEDBLOCK_TEXT, m_clrMovedText);
	
	SerializeColor(op, m_cSelMoved, OPT_CLR_SELECTED_MOVEDBLOCK, m_clrSelMoved);
	SerializeColor(op, m_cSelMovedDeleted, OPT_CLR_SELECTED_MOVEDBLOCK_DELETED, m_clrSelMovedDeleted);
	SerializeColor(op, m_cSelMovedText, OPT_CLR_SELECTED_MOVEDBLOCK_TEXT, m_clrSelMovedText);
	
	SerializeColor(op, m_cWordDiff, OPT_CLR_WORDDIFF, m_clrWordDiff);
	SerializeColor(op, m_cSelWordDiff, OPT_CLR_SELECTED_WORDDIFF, m_clrSelWordDiff);
	
	SerializeColor(op, m_cWordDiffText, OPT_CLR_WORDDIFF_TEXT, m_clrWordDiffText);
	SerializeColor(op, m_cSelWordDiffText, OPT_CLR_SELECTED_WORDDIFF_TEXT, m_clrSelWordDiffText);
}

void CPropMergeColors::SerializeColor(OPERATION op, CColorButton & btn, LPCTSTR optionName, COLORREF & color)
{
	switch (op)
	{
	case SET_DEFAULTS:
		m_pOptionsMgr->GetDefault(optionName, color);
		btn.SetColor(color);
		return;

	case WRITE_OPTIONS:
		m_pOptionsMgr->SaveOption(optionName, (int)color);
		return;

	case READ_OPTIONS:
		color = m_pOptionsMgr->GetInt(optionName);
		// Set colors for buttons, do NOT invalidate
		btn.SetColor(color, FALSE);
		return;
	}
}

/** 
 * @brief Resets colors to defaults
 */
void CPropMergeColors::OnDefaults()
{
	SerializeColors(SET_DEFAULTS);
}

/** 
 * @brief Loads color selection dialog's custom colors from registry
 */
void CPropMergeColors::LoadCustomColors()
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
void CPropMergeColors::SaveCustomColors()
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