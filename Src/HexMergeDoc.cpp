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
 * @file  HexMergeDoc.cpp
 *
 * @brief Implementation file for CHexMergeDoc
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include <afxinet.h>
#include "UnicodeString.h"
#include "FileTextEncoding.h"
#include "Merge.h"
#include "HexMergeDoc.h"
#include "HexMergeFrm.h"
#include "HexMergeView.h"
#include "DiffItem.h"
#include "FolderCmp.h"
#include "MainFrm.h"
#include "Environment.h"
#include "diffcontext.h"	// FILE_SAME
#include "getopt.h"
#include "fnmatch.h"
#include "coretools.h"
#include "dirdoc.h"
#include "files.h"
#include "OptionsDef.h"
#include "DiffFileInfo.h"
#include "SaveClosingDlg.h"
#include "DiffList.h"
#include "paths.h"
#include "OptionsMgr.h"
#include "FileOrFolderSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static void UpdateDiffItem(DIFFITEM &di, CDiffContext *pCtxt);
static int Try(HRESULT hr, UINT type = MB_OKCANCEL|MB_ICONSTOP);

/**
 * @brief Update diff item
 */
static void UpdateDiffItem(DIFFITEM &di, CDiffContext *pCtxt)
{
	di.diffcode.diffcode |= DIFFCODE::SIDEFLAGS;
	di.left.ClearPartial();
	di.right.ClearPartial();
	if (!pCtxt->UpdateInfoFromDiskHalf(di, TRUE))
		di.diffcode.diffcode &= ~DIFFCODE::LEFT;
	if (!pCtxt->UpdateInfoFromDiskHalf(di, FALSE))
		di.diffcode.diffcode &= ~DIFFCODE::RIGHT;
	// 1. Clear flags
	di.diffcode.diffcode &= ~(DIFFCODE::TEXTFLAGS | DIFFCODE::COMPAREFLAGS);
	// 2. Process unique files
	// We must compare unique files to itself to detect encoding
	if (di.diffcode.isSideLeftOnly() || di.diffcode.isSideRightOnly())
	{
		if (pCtxt->m_nCompMethod != CMP_DATE &&
			pCtxt->m_nCompMethod != CMP_DATE_SIZE &&
			pCtxt->m_nCompMethod != CMP_SIZE)
		{
			di.diffcode.diffcode |= DIFFCODE::SAME;
			FolderCmp folderCmp;
			int diffCode = folderCmp.prepAndCompareTwoFiles(pCtxt, di);
			// Add possible binary flag for unique items
			if (diffCode & DIFFCODE::BIN)
				di.diffcode.diffcode |= DIFFCODE::BIN;
		}
	}
	// 3. Compare two files
	else
	{
		// Really compare
		FolderCmp folderCmp;
		di.diffcode.diffcode |= folderCmp.prepAndCompareTwoFiles(pCtxt, di);
	}
}

/**
 * @brief Issue an error popup if passed in HRESULT is nonzero
 */
static int Try(HRESULT hr, UINT type)
{
	return hr ? CInternetException(hr).ReportError(type) : 0;
}

/////////////////////////////////////////////////////////////////////////////
// CHexMergeDoc

IMPLEMENT_DYNCREATE(CHexMergeDoc, CDocument)

