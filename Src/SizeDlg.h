/**
 *  @file SizeDlg.h 
 *
 *  @brief Declaration file for the dialog Size
 */ 
#pragma once

#include "TrDialogs.h"

/////////////////////////////////////////////////////////////////////////////
// CSizeDlgDlg dialog

class CSizeDlg : public CTrDialog
{
public:
// Construction
	CSizeDlg(const SIZE& size, CWnd* pParent = nullptr);
	~CSizeDlg();

// Dialog Data
	//{{AFX_DATA(CSizeDlg)
	enum { IDD = IDD_WEB_SIZE };
	SIZE m_size;
	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSizeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSizeDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
