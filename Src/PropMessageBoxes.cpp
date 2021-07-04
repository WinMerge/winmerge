/** 
 * @file  PropMessageBoxes.cpp
 *
 * @brief Implementation of PropMessageBoxes propertysheet
 */

#include "stdafx.h"
#include "PropMessageBoxes.h"
#include "MessageBoxDialog.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static struct MessageBox
{
	int nID;
	int nHelpID;
	int type;
} m_MessageBoxes[] =
{
	// folder compare window
	{ IDS_CREATE_PAIR_FOLDER, IDS_CREATE_PAIR_FOLDER, MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN },
	// file compare window
	{ IDS_COMPARE_LARGE_FILES, IDS_COMPARE_LARGE_FILES, MB_YESNOCANCEL | MB_ICONQUESTION | MB_DONT_ASK_AGAIN},
	{ IDS_FILESSAME, IDS_FILESSAME, MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN},
	{ IDS_FILE_TO_ITSELF, IDS_FILE_TO_ITSELF, MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN},
	{ IDS_NUM_REPLACED, IDS_NUM_REPLACED, MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN },
	{ IDS_SAVEREADONLY_MULTI, IDS_SAVEREADONLY_MULTI, MB_YESNOCANCEL | MB_ICONWARNING | MB_DEFBUTTON3 | MB_DONT_ASK_AGAIN | MB_YES_TO_ALL },
	{ IDS_SAVEREADONLY_FMT, IDS_SAVEREADONLY_FMT, MB_YESNOCANCEL | MB_ICONWARNING | MB_DEFBUTTON3 | MB_DONT_ASK_AGAIN | MB_YES_TO_ALL },
	{ IDS_MERGE_MODE, IDS_MERGE_MODE, MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN },
	{ IDS_FILECHANGED_RESCAN, IDS_FILECHANGED_RESCAN, MB_YESNO | MB_ICONWARNING },
	{ IDS_BACKUP_FAILED_PROMPT, IDS_BACKUP_FAILED_PROMPT,  MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN },
	{ IDS_SUGGEST_IGNOREEOL, IDS_SUGGEST_IGNOREEOL, MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN },
	{ IDS_MOVE_TO_NEXTFILE, IDS_MOVE_TO_NEXTFILE, MB_YESNO | MB_DONT_ASK_AGAIN },
	{ IDS_MOVE_TO_PREVFILE, IDS_MOVE_TO_PREVFILE, MB_YESNO | MB_DONT_ASK_AGAIN },
	{ IDS_MOVE_TO_FIRSTFILE, IDS_MOVE_TO_FIRSTFILE, MB_YESNO | MB_DONT_ASK_AGAIN },
	{ IDS_MOVE_TO_LASTFILE, IDS_MOVE_TO_LASTFILE, MB_YESNO | MB_DONT_ASK_AGAIN },
	{ IDS_MOVE_TO_NEXTPAGE, IDS_MOVE_TO_NEXTPAGE, MB_YESNO | MB_DONT_ASK_AGAIN },
	{ IDS_MOVE_TO_PREVPAGE, IDS_MOVE_TO_PREVPAGE, MB_YESNO | MB_DONT_ASK_AGAIN },
	// report dialog
	{ IDS_REPORT_FILEOVERWRITE, IDS_REPORT_FILEOVERWRITE, MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN },
	// patch dialog
	{ IDS_CANNOT_CREATE_BINARYPATCH, IDS_CANNOT_CREATE_BINARYPATCH, MB_ICONWARNING | MB_DONT_DISPLAY_AGAIN },
	{ IDS_DIFF_FILEOVERWRITE, IDS_DIFF_FILEOVERWRITE, MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN },
	{ IDS_DIFF_SUCCEEDED, IDS_DIFF_SUCCEEDED, MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN },
};

/** 
 * @brief Constructor.
 * @param [in] optionsMgr Pointer to options manager for handling options.
 */
