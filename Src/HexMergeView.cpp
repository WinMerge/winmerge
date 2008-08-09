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
 * @file  HexMergeView.cpp
 *
 * @brief Implementation file for CHexMergeDoc
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "Merge.h"
#include "MainFrm.h"
#include "HexMergeFrm.h"
#include "HexMergeView.h"
#include "OptionsDef.h"
#include "Environment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 * @brief Turn bool api result into success/error code.
 */
static HRESULT NTAPI SE(BOOL f)
{
	if (f)
		return S_OK;
	HRESULT hr = (HRESULT)::GetLastError();
	ASSERT(hr);
	if (hr == 0)
		hr = E_UNEXPECTED;
	return hr;
}

static UINT64 NTAPI GetLastWriteTime(HANDLE h)
{
	UINT64 ft;
	return ::GetFileTime(h, 0, 0, reinterpret_cast<FILETIME *>(&ft)) ? ft : 0;
}

static void NTAPI SetLastWriteTime(HANDLE h, UINT64 ft)
{
	::SetFileTime(h, 0, 0, reinterpret_cast<FILETIME *>(&ft));
}

/////////////////////////////////////////////////////////////////////////////
// CHexMergeView

IMPLEMENT_DYNCREATE(CHexMergeView, CView)

BEGIN_MESSAGE_MAP(CHexMergeView, CView)
	//{{AFX_MSG_MAP(CHexMergeView)
	ON_MESSAGE_VOID(WM_PAINT, CWnd::OnPaint)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_NCCALCSIZE()
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_REPLACE, OnEditReplace)
	ON_COMMAND(ID_EDIT_REPEAT, OnEditRepeat)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_FIRSTDIFF, OnFirstdiff)
	ON_COMMAND(ID_LASTDIFF, OnLastdiff)
	ON_COMMAND(ID_NEXTDIFF, OnNextdiff)
	ON_COMMAND(ID_PREVDIFF, OnPrevdiff)
	//}}AFX_MSG_MAP
	// Test case to verify WM_COMMAND won't accidentally go through Default()
	//ON_COMMAND(ID_APP_ABOUT, Default)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHexMergeView construction/destruction

/**
 * @brief Constructor.
 */
CHexMergeView::CHexMergeView()
: m_pif(0)
, m_nThisPane(0)
, m_mtime(0)
{
}

/**
 * @brief Drawing is not supported
 */
void CHexMergeView::OnDraw(CDC *)
{
	ASSERT(FALSE);
}

/**
 * @brief Load heksedit.dll and setup window class name
 */
BOOL CHexMergeView::PreCreateWindow(CREATESTRUCT& cs)
{
	static const TCHAR szFileName[] = _T("heksedit.dll");
	static const TCHAR szClassName[] = _T("frhed hexclass");
	if ((cs.hInstance = ::GetModuleHandle(szFileName)) == 0 &&
		(cs.hInstance = ::LoadLibrary(szFileName)) == 0)
	{
		return FALSE;
	}
	cs.lpszClass = szClassName;
	cs.style |= WS_HSCROLL | WS_VSCROLL;
	return TRUE;
}

/**
 * @brief Grab the control's IHexEditorWindow interface pointer upon window creation
 */
int CHexMergeView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_pif = reinterpret_cast<IHexEditorWindow *>(::GetWindowLongPtr(m_hWnd, GWL_USERDATA));
	if (m_pif == 0)
		return -1;
	return 0;
}

/**
 * @brief Skip default WM_NCCALCSIZE processing so as to prevent scrollbars from showing up
 */
void CHexMergeView::OnNcCalcSize(BOOL, NCCALCSIZE_PARAMS *)
{
}

/**
 * @brief Synchronize all involved scrollbars
 */
void CHexMergeView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	SCROLLINFO si;
	if (pScrollBar && nSBCode == SB_THUMBTRACK)
	{
		pScrollBar->GetScrollInfo(&si, SIF_ALL | SIF_DISABLENOSCROLL);
		si.nPos = si.nTrackPos;
		SetScrollInfo(SB_HORZ, &si);
	}
	CView::OnHScroll(nSBCode, nPos, pScrollBar);
	if (pScrollBar)
	{
		GetScrollInfo(SB_HORZ, &si, SIF_ALL | SIF_DISABLENOSCROLL);
		if (nSBCode != SB_THUMBTRACK)
		{
			pScrollBar->SetScrollInfo(&si);
		}
		CSplitterWndEx *pSplitter = (CSplitterWndEx *)GetParentSplitter(this, TRUE);
		int nID = GetDlgCtrlID();
		nID ^= pSplitter->IdFromRowCol(0, 0) ^ pSplitter->IdFromRowCol(0, 1);
		CWnd *pWnd = pSplitter->GetDlgItem(nID);
		pWnd->SetScrollInfo(SB_HORZ, &si);
		pWnd->SendMessage(WM_HSCROLL, MAKEWPARAM(nSBCode, nPos));
	}
}

