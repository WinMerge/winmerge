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
// DirView.cpp : implementation file
//

#include "stdafx.h"
#include "Merge.h"
#include "DirView.h"
#include "DirDoc.h"
#include "MainFrm.h"
#include "resource.h"
#include "coretools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirView



IMPLEMENT_DYNCREATE(CDirView, CListViewEx)

CDirView::CDirView()
{
	m_pList=NULL;
}

CDirView::~CDirView()
{
}


BEGIN_MESSAGE_MAP(CDirView, CListViewEx)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(CDirView)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_DIR_COPY_FILE_TO_LEFT, OnDirCopyFileToLeft)
	ON_UPDATE_COMMAND_UI(ID_DIR_COPY_FILE_TO_LEFT, OnUpdateDirCopyFileToLeft)
	ON_COMMAND(ID_DIR_COPY_FILE_TO_RIGHT, OnDirCopyFileToRight)
	ON_UPDATE_COMMAND_UI(ID_DIR_COPY_FILE_TO_RIGHT, OnUpdateDirCopyFileToRight)
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirView drawing

void CDirView::OnDraw(CDC* /*pDC*/)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CDirView diagnostics

#ifdef _DEBUG
void CDirView::AssertValid() const
{
	CListViewEx::AssertValid();
}

void CDirView::Dump(CDumpContext& dc) const
{
	CListViewEx::Dump(dc);
}

CDirDoc* CDirView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDirDoc)));
	return (CDirDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDirView message handlers

void CDirView::OnInitialUpdate() 
{
	CListViewEx::OnInitialUpdate();
	m_sortColumn = -1;	// start up in no sorted order.
	m_pList = &GetListCtrl();
	GetDocument()->m_pView = this;
	

	m_pList->InsertColumn(DV_NAME, _T("Filename"), LVCFMT_LEFT, 150);
	m_pList->InsertColumn(DV_PATH, _T("Directory"), LVCFMT_LEFT, 200);
	m_pList->InsertColumn(DV_STATUS, _T("Comparison result"), LVCFMT_LEFT, 250);
	m_pList->InsertColumn(DV_LTIME, _T("Left Time"), LVCFMT_LEFT, 150);
	m_pList->InsertColumn(DV_RTIME, _T("Right Time"), LVCFMT_LEFT, 150);

	CBitmap bm;
	VERIFY (m_imageList.Create (16, 16, ILC_MASK, 0, 1));
	VERIFY (bm.LoadBitmap (IDB_LFILE));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_RFILE));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_NOTEQUAL));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_EQUAL));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_UNKNOWN));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_BINARY));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_LFOLDER));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_RFOLDER));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	m_pList->SetImageList (&m_imageList, LVSIL_SMALL);
	UpdateResources();
}

void CDirView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int sel = m_pList->GetNextItem(-1, LVNI_SELECTED);
	if (sel != -1)
	{
		CDirDoc *pd = GetDocument();
		POSITION pos = reinterpret_cast<POSITION>(m_pList->GetItemData(sel));
		DIFFITEM di = pd->m_pCtxt->m_dirlist.GetAt(pos);
		switch(di.code)
		{
		case FILE_DIFF:
			{
				CString left, right;
				if (GetSelectedFileNames(left, right))
					mf->ShowMergeDoc(left, right);
			}
			break;
		case FILE_SAME:
			{
				CString s;
				VERIFY(s.LoadString(IDS_FILESSAME));
				AfxMessageBox(s, MB_ICONINFORMATION);
			}
			break;
		case FILE_LDIRUNIQUE:
		case FILE_RDIRUNIQUE:
			{
				CString s;
				VERIFY(s.LoadString(IDS_FILEISDIR));
				AfxMessageBox(s, MB_ICONINFORMATION);
			}
			break;
		case FILE_LUNIQUE:
		case FILE_RUNIQUE:
			{
				CString s;
				VERIFY(s.LoadString(IDS_FILEUNIQUE));
				AfxMessageBox(s, MB_ICONINFORMATION);
			}
			break;
		case FILE_BINDIFF:
			{
				CString s;
				VERIFY(s.LoadString(IDS_FILEBINARY));
				AfxMessageBox(s, MB_ICONSTOP);
			}
			break;
		default:
			{
				CString s;
				VERIFY(s.LoadString(IDS_FILEERROR));
				AfxMessageBox(s, MB_ICONSTOP);
			}
			break;
		}
	}
	CListViewEx::OnLButtonDblClk(nFlags, point);
}