BEGIN_MESSAGE_MAP(CHexMergeDoc, CDocument)
	//{{AFX_MSG_MAP(CHexMergeDoc)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_LEFT, OnFileSaveLeft)
	ON_COMMAND(ID_FILE_SAVE_RIGHT, OnFileSaveRight)
	ON_COMMAND(ID_FILE_SAVEAS_LEFT, OnFileSaveAsLeft)
	ON_COMMAND(ID_FILE_SAVEAS_RIGHT, OnFileSaveAsRight)
	ON_UPDATE_COMMAND_UI(ID_STATUS_DIFFNUM, OnUpdateStatusNum)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_LEFT, OnUpdateFileSaveLeft)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_RIGHT, OnUpdateFileSaveRight)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_L2R, OnL2r)
	ON_COMMAND(ID_R2L, OnR2l)
	ON_COMMAND(ID_ALL_LEFT, OnAllLeft)
	ON_COMMAND(ID_ALL_RIGHT, OnAllRight)
	ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomOut)
	ON_COMMAND(ID_VIEW_ZOOMNORMAL, OnViewZoomNormal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHexMergeDoc construction/destruction

/**
 * @brief Constructor.
 */
CHexMergeDoc::CHexMergeDoc()
: m_pDirDoc(NULL)
{
	m_pView[MERGE_VIEW_LEFT] = NULL;
	m_pView[MERGE_VIEW_RIGHT] = NULL;
	m_nBufferType[0] = BUFFER_NORMAL;
	m_nBufferType[1] = BUFFER_NORMAL;
}

/**
 * @brief Destructor.
 *
 * Informs associated dirdoc that mergedoc is closing.
 */
CHexMergeDoc::~CHexMergeDoc()
{	
	if (m_pDirDoc)
		m_pDirDoc->MergeDocClosing(this);
}

/**
 * @brief Update associated diff item
 */
void CHexMergeDoc::UpdateDiffItem(CDirDoc *pDirDoc)
{
	// If directory compare has results
	if (pDirDoc && pDirDoc->HasDiffs())
	{
		const String &pathLeft = m_filePaths.GetLeft();
		const String &pathRight = m_filePaths.GetRight();
		CDiffContext &ctxt = const_cast<CDiffContext &>(pDirDoc->GetDiffContext());
		if (UINT_PTR pos = pDirDoc->FindItemFromPaths(pathLeft.c_str(), pathRight.c_str()))
		{
			DIFFITEM &di = pDirDoc->GetDiffRefByKey(pos);
			::UpdateDiffItem(di, &ctxt);
		}
	}
	int lengthLeft = m_pView[MERGE_VIEW_LEFT]->GetLength();
	void *bufferLeft = m_pView[MERGE_VIEW_LEFT]->GetBuffer(lengthLeft);
	int lengthRight = m_pView[MERGE_VIEW_RIGHT]->GetLength();
	void *bufferRight = m_pView[MERGE_VIEW_RIGHT]->GetBuffer(lengthRight);
	GetParentFrame()->SetLastCompareResult(lengthLeft != lengthRight ||
		bufferLeft && bufferRight && memcmp(bufferLeft, bufferRight, lengthLeft));
}

/**
 * @brief Asks and then saves modified files
 */
BOOL CHexMergeDoc::PromptAndSaveIfNeeded(BOOL bAllowCancel)
{
	const BOOL bLModified = m_pView[MERGE_VIEW_LEFT]->GetModified();
	const BOOL bRModified = m_pView[MERGE_VIEW_RIGHT]->GetModified();

	if (!bLModified && !bRModified) //Both files unmodified
		return TRUE;

	const String &pathLeft = m_filePaths.GetLeft();
	const String &pathRight = m_filePaths.GetRight();

	BOOL result = TRUE;
	BOOL bLSaveSuccess = FALSE;
	BOOL bRSaveSuccess = FALSE;

	SaveClosingDlg dlg;
	dlg.DoAskFor(bLModified, bRModified);
	if (!bAllowCancel)
		dlg.m_bDisableCancel = TRUE;
	if (!pathLeft.empty())
		dlg.m_sLeftFile = pathLeft.c_str();
	else
		dlg.m_sLeftFile = m_strDesc[0].c_str();
	if (!pathRight.empty())
		dlg.m_sRightFile = pathRight.c_str();
	else
		dlg.m_sRightFile = m_strDesc[1].c_str();

	if (dlg.DoModal() == IDOK)
	{
		if (bLModified)
		{
			if (dlg.m_leftSave == SaveClosingDlg::SAVECLOSING_SAVE)
			{
				switch (Try(m_pView[MERGE_VIEW_LEFT]->SaveFile(pathLeft.c_str())))
				{
				case 0:
					bLSaveSuccess = TRUE;
					break;
				case IDCANCEL:
					result = FALSE;
					break;
				}
			}
			else
			{
				m_pView[MERGE_VIEW_LEFT]->SetModified(FALSE);
			}
		}
		if (bRModified)
		{
			if (dlg.m_rightSave == SaveClosingDlg::SAVECLOSING_SAVE)
			{
				switch (Try(m_pView[MERGE_VIEW_RIGHT]->SaveFile(pathRight.c_str())))
				{
				case 0:
					bRSaveSuccess = TRUE;
					break;
				case IDCANCEL:
					result = FALSE;
					break;
				}
			}
			else
			{
				m_pView[MERGE_VIEW_RIGHT]->SetModified(FALSE);
			}
		}
	}
	else
	{	
		result = FALSE;
	}

	// If file were modified and saving was successfull,
	// update status on dir view
	if (bLSaveSuccess || bRSaveSuccess)
	{
		UpdateDiffItem(m_pDirDoc);
	}

	return result;
}

/**
 * @brief Save modified documents
 */
BOOL CHexMergeDoc::SaveModified()
{
	return PromptAndSaveIfNeeded(TRUE);
}

/**
 * @brief Saves both files
 */
void CHexMergeDoc::OnFileSave() 
{
	BOOL bUpdate = FALSE;
	if (m_pView[MERGE_VIEW_LEFT]->GetModified())
	{
		const String &pathLeft = m_filePaths.GetLeft();
		if (Try(m_pView[MERGE_VIEW_LEFT]->SaveFile(pathLeft.c_str())) == IDCANCEL)
			return;
		bUpdate = TRUE;
	}
	if (m_pView[MERGE_VIEW_RIGHT]->GetModified())
	{
		const String &pathRight = m_filePaths.GetRight();
		if (Try(m_pView[MERGE_VIEW_RIGHT]->SaveFile(pathRight.c_str())) == IDCANCEL)
			return;
		bUpdate = TRUE;
	}
	if (bUpdate)
		UpdateDiffItem(m_pDirDoc);
}

/**
 * @brief Saves left-side file
 */
void CHexMergeDoc::OnFileSaveLeft()
{
	if (m_pView[MERGE_VIEW_LEFT]->GetModified())
	{
		const String &pathLeft = m_filePaths.GetLeft();
		if (Try(m_pView[MERGE_VIEW_LEFT]->SaveFile(pathLeft.c_str())) == IDCANCEL)
			return;
		UpdateDiffItem(m_pDirDoc);
	}
}

/**
 * @brief Saves right-side file
 */
void CHexMergeDoc::OnFileSaveRight()
{
	if (m_pView[MERGE_VIEW_RIGHT]->GetModified())
	{
		const String &pathRight = m_filePaths.GetRight();
		if (Try(m_pView[MERGE_VIEW_RIGHT]->SaveFile(pathRight.c_str())) == IDCANCEL)
			return;
		UpdateDiffItem(m_pDirDoc);
	}
}

/**
 * @brief Saves left-side file with name asked
 */
void CHexMergeDoc::OnFileSaveAsLeft()
{
	const String &pathLeft = m_filePaths.GetLeft();
	CString strPath;
	if (SelectFile(0, strPath, pathLeft.c_str(), IDS_SAVE_LEFT_AS, NULL, FALSE))
	{
		if (Try(m_pView[MERGE_VIEW_LEFT]->SaveFile(strPath)) == IDCANCEL)
			return;
		m_filePaths.SetLeft(strPath);
		UpdateDiffItem(m_pDirDoc);
	}
}

/**
 * @brief Saves right-side file with name asked
 */
void CHexMergeDoc::OnFileSaveAsRight()
{
	const String &pathRight = m_filePaths.GetRight();
	CString strPath;
	if (SelectFile(0, strPath, pathRight.c_str(), IDS_SAVE_LEFT_AS, NULL, FALSE))
	{
		if (Try(m_pView[MERGE_VIEW_RIGHT]->SaveFile(strPath)) == IDCANCEL)
			return;
		m_filePaths.SetRight(strPath);
		UpdateDiffItem(m_pDirDoc);
	}
}

/**
 * @brief Update diff-number pane text
 */
void CHexMergeDoc::OnUpdateStatusNum(CCmdUI* pCmdUI) 
{
	String s;
	pCmdUI->SetText(s.c_str());
}

/**
 * @brief DirDoc gives us its identity just after it creates us
 */
void CHexMergeDoc::SetDirDoc(CDirDoc * pDirDoc)
{
	ASSERT(pDirDoc && !m_pDirDoc);
	m_pDirDoc = pDirDoc;
}

/**
 * @brief Return pointer to parent frame
 */
CHexMergeFrame * CHexMergeDoc::GetParentFrame() 
{
	return static_cast<CHexMergeFrame *>(m_pView[MERGE_VIEW_LEFT]->GetParentFrame()); 
}

/**
 * @brief DirDoc is closing
 */
void CHexMergeDoc::DirDocClosing(CDirDoc * pDirDoc)
{
	ASSERT(m_pDirDoc == pDirDoc);
	m_pDirDoc = 0;
}

/**
 * @brief DirDoc commanding us to close
 */
BOOL CHexMergeDoc::CloseNow()
{
	// Allow user to cancel closing
	if (!PromptAndSaveIfNeeded(TRUE))
		return FALSE;

	GetParentFrame()->CloseNow();
	return TRUE;
}

/**
* @brief Load one file
*/
HRESULT CHexMergeDoc::LoadOneFile(int index, LPCTSTR filename, BOOL readOnly)
{
	if (Try(m_pView[index]->LoadFile(filename), MB_ICONSTOP) != 0)
		return E_FAIL;
	m_pView[index]->SetReadOnly(readOnly);
	m_filePaths.SetPath(index, filename);
	ASSERT(m_nBufferType[index] == BUFFER_NORMAL); // should have been initialized to BUFFER_NORMAL in constructor
	String strDesc = GetMainFrame()->m_strDescriptions[index];
	if (!strDesc.empty())
	{
		m_strDesc[index] = strDesc;
		m_nBufferType[index] = BUFFER_NORMAL_NAMED;
	}
	UpdateHeaderPath(index);
	m_pView[index]->ResizeWindow();
	return S_OK;
}

/**
 * @brief Load files and initialize frame's compare result icon
 */
HRESULT CHexMergeDoc::OpenDocs(LPCTSTR pathLeft, LPCTSTR pathRight, BOOL bROLeft, BOOL bRORight)
{
	CHexMergeFrame *pf = GetParentFrame();
	ASSERT(pf);
	HRESULT hr;
	if (SUCCEEDED(hr = LoadOneFile(MERGE_VIEW_LEFT, pathLeft, bROLeft)) &&
		SUCCEEDED(hr = LoadOneFile(MERGE_VIEW_RIGHT, pathRight, bRORight)))
	{
		UpdateDiffItem(0);
		pf->Invalidate();
		if (GetOptionsMgr()->GetBool(OPT_SCROLL_TO_FIRST))
			m_pView[MERGE_VIEW_LEFT]->SendMessage(WM_COMMAND, ID_FIRSTDIFF);
	}
	else
	{
		// Use verify macro to trap possible error in debug.
		VERIFY(pf->DestroyWindow());
	}
	return hr;
}

/**
 * @brief Write path and filename to headerbar
 * @note SetText() does not repaint unchanged text
 */
void CHexMergeDoc::UpdateHeaderPath(int pane)
{
	CHexMergeFrame *pf = GetParentFrame();
	ASSERT(pf);
	String sText;

	if (m_nBufferType[pane] == BUFFER_UNNAMED ||
		m_nBufferType[pane] == BUFFER_NORMAL_NAMED)
	{
		sText = m_strDesc[pane];
	}
	else
	{
		sText = m_filePaths.GetPath(pane);
		if (m_pDirDoc)
		{
			if (pane == 0)
				m_pDirDoc->ApplyLeftDisplayRoot(sText);
			else
				m_pDirDoc->ApplyRightDisplayRoot(sText);
		}
	}
	if (m_pView[pane]->GetModified())
		sText.insert(0, _T("* "));
	pf->GetHeaderInterface()->SetText(pane, sText.c_str());

	SetTitle(NULL);
}

/**
 * @brief Update document filenames to title
 */
void CHexMergeDoc::SetTitle(LPCTSTR lpszTitle)
{
	const TCHAR pszSeparator[] = _T(" - ");
	String sTitle;

	if (lpszTitle)
		sTitle = lpszTitle;
	else
	{
		if (!m_strDesc[0].empty())
			sTitle += m_strDesc[0];
		else
		{
			String file;
			String ext;
			SplitFilename(m_filePaths.GetLeft().c_str(), NULL, &file, &ext);
			sTitle += file.c_str();
			if (!ext.empty())
			{
				sTitle += _T(".");
				sTitle += ext.c_str();
			}
		}

		sTitle += pszSeparator;

		if (!m_strDesc[1].empty())
			sTitle += m_strDesc[1];
		else
		{
			String file;
			String ext;
			SplitFilename(m_filePaths.GetRight().c_str(), NULL, &file, &ext);
			sTitle += file.c_str();
			if (!ext.empty())
			{
				sTitle += _T(".");
				sTitle += ext.c_str();
			}
		}
	}
	CDocument::SetTitle(sTitle.c_str());
}

/**
 * @brief We have two child views (left & right), so we keep pointers directly
 * at them (the MFC view list doesn't have them both)
 */
void CHexMergeDoc::SetMergeViews(CHexMergeView * pLeft, CHexMergeView * pRight)
{
	ASSERT(pLeft && !m_pView[MERGE_VIEW_LEFT]);
	m_pView[MERGE_VIEW_LEFT] = pLeft;
	ASSERT(pRight && !m_pView[MERGE_VIEW_RIGHT]);
	m_pView[MERGE_VIEW_RIGHT] = pRight;
}

/**
 * @brief Called when "Save left" item is updated
 */
void CHexMergeDoc::OnUpdateFileSaveLeft(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pView[MERGE_VIEW_LEFT]->GetModified());
}

