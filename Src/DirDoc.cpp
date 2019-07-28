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
#include <Poco/StringTokenizer.h>
#include <boost/range/mfc.hpp>
#include "Merge.h"
#include "IMergeDoc.h"
#include "CompareOptions.h"
#include "UnicodeString.h"
#include "CompareStats.h"
#include "FilterList.h"
#include "DirView.h"
#include "DirFrame.h"
#include "MainFrm.h"
#include "paths.h"
#include "7zCommon.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsDiffOptions.h"
#include "LineFiltersList.h"
#include "FileFilterHelper.h"
#include "unicoder.h"
#include "DirActions.h"
#include "DirScan.h"
#include "MessageBoxDialog.h"
#include "DirCmpReport.h"
#include <Poco/Semaphore.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using Poco::StringTokenizer;
using boost::begin;
using boost::end;

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
, m_bMarkedRescan(false)
, m_pTempPathContext(nullptr)
, m_bGeneratingReport(false)
, m_pReport(nullptr)
{
	m_nDirs = m_nDirsTemp;

	m_bRO[0] = false;
	m_bRO[1] = false;
	m_bRO[2] = false;
}

/**
 * @brief Destructor.
 *
 * Clears document list and deleted possible archive-temp files.
 */
CDirDoc::~CDirDoc()
{
	// Inform all of our merge docs that we're closing
	for (auto pMergeDoc : m_MergeDocs)
		pMergeDoc->DirDocClosing(this);
	// Delete all temporary folders belonging to this document
	while (m_pTempPathContext != nullptr)
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
 * @param [in] bRecursive If `true` subdirectories are included to compare.
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
	// Default implementation of operator new() never returns `nullptr`.
	
	if (m_pCompareStats == nullptr)
		m_pCompareStats.reset(new CompareStats(m_nDirs));

	m_pCtxt.reset(new CDiffContext(paths,
			GetOptionsMgr()->GetInt(OPT_CMP_METHOD)));
	m_pCtxt->m_bRecursive = bRecursive;

	if (pTempPathContext != nullptr)
	{
		int nIndex;
		for (nIndex = 0; nIndex < m_nDirs; nIndex++)
			ApplyDisplayRoot(nIndex, pTempPathContext->m_strDisplayRoot[nIndex]);
		pTempPathContext->m_pParent = m_pTempPathContext;
		m_pTempPathContext = pTempPathContext;
		for (nIndex = 0; nIndex < m_nDirs; nIndex++)
			m_pTempPathContext->m_strRoot[nIndex] = m_pCtxt->GetNormalizedPath(nIndex);
	}
	
	// All plugin management is done by our plugin manager
	m_pCtxt->m_piPluginInfos = GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED) ? &m_pluginman : nullptr;
}


/**
 * @brief Load line filters to the compare context.
 * Loads linefilters, converts them to UTF-8 and sets them for compare context.
 */
