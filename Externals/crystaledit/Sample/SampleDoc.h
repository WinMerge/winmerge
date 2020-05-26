// SampleDoc.h : interface of the CSampleDoc class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CCrystalTextBuffer.h"
#include "SyntaxColors.h"
#include "CCrystalTextMarkers.h"

class CSampleDoc : public CDocument
{
protected: // create from serialization only
	CSampleDoc();
	DECLARE_DYNCREATE(CSampleDoc)

// Attributes
public:
	virtual BOOL IsModified() { return m_xTextBuffer.IsModified(); };
	virtual void SetModified(BOOL bModified) { m_xTextBuffer.SetModified(bModified); };
	CCrystalTextBuffer m_xTextBuffer;
	LOGFONT m_lf;
	SyntaxColors *m_pSyntaxColors;
	CCrystalTextMarkers *m_pMarkers;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampleDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSampleDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	template <class Func>
	void ForEachView(Func func)
	{
		for (POSITION pos = GetFirstViewPosition(); pos != nullptr; )
			func(static_cast<CSampleView*>(GetNextView(pos)));
	}

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSampleDoc)
	afx_msg void OnReadOnly();
	afx_msg void OnUpdateReadOnly(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
