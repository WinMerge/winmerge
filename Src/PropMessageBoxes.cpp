/** 
 * @file  PropMessageBoxes.cpp
 *
 * @brief Implementation of PropMessageBoxes propertysheet
 */

#include "stdafx.h"
#include "PropMessageBoxes.h"
#include "MessageBoxDialog.h"
#include "OptionsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static struct MessageBox
{
	int nID;
	int nHelpID;
	const tchar_t* key;
	int type;
} MessageBoxes[] =
{
	// folder compare window
	{ IDS_CREATE_PAIR_FOLDER, IDS_CREATE_PAIR_FOLDER, nullptr, MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN },
	{ IDS_CONFIRM_SINGLE_COPY, 0, _T("FolderCopyConfirmDlgDontAskAgain"), MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN },
	// file compare window
	{ IDS_COMPARE_LARGE_FILES, 0, _T("CompareLargeFiles"), MB_YESNOCANCEL | MB_ICONQUESTION | MB_DONT_ASK_AGAIN},
	{ IDS_FILESSAME, IDS_FILESSAME, nullptr, MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN},
	{ IDS_FILE_TO_ITSELF, IDS_FILE_TO_ITSELF, nullptr, MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN},
	{ IDS_NUM_REPLACED, IDS_NUM_REPLACED, nullptr, MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN },
	{ IDS_SAVEREADONLY_MULTI, IDS_SAVEREADONLY_MULTI, nullptr, MB_YESNOCANCEL | MB_ICONWARNING | MB_DEFBUTTON3 | MB_DONT_ASK_AGAIN | MB_YES_TO_ALL },
	{ IDS_SAVEREADONLY_FMT, IDS_SAVEREADONLY_FMT, nullptr, MB_YESNOCANCEL | MB_ICONWARNING | MB_DEFBUTTON3 | MB_DONT_ASK_AGAIN | MB_YES_TO_ALL },
	{ IDS_MERGE_MODE, IDS_MERGE_MODE, nullptr, MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN },
	{ IDS_FILECHANGED_RESCAN, IDS_FILECHANGED_RESCAN, nullptr, MB_YESNO | MB_ICONWARNING },
	{ IDS_BACKUP_FAILED_PROMPT, IDS_BACKUP_FAILED_PROMPT,  nullptr, MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN },
	{ IDS_SUGGEST_IGNOREEOL, IDS_SUGGEST_IGNOREEOL, nullptr, MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN },
	{ IDS_CONFIRM_COPY_ALL_DIFFS, IDS_CONFIRM_COPY_ALL_DIFFS, nullptr, MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN },
	{ IDS_MOVE_TO_NEXTFILE, IDS_MOVE_TO_NEXTFILE, nullptr, MB_YESNO | MB_DONT_ASK_AGAIN },
	{ IDS_MOVE_TO_PREVFILE, IDS_MOVE_TO_PREVFILE, nullptr, MB_YESNO | MB_DONT_ASK_AGAIN },
	{ IDS_MOVE_TO_FIRSTFILE, IDS_MOVE_TO_FIRSTFILE, nullptr, MB_YESNO | MB_DONT_ASK_AGAIN },
	{ IDS_MOVE_TO_LASTFILE, IDS_MOVE_TO_LASTFILE, nullptr, MB_YESNO | MB_DONT_ASK_AGAIN },
	{ IDS_MOVE_TO_NEXTPAGE, IDS_MOVE_TO_NEXTPAGE, nullptr, MB_YESNO | MB_DONT_ASK_AGAIN },
	{ IDS_MOVE_TO_PREVPAGE, IDS_MOVE_TO_PREVPAGE, nullptr, MB_YESNO | MB_DONT_ASK_AGAIN },
	// report dialog
	{ IDS_REPORT_FILEOVERWRITE, IDS_REPORT_FILEOVERWRITE, nullptr, MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN },
	// patch dialog
	{ IDS_CANNOT_CREATE_BINARYPATCH, IDS_CANNOT_CREATE_BINARYPATCH, nullptr, MB_ICONWARNING | MB_DONT_DISPLAY_AGAIN },
	{ IDS_DIFF_FILEOVERWRITE, IDS_DIFF_FILEOVERWRITE, nullptr, MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN },
	{ IDS_DIFF_SUCCEEDED, IDS_DIFF_SUCCEEDED, nullptr, MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN },
	// archive support
	{ IDS_FAILED_EXTRACT_ARCHIVE_FILES, IDS_FAILED_EXTRACT_ARCHIVE_FILES, nullptr, MB_YESNO | MB_DONT_ASK_AGAIN | MB_ICONWARNING }
};

static std::vector<String> Answers;
static std::vector<std::vector<String>> DropdownList;

/** 
 * @brief Constructor.
 * @param [in] optionsMgr Pointer to options manager for handling options.
 */
