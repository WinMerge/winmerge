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
, m_nMaxLineCount(10000)
{
	m_pMarkers->SetMarker(_T("warn"), "[WARN]", 0, COLORINDEX_MARKERBKGND1);
	m_pMarkers->SetMarker(_T("error"), "[ERROR]", 0, COLORINDEX_MARKERBKGND2);
	m_xTextBuffer.InitNew();
}

COutputDoc::~COutputDoc()
{
}

BOOL COutputDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return false;

	return true;
}

void COutputDoc::AppendLineWithAutoTrim(const String& text)
{
	CCrystalTextBuffer& buf = m_xTextBuffer;
	int nEndLine, nEndChar;
	POSITION pos = GetFirstViewPosition();
	COutputView* pOutputView = static_cast<COutputView*>(GetNextView(pos));
	const bool isCursorAtLastLine = (pOutputView && pOutputView->GetCursorPos().y + 1 == buf.GetLineCount());
	buf.InsertText(pOutputView, buf.GetLineCount() - 1, 0, text.c_str(), text.length(), nEndLine, nEndChar, 0, false);
	if (buf.GetLineCount() > m_nMaxLineCount)
	{
		const int nDeleteLines = m_nMaxLineCount / 10;
		buf.DeleteText(pOutputView, 0, 0, nDeleteLines, 0, 0, false);
		nEndLine -= nDeleteLines;
	}

	if (isCursorAtLastLine)
	{
		CEPoint pt{ nEndChar, nEndLine };
		pOutputView->SetCursorPos(pt);
		CEPoint ptStart{ 0, (std::max)(0, nEndLine - pOutputView->GetScreenLines()) };
		CEPoint ptEnd{ 0, (std::max)(0, nEndLine) };
		pOutputView->EnsureVisible(ptStart, ptEnd);
	}
}

void COutputDoc::ClearAll()
{
	CCrystalTextBuffer& buf = m_xTextBuffer;
	POSITION pos = GetFirstViewPosition();
	COutputView* pOutputView = static_cast<COutputView*>(GetNextView(pos));
	buf.DeleteText(pOutputView, 0, 0, 
		buf.GetLineCount() - 1, 0, 0, false);
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

