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
 * @file  MergeDoc.cpp
 *
 * @brief Implementation file for CMergeDoc
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "Merge.h"
#include "direct.h"
#include "MainFrm.h"

#include "diff.h"
#include "diffcontext.h"	// FILE_SAME
#include "getopt.h"
#include "fnmatch.h"
#include "coretools.h"
#include "VssPrompt.h"
#include "MergeEditView.h"
#include "MergeDiffDetailView.h"
#include "cs2cs.h"
#include "childFrm.h"
#include "dirdoc.h"
#include "files.h"
#include "WaitStatusCursor.h"
#include "FileTransform.h"
#include "unicoder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/**
 * @brief EOL types
 */
static LPCTSTR crlfs[] =
{
	_T ("\x0d\x0a"), //  DOS/Windows style
	_T ("\x0a"),     //  UNIX style
	_T ("\x0d")      //  Macintosh style
};

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc

IMPLEMENT_DYNCREATE(CMergeDoc, CDocument)

BEGIN_MESSAGE_MAP(CMergeDoc, CDocument)
	//{{AFX_MSG_MAP(CMergeDoc)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_LEFT, OnFileSaveLeft)
	ON_COMMAND(ID_FILE_SAVE_RIGHT, OnFileSaveRight)
	ON_COMMAND(ID_FILE_SAVEAS_LEFT, OnFileSaveAsLeft)
	ON_COMMAND(ID_FILE_SAVEAS_RIGHT, OnFileSaveAsRight)
	ON_UPDATE_COMMAND_UI(ID_DIFFNUM, OnUpdateStatusNum)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc construction/destruction


#pragma warning(disable:4355)
CMergeDoc::CMergeDoc() : m_ltBuf(this,TRUE), m_rtBuf(this,FALSE)
{
	m_diffs.SetSize(64);
	m_nDiffs=0;
	m_nCurDiff=-1;
	m_strTempLeftFile;
	m_strTempRightFile;
	m_bEnableRescan = TRUE;
	// COleDateTime m_LastRescan
	curUndo = undoTgt.begin();
	m_pLeftView=NULL;
	m_pRightView=NULL;
	m_pLeftDetailView=NULL;
	m_pRightDetailView=NULL;
	m_pDirDoc=NULL;
	m_pInfoUnpacker = new PackingInfo;
}
#pragma warning(default:4355)

CMergeDoc::~CMergeDoc()
{	
//<jtuc 2003-06-28>
	/*CUndoItem *pitem;
	while (!m_undoList.IsEmpty())
	{
		pitem = (CUndoItem*)m_undoList.RemoveHead();
		delete pitem;
	}*/
//</jtuc>
	if (m_pDirDoc)
	{
		m_pDirDoc->MergeDocClosing(this);
		m_pDirDoc = 0;
	}
	if (m_pInfoUnpacker) 
	{
		delete m_pInfoUnpacker;
		m_pInfoUnpacker = 0;
	}
}

void CMergeDoc::DeleteContents ()
{
	CDocument::DeleteContents ();
	m_ltBuf.FreeAll ();
	m_rtBuf.FreeAll ();
	CleanupTempFiles();
}

void CMergeDoc::OnFileEvent (WPARAM /*wEvent*/, LPCTSTR /*pszPathName*/)
{
	/*if (!(theApp.m_dwFlags & EP_NOTIFY_CHANGES))
    return;
	MessageBeep (MB_ICONEXCLAMATION);
	CFrameWnd *pwndMain= (CFrameWnd*) theApp.GetMainWnd ();
	ASSERT (pwndMain);
	if (!pwndMain->IsWindowVisible ())
          ((CMainFrame*) pwndMain)->FlashUntilFocus ();
	if (wEvent & FE_MODIFIED)
  	{
  	  bool bReload = (theApp.m_dwFlags & EP_AUTO_RELOAD) != 0;
  	  if (!bReload)
  	    {
          CString sMsg;
          sMsg.Format (IDS_FILE_CHANGED, pszPathName);
  	      bReload = AfxMessageBox (sMsg, MB_YESNO|MB_ICONQUESTION) == IDYES;
  	    }
  	  if (bReload)
        {
	        POSITION pos = GetFirstViewPosition ();
          ASSERT (pos);
	        CEditPadView *pView;
          do
            {
	            pView = (CEditPadView*) GetNextView (pos);
              pView->PushCursor ();
            }
          while (pos);
          m_xTextBuffer.FreeAll ();
          m_xTextBuffer.LoadFromFile (pszPathName);
	        pos = GetFirstViewPosition ();
          ASSERT (pos);
          do
            {
	            pView = (CEditPadView*) GetNextView (pos);
              pView->PopCursor ();
              HWND hWnd = pView->GetSafeHwnd ();
              ::RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE|RDW_INTERNALPAINT|RDW_ERASE|RDW_ERASENOW|RDW_UPDATENOW|RDW_NOFRAME);
            }
          while (pos);
        }
    }
  else if (wEvent & FE_DELETED)
    {
      if (!(theApp.m_dwFlags & EP_AUTO_RELOAD))
        {
          CString sMsg;
          sMsg.Format (IDS_FILE_DELETED, pszPathName);
        	AfxMessageBox (sMsg, MB_OK|MB_ICONINFORMATION);
        }
    }*/
}

BOOL CMergeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CString s;
	VERIFY(s.LoadString(IDS_FILE_COMPARISON_TITLE));
	SetTitle(s);
	
	m_ltBuf.InitNew ();
	m_rtBuf.InitNew ();
	return TRUE;
}


