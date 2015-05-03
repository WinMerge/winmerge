// OpenDoc.cpp : implementation file
//

#include "stdafx.h"
#include "OpenDoc.h"

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


BEGIN_MESSAGE_MAP(COpenDoc, CDocument)
END_MESSAGE_MAP()
