// OpenDoc.cpp : implementation file
//

#include "stdafx.h"
#include "OpenDoc.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

// COpenDoc

IMPLEMENT_DYNCREATE(COpenDoc, CDocument)

COpenDoc::COpenDoc() :
	m_bRecurse(FALSE)
,	m_dwFlags()
{
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
	m_bRecurse = GetOptionsMgr()->GetBool(OPT_CMP_INCLUDE_SUBDIRS);
	UpdateAllViews(NULL);
}

BEGIN_MESSAGE_MAP(COpenDoc, CDocument)
END_MESSAGE_MAP()
