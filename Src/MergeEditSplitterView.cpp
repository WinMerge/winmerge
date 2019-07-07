/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  MergeEditSplitterView.cpp
 *
 * @brief Implementation of the CMergeEditSplitterView class
 */

#include "StdAfx.h"
#include "MergeEditSplitterView.h"
#include "Merge.h"
#include "MergeDoc.h"
#include "MergeEditview.h"
#include "MainFrm.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SWAPPARAMS_IF(c, a, b) (c ? a : b), (c ? b : a)

/////////////////////////////////////////////////////////////////////////////
// CMergeEditSplitterView

IMPLEMENT_DYNCREATE(CMergeEditSplitterView, CView)

CMergeEditSplitterView::CMergeEditSplitterView(): m_bDetailView(false), m_nThisGroup(0)
{
}

CMergeEditSplitterView::~CMergeEditSplitterView()
{
	dynamic_cast<CMergeDoc *>(GetDocument())->RemoveMergeViews(m_nThisGroup);
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

	m_nThisGroup = pDoc->m_nGroups;

	// stash left & right pointers into the mergedoc
	CMergeEditView * pView[3];
	for (int nBuffer = 0; nBuffer < pDoc->m_nBuffers; nBuffer++)
	{
		pView[nBuffer] = static_cast<CMergeEditView *>(m_wndSplitter.GetPane(SWAPPARAMS_IF(bSplitVert, 0, nBuffer)));
		// connect merge views up to display of status info
		pView[nBuffer]->m_nThisPane = nBuffer;
		pView[nBuffer]->m_nThisGroup = m_nThisGroup;
		pView[nBuffer]->m_bDetailView = m_bDetailView;
	}
	pDoc->AddMergeViews(pView);
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
	CMergeDoc * pDoc = dynamic_cast<CMergeDoc *>(GetDocument());

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