/**
 * @brief Synchronize all involved scrollbars
 */
void CHexMergeView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	SCROLLINFO si;
	if (pScrollBar && nSBCode == SB_THUMBTRACK)
	{
		pScrollBar->GetScrollInfo(&si);
		si.nPos = si.nTrackPos;
		SetScrollInfo(SB_VERT, &si, SIF_ALL | SIF_DISABLENOSCROLL);
	}
	CView::OnVScroll(nSBCode, nPos, pScrollBar);
	if (pScrollBar && nSBCode != SB_THUMBTRACK)
	{
		GetScrollInfo(SB_VERT, &si);
		pScrollBar->SetScrollInfo(&si, SIF_ALL | SIF_DISABLENOSCROLL);
	}
}

/**
 * @brief Synchronize file path bar activation states
 */
void CHexMergeView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	CHexMergeFrame *pFrameWnd = static_cast<CHexMergeFrame *>(GetParentFrame());
	pFrameWnd->GetHeaderInterface()->SetActive(m_nThisPane, bActivate);
}

/**
 * @brief Get pointer to control's content buffer
 */
BYTE *CHexMergeView::GetBuffer(int length)
{
	return m_pif->get_buffer(length);
}

/**
 * @brief Get length of control's content buffer
 */
int CHexMergeView::GetLength()
{
	return m_pif->get_length();
}

/**
 * @brief Checks if file has changed since last update
 * @param [in] path File to check
 * @return TRUE if file is changed.
 */
