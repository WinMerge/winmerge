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
	ON_COMMAND(ID_R2L, OnDirCopyFileToLeft)
	ON_UPDATE_COMMAND_UI(ID_R2L, OnUpdateDirCopyFileToLeft)
	ON_COMMAND(ID_DIR_COPY_FILE_TO_LEFT, OnCtxtDirCopyFileToLeft)
	ON_UPDATE_COMMAND_UI(ID_DIR_COPY_FILE_TO_LEFT, OnUpdateCtxtDirCopyFileToLeft)
	ON_COMMAND(ID_L2R, OnDirCopyFileToRight)
	ON_UPDATE_COMMAND_UI(ID_L2R, OnUpdateDirCopyFileToRight)
	ON_COMMAND(ID_DIR_COPY_FILE_TO_RIGHT, OnCtxtDirCopyFileToRight)
	ON_UPDATE_COMMAND_UI(ID_DIR_COPY_FILE_TO_RIGHT, OnUpdateCtxtDirCopyFileToRight)
	ON_COMMAND(ID_DIR_DEL_LEFT, OnCtxtDirDelLeft)
	ON_UPDATE_COMMAND_UI(ID_DIR_DEL_LEFT, OnUpdateCtxtDirDelLeft)
	ON_COMMAND(ID_DIR_DEL_RIGHT, OnCtxtDirDelRight)
	ON_UPDATE_COMMAND_UI(ID_DIR_DEL_RIGHT, OnUpdateCtxtDirDelRight)
	ON_WM_DESTROY()
	ON_WM_CHAR()
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

    // Replace standard header with sort header
    if (HWND hWnd = ListView_GetHeader(m_pList->m_hWnd))
            m_ctlSortHeader.SubclassWindow(hWnd);
        
	
	int w;
	CString sKey;
	CString sFmt(_T("WDirHdr%d")), sSect(_T("DirView"));

	sKey.Format(sFmt, DV_NAME);
	w = max(10, theApp.GetProfileInt(sSect, sKey, 150));
	m_pList->InsertColumn(DV_NAME, _T("Filename"), LVCFMT_LEFT, w);

	sKey.Format(sFmt, DV_PATH);
	w = max(10, theApp.GetProfileInt(sSect, sKey, 200));
	m_pList->InsertColumn(DV_PATH, _T("Directory"), LVCFMT_LEFT, w);

	sKey.Format(sFmt, DV_STATUS);
	w = max(10, theApp.GetProfileInt(sSect, sKey, 250));
	m_pList->InsertColumn(DV_STATUS, _T("Comparison result"), LVCFMT_LEFT, w);

	sKey.Format(sFmt, DV_LTIME);
	w = max(10, theApp.GetProfileInt(sSect, sKey, 150));
	m_pList->InsertColumn(DV_LTIME, _T("Left Time"), LVCFMT_LEFT, w);

	sKey.Format(sFmt, DV_RTIME);
	w = max(10, theApp.GetProfileInt(sSect, sKey, 150));
	m_pList->InsertColumn(DV_RTIME, _T("Right Time"), LVCFMT_LEFT, w);

    // BSP - Create a column for the extension values
	sKey.Format(sFmt, DV_EXT);
	w = max(10, theApp.GetProfileInt(sSect, sKey, 150));
	m_pList->InsertColumn(DV_EXT, _T("Extension"), LVCFMT_LEFT, w);

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

	//m_ctlSortHeader.SetSortImage(m_sortColumn, m_bSortAscending);
}

void CDirView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OpenSelection();
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

		
		ModifyPopup(pPopup, IDS_COPY2DIR_LEFT_FMT, ID_DIR_COPY_FILE_TO_LEFT, sl);
		ModifyPopup(pPopup, IDS_COPY2DIR_RIGHT_FMT, ID_DIR_COPY_FILE_TO_RIGHT, sr);
		ModifyPopup(pPopup, IDS_DEL_LEFT_FMT, ID_DIR_DEL_LEFT, sl);
		ModifyPopup(pPopup, IDS_DEL_RIGHT_FMT, ID_DIR_DEL_RIGHT, sr);
		// 

		while (pWndPopupOwner->GetStyle() & WS_CHILD)
			pWndPopupOwner = pWndPopupOwner->GetParent();

		// invoke context menu
		// this will invoke all the OnUpdate methods to enable/disable the individual items
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
			pWndPopupOwner);
	}
}

// Change menu item by using string resource
// (Question: Why don't we just remove it from the menu resource entirely & do an Add here ?)
void CDirView::ModifyPopup(CMenu * pPopup, int nStringResource, int nMenuId, LPCTSTR szPath)
{
	CString s;
	AfxFormatString1(s, nStringResource, szPath);
	pPopup->ModifyMenu(nMenuId, MF_BYCOMMAND|MF_STRING, nMenuId, s);
}