void CMergeDoc::SetUnpacker(PackingInfo * infoNewHandler)
{
	if (infoNewHandler)
	{
		*m_pInfoUnpacker = *infoNewHandler;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc serialization

void CMergeDoc::Serialize(CArchive& ar)
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
// CMergeDoc diagnostics

#ifdef _DEBUG
void CMergeDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMergeDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc commands

/**
 * @brief Save an editor text buffer to a file for preprocessing (make UCS-2LE if appropriate)
 */
static void SaveBuffForDiff(CMergeDoc::CDiffTextBuffer & buf, const CString & filepath)
{
	// we subvert the buffer's memory of the original file encoding
	int temp=buf.m_nSourceEncoding;

	if (sizeof(TCHAR)>1 
		|| buf.m_nSourceEncoding==-20 // source file was UCS-2LE
		|| buf.m_nSourceEncoding==-21 // source file was UCS-2BE
		|| buf.m_nSourceEncoding==-22) // source file was UTF-8
	{
		buf.m_nSourceEncoding = -20; // write as UCS-2LE (for preprocessing)
	}

	// and we don't repack the file
	PackingInfo * tempPacker = NULL;

	// write buffer out to temporary file
	BOOL bTempFile = TRUE;
	BOOL bClearModifiedFlag = FALSE;
	buf.SaveToFile(filepath, bTempFile, tempPacker, CRLF_STYLE_AUTOMATIC, bClearModifiedFlag);
	
	// restore memory of encoding of original file
	buf.m_nSourceEncoding = temp;
}

/**
* @brief Save files to temp files & compare again.
*
* @param bForced If TRUE, suppressing is ignored and rescan is done always
*
* @return Tells status of rescan done (OK, suppressed, etc)
*
* @note Rescan() ALWAYS compares temp files. Actual user files are not
* touched by Rescan().
*
* @sa CDiffWrapper::RunFileDiff()
*
*/
int CMergeDoc::Rescan(BOOL bForced /* =FALSE */)
{
	DIFFOPTIONS diffOptions = {0};
	DIFFSTATUS status = {0};
	BOOL diffSuccess;
	int nResult = RESCAN_OK;

	if (!bForced)
	{
		if (!m_bEnableRescan)
			return RESCAN_SUPPRESSED;
	}

	m_LastRescan = COleDateTime::GetCurrentTime();

	// store modified status
	BOOL ltMod = m_ltBuf.IsModified();
	BOOL rtMod = m_rtBuf.IsModified();

	// remove blank lines and clear winmerge flags
	m_ltBuf.prepareForRescan();
	m_rtBuf.prepareForRescan();

	// restore modified status
	m_ltBuf.SetModified(ltMod);
	m_rtBuf.SetModified(rtMod);

	// get the desired files to temp locations so we can edit them dynamically
	if (!TempFilesExist())
	{
		if (!InitTempFiles(m_strLeftFile, m_strRightFile))
			return RESCAN_FILE_ERR;
	}

	// output buffers to temp files (in UTF-8 if TCHAR=wchar_t or buffer was Unicode)
	SaveBuffForDiff(m_ltBuf, m_strTempLeftFile);
	SaveBuffForDiff(m_rtBuf, m_strTempRightFile);

	// Set up DiffWrapper
	m_diffWrapper.SetCompareFiles(m_strTempLeftFile, m_strTempRightFile);
	m_diffWrapper.SetTextForAutomaticUnpack(m_strBothFilenames);
	m_diffWrapper.SetDiffList(&m_diffs);
	m_diffWrapper.SetUseDiffList(TRUE);		// Add diffs to list
	m_diffWrapper.GetOptions(&diffOptions);
	
	// Clear diff list
	m_diffs.RemoveAll();
	m_nDiffs = 0;
	m_nCurDiff = -1;

	// Run diff
	diffSuccess = m_diffWrapper.RunFileDiff();

	// Read diff-status
	m_diffWrapper.GetDiffStatus(&status);
	m_nDiffs = m_diffs.GetSize();

	// If comparing whitespaces and
	// other file has EOL before EOF and other not...
	if (status.bLeftMissingNL != status.bRightMissingNL &&
		!diffOptions.nIgnoreWhitespace && !diffOptions.bIgnoreBlankLines)
	{
		// ..lasf DIFFRANGE of file which has EOL must be
		// fixed to contain last line too
		m_diffWrapper.FixLastDiffRange(m_ltBuf.GetLineCount(), m_rtBuf.GetLineCount(),
				status.bRightMissingNL);
	}

	// Determine errors and binary file compares
	if (!diffSuccess)
		nResult = RESCAN_FILE_ERR;
	else if (status.bBinaries)
		nResult = RESCAN_BINARIES;
	else
	{
		// Identical files are also updated
		if (m_nDiffs == 0)
			nResult = RESCAN_IDENTICAL;

		// Analyse diff-list (updating real line-numbers)
		PrimeTextBuffers();
		
		// Display files
		m_pLeftView->PrimeListWithFile();
		m_pRightView->PrimeListWithFile();
		m_pLeftDetailView->PrimeListWithFile();
		m_pRightDetailView->PrimeListWithFile();
	}
	return nResult;
}

/// Prints (error) message by rescan resultcode
void CMergeDoc::ShowRescanError(int nRescanResult)
{
	CString s;
	switch (nRescanResult)
	{
	case RESCAN_IDENTICAL:
		VERIFY(s.LoadString(IDS_FILESSAME));
		AfxMessageBox(s, MB_ICONINFORMATION);
		break;

	case RESCAN_BINARIES:
		VERIFY(s.LoadString(IDS_BIN_FILES_DIFF));
		AfxMessageBox(s, MB_ICONINFORMATION);
		break;
			
	case RESCAN_FILE_ERR:
		VERIFY(s.LoadString(IDS_FILEERROR));
		AfxMessageBox(s, MB_ICONSTOP);
		break;
	}
}


//<jtuc 2003-06-28>
void CMergeDoc::CDiffTextBuffer::AddUndoRecord(BOOL bInsert, const CPoint & ptStartPos, const CPoint & ptEndPos, LPCTSTR pszText, int flags, int nActionType /*= CE_ACTION_UNKNOWN*/)
{
	CGhostTextBuffer::AddUndoRecord(bInsert, ptStartPos, ptEndPos, pszText, flags, nActionType);
	if (m_aUndoBuf[m_nUndoPosition - 1].m_dwFlags & UNDO_BEGINGROUP)
	{
		m_pOwnerDoc->undoTgt.erase(m_pOwnerDoc->curUndo, m_pOwnerDoc->undoTgt.end());
		m_pOwnerDoc->undoTgt.push_back(m_bIsLeft ? m_pOwnerDoc->m_pLeftView : m_pOwnerDoc->m_pRightView);
		m_pOwnerDoc->curUndo = m_pOwnerDoc->undoTgt.end();
	}
}
//<jtuc>

/*
void CMergeDoc::AddUndoAction(UINT nBegin, UINT nEnd, UINT nDiff, int nBlanks, BOOL bInsert, CMergeEditView *pList)
{
	CUndoItem *pitem = new CUndoItem;
	if (pitem != NULL)
	{
		pitem->begin = nBegin;
		pitem->end = nEnd;
		pitem->diffidx = nDiff;
		pitem->blank = nBlanks;
		pitem->bInsert = bInsert;
		pitem->m_pList = pList;
		if (bInsert)
			for (UINT i=nBegin; i <= nEnd; i++)
			{
				CString s = pitem->m_pList->GetLineText(i);
				pitem->list.AddTail(s);
			}

		m_undoList.AddHead(pitem);
	}
}
*/

BOOL CMergeDoc::Undo()
{
/*TODO	if (!m_undoList.IsEmpty())
	{
		CUndoItem *pitem = (CUndoItem *)m_undoList.RemoveHead();
		if (pitem != NULL)
		{
			if (pitem->bInsert)
			{
				CString s;
				POSITION pos = pitem->list.GetHeadPosition();
				UINT i = pitem->begin;
				while (pos != NULL)
				{
					s = pitem->list.GetNext(pos);
					pitem->m_pList->m_pList->SetItemText(i,0,s);
					i++;
				}
				if (pitem->m_pList == mf->m_pLeft)
					m_diffs[pitem->diffidx].blank0 = pitem->blank;
				else
					m_diffs[pitem->diffidx].blank1 = pitem->blank;
				
				// scroll to the diff
				int line = max(0, m_diffs[pitem->diffidx].dbegin0-CONTEXT_LINES);
				mf->m_pLeft->ScrollToLine(line);
				mf->m_pRight->ScrollToLine(line);
				
				// select the diff
				mf->m_pLeft->SelectDiff(pitem->diffidx);
			}
			else
			{
				ASSERT(0);
			}

			pitem->m_pList->SubMod();
			delete pitem;


			return TRUE;
		}
	}*/
	return FALSE;
}

/**
 * @brief An instance of RescanSuppress prevents rescan during its lifetime
 * (or until its Clear method is called, which ends its effect).
 */
class RescanSuppress
{
public:
	RescanSuppress(CMergeDoc & doc) : m_doc(doc)
	{
		m_bSuppress = TRUE;
		m_bPrev = doc.m_bEnableRescan;
		doc.m_bEnableRescan = FALSE;
	}
	void Clear() 
	{
		if (m_bSuppress)
		{
			m_bSuppress = FALSE;
			m_doc.m_bEnableRescan = m_bPrev;
		}
	}
	~RescanSuppress()
	{
		Clear();
	}
private:
	CMergeDoc & m_doc;
	BOOL m_bPrev;
	BOOL m_bSuppress;
};

/// Copy all diffs from one side to the other (as specified by caller)
void CMergeDoc::CopyAllList(bool bSrcLeft)
{
	CopyMultipleList(bSrcLeft, 0, m_nDiffs-1);
}

/// Copy some diffs from one side to the other (as specified by caller)
void CMergeDoc::CopyMultipleList(bool bSrcLeft, int firstDiff, int lastDiff)
{
	lastDiff = min(m_nDiffs-1, lastDiff);
	firstDiff = max(0, firstDiff);
	if (firstDiff > lastDiff)
		return;
	
	RescanSuppress suppressRescan(*this);

	// Note we don't care about m_nDiffs count to become zero,
	// because we don't rescan() so it does not change

	// copy from bottom up is more efficient
	for(int i = lastDiff; i>=firstDiff; --i)
	{
		SetCurrentDiff(i);
		ListCopy(bSrcLeft);
	}
	suppressRescan.Clear(); // done suppress Rescan
	FlushAndRescan();
}

/// Copies selected (=current) diff to another side
void CMergeDoc::ListCopy(bool bSrcLeft)
{
	// suppress Rescan during this method
	// (Not only do we not want to rescan a lot of times, but
	// it will wreck the line status array to rescan as we merge)
	RescanSuppress suppressRescan(*this);

	// make sure we're on a diff
	int curDiff = GetCurrentDiff();
	if (curDiff!=-1)
	{
		DIFFRANGE &cd = m_diffs[curDiff];
		CDiffTextBuffer& sbuf = bSrcLeft? m_ltBuf:m_rtBuf;
		CDiffTextBuffer& dbuf = bSrcLeft? m_rtBuf:m_ltBuf;
		BOOL bSrcWasMod = sbuf.IsModified();
		int cd_dbegin = bSrcLeft? cd.dbegin0:cd.dbegin1;
		int cd_dend = bSrcLeft? cd.dend0:cd.dend1;
		int cd_blank = bSrcLeft? cd.blank0:cd.blank1;

		// TODO: add the undo action

		// if the current diff contains missing lines, remove them from both sides
		int deleted_lines=0;
		int limit = cd_dend;

		// curView is the view which is changed, so the opposite of the source view
		CCrystalTextView* curView;
		/*POSITION pos = GetFirstViewPosition();
		curView = dynamic_cast<CCrystalTextView*>(GetNextView(pos));
		if(bSrcLeft)
		{
			curView = dynamic_cast<CCrystalTextView*>(GetNextView(pos));
		}*/
		if(bSrcLeft)
			curView = m_pRightView;
		else
			curView = m_pLeftView;

		dbuf.BeginUndoGroup();
		if (cd_blank>=0)
		{
			// text was missing, so delete rest of lines on both sides
			// delete only on destination side since rescan will clear the other side
			if(cd_blank==0)
			{
				dbuf.DeleteText(curView, cd_blank, 0, cd_dend+1, 0, CE_ACTION_MERGE);
			}
			else
			{
				dbuf.DeleteText(curView, cd_blank-1, dbuf.GetLineLength(cd_blank-1), cd_dend, dbuf.GetLineLength(cd_dend), CE_ACTION_MERGE);
			}
			deleted_lines=cd_dend-cd_blank+1;

			limit=cd_blank-1;
			dbuf.FlushUndoGroup(curView);
			dbuf.BeginUndoGroup(TRUE);
		}

		CString strLine;

		// copy the selected text over
		for (int i=cd_dbegin; i <= limit; i++)
		{
			// text exists on left side, so just replace
			strLine = _T("");
			sbuf.GetFullLine(i, strLine);
			dbuf.ReplaceFullLine(curView, i, strLine, CE_ACTION_MERGE);
			dbuf.FlushUndoGroup(curView);
			dbuf.BeginUndoGroup(TRUE);
		}
		dbuf.FlushUndoGroup(curView);


		//mf->m_pRight->ReplaceSelection(strText, 0);

		//pSrcList->InvalidateLines(cd_dbegin, cd_dend);
		//pDestList->InvalidateLines(cd_dbegin, cd_dend);

		// remove the diff			
		SetCurrentDiff(-1);
		// no longer needed since rescan automatically does this
/*		m_diffs.RemoveAt(curDiff);
		m_nDiffs--;

		// adjust remaining diffs
		if (deleted_lines>0)
		{
			for (int i=curDiff; i < (int)m_nDiffs; i++)
			{
				DIFFRANGE &cd = m_diffs[i];
				cd.dbegin0 -= deleted_lines;
				cd.dbegin1 -= deleted_lines;
				cd.dend0 -= deleted_lines;
				cd.dend1 -= deleted_lines;
				cd.blank0 -= deleted_lines;
				cd.blank1 -= deleted_lines;
			}
		}
*/

		// reset the mod status of the source view because we do make some
		// changes, but none that concern the source text
		sbuf.SetModified(bSrcWasMod);
	}
	// what does this do?
//	pDestList->AddMod();
	suppressRescan.Clear(); // done suppress Rescan
	FlushAndRescan();
}

/**
 * @brief Asks from filename to save from user and save file
 * @param strPath Initial path shown to user,
 * path to new filename returned if saving succeeds
 * @param bSaveSuccess Returns if saving itself succeeded/failed
 * @return False when saving fails, so we can ask again
 */
BOOL CMergeDoc::TrySaveAs(CString &strPath, BOOL &bSaveSuccess, BOOL bLeft, PackingInfo * pInfoTempUnpacker)
{
	BOOL result = TRUE;
	CString s;
	CString strSavePath;
	CString title;

	if (bSaveSuccess == SAVE_PACK_FAILED)
	{
		// display an error message

		AfxFormatString2(s, bLeft ? IDS_FILEPACK_FAILED_LEFT : IDS_FILEPACK_FAILED_RIGHT, strPath, pInfoTempUnpacker->pluginName);
		// replace the unpacker with a "do nothing" unpacker
		pInfoTempUnpacker->Initialize(UNPACK_MANUAL);
	}
	else
	{
		// display an error message
		AfxFormatString1(s, IDS_FILESAVE_FAILED, strPath);
	}

	switch(AfxMessageBox(s, MB_YESNO|MB_ICONQUESTION))
	{
	case IDYES:
		VERIFY(title.LoadString(IDS_SAVE_AS_TITLE));
		if (SelectFile(s, strPath, title, NULL, FALSE))
		{
			strSavePath = s;
			if (bLeft)
				bSaveSuccess = m_ltBuf.SaveToFile(strSavePath, FALSE, pInfoTempUnpacker);
			else
				bSaveSuccess = m_rtBuf.SaveToFile(strSavePath, FALSE, pInfoTempUnpacker);

			if (bSaveSuccess == SAVE_DONE)
			{
				strPath = strSavePath;
				UpdateHeaderPath(bLeft);
			}
			else
				result = FALSE;
		}
		break;
	case IDNO:
		break;
	}
	return result;
}

/**
* @brief Save file creating backups etc
* Safe top-level file saving function. Checks validity of given path.
* Creates backup file if wanted to. And if saving to given path fails,
* allows user to select new location/name for file.
*
* @param szPath Path where to save including filename
* @param bSaveSuccess Will contain information about save success
* @param bLeft If TRUE we are saving left(side) file, else right file
*
* @return Tells if caller can continue (no errors happened)
*
* @note Return value does not tell if SAVING succeeded. Caller must
* Check value of bSaveSuccess parameter after calling this function!
*
* @sa
*
*/
BOOL CMergeDoc::DoSave(LPCTSTR szPath, BOOL &bSaveSuccess, BOOL bLeft)
{
	CString strSavePath(szPath);

	// use a temp packer
	// first copy the m_pInfoUnpacker
	// if an error arises during packing, change and take a "do nothing" packer
	PackingInfo infoTempUnpacker = *m_pInfoUnpacker;

	bSaveSuccess = FALSE;
	if (!mf->m_strSaveAsPath.IsEmpty())
	{
		CFileStatus status;
		if (CFile::GetStatus(mf->m_strSaveAsPath, status)
			&& (status.m_attribute & CFile::Attribute::directory))
		{
			// third arg was a directory, so get append the filename
			CString sname;
			SplitFilename(szPath, 0, &sname, 0);
			strSavePath = mf->m_strSaveAsPath;
			if (mf->m_strSaveAsPath.Right(1) != _T('\\'))
				strSavePath += _T('\\');
			strSavePath += sname;
		}
		else
			strSavePath = mf->m_strSaveAsPath;	
	}

	if (!mf->CheckSavePath(strSavePath))
		return FALSE;

	if (!mf->CreateBackup(strSavePath))
		return FALSE;

	BOOL result;
	if(bLeft)
	{
		bSaveSuccess = m_ltBuf.SaveToFile(strSavePath, FALSE, &infoTempUnpacker);
		if(bSaveSuccess == SAVE_DONE)
		{
			m_strLeftFile = strSavePath;
			UpdateHeaderPath(TRUE);
			result = TRUE;
		}
		else
		{
			// Saving failed, user may save to another location if wants to
			// TODO: proper fix for handling save success here;
			// problem is we cannot return bSaveSuccess because callers use
			// it to determine if file statuses should be changed.
			BOOL bSaveAsSuccess = bSaveSuccess;
			do
				result = TrySaveAs(strSavePath, bSaveAsSuccess, bLeft, &infoTempUnpacker);
			while (!result);
		}
	}
	else
	{
		bSaveSuccess = m_rtBuf.SaveToFile(strSavePath, FALSE, &infoTempUnpacker);
		if(bSaveSuccess == SAVE_DONE)
		{
			m_strRightFile = strSavePath;
			UpdateHeaderPath(FALSE);
			result = TRUE;
		}
		else
		{
			// Saving failed, user may save to another location if wants to
			// TODO: proper fix for handling save success here;
			// problem is we cannot return bSaveSuccess because callers use
			// it to determine if file statuses should be changed.
			BOOL bSaveAsSuccess = bSaveSuccess;
			do
				result = TrySaveAs(strSavePath, bSaveAsSuccess, bLeft, &infoTempUnpacker);
			while (!result);
		}
	}
	return result;
}

/*CString CMergeDoc::ExpandTabs(LPCTSTR szText)
{
	LPCTSTR p;
	CString strResult(_T(""));
	CString spaces(_T(' '), mf->m_nTabSize);
	if (szText != NULL && *szText != _T('\0'))
	{
		for (p=szText; *p != _T('\0'); p=_tcsinc(p))
		{
			if (*p == _T('\t'))
				strResult += spaces;
			else
				strResult += *p;
		}
	}
	return strResult;
}

CString CMergeDoc::Tabify(LPCTSTR szText)
{
	LPCSTR p=szText,p2;
	TCHAR temp[1024];
	CString strResult("");
	CString spaces(' ',mf->m_nTabSize);
	while (1)
	{
		if ((p2=strstr(p, spaces)) != NULL)
		{
			strncpy(temp,p,p2-p);
			temp[p2-p]=NULL;
			strResult += temp;
			strResult += "\t";
			p = p2+mf->m_nTabSize;
		}
		else
		{
			strResult += p;
			break;
		}
	}
	return strResult;
}*/

/**
 * @brief Checks if line is inside diff
 */
BOOL CMergeDoc::LineInDiff(UINT nLine, UINT nDiff)
{
	ASSERT(nDiff >= 0 && nDiff <= m_nDiffs);
	if (nLine >= m_diffs[nDiff].dbegin0 &&
			nLine <= m_diffs[nDiff].dend0)
		return TRUE;
	else
		return FALSE;
}

/**
 * @brief Returns order num of diff for given line
 */
int CMergeDoc::LineToDiff(UINT nLine)
{
	for (UINT i = 0; i < m_nDiffs; i++)
	{
		if (LineInDiff(nLine, i))
			return i;
	}
	return -1;
}

void CMergeDoc::SetDiffViewMode(BOOL bEnable)
{
	if (bEnable)
	{
	}
	else
	{
	}
}


UINT CMergeDoc::CountPrevBlanks(UINT nCurLine, BOOL bLeft)
{
	UINT nBlanks=0;
	int blk,end;
	for (int i=0; i < (int)m_nDiffs; i++)
	{
		if (bLeft)
		{
			blk = m_diffs[i].blank0;
			end = m_diffs[i].dend0;
		}
		else
		{
			blk = m_diffs[i].blank1;
			end = m_diffs[i].dend0;
		}
		if (blk >= (int)nCurLine)
		{
			break;
		}
		else if (blk >= 0)
			nBlanks += end - blk + 1;
	}
	return nBlanks;
}

BOOL CMergeDoc::CanCloseFrame(CFrameWnd* /*pFrame*/) 
{
	if (SaveHelper())
	{
		// Set modified status to false so that we are not asking
		// about saving again in OnCloseDocument()
		m_ltBuf.SetModified(FALSE);
		m_rtBuf.SetModified(FALSE);
		return TRUE;
	}
	else
		return FALSE;
}

// If WinMerge is closed, CMainFrame::OnClose already takes
// care of saving so this function just returns TRUE
// to prevent further questions
BOOL CMergeDoc::SaveModified()
{
	return TRUE;
}

void CMergeDoc::SetCurrentDiff(int nDiff)
{
	if (nDiff >= 0 && nDiff < (int)m_nDiffs)
		m_nCurDiff = nDiff;
	else
		m_nCurDiff = -1;
}

BOOL CMergeDoc::CDiffTextBuffer::FlagIsSet(UINT line, DWORD flag)
{
	return ((m_aLines[line].m_dwFlags & flag) == flag);
}


/**
Remove blank lines and clear winmerge flags
(2003-06-21, Perry: I don't understand why this is necessary, but if this isn't 
done, more and more gray lines appear in the file)
(2003-07-31, Laoran I don't understand either why it is necessary, but it works
fine, so let's go on with it)
*/
void CMergeDoc::CDiffTextBuffer::prepareForRescan()
{
	RemoveAllGhostLines();
	for(int ct=GetLineCount()-1; ct>=0; --ct)
	{
		SetLineFlag(ct, LF_DIFF, FALSE, FALSE, FALSE);
		SetLineFlag(ct, LF_TRIVIAL, FALSE, FALSE, FALSE);
	}
}


void CMergeDoc::CDiffTextBuffer::OnNotifyLineHasBeenEdited(int nLine)
{
	SetLineFlag(nLine, LF_DIFF, FALSE, FALSE, FALSE);
	SetLineFlag(nLine, LF_TRIVIAL, FALSE, FALSE, FALSE);
	CGhostTextBuffer::OnNotifyLineHasBeenEdited(nLine);
}



/**
 * @brief Helper to determine the most used CRLF mode in the file
 * Normal call : determine the CRLF mode of the current line
 * Final call  : parameter lpLineBegin = NULL, return the style of the file
 *
 * @note  The lowest CRL_STYLE has the priority in case of equality
 */
int CMergeDoc::CDiffTextBuffer::NoteCRLFStyleFromBuffer(TCHAR *lpLineBegin, DWORD dwLineLen /* =0 */)
{
	static int count[3] = {0};
	int iStyle = -1;

	// give back the result when we ask for it
	if (lpLineBegin == NULL)
	{
		iStyle = 0;
		if (count[1] > count[iStyle])
			iStyle = 1;
		if (count[2] > count[iStyle])
			iStyle = 2;

		// reset the counter for the next file
		count[0] = count[1] = count[2] = 0;

		return iStyle;
	}

	if (dwLineLen >= 1)
	{
		if (lpLineBegin[dwLineLen-1] == _T('\r'))
			iStyle = CRLF_STYLE_MAC;
		if (lpLineBegin[dwLineLen-1] == _T('\n'))
			iStyle = CRLF_STYLE_UNIX;
	}
	if (dwLineLen >= 2)
	{
		if (lpLineBegin[dwLineLen-2] == _T('\r') && lpLineBegin[dwLineLen-1] == _T('\n'))
			iStyle = CRLF_STYLE_DOS;
	}
	ASSERT (iStyle != -1);
	count[iStyle] ++;
	return iStyle;
}


/// Reads one line from filebuffer and inserts to textbuffer
void CMergeDoc::CDiffTextBuffer::ReadLineFromBuffer(TCHAR *lpLineBegin, DWORD dwLineNum, DWORD dwLineLen /* =0 */)
{
	if (m_nSourceEncoding >= 0)
		iconvert (lpLineBegin, m_nSourceEncoding, 1, m_nSourceEncoding == 15);
	AppendLine(dwLineNum, lpLineBegin, dwLineLen);
}

/// Sets path for temporary files
void CMergeDoc::CDiffTextBuffer::SetTempPath(CString path)
{
	m_strTempPath = path;
}

/**
 * @brief Examine statistics in textFileStats and return a crystaltextbuffer enum value for line style
 */
int GetTextFileStyle(const ParsedTextFile & parsedTextFile)
{
	if (parsedTextFile.crlfs >= parsedTextFile.lfs)
	{
		if (parsedTextFile.crlfs >= parsedTextFile.crs)
		{
			return CRLF_STYLE_DOS;
		}
		else
		{
			return CRLF_STYLE_MAC;
		}
	}
	else
	{
		if (parsedTextFile.lfs >= parsedTextFile.crs)
		{
			return CRLF_STYLE_UNIX;
		}
		else
		{
			return CRLF_STYLE_MAC;
		}
	}
}

/// Loads file from disk to buffer
int CMergeDoc::CDiffTextBuffer::LoadFromFile(LPCTSTR pszFileNameInit, PackingInfo * infoUnpacker, CString sToFindUnpacker, BOOL & readOnly,
		int nCrlfStyle /*= CRLF_STYLE_AUTOMATIC*/)
{
	// Unpacking the file here, save the result in a temporary file
	CString sFileName = pszFileNameInit;
	int attrs=0;		// don't care about it, it is for DirScan
	if (infoUnpacker->bToBeScanned)
	{
		if (!FileTransform_Unpacking(sFileName, sToFindUnpacker, infoUnpacker, &unpackerSubcode))
			return FRESULT_ERROR;
	}
	else
	{
		if (!FileTransform_Unpacking(sFileName, *infoUnpacker, &unpackerSubcode))
			return FRESULT_ERROR;
	}
	// we use the same unpacker for both files, so it must be defined after first file
	ASSERT(infoUnpacker->bToBeScanned == FALSE);
	// we will load the transformed file
	LPCTSTR pszFileName = sFileName;

//	We call FreeAll just before reading m_aLines
//	ASSERT(!m_bInit);
//	ASSERT(m_aLines.GetSize() == 0);
	MAPPEDFILEDATA fileData = {0};
	CString sExt;
	BOOL bSuccess = FALSE;
	int nRetVal = FRESULT_OK;

	// Set encoding based on extension, if we know one
	SplitFilename(pszFileName, NULL, NULL, &sExt);
	CCrystalTextView::TextDefinition *def = 
			CCrystalTextView::GetTextType(sExt);
	if (def && def->encoding != -1)
		m_nSourceEncoding = def->encoding;
	
	// Init filedata struct and open file as memory mapped 
	_tcsncpy(fileData.fileName, pszFileName, countof(fileData.fileName));
	fileData.bWritable = FALSE;
	fileData.dwOpenFlags = OPEN_EXISTING;
	bSuccess = files_openFileMapped(&fileData);

	// Inefficiency here
	// We load up the line array in files_loadLines
	// only to recopy them all into m_aLines
	// This wouldn't be bad if it were a CString copy (as CStrings are reference counted)
	// but I think that AppendLine leads to a new allocation
	// so we are copying the whole file into textFileStats line buffers
	// and then recopying it into crystal line buffers
	// so... perhaps this could be improved.

	ParsedTextFile parsedTextFile;
	if (bSuccess)
		nRetVal = files_loadLines(&fileData, &parsedTextFile);
	else
		nRetVal = FRESULT_ERROR;
	
	if (nRetVal == FRESULT_OK)
	{
		// FreeAll() is needed before loading (this is complicated)
		FreeAll();

		m_aLines.SetSize(parsedTextFile.lines.GetSize(), 4096);
		
		DWORD dwBytesRead = 0;
		TCHAR *lpChar = (TCHAR *)fileData.pMapBase;
		TCHAR *lpLineBegin = lpChar;
		int eolChars = 0;
		UINT lineno = 0;
		for ( ; lineno < parsedTextFile.lines.GetSize(); ++lineno)
		{
			textline & lp = parsedTextFile.lines.GetAt(lineno);

			// Skipping iconvert call here
			// because I don't know if it even works
			// Perry 2003-09-15

			AppendLine(lineno, lp.sline, lp.sline.GetLength());
		}
		
		//Try to determine current CRLF mode (most frequent)
		if (nCrlfStyle == CRLF_STYLE_AUTOMATIC)
		{
			nCrlfStyle = GetTextFileStyle(parsedTextFile);
		}
		ASSERT(nCrlfStyle >= 0 && nCrlfStyle <= 2);
		SetCRLFMode(nCrlfStyle);
		
		//  At least one empty line must present
		// (memory mapping doesn't work for zero-length files)
		ASSERT(m_aLines.GetSize() > 0);
		
		m_bInit = TRUE;
		m_bModified = FALSE;
		m_bUndoGroup = m_bUndoBeginGroup = FALSE;
		m_nUndoBufSize = 1024; // crystaltextbuffer.cpp - UNDO_BUF_SIZE;
		m_nSyncPosition = m_nUndoPosition = 0;
		ASSERT(m_aUndoBuf.GetSize() == 0);
		
		FinishLoading();
		// flags don't need initialization because 0 is the default value

		// This is needed: Syntax hilighting does not work when
		// automatically scroll to first diff is enabled
		RetypeViews(pszFileName);

		UpdateViews(NULL, NULL, UPDATE_RESET);
		m_ptLastChange.x = m_ptLastChange.y = -1;
		nRetVal = FRESULT_OK;

		// stash original encoding away
		switch (parsedTextFile.codeset)
		{
		case ucr::UCS2LE:
			m_nSourceEncoding = -20;
			break;
		case ucr::UCS2BE:
			m_nSourceEncoding = -21;
			break;
		case ucr::UTF8:
			m_nSourceEncoding = -22;
			break;
		}
		if (parsedTextFile.lossy)
			readOnly = TRUE;
	}
	
	files_closeFileMapped(&fileData, 0xFFFFFFFF, FALSE);

	// delete the file that unpacking may have created
	if (_tcscmp(pszFileNameInit, pszFileName) != 0)
		::DeleteFile(pszFileName);

	return nRetVal;
}

/// Saves file from buffer to disk
// NOTE: bTempFile is FALSE if we are saving user files and
// TRUE if we are saving workin-temp-files for diff-engine
BOOL CMergeDoc::CDiffTextBuffer::SaveToFile (LPCTSTR pszFileName,
		BOOL bTempFile, PackingInfo * infoUnpacker /*= NULL*/, int nCrlfStyle /*= CRLF_STYLE_AUTOMATIC*/ ,
		BOOL bClearModifiedFlag /*= TRUE*/ )
{
	ASSERT (nCrlfStyle == CRLF_STYLE_AUTOMATIC || nCrlfStyle == CRLF_STYLE_DOS ||
		nCrlfStyle == CRLF_STYLE_UNIX || nCrlfStyle == CRLF_STYLE_MAC);
	ASSERT (m_bInit);
	MAPPEDFILEDATA fileData = {0};
	TCHAR szTempFileDir[_MAX_PATH] = {0};
	TCHAR szTempFileName[_MAX_PATH] = {0} ;
	BOOL bOpenSuccess = FALSE;
	BOOL bSaveSuccess = FALSE;
	TCHAR drive[_MAX_PATH] = {0};
	TCHAR dir[_MAX_PATH] = {0};

	if (nCrlfStyle == CRLF_STYLE_AUTOMATIC)
	{
		if (mf->m_bAllowMixedEol)
			// keep automatic, GetTextWithoutEmpty will read EOL from the m_aLines array
			;
		else
		{
			// get the default nCrlfStyle of the CDiffTextBuffer
			nCrlfStyle = GetCRLFMode();
			ASSERT(nCrlfStyle >= 0 && nCrlfStyle <= 2);
		}
	}
	// we don't need to apply the EOL mode to the m_aLines array
	// as GetTextWithoutEmpty receives nCrlfStyle as parameter
	
	int nLineCount = m_aLines.GetSize();
	CString text;			
	int nLastLength = GetFullLineLength(nLineCount-1);
		
	GetTextWithoutEmptys(0, 0, nLineCount - 1, nLastLength, text, nCrlfStyle);
	UINT nchars = text.GetLength();

	UINT nbytes = -1;
	if (m_nSourceEncoding == -20)
	{
		// UCS-2LE (+ 2 byte BOM)
		nbytes = 2 * nchars + 2;
	}
	else if (m_nSourceEncoding == -21)
	{
		// UCS-2BE (+ 2 byte BOM)
		nbytes = 2 * nchars + 2;
	}
	else if (m_nSourceEncoding == -22)
	{
		// UTF-8 (+ 3 byte BOM)
		nbytes = ucr::Utf8len_of_string(text) + 3;
	}
	else
	{
		// 8-bit encoding
		nbytes = nchars;
	}

	// Data to save is now in CString text

	if (!pszFileName)
		return SAVE_FAILED;	// No filename, cannot save...

	if (!bTempFile)
	{
		if (!::GetTempFileName(m_strTempPath, _T("MRG"), 0, szTempFileName))
			return SAVE_FAILED;  //Nothing to do if even tempfile name fails
	}

	// Init filedata struct and open file as memory mapped 
	if (bTempFile)
		_tcsncpy(fileData.fileName, pszFileName, countof(fileData.fileName));
	else
		_tcsncpy(fileData.fileName, szTempFileName, countof(fileData.fileName));

	fileData.bWritable = TRUE;
	fileData.dwOpenFlags = CREATE_ALWAYS;
	fileData.dwSize = nbytes;
	bOpenSuccess = files_openFileMapped(&fileData);

	if (bOpenSuccess)
	{
		// Should these Unicode codeset conversions be moved
		// into the iconvert (editlib/cs2cs.*) module ?

		if (m_nSourceEncoding == -20)
		{
			ucr::convertToBuffer(text, fileData.pMapBase, ucr::UCS2LE);
		}
		else if (m_nSourceEncoding == -21)
		{
			ucr::convertToBuffer(text, fileData.pMapBase, ucr::UCS2BE);
		}
		else if (m_nSourceEncoding == -22)
		{
			ucr::convertToBuffer(text, fileData.pMapBase, ucr::UTF8);
		}
		else 
		{
#ifdef _UNICODE
			// (We're ignoring m_nSourceEncoding here, which ought to be relevant
			// altho I don't think it ever gets set right now -- Perry, 2003-09-24)
			ucr::convertToBuffer(text, fileData.pMapBase, ucr::NONE);
#else
			if (m_nSourceEncoding >= 0)
			{
				LPTSTR pszBuf;
				iconvert_new((LPCTSTR)text, &pszBuf, 1,
						m_nSourceEncoding, m_nSourceEncoding == 15);
				CopyMemory(fileData.pMapBase, pszBuf, nbytes);
				free(pszBuf);
			}
			else
			{
				CopyMemory(fileData.pMapBase, (void *)(LPCTSTR)text, nbytes);
			}
#endif
		}

		// Force flushing of file buffers for user files
		BOOL bflush = !bTempFile;
		files_closeFileMapped(&fileData, nbytes, bflush);
		
		if (!bTempFile)
		{
			// If we are saving user files
			// we need an unpacker/packer, at least a "do nothing" one
			ASSERT(infoUnpacker != NULL);
			// repack the file here, overwrite the temporary file we did save in
			CString csTempFileName = szTempFileName;
			infoUnpacker->subcode = unpackerSubcode;
			if (!FileTransform_Packing(csTempFileName, *infoUnpacker))
			{
				::DeleteFile(szTempFileName);
				// returns now, don't overwrite the original file
				return SAVE_PACK_FAILED;
			}
			// the temp filename may have changed during packing
			if (csTempFileName != szTempFileName)
			{
				::DeleteFile(szTempFileName);
				_tcscpy(szTempFileName, csTempFileName);
			}

			// Write tempfile over original file
			if (::CopyFile(szTempFileName, pszFileName, FALSE))
			{
				::DeleteFile(szTempFileName);
				if (bClearModifiedFlag)
				{
					SetModified(FALSE);
					m_nSyncPosition = m_nUndoPosition;
				}
				bSaveSuccess = TRUE;
			}
		}
		else
		{
			if (bClearModifiedFlag)
			{
				SetModified(FALSE);
				m_nSyncPosition = m_nUndoPosition;
			}
			bSaveSuccess = TRUE;
		}
	}
	if (bSaveSuccess)
		return SAVE_DONE;
	else
		return SAVE_FAILED;
}

/// Replace text of line (no change to eol)
void CMergeDoc::CDiffTextBuffer::ReplaceLine(CCrystalTextView * pSource, int nLine, const CString &strText, int nAction /*=CE_ACTION_UNKNOWN*/)
{
	if (GetLineLength(nLine)>0)
		DeleteText(pSource, nLine, 0, nLine, GetLineLength(nLine), nAction);
	int endl,endc;
	if (! strText.IsEmpty())
		InsertText(pSource, nLine, 0, strText, endl,endc, nAction);
}

/// Return pointer to the eol chars of this string, or pointer to empty string if none
LPCTSTR getEol(const CString &str)
{
	if (str.GetLength()>1 && str[str.GetLength()-2]=='\r' && str[str.GetLength()-1]=='\n')
		return (LPCTSTR)str + str.GetLength()-2;
	if (str.GetLength()>0 && (str[str.GetLength()-1]=='\r' || str[str.GetLength()-1]=='\n'))
		return (LPCTSTR)str + str.GetLength()-1;
	return _T("");
}

/// Replace line (removing any eol, and only including one if in strText)
void CMergeDoc::CDiffTextBuffer::ReplaceFullLine(CCrystalTextView * pSource, int nLine, const CString &strText, int nAction /*=CE_ACTION_UNKNOWN*/)
{
	if (_tcscmp(GetLineEol(nLine), getEol(strText)) == 0)
	{
		// (optimization) eols are the same, so just replace text inside line
		// we must clean strText from its eol...
		CString strTextWithoutEol = strText;
		int newLength = strTextWithoutEol.GetLength()- _tcslen(getEol(strTextWithoutEol));
		strTextWithoutEol.ReleaseBuffer(newLength);
		ReplaceLine(pSource, nLine, strTextWithoutEol, nAction);
		return;
	}

	// we may need a last line as the DeleteText end is (x=0,y=line+1)
	if (nLine+1 == GetLineCount())
		InsertGhostLine (pSource, GetLineCount());

	if (GetFullLineLength(nLine))
		DeleteText(pSource, nLine, 0, nLine+1, 0, nAction); 
	int endl,endc;
	if (! strText.IsEmpty())
		InsertText(pSource, nLine, 0, strText, endl,endc, nAction);
}

/// Determine path for temporary files and init those files
BOOL CMergeDoc::InitTempFiles(const CString& srcPathL, const CString& strPathR)
{
	TCHAR strTempPath[MAX_PATH] = {0};

	if (!::GetTempPath(MAX_PATH, strTempPath))
	{
		return FALSE;
	}

	// Set temp paths for buffers
	m_ltBuf.SetTempPath(strTempPath);
	m_rtBuf.SetTempPath(strTempPath);

	if (m_strTempLeftFile.IsEmpty())
	{
		TCHAR name[MAX_PATH];
		::GetTempFileName (strTempPath, _T ("_LT"), 0, name);
		m_strTempLeftFile = name;

		if (!::CopyFile(srcPathL, m_strTempLeftFile, FALSE))
			return FALSE;
		::SetFileAttributes(m_strTempLeftFile, FILE_ATTRIBUTE_NORMAL);
	}
	if (m_strTempRightFile.IsEmpty())
	{
		TCHAR name[MAX_PATH];
		::GetTempFileName (strTempPath, _T ("_RT"), 0, name);
		m_strTempRightFile = name;

		if (!::CopyFile(strPathR, m_strTempRightFile, FALSE))
			return FALSE;
		::SetFileAttributes(m_strTempRightFile, FILE_ATTRIBUTE_NORMAL);
	}
	return TRUE;
}

/// Remove temporary files
void CMergeDoc::CleanupTempFiles()
{
	if (!m_strTempLeftFile.IsEmpty())
	{
		if (::DeleteFile(m_strTempLeftFile))
			m_strTempLeftFile = _T("");
		else
			LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s")
				, m_strTempLeftFile, GetSysError(GetLastError())));
	}
	if (!m_strTempRightFile.IsEmpty())
	{
		if (::DeleteFile(m_strTempRightFile))
			m_strTempRightFile = _T("");
		else
			LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s")
				, m_strTempRightFile, GetSysError(GetLastError())));
	}
}

