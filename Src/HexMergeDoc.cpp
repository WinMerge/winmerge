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
// $Id: HexMergeDoc.cpp 6836 2009-06-09 22:01:48Z kimmov $

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

int CHexMergeDoc::m_nBuffersTemp = 2;

static void UpdateDiffItem(int nBuffers, DIFFITEM &di, CDiffContext *pCtxt);
static int Try(HRESULT hr, UINT type = MB_OKCANCEL|MB_ICONSTOP);

/**
 * @brief Update diff item
 */
static void UpdateDiffItem(int nBuffers, DIFFITEM &di, CDiffContext *pCtxt)
{
	di.diffcode.diffcode |= DIFFCODE::SIDEFLAGS;
	for (int nBuffer = 0; nBuffer < nBuffers; nBuffer++)
	{
		di.diffFileInfo[nBuffer].ClearPartial();
		di.diffFileInfo[nBuffer].ClearPartial();
		if (!pCtxt->UpdateInfoFromDiskHalf(di, nBuffer))
		{
			if (nBuffer == 0)
				di.diffcode.diffcode &= ~DIFFCODE::FIRST;
			else if (nBuffer == 1)
				di.diffcode.diffcode &= ~DIFFCODE::SECOND;
			else
				di.diffcode.diffcode &= ~DIFFCODE::THIRD;
		}
	}
	// 1. Clear flags
	di.diffcode.diffcode &= ~(DIFFCODE::TEXTFLAGS | DIFFCODE::COMPAREFLAGS);
	// 2. Process unique files
	// We must compare unique files to itself to detect encoding
	if (!di.diffcode.isExistsFirst() || !di.diffcode.isExistsSecond() || (nBuffers == 3 && di.diffcode.isExistsThird()))
	{
		int compareMethod = pCtxt->GetCompareMethod();
		if (compareMethod != CMP_DATE && compareMethod != CMP_DATE_SIZE &&
			compareMethod != CMP_SIZE)
		{
			di.diffcode.diffcode |= DIFFCODE::SAME;
			FolderCmp folderCmp;
			int diffCode = folderCmp.prepAndCompareFiles(pCtxt, di);
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
		di.diffcode.diffcode |= folderCmp.prepAndCompareFiles(pCtxt, di);
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
	m_nBuffers = m_nBuffersTemp;
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		m_pView[nBuffer] = NULL;
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
			::UpdateDiffItem(m_nBuffers, di, &ctxt);
		}
	}
	BOOL bDiff = FALSE;
	int lengthFirst = m_pView[0]->GetLength();
	void *bufferFirst = m_pView[0]->GetBuffer(lengthFirst);
	for (int nBuffer = 1; nBuffer < m_nBuffers; nBuffer++)
	{
		int length = m_pView[nBuffer]->GetLength();
		if (lengthFirst != length)
			bDiff = TRUE;
		else
		{
			void *buffer = m_pView[nBuffer]->GetBuffer(length);
			bDiff = (memcmp(bufferFirst, buffer, lengthFirst) != 0);
		}
		if (bDiff)
			break;
	}
	GetParentFrame()->SetLastCompareResult(bDiff);
}

/**
 * @brief Asks and then saves modified files
 */
