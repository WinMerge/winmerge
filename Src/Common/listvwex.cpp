// ListVwEx.cpp : implementation of the CListViewEx class
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1996 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "ListVwEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListViewEx

IMPLEMENT_DYNCREATE(CListViewEx, CListView)

BEGIN_MESSAGE_MAP(CListViewEx, CListView)
	//{{AFX_MSG_MAP(CListViewEx)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
	ON_MESSAGE(LVM_SETIMAGELIST, OnSetImageList)
	ON_MESSAGE(LVM_SETTEXTCOLOR, OnSetTextColor)
	ON_MESSAGE(LVM_SETTEXTBKCOLOR, OnSetTextBkColor)
	ON_MESSAGE(LVM_SETBKCOLOR, OnSetBkColor)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListViewEx construction/destruction

CListViewEx::CListViewEx()
{
	m_bFullRowSel=FALSE;
	m_bClientWidthSel=TRUE;

	m_cxClient=0;
	m_cxStateImageOffset=0;

	m_clrText=::GetSysColor(COLOR_WINDOWTEXT);
	m_clrTextBk=::GetSysColor(COLOR_WINDOW);
	m_clrBkgnd=::GetSysColor(COLOR_WINDOW);
	m_nTabFlag = DT_EXPANDTABS;
}

CListViewEx::~CListViewEx()
{
}

BOOL CListViewEx::PreCreateWindow(CREATESTRUCT& cs)
{
	// default is report view and full row selection
	cs.style&=~LVS_TYPEMASK;
	cs.style|=LVS_REPORT | LVS_OWNERDRAWFIXED;
	m_bFullRowSel=TRUE;

	return(CListView::PreCreateWindow(cs));
}

BOOL CListViewEx::SetFullRowSel(BOOL bFullRowSel)
{
	// no painting during change
	LockWindowUpdate();

	m_bFullRowSel=bFullRowSel;

	BOOL bRet;

	if(m_bFullRowSel)
		bRet=ModifyStyle(0L,LVS_OWNERDRAWFIXED);
	else
		bRet=ModifyStyle(LVS_OWNERDRAWFIXED,0L);

	// repaint window if we are not changing view type
	if(bRet && (GetStyle() & LVS_TYPEMASK)==LVS_REPORT)
		Invalidate();

	// repaint changes
	UnlockWindowUpdate();

	return(bRet);
}

BOOL CListViewEx::GetFullRowSel()
{
	return(m_bFullRowSel);
}

/////////////////////////////////////////////////////////////////////////////
// CListViewEx drawing

// offsets for first and other columns
#define OFFSET_FIRST	2
#define OFFSET_OTHER	6

void CListViewEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CListCtrl& ListCtrl=GetListCtrl();
	CDC* pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem);
	UINT uiFlags=ILD_TRANSPARENT;
	CImageList* pImageList;
	int nItem=lpDrawItemStruct->itemID;
	BOOL bFocus=(GetFocus()==this);
	COLORREF clrTextSave, clrBkSave;
	COLORREF clrImage=m_clrBkgnd;
	static _TCHAR szBuff[MAX_PATH];
	LPCTSTR pszText;

// get item data

	LV_ITEM lvi;
	lvi.mask=LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.iItem=nItem;
	lvi.iSubItem=0;
	lvi.pszText=szBuff;
	lvi.cchTextMax=sizeof(szBuff);
	lvi.stateMask=0xFFFF;		// get all state flags
	ListCtrl.GetItem(&lvi);

	BOOL bSelected=(bFocus || (GetStyle() & LVS_SHOWSELALWAYS)) && lvi.state & LVIS_SELECTED;
	bSelected=bSelected || (lvi.state & LVIS_DROPHILITED);

// set colors if item is selected

	CRect rcAllLabels;
	ListCtrl.GetItemRect(nItem,rcAllLabels,LVIR_BOUNDS);
	CRect rcLabel;
	ListCtrl.GetItemRect(nItem,rcLabel,LVIR_LABEL);
	rcAllLabels.left=rcLabel.left;
	if(m_bClientWidthSel && rcAllLabels.right<m_cxClient)
		rcAllLabels.right=m_cxClient;

	if(bSelected)
	{
		clrTextSave=pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave=pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));

		pDC->FillRect(rcAllLabels,&CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}
	else
		pDC->FillRect(rcAllLabels,&CBrush(m_clrTextBk));

// set color and mask for the icon

	if(lvi.state & LVIS_CUT)
	{
		clrImage=m_clrBkgnd;
		uiFlags|=ILD_BLEND50;
	}
	else if(bSelected)
	{
		clrImage=::GetSysColor(COLOR_HIGHLIGHT);
		uiFlags|=ILD_BLEND50;
	}

// draw state icon

	UINT nStateImageMask=lvi.state & LVIS_STATEIMAGEMASK;
	if(nStateImageMask)
	{
		int nImage=(nStateImageMask>>12)-1;
		pImageList=ListCtrl.GetImageList(LVSIL_STATE);
		if(pImageList)
			pImageList->Draw(pDC,nImage,CPoint(rcItem.left,rcItem.top),ILD_TRANSPARENT);
	}

