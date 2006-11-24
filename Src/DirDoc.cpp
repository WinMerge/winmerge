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
#include <Shlwapi.h>		// PathFindFileName()
#include "Merge.h"
#include "CompareStats.h"
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
#include "FileActionScript.h"

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
: m_pCtxt(NULL)
, m_pDirView(NULL)
, m_pCompareStats(NULL)
, m_bROLeft(FALSE)
, m_bRORight(FALSE)
, m_bRecursive(FALSE)
, m_statusCursor(NULL)
, m_bReuseCloses(FALSE)
, m_bMarkedRescan(FALSE)
, m_pTempPathContext(NULL)
{
	DIFFOPTIONS options = {0};

	m_diffWrapper.SetDetectMovedBlocks(GetOptionsMgr()->GetBool(OPT_CMP_MOVED_BLOCKS));
	options.nIgnoreWhitespace = GetOptionsMgr()->GetInt(OPT_CMP_IGNORE_WHITESPACE);
	options.bIgnoreBlankLines = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_BLANKLINES);
	options.bFilterCommentsLines = GetOptionsMgr()->GetBool(OPT_CMP_FILTER_COMMENTLINES);
	options.bIgnoreCase = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CASE);
	options.bIgnoreEol = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_EOL);

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
	delete m_pCompareStats;

	// Inform all of our merge docs that we're closing
	for (POSITION pos = m_MergeDocs.GetHeadPosition(); pos; )
	{
		CMergeDoc * pMergeDoc = m_MergeDocs.GetNext(pos);
		pMergeDoc->DirDocClosing(this);
	}
	// Delete all temporary folders belonging to this document
	while (m_pTempPathContext)
	{
		m_pTempPathContext = m_pTempPathContext->DeleteHead();
	}
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
 * @brief Initialise directory compare for given paths.
 *
 * Initialises directory compare with paths given and recursive choice.
 * Previous compare context is first free'd.
 * @param [in] paths Paths to compare
 * @param [in] bRecursive If TRUE subdirectories are included to compare.
 */
void CDirDoc::InitCompare(const PathContext & paths, BOOL bRecursive, CTempPathContext *pTempPathContext)
{
	m_pDirView->DeleteAllDisplayItems();
	// Anything that can go wrong here will yield an exception.
	// Default implementation of operator new() never returns NULL.
	delete m_pCtxt;
	
	if (m_pCompareStats == NULL)
		m_pCompareStats = new CompareStats();

	m_pCtxt = new CDiffContext(paths.GetLeft(), paths.GetRight());

	if (pTempPathContext)
	{
		ApplyLeftDisplayRoot(pTempPathContext->m_strLeftDisplayRoot);
		ApplyRightDisplayRoot(pTempPathContext->m_strRightDisplayRoot);
		pTempPathContext->m_pParent = m_pTempPathContext;
		m_pTempPathContext = pTempPathContext;
		m_pTempPathContext->m_strLeftRoot = m_pCtxt->GetNormalizedLeft();
		m_pTempPathContext->m_strRightRoot = m_pCtxt->GetNormalizedRight();
	}
	
	m_bRecursive = bRecursive;
	// All plugin management is done by our plugin manager
	m_pCtxt->m_piPluginInfos = &m_pluginman;
}

/**
 * @brief Tell if user may use ".." and move to parents directory
 *
 * @return No : upward RESTRICTED
 * ParentIsRegularPath : upward ENABLED
 * ParentIsTempPath : upward ENABLED
 */
