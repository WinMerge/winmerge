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
 * @file  DirDoc.cpp
 *
 * @brief Implementation file for CDirDoc
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$
//

#include "stdafx.h"
#include <Shlwapi.h>		// PathCompactPathEx()
#include "Merge.h"
#include "DirDoc.h"
#include "DirFrame.h"
#include "diff.h"
#include "getopt.h"
#include "fnmatch.h"
#include "MainFrm.h"
#include "coretools.h"
#include "logfile.h"
#include "paths.h"
#include "WaitStatusCursor.h"
#include "7zCommon.h"
#include "OptionsDef.h"
#include "dllver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CLogFile gLog;

/////////////////////////////////////////////////////////////////////////////
// CDirDoc

IMPLEMENT_DYNCREATE(CDirDoc, CDocument)

/**
 * @brief Constructor.
 */
CDirDoc::CDirDoc()
{
	DIFFOPTIONS options = {0};

	m_pDirView = NULL;
	m_pCtxt=NULL;
	m_bReuseMergeDocs = TRUE;
	m_bROLeft = FALSE;
	m_bRORight = FALSE;
	m_bRecursive = FALSE;
	m_statusCursor = NULL;
	m_bReuseCloses = FALSE;

	m_diffWrapper.SetDetectMovedBlocks(mf->m_options.GetBool(OPT_CMP_MOVED_BLOCKS));
	options.nIgnoreWhitespace = mf->m_options.GetInt(OPT_CMP_IGNORE_WHITESPACE);
	options.bIgnoreBlankLines = mf->m_options.GetBool(OPT_CMP_IGNORE_BLANKLINES);
	options.bIgnoreCase = mf->m_options.GetBool(OPT_CMP_IGNORE_CASE);
	options.bEolSensitive = mf->m_options.GetBool(OPT_CMP_EOL_SENSITIVE);

	m_diffWrapper.SetOptions(&options);
}

/**
 * @brief Destructor.
 *
 * Clears document list and deleted possible archive-temp files.
 */
CDirDoc::~CDirDoc()
{
	delete m_pCtxt;
	// Inform all of our merge docs that we're closing
	for (POSITION pos = m_MergeDocs.GetHeadPosition(); pos; )
	{
		CMergeDoc * pMergeDoc = m_MergeDocs.GetNext(pos);
		pMergeDoc->DirDocClosing(this);
	}
	
	// Delete all temporary folders belonging to this document
	CTempPath(this);
}

/**
 * @brief Callback we give our frame which allows us to control whether
 * or not it closes.
 */
static bool DocClosableCallback(void * param)
{
	CDirDoc * pDoc = reinterpret_cast<CDirDoc *>(param);
	return pDoc->CanFrameClose();
}

/**
 * @brief Checks if there are mergedocs associated with this dirdoc.
 */
bool CDirDoc::CanFrameClose()
{
	return !!m_MergeDocs.IsEmpty();
}

/**
 * @brief Called when new dirdoc is created.
 */
BOOL CDirDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CString s;
	VERIFY(s.LoadString(IDS_DIRECTORY_WINDOW_TITLE));
	SetTitle(s);

	void * param = reinterpret_cast<void *>(this);
	GetMainView()->GetParentFrame()->SetClosableCallback(&DocClosableCallback, param);

	return TRUE;
}


BEGIN_MESSAGE_MAP(CDirDoc, CDocument)
	//{{AFX_MSG_MAP(CDirDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDirDoc serialization

void CDirDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDirDoc commands

/**
 * @brief Perform directory comparison again from scratch
 */