void CDirView::OnContextMenu(CWnd*, CPoint point)
{

	// CG: This block was added by the Pop-up Menu component
	{
		if (point.x == -1 && point.y == -1){
			//keystroke invocation
			CRect rect;
			GetClientRect(rect);
			ClientToScreen(rect);

			point = rect.TopLeft();
			point.Offset(5, 5);
		}

		CMenu menu;
		VERIFY(menu.LoadMenu(IDR_POPUP_DIRVIEW));

		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		CWnd* pWndPopupOwner = this;

		// set the menu items with the proper directory names
		CString s, sl, sr;
		GetSelectedDirNames(sl, sr);
		AfxFormatString1(s, IDS_COPY2DIR_FMT, sl);
		pPopup->ModifyMenu(ID_DIR_COPY_FILE_TO_LEFT, MF_BYCOMMAND|MF_STRING, ID_DIR_COPY_FILE_TO_LEFT, s);
		AfxFormatString1(s, IDS_COPY2DIR_FMT, sr);
		pPopup->ModifyMenu(ID_DIR_COPY_FILE_TO_RIGHT, MF_BYCOMMAND|MF_STRING, ID_DIR_COPY_FILE_TO_RIGHT, s);

		while (pWndPopupOwner->GetStyle() & WS_CHILD)
			pWndPopupOwner = pWndPopupOwner->GetParent();

		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
			pWndPopupOwner);
	}
}

void CDirView::OnDirCopyFileToLeft() 
{
	int sel = m_pList->GetNextItem(-1, LVNI_SELECTED);
	CString s, sl, sr, slFile, srFile;

	if (sel != -1
		&& GetSelectedDirNames(sl, sr)
		&& GetSelectedFileNames(slFile, srFile))
	{
		CDirDoc *pd = GetDocument();
		CString s;
		POSITION pos = reinterpret_cast<POSITION>(m_pList->GetItemData(sel));
		DIFFITEM di = pd->m_pCtxt->m_dirlist.GetAt(pos);
		switch(di.code)
		{
		case FILE_LDIRUNIQUE:
		case FILE_RDIRUNIQUE:
		case FILE_LUNIQUE:
		case FILE_SAME:
			//pCmdUI->Enable(FALSE);
			break;
		case FILE_RUNIQUE:
		case FILE_DIFF:
		case FILE_BINDIFF:
			AfxFormatString1(s, IDS_CONFIRM_COPY2DIR, sl);
			if (AfxMessageBox(s, MB_YESNO|MB_ICONQUESTION)==IDYES)
			{		
				if (mf->SyncFiles(srFile, slFile))
				{
					mf->UpdateCurrentFileStatus(FILE_SAME);
				}
			}

			break;
		}
	}
}

void CDirView::OnUpdateDirCopyFileToLeft(CCmdUI* pCmdUI) 
{
	int sel = m_pList->GetNextItem(-1, LVNI_SELECTED);
	if (sel != -1)
	{
		CDirDoc *pd = GetDocument();
		POSITION pos = reinterpret_cast<POSITION>(m_pList->GetItemData(sel));
		DIFFITEM di = pd->m_pCtxt->m_dirlist.GetAt(pos);
		switch(di.code)
		{
		case FILE_SAME:
		case FILE_LUNIQUE:
		case FILE_LDIRUNIQUE:
		case FILE_RDIRUNIQUE:
			pCmdUI->Enable(FALSE);
			break;
		case FILE_RUNIQUE:
		case FILE_DIFF:
		case FILE_BINDIFF:
			pCmdUI->Enable(TRUE);
			break;
		}
	}
}

void CDirView::OnDirCopyFileToRight() 
{
	int sel = m_pList->GetNextItem(-1, LVNI_SELECTED);
	CString s, sl, sr, slFile, srFile;

	if (sel != -1
		&& GetSelectedDirNames(sl, sr)
		&& GetSelectedFileNames(slFile, srFile))
	{
		CDirDoc *pd = GetDocument();
		CString s;
		POSITION pos = reinterpret_cast<POSITION>(m_pList->GetItemData(sel));
		DIFFITEM di = pd->m_pCtxt->m_dirlist.GetAt(pos);
		switch(di.code)
		{
		case FILE_RUNIQUE:
		case FILE_LDIRUNIQUE:
		case FILE_RDIRUNIQUE:
		case FILE_SAME:
			//pCmdUI->Enable(FALSE);
			break;
		case FILE_LUNIQUE:
		case FILE_DIFF:
		case FILE_BINDIFF:
			AfxFormatString1(s, IDS_CONFIRM_COPY2DIR, sr);
			if (AfxMessageBox(s, MB_YESNO|MB_ICONQUESTION)==IDYES)
			{		
				CString left, right;
				if (mf->SyncFiles(slFile, srFile))
				{
					mf->UpdateCurrentFileStatus(FILE_SAME);
				}
			}

			break;
		}
	}
}

void CDirView::OnUpdateDirCopyFileToRight(CCmdUI* pCmdUI) 
{
	int sel = m_pList->GetNextItem(-1, LVNI_SELECTED);
	if (sel != -1)
	{
		CDirDoc *pd = GetDocument();
		POSITION pos = reinterpret_cast<POSITION>(m_pList->GetItemData(sel));
		DIFFITEM di = pd->m_pCtxt->m_dirlist.GetAt(pos);
		switch(di.code)
		{
		case FILE_RUNIQUE:
		case FILE_LDIRUNIQUE:
		case FILE_RDIRUNIQUE:
		case FILE_SAME:
			pCmdUI->Enable(FALSE);
			break;
		case FILE_LUNIQUE:
		case FILE_DIFF:
		case FILE_BINDIFF:
			pCmdUI->Enable(TRUE);
			break;
		}
	}
}

