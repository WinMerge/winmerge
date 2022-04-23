/**
 * @file  SizeDlg.cpp
 *
 * @brief Size dialog implementation.
 */

#include "stdafx.h"
#include "SizeDlg.h"
#include "DDXHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CSizeDlg dialog

CSizeDlg::CSizeDlg(bool fitToWindow, const SIZE& size, double zoom, const String& userAgent, CWnd* pParent /*= nullptr*/)
	: CTrDialog(CSizeDlg::IDD, pParent)
	, m_fitToWindow(fitToWindow)
	, m_size(size)
	, m_zoom(zoom)
	, m_userAgent(userAgent)
{
}

CSizeDlg::~CSizeDlg()
{
}

void CSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSizeDlg)
	DDX_Control(pDX, IDC_USERAGENT, m_ctlUserAgent);
	DDX_Check(pDX, IDC_FITTOWINDOW, m_fitToWindow);
	DDX_Text(pDX, IDC_WIDTH, m_size.cx);
	DDX_Text(pDX, IDC_HEIGHT, m_size.cy);
	DDX_Text(pDX, IDC_ZOOM, m_zoom);
	DDX_Text(pDX, IDC_USERAGENT, m_userAgent);
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
	m_ctlUserAgent.SaveState(_T("UserAgent"));
}

BOOL CSizeDlg::OnInitDialog() 
{
	CTrDialog::OnInitDialog();
	m_ctlUserAgent.LoadState(_T("UserAgent"));
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