void CDirDoc::Rescan()
{
	if (!m_pCtxt) return;

	CDirFrame *pf = m_pDirView->GetParentFrame();

	// If we're already doing a rescan, bail out
	UINT threadState = m_diffThread.GetThreadState();
	if (threadState == THREAD_COMPARING)
		return;

	m_statusCursor = new CustomStatusCursor(0, IDC_APPSTARTING, LoadResString(IDS_STATUS_RESCANNING));

	gLog.Write(LOGLEVEL::LNOTICE, _T("Starting directory scan:\n\tLeft: %s\n\tRight: %s\n"),
			m_pCtxt->m_strLeft, m_pCtxt->m_strRight);
	pf->clearStatus();
	pf->ShowProcessingBar(TRUE);
	m_pCtxt->RemoveAll();

	// fix up for diff code (remove trailing slashes etc)
	m_pCtxt->m_strNormalizedLeft = m_pCtxt->m_strLeft;
	m_pCtxt->m_strNormalizedRight = m_pCtxt->m_strRight;
	m_pCtxt->m_hDirFrame = pf->GetSafeHwnd();
	m_pCtxt->m_msgUpdateStatus = MSG_STAT_UPDATE;
	m_pCtxt->m_bGuessEncoding = mf->m_options.GetBool(OPT_CP_DETECT);
	paths_normalize(m_pCtxt->m_strNormalizedLeft);
	paths_normalize(m_pCtxt->m_strNormalizedRight);
	UpdateHeaderPath(TRUE);
	UpdateHeaderPath(FALSE);
	// draw the headers as active ones
	pf->GetHeaderInterface()->SetActive(0, TRUE);
	pf->GetHeaderInterface()->SetActive(1, TRUE);

	// Make sure filters are up-to-date
	theApp.m_globalFileFilter.ReloadUpdatedFilters();
	m_pCtxt->m_piFilterGlobal = &theApp.m_globalFileFilter;

	// Show active filter name in statusbar
	pf->SetFilter(theApp.m_globalFileFilter.GetFilter());

	// Empty display before new compare
	m_pDirView->DeleteAllDisplayItems();

	m_diffWrapper.StartDirectoryDiff();
	
	m_diffThread.SetContext(m_pCtxt);
	m_diffThread.SetHwnd(m_pDirView->GetSafeHwnd());
	m_diffThread.SetMessageIDs(MSG_UI_UPDATE, MSG_STAT_UPDATE);
	m_diffThread.CompareDirectories(m_pCtxt->m_strNormalizedLeft,
			m_pCtxt->m_strNormalizedRight, m_bRecursive);
}

/**
 * @brief Determines if user wants to see this item
 * @param [in] pCtxt CDiffContext containing filedata
 * @param [in] di Item tested
 * @param [in] llen Lenght of normalised left path
 * @param [in] rlen Lenght of normalised right path
 * @return File- or subfolder name of item, NULL if user does not want to see it
 * @sa CDirDoc::Redisplay()
 */
LPCTSTR CDirDoc::GetItemPathIfShowable(CDiffContext *pCtxt, const DIFFITEM & di, int llen, int rlen)
{
	if (di.isResultFiltered())
	{
		// Treat SKIPPED as a 'super'-flag. If item is skipped and user
		// wants to see skipped items show item regardless of other flags
		if (mf->m_options.GetBool(OPT_SHOW_SKIPPED))
			goto ShowItem;
		else
			return 0;
	}

	// Subfolders in non-recursive compare can only be skipped or unique
	if (!m_bRecursive && di.isDirectory())
	{
		// result filters
		if (di.isResultError() && !mf->m_bShowErrors)
			return 0;

		// left/right filters
		if (di.isSideLeft() && !mf->m_options.GetBool(OPT_SHOW_UNIQUE_LEFT))
			return 0;
		if (di.isSideRight() && !mf->m_options.GetBool(OPT_SHOW_UNIQUE_RIGHT))
			return 0;
	}
	else
	{
		// file type filters
		if (di.isBin() && !mf->m_options.GetBool(OPT_SHOW_BINARIES))
			return 0;

		// result filters
		if (di.isResultSame() && !mf->m_options.GetBool(OPT_SHOW_IDENTICAL))
			return 0;
		if (di.isResultError() && !mf->m_bShowErrors)
			return 0;
		if (di.isResultDiff() && !mf->m_options.GetBool(OPT_SHOW_DIFFERENT))
			return 0;

		// left/right filters
		if (di.isSideLeft() && !mf->m_options.GetBool(OPT_SHOW_UNIQUE_LEFT))
			return 0;
		if (di.isSideRight() && !mf->m_options.GetBool(OPT_SHOW_UNIQUE_RIGHT))
			return 0;
	}

ShowItem:
	LPCTSTR p = NULL;
	if (di.isSideRight())
		p = _tcsninc(di.getRightFilepath(pCtxt), rlen);
	else
		p = _tcsninc(di.getLeftFilepath(pCtxt), llen);

	return p;
}

/**
 * @brief Empty & reload listview (of files & columns) with comparison results
 * @todo Better solution for special items ("..")?
 */