/**
 * @brief Returns TRUE if tempfile already exists
 */
BOOL CMergeDoc::TempFilesExist()
{
	CFileStatus s1,s2;
	return (!m_strTempLeftFile.IsEmpty() 
		&& CFile::GetStatus(m_strTempLeftFile, s1)
		&& !m_strTempRightFile.IsEmpty() 
		&& CFile::GetStatus(m_strTempRightFile, s2));
}

void CMergeDoc::FlushAndRescan(BOOL bForced /* =FALSE */)
{
	// Ignore suppressing when forced rescan
	if (!bForced)
		if (!m_bEnableRescan) return;

	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_RESCANNING));

	CMDIFrameWnd* mainWnd = dynamic_cast<CMDIFrameWnd*>(AfxGetMainWnd());
	CMDIChildWnd* diffWnd = dynamic_cast<CMDIChildWnd*>(mainWnd->MDIGetActive());
	CCrystalEditView* curView = dynamic_cast<CCrystalEditView*>(diffWnd->GetActiveView());

	int nRescanResult = RESCAN_OK;

	m_pLeftView->PushCursors();
	m_pRightView->PushCursors();

	nRescanResult = Rescan(bForced);
	UpdateAllViews(NULL);

	m_pLeftView->PopCursors();
	m_pRightView->PopCursors();

	if (curView)
		curView->EnsureVisible(curView->GetCursorPos());

	// Show possible error after updating screen
	if (nRescanResult != RESCAN_OK &&
			nRescanResult != RESCAN_SUPPRESSED)
		ShowRescanError(nRescanResult);
}