BOOL CHexMergeDoc::PromptAndSaveIfNeeded(BOOL bAllowCancel)
{
	BOOL bLModified = FALSE, bMModified = FALSE, bRModified = FALSE;

	if (m_nBuffers == 3)
	{
		bLModified = m_pView[0]->GetModified();
		bMModified = m_pView[1]->GetModified();
		bRModified = m_pView[2]->GetModified();
	}
	else
	{
		bLModified = m_pView[0]->GetModified();
		bRModified = m_pView[1]->GetModified();
	}
	if (!bLModified && !bMModified && !bRModified)
		 return TRUE;

	const String &pathLeft = m_filePaths.GetLeft();
	const String &pathMiddle = m_filePaths.GetMiddle();
	const String &pathRight = m_filePaths.GetRight();

	BOOL result = TRUE;
	BOOL bLSaveSuccess = FALSE, bMSaveSuccess = FALSE, bRSaveSuccess = FALSE;

	SaveClosingDlg dlg;
	dlg.DoAskFor(bLModified, bMModified, bRModified);
	if (!bAllowCancel)
		dlg.m_bDisableCancel = TRUE;
	if (!pathLeft.empty())
		dlg.m_sLeftFile = pathLeft.c_str();
	else
		dlg.m_sLeftFile = m_strDesc[0].c_str();
	if (m_nBuffers == 3)
	{
		if (!pathMiddle.empty())
			dlg.m_sMiddleFile = pathMiddle.c_str();
		else
			dlg.m_sMiddleFile = m_strDesc[1].c_str();
	}
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
				switch (Try(m_pView[0]->SaveFile(pathLeft.c_str())))
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
				m_pView[0]->SetModified(FALSE);
			}
		}
		if (bMModified)
		{
			if (dlg.m_middleSave == SaveClosingDlg::SAVECLOSING_SAVE)
			{
				switch (Try(m_pView[1]->SaveFile(pathMiddle.c_str())))
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
				m_pView[1]->SetModified(FALSE);
			}
		}
		if (bRModified)
		{
			if (dlg.m_rightSave == SaveClosingDlg::SAVECLOSING_SAVE)
			{
				switch (Try(m_pView[m_nBuffers - 1]->SaveFile(pathRight.c_str())))
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
				m_pView[m_nBuffers - 1]->SetModified(FALSE);
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
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		if (m_pView[nBuffer]->GetModified())
		{
			const String &path = m_filePaths.GetPath(nBuffer);
			if (Try(m_pView[nBuffer]->SaveFile(path.c_str())) == IDCANCEL)
				return;
			bUpdate = TRUE;
		}
	}
	if (bUpdate)
		UpdateDiffItem(m_pDirDoc);
}

void CHexMergeDoc::DoFileSave(int nBuffer)
{
	if (m_pView[nBuffer]->GetModified())
	{
		const String &path = m_filePaths.GetPath(nBuffer);
		if (Try(m_pView[nBuffer]->SaveFile(path.c_str())) == IDCANCEL)
			return;
		UpdateDiffItem(m_pDirDoc);
	}
}

void CHexMergeDoc::DoFileSaveAs(int nBuffer)
{
	const String &path = m_filePaths.GetPath(nBuffer);
	CString strPath;
	int id;
	if (nBuffer == 0)
		id = IDS_SAVE_LEFT_AS;
	else if (nBuffer == m_nBuffers - 1)
		id = IDS_SAVE_RIGHT_AS;
	else
		id = IDS_SAVE_MIDDLE_AS;
	if (SelectFile(0, strPath, path.c_str(), id, NULL, FALSE))
	{
		if (Try(m_pView[nBuffer]->SaveFile(strPath)) == IDCANCEL)
			return;
		m_filePaths.SetPath(nBuffer, strPath);
		UpdateDiffItem(m_pDirDoc);
	}
}

/**
 * @brief Saves left-side file
 */
void CHexMergeDoc::OnFileSaveLeft()
{
	DoFileSave(0);
}

/**
 * @brief Saves middle-side file
 */
void CHexMergeDoc::OnFileSaveMiddle()
{
	DoFileSave(1);
}

/**
 * @brief Saves right-side file
 */
void CHexMergeDoc::OnFileSaveRight()
{
	DoFileSave(m_nBuffers - 1);
}

/**
 * @brief Saves left-side file with name asked
 */
void CHexMergeDoc::OnFileSaveAsLeft()
{
	DoFileSaveAs(0);
}

/**
 * @brief Saves right-side file with name asked
 */
void CHexMergeDoc::OnFileSaveAsMiddle()
{
	DoFileSaveAs(1);
}

/**
 * @brief Saves right-side file with name asked
 */
