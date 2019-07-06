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

#include "stdafx.h"
#include "HexMergeDoc.h"
#include <afxinet.h>
#include "UnicodeString.h"
#include "HexMergeFrm.h"
#include "HexMergeView.h"
#include "DiffItem.h"
#include "FolderCmp.h"
#include "DiffContext.h"	// FILE_SAME
#include "DirDoc.h"
#include "DirActions.h"
#include "OptionsDef.h"
#include "DiffFileInfo.h"
#include "SaveClosingDlg.h"
#include "DiffList.h"
#include "paths.h"
#include "OptionsMgr.h"
#include "FileOrFolderSelect.h"
#include "DiffWrapper.h"
#include "SyntaxColors.h"
#include "Merge.h"
#include "Constants.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int CHexMergeDoc::m_nBuffersTemp = 2;

static void UpdateDiffItem(int nBuffers, DIFFITEM &di, CDiffContext *pCtxt);
static int Try(HRESULT hr, UINT type = MB_OKCANCEL|MB_ICONSTOP);

/**
 * @brief Update diff item
 */
static void UpdateDiffItem(int nBuffers, DIFFITEM &di, CDiffContext *pCtxt)
{
	di.diffcode.setSideNone();
	for (int nBuffer = 0; nBuffer < nBuffers; nBuffer++)
	{
		di.diffFileInfo[nBuffer].ClearPartial();
		if (pCtxt->UpdateInfoFromDiskHalf(di, nBuffer))
			di.diffcode.diffcode |= DIFFCODE::FIRST << nBuffer;
	}
	// Clear flags
	di.diffcode.diffcode &= ~(DIFFCODE::TEXTFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY);
	// Really compare
	FolderCmp folderCmp(pCtxt);
	di.diffcode.diffcode |= folderCmp.prepAndCompareFiles(di);
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
	ON_COMMAND(ID_RESCAN, OnFileReload)
	ON_COMMAND(ID_L2R, OnL2r)
	ON_COMMAND(ID_R2L, OnR2l)
	ON_COMMAND(ID_COPY_FROM_LEFT, OnCopyFromLeft)
	ON_COMMAND(ID_COPY_FROM_RIGHT, OnCopyFromRight)
	ON_COMMAND(ID_ALL_LEFT, OnAllLeft)
	ON_COMMAND(ID_ALL_RIGHT, OnAllRight)
	ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomOut)
	ON_COMMAND(ID_VIEW_ZOOMNORMAL, OnViewZoomNormal)
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_COMMAND_RANGE(ID_MERGE_COMPARE_TEXT, ID_MERGE_COMPARE_IMAGE, OnFileRecompareAs)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MERGE_COMPARE_TEXT, ID_MERGE_COMPARE_IMAGE, OnUpdateFileRecompareAs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHexMergeDoc construction/destruction

/**
 * @brief Constructor.
 */
CHexMergeDoc::CHexMergeDoc()
: m_pDirDoc(nullptr)
, m_nBuffers(m_nBuffersTemp)
, m_pView{}
, m_nBufferType{BUFFER_NORMAL, BUFFER_NORMAL, BUFFER_NORMAL}
{
	m_filePaths.SetSize(m_nBuffers);
}

/**
 * @brief Destructor.
 *
 * Informs associated dirdoc that mergedoc is closing.
 */
CHexMergeDoc::~CHexMergeDoc()
{	
	if (m_pDirDoc != nullptr)
		m_pDirDoc->MergeDocClosing(this);
}

/**
 * @brief Return active merge edit view (or left one if neither active)
 */
CHexMergeView * CHexMergeDoc::GetActiveMergeView() const
{
	CView * pActiveView = GetParentFrame()->GetActiveView();
	CHexMergeView * pHexMergeView = dynamic_cast<CHexMergeView *>(pActiveView);
	if (pHexMergeView == nullptr)
		pHexMergeView = m_pView[0]; // default to left view (in case some location or detail view active)
	return pHexMergeView;
}

/**
 * @brief Update associated diff item
 */
