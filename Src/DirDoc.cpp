/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  DirDoc.cpp
 *
 * @brief Implementation file for CDirDoc
 *
 */

#include "StdAfx.h"
#include "DirDoc.h"
#if !defined(__cppcheck__)
#include <boost/range/mfc.hpp>
#endif
#include "Merge.h"
#include "IMergeDoc.h"
#include "CompareOptions.h"
#include "UnicodeString.h"
#include "CompareStats.h"
#include "FilterList.h"
#include "SubstitutionList.h"
#include "DirView.h"
#include "DirFrame.h"
#include "MainFrm.h"
#include "paths.h"
#include "7zCommon.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsDiffOptions.h"
#include "LineFiltersList.h"
#include "SubstitutionFiltersList.h"
#include "FileFilterHelper.h"
#include "FilterExpression.h"
#include "FilterErrorMessages.h"
#include "DirActions.h"
#include "DirScan.h"
#include "MessageBoxDialog.h"
#include "DirCmpReport.h"
#include "DiffWrapper.h"
#include "FolderCmp.h"
#include "DirViewColItems.h"
#include <Poco/Semaphore.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using boost::begin;
using boost::end;

int CDirDoc::m_nDirsTemp = 2;
// If a folder comparison takes longer than this threshold,
// display a confirmation message when closing the folder comparison window.
static constexpr int COMPARISON_TIME_THRESHOLD_SECONDS = 30;

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
, m_compareStart(0)
, m_elapsed(0)
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
	// Progress dialog
	ON_BN_CLICKED(IDC_COMPARISON_STOP, OnBnClickedComparisonStop)
	ON_BN_CLICKED(IDC_COMPARISON_PAUSE, OnBnClickedComparisonPause)
	ON_BN_CLICKED(IDC_COMPARISON_CONTINUE, OnBnClickedComparisonContinue)
	ON_CBN_SELCHANGE(IDC_COMPARISON_CPUCORES, OnCbnSelChangeCPUCores)
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

	CMergeFrameCommon::LogComparisonStart(paths, m_strDesc, nullptr, nullptr);
}


/**
 * @brief Load line filters to the compare context.
 * Loads linefilters, converts them to UTF-8 and sets them for compare context.
 */
void CDirDoc::LoadLineFilterList(CDiffContext *pCtxt)
{
	ASSERT(pCtxt != nullptr);
	
	bool bFilters = GetOptionsMgr()->GetBool(OPT_LINEFILTER_ENABLED);
	auto filters = theApp.m_pLineFilters->MakeFilterList(false);
	if (!bFilters || !filters->HasRegExps())
	{
		pCtxt->m_pFilterList.reset();
		return;
	}
	pCtxt->m_pFilterList = filters;
}

void CDirDoc::LoadSubstitutionFiltersList(CDiffContext* pCtxt)
{
	ASSERT(pCtxt != nullptr);

	bool SubstitutionFiltersEnabled = theApp.m_pSubstitutionFiltersList->GetEnabled();
	if (!SubstitutionFiltersEnabled || theApp.m_pSubstitutionFiltersList->GetCount() == 0)
	{
		pCtxt->m_pSubstitutionList.reset();
		return;
	}

	pCtxt->m_pSubstitutionList = theApp.m_pSubstitutionFiltersList->MakeSubstitutionList();
}

void CDirDoc::DiffThreadCallback(int& state)
{
	if (state == CDiffThread::EVENT_COMPARE_COMPLETED)
		m_elapsed = clock() - m_compareStart;
	PostMessage(m_pDirView->GetSafeHwnd(), MSG_UI_UPDATE, state, false);
}

