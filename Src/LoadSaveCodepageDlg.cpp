/** 
 * @file  LoadSaveCodepageDlg.cpp
 *
 * @brief Implementation of the dialog used to select codepages
 */
// ID line follows -- this is updated by SVN
// $Id: LoadSaveCodepageDlg.cpp 5394 2008-05-29 09:47:36Z kimmov $

#include "stdafx.h"
#include "Merge.h"
#include "MainFrm.h"
#include "resource.h"
#include "LoadSaveCodepageDlg.h"
#include "unicoder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoadSaveCodepageDlg dialog


CLoadSaveCodepageDlg::CLoadSaveCodepageDlg(int nFiles, CWnd* pParent /*=NULL*/)
: CDialog(CLoadSaveCodepageDlg::IDD, pParent)
, m_bAffectsLeft(TRUE)
, m_bAffectsMiddle(TRUE)
, m_bAffectsRight(TRUE)
, m_bLoadSaveSameCodepage(TRUE)
// CString m_sAffectsLeftString
// CString m_sAffectsRightString
, m_nLoadCodepage(-1)
, m_nSaveCodepage(-1)
, m_bEnableSaveCodepage(false)
, m_nFiles(nFiles)
{
	//{{AFX_DATA_INIT(CLoadSaveCodepageDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CLoadSaveCodepageDlg::SetLeftRightAffectStrings(const CString & sAffectsLeft, const CString & sAffectsMiddle, const CString & sAffectsRight)
{
	m_sAffectsLeftString = sAffectsLeft;
	m_sAffectsMiddleString = sAffectsMiddle;
	m_sAffectsRightString = sAffectsRight;
}

void CLoadSaveCodepageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoadSaveCodepageDlg)
	DDX_Check(pDX, IDC_AFFECTS_LEFT_BTN, m_bAffectsLeft);
	DDX_Check(pDX, IDC_AFFECTS_MIDDLE_BTN, m_bAffectsMiddle);
	DDX_Check(pDX, IDC_AFFECTS_RIGHT_BTN, m_bAffectsRight);
	DDX_Check(pDX, IDC_LOAD_SAVE_SAME_CODEPAGE, m_bLoadSaveSameCodepage);
	DDX_Control(pDX, IDC_AFFECTS_LEFT_BTN, m_AffectsLeftBtn);
	DDX_Control(pDX, IDC_AFFECTS_MIDDLE_BTN, m_AffectsMiddleBtn);
	DDX_Control(pDX, IDC_AFFECTS_RIGHT_BTN, m_AffectsRightBtn);
	DDX_Text(pDX, IDC_LOAD_CODEPAGE_TEXTBOX, m_nLoadCodepage);
	DDX_Text(pDX, IDC_SAVE_CODEPAGE_TEXTBOX, m_nSaveCodepage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoadSaveCodepageDlg, CDialog)
	//{{AFX_MSG_MAP(CLoadSaveCodepageDlg)
	ON_BN_CLICKED(IDC_AFFECTS_LEFT_BTN, OnAffectsLeftBtnClicked)
	ON_BN_CLICKED(IDC_AFFECTS_MIDDLE_BTN, OnAffectsMiddleBtnClicked)
	ON_BN_CLICKED(IDC_AFFECTS_RIGHT_BTN, OnAffectsRightBtnClicked)
	ON_BN_CLICKED(IDC_LOAD_SAVE_SAME_CODEPAGE, OnLoadSaveSameCodepage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoadSaveCodepageDlg message handlers

/**
 * @brief Handler for WM_INITDIALOG; conventional location to initialize controls
 * At this point dialog and control windows exist
 */
BOOL CLoadSaveCodepageDlg::OnInitDialog() 
{
	theApp.TranslateDialog(m_hWnd);
	CDialog::OnInitDialog();

	CMainFrame::SetMainIcon(this);
	
	// setup handler for resizing this dialog	
	m_constraint.InitializeCurrentSize(this);
	// configure how individual controls adjust when dialog resizes
	m_constraint.ConstrainItem(IDC_AFFECTS_GROUP, 0, 1, 0, 0); // grows right
	// IDC_AFFECTS_LEFT_BTN doesn't move or grow
	m_constraint.ConstrainItem(IDC_LEFT_FILES_LABEL, 0, 1, 0, 0); // grows right
	// IDC_AFFECTS_RIGHT_BTN doesn't move or grow
	m_constraint.ConstrainItem(IDC_RIGHT_FILES_LABEL, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_LOADING_GROUP, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_LOAD_CODEPAGE_TEXTBOX, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_SAVING_GROUP, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_SAVE_CODEPAGE_TEXTBOX, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDOK, 0.33, 0, 0, 0); // floats right
	m_constraint.ConstrainItem(IDCANCEL, 0.67, 0, 0, 0); // floats right
	m_constraint.DisallowHeightGrowth();
	m_constraint.SubclassWnd(); // install subclassing
	m_constraint.LoadPosition(_T("ResizeableDialogs"), _T("LoadSaveCodepageDlg"), false); // persist size via registry

	ucr::IExconverterPtr pexconv(ucr::createConverterMLang());
	if (pexconv != NULL)
	{
		CComboBox combo;
		combo.Attach(::GetDlgItem(m_hWnd, IDC_LOAD_CODEPAGE_TEXTBOX));
		ucr::CodePageInfo cpi[256];
		int count = pexconv->enumCodePages(cpi, sizeof(cpi)/sizeof(ucr::CodePageInfo));
		int i, j;
		for (i = 0, j = 0; i < count; i++)
		{
			if (cpi[i].codepage == 1200 /* UNICODE */)
				continue;
			TCHAR desc[256];
			wsprintf(desc, _T("%05d - %ls"), cpi[i].codepage, cpi[i].desc);
			combo.AddString(desc);
			combo.SetItemData(j, cpi[i].codepage);
			if (cpi[i].codepage == m_nLoadCodepage)
				combo.SetCurSel(j);
			j++;
		}
		combo.Detach();
	}

	CMainFrame::CenterToMainFrame(this);

	SetDlgItemText(IDC_LEFT_FILES_LABEL, m_sAffectsLeftString);
	SetDlgItemText(IDC_MIDDLE_FILES_LABEL, m_sAffectsMiddleString);
	SetDlgItemText(IDC_RIGHT_FILES_LABEL, m_sAffectsRightString);

	UpdateSaveGroup();

	if (m_nFiles < 3)
		EnableDlgItem(IDC_AFFECTS_MIDDLE_BTN, false);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Shortcut to enable or disable a control
 */
void CLoadSaveCodepageDlg::EnableDlgItem(int item, bool enable)
{
	GetDlgItem(item)->EnableWindow(!!enable);
}

/**
 * @brief If user unchecks left, then check right (to ensure never have nothing checked)
 */
void CLoadSaveCodepageDlg::OnAffectsLeftBtnClicked()
{
	if (m_AffectsLeftBtn.GetCheck() != BST_CHECKED)
	{
		if (m_nFiles < 3)
			m_AffectsRightBtn.SetCheck(BST_CHECKED);
		else
		{
			if (m_AffectsMiddleBtn.GetCheck() != BST_CHECKED)
			{
				if (m_AffectsRightBtn.GetCheck() != BST_CHECKED)
					m_AffectsRightBtn.SetCheck(BST_CHECKED);
			}
		}
	}
}

/**
 * @brief If user unchecks middle, then check right (to ensure never have nothing checked)
 */
void CLoadSaveCodepageDlg::OnAffectsMiddleBtnClicked()
{
	if (m_AffectsMiddleBtn.GetCheck() != BST_CHECKED)
	{
		if (m_AffectsLeftBtn.GetCheck() != BST_CHECKED)
		{
			if (m_AffectsRightBtn.GetCheck() != BST_CHECKED)
				m_AffectsLeftBtn.SetCheck(BST_CHECKED);
		}
	}
}

/**
 * @brief If user unchecks right, then check left (to ensure never have nothing checked)
 */
void CLoadSaveCodepageDlg::OnAffectsRightBtnClicked()
{
	if (m_AffectsRightBtn.GetCheck() != BST_CHECKED)
	{
		if (m_nFiles < 3)
			m_AffectsLeftBtn.SetCheck(BST_CHECKED);
		else
		{
			if (m_AffectsMiddleBtn.GetCheck() != BST_CHECKED)
			{
				if (m_AffectsLeftBtn.GetCheck() != BST_CHECKED)
					m_AffectsLeftBtn.SetCheck(BST_CHECKED);
			}
		}
	}
}

/**
 * @brief Disable save group if save codepage slaved to load codepage
 */
void CLoadSaveCodepageDlg::OnLoadSaveSameCodepage()
{
	UpdateSaveGroup();
}

/**
 * @brief Disable save group if save codepage slaved to load codepage
 */
void CLoadSaveCodepageDlg::UpdateSaveGroup()
{
	UpdateDataFromWindow();
	if (!m_bEnableSaveCodepage)
		EnableDlgItem(IDC_LOAD_SAVE_SAME_CODEPAGE, false);
	bool EnableSave = m_bEnableSaveCodepage && !m_bLoadSaveSameCodepage;
	EnableDlgItem(IDC_SAVE_CODEPAGE_TEXTBOX, EnableSave);
}

/**
 * @brief User pressed Ok, ensure data members set correctly
 */
void CLoadSaveCodepageDlg::OnOK ()
{
	CDialog::OnOK();
	if (m_bLoadSaveSameCodepage)
	{
		m_nSaveCodepage = m_nLoadCodepage;
	}
}