/**
 * @brief Called when "Save right" item is updated
 */
void CHexMergeDoc::OnUpdateFileSaveRight(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pView[MERGE_VIEW_RIGHT]->GetModified());
}

/**
 * @brief Called when "Save" item is updated
 */
void CHexMergeDoc::OnUpdateFileSave(CCmdUI* pCmdUI)
{
	const BOOL bLModified = m_pView[MERGE_VIEW_LEFT]->GetModified();
	const BOOL bRModified = m_pView[MERGE_VIEW_RIGHT]->GetModified();
	pCmdUI->Enable(bLModified || bRModified);
}

/**
 * @brief Copy selected bytes from source view to destination view
 * @note Grows destination buffer as appropriate
 */
void CHexMergeDoc::CopySel(CHexMergeView *pViewSrc, CHexMergeView *pViewDst)
{
	const IHexEditorWindow::Status *pStatSrc = pViewSrc->GetStatus();
	int i = min(pStatSrc->iStartOfSelection, pStatSrc->iEndOfSelection);
	int j = max(pStatSrc->iStartOfSelection, pStatSrc->iEndOfSelection);
	int u = pViewSrc->GetLength();
	int v = pViewDst->GetLength();
	if (pStatSrc->bSelected && i <= v)
	{
		if (v <= j)
			v = j + 1;
		BYTE *p = pViewSrc->GetBuffer(u);
		BYTE *q = pViewDst->GetBuffer(v);
		memcpy(q + i, p + i, j - i + 1);
		CWnd *pwndFocus = CWnd::GetFocus();
		if (pwndFocus != pViewSrc)
			pViewDst->RepaintRange(i, j);
		if (pwndFocus != pViewDst)
			pViewSrc->RepaintRange(i, j);
		pViewDst->SetModified(TRUE);
	}
}

