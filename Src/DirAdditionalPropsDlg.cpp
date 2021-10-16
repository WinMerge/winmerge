/** 
 * @file  DirAdditionalPropsDlg.cpp
 *
 * @brief Implementation file for CDirAdditionalPropsDlg
 */

#include "stdafx.h"
#include "DirAdditionalPropsDlg.h"
#include "PropertySystem.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct Node
{
	String name;
	String canonicalName;
	String displayName;
	bool selected;
	std::vector<Node> subtree;
};

Node& makeNode(std::vector<Node>& tree, const std::vector<StringView>& path, std::vector<StringView>::iterator it)
{
	for (auto& node : tree)
	{
		if (node.name == *it)
		{
			++it;
			if (it == path.end())
				return node;
			return makeNode(node.subtree, path, it);
		}
	}
	Node node{};
	node.name = *it;
	tree.push_back(node);
	++it;
	if (it == path.end())
		return tree.back();
	return makeNode(tree.back().subtree, path, it);
}

static std::vector<Node> createTree(const std::vector<String>& canonicalNames)
{
	std::vector<Node> tree;
	for (const auto& canonicalName : canonicalNames)
	{
		auto path = strutils::split(canonicalName, '.');
		auto& node = makeNode(tree, path, path.begin());
		PropertySystem ps({ canonicalName });
		std::vector<String> displayNames;
		ps.GetDisplayNames(displayNames);
		node.canonicalName = canonicalName;
		node.displayName = displayNames[0];
		node.selected = false;
	}
	return tree;
}

void InsertItems(CTreeCtrl& ctl, const std::vector<Node>& tree)
{
	for (const auto& node : tree)
	{
//		ctl.InsertItem(name.c_str());
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDirAdditionalPropsDlg dialog

/**
 * @brief Default dialog constructor.
 * @param [in] pParent Dialog's parent component (window).
 */
CDirAdditionalPropsDlg::CDirAdditionalPropsDlg(const std::vector<String>& canonicalNames, CWnd* pParent /*= nullptr*/)
	: CTrDialog(CDirAdditionalPropsDlg::IDD, pParent)
	, m_canonicalNames(canonicalNames)
{
}

/**
 * @brief Handle dialog data exchange between controls and variables.
 */
void CDirAdditionalPropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDirAdditionalPropsDlg)
	DDX_Control(pDX, IDC_PROPS_TREEVIEW, m_treeProps);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDirAdditionalPropsDlg, CTrDialog)
	//{{AFX_MSG_MAP(CDirAdditionalPropsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirAdditionalPropsDlg message handlers

/**
 * @brief Dialog initialisation. Load column lists.
 */
BOOL CDirAdditionalPropsDlg::OnInitDialog() 
{
	CTrDialog::OnInitDialog();

	PropertySystem ps(PropertySystem::VIEWABLE);
	std::vector<String> canonicalNames =  ps.GetCanonicalNames();
	std::vector<Node> tree = createTree(canonicalNames);
	InsertItems(m_treeProps, tree);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief User clicked ok, so we update m_cols and close.
 */
void CDirAdditionalPropsDlg::OnOK() 
{
	CTrDialog::OnOK();
}

