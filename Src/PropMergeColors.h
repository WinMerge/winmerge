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

// Implement IOptionsPanel
	virtual void ReadOptions() override;
	virtual void WriteOptions() override;
	
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
	typedef enum { SET_DEFAULTS, WRITE_OPTIONS, READ_OPTIONS } OPERATION;

	void BrowseColor(CColorButton & colorButton);
	void SerializeColors(OPERATION op);
	void SerializeColor(OPERATION op, CColorButton & btn, const String& optionName);
	void UpdateTextColorButtonState(int checkboxId, CColorButton &btn);
	void UpdateTextColorButtonsState();

	std::array<COLORREF, CustomColorsAmount> m_cCustColors;

	// Generated message map functions
	//{{AFX_MSG(PropMergeColors)
	afx_msg void OnDifferenceColor();
	afx_msg void OnDifferenceDeletedColor();
	afx_msg void OnSelDifferenceDeletedColor();
	afx_msg void OnSelDifferenceColor();
	afx_msg void OnDifferenceTextColor();
	afx_msg void OnSelDifferenceTextColor();
	afx_msg void OnTrivialDiffColor();
	afx_msg void OnTrivialDiffDeletedColor();
	afx_msg void OnTrivialDiffTextColor();
	afx_msg void OnMovedColor();
	afx_msg void OnMovedDeletedColor();
	afx_msg void OnMovedTextColor();
	afx_msg void OnSelMovedColor();
	afx_msg void OnSelMovedDeletedColor();
	afx_msg void OnSelMovedTextColor();
	afx_msg void OnSNPColor();
	afx_msg void OnSNPDeletedColor();
	afx_msg void OnSNPTextColor();
	afx_msg void OnSelSNPColor();
	afx_msg void OnSelSNPDeletedColor();
	afx_msg void OnSelSNPTextColor();
	afx_msg void OnWordDiffColor();
	afx_msg void OnWordDiffDeletedColor();
	afx_msg void OnWordDiffTextColor();
	afx_msg void OnSelWordDiffColor();
	afx_msg void OnSelWordDiffDeletedColor();
	afx_msg void OnSelWordDiffTextColor();
	afx_msg void OnUseTextColor(int checkbox_id, int colorbutton_id);
	template<int checkbox_id, int colorbutton_id>
	afx_msg void OnUseTextColor() { OnUseTextColor(checkbox_id, colorbutton_id); }
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
