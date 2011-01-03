/**
 * @file  PropGeneral.h
 *
 * @brief Declaration of PropGeneral class
 */
// ID line follows -- this is updated by SVN
// $Id$

#if !defined(AFX_PROPGENERAL_H__30AD07B0_E420_11D1_BBC5_00A024706EDC__INCLUDED_)
#define AFX_PROPGENERAL_H__30AD07B0_E420_11D1_BBC5_00A024706EDC__INCLUDED_

#include "IOptionsPanel.h"

class COptionsMgr;

/////////////////////////////////////////////////////////////////////////////
// PropGeneral dialog

/**
 * @brief Class for General options -propertypage.
 */
class PropGeneral : public CPropertyPage, public IOptionsPanel
{
// Construction
public:
	PropGeneral(COptionsMgr *optionsMgr);
	~PropGeneral();

// Implement IOptionsPanel
	virtual void ReadOptions();
	virtual void WriteOptions();

// Dialog Data
	//{{AFX_DATA(PropGeneral)
	enum { IDD = IDD_PROPPAGE_GENERAL };
	BOOL  m_bScroll;
	BOOL  m_bDisableSplash;
	BOOL  m_bSingleInstance;
	BOOL  m_bVerifyPaths;
	BOOL  m_bCloseWindowWithEsc;
	BOOL  m_bAskMultiWindowClose;
	BOOL	m_bMultipleFileCmp;
	BOOL	m_bMultipleDirCmp;
	int		m_nAutoCompleteSource;
	BOOL	m_bPreserveFiletime;
	BOOL	m_bShowSelectFolderOnStartup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropGeneral)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(PropGeneral)
	afx_msg void OnResetAllMessageBoxes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	COptionsMgr *m_pOptionsMgr;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPGENERAL_H__30AD07B0_E420_11D1_BBC5_00A024706EDC__INCLUDED_)