/**
 * @brief Saves both files
 */
void CMergeDoc::OnFileSave() 
{
	BOOL bLSaveSuccess = FALSE;
	BOOL bRSaveSuccess = FALSE;
	BOOL bLModified = FALSE;
	BOOL bRModified = FALSE;

	if (m_ltBuf.IsModified() && !m_ltBuf.GetReadOnly())
	{
		bLModified = TRUE;
		DoSave(m_strLeftFile, bLSaveSuccess, TRUE );
	}

	if (m_rtBuf.IsModified() && !m_rtBuf.GetReadOnly())
	{
		bRModified = TRUE;
		DoSave(m_strRightFile, bRSaveSuccess, FALSE);
	}

	// If file were modified and saving succeeded,
	// update status on dir view
	if ((bLModified && bLSaveSuccess) || 
		(bRModified && bRSaveSuccess))
	{
		// If DirDoc contains diffs
		if (m_pDirDoc->m_pCtxt)
		{
			bool unified = (m_nDiffs==0); // true if status should be set to identical
			m_pDirDoc->UpdateChangedItem(m_strLeftFile, m_strRightFile, unified);
		}
	}
}

/**
 * @brief Saves left-side file
 */
void CMergeDoc::OnFileSaveLeft()
{
	BOOL bLSaveSuccess = FALSE;
	BOOL bLModified = FALSE;

	if (m_ltBuf.IsModified() && !m_ltBuf.GetReadOnly())
	{
		bLModified = TRUE;
		DoSave(m_strLeftFile, bLSaveSuccess, TRUE );
	}

	// If file were modified and saving succeeded,
	// update status on dir view
	if (bLModified && bLSaveSuccess)
	{
		// If DirDoc contains diffs
		if (m_pDirDoc->m_pCtxt)
		{
			bool unified = (m_nDiffs==0); // true if status should be set to identical
			m_pDirDoc->UpdateChangedItem(m_strLeftFile, m_strRightFile, unified);
		}
	}
}

