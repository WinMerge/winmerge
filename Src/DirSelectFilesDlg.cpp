/**
 * @file  DirSelectFilesDlg.cpp
 *
 * @brief Implementation of the DirSelectFilesDlg dialog.
 */

#include "stdafx.h"
#include "DirSelectFilesDlg.h"
#include "TrDialogs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class DirSelectFilesDlg::Impl : public CTrDialog
{
// Construction
public:
	explicit Impl(DirSelectFilesDlg *p, CWnd* pParent = nullptr);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DirSelectFilesDlg)
	enum { IDD = IDD_SELECT_FILES_OR_FOLDERS };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DirSelectFilesDlg)
	protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DirSelectFilesDlg)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnBnClicked(UINT id);
	afx_msg void OnBnClickedResetButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void UpdateButtonCaptions();
	DirSelectFilesDlg *m_p;
};

/////////////////////////////////////////////////////////////////////////////
// DirSelectFilesDlg dialog

/**
 * @brief Constructor.
 */
DirSelectFilesDlg::Impl::Impl(DirSelectFilesDlg *p, CWnd* pParent /*= nullptr*/)
	: CTrDialog(DirSelectFilesDlg::Impl::IDD, pParent), m_p(p)
{
}

void DirSelectFilesDlg::Impl::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DirSelectFilesDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DirSelectFilesDlg::Impl, CTrDialog)
	//{{AFX_MSG_MAP(DirSelectFilesDlg::Impl)
		// NOTE: the ClassWizard will add message map macros here
	ON_CONTROL_RANGE(BN_CLICKED, IDC_LEFT1, IDC_LEFT1 + 3 * 3, OnBnClicked)
	ON_BN_CLICKED(IDC_RESET, OnBnClickedResetButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DirSelectFilesDlg message handlers

BOOL DirSelectFilesDlg::Impl::OnInitDialog()
{
	CenterWindow();
	CTrDialog::OnInitDialog();
	for (int i = 0; i < 3; ++i)
	{
		if (m_p->m_pdi[i])
			SetDlgItemText(IDC_FIRST + i, m_p->m_pdi[i]->diffFileInfo[0].GetFile());
		for (int j = 0; j < 3; ++j)
			ShowDlgItem(IDC_LEFT1 + i * 3 + j, (m_p->m_pdi[i] && m_p->m_pdi[i]->diffcode.exists(j)));
	}
	return true;
}

void DirSelectFilesDlg::Impl::OnBnClicked(UINT id)
{
	std::vector<int>& selectedButtons = m_p->m_selectedButtons;
	String text;
	GetDlgItemText(id, text);
	if (!text.empty())
		selectedButtons.erase(std::find(selectedButtons.begin(), selectedButtons.end(), static_cast<int>(id - IDC_LEFT1)));
	else
		selectedButtons.push_back(id - IDC_LEFT1);
	if (selectedButtons.size() > 3)
		selectedButtons.erase(selectedButtons.begin());
	UpdateButtonCaptions();
}

void DirSelectFilesDlg::Impl::OnBnClickedResetButton()
{
	m_p->m_selectedButtons.clear();
	UpdateButtonCaptions();
}

void DirSelectFilesDlg::Impl::UpdateButtonCaptions()
{
	for (int i = 0; i < 9; ++i)
	{
		SetDlgItemText(IDC_LEFT1 + i, _T(""));
	}
	int c = 1;
	for (int i: m_p->m_selectedButtons)
	{
		SetDlgItemText(IDC_LEFT1 + i, strutils::to_str(c));
		++c;
	}
}

DirSelectFilesDlg::DirSelectFilesDlg() : m_pimpl(new DirSelectFilesDlg::Impl(this)), m_pdi{} {}
DirSelectFilesDlg::~DirSelectFilesDlg() = default;
int DirSelectFilesDlg::DoModal() { return static_cast<int>(m_pimpl->DoModal()); }