void CDirDoc::LoadLineFilterList()
{
	ASSERT(m_pCtxt != nullptr);
	
	bool bFilters = GetOptionsMgr()->GetBool(OPT_LINEFILTER_ENABLED);
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
	if (m_pCtxt == nullptr)
		return;

	CDirFrame *pf = m_pDirView->GetParentFrame();

	// If we're already doing a rescan, bail out
	UINT threadState = m_diffThread.GetThreadState();
	if (threadState == CDiffThread::THREAD_COMPARING)
		return;

	if (!m_bGeneratingReport)
		m_pCompareStats->Reset();
	m_pDirView->StartCompare(m_pCompareStats.get());

	if (!m_bGeneratingReport)
		m_pDirView->DeleteAllDisplayItems();
	// Don't clear if only scanning selected items
	if (!m_bMarkedRescan && !m_bGeneratingReport)
	{
		m_pCtxt->RemoveAll();
		m_pCtxt->InitDiffItemList();
	}

	LoadLineFilterList();

	DIFFOPTIONS options = {0};
	Options::DiffOptions::Load(GetOptionsMgr(), options);

	m_pCtxt->CreateCompareOptions(GetOptionsMgr()->GetInt(OPT_CMP_METHOD), options);

	m_pCtxt->m_iGuessEncodingType = GetOptionsMgr()->GetInt(OPT_CP_DETECT);
	if ((m_pCtxt->m_iGuessEncodingType >> 16) == 0)
		m_pCtxt->m_iGuessEncodingType |= 50001 << 16;
	m_pCtxt->m_bIgnoreSmallTimeDiff = GetOptionsMgr()->GetBool(OPT_IGNORE_SMALL_FILETIME);
	m_pCtxt->m_bStopAfterFirstDiff = GetOptionsMgr()->GetBool(OPT_CMP_STOP_AFTER_FIRST);
	m_pCtxt->m_nQuickCompareLimit = GetOptionsMgr()->GetInt(OPT_CMP_QUICK_LIMIT);
	m_pCtxt->m_nBinaryCompareLimit = GetOptionsMgr()->GetInt(OPT_CMP_BINARY_LIMIT);
	m_pCtxt->m_bPluginsEnabled = GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED);
	m_pCtxt->m_bWalkUniques = GetOptionsMgr()->GetBool(OPT_CMP_WALK_UNIQUE_DIRS);
	m_pCtxt->m_bIgnoreReparsePoints = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_REPARSE_POINTS);
	m_pCtxt->m_bIgnoreCodepage = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CODEPAGE);
	m_pCtxt->m_pCompareStats = m_pCompareStats.get();

	pf->GetHeaderInterface()->SetPaneCount(m_nDirs);
	pf->GetHeaderInterface()->SetOnSetFocusCallback([&](int pane) {
		m_pDirView->SetActivePane(pane);
		GetOptionsMgr()->SaveOption(OPT_ACTIVE_PANE, pane);
	});
	for (int nIndex = 0; nIndex < m_nDirs; nIndex++)
	{
		UpdateHeaderPath(nIndex);
		// draw the headers as inactive ones
		pf->GetHeaderInterface()->SetActive(nIndex, false);
	}
	pf->GetHeaderInterface()->Resize();
	int nPane = GetOptionsMgr()->GetInt(OPT_ACTIVE_PANE);
	m_pDirView->SetActivePane((nPane >= 0 && nPane < m_nDirs) ? nPane : 0);

	// Make sure filters are up-to-date
	theApp.m_pGlobalFileFilter->ReloadUpdatedFilters();
	m_pCtxt->m_piFilterGlobal = theApp.m_pGlobalFileFilter.get();
	
	m_pCtxt->m_pFilterCommentsManager = theApp.m_pFilterCommentsManager.get();

	// Show current compare method name and active filter name in statusbar
	pf->SetFilterStatusDisplay(theApp.m_pGlobalFileFilter->GetFilterNameOrMask().c_str());
	pf->SetCompareMethodStatusDisplay(m_pCtxt->GetCompareMethod());

	// Folder names to compare are in the compare context
	m_diffThread.SetContext(m_pCtxt.get());
	m_diffThread.RemoveListener(this, &CDirDoc::DiffThreadCallback);
	m_diffThread.AddListener(this, &CDirDoc::DiffThreadCallback);
	if (m_bGeneratingReport)
	{
		m_diffThread.SetCollectFunction([&](DiffFuncStruct* myStruct) {
			int m = 0;
			if (m_pReport->GetCopyToClipboard())
			{
				++m;
				if (m_pReport->GetReportType() == REPORT_TYPE_SIMPLEHTML)
					++m;
			}
			if (!m_pReport->GetReportFile().empty())
				++m;
			myStruct->context->m_pCompareStats->IncreaseTotalItems(
				(m_pDirView->GetListCtrl().GetItemCount() - (myStruct->context->m_bRecursive ? 0 : 1)) * m);
		});
		m_diffThread.SetCompareFunction([&](DiffFuncStruct* myStruct) {
			m_pReport->SetDiffFuncStruct(myStruct);
			myStruct->pSemaphore->wait();
			String errStr;
			if (m_pReport->GenerateReport(errStr))
			{
				if (errStr.empty())
				{
					if (GetReportFile().empty())
						LangMessageBox(IDS_REPORT_SUCCESS, MB_OK | MB_ICONINFORMATION);
				}
				else
				{
					String msg = strutils::format_string1(
						_("Error creating the report:\n%1"),
						errStr);
					AfxMessageBox(msg.c_str(), MB_OK | MB_ICONSTOP);
				}
			}
			SetGeneratingReport(false);
			SetReport(nullptr);
		});
	}
	else if (m_bMarkedRescan)
	{
		m_diffThread.SetCollectFunction(nullptr);
		m_diffThread.SetCompareFunction([](DiffFuncStruct* myStruct) {
			int nItems = DirScan_UpdateMarkedItems(myStruct, nullptr);
			myStruct->context->m_pCompareStats->IncreaseTotalItems(nItems);
			DirScan_CompareRequestedItems(myStruct, nullptr);
		});
	}
	else
	{
		m_diffThread.SetCollectFunction([](DiffFuncStruct* myStruct) {
			bool casesensitive = false;
			int depth = myStruct->context->m_bRecursive ? -1 : 0;
			PathContext paths = myStruct->context->GetNormalizedPaths();
			String subdir[3] = {_T(""), _T(""), _T("")}; // blank to start at roots specified in diff context
			// Build results list (except delaying file comparisons until below)
			DirScan_GetItems(paths, subdir, myStruct,
					casesensitive, depth, nullptr, myStruct->context->m_bWalkUniques);
		});
		m_diffThread.SetCompareFunction([](DiffFuncStruct* myStruct) {
			DirScan_CompareItems(myStruct, nullptr);
		});
	}
	m_diffThread.CompareDirectories();
	m_bMarkedRescan = false;
}

