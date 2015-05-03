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

#include "StdAfx.h"
#include "DirDoc.h"
#include <shlwapi.h>		// PathFindFileName()
#include <Poco/StringTokenizer.h>
#include "Merge.h"
#include "IMergeDoc.h"
#include "CompareOptions.h"
#include "UnicodeString.h"
#include "CompareStats.h"
#include "FilterList.h"
#include "DirView.h"
#include "DirFrame.h"
#include "coretools.h"
#include "paths.h"
#include "7zCommon.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsDiffOptions.h"
#include "FileActionScript.h"
#include "LineFiltersList.h"
#include "FileFilterHelper.h"
#include "unicoder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using Poco::StringTokenizer;

int CDirDoc::m_nDirsTemp = 2;

/////////////////////////////////////////////////////////////////////////////
// CDirDoc

IMPLEMENT_DYNCREATE(CDirDoc, CDocument)

/**
 * @brief Constructor.
 */
CDirDoc::CDirDoc()
: m_pCtxt(nullptr)
, m_pDirView(nullptr)
, m_pCompareStats(nullptr)
, m_bRecursive(FALSE)
, m_bMarkedRescan(FALSE)
, m_pTempPathContext(nullptr)
{
	m_nDirs = m_nDirsTemp;

	m_bRO[0] = FALSE;
	m_bRO[1] = FALSE;
	m_bRO[2] = FALSE;
}

/**
 * @brief Destructor.
 *
 * Clears document list and deleted possible archive-temp files.
 */
CDirDoc::~CDirDoc()
{
	// Inform all of our merge docs that we're closing
	POSITION pos = m_MergeDocs.GetHeadPosition();
	while (pos)
	{
		IMergeDoc * pMergeDoc = m_MergeDocs.GetNext(pos);
		pMergeDoc->DirDocClosing(this);
	}
	// Delete all temporary folders belonging to this document
	while (m_pTempPathContext)
	{
		m_pTempPathContext = m_pTempPathContext->DeleteHead();
	}
}

/**
 * @brief Called when new dirdoc is created.
 */
BOOL CDirDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

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
void CDirDoc::InitCompare(const PathContext & paths, bool bRecursive, CTempPathContext *pTempPathContext)
{
	// Abort previous comparing
	while (m_diffThread.GetThreadState() == CDiffThread::THREAD_COMPARING)
	{
		m_diffThread.Abort();
		Sleep(50);
	}

	m_pDirView->DeleteAllDisplayItems();
	// Anything that can go wrong here will yield an exception.
	// Default implementation of operator new() never returns NULL.
	
	if (m_pCompareStats == NULL)
		m_pCompareStats.reset(new CompareStats(m_nDirs));

	m_pCtxt.reset(new CDiffContext(paths,
			GetOptionsMgr()->GetInt(OPT_CMP_METHOD)));
	m_pCtxt->m_bRecursive = !!bRecursive;

	if (pTempPathContext)
	{
		int nIndex;
		for (nIndex = 0; nIndex < m_nDirs; nIndex++)
			ApplyDisplayRoot(nIndex, pTempPathContext->m_strDisplayRoot[nIndex]);
		pTempPathContext->m_pParent = m_pTempPathContext;
		m_pTempPathContext = pTempPathContext;
		for (nIndex = 0; nIndex < m_nDirs; nIndex++)
			m_pTempPathContext->m_strRoot[nIndex] = m_pCtxt->GetNormalizedPath(nIndex);
	}
	
	m_bRecursive = bRecursive;
	// All plugin management is done by our plugin manager
	m_pCtxt->m_piPluginInfos = GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED) ? &m_pluginman : NULL;
}

/**
 * @brief Tell if user may use ".." and move to parents directory.
 * This function checks if current compare's parent folders should
 * be allowed to open. I.e. if current compare folders are:
 * - C:\Work\Project1 and
 * - C:\Work\Project2
 * we check if C:\Work and C:\Work should be allowed to opened.
 * For regular folders we allow opening if both folders exist.
 * @param [out] leftParent Left parent folder to open.
 * @param [out] rightParent Right parent folder to open.
 * @return Info if opening parent folders should be enabled:
 * - No : upward RESTRICTED
 * - ParentIsRegularPath : upward ENABLED
 * - ParentIsTempPath : upward ENABLED
 */
