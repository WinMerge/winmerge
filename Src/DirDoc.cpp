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
// DirDoc.cpp : implementation file
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

extern int recursive;

int compare_files (LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR, CDiffContext*, int);


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CLogFile gLog(_T("WinMerge.log"), NULL, TRUE);
bool gWriteLog = true;



/////////////////////////////////////////////////////////////////////////////
// CDirDoc

IMPLEMENT_DYNCREATE(CDirDoc, CDocument)

CDirDoc::CDirDoc()
{
	m_pView = NULL;
	m_pCtxt=NULL;
}

CDirDoc::~CDirDoc()
{
	mf->m_pDirDoc = NULL;
	if (m_pCtxt != NULL)
		delete m_pCtxt;
}

BOOL CDirDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CString s;
	VERIFY(s.LoadString(IDS_DIRECTORY_WINDOW_TITLE));
	SetTitle(s);

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


void CDirDoc::Rescan()
{
	ASSERT(m_pCtxt != NULL);
	BeginWaitCursor();

	if (gWriteLog) gLog.Write(_T("Starting directory scan:\r\n\tLeft: %s\r\n\tRight: %s\r\n"),
			m_pCtxt->m_strLeft, m_pCtxt->m_strRight);
	m_pCtxt->RemoveAll();

	compare_files (0, (char const *)(LPCTSTR)m_pCtxt->m_strLeft,
			       0, (char const *)(LPCTSTR)m_pCtxt->m_strRight, m_pCtxt, 0);

	if (gWriteLog) gLog.Write(_T("Directory scan complete\r\n"));

	CString s;
	AfxFormatString2(s, IDS_DIRECTORY_WINDOW_STATUS_FMT, m_pCtxt->m_strLeft, m_pCtxt->m_strRight);
	((CDirFrame*)(m_pView->GetParent()))->SetStatus(s);
	Redisplay();

	EndWaitCursor();
}

void CDirDoc::Redisplay()
{
	if (m_pCtxt == NULL)
		return;

	CString s,s2;
	UINT cnt=0;
	int llen = m_pCtxt->m_strLeft.GetLength();
	int rlen = m_pCtxt->m_strRight.GetLength();

	m_pView->DeleteAllDisplayItems();

	POSITION diffpos = m_pCtxt->GetFirstDiffPosition();
	while (diffpos)
	{
		POSITION curdiffpos = diffpos;
		DIFFITEM di = m_pCtxt->GetNextDiffPosition(diffpos);

		LPCTSTR p=NULL;

		BOOL leftside = (di.code==FILE_LUNIQUE || di.code==FILE_LDIRUNIQUE);
		BOOL rightside = (di.code==FILE_RUNIQUE || di.code==FILE_RDIRUNIQUE);
		switch (di.code)
		{
		case FILE_DIFF:
			if (mf->m_bShowDiff
				&& (!mf->m_bHideBak || !FileExtMatches(di.filename,BACKUP_FILE_EXT)))
			{
				p = _tcsninc(di.lpath, llen);
			}
			break;
		case FILE_BINDIFF:
			if (mf->m_bShowDiff
				&& (!mf->m_bHideBak || !FileExtMatches(di.filename,BACKUP_FILE_EXT)))
			{
				p = _tcsninc(di.lpath, llen);
			}
			break;
		case FILE_LUNIQUE:
		case FILE_RUNIQUE:
		case FILE_LDIRUNIQUE:
		case FILE_RDIRUNIQUE:
			if (((mf->m_bShowUniqueLeft && leftside) || (mf->m_bShowUniqueRight && rightside))
				&& (!mf->m_bHideBak || !FileExtMatches(di.filename,BACKUP_FILE_EXT)))
			{
				if (di.code==FILE_LUNIQUE || di.code==FILE_LDIRUNIQUE)
					p = _tcsninc(di.lpath, llen);
				else
					p = _tcsninc(di.rpath, rlen);
			}
			break;
		case FILE_SAME:
			if (mf->m_bShowIdent
				&& (!mf->m_bHideBak || !FileExtMatches(di.filename,BACKUP_FILE_EXT)))
			{
				p = _tcsninc(di.lpath, llen);
			}
			break;
		default: // error
			p = _tcsninc(di.lpath, llen);
			break;
		}
		if (p)
		{
			m_pView->AddItem(cnt, DV_NAME, di.filename);
			m_pView->AddItem(cnt, DV_EXT, di.extension); // BSP - Add the current file extension
			s = _T(".");
			s += p;
			m_pView->AddItem(cnt, DV_PATH, s);
			m_pView->SetItemKey(cnt, curdiffpos);
			// This is inefficient, as we have the di in hand right now
			// But to improve this, have to figure out the intricacies of UpdateItemStatus
			UpdateItemStatus(cnt);
			cnt++;
		}
	}

}