void CDirDoc::InitDiffContext(CDiffContext *pCtxt)
{
	LoadLineFilterList(pCtxt);
	LoadSubstitutionFiltersList(pCtxt);

	DIFFOPTIONS options = {0};
	Options::DiffOptions::Load(GetOptionsMgr(), options);

	pCtxt->CreateCompareOptions(GetOptionsMgr()->GetInt(OPT_CMP_METHOD), options);

	pCtxt->m_iGuessEncodingType = GetOptionsMgr()->GetInt(OPT_CP_DETECT);
	if ((pCtxt->m_iGuessEncodingType >> 16) == 0)
		pCtxt->m_iGuessEncodingType |= 50001 << 16;
	pCtxt->m_bIgnoreSmallTimeDiff = GetOptionsMgr()->GetBool(OPT_IGNORE_SMALL_FILETIME);
	pCtxt->m_bStopAfterFirstDiff = GetOptionsMgr()->GetBool(OPT_CMP_STOP_AFTER_FIRST);
	pCtxt->m_nQuickCompareLimit = GetOptionsMgr()->GetInt(OPT_CMP_QUICK_LIMIT);
	pCtxt->m_nBinaryCompareLimit = GetOptionsMgr()->GetInt(OPT_CMP_BINARY_LIMIT);
	pCtxt->m_bPluginsEnabled = GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED);
	pCtxt->m_bWalkUniques = GetOptionsMgr()->GetBool(OPT_CMP_WALK_UNIQUE_DIRS);
	pCtxt->m_bIgnoreReparsePoints = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_REPARSE_POINTS);
	pCtxt->m_bIgnoreCodepage = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CODEPAGE);
	pCtxt->m_bEnableImageCompare = GetOptionsMgr()->GetBool(OPT_CMP_ENABLE_IMGCMP_IN_DIRCMP);
	pCtxt->m_dColorDistanceThreshold = GetOptionsMgr()->GetInt(OPT_CMP_IMG_THRESHOLD) / 1000.0;

	m_imgfileFilter.SetMaskOrExpression(GetOptionsMgr()->GetString(OPT_CMP_IMG_FILEPATTERNS));
	pCtxt->m_pImgfileFilter = &m_imgfileFilter;

	pCtxt->m_pCompareStats = m_pCompareStats.get();

	// Make sure filters are up-to-date
	auto* pGlobalFileFilter = theApp.GetGlobalFileFilter();
	pGlobalFileFilter->ReloadUpdatedFilters();
	m_fileHelper.CloneFrom(pGlobalFileFilter);
	pCtxt->m_piFilterGlobal = &m_fileHelper;
	pCtxt->m_piFilterGlobal->SetDiffContext(pCtxt);
	
	pCtxt->m_pAdditionalCompareExpression.reset();
	const String additionalCompareCondition = GetOptionsMgr()->GetString(OPT_CMP_ADDITIONAL_CONDITION);
	if (!additionalCompareCondition.empty())
	{
		pCtxt->m_pAdditionalCompareExpression = std::make_unique<FilterExpression>(ucr::toUTF8(additionalCompareCondition));
		pCtxt->m_pAdditionalCompareExpression->SetDiffContext(pCtxt);
	}

	std::vector<String> names;
	if (m_pDirView)
		names = m_pDirView->GetDirViewColItems()->GetAdditionalPropertyNames();
	std::vector<String> names2 = pGlobalFileFilter->GetPropertyNames();
	for (const auto& name : names2)
	{
		if (std::find(std::begin(names), std::end(names), name) == std::end(names))
			names.push_back(name);
	}
	if (pCtxt->m_pAdditionalCompareExpression)
	{
		const auto names3 = pCtxt->m_pAdditionalCompareExpression->GetPropertyNames();
		for (const auto& name : names3)
		{
			String tname = ucr::toTString(name);
			if (std::find(std::begin(names), std::end(names), tname) == std::end(names))
				names.push_back(tname);
		}
	}
	pCtxt->m_pPropertySystem.reset(new PropertySystem(names));

	// All plugin management is done by our plugin manager
	pCtxt->m_piPluginInfos = GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED) ? &m_pluginman : nullptr;

	CheckFilter();
	FilterExpression::SetLogger([](const std::string& msg) {
		RootLogger::Error(msg);
		});
}

