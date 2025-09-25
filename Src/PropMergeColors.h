/** 
 * @file  PropMergeColors.h
 *
 * @brief Declaration file for PropMergeColors propertyheet
 *
 */
#pragma once

#include "ColorButton.h"
#include "OptionsPanel.h"
#include <array>

class COptionsMgr;

constexpr int CustomColorsAmount = 16;

/** @brief Property page for colors options; used in options property sheet */
class PropMergeColors : public OptionsPanel
{
// Construction
public:
	explicit PropMergeColors(COptionsMgr *optionsMgr);

// Dialog Data
private:
	//{{AFX_DATA(PropMergeColors)
	enum { IDD = IDD_PROPPAGE_COLORS_WINMERGE };
	CColorButton	m_cTrivialDeleted;
	CColorButton	m_cTrivial;
	CColorButton	m_cSelDiffText;
	CColorButton	m_cDiffText;
	CColorButton	m_cSelDiffDeleted;
	CColorButton	m_cDiffDeleted;
	CColorButton	m_cSelDiff;
	CColorButton	m_cDiff;
	CColorButton	m_cTrivialText;
	CColorButton	m_cMoved;
	CColorButton	m_cMovedDeleted;
	CColorButton	m_cMovedText;
	CColorButton	m_cSelMoved;
	CColorButton	m_cSelMovedDeleted;
	CColorButton	m_cSelMovedText;
	CColorButton	m_cSNP;
	CColorButton	m_cSNPDeleted;
	CColorButton	m_cSNPText;
	CColorButton	m_cSelSNP;
	CColorButton	m_cSelSNPDeleted;
	CColorButton	m_cSelSNPText;
	CColorButton	m_cWordDiff;
	CColorButton	m_cWordDiffDeleted;
	CColorButton	m_cWordDiffText;
	CColorButton	m_cSelWordDiff;
	CColorButton	m_cSelWordDiffDeleted;
	CColorButton	m_cSelWordDiffText;
	unsigned	m_clrTrivialDeleted;
	unsigned	m_clrTrivial;
	unsigned	m_clrSelDiffText;
	unsigned	m_clrDiffText;
	unsigned	m_clrSelDiffDeleted;
	unsigned	m_clrDiffDeleted;
	unsigned	m_clrSelDiff;
	unsigned	m_clrDiff;
	unsigned	m_clrTrivialText;
	unsigned	m_clrMoved;
	unsigned	m_clrMovedDeleted;
	unsigned	m_clrMovedText;
	unsigned	m_clrSelMoved;
	unsigned	m_clrSelMovedDeleted;
	unsigned	m_clrSelMovedText;
	unsigned	m_clrSNP;
	unsigned	m_clrSNPDeleted;
	unsigned	m_clrSNPText;
	unsigned	m_clrSelSNP;
	unsigned	m_clrSelSNPDeleted;
	unsigned	m_clrSelSNPText;
	unsigned	m_clrWordDiff;
	unsigned	m_clrWordDiffDeleted;
	unsigned	m_clrWordDiffText;
	unsigned	m_clrSelWordDiff;
	unsigned	m_clrSelWordDiffDeleted;
	unsigned	m_clrSelWordDiffText;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PropMergeColors)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	static const COLORREF COLOR_NONE = 0xffffffff;

	void BrowseColor(unsigned id);
	void UpdateTextColorButtonState(int checkboxId, CColorButton &btn);
	void UpdateTextColorButtonsState();

	std::array<COLORREF, CustomColorsAmount> m_cCustColors;

	// Generated message map functions
	//{{AFX_MSG(PropMergeColors)
	afx_msg void OnUseTextColor(int checkbox_id, int colorbutton_id);
	template<int checkbox_id, int colorbutton_id>
	afx_msg void OnUseTextColor() { OnUseTextColor(checkbox_id, colorbutton_id); }
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
