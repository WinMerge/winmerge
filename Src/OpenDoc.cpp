// OpenDoc.cpp : implementation file
//

#include "stdafx.h"
#include "OpenDoc.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "LanguageSelect.h"
#include "Merge.h"

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

/**
 * @brief Update any resources necessary after a GUI language change
 */
void COpenDoc::UpdateResources()
{
	SetTitle(_("Select Files or Folders").c_str());
	POSITION pos = GetFirstViewPosition();
	CView *pView = GetNextView(pos);
	theApp.m_pLangDlg->RetranslateDialog(pView->m_hWnd, MAKEINTRESOURCE(IDD_OPEN));
}

BEGIN_MESSAGE_MAP(COpenDoc, CDocument)
END_MESSAGE_MAP()
