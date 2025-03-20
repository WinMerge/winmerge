// OutputDoc.cpp : implementation of the COutputDoc class
//

#include "stdafx.h"
#include "OutputDoc.h"
#include "OutputView.h"
#include "utils/filesup.h"
#include "ccrystaltextmarkers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputDoc

IMPLEMENT_DYNCREATE(COutputDoc, CDocument)

BEGIN_MESSAGE_MAP(COutputDoc, CDocument)
	//{{AFX_MSG_MAP(COutputDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputDoc construction/destruction

#pragma warning(disable:4355)
COutputDoc::COutputDoc()
: m_pMarkers(new CCrystalTextMarkers())
{
	m_pMarkers->SetMarker(_T("error"), "[ERROR]", 0, COLORINDEX_HIGHLIGHTBKGND1);
	m_pMarkers->SetMarker(_T("warn"), "[WARN]", 0, COLORINDEX_HIGHLIGHTBKGND2);
}

COutputDoc::~COutputDoc()
{
}

BOOL COutputDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return false;

	m_xTextBuffer.InitNew();
	return true;
}



/////////////////////////////////////////////////////////////////////////////
// COutputDoc serialization

void COutputDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// COutputDoc diagnostics

#ifdef _DEBUG
void COutputDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void COutputDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COutputDoc commands

void COutputDoc::DeleteContents()
{
	CDocument::DeleteContents();

	m_xTextBuffer.FreeAll();
}

