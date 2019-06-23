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
 * @brief Implementation file for CHexMergeView
 *
 */

#include "stdafx.h"
#include "HexMergeFrm.h"
#include "Merge.h"
#include "MainFrm.h"
#include "HexMergeView.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "Environment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @brief Turn bool api result into success/error code
 */
static HRESULT NTAPI SE(BOOL f)
{
	if (f)
		return S_OK;
	HRESULT hr = (HRESULT)::GetLastError();
	ASSERT(hr != NULL);
	if (hr == NULL)
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
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
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
: m_pif(nullptr)
, m_nThisPane(0)
{
}

/**
 * @brief Drawing is not supported
 */
void CHexMergeView::OnDraw(CDC *)
{
	ASSERT(false);
}

/**
 * @brief returns true if heksedit.dll is loadable
 */
bool CHexMergeView::IsLoadable()
{
	static void *pv = nullptr;
	if (pv == nullptr)
	{
		pv = LoadLibrary(_T("Frhed\\hekseditU.dll"));
	}
	return pv != nullptr;
}

/**
 * @brief Load heksedit.dll and setup window class name
 */
BOOL CHexMergeView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!IsLoadable())
		LangMessageBox(IDS_FRHED_NOTINSTALLED, MB_OK);
	cs.lpszClass = _T("heksedit");
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
	m_pif = reinterpret_cast<IHexEditorWindow *>(::GetWindowLongPtr(m_hWnd, GWLP_USERDATA));
	if (m_pif == nullptr || m_pif->get_interface_version() < HEKSEDIT_INTERFACE_VERSION)
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
	if (pScrollBar != nullptr)
	{
		GetScrollInfo(SB_HORZ, &si, SIF_ALL | SIF_DISABLENOSCROLL);
		if (nSBCode != SB_THUMBTRACK)
		{
			pScrollBar->SetScrollInfo(&si);
		}
		
		CSplitterWndEx *pSplitter = static_cast<CSplitterWndEx *>(GetParentSplitter(this, TRUE));
		for (int pane = 0; pane < pSplitter->GetColumnCount(); ++pane)
		{
			if (pane != m_nThisPane)
			{
				CWnd *pWnd = pSplitter->GetDlgItem(pSplitter->IdFromRowCol(0, pane));
				pWnd->SetScrollInfo(SB_HORZ, &si);
				pWnd->SendMessage(WM_HSCROLL, MAKEWPARAM(nSBCode, nPos));
			}
		}
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
	pFrameWnd->GetHeaderInterface()->SetActive(m_nThisPane, !!bActivate);
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
 * @return `true` if file is changed.
 */
bool CHexMergeView::IsFileChangedOnDisk(LPCTSTR path)
{
	DiffFileInfo dfi;
	dfi.Update(path);
	int tolerance = 0;
	if (GetOptionsMgr()->GetBool(OPT_IGNORE_SMALL_FILETIME))
		tolerance = SmallTimeDiff; // From MainFrm.h
	int64_t timeDiff = dfi.mtime - m_fileInfo.mtime;
	if (timeDiff < 0) timeDiff = -timeDiff;
	if ((timeDiff > tolerance * Poco::Timestamp::resolution()) || (dfi.size != m_fileInfo.size))
		return true;
	return false;
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
	if (h == INVALID_HANDLE_VALUE)
		return hr;
	DWORD length = GetFileSize(h, 0);
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
		else if (length != 0)
		{
			hr = E_OUTOFMEMORY;
		}
	}
	CloseHandle(h);
	m_fileInfo.Update(path);
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
		String msg = strutils::format_string1(_("Another application has updated file\n%1\nsince WinMerge loaded it.\n\nOverwrite changed file?"), path);
		if (AfxMessageBox(msg.c_str(), MB_ICONWARNING | MB_YESNO) == IDNO)
			return S_OK;
	}
	// Ask user what to do about FILE_ATTRIBUTE_READONLY
	String strPath = path;
	bool bApplyToAll = false;
	if (theApp.HandleReadonlySave(strPath, false, bApplyToAll) == IDCANCEL)
		return S_OK;
	path = strPath.c_str();
	// Take a chance to create a backup
	if (!theApp.CreateBackup(false, path))
		return S_OK;
	// Write data to an intermediate file
	String tempPath = env::GetTemporaryPath();
	String sIntermediateFilename = env::GetTemporaryFileName(tempPath, _T("MRG_"), 0);
	if (sIntermediateFilename.empty())
		return E_FAIL; //Nothing to do if even tempfile name fails
	HANDLE h = CreateFile(sIntermediateFilename.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
		0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	HRESULT hr = SE(h != INVALID_HANDLE_VALUE);
	if (h == INVALID_HANDLE_VALUE)
		return hr;
	DWORD length = GetLength();
	void *buffer = GetBuffer(length);
	if (buffer == 0)
	{
		CloseHandle(h);
		return E_POINTER;
	}
	DWORD cb = 0;
	hr = SE(WriteFile(h, buffer, length, &cb, 0) && cb == length);
	CloseHandle(h);
	if (hr != S_OK)
		return hr;
	hr = SE(CopyFile(sIntermediateFilename.c_str(), path, FALSE));
	if (hr != S_OK)
		return hr;
	m_fileInfo.Update(path);
	SetSavePoint();
	hr = SE(DeleteFile(sIntermediateFilename.c_str()));
	if (hr != S_OK)
	{
		LogErrorString(strutils::format(_T("DeleteFile(%s) failed: %s"),
			sIntermediateFilename, GetSysError(hr)));
	}
	return S_OK;
}

/**
 * @brief Get modified flag
 */
bool CHexMergeView::GetModified()
{
	return m_pif->get_status()->iFileChanged != 0;
}

/**
 * @brief Set modified flag
 */
void CHexMergeView::SetSavePoint()
{
	m_pif->set_savepoint();
}

/**
 * @brief Clear undo records
 */
void CHexMergeView::ClearUndoRecords()
{
	m_pif->clear_undorecords();
}

/**
 * @brief Get readonly flag
 */
bool CHexMergeView::GetReadOnly()
{
	return m_pif->get_settings()->bReadOnly;
}

/**
 * @brief Set readonly flag
 */
void CHexMergeView::SetReadOnly(bool bReadOnly)
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
* @brief Called when "Undo" item is updated
*/
void CHexMergeView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pif->can_undo());
}

/**
 * @brief Undo last action
 */
void CHexMergeView::OnEditUndo()
{
	m_pif->CMD_edit_undo();
}

/**
* @brief Called when "Redo" item is updated
*/
void CHexMergeView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pif->can_redo());
}

/**
 * @brief Redo last action
 */
void CHexMergeView::OnEditRedo()
{
	m_pif->CMD_edit_redo();
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
 * @brief Select entire content
 */
void CHexMergeView::OnEditSelectAll()
{
	m_pif->CMD_select_all();
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

void CHexMergeView::ZoomText(int amount)
{
	m_pif->CMD_zoom(amount);
}

/**
 * @brief Copy selected bytes from source view to destination view
 * @note Grows destination buffer as appropriate
 */
void CHexMergeView::CopySel(const CHexMergeView *src, CHexMergeView *dst)
{
	dst->m_pif->copy_sel_from(src->m_pif);
}

/**
 * @brief Copy all bytes from source view to destination view
 * @note Grows destination buffer as appropriate
 */
void CHexMergeView::CopyAll(const CHexMergeView *src, CHexMergeView *dst)
{
	dst->m_pif->copy_all_from(src->m_pif);
}

