// SampleDoc.cpp : implementation of the CSampleDoc class
//

#include "stdafx.h"
#include "Sample.h"
#include "ceditcmd.h"
#include "SampleDoc.h"
#include "SampleView.h"
#include "utils/filesup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSampleDoc

IMPLEMENT_DYNCREATE(CSampleDoc, CDocument)

BEGIN_MESSAGE_MAP(CSampleDoc, CDocument)
	//{{AFX_MSG_MAP(CSampleDoc)
	ON_COMMAND(ID_READ_ONLY, OnReadOnly)
	ON_UPDATE_COMMAND_UI(ID_READ_ONLY, OnUpdateReadOnly)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampleDoc construction/destruction

#pragma warning(disable:4355)
CSampleDoc::CSampleDoc()
{
	// TODO: add one-time construction code here

	//	Initialize LOGFONT structure
	memset(&m_lf, 0, sizeof(m_lf));
	m_lf.lfWeight = FW_NORMAL;
	m_lf.lfCharSet = ANSI_CHARSET;
	m_lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_lf.lfQuality = DEFAULT_QUALITY;
	m_lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	m_lf.lfHeight = -20;
	_tcscpy_s(m_lf.lfFaceName, _T("Courier New"));

	m_pSyntaxColors = new SyntaxColors();
	m_pMarkers = new CCrystalTextMarkers();
	m_pMarkers->LoadFromRegistry();
	CCrystalTextView::SetRenderingModeDefault(CCrystalTextView::RENDERING_MODE_DWRITE_NATURAL_SYMMETRIC);
}

CSampleDoc::~CSampleDoc()
{
	delete m_pSyntaxColors;
	delete m_pMarkers;
}

BOOL CSampleDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return false;

	m_xTextBuffer.InitNew();
	return true;
}



/////////////////////////////////////////////////////////////////////////////
// CSampleDoc serialization

void CSampleDoc::Serialize(CArchive& ar)
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
// CSampleDoc diagnostics

#ifdef _DEBUG
void CSampleDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSampleDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSampleDoc commands

void CSampleDoc::DeleteContents() 
{
	CDocument::DeleteContents();

	m_xTextBuffer.FreeAll();
}

BOOL CSampleDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return false;
	
	bool result = m_xTextBuffer.LoadFromFile(lpszPathName);
	if (result)
	{
		CString ext = GetExt(lpszPathName).MakeLower();
		if (ext == "csv" || ext == "tsv")
		{
			m_xTextBuffer.SetTableEditing(true);
			m_xTextBuffer.SetFieldDelimiter(ext == _T("csv") ? ',' : '\t');
			m_xTextBuffer.JoinLinesForTableEditingMode();
		}
		else
		{
			m_xTextBuffer.SetTableEditing(false);
		}
	}
	return result;
}

BOOL CSampleDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	m_xTextBuffer.SaveToFile(lpszPathName);
	return true;	//	Note - we didn't call inherited member!
}

void CSampleDoc::OnReadOnly() 
{
	if (! m_xTextBuffer.GetReadOnly())
	{
		m_xTextBuffer.SetReadOnly(true);
		AfxMessageBox(_T("Document now read-only!"));
	}
	else
		m_xTextBuffer.SetReadOnly(false);
}

void CSampleDoc::OnUpdateReadOnly(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_xTextBuffer.GetReadOnly());
}
