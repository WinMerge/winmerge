/** 
 * @file  PropCompareBinary.cpp
 *
 * @brief Implementation of PropCompareBinary propertysheet
 */

#include "stdafx.h"
#include "PropCompareBinary.h"
#include "WildcardDropList.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"
#include "heksedit.h"
#include "MergeDarkMode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class Heksedit
{
public:
	explicit Heksedit(CWnd *pwndParent)
	{
		HMODULE pv = GetModuleHandle(_T("hekseditU.dll"));
		if (pv == nullptr)
			pv = LoadLibrary(_T("Frhed\\hekseditU.dll"));
		if (pv == nullptr)
		{
			AfxMessageBox(strutils::format_string1(_("%1 is not installed."), _T("Frhed")).c_str(), MB_OK);
			return;
		}
		wnd.Create(_T("heksedit"), nullptr, 0, CRect(), pwndParent, 1);
		get_interface()->read_ini_data();
		get_interface()->get_settings()->bSaveIni = true;
		get_interface()->set_theme_callback([](HWND hwnd, IHexEditorWindow::WINDOW_TYPE windowType) {
			if (windowType == IHexEditorWindow::WINDOW_DIALOG)
				DarkMode::setDarkWndSafe(hwnd, true);
		});
	}

	~Heksedit()
	{
		wnd.DestroyWindow();
	}

	Heksedit(const Heksedit&) = delete;

	IHexEditorWindow *get_interface()
	{
		return reinterpret_cast<IHexEditorWindow *>(::GetWindowLongPtr(wnd.m_hWnd, GWLP_USERDATA));
	}

private:
	CWnd wnd;
};

/** 
 * @brief Constructor.
 * @param [in] optionsMgr Pointer to COptionsMgr.
 */
PropCompareBinary::PropCompareBinary(COptionsMgr *optionsMgr) 
 : OptionsPanel(optionsMgr, PropCompareBinary::IDD)
{
}

void PropCompareBinary::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropCompareBinary)
	DDX_Control(pDX, IDC_COMPAREBINARY_PATTERNS, m_comboPatterns);
	DDX_Text(pDX, IDC_COMPAREBINARY_PATTERNS, m_sFilePatterns);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropCompareBinary, OptionsPanel)
	//{{AFX_MSG_MAP(PropCompareBinary)
	ON_BN_CLICKED(IDC_COMPAREBINARY_VIEWSETTINGS, OnViewSettings)
	ON_BN_CLICKED(IDC_COMPAREBINARY_BINARYMODE, OnBinaryMode)
	ON_BN_CLICKED(IDC_COMPAREBINARY_CHARACTERSET, OnCharacterSet)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	ON_CBN_DROPDOWN(IDC_COMPAREBINARY_PATTERNS, OnDropDownPatterns)
	ON_CBN_CLOSEUP(IDC_COMPAREBINARY_PATTERNS, OnCloseUpPatterns)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 * Property sheet calls this before displaying GUI to load values
 * into members.
 */
void PropCompareBinary::ReadOptions()
{
	m_sFilePatterns = GetOptionsMgr()->GetString(OPT_CMP_BIN_FILEPATTERNS);
}

/** 
 * @brief Writes options values from UI to storage.
 * Property sheet calls this after dialog is closed with OK button to
 * store values in member variables.
 */
void PropCompareBinary::WriteOptions()
{
	WildcardRemoveDuplicatePatterns(m_sFilePatterns);
	GetOptionsMgr()->SaveOption(OPT_CMP_BIN_FILEPATTERNS, m_sFilePatterns);
}

/** 
 * @brief Show Frhed view settings dialog
 */
void PropCompareBinary::OnViewSettings()
{
	Heksedit heksedit(this);
	if (heksedit.get_interface())
		heksedit.get_interface()->CMD_view_settings();
}

/** 
 * @brief Show Frhed binary mode dialog
 */
void PropCompareBinary::OnBinaryMode()
{
	Heksedit heksedit(this);
	if (heksedit.get_interface())
		heksedit.get_interface()->CMD_binarymode();
}

/** 
 * @brief Show Frhed characterset dialog
 */
void PropCompareBinary::OnCharacterSet()
{
	Heksedit heksedit(this);
	if (heksedit.get_interface())
		heksedit.get_interface()->CMD_character_set();
}

/** 
 * @brief Sets options to defaults
 */
void PropCompareBinary::OnDefaults()
{
	m_sFilePatterns = GetOptionsMgr()->GetDefault<String>(OPT_CMP_BIN_FILEPATTERNS);
	UpdateData(FALSE);
}

/**
 * @brief Prepares multi-selection drop list 
 */
void PropCompareBinary::OnDropDownPatterns()
{
	String patterns = GetOptionsMgr()->GetDefault<String>(OPT_CMP_BIN_FILEPATTERNS);
	WildcardDropList::OnDropDown(m_comboPatterns, 6, patterns.c_str());
}

/**
 * @brief Finishes drop list multi-selection
 */
void PropCompareBinary::OnCloseUpPatterns()
{
	WildcardDropList::OnCloseUp(m_comboPatterns);
}
