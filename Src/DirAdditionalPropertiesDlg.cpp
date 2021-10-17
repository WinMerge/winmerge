/** 
 * @file  DirAdditionalPropertiesDlg.cpp
 *
 * @brief Implementation file for CDirAdditionalPropertiesDlg
 */

#include "stdafx.h"
#include "DirAdditionalPropertiesDlg.h"
#include "PropertySystem.h"
#include "unicoder.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirAdditionalPropertiesDlg dialog

/**
 * @brief Default dialog constructor.
 * @param [in] pParent Dialog's parent component (window).
 */
CDirAdditionalPropertiesDlg::CDirAdditionalPropertiesDlg(const std::vector<String>& canonicalNames, CWnd* pParent /*= nullptr*/)
	: CTrDialog(CDirAdditionalPropertiesDlg::IDD, pParent)
	, m_canonicalNames(canonicalNames)
	, m_root{}
{
}

/**
 * @brief Handle dialog data exchange between controls and variables.
 */
void CDirAdditionalPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDirAdditionalPropertiesDlg)
	DDX_Control(pDX, IDC_PROPS_TREEVIEW, m_treeProps);
	DDX_Control(pDX, IDC_PROPS_LISTVIEW, m_listProps);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDirAdditionalPropertiesDlg, CTrDialog)
	//{{AFX_MSG_MAP(CDirAdditionalPropertiesDlg)
	ON_BN_CLICKED(IDC_PROPS_ADD, OnAdd)
	ON_BN_CLICKED(IDC_PROPS_DEL, OnDelete)
	ON_NOTIFY(NM_DBLCLK, IDC_PROPS_TREEVIEW, OnDblClk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

Node& CDirAdditionalPropertiesDlg::MakeNode(Node& parentNode, const std::vector<StringView>& path, std::vector<StringView>::iterator it)
{
	for (auto& node : parentNode.childNodes)
	{
		if (node.name == *it)
		{
			++it;
			if (it != path.end())
				return MakeNode(node, path, it);
			return node;
		}
	}
	Node& node = parentNode.childNodes.emplace_back();
	node.selected = false;
	node.name = *it;
	++it;
	if (it != path.end())
	{
		node.hItem = m_treeProps.InsertItem(tr(ucr::toUTF8(node.name.c_str())).c_str(), parentNode.hItem);
		return MakeNode(node, path, it);
	}
	node.canonicalName = strutils::join(path.begin(), path.end(), _T("."));
	PropertySystem ps({ node.canonicalName });
	std::vector<String> displayNames;
	ps.GetDisplayNames(displayNames);
	node.displayName = displayNames[0];
	node.hItem = m_treeProps.InsertItem(node.displayName.c_str(), parentNode.hItem);
	m_treeProps.SetItemData(node.hItem, reinterpret_cast<DWORD_PTR>(&node));
	return node;
}

void CDirAdditionalPropertiesDlg::LoadList()
{
	m_root.hItem = TVI_ROOT;
	PropertySystem ps(PropertySystem::VIEWABLE);
	for (const auto& canonicalName : ps.GetCanonicalNames())
	{
		PropertySystem ps2({ canonicalName });
		std::vector<String> displayNames;
		ps2.GetDisplayNames(displayNames);
		if (!displayNames[0].empty())
		{
			auto path = strutils::split(canonicalName, '.');
			Node& node = MakeNode(m_root, path, path.begin());
			if (std::count(m_canonicalNames.begin(), m_canonicalNames.end(), canonicalName) > 0)
			{
				for (HTREEITEM hItem = m_treeProps.GetParentItem(node.hItem); hItem; hItem = m_treeProps.GetParentItem(hItem))
					m_treeProps.Expand(hItem, TVE_EXPAND);
				node.selected = true;
				m_treeProps.SetItemState(node.hItem, TVIS_BOLD, TVIS_BOLD);
				m_listProps.InsertItem(LVIF_TEXT|LVIF_PARAM,
					m_listProps.GetItemCount(), node.displayName.c_str(),
					0, 0, 0, reinterpret_cast<LPARAM>(&node));
			}
		}
	}
	if (!m_root.childNodes.empty())
		m_treeProps.SelectItem(m_root.childNodes.front().hItem);
}

/////////////////////////////////////////////////////////////////////////////
// CDirAdditionalPropertiesDlg message handlers

BOOL CDirAdditionalPropertiesDlg::OnInitDialog()
{
	CTrDialog::OnInitDialog();

	CRect rc;
	m_listProps.GetClientRect(&rc);
	m_listProps.InsertColumn(0, _T(""), LVCFMT_LEFT, rc.Width());
	LoadList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDirAdditionalPropertiesDlg::OnAdd() 
{
	HTREEITEM hItem = m_treeProps.GetSelectedItem();
	if (hItem)
	{
		Node* pNode = reinterpret_cast<Node*>(m_treeProps.GetItemData(hItem));
		if (pNode && !pNode->selected)
		{
			pNode->selected = true;
			m_treeProps.SetItemState(pNode->hItem, TVIS_BOLD, TVIS_BOLD);
			m_listProps.InsertItem(LVIF_TEXT|LVIF_PARAM,
				m_listProps.GetItemCount(), pNode->displayName.c_str(),
				0, 0, 0, reinterpret_cast<LPARAM>(pNode));
		}
	}
}

void CDirAdditionalPropertiesDlg::OnDelete() 
{
	for (int i = m_listProps.GetItemCount() - 1; i >= 0; --i)
	{
		if (m_listProps.GetItemState(i, LVIS_SELECTED) != 0)
		{
			Node* pNode = reinterpret_cast<Node*>(m_listProps.GetItemData(i));
			if (pNode)
			{
				pNode->selected = false;
				m_treeProps.SetItemState(pNode->hItem, 0, TVIS_BOLD);
				m_listProps.DeleteItem(i);
			}
		}
	}
}

void CDirAdditionalPropertiesDlg::OnDblClk(NMHDR* pNMHDR, LRESULT* pResult)
{
	HTREEITEM hItem = m_treeProps.GetSelectedItem();
	if (hItem)
	{
		Node* pNode = reinterpret_cast<Node*>(m_treeProps.GetItemData(hItem));
		if (pNode)
		{
			if (pNode->selected)
			{
				for (int i = 0; i < m_listProps.GetItemCount(); ++i)
				{
					bool selected = (pNode == reinterpret_cast<Node*>(m_listProps.GetItemData(i)));
					m_listProps.SetItemState(i, selected ? LVIS_SELECTED : 0, LVIS_SELECTED);
				}
				OnDelete();
			}
			else
			{
				OnAdd();
			}
		}
	}
}

void CDirAdditionalPropertiesDlg::OnOK() 
{
	CTrDialog::OnOK();
	m_canonicalNames.clear();
	for (int i = 0; i < m_listProps.GetItemCount(); ++i)
	{
		Node* pNode = reinterpret_cast<Node*>(m_listProps.GetItemData(i));
		if (pNode)
			m_canonicalNames.push_back(pNode->canonicalName);
	}
}

