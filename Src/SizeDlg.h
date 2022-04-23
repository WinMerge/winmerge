/**
 *  @file SizeDlg.h 
 *
 *  @brief Declaration file for the dialog Size
 */ 
#pragma once

#include "TrDialogs.h"
#include "SuperComboBox.h"

/////////////////////////////////////////////////////////////////////////////
// CSizeDlgDlg dialog

class CSizeDlg : public CTrDialog
{
public:
// Construction
	CSizeDlg(bool fitToWindow, const SIZE& size, double zoom, const String& userAgent, CWnd* pParent = nullptr);
	~CSizeDlg();

// Dialog Data
	//{{AFX_DATA(CSizeDlg)
	enum { IDD = IDD_WEB_SIZE };
	CSuperComboBox m_ctlUserAgent;
	bool m_fitToWindow;
	SIZE m_size;
	double m_zoom;
	String m_userAgent;
	
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
