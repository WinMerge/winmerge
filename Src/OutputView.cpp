// OutputView.cpp : implementation of the COutputView class
//

#include "stdafx.h"
#include "OutputView.h"
#include "OutputDoc.h"
#include "editcmd.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "MainFrm.h"
#include "Merge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputView

IMPLEMENT_DYNCREATE(COutputView, CCrystalTextView)

BEGIN_MESSAGE_MAP(COutputView, CCrystalTextView)
	//{{AFX_MSG_MAP(COutputView)
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputView construction/destruction

COutputView::COutputView()
{
	// TODO: add construction code here
	SetParser(&m_xParser);
}

COutputView::~COutputView()
{
}

BOOL COutputView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CCrystalTextView::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// COutputView diagnostics

#ifdef _DEBUG
void COutputView::AssertValid() const
{
	CCrystalTextView::AssertValid();
}

void COutputView::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}

COutputDoc* COutputView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COutputDoc)));
	return static_cast<COutputDoc*>(m_pDocument);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COutputView message handlers

CCrystalTextBuffer* COutputView::LocateTextBuffer()
{
	return &GetDocument()->m_xTextBuffer;
}

void COutputView::OnInitialUpdate()
{
	__super::OnInitialUpdate();
	SetRevisionMarkWidth(0);
	SetSelectionMargin(false);
	SetFont(theApp.m_lfDiff);
	AttachToBuffer();
	SetColorContext(theApp.GetMainSyntaxColors());
}

