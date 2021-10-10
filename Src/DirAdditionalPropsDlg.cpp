/** 
 * @file  DirAdditionalPropsDlg.cpp
 *
 * @brief Implementation file for CDirAdditionalPropsDlg
 */

#include "stdafx.h"
#include "DirAdditionalPropsDlg.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirAdditionalPropsDlg dialog

/**
 * @brief Default dialog constructor.
 * @param [in] pParent Dialog's parent component (window).
 */
CDirAdditionalPropsDlg::CDirAdditionalPropsDlg(CWnd* pParent /*= nullptr*/)
	: CTrDialog(CDirAdditionalPropsDlg::IDD, pParent)
{
}

/**
 * @brief Handle dialog data exchange between controls and variables.
 */
void CDirAdditionalPropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDirAdditionalPropsDlg)
	DDX_Control(pDX, IDC_PROPS_TREEVIEW, m_treeProps);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDirAdditionalPropsDlg, CTrDialog)
	//{{AFX_MSG_MAP(CDirAdditionalPropsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirAdditionalPropsDlg message handlers

/**
 * @brief Dialog initialisation. Load column lists.
 */
BOOL CDirAdditionalPropsDlg::OnInitDialog() 
{
	CTrDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief User clicked ok, so we update m_cols and close.
 */
void CDirAdditionalPropsDlg::OnOK() 
{
	CTrDialog::OnOK();
}

