/** 
 * @file  PropSyntaxColors.h
 *
 * @brief Declaration file for PropSyntaxColors propertyheet
 *
 */
#pragma once

#include "ColorButton.h"
#include "OptionsPanel.h"
#include "SyntaxColors.h"

const int NumCustomColors = 16;

class PropSyntaxColors : public OptionsPanel
{
	DECLARE_DYNAMIC(PropSyntaxColors)

// Construction & Destruction
public:
	PropSyntaxColors(COptionsMgr *optionsMgr, SyntaxColors *pColors);

// Implement IOptionsPanel
	virtual void ReadOptions();
	virtual void WriteOptions();

// Dialog Data
public:

private:
	enum { IDD = IDD_PROPPAGE_COLORS_SYNTAX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	template<unsigned colorIndex>
	afx_msg void OnBnClickedEcolor();
	afx_msg void OnBnClickedEcolorsBdefaults();
	template<unsigned ctlId, unsigned colorIndex>
	afx_msg void OnBnClickedBoldButton();

	void BrowseColorAndSave(unsigned colorIndex);
	int GetCheckVal(UINT nColorIndex);

	DECLARE_MESSAGE_MAP()
	
private:
	CColorButton m_colorButtons[COLORINDEX_LAST + 1];
	int m_nBolds[COLORINDEX_LAST + 1];

	SyntaxColors *m_pTempColors;
	COLORREF m_cCustColors[16];
};
