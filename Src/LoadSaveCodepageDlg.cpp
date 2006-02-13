/** 
 * @file  LoadSaveCodepageDlg.cpp
 *
 * @brief Implementation of the dialog used to select codepages
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "resource.h"
#include "LoadSaveCodepageDlg.h"
#include "dlgutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoadSaveCodepageDlg dialog


CLoadSaveCodepageDlg::CLoadSaveCodepageDlg(CWnd* pParent /*=NULL*/)
: CDialog(CLoadSaveCodepageDlg::IDD, pParent)
, m_bAffectsLeft(TRUE)
, m_bAffectsRight(TRUE)
, m_bLoadSaveSameCodepage(TRUE)
// CString m_sAffectsLeftString
// CString m_sAffectsRightString
, m_nLoadCodepage(-1)
, m_nSaveCodepage(-1)
, m_bEnableSaveCodepage(false)
{
	//{{AFX_DATA_INIT(CLoadSaveCodepageDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CLoadSaveCodepageDlg::SetLeftRightAffectStrings(const CString & sAffectsLeft, const CString & sAffectsRight)
{
	m_sAffectsLeftString = sAffectsLeft;
	m_sAffectsRightString = sAffectsRight;
}

void CLoadSaveCodepageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoadSaveCodepageDlg)
	DDX_Check(pDX, IDC_AFFECTS_LEFT_BTN, m_bAffectsLeft);
	DDX_Check(pDX, IDC_AFFECTS_RIGHT_BTN, m_bAffectsRight);
	DDX_Check(pDX, IDC_LOAD_SAVE_SAME_CODEPAGE, m_bLoadSaveSameCodepage);
	DDX_Control(pDX, IDC_AFFECTS_LEFT_BTN, m_AffectsLeftBtn);
	DDX_Control(pDX, IDC_AFFECTS_RIGHT_BTN, m_AffectsRightBtn);
	DDX_Text(pDX, IDC_LOAD_CODEPAGE_TEXTBOX, m_nLoadCodepage);
	DDX_Text(pDX, IDC_SAVE_CODEPAGE_TEXTBOX, m_nSaveCodepage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoadSaveCodepageDlg, CDialog)
	//{{AFX_MSG_MAP(CLoadSaveCodepageDlg)
	ON_BN_CLICKED(IDC_AFFECTS_LEFT_BTN, OnAffectsLeftBtnClicked)
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
	CDialog::OnInitDialog();

	dlgutil_SetMainIcon(this);
	
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
	m_constraint.ConstrainItem(IDC_LOAD_CODEPAGE_BROWSE, 1, 0, 0, 0); // moves right
	m_constraint.ConstrainItem(IDC_SAVING_GROUP, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_SAVE_CODEPAGE_TEXTBOX, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_SAVE_CODEPAGE_BROWSE, 1, 0, 0, 0); // moves right
	m_constraint.ConstrainItem(IDOK, 0.33, 0, 0, 0); // floats right
	m_constraint.ConstrainItem(IDCANCEL, 0.67, 0, 0, 0); // floats right
	m_constraint.DisallowHeightGrowth();
	m_constraint.SubclassWnd(); // install subclassing
	m_constraint.LoadPosition(_T("ResizeableDialogs"), _T("LoadSaveCodepageDlg"), false); // persist size via registry

	dlgutil_CenterToMainFrame(this);

	SetDlgItemText(IDC_LEFT_FILES_LABEL, m_sAffectsLeftString);
	SetDlgItemText(IDC_RIGHT_FILES_LABEL, m_sAffectsRightString);

	UpdateSaveGroup();

	// TODO: Implement browse
	EnableDlgItem(IDC_LOAD_CODEPAGE_BROWSE, false); // browse not yet implemented
	EnableDlgItem(IDC_SAVE_CODEPAGE_BROWSE, false); // browse not yet implemented
	
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
		m_AffectsRightBtn.SetCheck(BST_CHECKED);
}

/**
 * @brief If user unchecks right, then check left (to ensure never have nothing checked)
 */
void CLoadSaveCodepageDlg::OnAffectsRightBtnClicked()
{
	if (m_AffectsRightBtn.GetCheck() != BST_CHECKED)
		m_AffectsLeftBtn.SetCheck(BST_CHECKED);
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
	EnableDlgItem(IDC_SAVE_CODEPAGE_BROWSE, EnableSave);
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