// draw normal and overlay icon

	CRect rcIcon;
	ListCtrl.GetItemRect(nItem,rcIcon,LVIR_ICON);

	pImageList=ListCtrl.GetImageList(LVSIL_SMALL);
	if(pImageList)
	{
		UINT nOvlImageMask=lvi.state & LVIS_OVERLAYMASK;
		if(rcItem.left<rcItem.right-1)
			ImageList_DrawEx(pImageList->m_hImageList,lvi.iImage,pDC->m_hDC,rcIcon.left,rcIcon.top,16,16,m_clrBkgnd,clrImage,uiFlags | nOvlImageMask);
	}

// draw item label

	ListCtrl.GetItemRect(nItem,rcItem,LVIR_LABEL);
	rcItem.right-=m_cxStateImageOffset;

	pszText=MakeShortString(pDC,szBuff,rcItem.right-rcItem.left,2*OFFSET_FIRST);

	rcLabel=rcItem;
	rcLabel.left+=OFFSET_FIRST;
	rcLabel.right-=OFFSET_FIRST;

	pDC->DrawText(pszText,-1,rcLabel, m_nTabFlag | DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);

// draw labels for extra columns

	LV_COLUMN lvc;
	lvc.mask=LVCF_FMT | LVCF_WIDTH;

	for(int nColumn=1; ListCtrl.GetColumn(nColumn,&lvc); nColumn++)
	{
		rcItem.left=rcItem.right;
		rcItem.right+=lvc.cx;

		int nRetLen=ListCtrl.GetItemText(nItem,nColumn,szBuff,sizeof(szBuff));
		if(nRetLen==0) continue;

		pszText=MakeShortString(pDC,szBuff,rcItem.right-rcItem.left,2*OFFSET_OTHER);

		UINT nJustify=DT_LEFT;

		if(pszText==szBuff)
		{
			switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
			{
			case LVCFMT_RIGHT:
				nJustify=DT_RIGHT;
				break;
			case LVCFMT_CENTER:
				nJustify=DT_CENTER;
				break;
			default:
				break;
			}
		}

		rcLabel=rcItem;
		rcLabel.left+=OFFSET_OTHER;
		rcLabel.right-=OFFSET_OTHER;

		pDC->DrawText(pszText,-1,rcLabel,nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
	}

// draw focus rectangle if item has focus

	if(lvi.state & LVIS_FOCUSED && bFocus)
		pDC->DrawFocusRect(rcAllLabels);

// set original colors if item was selected

	if(bSelected)
	{
	        pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}

LPCTSTR CListViewEx::MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset)
{
	static const _TCHAR szThreeDots[]=_T("...");

	int nStringLen=lstrlen(lpszLong);

	CSize ext;
	if (m_nTabFlag == DT_EXPANDTABS)
		ext = pDC->GetTabbedTextExtent(lpszLong,nStringLen, 0, NULL);
	else
		ext = pDC->GetTextExtent(lpszLong,nStringLen);
	if(nStringLen==0 || ext.cx+nOffset<=nColumnLen)
		return(lpszLong);

	static _TCHAR szShort[MAX_PATH];

	lstrcpy(szShort,lpszLong);
	int nAddLen=pDC->GetTextExtent(szThreeDots,sizeof(szThreeDots)).cx;

	for(int i=nStringLen-1; i>0; i--)
	{
		szShort[i]=0;
		if (m_nTabFlag == DT_EXPANDTABS)
			ext = pDC->GetTabbedTextExtent(szShort,i, 0, NULL);
		else
			ext = pDC->GetTextExtent(szShort,i);
		if(ext.cx+nOffset+nAddLen<=nColumnLen)
			break;
	}

	lstrcat(szShort,szThreeDots);

	return(szShort);
}

void CListViewEx::RepaintSelectedItems()
{
	CListCtrl& ListCtrl=GetListCtrl();
	CRect rcItem, rcLabel;

// invalidate focused item so it can repaint properly

	int nItem=ListCtrl.GetNextItem(-1,LVNI_FOCUSED);

	if(nItem!=-1)
	{
		ListCtrl.GetItemRect(nItem,rcItem,LVIR_BOUNDS);
		ListCtrl.GetItemRect(nItem,rcLabel,LVIR_LABEL);
		rcItem.left=rcLabel.left;

		InvalidateRect(rcItem,FALSE);
	}

// if selected items should not be preserved, invalidate them

	if(!(GetStyle() & LVS_SHOWSELALWAYS))
	{
		for(nItem=ListCtrl.GetNextItem(-1,LVNI_SELECTED);
			nItem!=-1; nItem=ListCtrl.GetNextItem(nItem,LVNI_SELECTED))
		{
			ListCtrl.GetItemRect(nItem,rcItem,LVIR_BOUNDS);
			ListCtrl.GetItemRect(nItem,rcLabel,LVIR_LABEL);
			rcItem.left=rcLabel.left;

			InvalidateRect(rcItem,FALSE);
		}
	}

// update changes 

	UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CListViewEx diagnostics

#ifdef _DEBUG

void CListViewEx::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);

	dc << "m_bFullRowSel = " << (UINT)m_bFullRowSel;
	dc << "\n";
	dc << "m_cxStateImageOffset = " << m_cxStateImageOffset;
	dc << "\n";
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CListViewEx message handlers