int CHexMergeDoc::UpdateDiffItem(CDirDoc *pDirDoc)
{
	// If directory compare has results
	if (pDirDoc != nullptr && pDirDoc->HasDiffs())
	{
		CDiffContext &ctxt = pDirDoc->GetDiffContext();
		if (DIFFITEM *pos = FindItemFromPaths(ctxt, m_filePaths))
		{
			DIFFITEM &di = ctxt.GetDiffRefAt(pos);
			::UpdateDiffItem(m_nBuffers, di, &ctxt);
		}
	}
	bool bDiff = false;
	int lengthFirst = m_pView[0]->GetLength();
	void *bufferFirst = m_pView[0]->GetBuffer(lengthFirst);
	for (int nBuffer = 1; nBuffer < m_nBuffers; nBuffer++)
	{
		int length = m_pView[nBuffer]->GetLength();
		if (lengthFirst != length)
			bDiff = true;
		else
		{
			void *buffer = m_pView[nBuffer]->GetBuffer(length);
			bDiff = (memcmp(bufferFirst, buffer, lengthFirst) != 0);
		}
		if (bDiff)
			break;
	}
	GetParentFrame()->SetLastCompareResult(bDiff);
	return bDiff ? 1 : 0;
}

/**
 * @brief Asks and then saves modified files
 */
bool CHexMergeDoc::PromptAndSaveIfNeeded(bool bAllowCancel)
{
	bool bLModified = false, bMModified = false, bRModified = false;

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
		 return true;

	const String &pathLeft = m_filePaths.GetLeft();
	const String &pathMiddle = m_filePaths.GetMiddle();
	const String &pathRight = m_filePaths.GetRight();

	bool result = true;
	bool bLSaveSuccess = false, bMSaveSuccess = false, bRSaveSuccess = false;

	SaveClosingDlg dlg;
	dlg.DoAskFor(bLModified, bMModified, bRModified);
	if (!bAllowCancel)
		dlg.m_bDisableCancel = true;
	if (!pathLeft.empty())
		dlg.m_sLeftFile = pathLeft;
	else
		dlg.m_sLeftFile = m_strDesc[0];
	if (m_nBuffers == 3)
	{
		if (!pathMiddle.empty())
			dlg.m_sMiddleFile = pathMiddle;
		else
			dlg.m_sMiddleFile = m_strDesc[1];
	}
	if (!pathRight.empty())
		dlg.m_sRightFile = pathRight;
	else
		dlg.m_sRightFile = m_strDesc[1];

	if (dlg.DoModal() == IDOK)
	{
		if (bLModified)
		{
			if (dlg.m_leftSave == SaveClosingDlg::SAVECLOSING_SAVE)
			{
				switch (Try(m_pView[0]->SaveFile(pathLeft.c_str())))
				{
				case 0:
					bLSaveSuccess = true;
					break;
				case IDCANCEL:
					result = false;
					break;
				}
			}
			else
			{
				m_pView[0]->SetSavePoint();
			}
		}
		if (bMModified)
		{
			if (dlg.m_middleSave == SaveClosingDlg::SAVECLOSING_SAVE)
			{
				switch (Try(m_pView[1]->SaveFile(pathMiddle.c_str())))
				{
				case 0:
					bMSaveSuccess = true;
					break;
				case IDCANCEL:
					result = false;
					break;
				}
			}
			else
			{
				m_pView[1]->SetSavePoint();
			}
		}
		if (bRModified)
		{
			if (dlg.m_rightSave == SaveClosingDlg::SAVECLOSING_SAVE)
			{
				switch (Try(m_pView[m_nBuffers - 1]->SaveFile(pathRight.c_str())))
				{
				case 0:
					bRSaveSuccess = true;
					break;
				case IDCANCEL:
					result = false;
					break;
				}
			}
			else
			{
				m_pView[m_nBuffers - 1]->SetSavePoint();
			}
		}
	}
	else
	{	
		result = false;
	}

	// If file were modified and saving was successfull,
	// update status on dir view
	if (bLSaveSuccess || bMSaveSuccess || bRSaveSuccess)
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
	return PromptAndSaveIfNeeded(true);
}

/**
 * @brief Saves both files
 */
void CHexMergeDoc::OnFileSave() 
{
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		DoFileSave(nBuffer);
}

void CHexMergeDoc::DoFileSave(int nBuffer)
{
	if (m_pView[nBuffer]->GetModified())
	{
		if (m_nBufferType[nBuffer] == BUFFER_UNNAMED)
			DoFileSaveAs(nBuffer);
		else
		{
			const String &path = m_filePaths.GetPath(nBuffer);
			if (Try(m_pView[nBuffer]->SaveFile(path.c_str())) == IDCANCEL)
				return;
		}
		UpdateDiffItem(m_pDirDoc);
	}
}

