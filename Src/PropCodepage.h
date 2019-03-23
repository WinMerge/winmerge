/**
 * @file  PropCodepage.h
 *
 * @brief Declaration of PropCodepage class
 */
#pragma once

#include "OptionsPanel.h"

class COptionsMgr;

class PropCodepage : public OptionsPanel
{
// Construction
public:
	explicit PropCodepage(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions() override;
	virtual void WriteOptions() override;

// Dialog Data
	//{{AFX_DATA(PropCodepage)
	enum { IDD = IDD_PROPPAGE_CODEPAGE };
	CComboBox m_comboCustomCodepageValue;
	CComboBox m_comboAutodetectType;
	int		m_nCodepageSystem;
	int		m_nCustomCodepageValue;
	String	m_cCustomCodepageValue;
	bool	m_bDetectCodepage;
	bool	m_bDetectCodepage2;
	int		m_nAutodetectType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropCodepage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(PropCodepage)
	virtual BOOL OnInitDialog() override;
	void GetEncodingCodePageFromNameString();
	afx_msg void OnCpSystem();
	afx_msg void OnCpCustom();
	afx_msg void OnCpUi();
	afx_msg void OnDetectCodepage2();
	afx_msg void OnDetectAutodetecttype();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