void CDirDoc::Redisplay()
{
	m_pDirView->ToDoDeleteThisValidateColumnOrdering();

	if (m_pCtxt == NULL)
		return;

	CString s,s2;
	UINT cnt=0;
	int llen = m_pCtxt->m_strNormalizedLeft.GetLength();
	int rlen = m_pCtxt->m_strNormalizedRight.GetLength();

	m_pDirView->DeleteAllDisplayItems();
	
	// If non-recursive compare, add special item(s)
	if (!m_bRecursive)
		cnt += m_pDirView->AddSpecialItems();

	POSITION diffpos = m_pCtxt->GetFirstDiffPosition();
	while (diffpos)
	{
		POSITION curdiffpos = diffpos;
		DIFFITEM di = m_pCtxt->GetNextDiffPosition(diffpos);

		LPCTSTR p=GetItemPathIfShowable(m_pCtxt, di, llen, rlen);

		if (p)
		{
			int i = m_pDirView->AddDiffItem(cnt, di, p, curdiffpos);
			m_pDirView->UpdateDiffItemStatus(i, di);
			cnt++;
		}
	}
	m_pDirView->SortColumnsAppropriately();
}

CDirView * CDirDoc::GetMainView()
{
	// why are we doing this ? dirdocs only have one view

	POSITION pos = GetFirstViewPosition(), ps2=pos;

	while (pos != NULL)
	{
		CDirView* pView = (CDirView*)GetNextView(pos);
		if (pView->IsKindOf( RUNTIME_CLASS(CDirView)))
			return pView;
		else
		{
			ASSERT(0);
		}
	}
	return (CDirView*)GetNextView(ps2);
}

/**
 * @brief Update in-memory diffitem status from disk and update view
 * @param nIdx Index of item in UI list
 */
void CDirDoc::ReloadItemStatus(UINT nIdx, BOOL bLeft, BOOL bRight)
{
	// Get position of item in DiffContext
	POSITION diffpos = m_pDirView->GetItemKey(nIdx);

	// in case just copied (into existence) or modified
	m_pCtxt->UpdateStatusFromDisk(diffpos, bLeft, bRight);

	// Update view
	DIFFITEM & updated = m_pCtxt->GetDiffAt(diffpos);
	m_pDirView->UpdateDiffItemStatus(nIdx, updated);
}

void CDirDoc::InitStatusStrings()
{

}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CDirDoc::UpdateResources()
{
	m_pDirView->UpdateResources();

	CString s;
	VERIFY(s.LoadString(IDS_DIRECTORY_WINDOW_TITLE));
	SetTitle(s);

	Redisplay();
}

/**
 * @brief Sets pointer to DiffContext.
 */
void CDirDoc::SetDiffContext(CDiffContext *pCtxt)
{
	if (m_pCtxt != NULL)
		delete m_pCtxt;

	m_pCtxt = pCtxt;

	// All plugin management is done by our plugin manager
	m_pCtxt->m_piPluginInfos = &m_pluginman;
}

/**
 * @brief Find the CDiffContext diffpos of an item from its left & right paths
 * @return POSITION to item, NULL if not found.
 * @note Filenames must be same, if they differ NULL is returned.
 */
POSITION CDirDoc::FindItemFromPaths(LPCTSTR pathLeft, LPCTSTR pathRight)
{
	POSITION pos = m_pCtxt->GetFirstDiffPosition();
	POSITION currentPos;

	CString path1, file1;
	SplitFilename(pathLeft, &path1, &file1, 0);
	CString path2, file2;
	SplitFilename(pathRight, &path2, &file2, 0);

	// Filenames must be identical
	if (file1 != file2)
		return NULL;

	// Path can contain (because of difftools?) '/' and '\'
	// so for comparing purposes, convert whole path to use '\\'
	path1.Replace('/', '\\');
	path2.Replace('/', '\\');


	// Add trailing slash to root paths, to work with getLeftFilepath etc
	if (path1.GetLength() == 2 && path1[1] == ':')
		path1 += '\\';
	if (path2.GetLength() == 2 && path2[1] == ':')
		path2 += '\\';

	// Get first item
	DIFFITEM current = m_pCtxt->GetDiffAt(pos);

	int count = m_pCtxt->GetDiffCount();
	for (int i=0; i < count; ++i)
	{
		// Save our current pos before getting next
		currentPos = pos;
		current = m_pCtxt->GetNextDiffPosition(pos);

		if (path1 == current.getLeftFilepath(m_pCtxt) &&
			path2 == current.getRightFilepath(m_pCtxt) &&
			file1 == current.sfilename)
		{
			return currentPos;
		}
	}
	return NULL;
}

/**
 * @brief Stash away our view pointer.
 */
