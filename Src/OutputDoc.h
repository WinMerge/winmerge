// OutputDoc.h : interface of the COutputDoc class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CCrystalTextBuffer.h"

class CCrystalTextMarkers;

class COutputDoc : public CDocument
{
protected: // create from serialization only
	COutputDoc();
	DECLARE_DYNCREATE(COutputDoc)

	// Attributes
public:
	CCrystalTextBuffer m_xTextBuffer;
	std::unique_ptr<CCrystalTextMarkers> m_pMarkers; /**< Marker container */
	int m_nMaxLineCount;

	// Operations
public:
	void AppendLineWithAutoTrim(const String& text);
	void ClearAll();

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(COutputDoc)
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COutputDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(COutputDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