/**
 * @brief Copy all bytes from source view to destination view
 * @note Grows destination buffer as appropriate
 */
void CHexMergeDoc::CopyAll(CHexMergeView *pViewSrc, CHexMergeView *pViewDst)
{
	if (int i = pViewSrc->GetLength())
	{
		int j = pViewDst->GetLength();
		BYTE *p = pViewSrc->GetBuffer(i);
		BYTE *q = pViewDst->GetBuffer(max(i, j));
		if (q == 0)
			AfxThrowMemoryException();
		memcpy(q, p, i);
		CWnd *pwndFocus = CWnd::GetFocus();
		if (pwndFocus != pViewSrc)
			pViewDst->RepaintRange(0, i);
		if (pwndFocus != pViewDst)
			pViewSrc->RepaintRange(0, i);
		pViewDst->SetModified(TRUE);
	}
}

/**
 * @brief Copy selected bytes from left to right
 */
void CHexMergeDoc::OnL2r()
{
	CopySel(m_pView[MERGE_VIEW_LEFT], m_pView[MERGE_VIEW_RIGHT]);
}

/**
 * @brief Copy selected bytes from right to left
 */
void CHexMergeDoc::OnR2l()
{
	CopySel(m_pView[MERGE_VIEW_RIGHT], m_pView[MERGE_VIEW_LEFT]);
}

