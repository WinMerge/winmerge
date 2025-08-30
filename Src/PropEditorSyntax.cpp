/** 
 * @file  PropEditorSyntax.cpp
 *
 * @brief Implementation of PropEditorSyntax propertysheet
 */

#include "stdafx.h"
#include "PropEditorSyntax.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"
#include "OptionsEditorSyntax.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** 
 * @brief Constructor.
 * @param [in] optionsMgr Pointer to options manager for handling options.
 */
PropEditorSyntax::PropEditorSyntax(COptionsMgr *optionsMgr)
: OptionsPanel(optionsMgr, PropEditorSyntax::IDD)
  , m_init(false)
{
}

/** 
 * @brief Function handling dialog data exchange between GUI and variables.
 */
void PropEditorSyntax::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(PropEditor)
	DDX_Control(pDX, IDC_SYNTAX_LIST, m_listSyntax);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropEditorSyntax, OptionsPanel)
	//{{AFX_MSG_MAP(PropEditor)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropEditorSyntax::ReadOptions()
{
	Options::EditorSyntax::Load(GetOptionsMgr(), m_Extension);
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropEditorSyntax::WriteOptions()
{
	if (m_init)
		for (int i = CrystalLineParser::SRC_ABAP; i <= CrystalLineParser::SRC_XML; ++i)
		{
			m_Extension[i-1] = m_listSyntax.GetItemText(i-1, 1);
			WildcardRemoveDuplicatePatterns(m_Extension[i-1]);
		}

	Options::EditorSyntax::Save(GetOptionsMgr(), m_Extension);
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropEditorSyntax::OnInitDialog()
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
void PropEditorSyntax::InitList()
{
	// Show selection across entire row.
	// Also enable infotips.
	m_listSyntax.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	const int lpx = CClientDC(this).GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };

	String title = _("File Type");
	m_listSyntax.InsertColumn(0, title.c_str(), LVCFMT_LEFT, pointToPixel(60));
	title = _("Extension");
	m_listSyntax.InsertColumn(1, title.c_str(), LVCFMT_LEFT, pointToPixel(196));

	m_listSyntax.SetReadOnlyColumn(0);
	m_listSyntax.SetEditStyle(1, CSubeditList::EditStyle::WILDCARD_DROP_LIST);


	String fixedPattern[CrystalLineParser::SRC_XML];
	Options::EditorSyntax::GetDefaults(GetOptionsMgr(), fixedPattern);

	for (int i = CrystalLineParser::SRC_ABAP; i <= CrystalLineParser::SRC_XML; ++i)
	{
		CrystalLineParser::TextDefinition* def = CrystalLineParser::GetTextType(i);
		
		ASSERT(def != nullptr);
		if (def != nullptr)
		{
			m_listSyntax.InsertItem(i-1, def->name);
			m_listSyntax.SetItemText(i-1, 1, m_Extension[i-1].c_str());
			m_listSyntax.SetDropListFixedPattern(i-1, 1, fixedPattern[i-1]);
		}
	}
}

/**
 * @brief Sets options to defaults
 */
void PropEditorSyntax::OnDefaults()
{
	Options::EditorSyntax::GetDefaults(GetOptionsMgr(), m_Extension);
	for (int i = CrystalLineParser::SRC_ABAP; i <= CrystalLineParser::SRC_XML; ++i)
		m_listSyntax.SetItemText(i-1, 1, m_Extension[i-1].c_str());
}