void CDirDoc::SetDirView(CDirView * newView)
{
	m_pDirView = newView;
	// MFC has a view list for us, so lets check against it
	POSITION pos = GetFirstViewPosition();
	CDirView * temp = static_cast<CDirView *>(GetNextView(pos));
	ASSERT(temp == m_pDirView); // verify that our stashed pointer is the same as MFC's
}

/**
 * @brief A new MergeDoc has been opened.
 */
void CDirDoc::AddMergeDoc(CMergeDoc * pMergeDoc)
{
	ASSERT(pMergeDoc);
	m_MergeDocs.AddTail(pMergeDoc);
}

/**
 * @brief MergeDoc informs us it is closing.
 */
void CDirDoc::MergeDocClosing(CMergeDoc * pMergeDoc)
{
	ASSERT(pMergeDoc);
	POSITION pos = m_MergeDocs.Find(pMergeDoc);
	ASSERT(pos);
	m_MergeDocs.RemoveAt(pos);

	// If dir compare is empty (no compare results) and we are not closing
	// because of reuse close also dir compare
	if (m_pCtxt == NULL && !m_bReuseCloses)
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_CLOSE);
}

/**
 * @brief Close MergeDocs opened from DirDoc.
 *
 * Asks confirmation for docs containing unsaved data and then
 * closes MergeDocs.
 * @return TRUE if success, FALSE if user canceled or closing failed
 */
BOOL CDirDoc::CloseMergeDocs()
{
	for (POSITION pos = m_MergeDocs.GetHeadPosition(); pos; )
	{
		CMergeDoc * pMergeDoc = m_MergeDocs.GetNext(pos);
		if (!pMergeDoc->CloseNow())
			return FALSE;
	}
	return TRUE;
}

/**
 * @brief Prepare for reuse.
 *
 * Close all our merge docs (which gives them chance to save)
 * This may fail if user cancels a Save dialog
 * in which case this aborts and returns FALSE
 */
BOOL CDirDoc::ReusingDirDoc()
{
	m_bReuseCloses = TRUE;

	// Inform all of our merge docs that we're closing
	if (!CloseMergeDocs())
		return FALSE;

	m_bReuseCloses = FALSE;

	// clear diff display
	ASSERT(m_pDirView);
	m_pDirView->DeleteAllDisplayItems();

	// hide the floating state bar
	CDirFrame *pf = m_pDirView->GetParentFrame();
	pf->ShowProcessingBar(FALSE);

	// delete comparison parameters and results
	if (m_pCtxt != NULL)
		delete m_pCtxt;
	m_pCtxt = NULL;

	return TRUE;
}

/**
 * @brief Obtain a merge doc to display a difference in files.
 * @param [out] pNew Set to TRUE if a new doc is created,
 * and FALSE if an existing one reused.
 */
CMergeDoc * CDirDoc::GetMergeDocForDiff(BOOL * pNew)
{
	CMergeDoc * pMergeDoc = 0;
	// policy -- use an existing merge doc if available
	if (m_bReuseMergeDocs && !m_MergeDocs.IsEmpty())
	{
		*pNew = FALSE;
		pMergeDoc = m_MergeDocs.GetHead();
	}
	else
	{
		// Create a new merge doc
		pMergeDoc = (CMergeDoc*)theApp.m_pDiffTemplate->OpenDocumentFile(NULL);
		AddMergeDoc(pMergeDoc);
		pMergeDoc->SetDirDoc(this);
		*pNew = TRUE;
	}
	return pMergeDoc;
}

/**
 * @brief Update changed item's compare status
 * @param [in] pathLeft Left-side path
 * @param [in] pathRight Right-side path
 * @param [in] nDiffs Total amount of differences
 * @param [in] nTrivialDiffs Amount of ignored differences
 * @param [in] bIdentical TRUE if files became identical, FALSE otherwise.
 * @note Filenames must be same, otherwise function asserts.
 */
void CDirDoc::UpdateChangedItem(LPCTSTR pathLeft, LPCTSTR pathRight,
	UINT nDiffs, UINT nTrivialDiffs, BOOL bIdentical)
{
	POSITION pos = FindItemFromPaths(pathLeft, pathRight);
	ASSERT(pos);
	int ind = m_pDirView->GetItemIndex((DWORD)pos);

	// Figure out new status code
	UINT diffcode = (bIdentical ? DIFFCODE::SAME : DIFFCODE::DIFF);

	// Update both views and diff context memory
	SetDiffCompare(diffcode, ind);

	if (nDiffs != -1 && nTrivialDiffs != -1)
		SetDiffCounts(nDiffs, nTrivialDiffs, ind);
	ReloadItemStatus(ind, TRUE, TRUE);
}