// given index in list control, get position & DIFFITEM reference to its data
const DIFFITEM& CDirView::GetDiffItem(int sel, POSITION & pos)
{
	pos = reinterpret_cast<POSITION>(m_pList->GetItemData(sel));
	const DIFFITEM& di = GetDocument()->m_pCtxt->m_dirlist.GetAt(pos);
	return di;
}

// User chose (main menu) Copy from right to left
void CDirView::OnDirCopyFileToLeft() 
{
	DoCopyFileToLeft();
}
// User chose (main menu) Copy from left to right
void CDirView::OnDirCopyFileToRight() 
{
	DoCopyFileToRight();
}

// User chose (context men) Copy from right to left
void CDirView::OnCtxtDirCopyFileToLeft()
{
	DoCopyFileToLeft();
}
// User chose (context menu) Copy from left to right
void CDirView::OnCtxtDirCopyFileToRight()
{
	DoCopyFileToRight();
}

// Prompt & copy item from right to left, if legal
void CDirView::DoCopyFileToLeft() 
{
	int sel = m_pList->GetNextItem(-1, LVNI_SELECTED);
	if (sel == -1) return;

	CString sl, sr, slFile, srFile;
	if (!GetSelectedDirNames(sl, sr) || !GetSelectedFileNames(slFile, srFile))
		return;

	CDirDoc *pd = GetDocument();
	CString s;
	// find item from document
	POSITION pos;
	const DIFFITEM& di = GetDiffItem(sel, pos);
	// what we do depends on comparison result for this item
	switch(di.code)
	{
	case FILE_LUNIQUE:
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
	case FILE_LDIRUNIQUE:
	case FILE_RDIRUNIQUE:
		// TODO: Would be nice to allow copying directory
		// but must write code for recursive copy
		// and worse, this will make the item display invalid (new items not on it)
		// Perry 2002-11-26
	case FILE_SAME:
	default:
		// Not allowed, and should have been disabled choices anyway
		break;
	}
}
// Prompt & copy item from left to right, if legal
void CDirView::DoCopyFileToRight() 
{
	int sel = m_pList->GetNextItem(-1, LVNI_SELECTED);
	if (sel == -1) return;

	CString sl, sr, slFile, srFile;
	if (!GetSelectedDirNames(sl, sr) || !GetSelectedFileNames(slFile, srFile))
		return;

	CDirDoc *pd = GetDocument();
	CString s;
	// find item from document
	POSITION pos;
	const DIFFITEM& di = GetDiffItem(sel, pos);
	// what we do depends on comparison result for this item
	switch(di.code)
	{
	case FILE_RUNIQUE:
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
	case FILE_LDIRUNIQUE:
	case FILE_RDIRUNIQUE:
			// see comments in DoCopyFileToLeft
	case FILE_SAME:
	default:
		// Not allowed, and should have been disabled choices anyway
		break;
	}
}

// Update context menu Copy Right to Left item
void CDirView::OnUpdateCtxtDirCopyFileToLeft(CCmdUI* pCmdUI) 
{
	DoUpdateDirCopyFileToLeft(pCmdUI);
}
// Update context menu Copy Left to Right item
void CDirView::OnUpdateCtxtDirCopyFileToRight(CCmdUI* pCmdUI) 
{
	DoUpdateDirCopyFileToRight(pCmdUI);
}

// Update main menu Copy Right to Left item
void CDirView::OnUpdateDirCopyFileToLeft(CCmdUI* pCmdUI) 
{
	DoUpdateDirCopyFileToLeft(pCmdUI);
}
// Update main menu Copy Left to Right item
void CDirView::OnUpdateDirCopyFileToRight(CCmdUI* pCmdUI) 
{
	DoUpdateDirCopyFileToRight(pCmdUI);
}