void CHexMergeDoc::OnFileSaveAsRight()
{
	DoFileSaveAs(m_nBuffers - 1);
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
	return static_cast<CHexMergeFrame *>(m_pView[0]->GetParentFrame()); 
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
 * @brief Load files and initialize frame's compare result icon
 */
HRESULT CHexMergeDoc::OpenDocs(const PathContext &paths, BOOL bRO[])
{
	int nBuffer;
	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		String &path = paths.GetPath(nBuffer);
		if (Try(m_pView[nBuffer]->LoadFile(path.c_str()), MB_ICONSTOP) == 0)
		{
			m_pView[nBuffer]->SetReadOnly(bRO[nBuffer]);
			m_filePaths.SetPath(nBuffer, path.c_str());
			m_strDesc[nBuffer] = path;
		}
	}
	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		UpdateHeaderPath(nBuffer);
		m_pView[nBuffer]->ResizeWindow();
	}
	UpdateDiffItem(0);
	if (GetOptionsMgr()->GetBool(OPT_SCROLL_TO_FIRST))
		m_pView[0]->SendMessage(WM_COMMAND, ID_FIRSTDIFF);
	return S_OK;
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
			m_pDirDoc->ApplyDisplayRoot(pane, sText);
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
	String sFileName[3];

	if (lpszTitle)
		sTitle = lpszTitle;
	else
	{
		for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			if (!m_strDesc[nBuffer].empty())
				sFileName[nBuffer] = m_strDesc[nBuffer];
			else
			{
				String file;
				String ext;
				SplitFilename(m_filePaths[nBuffer].c_str(), NULL, &file, &ext);
				sFileName[nBuffer] += file.c_str();
				if (!ext.empty())
				{
					sFileName[nBuffer] += _T(".");
					sFileName[nBuffer] += ext.c_str();
				}
			}
		}
		if (m_nBuffers < 3)
		{
			if (sFileName[0] == sFileName[1])
			{
				sTitle = sFileName[0];
				sTitle += _T(" x 2");
			}
			else
			{
				sTitle = sFileName[0];
				sTitle += pszSeparator;
				sTitle += sFileName[1];
			}
		}
		else
		{
			if (sFileName[0] == sFileName[1] && sFileName[0] == sFileName[2])
			{
				sTitle = sFileName[0];
				sTitle += _T(" x 3");
			}
			else
			{
				sTitle = sFileName[0];
				sTitle += pszSeparator;
				sTitle += sFileName[1];
				sTitle += pszSeparator;
				sTitle += sFileName[2];
			}
		}

	}
	CDocument::SetTitle(sTitle.c_str());
}

/**
 * @brief We have two child views (left & right), so we keep pointers directly
 * at them (the MFC view list doesn't have them both)
 */
void CHexMergeDoc::SetMergeViews(CHexMergeView *pView[])
{
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		ASSERT(pView[nBuffer] && !m_pView[nBuffer]);
		m_pView[nBuffer] = pView[nBuffer];
	}
}

/**
 * @brief Called when "Save left" item is updated
 */
void CHexMergeDoc::OnUpdateFileSaveLeft(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pView[0]->GetModified());
}

/**
 * @brief Called when "Save middle" item is updated
 */
void CHexMergeDoc::OnUpdateFileSaveMiddle(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nBuffers == 3 && m_pView[1]->GetModified());
}

/**
 * @brief Called when "Save right" item is updated
 */
void CHexMergeDoc::OnUpdateFileSaveRight(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pView[m_nBuffers - 1]->GetModified());
}

/**
 * @brief Called when "Save" item is updated
 */
void CHexMergeDoc::OnUpdateFileSave(CCmdUI* pCmdUI)
{
	BOOL bModified;
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		bModified |= m_pView[nBuffer]->GetModified();
	pCmdUI->Enable(bModified);
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
	CopySel(m_pView[0], m_pView[1]);
}

/**
 * @brief Copy selected bytes from right to left
 */
void CHexMergeDoc::OnR2l()
{
	CopySel(m_pView[1], m_pView[0]);
}

/**
 * @brief Copy all bytes from left to right
 */
void CHexMergeDoc::OnAllRight()
{
	CopyAll(m_pView[0], m_pView[1]);
}

/**
 * @brief Copy all bytes from right to left
 */
void CHexMergeDoc::OnAllLeft()
{
	CopyAll(m_pView[1], m_pView[0]);
}

/**
 * @brief Called when user selects View/Zoom In from menu.
 */
void CHexMergeDoc::OnViewZoomIn()
{
	for (int pane = 0; pane < m_nBuffers; pane++)
		m_pView[pane]->ZoomText(1);
}

/**
 * @brief Called when user selects View/Zoom Out from menu.
 */
void CHexMergeDoc::OnViewZoomOut()
{
	for (int pane = 0; pane < m_nBuffers; pane++)
		m_pView[pane]->ZoomText(-1);
}

/**
 * @brief Called when user selects View/Zoom Normal from menu.
 */
void CHexMergeDoc::OnViewZoomNormal()
{
	for (int pane = 0; pane < m_nBuffers; pane++)
		m_pView[pane]->ZoomText(0);
}