void CHexMergeDoc::DoFileSaveAs(int nBuffer)
{
	const String &path = m_filePaths.GetPath(nBuffer);
	String strPath;
	String title;
	if (nBuffer == 0)
		title = _("Save Left File As");
	else if (nBuffer == m_nBuffers - 1)
		title = _("Save Right File As");
	else
		title = _("Save Middle File As");
	if (SelectFile(AfxGetMainWnd()->GetSafeHwnd(), strPath, false, path.c_str(), title))
	{
		if (Try(m_pView[nBuffer]->SaveFile(strPath.c_str())) == IDCANCEL)
			return;
		if (path.empty())
		{
			// We are saving scratchpad (unnamed file)
			m_nBufferType[nBuffer] = BUFFER_UNNAMED_SAVED;
			m_strDesc[nBuffer].erase();
		}

		m_filePaths.SetPath(nBuffer, strPath);
		UpdateDiffItem(m_pDirDoc);
		UpdateHeaderPath(nBuffer);
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
	ASSERT(pDirDoc != nullptr && m_pDirDoc == nullptr);
	m_pDirDoc = pDirDoc;
}

/**
 * @brief Return pointer to parent frame
 */
CHexMergeFrame * CHexMergeDoc::GetParentFrame() const
{
	return static_cast<CHexMergeFrame *>(m_pView[0]->GetParentFrame()); 
}

/**
 * @brief DirDoc is closing
 */
void CHexMergeDoc::DirDocClosing(CDirDoc * pDirDoc)
{
	ASSERT(m_pDirDoc == pDirDoc);
	m_pDirDoc = nullptr;
}

/**
 * @brief DirDoc commanding us to close
 */
bool CHexMergeDoc::CloseNow()
{
	// Allow user to cancel closing
	if (!PromptAndSaveIfNeeded(true))
		return false;

	GetParentFrame()->CloseNow();
	return true;
}

/**
* @brief Load one file
*/
HRESULT CHexMergeDoc::LoadOneFile(int index, LPCTSTR filename, bool readOnly, const String& strDesc)
{
	if (filename[0])
	{
		if (Try(m_pView[index]->LoadFile(filename), MB_ICONSTOP) != 0)
			return E_FAIL;
		m_pView[index]->SetReadOnly(readOnly);
		m_filePaths.SetPath(index, filename);
		ASSERT(m_nBufferType[index] == BUFFER_NORMAL); // should have been initialized to BUFFER_NORMAL in constructor
		if (!strDesc.empty())
		{
			m_strDesc[index] = strDesc;
			m_nBufferType[index] = BUFFER_NORMAL_NAMED;
		}
	}
	else
	{
		m_nBufferType[index] = BUFFER_UNNAMED;
		m_strDesc[index] = strDesc;
	}
	UpdateHeaderPath(index);
	m_pView[index]->ResizeWindow();
	return S_OK;
}

/**
 * @brief Load files and initialize frame's compare result icon
 */
bool CHexMergeDoc::OpenDocs(int nFiles, const FileLocation fileloc[], const bool bRO[], const String strDesc[])
{
	CHexMergeFrame *pf = GetParentFrame();
	ASSERT(pf != nullptr);
	bool bSucceeded = true;
	int nBuffer;
	for (nBuffer = 0; nBuffer < nFiles; nBuffer++)
	{
		if (FAILED(LoadOneFile(nBuffer, fileloc[nBuffer].filepath.c_str(), bRO[nBuffer], strDesc ? strDesc[nBuffer] : _T(""))))
		{
			bSucceeded = false;
			break;
		}
	}
	if (nBuffer == nFiles)
	{
		// An extra ResizeWindow() on the left view aligns scroll ranges, and
		// also triggers initial diff coloring by invalidating the client area.
		m_pView[0]->ResizeWindow();

		OnRefresh();
	}
	else
	{
		// Use verify macro to trap possible error in debug.
		VERIFY(pf->DestroyWindow());
	}
	return bSucceeded;
}

void CHexMergeDoc::MoveOnLoad(int nPane, int)
{
	if (nPane < 0)
	{
		nPane = GetOptionsMgr()->GetInt(OPT_ACTIVE_PANE);
		if (nPane < 0 || nPane >= m_nBuffers)
			nPane = 0;
	}

	GetParentFrame()->SetActivePane(nPane);

	if (GetOptionsMgr()->GetBool(OPT_SCROLL_TO_FIRST))
		m_pView[0]->SendMessage(WM_COMMAND, ID_FIRSTDIFF);
}

void CHexMergeDoc::CheckFileChanged(void)
{
	for (int pane = 0; pane < m_nBuffers; ++pane)
	{
		if (m_pView[pane]->IsFileChangedOnDisk(m_filePaths[pane].c_str()))
		{
			String msg = strutils::format_string1(_("Another application has updated file\n%1\nsince WinMerge scanned it last time.\n\nDo you want to reload the file?"), m_filePaths[pane]);
			if (AfxMessageBox(msg.c_str(), MB_YESNO | MB_ICONWARNING) == IDYES)
			{
				OnFileReload();
			}
			break;
		}
	}
}

/**
 * @brief Write path and filename to headerbar
 * @note SetText() does not repaint unchanged text
 */
void CHexMergeDoc::UpdateHeaderPath(int pane)
{
	CHexMergeFrame *pf = GetParentFrame();
	ASSERT(pf != nullptr);
	String sText;

	if (m_nBufferType[pane] == BUFFER_UNNAMED ||
		m_nBufferType[pane] == BUFFER_NORMAL_NAMED)
	{
		sText = m_strDesc[pane];
	}
	else
	{
		sText = m_filePaths.GetPath(pane);
		if (m_pDirDoc != nullptr)
			m_pDirDoc->ApplyDisplayRoot(pane, sText);
	}
	if (m_pView[pane]->GetModified())
		sText.insert(0, _T("* "));
	pf->GetHeaderInterface()->SetText(pane, sText);

	SetTitle(nullptr);
}


/**
 * @brief Customize a heksedit control's settings
 */
static void Customize(IHexEditorWindow::Settings *settings)
{
	settings->bSaveIni = false;
	//settings->iAutomaticBPL = false;
	//settings->iBytesPerLine = 16;
	//settings->iFontSize = 8;
}

/**
 * @brief Customize a heksedit control's colors
 */
static void Customize(IHexEditorWindow::Colors *colors)
{
	COptionsMgr *pOptionsMgr = GetOptionsMgr();
	colors->iSelBkColorValue = RGB(224, 224, 224);
	colors->iDiffBkColorValue = pOptionsMgr->GetInt(OPT_CLR_DIFF);
	colors->iSelDiffBkColorValue = pOptionsMgr->GetInt(OPT_CLR_SELECTED_DIFF);
	colors->iDiffTextColorValue = pOptionsMgr->GetInt(OPT_CLR_DIFF_TEXT);
	if (colors->iDiffTextColorValue == 0xFFFFFFFF)
		colors->iDiffTextColorValue = 0;
	colors->iSelDiffTextColorValue = pOptionsMgr->GetInt(OPT_CLR_SELECTED_DIFF_TEXT);
	if (colors->iSelDiffTextColorValue == 0xFFFFFFFF)
		colors->iSelDiffTextColorValue = 0;
	SyntaxColors *pSyntaxColors = theApp.GetMainSyntaxColors();
	colors->iTextColorValue = pSyntaxColors->GetColor(COLORINDEX_NORMALTEXT);
	colors->iBkColorValue = pSyntaxColors->GetColor(COLORINDEX_BKGND);
	colors->iSelTextColorValue = pSyntaxColors->GetColor(COLORINDEX_SELTEXT);
	colors->iSelBkColorValue = pSyntaxColors->GetColor(COLORINDEX_SELBKGND);
}

/**
 * @brief Customize a heksedit control's settings and colors
 */
static void Customize(IHexEditorWindow *pif)
{
	Customize(pif->get_settings());
	Customize(pif->get_colors());
	//LANGID wLangID = (LANGID)GetThreadLocale();
	//pif->load_lang(wLangID);
}

void CHexMergeDoc::RefreshOptions()
{
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		IHexEditorWindow *pif = m_pView[nBuffer]->GetInterface();
		pif->read_ini_data();
		Customize(pif);
		pif->resize_window();
	}
}