void CDirDoc::CheckFilter()
{
	if (!m_pCtxt || !m_pCtxt->m_piFilterGlobal)
		return;
	for (const auto* error: m_pCtxt->m_piFilterGlobal->GetErrorList())
	{
		const String msg = FormatFilterErrorSummary(*error);
		RootLogger::Error(msg);
	}
	if (m_pCtxt->m_pAdditionalCompareExpression && m_pCtxt->m_pAdditionalCompareExpression->errorCode != 0)
	{
		const String msg = FormatFilterErrorSummary(*m_pCtxt->m_pAdditionalCompareExpression);
		RootLogger::Error(msg);
		m_pCtxt->m_pAdditionalCompareExpression.reset();
	}
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

	m_compareStart = clock();

	if (m_pCmpProgressBar == nullptr)
		m_pCmpProgressBar.reset(new DirCompProgressBar());

	if (!::IsWindow(m_pCmpProgressBar->GetSafeHwnd()))
		m_pCmpProgressBar->Create(m_pDirView->GetParentFrame());

	m_pCmpProgressBar->SetCompareStat(m_pCompareStats.get());
	m_pCmpProgressBar->StartUpdating();

	m_pDirView->GetParentFrame()->ShowControlBar(m_pCmpProgressBar.get(), TRUE, FALSE);

	if (!m_bGeneratingReport)
		m_pDirView->DeleteAllDisplayItems();
	// Don't clear if only scanning selected items
	if (!m_bMarkedRescan && !m_bGeneratingReport)
	{
		m_pCtxt->RemoveAll();
		m_pCtxt->InitDiffItemList();
	}

	InitDiffContext(m_pCtxt.get());

	auto* pHeaderBar = pf->GetHeaderInterface();
	pHeaderBar->SetPaneCount(m_nDirs);
	pHeaderBar->SetOnSetFocusCallback([&](int pane) {
		m_pDirView->SetActivePane(pane);
		GetOptionsMgr()->SaveOption(OPT_ACTIVE_PANE, pane);
	});
	pHeaderBar->SetOnCaptionChangedCallback([&](int pane, const String& sText) {
		m_strDesc[pane] = sText;
		UpdateHeaderPath(pane);
	});
	pHeaderBar->SetOnFolderSelectedCallback([&](int pane, const String& sFolderpath) {
		PathContext paths = m_pCtxt->GetNormalizedPaths();
		paths.SetPath(pane, sFolderpath);
		m_strDesc[pane].clear();
		InitCompare(paths, m_pCtxt->m_bRecursive, nullptr);
		Rescan();
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
	m_pDirView->GetParentFrame()->SetStatus(_("Comparing items...").c_str());

	// Show current compare method name and active filter name in statusbar
	pf->SetFilterStatusDisplay(theApp.GetGlobalFileFilter()->GetMaskOrExpression().c_str());
	pf->SetCompareMethodStatusDisplay(m_pCtxt->GetCompareMethod());

	// Folder names to compare are in the compare context
	m_diffThread.SetContext(m_pCtxt.get());
	m_diffThread.SetThreadCount(GetOptionsMgr()->GetInt(OPT_CMP_COMPARE_THREADS));
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
						I18n::MessageBox(IDS_REPORT_SUCCESS, MB_OK | MB_ICONINFORMATION);
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
		m_diffThread.SetMarkedRescan(false);
	}
	else if (m_bMarkedRescan)
	{
		m_diffThread.SetCollectFunction([](DiffFuncStruct* myStruct) {
			int nItems = DirScan_UpdateMarkedItems(myStruct, nullptr);
			myStruct->context->m_pCompareStats->IncreaseTotalItems(nItems);
			});
		m_diffThread.SetCompareFunction([](DiffFuncStruct* myStruct) {
			DirScan_CompareRequestedItems(myStruct, nullptr);
			});
		m_diffThread.SetMarkedRescan(true);
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
		m_diffThread.SetMarkedRescan(false);
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
	ASSERT(m_pDirView == static_cast<CDirView *>(GetNextView(pos))); // verify that our stashed pointer is the same as MFC's
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
	// Close and destroy the dialog after compare
	if (m_pCmpProgressBar != nullptr)
		m_pDirView->GetParentFrame()->ShowControlBar(m_pCmpProgressBar.get(), FALSE, FALSE);
	m_pCmpProgressBar.reset();
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
		int ans = I18n::MessageBox(IDS_CONFIRM_CLOSE_WINDOW_COMPARING, MB_YESNO | MB_ICONWARNING);
		if (ans == IDNO)
			return FALSE;
		m_diffThread.Abort();
		while (m_diffThread.GetThreadState() == CDiffThread::THREAD_COMPARING)
			Sleep(50);
		CompareReady();
	}

	if (m_elapsed >= COMPARISON_TIME_THRESHOLD_SECONDS * 1000)
	{
		int ans = I18n::MessageBox(IDS_CONFIRM_CLOSE_WINDOW_LONG_COMPARISON, MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN);
		if (ans == IDNO)
			return FALSE;
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
		if (sText.find(m_pTempPathContext->m_strRoot[nIndex]) == String::npos)
		{
			for (int pane = 0; pane < m_nDirs; ++pane)
			{
				if (sText.find(m_pTempPathContext->m_strRoot[pane]) != String::npos)
				{
					nIndex = pane;
					break;
				}
			}
		}
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
 * @brief A string to display as a tooltip for MDITabbar
 */
CString CDirDoc::GetTooltipString() const
{
	return CMergeFrameCommon::GetTooltipString(m_pCtxt->GetNormalizedPaths(), m_strDesc, nullptr, nullptr).c_str();
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
	if (m_diffThread.GetThreadState() != CDiffThread::THREAD_COMPLETED)
		return;
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
	CMessageBoxDialog dlg(nullptr, _("Move to next file?").c_str());
	const int nFormerResult = dlg.GetFormerResult();
	if (nFormerResult != -1 && nFormerResult == IDNO)
		return false;
	return m_pDirView->HasNextDiff();
}

bool CDirDoc::MoveableToPrevDiff()
{
	if (m_pDirView == nullptr)
		return false;
	CMessageBoxDialog dlg(nullptr, _("Move to previous file?").c_str());
	const int nFormerResult = dlg.GetFormerResult();
	if (nFormerResult != -1 && nFormerResult == IDNO)
		return false;
	return m_pDirView->HasPrevDiff();
}

void CDirDoc::MoveToNextDiff(IMergeDoc *pMergeDoc)
{
	if (m_pDirView == nullptr)
		return;
	if (AfxMessageBox(_("Move to next file?").c_str(), MB_YESNO | MB_DONT_ASK_AGAIN, IDS_MOVE_TO_NEXTFILE) == IDYES)
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
	if (AfxMessageBox(_("Move to previous file?").c_str(), MB_YESNO | MB_DONT_ASK_AGAIN, IDS_MOVE_TO_PREVFILE) == IDYES)
	{
		pMergeDoc->CloseNow();
		m_pDirView->OpenPrevDiff();
		GetMainFrame()->OnUpdateFrameTitle(FALSE);
	}
}

void CDirDoc::MoveToFirstFile(IMergeDoc* pMergeDoc)
{
	if (m_pDirView == nullptr)
		return;
	if (AfxMessageBox(_("Move to first file?").c_str(), MB_YESNO | MB_DONT_ASK_AGAIN, IDS_MOVE_TO_FIRSTFILE) == IDYES)
	{
		pMergeDoc->CloseNow();
		m_pDirView->OpenFirstFile();
		GetMainFrame()->OnUpdateFrameTitle(FALSE);
	}
}

void CDirDoc::MoveToNextFile(IMergeDoc* pMergeDoc)
{
	if (m_pDirView == nullptr)
		return;
	if (AfxMessageBox(_("Move to next file?").c_str(), MB_YESNO | MB_DONT_ASK_AGAIN, IDS_MOVE_TO_NEXTFILE) == IDYES)
	{
		pMergeDoc->CloseNow();
		m_pDirView->OpenNextFile();
		GetMainFrame()->OnUpdateFrameTitle(FALSE);
	}
}

void CDirDoc::MoveToPrevFile(IMergeDoc* pMergeDoc)
{
	if (m_pDirView == nullptr)
		return;
	if (AfxMessageBox(_("Move to previous file?").c_str(), MB_YESNO | MB_DONT_ASK_AGAIN, IDS_MOVE_TO_PREVFILE) == IDYES)
	{
		pMergeDoc->CloseNow();
		m_pDirView->OpenPrevFile();
		GetMainFrame()->OnUpdateFrameTitle(FALSE);
	}
}

void CDirDoc::MoveToLastFile(IMergeDoc* pMergeDoc)
{
	if (m_pDirView == nullptr)
		return;
	if (AfxMessageBox(_("Move to last file?").c_str(), MB_YESNO | MB_DONT_ASK_AGAIN, IDS_MOVE_TO_LASTFILE) == IDYES)
	{
		pMergeDoc->CloseNow();
		m_pDirView->OpenLastFile();
		GetMainFrame()->OnUpdateFrameTitle(FALSE);
	}
}

bool CDirDoc::IsFirstFile()
{
	if (m_pDirView == nullptr)
		return true;
	return m_pDirView->IsFirstFile();
}

bool CDirDoc::IsLastFile()
{
	if (m_pDirView == nullptr)
		return true;
	return m_pDirView->IsLastFile();
}

bool CDirDoc::CompareFilesIfFilesAreLarge(int nFiles, const FileLocation ifileloc[])
{
	DIFFITEM di;
	bool bLargeFile = false;
	for (int i = 0; i < nFiles; ++i)
	{
		di.diffFileInfo[i].SetFile(paths::FindFileName(ifileloc[i].filepath));
		if (di.diffFileInfo[i].Update(ifileloc[i].filepath))
			di.diffcode.setSideFlag(i);
	}
	if (nFiles == 3)
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

	size_t fileSizeThreshold = GetOptionsMgr()->GetInt(OPT_FILE_SIZE_THRESHOLD);
	for (int i = 0; i < nFiles; ++i)
	{
		if (di.diffFileInfo[i].size != DirItem::FILE_SIZE_NONE && di.diffFileInfo[i].size > fileSizeThreshold)
			bLargeFile = true;
	}
	if (!bLargeFile)
		return false;

	PathContext paths;
	for (int i = 0; i < nFiles; ++i)
		paths.SetPath(i, ifileloc[i].filepath.empty() ? paths::NATIVE_NULL_DEVICE_NAME : paths::GetParentPath(ifileloc[i].filepath));
	CDiffContext ctxt(paths, CMP_QUICK_CONTENT);
	DirViewColItems ci(nFiles, std::vector<String>{});
	String msg = I18n::LoadString(IDS_COMPARE_LARGE_FILES) + _T("\n");
	if (nFiles < 3)
	{
		String sidestr[] = { _("Left:"), _("Right:") };
		for (int i = 0; i < nFiles; ++i)
		{
			if (ifileloc[i].filepath.empty())
			{
				msg += strutils::format(_T("%s %s\n\n"), sidestr[i], _("None"));
			}
			else
			{
				msg += strutils::format(_T("%s %s\n %s:  %s\n %s: %s (%s)\n\n"),
					sidestr[i].c_str(), ifileloc[i].filepath.c_str(),
					ci.GetColDisplayName(3 + i).c_str(), ci.ColGetTextToDisplay(&ctxt, 3 + i, di).c_str(),
					ci.GetColDisplayName(8 + i).c_str(), ci.ColGetTextToDisplay(&ctxt, 8 + i, di).c_str(), ci.ColGetTextToDisplay(&ctxt, 10 + i, di).c_str());
			}
		}
	}
	else
	{
		String sidestr[] = { _("Left:"), _("Middle:"), _("Right:") };
		for (int i = 0; i < nFiles; ++i)
		{
			if (ifileloc[i].filepath.empty())
			{
				msg += strutils::format(_T("%s %s\n\n"), sidestr[i], _("None"));
			}
			else
			{
				msg += strutils::format(_T("%s %s\n %s:  %s\n %s: %s (%s)\n\n"),
					sidestr[i].c_str(), ifileloc[i].filepath.c_str(),
					ci.GetColDisplayName(3 + i).c_str(), ci.ColGetTextToDisplay(&ctxt, 3 + i, di).c_str(),
					ci.GetColDisplayName(10 + i).c_str(), ci.ColGetTextToDisplay(&ctxt, 10 + i, di).c_str(), ci.ColGetTextToDisplay(&ctxt, 13 + i, di).c_str());
			}
		}
	}
	CMessageBoxDialog dlg(
		m_pDirView ? m_pDirView->GetParentFrame() : nullptr,
		msg.c_str(), _T(""),
		MB_YESNOCANCEL | MB_ICONQUESTION | MB_DONT_ASK_AGAIN, 0U, _T("CompareLargeFiles"));
	INT_PTR ans = dlg.GetFormerResult();
	if (ans != -1 || !theApp.GetNonInteractive())
	{
		ans = theApp.DoMessageBox(msg.c_str(),
			MB_YESNOCANCEL | MB_ICONQUESTION | MB_DONT_ASK_AGAIN, 0U, _T("CompareLargeFiles"));
		if (ans == IDCANCEL)
			return true;
		else if (ans == IDNO)
			return false;
	}

	int oldCompareMethod = GetOptionsMgr()->GetInt(OPT_CMP_METHOD);
	GetOptionsMgr()->SaveOption(OPT_CMP_METHOD, CMP_QUICK_CONTENT); // Use quick content compare for large files
	InitDiffContext(&ctxt);
	GetOptionsMgr()->SaveOption(OPT_CMP_METHOD, oldCompareMethod); // Restore previous compare method
	FolderCmp cmp(&ctxt);
	CWaitCursor waitstatus;
	di.diffcode.diffcode |= cmp.prepAndCompareFiles(di);
	if (di.diffcode.isResultSame())
	{
		ctxt.GetComparePaths(di, paths);
		CMergeFrameCommon::ShowIdenticalMessage(paths, true);
	}
	else
	{
		AfxMessageBox(ci.ColGetTextToDisplay(&ctxt, 2, di).c_str(), MB_OK | MB_ICONINFORMATION);
	}
	theApp.SetLastCompareResult(di.diffcode.isResultDiff() ? 1 : 0);
	return true;
}

void CDirDoc::OnBnClickedComparisonStop()
{
	if (m_pCmpProgressBar != nullptr)
		m_pCmpProgressBar->EndUpdating();
	AbortCurrentScan();
}

void CDirDoc::OnBnClickedComparisonPause()
{
	if (m_pCmpProgressBar != nullptr)
		m_pCmpProgressBar->SetPaused(true);
	PauseCurrentScan();
}

void CDirDoc::OnBnClickedComparisonContinue()
{
	if (m_pCmpProgressBar != nullptr)
		m_pCmpProgressBar->SetPaused(false);
	ContinueCurrentScan();
}

void CDirDoc::OnCbnSelChangeCPUCores()
{
	if (!m_pCmpProgressBar)
		return;
	m_pCtxt->m_pCompareStats->SetIdleCompareThreadCount(
		m_pCtxt->m_pCompareStats->GetCompareThreadCount() - m_pCmpProgressBar->GetNumberOfCPUCoresToUse()
	);
}