/**
 * @brief Cleans up after directory compare
 */
void CDirDoc::CompareReady()
{
	gLog.Write(LOGLEVEL::LNOTICE, _T("Directory scan complete\n"));

	// finish the cursor (the hourglass/pointer combo) we had open during display
	delete m_statusCursor;
	m_statusCursor = NULL;

	m_diffWrapper.EndDirectoryDiff();
}

/**
 * @brief Refresh cached options.
 *
 * For compare speed, we have to cache some frequently needed options,
 * instead of getting option value every time from OptionsMgr. This
 * function must be called every time options are changed to OptionsMgr.
 */
void CDirDoc::RefreshOptions()
{
	DIFFOPTIONS options;

	m_diffWrapper.SetDetectMovedBlocks(mf->m_options.GetBool(OPT_CMP_MOVED_BLOCKS));
	options.nIgnoreWhitespace = mf->m_options.GetInt(OPT_CMP_IGNORE_WHITESPACE);
	options.bIgnoreBlankLines = mf->m_options.GetBool(OPT_CMP_IGNORE_BLANKLINES);
	options.bIgnoreCase = mf->m_options.GetBool(OPT_CMP_IGNORE_CASE);
	options.bEolSensitive = mf->m_options.GetBool(OPT_CMP_EOL_SENSITIVE);

	m_diffWrapper.SetOptions(&options);
	m_pDirView->RefreshOptions();
}

/**
 * @brief Set left/right side readonly-status
 * @param bLeft Select side to set (TRUE = left)
 * @param bReadOnly New status of selected side
 */
void CDirDoc::SetReadOnly(BOOL bLeft, BOOL bReadOnly)
{
	if (bLeft)
		m_bROLeft = bReadOnly;
	else
		m_bRORight = bReadOnly;
}

/**
 * @brief Return left/right side readonly-status
 * @param bLeft Select side to ask (TRUE = left)
 */
BOOL CDirDoc::GetReadOnly(BOOL bLeft) const
{
	if (bLeft)
		return m_bROLeft;
	else
		return m_bRORight;
}

/**
 * @brief Enable/disable recursive directory compare
 */
void CDirDoc::SetRecursive(BOOL bRecursive)
{
	m_bRecursive = bRecursive;
}

/**
 * @brief Set side status of diffitem
 * @note This does not update UI - ReloadItemStatus() does
 * @sa CDirDoc::ReloadItemStatus()
 */
void CDirDoc::SetDiffSide(UINT diffcode, int idx)
{
	SetDiffStatus(diffcode, DIFFCODE::SIDEFLAG, idx);
}

/**
 * @brief Set compare status of diffitem
 * @note This does not update UI - ReloadItemStatus() does
 * @sa CDirDoc::ReloadItemStatus()
 */
void CDirDoc::SetDiffCompare(UINT diffcode, int idx)
{
	SetDiffStatus(diffcode, DIFFCODE::COMPAREFLAGS, idx);
}

/**
 * @brief Set status for diffitem
 * @param diffcode New code
 * @param mask Defines allowed set of flags to change
 * @param idx Item's index to list in UI
 */
void CDirDoc::SetDiffStatus(UINT diffcode, UINT mask, int idx)
{
	// Get position of item in DiffContext 
	POSITION diffpos = m_pDirView->GetItemKey(idx);

	// TODO: Why is the update broken into these pieces ?
	// Someone could figure out these pieces and probably simplify this.

	// Update DIFFITEM code (comparison result) to DiffContext
	m_pCtxt->SetDiffStatusCode(diffpos, diffcode, mask);

	// update DIFFITEM time (and other disk info), and tell views
}

/**
 * @brief Write path and filename to headerbar
 * @note SetText() does not repaint unchanged text
 */
void CDirDoc::UpdateHeaderPath(BOOL bLeft)
{
	CDirFrame *pf = m_pDirView->GetParentFrame();
	ASSERT(pf);
	int nPane = 0;
	CString sText;

	if (bLeft)
	{
		if (!m_strLeftDesc.IsEmpty())
			sText = m_strLeftDesc;
		else
			sText = m_pCtxt->m_strLeft;
		nPane = 0;
	}
	else
	{
		if (!m_strRightDesc.IsEmpty())
			sText = m_strRightDesc;
		else
			sText = m_pCtxt->m_strRight;
		nPane = 1;
	}

	pf->GetHeaderInterface()->SetText(nPane, sText);
}

