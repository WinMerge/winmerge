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
	m_pFilter = NULL;
	m_bRecursive = FALSE;
}

CDirDoc::~CDirDoc()
{
	if (m_pCtxt != NULL)
		delete m_pCtxt;
	// Inform all of our merge docs that we're closing
	for (POSITION pos = m_MergeDocs.GetHeadPosition(); pos; )
	{
		CMergeDoc * pMergeDoc = m_MergeDocs.GetNext(pos);
		pMergeDoc->DirDocClosing(this);
	}
	
	if (m_pFilter != NULL)
		delete m_pFilter;
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

// Perform directory comparison again from scratch
void CDirDoc::Rescan()
{
	if (!m_pCtxt) return;

	// If we're already doing a rescan, bail out
	UINT threadState = m_diffThread.GetThreadState();
	if (threadState == THREAD_COMPARING)
		return;

	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_RESCANNING));

	gLog.Write(_T("Starting directory scan:\r\n\tLeft: %s\r\n\tRight: %s\r\n"),
			m_pCtxt->m_strLeft, m_pCtxt->m_strRight);
	m_pCtxt->RemoveAll();
	mf->clearStatus();

	// fix up for diff code (remove trailing slashes etc)
	m_pCtxt->m_strNormalizedLeft = m_pCtxt->m_strLeft;
	m_pCtxt->m_strNormalizedRight = m_pCtxt->m_strRight;
	m_pCtxt->m_hMainFrame = mf->GetSafeHwnd();
	m_pCtxt->m_msgUpdateStatus = MSG_STAT_UPDATE;
	paths_normalize(m_pCtxt->m_strNormalizedLeft);
	paths_normalize(m_pCtxt->m_strNormalizedRight);

	if (m_pFilter == NULL)
		m_pFilter = new DirDocFilter;

	m_pCtxt->m_piFilter = m_pFilter;

	// Empty display before new compare
	m_pDirView->DeleteAllDisplayItems();

	m_diffWrapper.StartDirectoryDiff();
	
	m_diffThread.SetContext(m_pCtxt);
	m_diffThread.SetHwnd(m_pDirView->GetSafeHwnd());
	m_diffThread.SetMessageIDs(MSG_UI_UPDATE, MSG_STAT_UPDATE);
	m_diffThread.CompareDirectories(m_pCtxt->m_strNormalizedLeft,
			m_pCtxt->m_strNormalizedRight, m_bRecursive);

	CString s;
	AfxFormatString2(s, IDS_DIRECTORY_WINDOW_STATUS_FMT, m_pCtxt->m_strLeft, m_pCtxt->m_strRight);
	((CDirFrame*)(m_pDirView->GetParent()))->SetStatus(s);
}

// return true if we need to hide this item because it is a backup
static bool IsItemHiddenBackup(const DIFFITEM & di)
{
	return mf->m_bHideBak && FileExtMatches(di.sfilename,BACKUP_FILE_EXT);
}

// returns path of item if user wants to see this item
// preferably left path, but right path if a right-only item
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

	POSITION diffpos = m_pCtxt->GetFirstDiffPosition();
	while (diffpos)
	{
		POSITION curdiffpos = diffpos;
		DIFFITEM di = m_pCtxt->GetNextDiffPosition(diffpos);

		LPCTSTR p=GetItemPathIfShowable(di, llen, rlen);

		if (p)
		{
			int i = m_pDirView->AddDiffItem(cnt, di, p, curdiffpos);
			UpdateScreenItemStatus(i, di);
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
 * @brief Update in-memory diffitem status from disk
 */
void CDirDoc::ReloadItemStatus(UINT nIdx)
{
	POSITION diffpos = m_pDirView->GetItemKey(nIdx);
	DIFFITEM di = m_pCtxt->GetDiffAt(diffpos);

	m_pCtxt->UpdateInfoFromDisk(di); // in case just copied (into existence) or modified
	UpdateScreenItemStatus(nIdx, di);
}

/**
 * @brief Push current in-memory diffitem status out to screen for selected item
 */
void CDirDoc::UpdateScreenItemStatus(UINT nIdx, DIFFITEM di)
{
	m_pDirView->UpdateDiffItemStatus(nIdx, di);
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
	CDirView * currentView = m_pDirView;
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

	// delete comparison parameters and results
	if (m_pCtxt != NULL)
		delete m_pCtxt;
	m_pCtxt = NULL;
	if (m_pFilter != NULL)
		delete m_pFilter;
	m_pFilter = NULL;

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
 * @brief Item specified has changed on disk (and is now the same if unified==true).
 */
void CDirDoc::UpdateChangedItem(LPCTSTR pathLeft, LPCTSTR pathRight, bool unified)
{
	POSITION pos = FindItemFromPaths(pathLeft, pathRight);
	ASSERT(pos);
	int ind = m_pDirView->GetItemIndex((DWORD)pos);

	// Get index at view, update filetimes to context
	// and tell view to update found item
	m_pCtxt->UpdateStatusFromDisk(pos);

	// Figure out new status code
	UINT diffcode = (unified ? DIFFCODE::SAME : DIFFCODE::DIFF);
	// Save new status code to diff context memory
	m_pCtxt->SetDiffStatusCode(pos, diffcode, DIFFCODE::COMPAREFLAGS);

	// Update view
	const DIFFITEM & updated = m_pCtxt->GetDiffAt(pos);
	UpdateScreenItemStatus(ind, updated);
}

/**
 * @brief Cleans up after directory compare
 */
void CDirDoc::CompareReady()
{
	gLog.Write(_T("Directory scan complete\r\n"));
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

void CDirDoc::SetDiffSide(UINT diffcode, int idx)
{
	SetDiffStatus(diffcode, DIFFCODE::SIDEFLAG, idx);
}

void CDirDoc::SetDiffCompare(UINT diffcode, int idx)
{
	SetDiffStatus(diffcode, DIFFCODE::COMPAREFLAGS, idx);
}

void CDirDoc::SetDiffStatus(UINT diffcode, UINT mask, int idx)
{
	CDirView *pv = GetMainView();
	ASSERT(pv);
	// first change it in the dirlist
	POSITION diffpos = pv->GetItemKey(idx);

	// TODO: Why is the update broken into these pieces ?
	// Someone could figure out these pieces and probably simplify this.

	// update DIFFITEM code
	m_pCtxt->SetDiffStatusCode(diffpos, diffcode, mask);
	// update DIFFITEM time, and also tell views
	ReloadItemStatus(idx);
}

