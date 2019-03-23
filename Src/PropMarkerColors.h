/** 
 * @file  PropMarkerColors.h
 *
 * @brief Declaration file for PropMarkerColors propertyheet
 *
 */
#pragma once

#include "ColorButton.h"
#include "OptionsPanel.h"
#include <array>

class COptionsMgr;
class SyntaxColors;

/** @brief Property page for colors options; used in options property sheet */
class PropMarkerColors : public OptionsPanel
{

// Construction
public:

	PropMarkerColors(COptionsMgr *optionsMgr, SyntaxColors *pColors);

// Implement IOptionsPanel
	virtual void ReadOptions() override;
	virtual void WriteOptions() override;

// Dialog Data
private:

	SyntaxColors *m_pTempColors;
	std::array<COLORREF, 16> m_cCustColors;

	//{{AFX_DATA(PropMarkerColors)
	enum { IDD = IDD_PROPPAGE_COLORS_MARKER };
	CColorButton    m_btnMarkerColors[4];
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PropMarkerColors)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	typedef enum { SET_DEFAULTS, READ_OPTIONS, LOAD_COLORS } OPERATION;

	void BrowseColorAndSave(CColorButton & colorButton, int colorIndex);
	void SerializeColorsToFromScreen(OPERATION op);
	void SerializeColorToFromScreen(OPERATION op, CColorButton & btn, int colorIndex);

	// Generated message map functions
	//{{AFX_MSG(PropMarkerColors)
	afx_msg void OnMarkerColors(UINT nID);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
