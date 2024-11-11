/** 
 * @file  PropSysColors.h
 *
 * @brief Declaration file for PropSysColors propertyheet
 *
 */
#pragma once

#include "ColorButton.h"
#include "OptionsPanel.h"
#include <array>

class COptionsMgr;

/** @brief Property page for system colors options; used in options property sheet */
class PropSysColors : public OptionsPanel
{
// Construction
public:
	explicit PropSysColors(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions() override;
	virtual void WriteOptions() override;
	
// Dialog Data
private:
	//{{AFX_DATA(PropSysColors)
	enum { IDD = IDD_PROPPAGE_COLORS_SYSTEM };
	CComboBox		m_cboSysColorName;
	CColorButton	m_btnSysColor;
	bool            m_bEnableSysColorHook;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PropSysColors)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	typedef enum { WRITE_OPTIONS, READ_OPTIONS } OPERATION;

	void UpdateControls();
	void BrowseColor(CColorButton & colorButton);

	std::array<COLORREF, COLOR_MENUBAR + 1> m_cSysColors;
	String m_oldColors;

	// Generated message map functions
	//{{AFX_MSG(PropSysColors)
	afx_msg BOOL OnInitDialog() override;
	afx_msg void OnBnClickedSysColorHookEnabled();
	afx_msg void OnCbnSelchangeSysColorName();
	afx_msg void OnBnClickedSysColor();
	afx_msg void OnDefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
