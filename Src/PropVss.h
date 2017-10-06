/**
 * @file  PropVss.h
 *
 * @brief Declaration of VSS properties dialog.
 */
#pragma once

#include "OptionsPanel.h"
#include "UnicodeString.h"

class COptionsMgr;

/** @brief Options property page covering Visual SourceSafe integration */
class PropVss : public OptionsPanel
{

// Construction & Destruction
public:
	explicit PropVss(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions();
	virtual void WriteOptions();

// Dialog Data
	//{{AFX_DATA(PropVss)
	enum { IDD = IDD_PROP_VSS };
	String	m_strPath;
	int		m_nVerSys;
	CComboBox  m_ctlVerSys;
	//}}AFX_DATA


protected:
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropVss)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	void LoadVssOptionStrings();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(PropVss)
	afx_msg void OnBrowseButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokVerSys();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