CDirDoc::AllowUpwardDirectory::ReturnCode CDirDoc::AllowUpwardDirectory(CString &leftParent, CString &rightParent)
{
	const CString & left = GetLeftBasePath();
	const CString & right = GetRightBasePath();
	LPCTSTR lname = PathFindFileName(left);
	LPCTSTR rname = PathFindFileName(right);
	if (m_pTempPathContext)
	{
		int cchLeftRoot = m_pTempPathContext->m_strLeftRoot.GetLength();
		if (left.GetLength() <= cchLeftRoot)
		{
			if (m_pTempPathContext->m_pParent)
			{
				leftParent = m_pTempPathContext->m_pParent->m_strLeftRoot;
				rightParent = m_pTempPathContext->m_pParent->m_strRightRoot;
				if (GetPairComparability(leftParent, rightParent) != IS_EXISTING_DIR)
					return AllowUpwardDirectory::Never;
				return AllowUpwardDirectory::ParentIsTempPath;
			}
			leftParent = m_pTempPathContext->m_strLeftDisplayRoot;
			rightParent = m_pTempPathContext->m_strRightDisplayRoot;
			if (!m_pCtxt->m_piFilterGlobal->includeFile(leftParent, rightParent))
				return AllowUpwardDirectory::Never;
			if (lstrcmpi(lname, _T("ORIGINAL")) == 0 && lstrcmpi(rname, _T("ALTERED")) == 0)
			{
				leftParent = paths_GetParentPath(leftParent);
				rightParent = paths_GetParentPath(rightParent);
			}
			lname = PathFindFileName(leftParent);
			rname = PathFindFileName(rightParent);
			if (lstrcmpi(lname, rname) == 0)
			{
				leftParent = paths_GetParentPath(leftParent);
				rightParent = paths_GetParentPath(rightParent);
				if (GetPairComparability(leftParent, rightParent) != IS_EXISTING_DIR)
					return AllowUpwardDirectory::Never;
				return AllowUpwardDirectory::ParentIsTempPath;
			}
			return AllowUpwardDirectory::No;
		}
		rname = lname;
	}
	if (lstrcmpi(lname, rname) == 0)
	{
		leftParent = paths_GetParentPath(left);
		rightParent = paths_GetParentPath(right);
		if (GetPairComparability(leftParent, rightParent) != IS_EXISTING_DIR)
			return AllowUpwardDirectory::Never;
		return AllowUpwardDirectory::ParentIsRegularPath;
	}
	return AllowUpwardDirectory::No;
}

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

	gLog.Write(CLogFile::LNOTICE, _T("Starting directory scan:\n\tLeft: %s\n\tRight: %s\n"),
			m_pCtxt->GetLeftPath(), m_pCtxt->GetRightPath());
	m_pCompareStats->Reset();
	m_pDirView->StartCompare(m_pCompareStats);

	// Don't clear if only scanning selected items
	if (!m_bMarkedRescan)
	{
		m_pDirView->DeleteAllDisplayItems();
		m_pCtxt->RemoveAll();
	}

	m_pCtxt->m_hDirFrame = pf->GetSafeHwnd();
	m_pCtxt->m_bGuessEncoding = GetOptionsMgr()->GetBool(OPT_CP_DETECT);
	m_pCtxt->m_nCompMethod = GetOptionsMgr()->GetInt(OPT_CMP_METHOD);
	m_pCtxt->m_bIgnoreSmallTimeDiff = GetOptionsMgr()->GetBool(OPT_IGNORE_SMALL_FILETIME);
	m_pCtxt->m_bStopAfterFirstDiff = GetOptionsMgr()->GetBool(OPT_CMP_STOP_AFTER_FIRST);
	m_pCtxt->m_nQuickCompareLimit = GetOptionsMgr()->GetInt(OPT_CMP_QUICK_LIMIT);
	m_pCtxt->m_pCompareStats = m_pCompareStats;

	// Set total items count since we don't collect items
	if (m_bMarkedRescan)
		m_pCompareStats->IncreaseTotalItems(m_pDirView->GetSelectedCount());

	UpdateHeaderPath(0);
	UpdateHeaderPath(1);
	// draw the headers as active ones
	pf->GetHeaderInterface()->SetActive(0, TRUE);
	pf->GetHeaderInterface()->SetActive(1, TRUE);

	// Make sure filters are up-to-date
	theApp.m_globalFileFilter.ReloadUpdatedFilters();
	m_pCtxt->m_piFilterGlobal = &theApp.m_globalFileFilter;

	// Show active filter name in statusbar
	pf->SetFilterStatusDisplay(theApp.m_globalFileFilter.GetFilterNameOrMask());

	// Empty display before new compare
	m_pDirView->DeleteAllDisplayItems();

	m_diffWrapper.StartDirectoryDiff();
	
	m_diffThread.SetContext(m_pCtxt);
	m_diffThread.SetHwnd(m_pDirView->GetSafeHwnd());
	m_diffThread.SetMessageIDs(MSG_UI_UPDATE, MSG_STAT_UPDATE);
	m_diffThread.SetCompareSelected(!!m_bMarkedRescan);
	m_diffThread.CompareDirectories(m_pCtxt->GetNormalizedLeft(),
			m_pCtxt->GetNormalizedRight(), m_bRecursive);
	m_bMarkedRescan = FALSE;
}

