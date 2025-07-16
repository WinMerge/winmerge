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
	String GetExpression() const;

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

	// Generated message map functions
	//{{AFX_MSG(CFilterConditionDlg)
	afx_msg void OnClickFileType(UINT nID);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
