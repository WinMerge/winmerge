/** 
 * @file  PropProject.cpp
 *
 * @brief Implementation of PropProject propertysheet
 */

#include "stdafx.h"
#include "PropProject.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"
#include "OptionsProject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** 
 * @brief Constructor.
 * @param [in] optionsMgr Pointer to options manager for handling options.
 */
PropProject::PropProject(COptionsMgr *optionsMgr)
: OptionsPanel(optionsMgr, PropProject::IDD)
{
	for (int i = 0; i < Options::Project::OperationCount; i++)
		for (int j = 0; j < Options::Project::ItemCount; j++)
			m_settings[i][j] = false;
}

/** 
 * @brief Function handling dialog data exchange between GUI and variables.
 */
void PropProject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(PropEditor)
	DDX_Control(pDX, IDC_PROJECT_LIST, m_list);
	//}}AFX_DATA_MAP

	if (!pDX->m_bSaveAndValidate)
	{
		for (int i = 0; i < Options::Project::OperationCount; i++)
			for (int j = 0; j < Options::Project::ItemCount; j++)
				m_list.SetItemBooleanValue(j, i + 1, m_settings[i][j]);

	}
	else
	{
		for (int i = 0; i < Options::Project::OperationCount; i++)
			for (int j = 0; j < Options::Project::ItemCount; j++)
				m_settings[i][j] = m_list.GetItemBooleanValue(j, i + 1);
	}

}


BEGIN_MESSAGE_MAP(PropProject, OptionsPanel)
	//{{AFX_MSG_MAP(PropEditor)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropProject::ReadOptions()
{
	Options::Project::Load(GetOptionsMgr(), m_settings);
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropProject::WriteOptions()
{
	Options::Project::Save(GetOptionsMgr(), m_settings);
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropProject::OnInitDialog()
{
	OptionsPanel::OnInitDialog();

	InitList();

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Sets options to defaults
 */
void PropProject::OnDefaults()
{
	for (int i = 0; i < Options::Project::OperationCount; i++)
		for (int j = 0; j < Options::Project::ItemCount; j++)
			m_settings[i][j] = Options::Project::GetDefault(GetOptionsMgr(), static_cast<Options::Project::Operation>(i), static_cast<Options::Project::Item>(j));

	UpdateData(FALSE);
}

/**
 * @brief Initialize listcontrol containing project settings.
 */
void PropProject::InitList()
{
	// Show selection across entire row.
	// Also enable infotips.
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	const int lpx = CClientDC(this).GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };

	String title = _("Item");
	m_list.InsertColumn(0, title.c_str(), LVCFMT_LEFT, pointToPixel(150));
	title = _("Open");
	m_list.InsertColumn(1, title.c_str(), LVCFMT_LEFT, pointToPixel(50));
	title = _("Load");
	m_list.InsertColumn(2, title.c_str(), LVCFMT_LEFT, pointToPixel(50));
	title = _("Save");
	m_list.InsertColumn(3, title.c_str(), LVCFMT_LEFT, pointToPixel(50));

	m_list.SetReadOnlyColumn(0);
	for (int i = 0; i < Options::Project::OperationCount; ++i)
	{
		m_list.SetEditStyle(i+1, CSubeditList::EditStyle::EDIT_BOX);
		m_list.SetBooleanValueColumn(i+1);
	}

	int nID[Options::Project::ItemCount] = {
		IDS_PROJECT_ITEM_FILE_FILTER,
		IDS_PROJECT_ITEM_INCLUDE_SUBFOLDERS,
		IDS_PROJECT_ITEM_PLUGIN,
		IDS_PROJECT_ITEM_COMPARE_OPTIONS,
		IDS_PROJECT_ITEM_HIDDEN_ITEMS
	};

	for (int i = 0; i < Options::Project::ItemCount; i++)
	{
		String str = I18n::LoadString(nID[i]);
		m_list.InsertItem(i, str.c_str());
	}
}
