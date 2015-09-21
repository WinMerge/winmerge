// OpenDoc.cpp : implementation file
//

#include "stdafx.h"
#include "OpenDoc.h"
#include "Merge.h"

// COpenDoc

IMPLEMENT_DYNCREATE(COpenDoc, CDocument)

COpenDoc::COpenDoc() :
	m_bRecurse(FALSE)
{
	memset(m_dwFlags, 0, sizeof(m_dwFlags));
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

void COpenDoc::RefreshOptions()
{
	m_bRecurse = (theApp.GetProfileInt(_T("Settings"), _T("Recurse"), 0) == 1);
	UpdateAllViews(NULL);
}

BEGIN_MESSAGE_MAP(COpenDoc, CDocument)
END_MESSAGE_MAP()
