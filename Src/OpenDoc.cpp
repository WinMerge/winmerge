// OpenDoc.cpp : implementation file
//

#include "stdafx.h"
#include "OpenDoc.h"
#include "OpenView.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

// COpenDoc

IMPLEMENT_DYNCREATE(COpenDoc, CDocument)

COpenDoc::COpenDoc()
	: m_bRecurse(false)
	, m_dwFlags()
	, m_nWindowType(-1)
	, m_cTableDelimiter(',')
	, m_cTableQuote('\"')
	, m_bTableAllowNewLinesInQuotes(false)
{
	PackingInfo infoUnpacker;
	PrediffingInfo infoPrediffer;
	m_strUnpackerPipeline = GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED) ? infoUnpacker.GetPluginPipeline() : _T("");
	m_strPredifferPipeline = GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED) ? infoPrediffer.GetPluginPipeline() : _T("");
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
