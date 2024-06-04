/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  MergeEditSplitterView.cpp
 *
 * @brief Implementation of the CMergeEditSplitterView class
 */

#include "StdAfx.h"
#include "MergeEditSplitterView.h"
#include "MergeDoc.h"
#include "MergeEditview.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SWAPPARAMS_IF(c, a, b) (c ? a : b), (c ? b : a)

/////////////////////////////////////////////////////////////////////////////
// CMergeEditSplitterView

IMPLEMENT_DYNCREATE(CMergeEditSplitterView, CView)

CMergeEditSplitterView::CMergeEditSplitterView(): m_bDetailView(false)
{
}

CMergeEditSplitterView::~CMergeEditSplitterView()
{
	dynamic_cast<CMergeDoc *>(GetDocument())->RemoveMergeViews(this);
}


BEGIN_MESSAGE_MAP(CMergeEditSplitterView, CView)
	//{{AFX_MSG_MAP(CMergeEditSplitterView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMergeEditSplitterView diagnostics

/////////////////////////////////////////////////////////////////////////////
// CMergeEditSplitterView message handlers

BOOL CMergeEditSplitterView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	bool bSplitVert = !GetOptionsMgr()->GetBool(OPT_SPLIT_HORIZONTALLY);
	if (m_bDetailView)
		bSplitVert = !bSplitVert;

	CMergeDoc * pDoc = dynamic_cast<CMergeDoc *>(GetDocument());
	if (pDoc == nullptr)
		return FALSE;

	// create a splitter with 1 row, 2 columns
	if (!m_wndSplitter.CreateStatic(this, SWAPPARAMS_IF(bSplitVert, 1, pDoc->m_nBuffers),
		WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL) )
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

	for (int nBuffer = 0; nBuffer < pDoc->m_nBuffers; nBuffer++)
	{
		if (!m_wndSplitter.CreateView(SWAPPARAMS_IF(bSplitVert, 0, nBuffer),
			RUNTIME_CLASS(CMergeEditView), CSize(-1, 200), pContext))
		{
			TRACE1("Failed to create pane%d\n", nBuffer);
			return FALSE;
		}
	}

	m_wndSplitter.ResizablePanes(true);
	m_wndSplitter.AutoResizePanes(GetOptionsMgr()->GetBool(OPT_RESIZE_PANES));

	// stash left & right pointers into the mergedoc
	CMergeEditView * pView[3];
	for (int nBuffer = 0; nBuffer < pDoc->m_nBuffers; nBuffer++)
	{
		pView[nBuffer] = static_cast<CMergeEditView *>(m_wndSplitter.GetPane(SWAPPARAMS_IF(bSplitVert, 0, nBuffer)));
		// connect merge views up to display of status info
		pView[nBuffer]->m_nThisPane = nBuffer;
		pView[nBuffer]->m_nThisGroup = pDoc->m_nGroups;
		pView[nBuffer]->m_bDetailView = m_bDetailView;
	}
	pDoc->AddMergeViews(this, pView);
	if (!m_bDetailView && pDoc->m_nGroups > 1)
	{
		for (int nBuffer = 0; nBuffer < pDoc->m_nBuffers; nBuffer++)
		{
			CMergeEditView *pView2 = pDoc->GetView(pDoc->m_nGroups - 1, nBuffer);
			pView2->SendMessage(WM_INITIALUPDATE);
			pView2->CopyProperties(pDoc->GetView(0, nBuffer));
			pView2->SetStatusInterface(pDoc->GetView(0, nBuffer)->m_piMergeEditStatus);
		}
		m_wndSplitter.RecalcLayout();
	}
	return TRUE;
}

void CMergeEditSplitterView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
}

void CMergeEditSplitterView::OnDraw(CDC* pDC)
{
}

BOOL CMergeEditSplitterView::OnEraseBkgnd (CDC * pdc)
{
  UNREFERENCED_PARAMETER(pdc);
  return true;
}

void CMergeEditSplitterView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	if (m_wndSplitter.m_hWnd)
		m_wndSplitter.MoveWindow(0, 0, cx, cy);
}