CDirView * CDirDoc::GetMainView()
{
	POSITION pos = GetFirstViewPosition(), ps2=pos;

	while (pos != NULL)
	{
		CDirView* pView = (CDirView*)GetNextView(pos);
		if (pView->IsKindOf( RUNTIME_CLASS(CDirView)))
			return pView;
	}
	return (CDirView*)GetNextView(ps2);
}

static long GetModTime(LPCTSTR szPath)
{
	struct stat mystats;
	bzero(&mystats, sizeof(mystats));
	int stat_result = stat(szPath, &mystats);
	if (stat_result!=0)
		return 0;
	return mystats.st_mtime;
}

static void UpdateTimes(DIFFITEM * pdi)
{
	CString sLeft = (CString)pdi->lpath + _T("\\") + pdi->filename;
	pdi->ltime = GetModTime(sLeft);

	CString sRight = (CString)pdi->rpath + _T("\\") + pdi->filename;
	pdi->rtime = GetModTime(sRight);
}

void CDirDoc::UpdateItemStatus(UINT nIdx)
{
	CString s,s2;
//	These are not used currently
//	UINT cnt=0;
//	int llen = m_pCtxt->m_strLeft.GetLength();
//	int rlen = m_pCtxt->m_strRight.GetLength();
	POSITION diffpos = m_pView->GetItemKey(nIdx);
	DIFFITEM di = m_pCtxt->GetDiffAt(diffpos);
	TCHAR sTime[80];

	UpdateTimes(&di); // in case just copied (into existence) or modified

	switch (di.code)
	{
	case FILE_DIFF:
		VERIFY(s.LoadString(IDS_FILES_ARE_DIFFERENT));
		m_pView->AddItem(nIdx, DV_STATUS, s);
		m_pView->SetImage(nIdx, FILE_DIFF);
		_tcsftime(sTime, 80, _T("%c"), localtime(&di.ltime));
		m_pView->AddItem(nIdx, DV_LTIME, sTime);
		_tcsftime(sTime, 80, _T("%c"), localtime(&di.rtime));
		m_pView->AddItem(nIdx, DV_RTIME, sTime);
		break;
	case FILE_BINDIFF:
		VERIFY(s.LoadString(IDS_BIN_FILES_DIFF));
		m_pView->AddItem(nIdx, DV_STATUS, s);
		m_pView->SetImage(nIdx, FILE_BINDIFF);
		_tcsftime(sTime, 80, _T("%c"), localtime(&di.ltime));
		m_pView->AddItem(nIdx, DV_LTIME, sTime);
		_tcsftime(sTime, 80, _T("%c"), localtime(&di.rtime));
		m_pView->AddItem(nIdx, DV_RTIME, sTime);
		break;
	case FILE_LUNIQUE:
	case FILE_LDIRUNIQUE:
		AfxFormatString1(s, IDS_ONLY_IN_FMT, di.lpath);
		m_pView->AddItem(nIdx, DV_STATUS, s);
		m_pView->SetImage(nIdx, di.code);
		_tcsftime(sTime, 80, _T("%c"), localtime(&di.ltime));
		m_pView->AddItem(nIdx, DV_LTIME, sTime);
		m_pView->AddItem(nIdx, DV_RTIME, _T("---"));
		break;
	case FILE_RUNIQUE:
	case FILE_RDIRUNIQUE:
		AfxFormatString1(s, IDS_ONLY_IN_FMT, di.rpath);
		m_pView->AddItem(nIdx, DV_STATUS, s);
		m_pView->SetImage(nIdx, di.code);
		m_pView->AddItem(nIdx, DV_LTIME, _T("---"));
		_tcsftime(sTime, 80, _T("%c"), localtime(&di.rtime));
		m_pView->AddItem(nIdx, DV_RTIME, sTime);
		break;
	case FILE_SAME:
		VERIFY(s.LoadString(IDS_IDENTICAL));
		m_pView->AddItem(nIdx, DV_STATUS,s);
		m_pView->SetImage(nIdx, FILE_SAME);
		_tcsftime(sTime, 80, _T("%c"), localtime(&di.ltime));
		m_pView->AddItem(nIdx, DV_LTIME, sTime);
		_tcsftime(sTime, 80, _T("%c"), localtime(&di.rtime));
		m_pView->AddItem(nIdx, DV_RTIME, sTime);
		break;
	default: // error
		VERIFY(s.LoadString(IDS_CANT_COMPARE_FILES));
		m_pView->AddItem(nIdx, DV_STATUS, s);
		m_pView->SetImage(nIdx, FILE_ERROR);
		if (di.ltime>0)
			_tcsftime(sTime, 80, _T("%c"), localtime(&di.ltime));
		else
			*sTime = _T('\0');
		m_pView->AddItem(nIdx, DV_LTIME, sTime);
		if (di.rtime>0)
			_tcsftime(sTime, 80, _T("%c"), localtime(&di.rtime));
		else
			*sTime = _T('\0');
		m_pView->AddItem(nIdx, DV_RTIME, sTime);
		break;
	}
}