/**
 * @brief Determines if the user wants to see given item.
 * @param [in] di Item to check.
 * @return TRUE if item should be shown, FALSE if not.
 * @sa CDirDoc::Redisplay()
 */
BOOL CDirDoc::IsShowable(const DIFFITEM & di)
{
	if (di.isResultFiltered())
	{
		// Treat SKIPPED as a 'super'-flag. If item is skipped and user
		// wants to see skipped items show item regardless of other flags
		return GetOptionsMgr()->GetBool(OPT_SHOW_SKIPPED);
	}

	// Subfolders in non-recursive compare can only be skipped or unique
	if (!m_bRecursive && di.isDirectory())
	{
		// result filters
		if (di.isResultError() && !GetMainFrame()->m_bShowErrors)
			return 0;

		// left/right filters
		if (di.isSideLeft() && !GetOptionsMgr()->GetBool(OPT_SHOW_UNIQUE_LEFT))
			return 0;
		if (di.isSideRight() && !GetOptionsMgr()->GetBool(OPT_SHOW_UNIQUE_RIGHT))
			return 0;
	}
	else
	{
		// file type filters
		if (di.isBin() && !GetOptionsMgr()->GetBool(OPT_SHOW_BINARIES))
			return 0;

		// result filters
		if (di.isResultSame() && !GetOptionsMgr()->GetBool(OPT_SHOW_IDENTICAL))
			return 0;
		if (di.isResultError() && !GetMainFrame()->m_bShowErrors)
			return 0;
		if (di.isResultDiff() && !GetOptionsMgr()->GetBool(OPT_SHOW_DIFFERENT))
			return 0;

		// left/right filters
		if (di.isSideLeft() && !GetOptionsMgr()->GetBool(OPT_SHOW_UNIQUE_LEFT))
			return 0;
		if (di.isSideRight() && !GetOptionsMgr()->GetBool(OPT_SHOW_UNIQUE_RIGHT))
			return 0;
	}
	return 1;
}

/**
 * @brief Empty & reload listview (of files & columns) with comparison results
 * @todo Better solution for special items ("..")?
 */
void CDirDoc::Redisplay()
{
	if (!m_pDirView)
		return;

	// Do not redisplay an empty CDirView
	// Not only does it not have results, but AddSpecialItems will crash
	// trying to dereference null context pointer to get to paths
	if (!HasDiffs())
		return;

	m_pDirView->Redisplay();
}

CDirView * CDirDoc::GetMainView()
{
	CDirView *pView = NULL;
	if (POSITION pos = GetFirstViewPosition())
	{
		pView = static_cast<CDirView*>(GetNextView(pos));
		ASSERT_KINDOF(CDirView, pView);
	}
	return pView;
}