CDirDoc::AllowUpwardDirectory::ReturnCode
CDirDoc::AllowUpwardDirectory(PathContext &pathsParent)
{
	const String & path0 = GetBasePath(0);
	const String & path1 = GetBasePath(1);
	const String & path2 = m_nDirs > 2 ? GetBasePath(2) : _T("");

	// If we have temp context it means we are comparing archives
	if (IsArchiveFolders())
	{
		LPCTSTR name0 = PathFindFileName(path0.c_str());
		LPCTSTR name1 = PathFindFileName(path1.c_str());
		LPCTSTR name2 = (m_nDirs > 2) ? PathFindFileName(path2.c_str()) : NULL;

		/* FIXME: for 3way diff*/
		String::size_type cchLeftRoot = m_pTempPathContext->m_strRoot[0].length();
		if (path0.length() <= cchLeftRoot)
		{
			pathsParent.SetSize(m_nDirs);
			if (m_pTempPathContext->m_pParent)
			{
				pathsParent[0] = m_pTempPathContext->m_pParent->m_strRoot[0];
				pathsParent[1] = m_pTempPathContext->m_pParent->m_strRoot[1];
				if (GetPairComparability(PathContext(pathsParent[0], pathsParent[1])) != IS_EXISTING_DIR)
					return AllowUpwardDirectory::Never;
				return AllowUpwardDirectory::ParentIsTempPath;
			}
			pathsParent[0] = m_pTempPathContext->m_strDisplayRoot[0];
			pathsParent[1] = m_pTempPathContext->m_strDisplayRoot[1];
			if (!m_pCtxt->m_piFilterGlobal->includeFile(pathsParent[0].c_str(), pathsParent[1].c_str()))
				return AllowUpwardDirectory::Never;
			if (lstrcmpi(name0, _T("ORIGINAL")) == 0 && lstrcmpi(name1, _T("ALTERED")) == 0)
			{
				pathsParent[0] = paths_GetParentPath(pathsParent[0]);
				pathsParent[1] = paths_GetParentPath(pathsParent[1]);
			}
			name0 = PathFindFileName(pathsParent[0].c_str());
			name1 = PathFindFileName(pathsParent[1].c_str());
			if (lstrcmpi(name0, name1) == 0)
			{
				pathsParent[0] = paths_GetParentPath(pathsParent[0]);
				pathsParent[1] = paths_GetParentPath(pathsParent[1]);
				if (GetPairComparability(PathContext(pathsParent[0], pathsParent[1])) != IS_EXISTING_DIR)
					return AllowUpwardDirectory::Never;
				return AllowUpwardDirectory::ParentIsTempPath;
			}
			return AllowUpwardDirectory::No;
		}
		name1 = name0;
	}

	// If regular parent folders exist, allow opening them
	pathsParent.SetSize(m_nDirs);
	pathsParent[0] = paths_GetParentPath(path0);
	pathsParent[1] = paths_GetParentPath(path1);
	if (m_nDirs > 2)
		pathsParent[2] = paths_GetParentPath(path2);
	if (GetPairComparability(pathsParent) != IS_EXISTING_DIR)
		return AllowUpwardDirectory::Never;
	return AllowUpwardDirectory::ParentIsRegularPath;
}

/**
 * @brief Load line filters to the compare context.
 * Loads linefilters, converts them to UTF-8 and sets them for compare context.
 */