// Should Copy to Left be enabled or disabled ? (both main menu & context menu use this)
void CDirView::DoUpdateDirCopyFileToLeft(CCmdUI* pCmdUI) 
{
	int sel = m_pList->GetNextItem(-1, LVNI_SELECTED);
	if (sel == -1)
	{
		// no item there
		pCmdUI->Enable(FALSE);
		return;
	}
	// found item (normal case)
	// find item from document
	POSITION pos;
	const DIFFITEM& di = GetDiffItem(sel, pos);
	// what we do depends on comparison result for this item
	switch(di.code)
	{
	case FILE_LUNIQUE:
		pCmdUI->Enable(FALSE); // no right item, so can't copy to left
		break;
	case FILE_RUNIQUE:
	case FILE_DIFF:
	case FILE_BINDIFF:
		pCmdUI->Enable(TRUE);
		break;
	case FILE_SAME:
		pCmdUI->Enable(FALSE);
		break;
	case FILE_LDIRUNIQUE:
		pCmdUI->Enable(FALSE); // no right item, so can't copy to left
		break;
	case FILE_RDIRUNIQUE:
	// TODO 2002-11-22: Enable unique also, but must write code to do recursive copy
		pCmdUI->Enable(TRUE);
		break;
	default:
		pCmdUI->Enable(FALSE);
		break;
	}
}
// Should Copy to Right be enabled or disabled ? (both main menu & context menu use this)
void CDirView::DoUpdateDirCopyFileToRight(CCmdUI* pCmdUI) 
{
	int sel = m_pList->GetNextItem(-1, LVNI_SELECTED);
	if (sel == -1)
	{
		// no item there
		pCmdUI->Enable(FALSE);
		return;
	}
	// found item (normal case)
	// find item from document
	POSITION pos;
	const DIFFITEM& di = GetDiffItem(sel, pos);
	switch(di.code)
	{
	case FILE_LUNIQUE:
		pCmdUI->Enable(TRUE);
		break;
	case FILE_RUNIQUE:
		pCmdUI->Enable(FALSE); // no left item, so can't copy to right
		break;
	case FILE_DIFF:
	case FILE_BINDIFF:
		pCmdUI->Enable(TRUE);
		break;
	case FILE_LDIRUNIQUE:
	// TODO 2002-11-22: Enable unique also, but must write code to do recursive copy
		pCmdUI->Enable(FALSE);
		break;
	case FILE_RDIRUNIQUE:
		pCmdUI->Enable(FALSE); // no left item, so can't copy to right
		break;
	case FILE_SAME:
	default:
		pCmdUI->Enable(FALSE);
		break;
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
	VERIFY(s.LoadString(IDS_EXTENSION_HEADER));
	lvc.pszText = (LPTSTR)((LPCTSTR)s);
	m_pList->SetColumn(DV_EXT, &lvc);
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
	case DV_EXT: // File extension.                    // BSP - Provide a comparison by file extension
		retVal = _tcscmp(lDi.extension, rDi.extension);
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
		m_sortColumn = pNMListView->iSubItem;
		// date columns get default descending sort.
		if(m_sortColumn==3 || m_sortColumn==4)
		{
			m_bSortAscending = false;
		}
		else
		{
			m_bSortAscending = true;
		}
	}
	m_ctlSortHeader.SetSortImage(m_sortColumn, m_bSortAscending);

	//sort using static CompareFunc comparison function
	GetListCtrl ().SortItems (CompareFunc, reinterpret_cast<DWORD>(this));//pNMListView->iSubItem);
	*pResult = 0;
}

void CDirView::OnDestroy() 
{
	// save the column widths
	CListCtrl& ctl = GetListCtrl();

	CHeaderCtrl *phdr = ctl.GetHeaderCtrl();
	for (int i=0; i < phdr->GetItemCount(); i++)
	{
		CString s;
		s.Format(_T("WDirHdr%d"), i);
		theApp.WriteProfileInt(_T("DirView"), s, ctl.GetColumnWidth(i));
	}

	CListViewEx::OnDestroy();
	
}



void CDirView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar==VK_RETURN)
	{
		OpenSelection();
	}
	CListViewEx::OnChar(nChar, nRepCnt, nFlags);
}

void CDirView::OpenSelection()
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

}

// User chose (context menu) delete left
void CDirView::OnCtxtDirDelLeft()
{
	DoDelLeft();
}
// User chose (context menu) delete right
void CDirView::OnCtxtDirDelRight()
{
	DoDelRight();
}

// Prompt & delete left, if legal
void CDirView::DoDelLeft() 
{
	int sel = m_pList->GetNextItem(-1, LVNI_SELECTED);
	if (sel == -1) return;

	CString sl, sr, slFile, srFile;
	if (!GetSelectedDirNames(sl, sr) || !GetSelectedFileNames(slFile, srFile))
		return;

	// find item from document
	POSITION pos;
	const DIFFITEM& di = GetDiffItem(sel, pos);

	// need to know if file or directory
	switch(di.code)
	{
	case FILE_LUNIQUE:
	case FILE_DIFF:
	case FILE_BINDIFF:
		ConfirmAndDeleteFileAndUpdate(slFile, pos, sel);
		break;
	case FILE_LDIRUNIQUE:
		ConfirmAndDeleteDirAndUpdate(slFile, pos, sel);
		break;
	}
}
// Prompt & delete right, if legal
void CDirView::DoDelRight() 
{
	int sel = m_pList->GetNextItem(-1, LVNI_SELECTED);
	if (sel == -1) return;

	CString sl, sr, slFile, srFile;
	if (!GetSelectedDirNames(sl, sr) || !GetSelectedFileNames(slFile, srFile))
		return;

	// find item from document
	POSITION pos;
	const DIFFITEM& di = GetDiffItem(sel, pos);

	// need to know if file or directory
	switch(di.code)
	{
	case FILE_RUNIQUE:
	case FILE_DIFF:
	case FILE_BINDIFF:
		ConfirmAndDeleteFileAndUpdate(srFile, pos, sel);
		break;
	case FILE_RDIRUNIQUE:
		ConfirmAndDeleteDirAndUpdate(srFile, pos, sel);
		break;
	}
}

