/** 
 * @file  FilterConditionDlg.h
 *
 * @brief Declaration of the dialog used to select codepages
 */
#pragma once

#include "TrDialogs.h"
#include "UnicodeString.h"

/////////////////////////////////////////////////////////////////////////////
// CFilterConditionDlg dialog

class CFilterConditionDlg : public CTrDialog
{
public:
// Construction
	explicit CFilterConditionDlg(CWnd* pParent = nullptr);   // standard constructor
	CFilterConditionDlg(bool diff, int side, const String& field, const String& ope, const String& transform, CWnd* pParent = nullptr);

// Dialog Data
	//{{AFX_DATA(CFilterConditionDlg)
	enum { IDD = IDD_FILTERS_CONDITION };
	bool m_bDiff;
	int m_nSide;
	String m_sField;
	String m_sTransform;
	String m_sOperator;
	String m_sValue1;
	String m_sValue2;
	String m_sExpression;
	CComboBox m_ctlValue1;
	CComboBox m_ctlValue2;
	CTime m_tmValue1;
	CTime m_tmValue2;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterConditionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	//}}AFX_VIRTUAL

// Implementation
protected:
	String GetExpression();

	// Generated message map functions
	//{{AFX_MSG(CFilterConditionDlg)
	afx_msg void OnCbnSelchangeOperator();
	afx_msg void OnCbnEditchangeValue();
	afx_msg void OnCbnSelchangeValue();
	afx_msg void OnDateTimeChange(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