void CDirDoc::LoadLineFilterList()
{
	ASSERT(m_pCtxt);
	
	BOOL bFilters = GetOptionsMgr()->GetBool(OPT_LINEFILTER_ENABLED);
	String filters = theApp.m_pLineFilters->GetAsString();
	if (!bFilters || filters.empty())
	{
		m_pCtxt->m_pFilterList.reset();
		return;
	}

	if (m_pCtxt->m_pFilterList)
		m_pCtxt->m_pFilterList->RemoveAllFilters();
	else
		m_pCtxt->m_pFilterList.reset(new FilterList());

	std::string regexp_str = ucr::toUTF8(filters);

	// Add every "line" of regexps to regexp list
	StringTokenizer tokens(regexp_str, "\r\n");
	for (StringTokenizer::Iterator it = tokens.begin(); it != tokens.end(); ++it)
		m_pCtxt->m_pFilterList->AddRegExp(*it);
}

void CDirDoc::DiffThreadCallback(int& state)
{
	PostMessage(m_pDirView->GetSafeHwnd(), MSG_UI_UPDATE, state, false);
}

/**
 * @brief Perform directory comparison again from scratch
 */
void CDirDoc::Rescan()
{
	if (!m_pCtxt)
		return;

	CDirFrame *pf = m_pDirView->GetParentFrame();

	// If we're already doing a rescan, bail out
	UINT threadState = m_diffThread.GetThreadState();
	if (threadState == CDiffThread::THREAD_COMPARING)
		return;

	m_pCompareStats->Reset();
	m_pDirView->StartCompare(m_pCompareStats.get());

	m_pDirView->DeleteAllDisplayItems();
	// Don't clear if only scanning selected items
	if (!m_bMarkedRescan)
		m_pCtxt->RemoveAll();

	LoadLineFilterList();

	DIFFOPTIONS options = {0};
	Options::DiffOptions::Load(options);

	m_pCtxt->CreateCompareOptions(GetOptionsMgr()->GetInt(OPT_CMP_METHOD), options);

	m_pCtxt->m_iGuessEncodingType = GetOptionsMgr()->GetInt(OPT_CP_DETECT);
	if ((m_pCtxt->m_iGuessEncodingType >> 16) == 0)
		m_pCtxt->m_iGuessEncodingType |= 50001 << 16;
	m_pCtxt->m_bIgnoreSmallTimeDiff = GetOptionsMgr()->GetBool(OPT_IGNORE_SMALL_FILETIME);
	m_pCtxt->m_bStopAfterFirstDiff = GetOptionsMgr()->GetBool(OPT_CMP_STOP_AFTER_FIRST);
	m_pCtxt->m_nQuickCompareLimit = GetOptionsMgr()->GetInt(OPT_CMP_QUICK_LIMIT);
	m_pCtxt->m_bPluginsEnabled = GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED);
	m_pCtxt->m_bWalkUniques = GetOptionsMgr()->GetBool(OPT_CMP_WALK_UNIQUE_DIRS);
	m_pCtxt->m_bIgnoreReparsePoints = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_REPARSE_POINTS);
	m_pCtxt->m_pCompareStats = m_pCompareStats.get();
	m_pCtxt->m_bRecursive = !!m_bRecursive;

	pf->GetHeaderInterface()->SetPaneCount(m_nDirs);
	for (int nIndex = 0; nIndex < m_nDirs; nIndex++)
	{
		UpdateHeaderPath(nIndex);
		// draw the headers as active ones
		pf->GetHeaderInterface()->SetActive(nIndex, TRUE);
	}
	pf->GetHeaderInterface()->Resize();

	// Make sure filters are up-to-date
	theApp.m_pGlobalFileFilter->ReloadUpdatedFilters();
	m_pCtxt->m_piFilterGlobal = theApp.m_pGlobalFileFilter.get();

	// Show current compare method name and active filter name in statusbar
	pf->SetFilterStatusDisplay(theApp.m_pGlobalFileFilter->GetFilterNameOrMask().c_str());
	pf->SetCompareMethodStatusDisplay(m_pCtxt->GetCompareMethod());

	// Folder names to compare are in the compare context
	m_diffThread.SetContext(m_pCtxt.get());
	m_diffThread.RemoveListener(this, &CDirDoc::DiffThreadCallback);
	m_diffThread.AddListener(this, &CDirDoc::DiffThreadCallback);
	m_diffThread.SetCompareSelected(!!m_bMarkedRescan);
	m_diffThread.CompareDirectories();
	m_bMarkedRescan = FALSE;
}