// Enable/disable Delete Left menu choice on context menu
void CDirView::OnUpdateCtxtDirDelLeft(CCmdUI* pCmdUI)
{
	DoUpdateCtxtDirDelLeft(pCmdUI);
}

// Enable/disable Delete Right menu choice on context menu
void CDirView::OnUpdateCtxtDirDelRight(CCmdUI* pCmdUI) 
{
	DoUpdateCtxtDirDelRight(pCmdUI);
}

// Should Delete left be enabled or disabled ?
void CDirView::DoUpdateCtxtDirDelLeft(CCmdUI* pCmdUI) 
{
	int sel = m_pList->GetNextItem(-1, LVNI_SELECTED);
	if (sel == -1)
	{
		// no item there
		pCmdUI->Enable(FALSE);
		return;
	}
	// found item (normal case)
	// find item from document
	POSITION pos;
	const DIFFITEM& di = GetDiffItem(sel, pos);
	// what we do depends on comparison result for this item
	switch(di.code)
	{
	case FILE_LUNIQUE:
		pCmdUI->Enable(TRUE);
		break;
	case FILE_RUNIQUE:
		pCmdUI->Enable(FALSE); // no left item, so can't delete left
		break;
	case FILE_DIFF:
	case FILE_BINDIFF:
		pCmdUI->Enable(TRUE);
		break;
	case FILE_SAME:
		pCmdUI->Enable(FALSE);
		break;
	case FILE_LDIRUNIQUE:
		pCmdUI->Enable(TRUE);
		break;
	case FILE_RDIRUNIQUE:
		pCmdUI->Enable(FALSE); // no left item, so can't delete left
		break;
	default:
		pCmdUI->Enable(FALSE);
		break;
	}
}
// Should Delete right be enabled or disabled ?
void CDirView::DoUpdateCtxtDirDelRight(CCmdUI* pCmdUI) 
{
	int sel = m_pList->GetNextItem(-1, LVNI_SELECTED);
	if (sel == -1)
	{
		// no item there
		pCmdUI->Enable(FALSE);
		return;
	}
	// found item (normal case)
	// find item from document
	POSITION pos;
	const DIFFITEM& di = GetDiffItem(sel, pos);
	// what we do depends on comparison result for this item
	switch(di.code)
	{
	case FILE_LUNIQUE:
		pCmdUI->Enable(FALSE); // no right item, so can't delete right
		break;
	case FILE_RUNIQUE:
		pCmdUI->Enable(TRUE);
		break;
	case FILE_DIFF:
	case FILE_BINDIFF:
		pCmdUI->Enable(TRUE);
		break;
	case FILE_SAME:
		pCmdUI->Enable(FALSE);
		break;
	case FILE_LDIRUNIQUE:
		pCmdUI->Enable(FALSE); // no right item, so can't delete right
		break;
	case FILE_RDIRUNIQUE:
		pCmdUI->Enable(TRUE);
		break;
	default:
		pCmdUI->Enable(FALSE);
		break;
	}
}

// Prompt & delete file, & remove its data entries
void CDirView::ConfirmAndDeleteFileAndUpdate(LPCTSTR szFile, POSITION pos, int sel)
{
	// May want an option to suppress these message boxes

	if (!mf->ConfirmAndDeleteFile(szFile))
		return;
	// remove item data from document & screen
	GetDocument()->m_pCtxt->m_dirlist.RemoveAt(pos);
	GetListCtrl().DeleteItem(sel);
}

// Prompt & delete directory, & remove its data entries
void CDirView::ConfirmAndDeleteDirAndUpdate(LPCTSTR szDir, POSITION pos, int sel)
{
	// May want an option to suppress these message boxes

	if (!mf->ConfirmAndDeleteDir(szDir))
		return;
	// remove item data from document & screen
	// so, this assumes it is a unique entry (has no children listed)
	GetDocument()->m_pCtxt->m_dirlist.RemoveAt(pos);
	GetListCtrl().DeleteItem(sel);
}
