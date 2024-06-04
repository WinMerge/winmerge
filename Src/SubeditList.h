#if !defined(AFX_SUBEDITLIST_H__335134F3_37B4_4739_AC9B_4AFB32C37E60__INCLUDED_)
#define AFX_SUBEDITLIST_H__335134F3_37B4_4739_AC9B_4AFB32C37E60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SubeditList.h : header file
//

class CInPlaceEdit : public CEdit
{
// Construction
public:
	CInPlaceEdit(int iItem, int iSubItem, CString sInitText);
 
// Attributes
public:
 
// Operations
public:
 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
 
// Implementation
public:
	virtual ~CInPlaceEdit();
 
	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
 
	DECLARE_MESSAGE_MAP()
private:
	int m_iItem;
	int m_iSubItem;
	CString m_sInitText;
	bool	m_bESC;	 	// To indicate whether ESC key was pressed
};
 
class CInPlaceComboBox : public CComboBox
{
// Construction
public:
	CInPlaceComboBox(int iItem, int iSubItem, CString sInitText, const std::vector<String>& list);
 
// Attributes
public:
 
// Operations
public:
 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceComboBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
 
// Implementation
public:
	virtual ~CInPlaceComboBox();
 
	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceComboBox)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
 
	DECLARE_MESSAGE_MAP()
private:
	int m_iItem;
	int m_iSubItem;
	CString m_sInitText;
	std::vector<String> m_list;
	bool	m_bESC;	 	// To indicate whether ESC key was pressed
};
 
class CSubeditList : public CListCtrl
{
public:
	enum class EditStyle
	{
		EDIT_BOX,
		DROPDOWN_LIST,
		WILDCARD_DROP_LIST,
	};

// Construction
public:
	CSubeditList();

// Attributes
private:
	//low 16 bits reserved for EditStyle
	enum { EDIT_STYLES_ALL = 0xffff, READ_ONLY = (1 << 16), BOOLEAN_VALUE = (1 << 17) };

	std::vector<std::pair<int, int>> m_columnsAttributes;		/**< Stores pairs of <Character limit for each column; bitset of attributes|EditStyle> */
	std::vector<std::vector<String>> m_dropListFixedPattern;	/**< Wildcard drop list fixed pattern for each cell */
	std::vector<std::vector<std::vector<String>>> m_dropList;	/**< dropdown list data for each cell */

// Operations
public:
	void SetReadOnlyColumn(int nCol) { SetColumnAttribute(nCol, 0, READ_ONLY); }
	void SetBooleanValueColumn(int nCol) { SetColumnAttribute(nCol, 0, BOOLEAN_VALUE); }
	void SetItemBooleanValue(int nItem, int nSubItem, bool value);
	bool GetItemBooleanValue(int nItem, int nSubItem) const;
	EditStyle GetEditStyle(int nCol) const;
	void SetEditStyle(int nCol, EditStyle style);
	int GetLimitTextSize(int nCol) const;
	void SetLimitTextSize(int nCol, int nLimitTextSize);
	String GetDropListFixedPattern(int nItem, int nSubItem) const;
	void SetDropListFixedPattern(int nItem, int nSubItem, const String& fixedPattern);
	std::vector<String> GetDropdownList(int nItem, int nSubItem) const;
	void SetDropdownList(int nItem, int nSubItem, const std::vector<String>& list);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSubeditList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSubeditList();

	CInPlaceEdit *EditSubLabel(int nItem, int nCol);
	void EditSubLabelWildcardDropList(int nItem, int nCol);
	void EditSubLabelDropdownList(int nItem, int nCol);
private:
	bool IsValidCol(int nSubItem) const;
	bool IsValidRowCol(int nItem, int nSubItem) const;
	//set parameter only if not equal 0
	void SetColumnAttribute(int nCol, int limit, int attribute);
public:
	// Generated message map functions
//protected:
	//{{AFX_MSG(CSubeditList)
	// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg int HitTestEx(CPoint& point, int* col) const;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBEDITLIST_H__335134F3_37B4_4739_AC9B_4AFB32C37E60__INCLUDED_)