/**
 * @brief virtual override called just before document is saved and closed
 */
BOOL CDirDoc::SaveModified() 
{
	// Do not allow closing if there is a thread running
	if (m_diffThread.GetThreadState() == THREAD_COMPARING)
		return FALSE;
	
	return CDocument::SaveModified();
}

/**
 * @brief Send signal to thread to stop current scan
 *
 * @todo: Call this from somewhere in GUI, eg, <escape> button
 */
void CDirDoc::AbortCurrentScan()
{
	gLog.Write(LOGLEVEL::LNOTICE, _T("Dircompare aborted!"));
	m_diffThread.Abort();
}

/**
 * @brief Returns true if there is an active scan that hasn't been aborted.
 *
 * @todo: This is for Update command handler of menu item or toolbar button to cancel scan
 */
bool CDirDoc::IsCurrentScanAbortable() const
{
	return (m_diffThread.GetThreadState() == THREAD_COMPARING 
		&& !m_diffThread.IsAborting());
}

/**
 * @brief Set directory description texts shown in headerbar
 */
void CDirDoc::SetDescriptions(CString strLeftDesc, CString strRightDesc)
{
	m_strLeftDesc = strLeftDesc;
	m_strRightDesc = strRightDesc;
}

/**
 * @brief Store a plugin setting for specified file comparison
 */
void CDirDoc::SetPluginPrediffSetting(const CString & filteredFilenames, int newsetting)
{
	m_pluginman.SetPrediffSetting(filteredFilenames, newsetting);
}

/**
 * @brief Retrieve any cached plugin info for specified comparison
 */
void CDirDoc::FetchPluginInfos(const CString& filteredFilenames, 
                               PackingInfo ** infoUnpacker, 
                               PrediffingInfo ** infoPrediffer)
{
	// This will manufacture the needed objects if not already cached
	IPluginInfos * piPluginInfos = &m_pluginman;
	piPluginInfos->FetchPluginInfos(filteredFilenames, infoUnpacker, infoPrediffer);
}

void CDirDoc::SetDiffCounts(UINT diffs, UINT ignored, int idx)
{
	// Get position of item in DiffContext 
	POSITION diffpos = m_pDirView->GetItemKey(idx);

	// Update diff counts
	m_pCtxt->SetDiffCounts(diffpos, diffs, ignored);
}

/**
 * @brief Set document title to given string or items compared.
 */
void CDirDoc::SetTitle(LPCTSTR lpszTitle)
{
	if (!m_pDirView)
		return;

	if (lpszTitle)
		CDocument::SetTitle(lpszTitle);
	else if (!m_pCtxt || m_pCtxt->m_strLeft.IsEmpty() ||
		m_pCtxt->m_strRight.IsEmpty())
	{
		CString title;
		VERIFY(title.LoadString(IDS_DIRECTORY_WINDOW_TITLE));
		CDocument::SetTitle(title);
	}
	else
	{
		// PathCompactPath() supported in versions 4.71 and higher
		if (GetDllVersion(_T("shlwapi.dll")) >= PACKVERSION(4,71))
		{
			// Combine title from file/dir names
			TCHAR *pszLeftFile;
			TCHAR *pszRightFile;
			CString sLeftFile;
			CString sRightFile;
			CRect rcClient;
			CString strTitle;
			const TCHAR strSeparator[] = _T(" - ");
			CClientDC lDC(m_pDirView);
			
			m_pDirView->GetClientRect(&rcClient);
			const DWORD width = rcClient.right / 3;

			sLeftFile = m_pCtxt->m_strLeft;
			pszLeftFile = sLeftFile.GetBuffer(MAX_PATH);

			if (PathCompactPath(lDC.GetSafeHdc(), pszLeftFile, width))
				strTitle = pszLeftFile;
			else
				strTitle = m_pCtxt->m_strLeft;

			sLeftFile.ReleaseBuffer();
			strTitle += strSeparator;

			sRightFile = m_pCtxt->m_strRight;
			pszRightFile = sRightFile.GetBuffer(MAX_PATH);

			if (PathCompactPath(lDC.GetSafeHdc(), pszRightFile, width))
				strTitle += pszRightFile;
			else
				strTitle += m_pCtxt->m_strRight;
			sRightFile.ReleaseBuffer();

			CDocument::SetTitle(strTitle);
		}
		else
		{
			CString title;
			VERIFY(title.LoadString(IDS_DIRECTORY_WINDOW_TITLE));
			CDocument::SetTitle(title);
		}
	}	
}
