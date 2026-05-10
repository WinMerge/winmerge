/** 
 * @file  MatchInsideDlg.h
 *
 * @brief Declaration of CMatchInsideDlg class, which implements the dialog for entering two filter filteressions for MatchInside/MatchOutside functions.
 */
#pragma once

#include "TrDialogs.h"
#include "UnicodeString.h"
#include "SuperComboBox.h"
#include "ValidatingEdit.h"

class LineFilterHelper;

/////////////////////////////////////////////////////////////////////////////
// CMatchInsideDlg dialog

class CMatchInsideDlg : public CTrDialog
{
public:
	// Construction
	explicit CMatchInsideDlg(CWnd* pParent = nullptr);
	CMatchInsideDlg(const String& filter1, const String& filter2, CWnd* pParent = nullptr);

	// Dialog Data
	//{{AFX_DATA(CMatchInsideDlg)
	enum { IDD = IDD_FILTERS_MATCHINSIDE };
	CSuperComboBox m_ctlFilter1;
	CSuperComboBox m_ctlFilter2;
	CValidatingEdit m_ctlFilterEdit1;
	CValidatingEdit m_ctlFilterEdit2;
	String m_sFilter1;
	String m_sFilter2;
	std::unique_ptr<LineFilterHelper> m_pLineFilterHelper1;
	std::unique_ptr<LineFilterHelper> m_pLineFilterHelper2;
	//}}AFX_DATA

	// Getters
	String GetFilter1() const { return m_sFilter1; }
	String GetFilter2() const { return m_sFilter2; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMatchInsideDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMatchInsideDlg)
	afx_msg void OnFilter1Button();
	afx_msg void OnFilter2Button();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void SetupFilterValidation(CValidatingEdit& edit, CSuperComboBox& combo, 
							   std::unique_ptr<LineFilterHelper>& helper);
	void OnFilterButton(int buttonId, String& filter);
};
