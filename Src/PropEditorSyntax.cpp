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
	DDX_Control(pDX, IDC_SQL_DIALECT, m_comboSqlDialect);
	DDX_Control(pDX, IDC_SYNTAX_LIST, m_listSyntax);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropEditorSyntax, OptionsPanel)
	//{{AFX_MSG_MAP(PropEditor)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	ON_CBN_SELCHANGE(IDC_SQL_DIALECT, OnSqlDialectChanged)
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
		for (int i = LangServices::LanguageId::SRC_ABAP; i < LangServices::LanguageId::SRC_MAX_ENTRY; ++i)
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

	const int dialects[] = {
		LangServices::LanguageId::SRC_SQL,
		LangServices::LanguageId::SRC_SQL_POSTGRESQL,
		LangServices::LanguageId::SRC_SQL_MYSQL,
	};
	int selected = 0;
	for (int i = 0; i < _countof(dialects); ++i)
	{
		const int item = m_comboSqlDialect.AddString(LangServices::GetTextType(dialects[i])->name);
		m_comboSqlDialect.SetItemData(item, dialects[i]);
		if (HasSqlExtension(m_Extension[dialects[i] - 1]))
			selected = i;
	}
	m_comboSqlDialect.SetCurSel(selected);

	InitList();
	m_init = true;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Returns whether an extension list contains the sql extension.
 */
bool PropEditorSyntax::HasSqlExtension(const String& extensions)
{
	size_t begin = 0;
	while (begin < extensions.length())
	{
		const size_t end = extensions.find_first_of(L"; ", begin);
		const String extension = strutils::trim_ws(extensions.substr(begin, end - begin));
		if (strutils::compare_nocase(extension, _T("sql")) == 0)
			return true;
		if (end == String::npos)
			break;
		begin = end + 1;
	}
	return false;
}

/**
 * @brief Removes the sql extension from an extension list.
 */
String PropEditorSyntax::RemoveSqlExtension(const String& extensions)
{
	String result;
	size_t begin = 0;
	while (begin < extensions.length())
	{
		const size_t end = extensions.find_first_of(L"; ", begin);
		const String extension = strutils::trim_ws(extensions.substr(begin, end - begin));
		if (!extension.empty() && strutils::compare_nocase(extension, _T("sql")) != 0)
		{
			if (!result.empty())
				result += _T(';');
			result += extension;
		}
		if (end == String::npos)
			break;
		begin = end + 1;
	}
	return result;
}

/**
 * @brief Moves the sql extension to the selected SQL dialect.
 */
void PropEditorSyntax::MoveSqlExtension(int dialect)
{
	const int dialects[] = {
		LangServices::LanguageId::SRC_SQL,
		LangServices::LanguageId::SRC_SQL_POSTGRESQL,
		LangServices::LanguageId::SRC_SQL_MYSQL,
	};

	for (const int currentDialect : dialects)
	{
		const int index = currentDialect - 1;
		m_Extension[index] = m_listSyntax.GetItemText(index, 1);
		m_Extension[index] = RemoveSqlExtension(m_Extension[index]);
	}

	const int targetIndex = dialect - 1;
	if (!m_Extension[targetIndex].empty())
		m_Extension[targetIndex] += _T(';');
	m_Extension[targetIndex] += _T("sql");
	WildcardRemoveDuplicatePatterns(m_Extension[targetIndex]);

	for (const int currentDialect : dialects)
		m_listSyntax.SetItemText(currentDialect - 1, 1, m_Extension[currentDialect - 1].c_str());
}

void PropEditorSyntax::OnSqlDialectChanged()
{
	if (!m_init)
		return;

	const int item = m_comboSqlDialect.GetCurSel();
	if (item != CB_ERR)
		MoveSqlExtension(static_cast<int>(m_comboSqlDialect.GetItemData(item)));
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
	m_listSyntax.InsertColumn(1, title.c_str(), LVCFMT_LEFT, pointToPixel(256));

	m_listSyntax.SetReadOnlyColumn(0);
	m_listSyntax.SetEditStyle(1, CSubeditList::EditStyle::WILDCARD_DROP_LIST);


	String fixedPattern[LangServices::LanguageId::SRC_MAX_ENTRY];
	Options::EditorSyntax::GetDefaults(GetOptionsMgr(), fixedPattern);

	for (int i = LangServices::LanguageId::SRC_ABAP; i < LangServices::LanguageId::SRC_MAX_ENTRY; ++i)
	{
		LangServices::TextDefinition* def = LangServices::GetTextType(i);
		
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
	for (int i = LangServices::LanguageId::SRC_ABAP; i < LangServices::LanguageId::SRC_MAX_ENTRY; ++i)
		m_listSyntax.SetItemText(i-1, 1, m_Extension[i-1].c_str());
}
