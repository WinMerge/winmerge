/**
 * @file  SizeDlg.cpp
 *
 * @brief Size dialog implementation.
 */

#include "stdafx.h"
#include "SizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CSizeDlg dialog

CSizeDlg::CSizeDlg(const SIZE& size, CWnd* pParent /*= nullptr*/)
	: CTrDialog(CSizeDlg::IDD, pParent)
	, m_size(size)
{
}

CSizeDlg::~CSizeDlg()
{
}

void CSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSizeDlg)
	DDX_Text(pDX, IDC_WIDTH, m_size.cx);
	DDX_Text(pDX, IDC_HEIGHT, m_size.cy);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSizeDlg, CTrDialog)
	//{{AFX_MSG_MAP(CSizeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSizeDlg message handlers

void CSizeDlg::OnOK()
{
	CTrDialog::OnOK();
	m_size.cx = std::clamp(m_size.cx, 1L, 9999L);
	m_size.cy = std::clamp(m_size.cy, 1L, 9999L);
}

BOOL CSizeDlg::OnInitDialog() 
{
	CTrDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