/**
 * @brief Saves right-side file
 */
void CMergeDoc::OnFileSaveRight()
{
	BOOL bRSaveSuccess = FALSE;
	BOOL bRModified = FALSE;

	if (m_rtBuf.IsModified() && !m_rtBuf.GetReadOnly())
	{
		bRModified = TRUE;
		DoSave(m_strRightFile, bRSaveSuccess, FALSE);
	}

	// If file were modified and saving succeeded,
	// update status on dir view
	if (bRModified && bRSaveSuccess)
	{
		// If DirDoc contains diffs
		if (m_pDirDoc->m_pCtxt)
		{
			bool unified = (m_nDiffs==0); // true if status should be set to identical
			m_pDirDoc->UpdateChangedItem(m_strLeftFile, m_strRightFile, unified);
		}
	}
}

/**
 * @brief Saves left-side file with name asked
 */
void CMergeDoc::OnFileSaveAsLeft()
{
	BOOL result = TRUE;
	CString s;
	CString title;
	BOOL bSaveSuccess = FALSE;
	
	// use a temp packer
	// first copy the m_pInfoUnpacker
	// if an error arises during packing, change and take a "do nothing" packer
	PackingInfo infoTempUnpacker = *m_pInfoUnpacker;

	VERIFY(title.LoadString(IDS_SAVE_AS_TITLE));
	if (SelectFile(s, m_strLeftFile, title, NULL, FALSE))
	{
		bSaveSuccess = m_ltBuf.SaveToFile(s, FALSE, &infoTempUnpacker);
		if(bSaveSuccess != SAVE_DONE)
		{
			BOOL bSaveAsSuccess = bSaveSuccess;
			// Saving failed, user may save to another location if wants to
			do
				result = TrySaveAs(s, bSaveAsSuccess, TRUE, &infoTempUnpacker);
			while (!result);
		}
		else
		{
			// Update filename
			m_strLeftFile = s;
			UpdateHeaderPath(TRUE);
		}
	}
}

