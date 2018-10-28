/** 
 * @file  PropCompareBinary.cpp
 *
 * @brief Implementation of PropCompareBinary propertysheet
 */

#include "stdafx.h"
#include "PropCompareBinary.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"
#include "heksedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class Heksedit
{
public:
	explicit Heksedit(CWnd *pwndParent)
	{
		pv = LoadLibrary(_T("Frhed\\hekseditU.dll"));
		if (pv == nullptr)
		{
			LangMessageBox(IDS_FRHED_NOTINSTALLED, MB_OK);
			return;
		}
		wnd.Create(_T("heksedit"), nullptr, 0, CRect(), pwndParent, 1);
		get_interface()->read_ini_data();
		get_interface()->get_settings()->bSaveIni = true;
	}

	IHexEditorWindow *get_interface()
	{
		return reinterpret_cast<IHexEditorWindow *>(::GetWindowLongPtr(wnd.m_hWnd, GWLP_USERDATA));
	}

private:
	HMODULE pv;
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
	DDX_Text(pDX, IDC_COMPAREBINARY_PATTERNS, m_sFilePatterns);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropCompareBinary, CPropertyPage)
	//{{AFX_MSG_MAP(PropCompareBinary)
	ON_BN_CLICKED(IDC_COMPAREBINARY_VIEWSETTINGS, OnViewSettings)
	ON_BN_CLICKED(IDC_COMPAREBINARY_BINARYMODE, OnBinaryMode)
	ON_BN_CLICKED(IDC_COMPAREBINARY_CHARACTERSET, OnCharacterSet)
	ON_BN_CLICKED(IDC_COMPAREBINARY_DEFAULTS, OnDefaults)
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
