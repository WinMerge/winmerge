/** 
 * @file  DirAdditionalPropertiesDlg.h
 *
 * @brief Declaration file for CDirAdditionalPropertiesDlg
 */
#pragma once

#include <vector>
#include "TrDialogs.h"
#include "UnicodeString.h"

/////////////////////////////////////////////////////////////////////////////
// CDirAdditionalPropertiesDlg dialog

struct Node
{
	String name;
	String canonicalName;
	String displayName;
	bool selected = false;
	HTREEITEM hItem = nullptr;
	std::list<Node> childNodes;
};

/**
 * @brief A Dialog for choosing visible folder compare columns.
 * This class implements a dialog for choosing visible columns in folder
 * compare. Columns can be also re-ordered. There is one listview component
 * which lists all available columns. Every column name has a checkbox with
 * it. If the checkbox is checked, the column is visible.
 *
 * @note: Due to how columns handling code is implemented, hidden columns
 * must be always be last in the list with order number -1.
 * @todo: Allow hidden columns between visible columns.
 */
class CDirAdditionalPropertiesDlg : public CTrDialog
{
// Public types
public:

// Construction
public:
	explicit CDirAdditionalPropertiesDlg(const std::vector<String>& canonicalNames, CWnd* pParent = nullptr);   // standard constructor
	const std::vector<String>& GetSelectedCanonicalNames() const { return m_canonicalNames; }

// Dialog Data
	//{{AFX_DATA(CDirAdditionalPropertiesDlg)
	enum { IDD = IDD_DIRADDITIONALPROPS };
	CTreeCtrl m_treeProps;
	CListCtrl m_listProps;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirAdditionalPropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation methods
protected:
	Node& MakeNode(Node& parentNode, const std::vector<StringView>& path, std::vector<StringView>::iterator it);
	void LoadList();

// Implementation data
private:
	std::vector<String> m_canonicalNames;
	Node m_root;
	// Generated message map functions
	//{{AFX_MSG(CDirAdditionalPropertiesDlg)
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	afx_msg void OnAdd();
	afx_msg void OnDelete(UINT nId);
	afx_msg void OnDblClkTreeView(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDownTreeView(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDownListView(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