/**
 * @brief Update document filenames to title
 */
void CHexMergeDoc::SetTitle(LPCTSTR lpszTitle)
{
	String sTitle;
	String sFileName[3];

	if (lpszTitle != nullptr)
		sTitle = lpszTitle;
	else
	{
		for (int nBuffer = 0; nBuffer < m_filePaths.GetSize(); nBuffer++)
			sFileName[nBuffer] = !m_strDesc[nBuffer].empty() ? m_strDesc[nBuffer] : paths::FindFileName(m_filePaths[nBuffer]);
		if (std::count(&sFileName[0], &sFileName[0] + m_nBuffers, sFileName[0]) == m_nBuffers)
			sTitle = sFileName[0] + strutils::format(_T(" x %d"), m_nBuffers);
		else
			sTitle = strutils::join(&sFileName[0], &sFileName[0] + m_nBuffers, _T(" - "));
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
		ASSERT(pView[nBuffer] != nullptr && m_pView[nBuffer] == nullptr);
		m_pView[nBuffer] = pView[nBuffer];
		m_pView[nBuffer]->m_nThisPane = nBuffer;
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
	bool bModified = false;
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		bModified |= m_pView[nBuffer]->GetModified();
	pCmdUI->Enable(bModified);
}

/**
 * @brief Reloads the opened files
 */
void CHexMergeDoc::OnFileReload()
{
	if (!PromptAndSaveIfNeeded(true))
		return;
	
	FileLocation fileloc[3];
	bool bRO[3];
	for (int pane = 0; pane < m_nBuffers; pane++)
	{
		fileloc[pane].setPath(m_filePaths[pane]);
		bRO[pane] = m_pView[pane]->GetReadOnly();
	}
	OpenDocs(m_nBuffers, fileloc, bRO, m_strDesc);
	MoveOnLoad(GetActiveMergeView()->m_nThisPane);
}

/**
 * @brief Copy selected bytes from left to right
 */
void CHexMergeDoc::OnL2r()
{
	int dstPane = (GetActiveMergeView()->m_nThisPane < m_nBuffers - 1) ? GetActiveMergeView()->m_nThisPane + 1 : m_nBuffers - 1;
	int srcPane = dstPane - 1;
	CHexMergeView::CopySel(m_pView[srcPane], m_pView[dstPane]);
}

/**
 * @brief Copy selected bytes from right to left
 */
void CHexMergeDoc::OnR2l()
{
	int dstPane = (GetActiveMergeView()->m_nThisPane > 0) ? GetActiveMergeView()->m_nThisPane - 1 : 0;
	int srcPane = dstPane + 1;
	CHexMergeView::CopySel(m_pView[srcPane], m_pView[dstPane]);
}

/**
 * @brief Copy selected bytes from left to active pane
 */
void CHexMergeDoc::OnCopyFromLeft()
{
	int dstPane = GetActiveMergeView()->m_nThisPane;
	int srcPane = (dstPane - 1 < 0) ? 0 : dstPane - 1;
	CHexMergeView::CopySel(m_pView[srcPane], m_pView[dstPane]);
}

/**
 * @brief Copy selected bytes from right to active pane
 */
void CHexMergeDoc::OnCopyFromRight()
{
	int dstPane = GetActiveMergeView()->m_nThisPane;
	int srcPane = (dstPane + 1 > m_nBuffers - 1) ? m_nBuffers - 1 : dstPane + 1;
	CHexMergeView::CopySel(m_pView[srcPane], m_pView[dstPane]);
}

/**
 * @brief Copy all bytes from left to right
 */
void CHexMergeDoc::OnAllRight()
{
	int dstPane = (GetActiveMergeView()->m_nThisPane < m_nBuffers - 1) ? GetActiveMergeView()->m_nThisPane + 1 : m_nBuffers - 1;
	int srcPane = dstPane - 1;
	CHexMergeView::CopyAll(m_pView[srcPane], m_pView[dstPane]);
}

/**
 * @brief Copy all bytes from right to left
 */
void CHexMergeDoc::OnAllLeft()
{
	int dstPane = (GetActiveMergeView()->m_nThisPane > 0) ? GetActiveMergeView()->m_nThisPane - 1 : 0;
	int srcPane = dstPane + 1;
	CHexMergeView::CopyAll(m_pView[srcPane], m_pView[dstPane]);
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

void CHexMergeDoc::OnRefresh()
{
	if (UpdateDiffItem(m_pDirDoc) == 0)
		LangMessageBox(IDS_FILESSAME, MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN);
}

void CHexMergeDoc::OnFileRecompareAs(UINT nID)
{
	FileLocation fileloc[3];
	DWORD dwFlags[3];
	String strDesc[3];
	int nBuffers = m_nBuffers;
	CDirDoc *pDirDoc = m_pDirDoc->GetMainView() ? m_pDirDoc : 
		static_cast<CDirDoc*>(theApp.m_pDirTemplate->CreateNewDocument());
	for (int nBuffer = 0; nBuffer < nBuffers; ++nBuffer)
	{
		fileloc[nBuffer].setPath(m_filePaths[nBuffer]);
		dwFlags[nBuffer] = m_pView[nBuffer]->GetReadOnly() ? FFILEOPEN_READONLY : 0;
		strDesc[nBuffer] = m_strDesc[nBuffer];
	}
	CloseNow();
	if (nID == ID_MERGE_COMPARE_TEXT)
		GetMainFrame()->ShowMergeDoc(pDirDoc, nBuffers, fileloc, dwFlags, strDesc);
	else
		GetMainFrame()->ShowImgMergeDoc(pDirDoc, nBuffers, fileloc, dwFlags, strDesc);
}

void CHexMergeDoc::OnUpdateFileRecompareAs(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(pCmdUI->m_nID != ID_MERGE_COMPARE_XML);
}