/**
 * @brief Update in-memory diffitem status from disk.
 * @param [in] diffPos POSITION of item in UI list.
 * @param [in] bLeft If TRUE left-side item is updated.
 * @param [in] bRight If TRUE right-side item is updated.
 */
void CDirDoc::UpdateStatusFromDisk(POSITION diffPos, BOOL bLeft, BOOL bRight)
{
	m_pCtxt->UpdateStatusFromDisk(diffPos, bLeft, bRight);
}

/**
 * @brief Update in-memory diffitem status from disk and update view.
 * @param [in] nIdx Index of item in UI list.
 * @param [in] bLeft If TRUE left-side item is updated.
 * @param [in] bRight If TRUE right-side item is updated.
 * @note Do not call this function from DirView code! This function
 * calls slow DirView functions to get item position and to update GUI.
 * Use UpdateStatusFromDisk() function instead.
 */
void CDirDoc::ReloadItemStatus(UINT nIdx, BOOL bLeft, BOOL bRight)
{
	// Get position of item in DiffContext
	POSITION diffpos = m_pDirView->GetItemKey(nIdx);

	// in case just copied (into existence) or modified
	UpdateStatusFromDisk(diffpos, bLeft, bRight);

	// Update view
	m_pDirView->UpdateDiffItemStatus(nIdx);
}

void CDirDoc::InitStatusStrings()
{

}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CDirDoc::UpdateResources()
{
	if (m_pDirView)
		m_pDirView->UpdateResources();

	CString s;
	VERIFY(s.LoadString(IDS_DIRECTORY_WINDOW_TITLE));
	SetTitle(s);

	Redisplay();
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
	if (file1.CompareNoCase(file2) != 0)
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

		if (path1 == current.getLeftFilepath(GetLeftBasePath()) &&
			path2 == current.getRightFilepath(GetRightBasePath()) &&
			file1 == current.sLeftFilename &&
			file2 == current.sRightFilename)
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
	if (m_pCtxt == NULL && !m_bReuseCloses && m_pDirView)
		m_pDirView->PostMessage(WM_COMMAND, ID_FILE_CLOSE);

	if (m_MergeDocs.GetCount() == 0 && !m_pDirView)
		delete this;
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

	// delete comparison parameters and results
	delete m_pCtxt;
	m_pCtxt = NULL;

	while (m_pTempPathContext)
	{
		m_pTempPathContext = m_pTempPathContext->DeleteHead();
	}

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
	if (!GetOptionsMgr()->GetBool(OPT_MULTIDOC_MERGEDOCS) && !m_MergeDocs.IsEmpty())
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
 * @param [in] paths Paths for files we update
 * @param [in] nDiffs Total amount of differences
 * @param [in] nTrivialDiffs Amount of ignored differences
 * @param [in] bIdentical TRUE if files became identical, FALSE otherwise.
 */
void CDirDoc::UpdateChangedItem(PathContext &paths,
	UINT nDiffs, UINT nTrivialDiffs, BOOL bIdentical)
{
	POSITION pos = FindItemFromPaths(paths.GetLeft(), paths.GetRight());
	// If we failed files could have been swapped so lets try again
	if (!pos)
		pos = FindItemFromPaths(paths.GetRight(), paths.GetLeft());
	
	// Update status if paths were found for items.
	// Fail means we had unique items compared as 'renamed' items
	// so there really is not status to update.
	if (pos > 0)
	{
		int ind = m_pDirView->GetItemIndex(pos);

		// Figure out new status code
		UINT diffcode = (bIdentical ? DIFFCODE::SAME : DIFFCODE::DIFF);

		// Update both views and diff context memory
		SetDiffCompare(diffcode, ind);

		if (nDiffs != -1 && nTrivialDiffs != -1)
			SetDiffCounts(nDiffs, nTrivialDiffs, ind);
		ReloadItemStatus(ind, TRUE, TRUE);
	}
}

/**
 * @brief Cleans up after directory compare
 */
void CDirDoc::CompareReady()
{
	gLog.Write(CLogFile::LNOTICE, _T("Directory scan complete\n"));

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

	m_diffWrapper.SetDetectMovedBlocks(GetOptionsMgr()->GetBool(OPT_CMP_MOVED_BLOCKS));
	options.nIgnoreWhitespace = GetOptionsMgr()->GetInt(OPT_CMP_IGNORE_WHITESPACE);
	options.bIgnoreBlankLines = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_BLANKLINES);
	options.bFilterCommentsLines = GetOptionsMgr()->GetBool(OPT_CMP_FILTER_COMMENTLINES);
	options.bIgnoreCase = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CASE);
	options.bIgnoreEol = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_EOL);

	m_diffWrapper.SetOptions(&options);
	if (m_pDirView)
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
 * @brief Set side status of diffitem
 * @note This does not update UI - ReloadItemStatus() does
 * @sa CDirDoc::ReloadItemStatus()
 */
