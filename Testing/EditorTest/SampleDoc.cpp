// SampleDoc.cpp : implementation of the CSampleDoc class
//

#include "stdafx.h"
#include "Sample.h"
#include "ceditcmd.h"

#include "SampleDoc.h"
#include "OptionsDef.h"

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
CSampleDoc::CSampleDoc() : m_xTextBuffer(this)
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
   _tcscpy(m_lf.lfFaceName, _T("Courier New"));

	m_options.InitOption(OPT_CLR_DEFAULT_TEXT_COLORING, true);

	m_pSyntaxColors = new SyntaxColors();
	if (m_pSyntaxColors)
		m_pSyntaxColors->Initialize(&m_options);
}

CSampleDoc::~CSampleDoc()
{
}

BOOL CSampleDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_xTextBuffer.InitNew();
	return TRUE;
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
		return FALSE;
	
	return m_xTextBuffer.LoadFromFile(lpszPathName);
}

BOOL CSampleDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	m_xTextBuffer.SaveToFile(lpszPathName);
	return TRUE;	//	Note - we didn't call inherited member!
}

void CSampleDoc::OnReadOnly() 
{
	if (! m_xTextBuffer.GetReadOnly())
	{
		m_xTextBuffer.SetReadOnly(TRUE);
		AfxMessageBox(_T("Document now read-only!"));
	}
	else
		m_xTextBuffer.SetReadOnly(FALSE);
}

void CSampleDoc::OnUpdateReadOnly(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_xTextBuffer.GetReadOnly());
}