/**
 * @brief Empty & reload listview (of files & columns) with comparison results
 * @todo Better solution for special items ("..")?
 */
void CDirDoc::Redisplay()
{
	if (m_pDirView == nullptr)
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
	CDirView *pView = nullptr;
	if (POSITION pos = GetFirstViewPosition())
	{
		pView = static_cast<CDirView*>(GetNextView(pos));
		ASSERT_KINDOF(CDirView, pView);
	}
	return pView;
}

/**
 * @brief Update in-memory diffitem status from disk and update view.
 * @param [in] diffPos POSITION of item in UI list.
 * @param [in] idx index to reload.
 * @note Do not call this function from DirView code! This function
 * calls slow DirView functions to get item position and to update GUI.
 * Use UpdateStatusFromDisk() function instead.
 */
void CDirDoc::ReloadItemStatus(DIFFITEM *diffPos, int idx)
{
	// in case just copied (into existence) or modified
	m_pCtxt->UpdateStatusFromDisk(diffPos, idx);

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
	if (m_pDirView != nullptr)
		m_pDirView->UpdateResources();

	SetTitle(nullptr);

	Redisplay();
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
	ASSERT(pMergeDoc != nullptr);
	m_MergeDocs.AddTail(pMergeDoc);
}

/**
 * @brief MergeDoc informs us it is closing.
 */