BOOL CDirView::GetSelectedFileNames(CString& strLeft, CString& strRight)
{
	int sel = m_pList->GetNextItem(-1, LVNI_SELECTED);
	if (sel != -1)
	{
		CDirDoc *pd = GetDocument();
		CString name, pathex;
		name = m_pList->GetItemText(sel, DV_NAME);
		pathex = m_pList->GetItemText(sel, DV_PATH);
		if (pathex.Left(2) == _T(".\\") || pathex.Left(2) == _T("./"))
		{
			strLeft.Format(_T("%s\\%s\\%s"), pd->m_pCtxt->m_strLeft, pathex.Right(pathex.GetLength()-2), name);
			strRight.Format(_T("%s\\%s\\%s"), pd->m_pCtxt->m_strRight, pathex.Right(pathex.GetLength()-2), name);
		}
		else
		{
			strLeft.Format(_T("%s\\%s"), pd->m_pCtxt->m_strLeft, name);
			strRight.Format(_T("%s\\%s"), pd->m_pCtxt->m_strRight, name);
		}
		return TRUE;
	}
	return FALSE;
}


void CDirView::UpdateResources()
{
	CString s;
	LV_COLUMN lvc;
	lvc.mask = LVCF_TEXT;

	VERIFY(s.LoadString(IDS_FILENAME_HEADER));
	lvc.pszText = (LPTSTR)((LPCTSTR)s);
	m_pList->SetColumn(DV_NAME, &lvc);
	VERIFY(s.LoadString(IDS_DIR_HEADER));
	lvc.pszText = (LPTSTR)((LPCTSTR)s);
	m_pList->SetColumn(DV_PATH, &lvc);
	VERIFY(s.LoadString(IDS_RESULT_HEADER));
	lvc.pszText = (LPTSTR)((LPCTSTR)s);
	m_pList->SetColumn(DV_STATUS, &lvc);
	VERIFY(s.LoadString(IDS_LTIME_HEADER));
	lvc.pszText = (LPTSTR)((LPCTSTR)s);
	m_pList->SetColumn(DV_LTIME, &lvc);
	VERIFY(s.LoadString(IDS_RTIME_HEADER));
	lvc.pszText = (LPTSTR)((LPCTSTR)s);
	m_pList->SetColumn(DV_RTIME, &lvc);
}

BOOL CDirView::GetSelectedDirNames(CString& strLeft, CString& strRight)
{
	BOOL bResult = GetSelectedFileNames(strLeft, strRight);

	if (bResult)
	{
		TCHAR path[MAX_PATH];
		split_filename(strLeft, path, NULL, NULL);
		strLeft = path;

		split_filename(strRight, path, NULL, NULL);
		strRight = path;
	}
	return bResult;
}

int CALLBACK CDirView::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	// initialize structures to obtain required information
	CDirView* pView = reinterpret_cast<CDirView*>(lParamSort);
	DIFFITEM lDi = pView->GetDocument()->m_pCtxt->m_dirlist.GetAt( reinterpret_cast<POSITION>(lParam1) );
	DIFFITEM rDi = pView->GetDocument()->m_pCtxt->m_dirlist.GetAt( reinterpret_cast<POSITION>(lParam2) );

	// compare 'left' and 'right' parameters as appropriate
	int retVal = 0;		// initialize for default case
	switch (pView->m_sortColumn)
	{
	case DV_NAME: // File name.
		retVal = _tcscmp(lDi.filename, rDi.filename);
		break;
	case DV_PATH: // File Path.
		retVal =  _tcscmp(lDi.lpath, rDi.lpath);
		break;
	case DV_STATUS: // Diff Status.
		retVal = rDi.code-lDi.code;
		break;
	case DV_LTIME: // Diff Status.
		retVal = rDi.ltime-lDi.ltime;
		break;
	case DV_RTIME: // Diff Status.
		retVal = lDi.rtime-rDi.rtime;
		break;
	}
	// return compare result, considering sort direction
	return (pView->m_bSortAscending)?retVal:-retVal;
}

void CDirView::OnColumnClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// set sort parameters and handle ascending/descending
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*) pNMHDR;
	if(pNMListView->iSubItem==m_sortColumn)
	{
		m_bSortAscending = !m_bSortAscending;
	}
	else
	{
		m_bSortAscending = true;
		m_sortColumn = pNMListView->iSubItem;
	}
	//sort using static CompareFunc comparison function
	GetListCtrl ().SortItems (CompareFunc, reinterpret_cast<DWORD>(this));//pNMListView->iSubItem);
	*pResult = 0;
}
