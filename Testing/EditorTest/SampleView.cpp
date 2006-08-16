// SampleView.cpp : implementation of the CSampleView class
//

#include "stdafx.h"
#include "Sample.h"

#include "SampleDoc.h"
#include "SampleView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSampleView

IMPLEMENT_DYNCREATE(CSampleView, CCrystalEditView)

BEGIN_MESSAGE_MAP(CSampleView, CCrystalEditView)
	//{{AFX_MSG_MAP(CSampleView)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CCrystalEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CCrystalEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CCrystalEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampleView construction/destruction

CSampleView::CSampleView()
{
	// TODO: add construction code here
	SetParser(&m_xParser);
}

CSampleView::~CSampleView()
{
}

BOOL CSampleView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CCrystalEditView::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// CSampleView diagnostics

#ifdef _DEBUG
void CSampleView::AssertValid() const
{
	CCrystalEditView::AssertValid();
}

void CSampleView::Dump(CDumpContext& dc) const
{
	CCrystalEditViewEx::Dump(dc);
}

CSampleDoc* CSampleView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSampleDoc)));
	return (CSampleDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSampleView message handlers

CCrystalTextBuffer *CSampleView::LocateTextBuffer()
{
	return &GetDocument()->m_xTextBuffer;
}

void CSampleView::OnInitialUpdate() 
{
	CCrystalEditViewEx::OnInitialUpdate();

	SetFont(GetDocument()->m_lf);
	SetColorContext(GetDocument()->m_pSyntaxColors);
	SetWordWrapping(TRUE);
}

void CSampleView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	AfxMessageBox(_T("Build your own context menu!"));
}