void CDirDoc::MergeDocClosing(IMergeDoc * pMergeDoc)
{
	ASSERT(pMergeDoc != nullptr);
	if (POSITION pos = m_MergeDocs.CPtrList::Find(pMergeDoc))
		m_MergeDocs.RemoveAt(pos);
	else
		ASSERT(false);

	// If dir compare is empty (no compare results) and we are not closing
	// because of reuse close also dir compare
	if (m_pDirView != nullptr)
	{
		if (m_pCtxt == nullptr)
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
 * @return `true` if success, `false` if user canceled or closing failed
 */
bool CDirDoc::CloseMergeDocs()
{
	while (!m_MergeDocs.IsEmpty())
		if (!m_MergeDocs.GetTail()->CloseNow())
			return false;
	return true;
}

/**
 * @brief Update changed item's compare status
 * @param [in] paths Paths for files we update
 * @param [in] nDiffs Total amount of differences
 * @param [in] nTrivialDiffs Amount of ignored differences
 * @param [in] bIdentical `true` if files became identical, `false` otherwise.
 */
void CDirDoc::UpdateChangedItem(const PathContext &paths,
	UINT nDiffs, UINT nTrivialDiffs, bool bIdentical)
{
	DIFFITEM *pos = FindItemFromPaths(*m_pCtxt, paths);
	// If we failed files could have been swapped so lets try again
	if (!pos)
	{
		PathContext pathsSwapped(paths);
		std::swap(pathsSwapped[0], pathsSwapped[pathsSwapped.GetSize() - 1]);
		pos = FindItemFromPaths(*m_pCtxt, pathsSwapped);
		if (!pos && paths.GetSize() > 2)
		{
			pathsSwapped = paths;
			std::swap(pathsSwapped[0], pathsSwapped[1]);
			pos = FindItemFromPaths(*m_pCtxt, pathsSwapped);
			if (!pos && paths.GetSize() > 2)
			{
				pathsSwapped = paths;
				std::swap(pathsSwapped[1], pathsSwapped[2]);
				pos = FindItemFromPaths(*m_pCtxt, pathsSwapped);
			}
		}
	}
	
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
		for (int i = 0; i < m_pCtxt->GetCompareDirs(); ++i)
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
	if (m_pCtxt != nullptr)
		m_pCtxt->m_bRecursive = GetOptionsMgr()->GetBool(OPT_CMP_INCLUDE_SUBDIRS);
	if (m_pDirView != nullptr)
		m_pDirView->RefreshOptions();
}

/**
 * @brief Write path and filename to headerbar
 * @note SetText() does not repaint unchanged text
 */
void CDirDoc::UpdateHeaderPath(int nIndex)
{
	CDirFrame *pf = m_pDirView->GetParentFrame();
	ASSERT(pf != nullptr);
	String sText;

	if (!m_strDesc[nIndex].empty())
		sText = m_strDesc[nIndex];
	else
	{
		sText = m_pCtxt->GetPath(nIndex);
		ApplyDisplayRoot(nIndex, sText);
	}

	pf->GetHeaderInterface()->SetText(nIndex, sText);
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
 * @brief Send signal to thread to pause current scan
 */
void CDirDoc::PauseCurrentScan()
{
	m_diffThread.Pause();
}

/**
 * @brief Send signal to thread to continue current scan
 */
void CDirDoc::ContinueCurrentScan()
{
	m_diffThread.Continue();
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
	if (strDesc != nullptr)
		std::copy_n(strDesc, m_nDirs, m_strDesc);
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
	if (m_pTempPathContext != nullptr)
	{
		sText.erase(0, m_pTempPathContext->m_strRoot[nIndex].length());
		sText.insert(0, m_pTempPathContext->m_strDisplayRoot[nIndex]);
	}
}

/**
 * @brief Set document title to given string or items compared.
 * 
 * Formats and sets caption for directory compare window. Caption
 * has left- and right-side paths separated with '-'.
 *
 * @param [in] lpszTitle New title for window. If this parameter
 * is not `nullptr` we use this string, otherwise format caption from
 * actual paths.
 */
void CDirDoc::SetTitle(LPCTSTR lpszTitle)
{
	if (m_pDirView == nullptr)
		return;

	if (lpszTitle != nullptr)
		CDocument::SetTitle(lpszTitle);
	else if (m_pCtxt == nullptr || m_pCtxt->GetLeftPath().empty() ||
		m_pCtxt->GetRightPath().empty() || 
		(m_nDirs > 2 && m_pCtxt->GetMiddlePath().empty()))
	{
		String title = _("Folder Comparison Results");
		CDocument::SetTitle(title.c_str());
	}
	else
	{
		String sTitle;
		String sDirName[3];
		for (int index = 0; index < m_nDirs; index++)
		{
			String strPath = m_pCtxt->GetPath(index);
			ApplyDisplayRoot(index, strPath);
			sDirName[index] = paths::FindFileName(strPath);
		}
		if (std::count(&sDirName[0], &sDirName[0] + m_nDirs, sDirName[0]) == m_nDirs)
			sTitle = sDirName[0] + strutils::format(_T(" x %d"), m_nDirs);
 		else
			sTitle = strutils::join(&sDirName[0], &sDirName[0] + m_nDirs, _T(" - "));
 		CDocument::SetTitle(sTitle.c_str());
	}	
}


/**
 * @brief Checks if current folders are opened from archive file.
 * @return true if we are inside archive, false otherwise.
 */
bool CDirDoc::IsArchiveFolders() const
{
	if (m_pTempPathContext != nullptr)
		return true;
	else
		return false;
}

void CDirDoc::Swap(int idx1, int idx2)
{
	std::swap(m_bRO[idx1], m_bRO[idx2]);
	std::swap(m_strDesc[idx1], m_strDesc[idx2]);
	if (m_pTempPathContext != nullptr)
		m_pTempPathContext->Swap(idx1, idx2);
	m_pCtxt->Swap(idx1, idx2);
	m_pCompareStats->Swap(idx1, idx2);
	for (int nIndex = 0; nIndex < m_nDirs; nIndex++)
		UpdateHeaderPath(nIndex);
	SetTitle(nullptr);
}

bool CDirDoc::MoveableToNextDiff()
{
	if (m_pDirView == nullptr)
		return false;
	CMessageBoxDialog dlg(nullptr, _("Do you want to move to the next file?").c_str());
	const int nFormerResult = dlg.GetFormerResult();
	if (nFormerResult != -1 && nFormerResult == IDNO)
		return false;
	return m_pDirView->HasNextDiff();
}

bool CDirDoc::MoveableToPrevDiff()
{
	if (m_pDirView == nullptr)
		return false;
	CMessageBoxDialog dlg(nullptr, _("Do you want to move to the previous file?").c_str());
	const int nFormerResult = dlg.GetFormerResult();
	if (nFormerResult != -1 && nFormerResult == IDNO)
		return false;
	return m_pDirView->HasPrevDiff();
}

void CDirDoc::MoveToNextDiff(IMergeDoc *pMergeDoc)
{
	if (m_pDirView == nullptr)
		return;
	if (AfxMessageBox(_("Do you want to move to the next file?").c_str(), MB_YESNO | MB_DONT_ASK_AGAIN) == IDYES)
	{
		pMergeDoc->CloseNow();
		m_pDirView->OpenNextDiff();
		GetMainFrame()->OnUpdateFrameTitle(FALSE);
	}
}

void CDirDoc::MoveToPrevDiff(IMergeDoc *pMergeDoc)
{
	if (m_pDirView == nullptr)
		return;
	if (AfxMessageBox(_("Do you want to move to the previous file?").c_str(), MB_YESNO | MB_DONT_ASK_AGAIN) == IDYES)
	{
		pMergeDoc->CloseNow();
		m_pDirView->OpenPrevDiff();
		GetMainFrame()->OnUpdateFrameTitle(FALSE);
	}
}