/**
 * @brief Copy all bytes from left to right
 */
void CHexMergeDoc::OnAllRight()
{
	CopyAll(m_pView[MERGE_VIEW_LEFT], m_pView[MERGE_VIEW_RIGHT]);
}

/**
 * @brief Copy all bytes from right to left
 */
void CHexMergeDoc::OnAllLeft()
{
	CopyAll(m_pView[MERGE_VIEW_RIGHT], m_pView[MERGE_VIEW_LEFT]);
}

/**
 * @brief Called when user selects View/Zoom In from menu.
 */
void CHexMergeDoc::OnViewZoomIn()
{
	m_pView[MERGE_VIEW_LEFT]->ZoomText(1);
	m_pView[MERGE_VIEW_RIGHT]->ZoomText(1);
}

/**
 * @brief Called when user selects View/Zoom Out from menu.
 */
void CHexMergeDoc::OnViewZoomOut()
{
	m_pView[MERGE_VIEW_LEFT]->ZoomText(-1);
	m_pView[MERGE_VIEW_RIGHT]->ZoomText(-1);
}

/**
 * @brief Called when user selects View/Zoom Normal from menu.
 */
void CHexMergeDoc::OnViewZoomNormal()
{
	m_pView[MERGE_VIEW_LEFT]->ZoomText(0);
	m_pView[MERGE_VIEW_RIGHT]->ZoomText(0);
}