/**
 * @brief Saves right-side file with name asked
 */
void CMergeDoc::OnFileSaveAsRight()
{
	BOOL result = TRUE;
	CString s;
	CString title;
	BOOL bSaveSuccess = FALSE;
	
	// use a temp packer
	// first copy the m_pInfoUnpacker
	// if an error arises during packing, change and take a "do nothing" packer
	PackingInfo infoTempUnpacker = *m_pInfoUnpacker;

	VERIFY(title.LoadString(IDS_SAVE_AS_TITLE));
	if (SelectFile(s, m_strRightFile, title, NULL, FALSE))
	{
		bSaveSuccess = m_rtBuf.SaveToFile(s, FALSE);
		if(bSaveSuccess != SAVE_DONE)
		{
			BOOL bSaveAsSuccess = bSaveSuccess;
			// Saving failed, user may save to another location if wants to
			do
				result = TrySaveAs(s, bSaveAsSuccess, FALSE, &infoTempUnpacker);
			while (!result);
		}
		else
		{
			// Update filename
			m_strRightFile = s;
			UpdateHeaderPath(FALSE);
		}
	}
}

/**
 * @brief Update diff-number pane text
 */
void CMergeDoc::OnUpdateStatusNum(CCmdUI* pCmdUI) 
{
	CString sIdx,sCnt,s;
	
	// Files are identical - show text "Identical"
	if (m_nDiffs <= 0)
		VERIFY(s.LoadString(IDS_IDENTICAL));
	
	// There are differences, but no selected diff
	// - show amount of diffs
	else if (GetCurrentDiff() < 0)
	{
		sCnt.Format(_T("%ld"), m_nDiffs);
		AfxFormatString1(s, IDS_NO_DIFF_SEL_FMT, sCnt); 
	}
	
	// There are differences and diff selected
	// - show diff number and amount of diffs
	else
	{
		sIdx.Format(_T("%ld"), GetCurrentDiff()+1);
		sCnt.Format(_T("%ld"), m_nDiffs);
		AfxFormatString2(s, IDS_DIFF_NUMBER_STATUS_FMT, sIdx, sCnt); 
	}
	pCmdUI->SetText(s);
}

bool CMergeDoc::CDiffTextBuffer::curUndoGroup()
{
	return (m_aUndoBuf.GetSize()!=0 && m_aUndoBuf[0].m_dwFlags&UNDO_BEGINGROUP);
}

void CMergeDoc::PrimeTextBuffers()
{
	UINT LeftExtras=0;   // extra lines added to view
	UINT RightExtras=0;   // extra lines added to view

	// walk the diff stack and flag the line codes
	SetCurrentDiff(-1);
	UINT nTrivials = 0;
	for (UINT nDiff=0; nDiff < m_nDiffs; ++nDiff)
	{
		DIFFRANGE &curDiff = m_diffs[nDiff];

		// handle left-only for the left view
		switch (curDiff.op)
		{
		case OP_LEFTONLY:
			// left side
			{
				// just flag the lines
				curDiff.dbegin0 = curDiff.begin0+LeftExtras;
				curDiff.dend0 = curDiff.end0+LeftExtras;
				curDiff.blank0 = -1;
				for (UINT i=curDiff.dbegin0; i <= curDiff.dend0; i++)
				{
					m_ltBuf.SetLineFlag(i, LF_DIFF, TRUE, FALSE, FALSE);
				}
			}
			// right side
			{

				// need to insert blanks to compensate for diff on other side
				curDiff.dbegin1 = curDiff.begin1+RightExtras;
				curDiff.dend1 = curDiff.dbegin1+(curDiff.end0-curDiff.begin0);
				curDiff.blank1 = curDiff.dbegin1;
				for (UINT i=curDiff.dbegin1; i <= curDiff.dend1; i++)
				{
					m_rtBuf.InsertLine(NULL, 0, i);
					m_rtBuf.SetLineFlag(i, LF_GHOST, TRUE, FALSE, FALSE);
					++RightExtras;
				}
			}
			break;
		case OP_RIGHTONLY:
			// right side
			{
				// just flag the lines
				curDiff.dbegin1 = curDiff.begin1+RightExtras;
				curDiff.dend1 = curDiff.end1+RightExtras;
				curDiff.blank1 = -1;
				for (UINT i=curDiff.dbegin1; i <= curDiff.dend1; i++)
				{
					m_rtBuf.SetLineFlag(i, LF_DIFF, TRUE, FALSE, FALSE);
				}
			}
			// left side
			{
				// need to insert blanks to compensate for diff on other side
				curDiff.dbegin0 = curDiff.begin0+LeftExtras;
				curDiff.dend0 = curDiff.dbegin0+(curDiff.end1-curDiff.begin1);
				curDiff.blank0 = curDiff.dbegin0;
				for (UINT i=curDiff.dbegin0; i <= curDiff.dend0; i++)
				{
					m_ltBuf.InsertLine(NULL, 0, i);
					m_ltBuf.SetLineFlag(i, LF_GHOST, TRUE, FALSE, FALSE);
					++LeftExtras;
				}
			}
			break;
		case OP_TRIVIAL:
			++nTrivials;
			// fall through and handle as diff
		case OP_DIFF:
			// left side
			{
				// just flag the lines
				curDiff.dbegin0 = curDiff.begin0+LeftExtras;
				curDiff.dend0 = curDiff.end0+LeftExtras;
				for (UINT i=curDiff.dbegin0; i <= curDiff.dend0; i++)
				{
					DWORD dflag = (curDiff.op == OP_DIFF) ? LF_DIFF : LF_TRIVIAL;
					// set, don't remove previous line, don't update
					m_ltBuf.SetLineFlag(i, dflag, TRUE, FALSE, FALSE);
				}

				// insert blanks if needed
				int blanks = (curDiff.end1-curDiff.begin1)-(curDiff.end0-curDiff.begin0);
				if (blanks>0)
				{
					curDiff.blank0 = curDiff.dend0+1;
					curDiff.blank1 = -1;
					for (int b=0; b < blanks; b++)
					{
						int idx = curDiff.blank0+b;
						m_ltBuf.InsertLine(NULL, 0, idx);
						// ghost lines opposite to trivial lines are ghost and trivial
						m_ltBuf.SetLineFlag(idx, LF_GHOST, TRUE, FALSE, FALSE);
						if (curDiff.op == OP_TRIVIAL)
							m_ltBuf.SetLineFlag(idx, LF_TRIVIAL, TRUE, FALSE, FALSE);
						curDiff.dend0++;
						LeftExtras++;
					}
				}
			}
			// right side
			{
				// just flag the lines
				curDiff.dbegin1 = curDiff.begin1+RightExtras;
				curDiff.dend1 = curDiff.end1+RightExtras;
				for (UINT i=curDiff.dbegin1; i <= curDiff.dend1; i++)
				{
					DWORD dflag = (curDiff.op == OP_DIFF) ? LF_DIFF : LF_TRIVIAL;
					// set, don't remove previous line, don't update
					m_rtBuf.SetLineFlag(i, dflag, TRUE, FALSE, FALSE);
				}

				// insert blanks if needed
				int blanks = (curDiff.end0-curDiff.begin0)-(curDiff.end1-curDiff.begin1);
				if (blanks>0)
				{
					curDiff.blank1 = curDiff.dend1+1;
					curDiff.blank0 = -1;
					for (int b=0; b < blanks; b++)
					{
						int idx = curDiff.blank1+b;
						m_rtBuf.InsertLine(NULL, 0, idx);
						// ghost lines opposite to trivial lines are ghost and trivial
						m_rtBuf.SetLineFlag(idx, LF_GHOST, TRUE, FALSE, FALSE);
						if (curDiff.op == OP_TRIVIAL)
							m_rtBuf.SetLineFlag(idx, LF_TRIVIAL, TRUE, FALSE, FALSE);
						curDiff.dend1++;
						++RightExtras;
					}
				}
			}
			break;
		}
	}

	if (nTrivials)
	{
		// The following code deletes all trivial changes
		//
		// #1) Copy nontrivial diffs into new array
		CArray<DIFFRANGE,DIFFRANGE> newdiffs;
		newdiffs.SetSize(m_diffs.GetSize()-nTrivials);
		UINT i,j;
		for (i=0,j=0; j < m_nDiffs; ++j)
		{
			// j is index into m_diffs
			// i is index into newdiffs
			// i grows more slowly than j, as i skips trivials
			if (m_diffs[j].op != OP_TRIVIAL)
			{
				newdiffs[i] = m_diffs[j];
				++i;
			}
		}
		// #2) Now copy from new array back into master array
		m_nDiffs = newdiffs.GetSize();
		m_diffs.SetSize(m_nDiffs);
		for (i=0; i < m_nDiffs; ++i)
		{
			m_diffs[i] = newdiffs[i];
		}
	}

	m_ltBuf.FinishLoading();
	m_rtBuf.FinishLoading();
}

