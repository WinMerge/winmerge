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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CLogFile gLog;

/////////////////////////////////////////////////////////////////////////////
// CDirDoc

IMPLEMENT_DYNCREATE(CDirDoc, CDocument)

CDirDoc::CDirDoc()
{
	m_pDirView = NULL;
	m_pCtxt=NULL;
	m_bReuseMergeDocs = TRUE;
	m_pFilterGlobal = NULL;
	m_pFilterUI = NULL;
	m_bROLeft = FALSE;
	m_bRORight = FALSE;
	m_bRecursive = FALSE;
	m_statusCursor = NULL;
}

CDirDoc::~CDirDoc()
{
	delete m_pCtxt;
	// Inform all of our merge docs that we're closing
	for (POSITION pos = m_MergeDocs.GetHeadPosition(); pos; )
	{
		CMergeDoc * pMergeDoc = m_MergeDocs.GetNext(pos);
		pMergeDoc->DirDocClosing(this);
	}
	
	delete m_pFilterGlobal;
	delete m_pFilterUI;
}

// callback we give our frame
// which allows us to control whether or not it closes
static bool DocClosableCallback(void * param)
{
	CDirDoc * pDoc = reinterpret_cast<CDirDoc *>(param);
	return pDoc->CanFrameClose();
}

bool CDirDoc::CanFrameClose()
{
	return !!m_MergeDocs.IsEmpty();
}

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
// CDirDoc diagnostics

#ifdef _DEBUG
void CDirDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDirDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

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

/*int
diff_dirs2 (filevec, handle_file, depth)
     struct file_data const filevec[];
     int (*handle_file) PARAMS((char const *, char const *, char const *, char const *, int));
     int depth;
{
  struct dirdata dirdata[2];
  int val = 0;
  int i;

  // Get sorted contents of both dirs.
  for (i = 0; i < 2; i++)
    if (dir_sort (&filevec[i], &dirdata[i]) != 0)
      {
	perror_with_name (filevec[i].name);
	val = 2;
      }

  if (val == 0)
    {
      char const * const *names0 = dirdata[0].names;
      char const * const *names1 = dirdata[1].names;
      char const *name0 = filevec[0].name;
      char const *name1 = filevec[1].name;

      // If `-S name' was given, and this is the topmost level of comparison,
	 //ignore all file names less than the specified starting name.

      if (dir_start_file && depth == 0)
	{
	  while (*names0 && stricmp (*names0, dir_start_file) < 0)
	    names0++;
	  while (*names1 && stricmp (*names1, dir_start_file) < 0)
	    names1++;
	}

      // Loop while files remain in one or both dirs.
      while (*names0 || *names1)
	{
	  // Compare next name in dir 0 with next name in dir 1.
	     At the end of a dir,
	     pretend the "next name" in that dir is very large.
	  int nameorder = (!*names0 ? 1 : !*names1 ? -1
			   : stricmp (*names0, *names1));
	  int v1 = (*handle_file) (name0, 0 < nameorder ? 0 : *names0++,
				   name1, nameorder < 0 ? 0 : *names1++,
				   depth + 1);
	  if (v1 > val)
	    val = v1;
	}
    }

  for (i = 0; i < 2; i++)
    {
      if (dirdata[i].names)
	free (dirdata[i].names);
      if (dirdata[i].data)
	free (dirdata[i].data);
    }

  return val;
}*/

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

	gLog.Write(_T("Starting directory scan:\r\n\tLeft: %s\r\n\tRight: %s\r\n"),
			m_pCtxt->m_strLeft, m_pCtxt->m_strRight);
	pf->clearStatus();
	pf->ShowProcessingBar(TRUE);
	m_pCtxt->RemoveAll();

	// fix up for diff code (remove trailing slashes etc)
	m_pCtxt->m_strNormalizedLeft = m_pCtxt->m_strLeft;
	m_pCtxt->m_strNormalizedRight = m_pCtxt->m_strRight;
	m_pCtxt->m_hDirFrame = pf->GetSafeHwnd();
	m_pCtxt->m_msgUpdateStatus = MSG_STAT_UPDATE;
	paths_normalize(m_pCtxt->m_strNormalizedLeft);
	paths_normalize(m_pCtxt->m_strNormalizedRight);
	UpdateHeaderPath(TRUE);
	UpdateHeaderPath(FALSE);
	// draw the headers as active ones
	pf->GetHeaderInterface()->SetActive(0, TRUE);
	pf->GetHeaderInterface()->SetActive(1, TRUE);

	if (m_pFilterGlobal == NULL)
		m_pFilterGlobal = new DirDocFilterGlobal;
	m_pCtxt->m_piFilterGlobal = m_pFilterGlobal;

	if (m_pFilterUI == NULL)
		m_pFilterUI = new DirDocFilterByExtension(m_pCtxt->m_strRegExp);
	m_pCtxt->m_piFilterUI = m_pFilterUI;

	// Empty display before new compare
	m_pDirView->DeleteAllDisplayItems();

	m_diffWrapper.StartDirectoryDiff();
	
	m_diffThread.SetContext(m_pCtxt);
	m_diffThread.SetHwnd(m_pDirView->GetSafeHwnd());
	m_diffThread.SetMessageIDs(MSG_UI_UPDATE, MSG_STAT_UPDATE);
	m_diffThread.CompareDirectories(m_pCtxt->m_strNormalizedLeft,
			m_pCtxt->m_strNormalizedRight, m_bRecursive);

	// the directories are now displayed in the headerbar
	// CString s;
	// AfxFormatString2(s, IDS_DIRECTORY_WINDOW_STATUS_FMT, m_pCtxt->m_strLeft, m_pCtxt->m_strRight);
	// ((CDirFrame*)(m_pDirView->GetParent()))->SetStatus(s);
}