PropMessageBoxes::PropMessageBoxes(COptionsMgr *optionsMgr)
: OptionsPanel(optionsMgr, PropMessageBoxes::IDD)
{
	Answers = {
		_T(""),
		_("OK"),
		_("Cancel"),
		_("Abort"),
		_("Retry"),
		_("Ignore"),
		_("Yes"),
		_("No"),
		_("Close"),
		_("Help"),
		_("Try Again"),
		_("Continue"),
	};

	DropdownList = {
		{ _("OK") },
		{ _("OK"), _("Cancel") },
		{ _("Abort"), _("Retry"), _("Ignore") },
		{ _("Yes"), _("No"), _("Cancel") },
		{ _("Yes"), _("No") },
		{ _("Retry"), _("Cancel") },
		{ _("Cancel"), _("Try Again"), _("Continue") },
	};
}

static int AnswerStringtoID(const String& text)
{
	auto it = std::find(Answers.begin(), Answers.end(), text);
	return static_cast<int>(it - Answers.begin());
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
	if (m_list.GetItemCount() > 0)
	{
		if (!pDX->m_bSaveAndValidate)
		{
			for (unsigned i = 0; i < static_cast<unsigned>(std::size(MessageBoxes)); i++)
			{
				int ans = m_answers[i];
				m_list.SetCheck(i, ans != -1);
				m_list.SetItemText(i, 1, (ans < 0 || ans >= static_cast<int>(Answers.size())) ? _T("") : Answers[ans].c_str());
			}
		}
		else
		{
			for (unsigned i = 0; i < static_cast<unsigned>(std::size(MessageBoxes)); i++)
			{
				m_answers[i] = -1;
				if (m_list.GetCheck(i))
				{
					int ans = AnswerStringtoID(String{ m_list.GetItemText(i, 1) });
					if (ans >= IDOK && ans <= IDCONTINUE)
						m_answers[i] = ans;
				}
			}
		}
	}
}

BEGIN_MESSAGE_MAP(PropMessageBoxes, OptionsPanel)
	//{{AFX_MSG_MAP(PropEditor)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MESSAGEBOX_LIST, OnLVNItemChanged)
	ON_BN_CLICKED(IDC_RESET_ALL_MESSAGE_BOXES, OnResetAllMessageBoxes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropMessageBoxes::ReadOptions()
{
	m_answers.clear();
	for (unsigned i = 0; i < static_cast<unsigned>(std::size(MessageBoxes)); i++)
	{
		CString key = (MessageBoxes[i].key != nullptr) ? MessageBoxes[i].key : CMessageBoxDialog::GenerateRegistryKey(
			MessageBoxes[i].nID, MessageBoxes[i].nHelpID);
		m_answers.push_back(AfxGetApp()->GetProfileInt(_T("MessageBoxes"), key, -1));
	}
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropMessageBoxes::WriteOptions()
{
	CMessageBoxDialog::ResetMessageBoxes();
	for (unsigned i = 0; i < static_cast<unsigned>(std::size(MessageBoxes)); i++)
	{
		CString key = (MessageBoxes[i].key != nullptr) ? MessageBoxes[i].key :
			CMessageBoxDialog::GenerateRegistryKey(
			MessageBoxes[i].nID, MessageBoxes[i].nHelpID);
		int ans = m_answers[i];
		if (ans >= IDOK && ans <= IDCONTINUE)
			AfxGetApp()->WriteProfileInt(_T("MessageBoxes"), key, ans);
	}
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropMessageBoxes::OnInitDialog()
{
	OptionsPanel::OnInitDialog();

	m_list.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	const int lpx = CClientDC(this).GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };

	String title = _("Message");
	m_list.InsertColumn(0, title.c_str(), LVCFMT_LEFT, pointToPixel(220));
	title = _("Answer");
	m_list.InsertColumn(1, title.c_str(), LVCFMT_LEFT, pointToPixel(70));

	m_list.SetReadOnlyColumn(0);
	m_list.SetEditStyle(1, CSubeditList::EditStyle::DROPDOWN_LIST);

	for (unsigned i = 0; i < static_cast<unsigned>(std::size(MessageBoxes)); i++)
	{
		String str = LoadResString(MessageBoxes[i].nID);
		strutils::replace(str, _T("\n"), _T(" "));
		m_list.InsertItem(i, str.c_str());
		unsigned type = MessageBoxes[i].type & 0xf;
		m_list.SetDropdownList(i, 1, (type >= DropdownList.size()) ? std::vector<String>{} : DropdownList[type]);
	}

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void PropMessageBoxes::OnLVNItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLISTVIEW* pnmv = reinterpret_cast<NMLISTVIEW *>(pNMHDR);
	int iItem = pnmv->iItem;
	if (m_list.GetCheck(iItem))
	{
		if (m_list.GetItemText(iItem, 1).IsEmpty())
			m_list.SetItemText(iItem, 1, m_list.GetDropdownList(iItem, 1)[0].c_str());
	}
	else
	{
		if (!m_list.GetItemText(iItem, 1).IsEmpty())
			m_list.SetItemText(iItem, 1, _T(""));
	}
}

/** 
 * @brief Called when user wants to see all messageboxes again.
 */
void PropMessageBoxes::OnResetAllMessageBoxes()
{
	CMessageBoxDialog::ResetMessageBoxes();
	ReadOptions();
	UpdateData(false);
	// The "don't show again" checkbox of the Confirm Copy dialog uses the same registry key
	// as CMessageBoxDialog does, so its state will also be reset
	AfxMessageBox(_("All message boxes are now displayed again.").c_str(), MB_ICONINFORMATION);
}