/**
 * @brief Saves file if file is modified. If file is
 * opened from directory compare, status there is updated.
 */
BOOL CMergeDoc::SaveHelper()
{
	BOOL result = TRUE;
	CString s;
	BOOL bLSaveSuccess = FALSE;
	BOOL bRSaveSuccess = FALSE;
	BOOL bLModified = FALSE;
	BOOL bRModified = FALSE;

	AfxFormatString1(s, IDS_SAVE_FMT, m_strLeftFile); 
	if (m_ltBuf.IsModified())
	{
		bLModified = TRUE;
		switch(AfxMessageBox(s, MB_YESNOCANCEL|MB_ICONQUESTION))
		{
		case IDYES:
			if (!DoSave(m_strLeftFile, bLSaveSuccess, TRUE))
				result=FALSE;
			break;
		case IDNO:
			break;
		default:  // IDCANCEL
			result=FALSE;
			break;
		}
	}

	AfxFormatString1(s, IDS_SAVE_FMT, m_strRightFile); 
	if (m_rtBuf.IsModified())
	{
		bRModified = TRUE;
		switch(AfxMessageBox(s, MB_YESNOCANCEL|MB_ICONQUESTION))
		{
		case IDYES:
			if (!DoSave(m_strRightFile, bRSaveSuccess, FALSE))
				result=FALSE;
			break;
		case IDNO:
			break;
		default:  // IDCANCEL
			result=FALSE;
			break;
		}
	}

	// If file were modified and saving was successfull,
	// update status on dir view
	if ((bLModified && bLSaveSuccess) ||
		 (bRModified && bRSaveSuccess))
	{
		// If DirDoc contains diffs
		if (m_pDirDoc->m_pCtxt)
		{
			bool unified = (m_nDiffs==0); // true if status should be set to identical
			m_pDirDoc->UpdateChangedItem(m_strLeftFile, m_strRightFile, unified);
		}
	}
	return result;
}

/** Rescan only if we did not Rescan during the last timeOutInSecond seconds*/
void CMergeDoc::RescanIfNeeded(float timeOutInSecond)
{
	// if we did not rescan during the request timeOut, Rescan
	// else we did Rescan after the request, so do nothing
	COleDateTimeSpan elapsed = COleDateTime::GetCurrentTime() - m_LastRescan;
	if (elapsed.GetTotalSeconds() >= timeOutInSecond)
		// (laoran 08-01-2003) maybe should be FlushAndRescan(TRUE) ??
		FlushAndRescan();
}

/**
 * @brief We have two child views (left & right), so we keep pointers directly
 * at them (the MFC view list doesn't have them both)
 */
void CMergeDoc::SetMergeViews(CMergeEditView * pLeft, CMergeEditView * pRight)
{
	ASSERT(pLeft && !m_pLeftView);
	m_pLeftView = pLeft;
	ASSERT(pRight && !m_pRightView);
	m_pRightView = pRight;
}

/**
 * @brief Someone is giving us pointers to our detail views
 */
void CMergeDoc::SetMergeDetailViews(CMergeDiffDetailView * pLeft, CMergeDiffDetailView * pRight)
{
	ASSERT(pLeft && !m_pLeftDetailView);
	m_pLeftDetailView = pLeft;
	ASSERT(pRight && !m_pRightDetailView);
	m_pRightDetailView = pRight;
}

/**
 * @brief DirDoc gives us its identity just after it creates us
 */
void CMergeDoc::SetDirDoc(CDirDoc * pDirDoc)
{
	ASSERT(pDirDoc && !m_pDirDoc);
	m_pDirDoc = pDirDoc;
}

/**
 * @brief Return pointer to parent frame
 */
CChildFrame * CMergeDoc::GetParentFrame() 
{
	return dynamic_cast<CChildFrame *>(m_pLeftView->GetParentFrame()); 
}

/**
 * @brief DirDoc is closing
 */
void CMergeDoc::DirDocClosing(CDirDoc * pDirDoc)
{
	ASSERT(m_pDirDoc == pDirDoc);
	m_pDirDoc = 0;
	// TODO (Perry 2003-03-30): perhaps merge doc should close now ?
}

/**
 * @brief DirDoc commanding us to close
 */
BOOL CMergeDoc::CloseNow()
{
	if (!SaveHelper())
		return FALSE;

	GetParentFrame()->CloseNow();
	return TRUE;
}

/**
 * @brief Return true if characters match
 */
static bool casematch(BOOL case_sensitive, TCHAR ch1, TCHAR ch2)
{
	if (case_sensitive) 
		return ch1==ch2;
	else 
		return toupper(ch1)==toupper(ch2);
}

/**
 * @brief Find first difference between str1 and str2
 * @param whitespace Value = 1 means ignore changes and
 * value = 2 means ignore all whitespaces
 */
static int firstdiff(BOOL case_sensitive, int whitespace, const CString & str1, const CString & str2)
{
	int i1=0, i2=0;
	while (1)
	{
		if (whitespace==2)
		{
			// we don't care at all about whitespace
			// skip all whitespace on both sides
			while (i1<str1.GetLength() && isspace(str1[i1]))
				++i1;
			while (i2<str2.GetLength() && isspace(str2[i2]))
				++i2;
			if (i1==str1.GetLength())
				return (i2==str2.GetLength()) ? -1 : i1;
			if (i2==str2.GetLength())
				return i1;
		}
		else if (whitespace==1)
		{
			// we don't care about whitespace changes
			if (i1==str1.GetLength())
				return -1;
			if (i2==str2.GetLength())
				return i1;
			if (isspace(str1[i1]))
			{
				if (!isspace(str2[i2]))
					return i1;
				// skip whitespace run on both sides
				while (i1<str1.GetLength() && isspace(str1[i1]))
					++i1;
				if (i1==str1.GetLength())
					return -1;
				while (i2<str2.GetLength() && isspace(str2[i2]))
					++i2;
				if (i2==str2.GetLength())
					return i1;
			}
		}
		else
		{
			// whitespace is like all other characters
			if (i1==str1.GetLength())
				return (i2==str2.GetLength()) ? -1 : i1;
			if (i2==str2.GetLength())
				return i1;
		}
		if (!casematch(case_sensitive, str1[i1], str2[i2]))
			return i1;
		++i1;
		++i2;
	}
}

/**
 * @brief Find last difference between str1 and str2
 * @param whitespace Value = 1 means ignore changes and
 * value = 2 means ignore all whitespaces
 */
static int lastdiff(BOOL case_sensitive, int whitespace, const CString & str1, const CString & str2)
{
	int i1=str1.GetLength()-1, i2=str2.GetLength()-1;
	while (1)
	{
		if (whitespace==2)
		{
			// we don't care at all about whitespace
			// skip all whitespace on boths ides
			while (i1>=0 && isspace(str1[i1]))
				--i1;
			while (i2>=0 && isspace(str2[i2]))
				--i2;
			if (i1<0)
				return (i2<0) ? -1 : 0;
			if (i2<0)
				return i1;
		}
		else if (whitespace==1)
		{
			// we don't care about whitespace changes
			if (i1<0)
				return -1;
			if (i2<0)
				return i1;
			if (isspace(str1[i1]))
			{
				if (!isspace(str2[i2]))
					return i1;
				// skip whitespace run on both sides
				while (i1>=0 && isspace(str1[i1]))
					--i1;
				if (i1<0)
					return -1;
				while (i2>=0 && isspace(str2[i2]))
					--i2;
				if (i2<0)
					return i1;
			}
		}
		else
		{
			// whitespace is like all other characters
			if (i1<0)
				return (i2<0) ? -1 : 0;
			if (i2<0)
				return i1;
		}
		if (!casematch(case_sensitive, str1[i1], str2[i2]))
			return i1;
		--i1;
		--i2;
	}
}