/**
 * @brief Determines if item is backup item and needed to be hidden
 * @return true if item is a backup
 */
static bool IsItemHiddenBackup(const DIFFITEM & di)
{
	return mf->m_bHideBak && FileExtMatches(di.sfilename,BACKUP_FILE_EXT);
}

/**
 * @brief Determines if user wants to see this item
 * @return Path to item, NULL if user does not want to see it
 * @note Preferably left path, but right path if a right-only item
 */
static LPCTSTR GetItemPathIfShowable(const DIFFITEM & di, int llen, int rlen)
{
	if (IsItemHiddenBackup(di))
		return NULL;

	// file type filters
	if (di.isBin() && !mf->m_bShowBinaries)
		return 0;

	// result filters
	if (di.isResultSame() && !mf->m_bShowIdent)
		return 0;
	if (di.isResultError() && !mf->m_bShowErrors)
		return 0;
	if (di.isResultSkipped() && !mf->m_bShowSkipped)
		return 0;
	if (di.isResultDiff() && !mf->m_bShowDiff)
		return 0;

	// left/right filters
	if (di.isSideLeft() && !mf->m_bShowUniqueLeft)
		return 0;
	if (di.isSideRight() && !mf->m_bShowUniqueRight)
		return 0;


	LPCTSTR p = NULL;
	if (di.isSideRight())
		p = _tcsninc(di.getRightFilepath(), rlen);
	else
		p = _tcsninc(di.getLeftFilepath(), llen);

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

		LPCTSTR p=GetItemPathIfShowable(di, llen, rlen);

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
 * @brief Get modification time of specified file
 */
static long GetModTime(LPCTSTR szPath)
{
	struct _stat mystats;
	bzero(&mystats, sizeof(mystats));
	int stat_result = _tstat(szPath, &mystats);
	if (stat_result!=0)
		return 0;
	return mystats.st_mtime;
}


/**
 * @brief Update in-memory diffitem status from disk and update view
 * @param nIdx Index of item in UI list
 */
void CDirDoc::ReloadItemStatus(UINT nIdx)
{
	// Get position of item in DiffContext
	POSITION diffpos = m_pDirView->GetItemKey(nIdx);

	m_pCtxt->UpdateStatusFromDisk(diffpos); // in case just copied (into existence) or modified

	// Update view
	const DIFFITEM & updated = m_pCtxt->GetDiffAt(diffpos);
	m_pDirView->UpdateDiffItemStatus(nIdx, updated);
}

void CDirDoc::InitStatusStrings()
{

}

/// Update any language-dependent data
void CDirDoc::UpdateResources()
{
	m_pDirView->UpdateColumnNames();

	CString s;
	VERIFY(s.LoadString(IDS_DIRECTORY_WINDOW_TITLE));
	SetTitle(s);

	Redisplay();
}

void CDirDoc::SetDiffContext(CDiffContext *pCtxt)
{
	if (m_pCtxt != NULL)
		delete m_pCtxt;

	m_pCtxt = pCtxt;
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

	// Path can contain (because of difftools?) '/' and '\'
	// so for comparing purposes, convert whole path to use '\\'
	path1.Replace('/', '\\');
	path2.Replace('/', '\\');

	// Filenames must be identical
	if (file1 != file2)
		return NULL;

	// Get first item
	DIFFITEM current = m_pCtxt->GetDiffAt(pos);

	int count = m_pCtxt->GetDiffCount();
	for (int i=0; i < count; ++i)
	{
		// Save our current pos before getting next
		currentPos = pos;
		current = m_pCtxt->GetNextDiffPosition(pos);

		if (path1 == current.getLeftFilepath() &&
			path2 == current.getRightFilepath() &&
			file1 == current.sfilename)
		{
			return currentPos;
		}
	}
	return NULL;
}

// stash away our view pointer
void CDirDoc::SetDirView(CDirView * newView)
{
	m_pDirView = newView;
	// MFC has a view list for us, so lets check against it
	POSITION pos = GetFirstViewPosition();
	CDirView * temp = static_cast<CDirView *>(GetNextView(pos));
	ASSERT(temp == m_pDirView); // verify that our stashed pointer is the same as MFC's
}

// a new merge doc has been opened
void CDirDoc::AddMergeDoc(CMergeDoc * pMergeDoc)
{
	ASSERT(pMergeDoc);
	m_MergeDocs.AddTail(pMergeDoc);
}

// merge doc informs us it is closing
void CDirDoc::MergeDocClosing(CMergeDoc * pMergeDoc)
{
	ASSERT(pMergeDoc);
	POSITION pos = m_MergeDocs.Find(pMergeDoc);
	ASSERT(pos);
	m_MergeDocs.RemoveAt(pos);
}

// Prepare for reuse
// Close all our merge docs (which gives them chance to save)
// This may fail if user cancels a Save dialog
// in which case this aborts and returns FALSE
BOOL CDirDoc::ReusingDirDoc()
{
	// Inform all of our merge docs that we're closing
	for (POSITION pos = m_MergeDocs.GetHeadPosition(); pos; )
	{
		CMergeDoc * pMergeDoc = m_MergeDocs.GetNext(pos);
		if (!pMergeDoc->CloseNow())
			return FALSE;
	}

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
	if (m_pFilterGlobal != NULL)
		delete m_pFilterGlobal;
	m_pFilterGlobal = NULL;
	if (m_pFilterUI != NULL)
		delete m_pFilterUI;
	m_pFilterUI = NULL;

	return TRUE;
}

// Obtain a merge doc to display a difference in files
// pNew is set to TRUE if a new doc is created, and FALSE if an existing one reused
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
 * @param unified true if files became identical, false otherwise.
 * @note Filenames must be same, otherwise function asserts.
 */
void CDirDoc::UpdateChangedItem(LPCTSTR pathLeft, LPCTSTR pathRight, bool unified)
{
	POSITION pos = FindItemFromPaths(pathLeft, pathRight);
	ASSERT(pos);
	int ind = m_pDirView->GetItemIndex((DWORD)pos);

	// Figure out new status code
	UINT diffcode = (unified ? DIFFCODE::SAME : DIFFCODE::DIFF);

	// Update both view and diff context memory
	SetDiffCompare(diffcode, ind);
	ReloadItemStatus(ind);
}

/**
 * @brief Cleans up after directory compare
 */
void CDirDoc::CompareReady()
{
	gLog.Write(_T("Directory scan complete\r\n"));

	// finish the cursor (the hourglass/pointer combo) we had open during display
	delete m_statusCursor;
	m_statusCursor = 0;

	// hide the floating state bar
	// CDirFrame *pf = m_pDirView->GetParentFrame();
	// pf->ShowProcessingBar(FALSE);

	m_diffWrapper.EndDirectoryDiff();
}

/**
 * @brief Read doc settings from registry
 *
 * @note Currently loads only diffutils settings, but later others too
 */
void CDirDoc::ReadSettings()
{
	DIFFOPTIONS options;
	
	CDiffWrapper::ReadDiffOptions(&options);
	m_diffWrapper.SetOptions(&options);
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


BOOL DirDocFilterGlobal::includeDir(LPCTSTR szDirName) 
{ 
#ifdef _UNICODE
	// TODO
	// regexp has some problem in Unicode ? 2003-09-14
	// but 2003-10-24 let's test it
	// return TRUE;
#endif
	// preprend a backslash if there is none
	CString strDirName;
	if (strDirName != _T('\\'))
		strDirName = _T('\\');
	strDirName += szDirName;
	return theApp.includeDir(strDirName);
}
BOOL DirDocFilterGlobal::includeFile(LPCTSTR szFileName) 
{ 
#ifdef _UNICODE
	// TODO
	// regexp has some problem in Unicode ? 2003-09-14
	// but 2003-10-24 let's test it
	// return TRUE;
#endif
	// preprend a backslash if there is none
	CString strFileName = szFileName;
	if (strFileName[0] != _T('\\'))
		strFileName = _T('\\') + strFileName;
	// append a point if there is no extension
	if (strFileName.Find(_T('.')) == -1)
		strFileName = strFileName + _T('.');
	return theApp.includeFile(strFileName);
}

DirDocFilterByExtension::DirDocFilterByExtension(LPCTSTR strRegExp)
{
	m_rgx.RegComp( strRegExp );
}

BOOL DirDocFilterByExtension::includeDir(LPCTSTR szDirName) 
{ 
	// directory have no extension
	return TRUE;
}
BOOL DirDocFilterByExtension::includeFile(LPCTSTR szFileName) 
{ 
#ifdef _UNICODE
	// TODO
	// regexp has some problem in Unicode ? 2003-09-14
	// but 2003-10-24 let's test it
	// return TRUE;
#endif
	// preprend a backslash if there is none
	CString strFileName = szFileName;
	if (strFileName[0] != _T('\\'))
		strFileName = _T('\\') + strFileName;
	// append a point if there is no extension
	if (strFileName.Find(_T('.')) == -1)
		strFileName = strFileName + _T('.');
	return (! m_rgx.RegFind(strFileName));
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