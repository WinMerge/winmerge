/** 
 * @file  PropDirColors.h
 *
 * @brief Declaration file for PropDirColors propertyheet
 *
 */
#pragma once

#include "ColorButton.h"
#include "OptionsPanel.h"
#include <array>

class COptionsMgr;

const int CustomDirColorsAmount = 16;

/** @brief Property page for folder compare colors options; used in options property sheet */
class PropDirColors : public OptionsPanel
{
// Construction
public:
	explicit PropDirColors(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions() override;
	virtual void WriteOptions() override;
	
// Dialog Data
private:
	//{{AFX_DATA(PropDirColors)
	enum { IDD = IDD_PROPPAGE_COLORS_DIR };
	CColorButton	m_cDirItemEqual;
	CColorButton	m_cDirItemEqualText;
	CColorButton	m_cDirItemDiff;
	CColorButton	m_cDirItemDiffText;
	CColorButton	m_cDirItemNotExistAll;
	CColorButton	m_cDirItemNotExistAllText;
	CColorButton	m_cDirItemFiltered;
	CColorButton	m_cDirItemFilteredText;
	bool            m_bUseColors;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PropDirColors)
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

	std::array<COLORREF, CustomDirColorsAmount> m_cCustColors;

	// Generated message map functions
	//{{AFX_MSG(PropDirColors)
	afx_msg void OnDirItemEqualColor();
	afx_msg void OnDirItemEqualTextColor();
	afx_msg void OnDirItemDiffColor();
	afx_msg void OnDirItemDiffTextColor();
	afx_msg void OnDirItemNotExistAllColor();
	afx_msg void OnDirItemNotExistAllTextColor();
	afx_msg void OnDirItemFilteredColor();
	afx_msg void OnDirItemFilteredTextColor();
	afx_msg void OnDefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