void CDirDoc::SetDiffSide(UINT diffcode, int idx)
{
	SetDiffStatus(diffcode, DIFFCODE::SIDEFLAGS, idx);
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
		{
			sText = m_pCtxt->GetLeftPath();
			ApplyLeftDisplayRoot(sText);
		}
		nPane = 0;
	}
	else
	{
		if (!m_strRightDesc.IsEmpty())
			sText = m_strRightDesc;
		else
		{
			sText = m_pCtxt->GetRightPath();
			ApplyRightDisplayRoot(sText);
		}
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
 * @sa CDirCompStateBar::OnStop()
 */
void CDirDoc::AbortCurrentScan()
{
	gLog.Write(CLogFile::LNOTICE, _T("Dircompare aborted!"));
	m_diffThread.Abort();
}

/**
 * @brief Returns true if there is an active scan that hasn't been aborted.
 */
bool CDirDoc::IsCurrentScanAbortable() const
{
	return (m_diffThread.GetThreadState() == THREAD_COMPARING 
		&& !m_diffThread.IsAborting());
}

/**
 * @brief Set directory description texts shown in headerbar
 */
void CDirDoc::SetDescriptions(const CString &strLeftDesc, const CString &strRightDesc)
{
	m_strLeftDesc = strLeftDesc;
	m_strRightDesc = strRightDesc;
}

/**
 * @brief Replace internal root by display root (left)
 */
void CDirDoc::ApplyLeftDisplayRoot(CString &sText)
{
	if (m_pTempPathContext)
	{
		sText.Delete(0, m_pTempPathContext->m_strLeftRoot.GetLength());
		sText.Insert(0, m_pTempPathContext->m_strLeftDisplayRoot);
	}
}

/**
 * @brief Replace internal root by display root (right)
 */
void CDirDoc::ApplyRightDisplayRoot(CString &sText)
{
	if (m_pTempPathContext)
	{
		sText.Delete(0, m_pTempPathContext->m_strRightRoot.GetLength());
		sText.Insert(0, m_pTempPathContext->m_strRightDisplayRoot);
	}
}

/**
 * @brief Store a plugin setting for specified file comparison
 */
void CDirDoc::SetPluginPrediffSetting(const CString & filteredFilenames, int newsetting)
{
	m_pluginman.SetPrediffSetting(filteredFilenames, newsetting);
}

/**
 * @brief Store a plugin setting for specified file comparison
 */
void CDirDoc::SetPluginPrediffer(const CString & filteredFilenames, const CString & prediffer)
{
	m_pluginman.SetPrediffer(filteredFilenames, prediffer);
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
 * @brief Update results for FileActionItem.
 * This functions is called to update DIFFITEM after FileActionItem.
 * @param [in] act Action that was done.
 * @param [in] pos List position for DIFFITEM affected.
 */
void CDirDoc::UpdateDiffAfterOperation(const FileActionItem & act, POSITION pos)
{
	ASSERT(pos != NULL);
	DIFFITEM di;
	di = GetDiffByKey(pos);

	// Use FileActionItem types for simplicity for now.
	// Better would be to use FileAction contained, since it is not
	// UI dependent.
	switch (act.UIResult)
	{
	case FileActionItem::UI_SYNC:
		SetDiffSide(DIFFCODE::BOTH, act.context);
		if (act.dirflag)
			SetDiffCompare(DIFFCODE::NOCMP, act.context);
		else
			SetDiffCompare(DIFFCODE::SAME, act.context);
		SetDiffCounts(0, 0, act.context);
		break;

	case FileActionItem::UI_DEL_LEFT:
		if (di.isSideLeft())
		{
			RemoveDiffByKey(pos);
		}
		else
		{
			SetDiffSide(DIFFCODE::RIGHT, act.context);
			SetDiffCompare(DIFFCODE::NOCMP, act.context);
		}
		break;

	case FileActionItem::UI_DEL_RIGHT:
		if (di.isSideRight())
		{
			RemoveDiffByKey(pos);
		}
		else
		{
			SetDiffSide(DIFFCODE::LEFT, act.context);
			SetDiffCompare(DIFFCODE::NOCMP, act.context);
		}
		break;

	case FileActionItem::UI_DEL_BOTH:
		RemoveDiffByKey(pos);
		break;
	}
}

/**
 * @brief Set document title to given string or items compared.
 * 
 * Formats and sets caption for directory compare window. Caption
 * has left- and right-side paths separated with '-'.
 *
 * @param [in] lpszTitle New title for window if. If this parameter
 * is not NULL we use this string, otherwise format caption from
 * actual paths.
 */
void CDirDoc::SetTitle(LPCTSTR lpszTitle)
{
	if (!m_pDirView)
		return;

	if (lpszTitle)
		CDocument::SetTitle(lpszTitle);
	else if (!m_pCtxt || m_pCtxt->GetLeftPath().IsEmpty() ||
		m_pCtxt->GetRightPath().IsEmpty())
	{
		CString title;
		VERIFY(title.LoadString(IDS_DIRECTORY_WINDOW_TITLE));
		CDocument::SetTitle(title);
	}
	else
	{
		const TCHAR strSeparator[] = _T(" - ");
		CString strPath = m_pCtxt->GetLeftPath();
		ApplyLeftDisplayRoot(strPath);
		CString strTitle = PathFindFileName(strPath);
		strTitle += strSeparator;
		strPath = m_pCtxt->GetRightPath();
		ApplyRightDisplayRoot(strPath);
		strTitle += PathFindFileName(strPath);
		CDocument::SetTitle(strTitle);
	}	
}

/**
 * @brief Set item's view-flag.
 * @param [in] key Item fow which flag is set.
 * @param [in] flag Flag value to set.
 * @param [in] mask Mask for possible flag values.
 */
void CDirDoc::SetItemViewFlag(POSITION key, UINT flag, UINT mask)
{
	UINT curFlags = m_pCtxt->GetCustomFlags1(key);
	curFlags &= ~mask; // Zero bits masked
	curFlags |= flag;
	m_pCtxt->SetCustomFlags1(key, curFlags);
}

/**
 * @brief Set all item's view-flag.
 * @param [in] flag Flag value to set.
 * @param [in] mask Mask for possible flag values.
 */
void CDirDoc::SetItemViewFlag(UINT flag, UINT mask)
{
	POSITION pos = m_pCtxt->GetFirstDiffPosition();

	while (pos != NULL)
	{
		UINT curFlags = m_pCtxt->GetCustomFlags1(pos);
		curFlags &= ~mask; // Zero bits masked
		curFlags |= flag;
		m_pCtxt->SetCustomFlags1(pos, curFlags);
		m_pCtxt->GetNextDiffPosition(pos);
	}
}