LRESULT CListViewEx::OnSetImageList(WPARAM wParam, LPARAM lParam)
{
	if((int)wParam==LVSIL_STATE)
	{
		int cx, cy;

		if(::ImageList_GetIconSize((HIMAGELIST)lParam,&cx,&cy))
			m_cxStateImageOffset=cx;
		else
			m_cxStateImageOffset=0;
	}

	return(Default());
}

LRESULT CListViewEx::OnSetTextColor(WPARAM wParam, LPARAM lParam)
{
	m_clrText=(COLORREF)lParam;
	return(Default());
}

LRESULT CListViewEx::OnSetTextBkColor(WPARAM wParam, LPARAM lParam)
{
	m_clrTextBk=(COLORREF)lParam;
	return(Default());
}

LRESULT CListViewEx::OnSetBkColor(WPARAM wParam, LPARAM lParam)
{
	m_clrBkgnd=(COLORREF)lParam;
	return(Default());
}

void CListViewEx::OnSize(UINT nType, int cx, int cy) 
{
	m_cxClient=cx;
	CListView::OnSize(nType, cx, cy);
}

void CListViewEx::OnPaint() 
{
	// in full row select mode, we need to extend the clipping region
	// so we can paint a selection all the way to the right
	if(m_bClientWidthSel && (GetStyle() & LVS_TYPEMASK)==LVS_REPORT && GetFullRowSel())
	{
		CRect rcAllLabels;
		GetListCtrl().GetItemRect(0,rcAllLabels,LVIR_BOUNDS);

		if(rcAllLabels.right<m_cxClient)
		{
			// need to call BeginPaint (in CPaintDC c-tor)
			// to get correct clipping rect
			CPaintDC dc(this);

			CRect rcClip;
			dc.GetClipBox(rcClip);

			rcClip.left=min(rcAllLabels.right-1,rcClip.left);
			rcClip.right=m_cxClient;

			InvalidateRect(rcClip,FALSE);
			// EndPaint will be called in CPaintDC d-tor
		}
	}

	CListView::OnPaint();
}

void CListViewEx::OnSetFocus(CWnd* pOldWnd) 
{
	CListView::OnSetFocus(pOldWnd);

	// check if we are getting focus from label edit box
	if(pOldWnd!=NULL && pOldWnd->GetParent()==this)
		return;

	// repaint items that should change appearance
	if(m_bFullRowSel && (GetStyle() & LVS_TYPEMASK)==LVS_REPORT)
		RepaintSelectedItems();
}

void CListViewEx::OnKillFocus(CWnd* pNewWnd) 
{
	CListView::OnKillFocus(pNewWnd);

	// check if we are losing focus to label edit box
	if(pNewWnd!=NULL && pNewWnd->GetParent()==this)
		return;

	// repaint items that should change appearance
	if(m_bFullRowSel && (GetStyle() & LVS_TYPEMASK)==LVS_REPORT)
		RepaintSelectedItems();
}


void CListViewEx::AddItem(int nItem,int nSubItem,LPCTSTR strItem)
{
  LV_ITEM lvItem;
  lvItem.mask = LVIF_TEXT;
  lvItem.iItem = nItem;
  
  lvItem.iSubItem = nSubItem;
  lvItem.pszText = (LPTSTR)strItem;
  
  CListCtrl& lc = GetListCtrl();
  if(lvItem.iSubItem == 0)
	lc.InsertItem(&lvItem);
  else
	lc.SetItem(&lvItem);
}


void CListViewEx::SetImage(int nItem, UINT nImage)
{
	LV_ITEM lvItem;
	lvItem.mask = LVIF_IMAGE;
	lvItem.iItem = nItem;
	lvItem.iSubItem = 0;
	lvItem.iImage = nImage;
	
	GetListCtrl().SetItem(&lvItem);
}

void CListViewEx::ClearSelection()
{
	CListCtrl& ctl = GetListCtrl();
	int sel = -1;
	
	while ((sel=ctl.GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		ctl.SetItemState(sel, 0, LVIS_SELECTED);
	}
}

void CListViewEx::SelectItems(int nFirst, int nLast, BOOL bSelected /*=TRUE*/)
{
	CListCtrl& ctl = GetListCtrl();
	int first = min(nFirst, nLast);
	int last = max(nFirst, nLast);
	UINT state = bSelected? LVIS_SELECTED : 0;

	for (int i=first; i <= last; i++)
		ctl.SetItemState(i, state, LVIS_SELECTED);
}

int CListViewEx::InsertColumn(int nCol, DWORD idstrColumnHeading, int nFormat /*= LVCFMT_LEFT*/, int nWidth /*= -1*/, int nSubItem /*= -1*/)
{
	CString s;
	CListCtrl& ctl = GetListCtrl();
	s.LoadString(idstrColumnHeading);
	return ctl.InsertColumn(nCol, s, nFormat, nWidth, nSubItem);
}