BOOL CHexMergeView::IsFileChangedOnDisk(LPCTSTR path)
{
	// NB: FileTimes are measured in 100 nanosecond intervals since 1601-01-01.
	BOOL bChanged = FALSE;
	HANDLE h = CreateFile(path, FILE_READ_ATTRIBUTES, FILE_SHARE_READ|FILE_SHARE_WRITE,
		0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (h != INVALID_HANDLE_VALUE)
	{
		UINT64 mtime = GetLastWriteTime(h);
		UINT64 lower = min(mtime, m_mtime);
		UINT64 upper = max(mtime, m_mtime);
		BOOL bIgnoreSmallDiff = GetOptionsMgr()->GetBool(OPT_IGNORE_SMALL_FILETIME);
		UINT64 tolerance = bIgnoreSmallDiff ? SmallTimeDiff * 10000000 : 0;
		bChanged = upper - lower > tolerance || m_size != GetFileSize(h, 0);
		CloseHandle(h);
	}
	return bChanged;
}

/**
 * @brief Load file
 */
HRESULT CHexMergeView::LoadFile(LPCTSTR path)
{
	HANDLE h = CreateFile(path, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	HRESULT hr = SE(h != INVALID_HANDLE_VALUE);
	if (hr != S_OK)
		return hr;
	m_mtime = GetLastWriteTime(h);
	DWORD length = m_size = GetFileSize(h, 0);
	hr = SE(length != INVALID_FILE_SIZE);
	if (hr == S_OK)
	{
		if (void *buffer = GetBuffer(length))
		{
			DWORD cb = 0;
			hr = SE(ReadFile(h, buffer, length, &cb, 0) && cb == length);
			if (hr != S_OK)
				GetBuffer(0);
		}
		else
		{
			hr = E_OUTOFMEMORY;
		}
	}
	CloseHandle(h);
	return hr;
}

/**
 * @brief Save file
 */
HRESULT CHexMergeView::SaveFile(LPCTSTR path)
{
	// Warn user in case file has been changed by someone else
	if (IsFileChangedOnDisk(path))
	{
		CString msg;
		LangFormatString1(msg, IDS_FILECHANGED_ONDISK, path);
		if (AfxMessageBox(msg, MB_ICONWARNING | MB_YESNO) == IDNO)
			return S_OK;
	}
	// Ask user what to do about FILE_ATTRIBUTE_READONLY
	CString strPath = path;
	BOOL bApplyToAll = FALSE;
	if (GetMainFrame()->HandleReadonlySave(strPath, FALSE, bApplyToAll) == IDCANCEL)
		return S_OK;
	path = strPath;
	// Take a chance to create a backup
	if (!GetMainFrame()->CreateBackup(FALSE, path))
		return S_OK;
	// Write data to an intermediate file
	String tempPath = env_GetTempPath(0);
	String sIntermediateFilename = env_GetTempFileName(tempPath.c_str(), _T("MRG_"), 0);
	if (sIntermediateFilename.empty())
		return E_FAIL; //Nothing to do if even tempfile name fails
	HANDLE h = CreateFile(sIntermediateFilename.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
		0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	HRESULT hr = SE(h != INVALID_HANDLE_VALUE);
	if (hr != S_OK)
		return hr;
	DWORD length = GetLength();
	void *buffer = GetBuffer(length);
	if (buffer == 0)
		return E_POINTER;
	DWORD cb = 0;
	hr = SE(WriteFile(h, buffer, length, &cb, 0) && cb == length);
	UINT64 mtime = GetLastWriteTime(h);
	CloseHandle(h);
	if (hr != S_OK)
		return hr;
	hr = SE(CopyFile(sIntermediateFilename.c_str(), path, FALSE));
	if (hr != S_OK)
		return hr;
	m_mtime = mtime;
	SetModified(FALSE);
	hr = SE(DeleteFile(sIntermediateFilename.c_str()));
	if (hr != S_OK)
	{
		LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
			sIntermediateFilename.c_str(), GetSysError(hr)));
	}
	return S_OK;
}

/**
 * @brief Get status
 */
IHexEditorWindow::Status *CHexMergeView::GetStatus()
{
	return m_pif->get_status();
}

/**
 * @brief Get modified flag
 */
BOOL CHexMergeView::GetModified()
{
	return m_pif->get_status()->iFileChanged;
}

/**
 * @brief Set modified flag
 */
void CHexMergeView::SetModified(BOOL bModified)
{
	m_pif->get_status()->iFileChanged = bModified;
}

/**
 * @brief Get readonly flag
 */
BOOL CHexMergeView::GetReadOnly()
{
	return m_pif->get_settings()->bReadOnly;
}

/**
 * @brief Set readonly flag
 */
void CHexMergeView::SetReadOnly(BOOL bReadOnly)
{
	m_pif->get_settings()->bReadOnly = bReadOnly;
}

/**
 * @brief Allow the control to update all kinds of things that need to be updated when
 * the window or content buffer have been resized or certain settings have been changed.
 */
void CHexMergeView::ResizeWindow()
{
	m_pif->resize_window();
}

/**
 * @brief Repaint a range of bytes
 */
void CHexMergeView::RepaintRange(int i, int j)
{
	int iBytesPerLine = m_pif->get_settings()->iBytesPerLine;
	m_pif->repaint(i / iBytesPerLine, j / iBytesPerLine);
}

/**
 * @brief Find a sequence of bytes
 */
void CHexMergeView::OnEditFind()
{
	m_pif->CMD_find();
}

/**
 * @brief Find & replace a sequence of bytes
 */
void CHexMergeView::OnEditReplace()
{
	m_pif->CMD_replace();
}

/**
 * @brief Repeat last find in one or another direction
 */
void CHexMergeView::OnEditRepeat()
{
	if (GetKeyState(VK_SHIFT) < 0)
		m_pif->CMD_findprev();
	else
		m_pif->CMD_findnext();
}

/**
 * @brief Cut selected content
 */
void CHexMergeView::OnEditCut()
{
	m_pif->CMD_edit_cut();
}

/**
 * @brief Copy selected content
 */
void CHexMergeView::OnEditCopy()
{
	m_pif->CMD_edit_copy();
}

/**
 * @brief Paste clipboard content over selected content
 */
void CHexMergeView::OnEditPaste()
{
	m_pif->CMD_edit_paste();
}

/**
 * @brief Clear selected content
 */
void CHexMergeView::OnEditClear()
{
	m_pif->CMD_edit_clear();
}

/**
 * @brief Check for keyboard commands
 */
BOOL CHexMergeView::PreTranslateMessage(MSG* pMsg)
{
	if (GetTopLevelFrame()->PreTranslateMessage(pMsg))
		return TRUE;
	if (pMsg->message == WM_KEYDOWN)
	{
		// Close window in response to VK_ESCAPE if user has allowed it from options
		if (pMsg->wParam == VK_ESCAPE && GetOptionsMgr()->GetBool(OPT_CLOSE_WITH_ESC))
		{
			GetParentFrame()->PostMessage(WM_CLOSE, 0, 0);
			return TRUE;
		}
	}
	return m_pif->translate_accelerator(pMsg);
}

/**
 * @brief Go to first diff
 */
void CHexMergeView::OnFirstdiff()
{
	m_pif->select_next_diff(TRUE);
}

/**
 * @brief Go to last diff
 */
void CHexMergeView::OnLastdiff()
{
	m_pif->select_prev_diff(TRUE);
}

/**
 * @brief Go to next diff
 */
void CHexMergeView::OnNextdiff()
{
	m_pif->select_next_diff(FALSE);
}

/**
 * @brief Go to previous diff
 */
void CHexMergeView::OnPrevdiff()
{
	m_pif->select_prev_diff(FALSE);
}
