/** 
 * @file  FilterConditionDlg.cpp
 *
 * @brief Implementation of the dialog used to select table properties
 */

#include "stdafx.h"
#include "FilterConditionDlg.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilterConditionDlg dialog


CFilterConditionDlg::CFilterConditionDlg(CWnd* pParent /*= nullptr*/)
: CTrDialog(CFilterConditionDlg::IDD, pParent)
{
}

CFilterConditionDlg::CFilterConditionDlg(bool diff, int side, const String& field, const String& ope, const String& transform, CWnd* pParent /* = nullptr*/)
: CTrDialog(CFilterConditionDlg::IDD, pParent)
, m_bDiff(diff)
, m_nSide(side)
, m_sField(field)
, m_sOperator(ope)
, m_sTransform(transform)
{
	//{{AFX_DATA_INIT(CFilterConditionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CFilterConditionDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterConditionDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFilterConditionDlg, CTrDialog)
	//{{AFX_MSG_MAP(CFilterConditionDlg)
	ON_COMMAND_RANGE(IDC_COMPARETABLE_CSV, IDC_COMPARETABLE_DSV, OnClickFileType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


String CFilterConditionDlg::GetExpression() const
{
	return _T("");
}

/////////////////////////////////////////////////////////////////////////////
// CFilterConditionDlg message handlers

/**
 * @brief Dialog initializer function.
 */
BOOL CFilterConditionDlg::OnInitDialog()
{
	CTrDialog::OnInitDialog();


	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFilterConditionDlg::OnOK()
{
	CDialog::OnOK();
}

void CFilterConditionDlg::OnClickFileType(UINT nID)
{
}