PropMessageBoxes::PropMessageBoxes(COptionsMgr *optionsMgr)
: OptionsPanel(optionsMgr, PropMessageBoxes::IDD)
  , m_init(false)
{
}

/** 
 * @brief Function handling dialog data exchange between GUI and variables.
 */
void PropMessageBoxes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(PropEditor)
	DDX_Control(pDX, IDC_MESSAGEBOX_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropMessageBoxes, OptionsPanel)
	//{{AFX_MSG_MAP(PropEditor)
	ON_BN_CLICKED(IDC_RESET_ALL_MESSAGE_BOXES, OnResetAllMessageBoxes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropMessageBoxes::ReadOptions()
{
	m_answers.clear();
	for (unsigned i = 0; i < static_cast<unsigned>(std::size(m_MessageBoxes)); i++)
	{
		CString key = CMessageBoxDialog::GenerateRegistryKey(
			m_MessageBoxes[i].nID, m_MessageBoxes[i].nHelpID);
		m_answers.push_back(AfxGetApp()->GetProfileInt(_T("MessageBoxes"), key, -1));
	}
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropMessageBoxes::WriteOptions()
{
	if (m_init)
	{
		CMessageBoxDialog::ResetMessageBoxes();
		for (unsigned i = 0; i < static_cast<unsigned>(std::size(m_MessageBoxes)); i++)
		{
			CString key = CMessageBoxDialog::GenerateRegistryKey(
				m_MessageBoxes[i].nID, m_MessageBoxes[i].nHelpID);
			if (m_list.GetCheck(i))
				AfxGetApp()->WriteProfileInt(_T("MessageBoxes"), key, m_answers[i]);
		}
	}
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropMessageBoxes::OnInitDialog()
{
	OptionsPanel::OnInitDialog();

	InitList();
	m_init = true;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Initialize listcontrol containing file type extension settings.
 */
void PropMessageBoxes::InitList()
{
	m_list.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	const int lpx = CClientDC(this).GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };

	String title = _("Message");
	m_list.InsertColumn(0, title.c_str(), LVCFMT_LEFT, pointToPixel(160));
	title = _("Answer");
	m_list.InsertColumn(1, title.c_str(), LVCFMT_LEFT, pointToPixel(60));

	m_list.SetReadOnlyColumn(0);
	m_list.SetEditStyle(1, CSubeditList::EditStyle::DROPDOWN_LIST);

	for (unsigned i = 0; i < static_cast<unsigned>(std::size(m_MessageBoxes)); i++)
	{
		String str = LoadResString(m_MessageBoxes[i].nID);
		CString key = CMessageBoxDialog::GenerateRegistryKey(
			m_MessageBoxes[i].nID, m_MessageBoxes[i].nHelpID);
		int ans = AfxGetApp()->GetProfileInt(_T("MessageBoxes"), key, -1);
		
		m_list.InsertItem(i, str.c_str());
		m_list.SetCheck(i, ans != -1);
		String ansstr;
		switch (ans)
		{
		case IDYES: ansstr = _("Yes"); break;
		case IDNO: ansstr = _("No"); break;
		case IDCANCEL: ansstr = _("Cancel"); break;
		case IDOK: ansstr = _("OK"); break;
		default: ansstr = _T(""); break;
		}
		m_list.SetItemText(i, 1, ansstr.c_str());
	}
}

/** 
 * @brief Called when user wants to see all messageboxes again.
 */
void PropMessageBoxes::OnResetAllMessageBoxes()
{
	CMessageBoxDialog::ResetMessageBoxes();
	ReadOptions();
	InitList();
	// The "don't show again" checkbox of the Confirm Copy dialog uses the same registry key
	// as CMessageBoxDialog does, so its state will also be reset
	AfxMessageBox(_("All message boxes are now displayed again.").c_str(), MB_ICONINFORMATION);
}
