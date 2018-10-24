// OpenDoc.cpp : implementation file
//

#include "stdafx.h"
#include "OpenDoc.h"
#include "OpenView.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "MergeApp.h"

// COpenDoc

IMPLEMENT_DYNCREATE(COpenDoc, CDocument)

COpenDoc::COpenDoc() :
	m_bRecurse(false)
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
	UpdateAllViews(nullptr);
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void COpenDoc::UpdateResources()
{
	SetTitle(_("Select Files or Folders").c_str());
	POSITION pos = GetFirstViewPosition();
	COpenView *pView = static_cast<COpenView *>(GetNextView(pos));
	pView->UpdateResources();
}

BEGIN_MESSAGE_MAP(COpenDoc, CDocument)
END_MESSAGE_MAP()