void CDirDoc::InitStatusStrings()
{

}

void CDirDoc::UpdateResources()
{
	m_pView->UpdateResources();

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

BOOL CDirDoc::UpdateItemStatus( LPCTSTR pathLeft, LPCTSTR pathRight,
							   UINT status )
{
	TCHAR path1[_MAX_PATH] = {0};
	TCHAR path2[_MAX_PATH] = {0};
	TCHAR file1[_MAX_PATH] = {0};
	TCHAR file2[_MAX_PATH] = {0};
	TCHAR ext1[_MAX_PATH] = {0};
	TCHAR ext2[_MAX_PATH] = {0};
	POSITION pos = m_pCtxt->GetFirstDiffPosition();
	POSITION currentPos;
	DIFFITEM current;
	int count = m_pCtxt->GetDiffCount();
	int i = 0;
	BOOL found = FALSE;

	split_filename(pathLeft, path1, file1, ext1);
	split_filename(pathRight, path2, file2, ext2);

	// Path can contain (because of difftools?) '/' and '\'
	// so for comparing purposes, convert whole path to use '/'
	mf->ConvertPathToSlashes(path1);
	mf->ConvertPathToSlashes(path2);

	// Add extensions back
	if (ext1 != NULL)
	{
		_tcscat(file1, ".");
		_tcscat(file1, ext1);
	}
	if (ext2 != NULL)
	{
		_tcscat(file2, ".");
		_tcscat(file2, ext2);
	}

	// Filenames must be identical
	if (_tcsicmp( file1, file2) != 0)
		return FALSE;

	// Get first item
	current = m_pCtxt->GetDiffAt(pos);

	while (i < count && found == FALSE)
	{
		// Save our current pos before getting next
		currentPos = pos;
		current = m_pCtxt->GetNextDiffPosition(pos);

		// Path can contain (because of difftools?) '/' and '\'
		// so for comparing purposes, convert whole path to use '/'
		mf->ConvertPathToSlashes(current.rpath);
		mf->ConvertPathToSlashes(current.lpath);

		if ( (_tcsicmp(current.lpath, path1) == 0) &&
			(_tcsicmp(current.rpath, path2) == 0) &&
			(_tcsicmp(current.filename, file1) == 0) )
		{
			// Right item found!
			// Get index at view, update status to context
			// and tell view to update found item
			int ind = m_pView->GetItemIndex((DWORD)currentPos);
			m_pCtxt->UpdateStatusCode(currentPos, (BYTE)status);
			UpdateItemStatus(ind);
			found = TRUE;
		}
		i++;
	}
	return found;
}