/**
 * @brief Highlight difference in current line
 */
void CMergeDoc::Showlinediff(CMergeEditView * pView)
{
	CMergeEditView * pOther = (pView == m_pLeftView ? m_pRightView : m_pLeftView);
	CRect rectDiff = Computelinediff(pView, pOther);
	if (rectDiff.top == -1)
		return;
	pView->SelectArea(rectDiff.TopLeft(), rectDiff.BottomRight());
	pView->SetCursorPos(rectDiff.TopLeft());
	pView->EnsureVisible(rectDiff.TopLeft());
}

/**
 * @brief Highlight difference in diff pane's current line
 */
void CMergeDoc::Showlinediff(CMergeDiffDetailView * pView)
{
	CMergeDiffDetailView * pOther = (pView == m_pLeftDetailView ? m_pRightDetailView : m_pLeftDetailView);
	CRect rectDiff = Computelinediff(pView, pOther);
	if (rectDiff.top == -1)
		return;
	pView->SelectArea(rectDiff.TopLeft(), rectDiff.BottomRight());
	pView->SetCursorPos(rectDiff.TopLeft());
	pView->EnsureVisible(rectDiff.TopLeft());
}

// Returns a rectangle of the difference in the current line 
RECT CMergeDoc::Computelinediff(CCrystalTextView * pView, CCrystalTextView * pOther)
{
	DIFFOPTIONS diffOptions = {0};
	m_diffWrapper.GetOptions(&diffOptions);

	int line = pView->GetCursorPos().y;
	int width = pView->GetLineLength(line);

	CString str1 = pView->GetLineChars(line);
	CString str2 = pOther->GetLineChars(line);

	int begin = firstdiff(!diffOptions.bIgnoreCase, diffOptions.nIgnoreWhitespace, str1, str2);

	if (begin<0)
	{
		MessageBox(0, _T("No difference"), _T("Line difference"), MB_OK);
		return CRect(-1,-1,-1,-1);
	}
	if (begin>=width)
		begin = width;

	int end = lastdiff(!diffOptions.bIgnoreCase, diffOptions.nIgnoreWhitespace, str1, str2)+1;
	if (end>=width)
		end = width;

	CPoint ptBegin(begin,line), ptEnd(end,line);
	return CRect(ptBegin, ptEnd);
}

/**
* @brief Loads file to buffer and shows load-errors
*
* @param sFileName File to open
* @param bLeft Left/right-side file
*
* @return Tells if files were loaded succesfully
*
* @note Binary loading message could be improved...
*
* @sa CMergeDoc::OpenDocs()
*
*/
int CMergeDoc::LoadFile(CString sFileName, BOOL bLeft, BOOL & readOnly)
{
	CDiffTextBuffer *pBuf;
	CString sError;
	int retVal = FRESULT_ERROR;

	if (bLeft)
	{
		pBuf = &m_ltBuf;
		m_strLeftFile = sFileName;
	}
	else
	{
		pBuf = &m_rtBuf;
		m_strRightFile = sFileName;
	}

	retVal = pBuf->LoadFromFile(sFileName, m_pInfoUnpacker, m_strBothFilenames, readOnly);

	if (retVal != FRESULT_OK)
	{
		if (retVal == FRESULT_ERROR)
			AfxFormatString1(sError, IDS_ERROR_FILE_NOT_FOUND, sFileName);
		else if (retVal == FRESULT_BINARY)
		{
			sError.LoadString(IDS_FILEBINARY);
			sError += "\n(";
			sError += sFileName;
			sError += ")";
		}

		// Clear buffer, but don't leave as uninitialised
		pBuf->FreeAll();
		pBuf->InitNew();
		AfxMessageBox(sError, MB_OK | MB_ICONSTOP);
	}
	return retVal;
}

/**
* @brief Loads files and does initial rescan
*
* @param sLeftFile File to open to left side
* @param sRightFile File to open to right side
* @param bROLeft Is left file read-only
* @param bRORight Is right file read-only
*
* @return Tells if files were loaded and scanned succesfully
*
* @note Options are still read from CMainFrame, this will change
*
* @sa CMainFrame::ShowMergeDoc()
*
*/
BOOL CMergeDoc::OpenDocs(CString sLeftFile, CString sRightFile,
		BOOL bROLeft, BOOL bRORight)
{
	DIFFOPTIONS diffOptions = {0};
	int nRescanResult = RESCAN_OK;
	m_diffWrapper.GetOptions(&diffOptions);

	m_ltBuf.SetEolSensitivity(diffOptions.bEolSensitive);
	m_rtBuf.SetEolSensitivity(diffOptions.bEolSensitive);
	undoTgt.clear();
	curUndo = undoTgt.begin();

	// build the text being filtered, "|" separates files as it is forbidden in filenames
	m_strBothFilenames = sLeftFile + "|" + sRightFile;

	// Load left side file
	int nLeftSuccess = LoadFile(sLeftFile, TRUE, bROLeft);
	
	// Load right side only if left side was succesfully loaded
	int nRightSuccess = FRESULT_ERROR;
	if (nLeftSuccess == FRESULT_OK)
		nRightSuccess = LoadFile(sRightFile, FALSE, bRORight);

	// Bail out if either side failed
	if (nLeftSuccess != FRESULT_OK || nRightSuccess != FRESULT_OK)
	{
		GetParentFrame()->DestroyWindow();
		return FALSE;
	}
	
	// Set read-only statuses
	m_ltBuf.SetReadOnly(bROLeft);
	m_rtBuf.SetReadOnly(bRORight);

	nRescanResult = Rescan();

	// Open different and identical files
	if (nRescanResult == RESCAN_OK ||
		nRescanResult == RESCAN_IDENTICAL)
	{
		CMergeEditView * pLeft = GetLeftView();
		CMergeEditView * pRight = GetRightView();
		CMergeDiffDetailView * pLeftDetail = GetLeftDetailView();
		CMergeDiffDetailView * pRightDetail = GetRightDetailView();
		
		// scroll to first diff
		if(mf->m_bScrollToFirst && m_diffs.GetSize() != 0)
			pLeft->SelectDiff(0, TRUE, FALSE);

		// Enable/disable automatic rescan (rescanning after edit)
		pLeft->EnableRescan(mf->m_bAutomaticRescan);
		pRight->EnableRescan(mf->m_bAutomaticRescan);

		// set the document types
		CString sext;
		SplitFilename(sLeftFile, 0, 0, &sext);
		pLeft->SetTextType(sext);
		SplitFilename(sRightFile, 0, 0, &sext);
		pRight->SetTextType(sext);

		pLeftDetail->SetTextType(sext);
		pRightDetail->SetTextType(sext);

		// SetTextType will revert to language dependent defaults for tab
		pLeft->SetTabSize(mf->m_nTabSize);
		pRight->SetTabSize(mf->m_nTabSize);
		pLeft->SetViewTabs(mf->m_bViewWhitespace);
		pRight->SetViewTabs(mf->m_bViewWhitespace);
		pLeft->SetViewEols(mf->m_bViewWhitespace, mf->m_bAllowMixedEol);
		pRight->SetViewEols(mf->m_bViewWhitespace, mf->m_bAllowMixedEol);

		pLeftDetail->SetTabSize(mf->m_nTabSize);
		pRightDetail->SetTabSize(mf->m_nTabSize);
		pLeftDetail->SetViewTabs(mf->m_bViewWhitespace);
		pRightDetail->SetViewTabs(mf->m_bViewWhitespace);
		pLeftDetail->SetViewEols(mf->m_bViewWhitespace, mf->m_bAllowMixedEol);
		pRightDetail->SetViewEols(mf->m_bViewWhitespace, mf->m_bAllowMixedEol);

		// Enable Backspace at beginning of line
		pLeft->SetDisableBSAtSOL(FALSE);
		pRight->SetDisableBSAtSOL(FALSE);
		
		// set the frame window header
		UpdateHeaderPath(TRUE);
		UpdateHeaderPath(FALSE);

		// Set tab type (tabs/spaces)
		BOOL bInsertTabs = (mf->m_nTabType == 0);
		pLeft->SetInsertTabs(bInsertTabs);
		pRight->SetInsertTabs(bInsertTabs);

		// Inform user that files are identical
		if (nRescanResult == RESCAN_IDENTICAL)
			ShowRescanError(nRescanResult);
	}
	else
	{
		// CMergeDoc::Rescan fails if files are identical, or 
		// does not exist on both sides or the really arcane case
		// that the temp files couldn't be created, which is too
		// obscure to bother reporting if you can't write to your
		// temp directory, doing nothing is graceful enough for that).
		ShowRescanError(nRescanResult);
		GetParentFrame()->DestroyWindow();
		return FALSE;
	}
	return TRUE;
}

/**
 * @brief Read doc settings from registry
 *
 * @note Currently loads only diffutils settings, but later others too
 */
void CMergeDoc::ReadSettings()
{
	DIFFOPTIONS diffOptions = {0};
	
	// We have to first get current options
	CDiffWrapper::ReadDiffOptions(&diffOptions);
	m_diffWrapper.SetOptions(&diffOptions);
}

/**
 * @brief Write path and filename to headerbar
 * @note SetHeaderText() does not repaint unchanged text
 */
void CMergeDoc::UpdateHeaderPath(BOOL bLeft)
{
	CChildFrame *pf = GetParentFrame();
	ASSERT(pf);
	int nPane = 0;
	CString sText;
	BOOL bChanges = FALSE;

	if (bLeft)
	{
		if (!mf->m_strLeftDesc.IsEmpty())
			sText = mf->m_strLeftDesc;
		else
			sText = m_strLeftFile;
		bChanges = m_ltBuf.IsModified();
		nPane = 0;
	}
	else
	{
		if (!mf->m_strRightDesc.IsEmpty())
			sText = mf->m_strRightDesc;
		else
			sText = m_strRightFile;
		bChanges = m_rtBuf.IsModified();
		nPane = 1;
	}

	if (bChanges)
		sText.Insert(0, _T("* "));

	pf->SetHeaderText(nPane, sText);
}