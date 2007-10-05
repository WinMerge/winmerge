/**
 * @file  PropVss.h
 *
 * @brief Declaration of VSS properties dialog.
 */
// ID line follows -- this is updated by SVN
// $Id$

#if !defined(AFX_PROPVSS_H__30AD07AF_E420_11D1_BBC5_00A024706EDC__INCLUDED_)
#define AFX_PROPVSS_H__30AD07AF_E420_11D1_BBC5_00A024706EDC__INCLUDED_

#include "IOptionsPanel.h"

class COptionsMgr;

/////////////////////////////////////////////////////////////////////////////
// CPropVss dialog

/** @brief Options property page covering Visual SourceSafe integration */
class CPropVss : public CPropertyPage, public IOptionsPanel
{

// Construction & Destruction
public:
	CPropVss(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions();
	virtual void WriteOptions();

// Dialog Data
	//{{AFX_DATA(CPropVss)
	enum { IDD = IDD_PROP_VSS };
	CStatic	m_ctlVssL1;
	CEdit	m_ctlPath;
	CButton	m_ctlBrowse;
	CString	m_strPath;
	int		m_nVerSys;
	CComboBox  m_ctlVerSys;
	//}}AFX_DATA


protected:
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropVss)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	void LoadVssOptionStrings();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropVss)
	afx_msg void OnBrowseButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokVerSys();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	COptionsMgr * m_pOptionsMgr; /**< Options manager. */
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPVSS_H__30AD07AF_E420_11D1_BBC5_00A024706EDC__INCLUDED_)
