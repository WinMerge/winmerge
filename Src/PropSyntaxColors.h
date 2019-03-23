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
#include <array>

const int NumCustomColors = 16;

class PropSyntaxColors : public OptionsPanel
{
	DECLARE_DYNAMIC(PropSyntaxColors)

// Construction & Destruction
public:
	PropSyntaxColors(COptionsMgr *optionsMgr, SyntaxColors *pColors);

// Implement IOptionsPanel
	virtual void ReadOptions() override;
	virtual void WriteOptions() override;

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
	std::array<CColorButton, COLORINDEX_LAST + 1> m_colorButtons;
	std::array<int, COLORINDEX_LAST + 1> m_nBolds;

	SyntaxColors *m_pTempColors;
	std::array<COLORREF, 16> m_cCustColors;
};