/**
 * @brief Determines if the user wants to see given item.
 * This function determines what items to show and what items to hide. There
 * are lots of combinations, but basically we check if menuitem is enabled or
 * disabled and show/hide matching items. For non-recursive compare we never
 * hide folders as that would disable user browsing into them. And we even
 * don't really know if folders are identical or different as we haven't
 * compared them.
 * @param [in] di Item to check.
 * @return TRUE if item should be shown, FALSE if not.
 * @sa CDirDoc::Redisplay()
 */
bool CDirDoc::IsShowable(const DIFFITEM & di) const
{
	if (di.customFlags1 & ViewCustomFlags::HIDDEN)
		return FALSE;

	if (di.diffcode.isResultFiltered())
	{
		// Treat SKIPPED as a 'super'-flag. If item is skipped and user
		// wants to see skipped items show item regardless of other flags
		return GetOptionsMgr()->GetBool(OPT_SHOW_SKIPPED);
	}

	if (di.diffcode.isDirectory())
	{
		// Subfolders in non-recursive compare can only be skipped or unique
		if (!m_bRecursive)
		{
			// left/right filters
			if (di.diffcode.isSideFirstOnly() && !GetOptionsMgr()->GetBool(OPT_SHOW_UNIQUE_LEFT))
				return FALSE;
			if (di.diffcode.isSideSecondOnly() && !GetOptionsMgr()->GetBool(OPT_SHOW_UNIQUE_RIGHT))
				return FALSE;

			// result filters
			if (di.diffcode.isResultError())
				return FALSE;
		}
		else // recursive mode (including tree-mode)
		{
			// left/right filters
			if (di.diffcode.isSideFirstOnly() &&	!GetOptionsMgr()->GetBool(OPT_SHOW_UNIQUE_LEFT))
				return FALSE;
			if (di.diffcode.isSideSecondOnly() && !GetOptionsMgr()->GetBool(OPT_SHOW_UNIQUE_RIGHT))
				return FALSE;

			// ONLY filter folders by result (identical/different) for tree-view.
			// In the tree-view we show subfolders with identical/different
			// status. The flat view only shows files inside folders. So if we
			// filter by status the files inside folder are filtered too and
			// users see files appearing/disappearing without clear logic.		
			if (GetOptionsMgr()->GetBool(OPT_TREE_MODE))
			{
				// result filters
				if (di.diffcode.isResultError() && FALSE)
					return FALSE;

				// result filters
				if (di.diffcode.isResultSame() && !GetOptionsMgr()->GetBool(OPT_SHOW_IDENTICAL))
					return FALSE;
				if (di.diffcode.isResultDiff() && !GetOptionsMgr()->GetBool(OPT_SHOW_DIFFERENT))
					return FALSE;
			}
		}
	}
	else
	{
		// left/right filters
		if (di.diffcode.isSideFirstOnly() && !GetOptionsMgr()->GetBool(OPT_SHOW_UNIQUE_LEFT))
			return FALSE;
		if (di.diffcode.isSideSecondOnly() && !GetOptionsMgr()->GetBool(OPT_SHOW_UNIQUE_RIGHT))
			return FALSE;

		// file type filters
		if (di.diffcode.isBin() && !GetOptionsMgr()->GetBool(OPT_SHOW_BINARIES))
			return FALSE;

		// result filters
		if (di.diffcode.isResultSame() && !GetOptionsMgr()->GetBool(OPT_SHOW_IDENTICAL))
			return FALSE;
		if (di.diffcode.isResultError() && FALSE)
			return FALSE;
		if (di.diffcode.isResultDiff() && !GetOptionsMgr()->GetBool(OPT_SHOW_DIFFERENT))
			return FALSE;
	}
	return TRUE;
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

CDirView * CDirDoc::GetMainView() const
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
 * @param [in] idx index to update.
 */
void CDirDoc::UpdateStatusFromDisk(uintptr_t diffPos, int idx)
{
	m_pCtxt->UpdateStatusFromDisk(diffPos, idx);
}

/**
 * @brief Update in-memory diffitem status from disk and update view.
 * @param [in] diffPos POSITION of item in UI list.
 * @param [in] idx index to reload.
 * @note Do not call this function from DirView code! This function
 * calls slow DirView functions to get item position and to update GUI.
 * Use UpdateStatusFromDisk() function instead.
 */
void CDirDoc::ReloadItemStatus(uintptr_t diffPos, int idx)
{
	// in case just copied (into existence) or modified
	UpdateStatusFromDisk(diffPos, idx);

	int nIdx = m_pDirView->GetItemIndex(diffPos);
	if (nIdx != -1)
	{
		// Update view
		m_pDirView->UpdateDiffItemStatus(nIdx);
	}
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

	SetTitle(0);

	Redisplay();
}

/**
 * @brief Find the CDiffContext diffpos of an item from its left & right paths
 * @return POSITION to item, NULL if not found.
 * @note Filenames must be same, if they differ NULL is returned.
 */
uintptr_t CDirDoc::FindItemFromPaths(const String& pathLeft, const String& pathRight)
{
	String file1 = paths_FindFileName(pathLeft);
	String file2 = paths_FindFileName(pathRight);

	// Filenames must be identical
	if (string_compare_nocase(file1, file2) != 0)
		return NULL;

	String path1(pathLeft, 0, pathLeft.length() - file1.length()); // include trailing backslash
	String path2(pathRight, 0, pathRight.length() - file2.length()); // include trailing backslash

	// Path can contain (because of difftools?) '/' and '\'
	// so for comparing purposes, convert whole path to use '\\'
	replace_char(&*path1.begin(), '/', '\\');
	replace_char(&*path2.begin(), '/', '\\');

	String base1 = m_pCtxt->GetLeftPath(); // include trailing backslash
	if (path1.compare(0, base1.length(), base1.c_str()) != 0)
		return NULL;
	path1.erase(0, base1.length()); // turn into relative path
	if (String::size_type length = path1.length())
		path1.resize(length - 1); // remove trailing backslash

	String base2 = m_pCtxt->GetRightPath(); // include trailing backslash
	if (path2.compare(0, base2.length(), base2.c_str()) != 0)
		return NULL;
	path2.erase(0, base2.length()); // turn into relative path
	if (String::size_type length = path2.length())
		path2.resize(length - 1); // remove trailing backslash

	uintptr_t pos = m_pCtxt->GetFirstDiffPosition();
	while (uintptr_t currentPos = pos) // Save our current pos before getting next
	{
		const DIFFITEM &di = m_pCtxt->GetNextDiffPosition(pos);
		if (di.diffFileInfo[0].path == path1 &&
			di.diffFileInfo[1].path == path2 &&
			di.diffFileInfo[0].filename == file1 &&
			di.diffFileInfo[1].filename == file2)
		{
			return currentPos;
		}
	}
	return 0;
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
void CDirDoc::AddMergeDoc(IMergeDoc * pMergeDoc)
{
	ASSERT(pMergeDoc);
	m_MergeDocs.AddTail(pMergeDoc);
}

/**
 * @brief MergeDoc informs us it is closing.
 */
void CDirDoc::MergeDocClosing(IMergeDoc * pMergeDoc)
{
	ASSERT(pMergeDoc);
	if (POSITION pos = m_MergeDocs.CPtrList::Find(pMergeDoc))
		m_MergeDocs.RemoveAt(pos);
	else
		ASSERT(FALSE);

	// If dir compare is empty (no compare results) and we are not closing
	// because of reuse close also dir compare
	if (m_pDirView)
	{
		if (m_pCtxt == NULL)
			m_pDirView->PostMessage(WM_COMMAND, ID_FILE_CLOSE);
	}
	else if (m_MergeDocs.GetCount() == 0)
	{
		delete this;
	}
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
	POSITION pos = m_MergeDocs.GetHeadPosition();
	while (pos)
	{
		IMergeDoc * pMergeDoc = m_MergeDocs.GetNext(pos);
		if (!pMergeDoc->CloseNow())
			return FALSE;
	}
	return TRUE;
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
	uintptr_t pos = FindItemFromPaths(paths.GetLeft(), paths.GetRight());
	// If we failed files could have been swapped so lets try again
	if (!pos)
		pos = FindItemFromPaths(paths.GetRight(), paths.GetLeft());
	
	// Update status if paths were found for items.
	// Fail means we had unique items compared as 'renamed' items
	// so there really is not status to update.
	if (pos > 0)
	{
		// Figure out new status code
		UINT diffcode = (bIdentical ? DIFFCODE::SAME : DIFFCODE::DIFF);

		// Update both views and diff context memory
		m_pCtxt->SetDiffStatusCode(pos, diffcode, DIFFCODE::COMPAREFLAGS);

		if (nDiffs != -1 && nTrivialDiffs != -1)
			m_pCtxt->SetDiffCounts(pos, nDiffs, nTrivialDiffs);
		for (int i = 0; i < m_nDirs; ++i)
			ReloadItemStatus(pos, i);
	}
}

/**
 * @brief Cleans up after directory compare
 */
void CDirDoc::CompareReady()
{
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
	if (m_pDirView)
		m_pDirView->RefreshOptions();
}

/**
 * @brief Set left/middle/right side readonly-status
 * @param nIndex Select side to set 
 * @param bReadOnly New status of selected side
 */
void CDirDoc::SetReadOnly(int nIndex, bool bReadOnly)
{
	m_bRO[nIndex] = bReadOnly;
}

/**
 * @brief Return left/middle/right side readonly-status
 * @param nIndex Select side to ask
 */
bool CDirDoc::GetReadOnly(int nIndex) const
{
	return m_bRO[nIndex];
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
	uintptr_t diffpos = m_pDirView->GetItemKey(idx);

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
void CDirDoc::UpdateHeaderPath(int nIndex)
{
	CDirFrame *pf = m_pDirView->GetParentFrame();
	ASSERT(pf);
	String sText;

	if (!m_strDesc[nIndex].empty())
		sText = m_strDesc[nIndex];
	else
	{
		sText = m_pCtxt->GetPath(nIndex);
		ApplyDisplayRoot(nIndex, sText);
	}

	pf->GetHeaderInterface()->SetText(nIndex, sText.c_str());
}

/**
 * @brief virtual override called just before document is saved and closed
 */
BOOL CDirDoc::SaveModified() 
{
	// Do not allow closing if there is a thread running
	if (m_diffThread.GetThreadState() == CDiffThread::THREAD_COMPARING)
	{
		int ans = LangMessageBox(IDS_CONFIRM_CLOSE_WINDOW, MB_YESNO | MB_ICONWARNING);
		if (ans == IDNO)
			return FALSE;
		m_diffThread.Abort();
		while (m_diffThread.GetThreadState() == CDiffThread::THREAD_COMPARING)
			Sleep(50);
	}
	
	return CDocument::SaveModified();
}

/**
 * @brief Send signal to thread to stop current scan
 *
 * @sa CDirCompStateBar::OnStop()
 */
void CDirDoc::AbortCurrentScan()
{
	m_diffThread.Abort();
}

/**
 * @brief Returns true if there is an active scan that hasn't been aborted.
 */
bool CDirDoc::IsCurrentScanAbortable() const
{
	return (m_diffThread.GetThreadState() == CDiffThread::THREAD_COMPARING 
		&& !m_diffThread.IsAborting());
}

/**
 * @brief Set directory description texts shown in headerbar
 */
void CDirDoc::SetDescriptions(const String strDesc[])
{
	std::copy(strDesc, strDesc + m_nDirs, m_strDesc);
}

/**
 * @brief Replace internal root by display root
 * When we have a archive file open, this function converts physical folder
 * (that is in the temp folder where archive was extracted) to the virtual
 * path for showing. The virtual path is path to the archive file, archive
 * file name and folder inside the archive.
 * @param [in, out] sText Path to convert.
 */
void CDirDoc::ApplyDisplayRoot(int nIndex, String &sText)
{
	if (m_pTempPathContext)
	{
		sText.erase(0, m_pTempPathContext->m_strRoot[nIndex].length());
		sText.insert(0, m_pTempPathContext->m_strDisplayRoot[nIndex].c_str());
	}
}

/**
 * @brief Store a plugin setting for specified file comparison
 */
void CDirDoc::SetPluginPrediffSetting(const String& filteredFilenames, int newsetting)
{
	m_pluginman.SetPrediffSetting(filteredFilenames, newsetting);
}

/**
 * @brief Store a plugin setting for specified file comparison
 */
void CDirDoc::SetPluginPrediffer(const String& filteredFilenames, const String & prediffer)
{
	m_pluginman.SetPrediffer(filteredFilenames, prediffer);
}

/**
 * @brief Retrieve any cached plugin info for specified comparison
 */
void CDirDoc::FetchPluginInfos(const String& filteredFilenames, 
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
	uintptr_t diffpos = m_pDirView->GetItemKey(idx);

	// Update diff counts
	m_pCtxt->SetDiffCounts(diffpos, diffs, ignored);
}

/**
 * @brief Update results for FileActionItem.
 * This functions is called to update DIFFITEM after FileActionItem.
 * @param [in] act Action that was done.
 * @param [in] pos List position for DIFFITEM affected.
 */
void CDirDoc::UpdateDiffAfterOperation(const FileActionItem & act, uintptr_t pos)
{
	ASSERT(pos != NULL);
	const DIFFITEM &di = GetDiffByKey(pos);

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
		if (di.diffcode.isSideFirstOnly())
		{
			RemoveDiffByKey(pos);
		}
		else
		{
			SetDiffSide(DIFFCODE::SECOND, act.context);
			SetDiffCompare(DIFFCODE::NOCMP, act.context);
		}
		break;

	case FileActionItem::UI_DEL_RIGHT:
		if (di.diffcode.isSideSecondOnly())
		{
			RemoveDiffByKey(pos);
		}
		else
		{
			SetDiffSide(DIFFCODE::FIRST, act.context);
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
	else if (!m_pCtxt || m_pCtxt->GetLeftPath().empty() ||
		m_pCtxt->GetRightPath().empty() || 
		(m_nDirs > 2 && m_pCtxt->GetMiddlePath().empty()))
	{
		String title = theApp.LoadString(IDS_DIRECTORY_WINDOW_TITLE);
		CDocument::SetTitle(title.c_str());
	}
	else
	{
		String sTitle;
		String sDirName[3];
		int index;
		for (index = 0; index < m_nDirs; index++)
		{
			String strPath = m_pCtxt->GetPath(index);
			ApplyDisplayRoot(index, strPath);
			sDirName[index] = PathFindFileName(strPath.c_str());
		}
		if (std::count(&sDirName[0], &sDirName[0] + m_nDirs, sDirName[0]) == m_nDirs)
			sTitle = sDirName[0] + string_format(_T(" x %d"), m_nDirs);
 		else
			sTitle = string_join(&sDirName[0], &sDirName[0] + m_nDirs, _T(" - "));
 		CDocument::SetTitle(sTitle.c_str());
	}	
}

/**
 * @brief Set item's view-flag.
 * @param [in] key Item fow which flag is set.
 * @param [in] flag Flag value to set.
 * @param [in] mask Mask for possible flag values.
 */
void CDirDoc::SetItemViewFlag(uintptr_t key, UINT flag, UINT mask)
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
	uintptr_t pos = m_pCtxt->GetFirstDiffPosition();

	while (pos != NULL)
	{
		UINT curFlags = m_pCtxt->GetCustomFlags1(pos);
		curFlags &= ~mask; // Zero bits masked
		curFlags |= flag;
		m_pCtxt->SetCustomFlags1(pos, curFlags);
		m_pCtxt->GetNextDiffPosition(pos);
	}
}

/**
 * @brief Checks if current folders are opened from archive file.
 * @return true if we are inside archive, false otherwise.
 */
bool CDirDoc::IsArchiveFolders() const
{
	if (m_pTempPathContext)
		return true;
	else
		return false;
}
