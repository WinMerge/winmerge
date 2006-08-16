// SampleDoc.h : interface of the CSampleDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAMPLEDOC_H__B1B69ED1_9FCE_11D2_8CA4_0080ADB8683C__INCLUDED_)
#define AFX_SAMPLEDOC_H__B1B69ED1_9FCE_11D2_8CA4_0080ADB8683C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CCrystalTextBuffer.h"
#include "SyntaxColors.h"
#include "OptionsMgr.h"

class CSampleDoc : public CDocument
{
protected: // create from serialization only
	CSampleDoc();
	DECLARE_DYNCREATE(CSampleDoc)

// Attributes
public:
	class CSampleTextBuffer : public CCrystalTextBuffer
	{
	private:
		CSampleDoc *m_pOwnerDoc;
	public:
		CSampleTextBuffer(CSampleDoc *pDoc) { m_pOwnerDoc = pDoc; };
		virtual void SetModified(BOOL bModified = TRUE)
			{ m_pOwnerDoc->SetModifiedFlag(bModified); };
	};

	CSampleTextBuffer m_xTextBuffer;
	LOGFONT m_lf;
	CRegOptionsMgr m_options; /**< Options manager */
	SyntaxColors *m_pSyntaxColors;

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

#endif // !defined(AFX_SAMPLEDOC_H__B1B69ED1_9FCE_11D2_8CA4_0080ADB8683C__INCLUDED_)
