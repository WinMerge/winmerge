// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file  WMGotoDlg.cpp
 *
 * @brief Implementation of the WMGotoDlg dialog.
 */

#include "stdafx.h"
#include "WMGotoDlg.h"
#include "TrDialogs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @brief Class for Goto-dialog.
 * This dialog allows user to go to certain line or or difference in the file
 * compare. As there are two panels with different line numbers, there is a
 * choice for target panel. When dialog is opened, its values are initialized
 * for active file's line number.
 */
class WMGotoDlg::Impl : public CTrDialog
{
// Construction
public:
	explicit Impl(WMGotoDlg *p, CWnd* pParent = nullptr);   // standard constructor

// Dialog Data
	//{{AFX_DATA(WMGotoDlg)
	enum { IDD = IDD_WMGOTO };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(WMGotoDlg)
	protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(WMGotoDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	WMGotoDlg *m_p;
};

/////////////////////////////////////////////////////////////////////////////
// CGotoDlg dialog

/**
 * @brief Constructor.
 */
WMGotoDlg::Impl::Impl(WMGotoDlg *p, CWnd* pParent /*= nullptr*/)
	: CTrDialog(WMGotoDlg::Impl::IDD, pParent), m_p(p)
{
}

/**
 * @brief Initialize the dialog.
 * @return Always TRUE.
 */
BOOL WMGotoDlg::Impl::OnInitDialog()
{
	LangTranslateDialog(m_hWnd);
	CDialog::OnInitDialog();

	if (m_p->m_nFiles < 3)
		EnableDlgItem(IDC_WMGOTO_FILEMIDDLE, false);

	return TRUE;
}

void WMGotoDlg::Impl::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(WMGotoDlg)
	DDX_Text(pDX, IDC_WMGOTO_PARAM, m_p->m_strParam);
	DDX_Radio(pDX, IDC_WMGOTO_FILELEFT, m_p->m_nFile);
	DDX_Radio(pDX, IDC_WMGOTO_TOLINE, m_p->m_nGotoWhat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(WMGotoDlg::Impl, CTrDialog)
	//{{AFX_MSG_MAP(WMGotoDlg::Impl)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// WMGotoDlg message handlers



WMGotoDlg::WMGotoDlg()
	: m_pimpl(new WMGotoDlg::Impl(this)), m_nFile(-1), m_nGotoWhat(-1), m_nFiles(-1) {}
WMGotoDlg::~WMGotoDlg() = default;
int WMGotoDlg::DoModal() { return static_cast<int>(m_pimpl->DoModal()); }

