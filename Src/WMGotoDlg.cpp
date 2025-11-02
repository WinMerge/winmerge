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
	afx_msg void OnChangeParam();
	afx_msg void OnBnClicked(UINT nID);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	WMGotoDlg *m_p;
	String m_strRange;			/**< Acceptable range */

	int GetRangeMax();
	void UpdateRange();
	void UpdateGoToButton();
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
	__super::OnInitDialog();

	if (m_p->m_nFiles < 3)
		EnableDlgItem(IDC_WMGOTO_FILEMIDDLE, false);
	if (m_p->m_nLastDiff == 0)
		EnableDlgItem(IDC_WMGOTO_TODIFF, false);

	UpdateRange();
	UpdateGoToButton();
	UpdateData(FALSE);
	return TRUE;
}

void WMGotoDlg::Impl::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(WMGotoDlg)
	DDX_Text(pDX, IDC_WMGOTO_PARAM, m_p->m_strParam);
	DDX_Radio(pDX, IDC_WMGOTO_FILELEFT, m_p->m_nFile);
	DDX_Radio(pDX, IDC_WMGOTO_TOLINE, m_p->m_nGotoWhat);
	DDX_Text(pDX, IDC_WMGOTO_RANGE, m_strRange);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(WMGotoDlg::Impl, CTrDialog)
	//{{AFX_MSG_MAP(WMGotoDlg::Impl)
		// NOTE: the ClassWizard will add message map macros here
	ON_EN_CHANGE(IDC_WMGOTO_PARAM, OnChangeParam)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_WMGOTO_FILELEFT, IDC_WMGOTO_FILERIGHT, OnBnClicked)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_WMGOTO_TOLINE, IDC_WMGOTO_TODIFF, OnBnClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/**
 * @brief Called when the edit string changes.
 */
void WMGotoDlg::Impl::OnChangeParam()
{
	UpdateData(TRUE);
	UpdateGoToButton();
}

/**
 * @brief Called when user selects "File" or "Go to what" radio button.
 * @param [in] nID Button ID of the selected item
 */
void WMGotoDlg::Impl::OnBnClicked(UINT nID)
{
	bool bIsValidId = (nID >= IDC_WMGOTO_FILELEFT && nID <= IDC_WMGOTO_FILERIGHT) || (nID >= IDC_WMGOTO_TOLINE && nID <= IDC_WMGOTO_TODIFF);
	assert(bIsValidId);
	if (!bIsValidId)
		return;

	UpdateData(TRUE);
	UpdateRange();
	UpdateGoToButton();
	UpdateData(FALSE);
}

/**
 * @brief Get upper bound of acceptable range for selected "File" and "Go to what".
 * @return Upper bound of acceptable range for selected "File" and "Go to what".
 */
int WMGotoDlg::Impl::GetRangeMax()
{
	bool bIsValidState = ((m_p->m_nGotoWhat >= 0 && m_p->m_nGotoWhat <= 1) && (m_p->m_nFile >= 0 && m_p->m_nFile <= 2));
	assert(bIsValidState);
	if (!bIsValidState)
		return -1;

	return (m_p->m_nGotoWhat == 0) ? m_p->m_nLastLine[m_p->m_nFile] : m_p->m_nLastDiff;
}

/**
 * @brief Update the acceptable range.
 */
void WMGotoDlg::Impl::UpdateRange()
{
	int nRangeMax = GetRangeMax();
	m_strRange = (nRangeMax > 0) ? strutils::format(_T("(1-%d)"), nRangeMax) : _T("");
}

/**
 * @brief Update the enabled state of the "Go to" button.
 */
void WMGotoDlg::Impl::UpdateGoToButton()
{
	int nNum = 0;
	try
	{
		nNum = std::stoi(m_p->m_strParam);
	}
	catch (...)
	{
		nNum = 0;
	}

	bool bEnable = (nNum > 0 && nNum <= GetRangeMax());
	EnableDlgItem(IDOK, bEnable);
}

/////////////////////////////////////////////////////////////////////////////
// WMGotoDlg message handlers



WMGotoDlg::WMGotoDlg()
	: m_pimpl(new WMGotoDlg::Impl(this)), m_nFile(-1), m_nGotoWhat(-1), m_nFiles(-1), m_nLastLine{-1, -1, -1}, m_nLastDiff(-1) {}
WMGotoDlg::~WMGotoDlg() = default;
int WMGotoDlg::DoModal() { return static_cast<int>(m_pimpl->DoModal()); }

