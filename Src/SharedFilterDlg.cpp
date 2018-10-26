/** 
 * @file  SharedFilterDlg.cpp
 *
 * @brief Dialog where user choose shared or private filter
 */

#include "stdafx.h"
#include "SharedFilterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CSharedFilterDlg dialog


/**
 * @brief A constructor.
 */
CSharedFilterDlg::CSharedFilterDlg(FilterType type, CWnd* pParent /*= nullptr*/)
	: CTrDialog(CSharedFilterDlg::IDD, pParent), m_selectedType(type)
{
}


void CSharedFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSharedFilterDlg)
	DDX_Radio(pDX, IDC_SHARED, *(reinterpret_cast<int *>(&m_selectedType)));
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSharedFilterDlg, CTrDialog)
	//{{AFX_MSG_MAP(CSharedFilterDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
