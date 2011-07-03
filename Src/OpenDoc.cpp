// OpenDoc.cpp : implementation file
//

#include "stdafx.h"
#include "Merge.h"
#include "OpenDoc.h"
#include "Constants.h"

// COpenDoc

IMPLEMENT_DYNCREATE(COpenDoc, CDocument)

COpenDoc::COpenDoc() :
	m_bRecurse(FALSE)
{
	m_dwFlags[0] = FFILEOPEN_NOMRU;
	m_dwFlags[1] = FFILEOPEN_NOMRU;
	m_dwFlags[2] = FFILEOPEN_NOMRU;
}

BOOL COpenDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

COpenDoc::~COpenDoc()
{
}


BEGIN_MESSAGE_MAP(COpenDoc, CDocument)
END_MESSAGE_MAP()


// COpenDoc diagnostics

#ifdef _DEBUG
void COpenDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void COpenDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

