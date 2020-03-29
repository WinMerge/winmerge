/** 
 * @file  PropColorSchemes.h
 *
 * @brief Declaration file for PropColorSchemes propertyheet
 *
 */
#pragma once

#include "OptionsPanel.h"
#include <array>

class COptionsMgr;
class CPreferencesDlg;

/** @brief Property page for colors options; used in options property sheet */
class PropColorSchemes : public OptionsPanel
{
// Construction
public:
	explicit PropColorSchemes(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions() override;
	virtual void WriteOptions() override;
	
// Dialog Data
private:
	//{{AFX_DATA(PropColorSchemes)
	enum { IDD = IDD_PROPPAGE_COLOR_SCHEMES };
	String m_sColorScheme;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PropColorSchemes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PropColorSchemes)
	afx_msg BOOL OnInitDialog() override;
	afx_msg void OnCbnSelchangeColorSchemes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
