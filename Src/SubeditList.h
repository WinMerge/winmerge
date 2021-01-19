#if !defined(AFX_SUBEDITLIST_H__335134F3_37B4_4739_AC9B_4AFB32C37E60__INCLUDED_)
#define AFX_SUBEDITLIST_H__335134F3_37B4_4739_AC9B_4AFB32C37E60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SubeditList.h : header file
//

#include <set>

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
	BOOL	m_bESC;	 	// To indicate whether ESC key was pressed
};
 
class CSubeditList : public CListCtrl
{
// Construction
public:
	CSubeditList();

// Attributes
public:
	std::set<int> m_binaryValueColumns;

// Operations
public:
	void SetBinaryValueColumn(int nCol) { m_binaryValueColumns.insert(nCol); };

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSubeditList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSubeditList();

	CInPlaceEdit *EditSubLabel(int nItem, int nCol);
	// Generated message map functions
//protected:
	//{{AFX_MSG(CSubeditList)
	// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg int HitTestEx(CPoint& point, int* col) const;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBEDITLIST_H__335134F3_37B4_4739_AC9B_4AFB32C37E60__INCLUDED_)
