// WebPageDiffDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WebPageDiffBar.h"
#include "MergeApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CWebPageDiffBar dialog


CWebPageDiffBar::CWebPageDiffBar()
{
	//{{AFX_DATA_INIT(CWebPageDiffBar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CWebPageDiffBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWebPageDiffBar)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWebPageDiffBar, CTrDialogBar)
	//{{AFX_MSG_MAP(CWebPageDiffBar)
	ON_UPDATE_COMMAND_UI(IDC_COMPARE, OnUpdateBnClickedCompare)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebPageDiffBar message handlers

BOOL CWebPageDiffBar::Create(CWnd *pParentWnd) 
{
	if (! CTrDialogBar::Create(pParentWnd, CWebPageDiffBar::IDD, 
			CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, CWebPageDiffBar::IDD))
		return FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWebPageDiffBar::OnUpdateBnClickedCompare(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}
