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
// MergeDoc.cpp : implementation of the CMergeDoc class
//

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
#include "cs2cs.h"
#include "childFrm.h"
#include "dirdoc.h"
#include "files.h"
#include "WaitStatusCursor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


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
	m_bNeedIdleRescan = FALSE;
	// COleDateTime m_LastRescan
	curUndo = undoTgt.begin();
	m_pLeftView=NULL;
	m_pRightView=NULL;
	m_pDirDoc=NULL;
}
#pragma warning(default:4355)

CMergeDoc::~CMergeDoc()
{	
	CUndoItem *pitem;
	while (!m_undoList.IsEmpty())
	{
		pitem = (CUndoItem*)m_undoList.RemoveHead();
		delete pitem;
	}
	if (m_pDirDoc)
	{
		m_pDirDoc->MergeDocClosing(this);
		m_pDirDoc = 0;
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


// remove all ghost lines (except last one in file)
// (2003-06-21, Perry: I don't understand why this is necessary, but if this isn't
//  done, more and more gray lines appear in the file)
static void RemoveEmptyLinesExceptLast(CMergeDoc::CDiffTextBuffer * buf)
{
	for(int ct=buf->GetLineCount()-1; ct>=0; --ct)
	{
		if (ct < buf->GetLineCount()-1 && !buf->GetFullLineLength(ct))
		{
			buf->DeleteLine(ct);
		}
		else
			buf->SetLineFlag(ct, LF_WINMERGE_FLAGS, FALSE, FALSE, FALSE);
	}
}

// Save files under edit to temp files & compare again, to update diffs on screen
int CMergeDoc::Rescan(BOOL bForced /* =FALSE */)
{
	if (!bForced)
	{
		if (!m_bEnableRescan)
			return RESCAN_SUPPRESSED;
	}

	m_bNeedIdleRescan = FALSE;
	m_LastRescan = COleDateTime::GetCurrentTime();

	// store modified status
	BOOL ltMod = m_ltBuf.IsModified();
	BOOL rtMod = m_rtBuf.IsModified();

	// remove blank lines and clear winmerge flags
	RemoveEmptyLinesExceptLast(&m_ltBuf);
	RemoveEmptyLinesExceptLast(&m_rtBuf);

	// restore modified status
	m_ltBuf.SetModified(ltMod);
	m_rtBuf.SetModified(rtMod);

	// get the desired files to temp locations so we can edit them dynamically
	if (!TempFilesExist())
	{
		if (!InitTempFiles(m_strLeftFile, m_strRightFile))
			return FALSE;
	}

	// output to temp file
	m_ltBuf.SaveToFile(m_strTempLeftFile, TRUE, CRLF_STYLE_AUTOMATIC, FALSE);
	m_rtBuf.SaveToFile(m_strTempRightFile, TRUE, CRLF_STYLE_AUTOMATIC, FALSE);

	// perform rescan
	struct file_data inf[2];
	char *free0=NULL,*free1=NULL;
	CString sdir0, sdir1, sname0, sname1, sext0, sext1;
	int val,failed=0, depth=0;
	bool same_files=FALSE;
	struct change *e, *p;
	struct change *script=NULL;
	int nResult = RESCAN_OK;

	m_diffs.RemoveAll();
	m_nDiffs=0;
	m_nCurDiff=-1;
	
	SplitFilename(m_strTempLeftFile, &sdir0, &sname0, 0);
	SplitFilename(m_strTempRightFile, &sdir1, &sname1, 0);
	ZeroMemory(&inf[0], sizeof(inf[0]));
	ZeroMemory(&inf[1], sizeof(inf[1]));
	
	/* Both exist and neither is a directory.  */
	int o_binary = always_text_flag ? 0:O_BINARY;
	
	/* Open the files and record their descriptors.  */
	if (sdir0.IsEmpty())
		inf[0].name = sname0;
	else
		inf[0].name = free0 = dir_file_pathname (sdir0, sname0);
	inf[0].desc = -2;
	if (sdir1.IsEmpty())
		inf[1].name = sname1;
	else
		inf[1].name = free1 = dir_file_pathname (sdir1, sname1);
	inf[1].desc = -2;
	if (inf[0].desc == -2)
	{
		if ((inf[0].desc = open (inf[0].name, O_RDONLY|o_binary, 0)) < 0)
		{
			perror_with_name (inf[0].name);
			failed = 1;
		}
		if (inf[1].desc == -2)
		{
			if (same_files)
				inf[1].desc = inf[0].desc;
			else if ((inf[1].desc = open (inf[1].name, O_RDONLY|o_binary, 0)) < 0)
			{
				perror_with_name (inf[1].name);
				failed = 1;
			}
			
			/* Compare the files, if no error was found.  */
			int diff_flag=0;

			script = diff_2_files (inf, depth, &diff_flag);

			// throw the diff into a temp file
			char lpBuffer[MAX_PATH];       // path buffer
			GetTempPath(MAX_PATH,lpBuffer);		// get path to Temp folder
			CString path = CString(lpBuffer) + _T("Diff.txt");

			outfile = fopen(path, "w+");
			if (outfile != NULL)
			{
				print_normal_script(script);
				fclose(outfile);
				outfile=NULL;
			}
			
			struct change *next = script;
			int trans_a0, trans_b0, trans_a1, trans_b1;
			int first0, last0, first1, last1, deletes, inserts, op;
			struct change *thisob, *end;
			
			while (next)
			{
				/* Find a set of changes that belong together.  */
				thisob = next;
				end = find_change(next);
				
				/* Disconnect them from the rest of the changes,
				making them a hunk, and remember the rest for next iteration.  */
				next = end->link;
				end->link = 0;
#ifdef DEBUG
				debug_script (thisob);
#endif
				
				/* Print thisob hunk.  */
				//(*printfun) (thisob);
				{					
					/* Determine range of line numbers involved in each file.  */
					analyze_hunk (thisob, &first0, &last0, &first1, &last1, &deletes, &inserts);
					if (!(!deletes && !inserts))
					{
						if (deletes && inserts)
							op = OP_DIFF;
						else if (deletes)
							op = OP_LEFTONLY;
						else
							op = OP_RIGHTONLY;
						
						/* Print the lines that the first file has.  */
						translate_range (&inf[0], first0, last0, &trans_a0, &trans_b0);
						translate_range (&inf[1], first1, last1, &trans_a1, &trans_b1);
						AddDiffRange(trans_a0-1, trans_b0-1, trans_a1-1, trans_b1-1, (BYTE)op);
						TRACE("left=%d,%d   right=%d,%d   op=%d\n",trans_a0-1, trans_b0-1, trans_a1-1, trans_b1-1, op);
					}
				}
				
				/* Reconnect the script so it will all be freed properly.  */
				end->link = next;
			}
			
			// cleanup the script
			for (e = script; e; e = p)
			{
				p = e->link;
				free (e);
			}

			// If comparing whitespaces and
			// other file has EOL before EOF and other not...
			if (inf[0].missing_newline != inf[1].missing_newline &&
				!mf->m_nIgnoreWhitespace)
			{
				// ..lasf DIFFRANGE of file which has EOL must be
				// fixed to contain last line too
				FixLastDiffRange(inf[1].missing_newline);
			}
			
			cleanup_file_buffers(inf);
			
			/* Close the file descriptors.  */
			if (inf[0].desc >= 0 && close (inf[0].desc) != 0)
			{
				perror_with_name (inf[0].name);
				val = 2;
			}
			if (inf[1].desc >= 0 && inf[0].desc != inf[1].desc
				&& close (inf[1].desc) != 0)
			{
				perror_with_name (inf[1].name);
				val = 2;
			}
			
			// display the files
			if (m_nDiffs>0)
			{
				PrimeTextBuffers();
				m_pLeftView->PrimeListWithFile();
				m_pRightView->PrimeListWithFile();

				// PrimeListWithFile will call resetview which resets tabs
//				mf->m_pLeft->SetTabSize(mf->m_nTabSize);
//				mf->m_pRight->SetTabSize(mf->m_nTabSize);

				/*TODO: int lcnt = m_ltBuf.GetLineCount();
				int rcnt = m_rtBuf.GetLineCount();
				if (lcnt < rcnt)
				{
					m_diffs[m_nDiffs-1].dbegin0 = lcnt;
					m_diffs[m_nDiffs-1].dend0 = rcnt;
					m_diffs[m_nDiffs-1].blank0 = lcnt;
				}
				else if (rcnt < lcnt)
				{
					m_diffs[m_nDiffs-1].dbegin1 = rcnt;
					m_diffs[m_nDiffs-1].dend1 = lcnt;
					m_diffs[m_nDiffs-1].blank1 = rcnt;
				}

				while (lcnt < rcnt)
				{
					mf->m_pLeft->AddItem(lcnt, 0, "");
					mf->m_pLeft->m_pList->SetItemData(lcnt, 1);
					lcnt++;
				}
				while (rcnt < lcnt)
				{
					mf->m_pRight->AddItem(rcnt, 0, "");
					mf->m_pRight->m_pList->SetItemData(rcnt, 1);
					rcnt++;
				}*/
				nResult = RESCAN_OK;
			}
			else if (diff_flag)
			{
				nResult = RESCAN_BINARIES;
			}
			else if (failed)
			{
				nResult = RESCAN_FILE_ERR;
			}
			else
			{
				nResult = RESCAN_IDENTICAL;
			}
		}
	}
	
	if (free0)
		free (free0);
	if (free1)
		free (free1);

	return nResult;
}

void CMergeDoc::AddDiffRange(UINT begin0, UINT end0, UINT begin1, UINT end1, BYTE op)
{
	TRY {
		DIFFRANGE dr = {0};
		dr.begin0 = begin0;
		dr.end0 = end0;
		dr.begin1 = begin1;
		dr.end1 = end1;
		dr.op = op;
		dr.blank0 = dr.blank1 = -1;
		m_diffs.SetAtGrow(m_nDiffs, dr);
		m_nDiffs++;
	}
	CATCH_ALL(e)
	{
		TCHAR msg[1024] = {0};
		e->GetErrorMessage(msg, 1024);
		AfxMessageBox(msg, MB_ICONSTOP);
	}
	END_CATCH_ALL;
}

// Expand last DIFFRANGE of file by one line to contain last line
// after EOL
void CMergeDoc::FixLastDiffRange(BOOL left)
{
	DIFFRANGE dr = {0};
	int count = m_diffs.GetSize();
	dr = m_diffs.GetAt(count - 1);

	if (left)
	{
		if (dr.op == OP_RIGHTONLY)
			dr.op = OP_DIFF;
		dr.end0++;
	}
	else
	{
		if (dr.op == OP_LEFTONLY)
			dr.op = OP_DIFF;
		dr.end1++;
	}

	m_diffs.SetAt(count - 1, dr); 
}

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

// An instance of RescanSuppress prevents rescan during its lifetime
// (or until its Clear method is called, which ends its effect).
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

		POSITION pos = GetFirstViewPosition();
		CCrystalTextView* curView = dynamic_cast<CCrystalTextView*>(GetNextView(pos));
		if(bSrcLeft)
		{
			curView = dynamic_cast<CCrystalTextView*>(GetNextView(pos));
		}

		dbuf.BeginUndoGroup();
		if (cd_blank>=0)
		{
			// text was missing, so delete rest of lines on both sides
			// delete only on destination side since rescan will clear the other side
			CMergeEditView *active = static_cast<CMergeEditView*>(
									 static_cast<CMDIFrameWnd*>(AfxGetMainWnd())
									 ->MDIGetActive()->GetActiveView());
			CPoint pt = active->GetCursorPos();
			if(cd_blank==0)
			{
				if(pt.y>=cd_blank && pt.y<=cd_dend)
				{
					pt.y = cd_blank;
					active->SetCursorPos(pt);
				}
				dbuf.DeleteText(NULL, cd_blank, 0, cd_dend+1, 0, CE_ACTION_DELETE, FALSE);
			}
			else
			{
				if(pt.y>=cd_blank && pt.y<=cd_dend)
				{
					pt.y = cd_blank-1;
					// If removing first line, set X to 0
					// Calculating line length is unneccessary work
					pt.x = 0;
					active->SetCursorPos(pt);
				}
				dbuf.DeleteText(NULL, cd_blank-1, dbuf.GetLineLength(cd_blank-1), cd_dend, dbuf.GetLineLength(cd_dend), CE_ACTION_DELETE, FALSE);
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
			// clear the line flags
			sbuf.SetLineFlag(i, LF_WINMERGE_FLAGS, FALSE, FALSE, FALSE);
			dbuf.SetLineFlag(i, LF_WINMERGE_FLAGS, FALSE, FALSE, FALSE);
			// text exists on left side, so just replace
			strLine = _T("");
			sbuf.GetFullLine(i, strLine);
			dbuf.ReplaceFullLine(i, strLine);
			dbuf.FlushUndoGroup(curView);
			dbuf.BeginUndoGroup(TRUE);
		}
		dbuf.FlushUndoGroup(NULL);


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

// Return false when saving fails, so we can ask again
// bSaveSuccess is TRUE if saving succeeded
BOOL CMergeDoc::TrySaveAs(CString strPath, BOOL &bSaveSuccess, BOOL bLeft)
{
	BOOL result = TRUE;
	CString s;
	CString title;

	bSaveSuccess = FALSE;
	AfxFormatString1(s, IDS_FILESAVE_FAILED, strPath);
	switch(AfxMessageBox(s, MB_YESNO|MB_ICONQUESTION))
	{
	case IDYES:
		VERIFY(title.LoadString(IDS_SAVE_AS_TITLE));
		if (SelectFile(s, strPath, title, NULL, FALSE))
		{
			strPath = s;
			if (bLeft)
				bSaveSuccess = m_ltBuf.SaveToFile(strPath, FALSE);
			else
				bSaveSuccess = m_rtBuf.SaveToFile(strPath, FALSE);

			if (!bSaveSuccess)
				result = FALSE;
		}
		break;
	case IDNO:
		break;
	}
	return result;
}

// Return value tells if caller can continue (no errors)
// bSaveSuccess is TRUE if file saving succeeded
BOOL CMergeDoc::DoSave(LPCTSTR szPath, BOOL &bSaveSuccess, BOOL bLeft)
{
	CString strSavePath(szPath);

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
		result = m_ltBuf.SaveToFile(strSavePath, FALSE);
		if(result)
		{
			bSaveSuccess = TRUE;
			m_strLeftFile = strSavePath;
			CChildFrame *parent = dynamic_cast<CChildFrame*>(dynamic_cast<CMDIFrameWnd*>(AfxGetMainWnd())->MDIGetActive());
			if(parent)
			{
				parent->SetHeaderText(0, m_strLeftFile);
			}
		}
		else
		{
			// Saving failed, user may save to another location if wants to
			// TODO: proper fix for handling save success here;
			// problem is we cannot return bSaveSuccess because callers use
			// it to determine if file statuses should be changed.
			BOOL bSaveAsSuccess;
			while (!result)
				result = TrySaveAs(strSavePath, bSaveAsSuccess, bLeft);
		}
	}
	else
	{
		result = m_rtBuf.SaveToFile(strSavePath, FALSE);
		if(result)
		{
			bSaveSuccess = TRUE;
			m_strRightFile = strSavePath;
			CChildFrame *parent = dynamic_cast<CChildFrame*>(dynamic_cast<CMDIFrameWnd*>(AfxGetMainWnd())->MDIGetActive());
			if(parent)
			{
				parent->SetHeaderText(1, m_strRightFile);
			}
		}
		else
		{
			// Saving failed, user may save to another location if wants to
			// TODO: proper fix for handling save success here;
			// problem is we cannot return bSaveSuccess because callers use
			// it to determine if file statuses should be changed.
			BOOL bSaveAsSuccess;
			while (!result)
				result = TrySaveAs(strSavePath, bSaveAsSuccess, bLeft);
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

// Checks if line is inside diff
BOOL CMergeDoc::LineInDiff(UINT nLine, UINT nDiff)
{
	ASSERT(nDiff >= 0 && nDiff <= m_nDiffs);
	if (nLine >= m_diffs[nDiff].dbegin0 &&
			nLine <= m_diffs[nDiff].dend0)
		return TRUE;
	else
		return FALSE;
}

// Returns order num of diff for given line
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

// Get text of specified lines
// (ghost lines will not contribute text)
// CrystalTextBuffer::GetText() returns text including ghost lines
UINT CMergeDoc::CDiffTextBuffer::GetTextWithoutEmptys(int nStartLine, int nStartChar, 
								 int nEndLine, int nEndChar, 
								 CString &text, BOOL bLeft, int nCrlfStyle /* CRLF_STYLE_AUTOMATIC */)
{
	int lines = m_aLines.GetSize();
	ASSERT(nStartLine >= 0 && nStartLine < lines);
	ASSERT(nStartChar >= 0 && nStartChar <= GetLineLength(nStartLine));
	ASSERT(nEndLine >= 0 && nEndLine < lines);
	ASSERT(nEndChar >= 0 && nEndChar <= GetFullLineLength(nEndLine));
	ASSERT(nStartLine < nEndLine || nStartLine == nEndLine && 
		nStartChar < nEndChar);

	// estimate size (upper bound)
	int nBufSize = 0;
	for (int i=nStartLine; i<=nEndLine; ++i)
		nBufSize += (GetFullLineLength(i) + 2); // in case we insert EOLs
	LPTSTR pszBuf = text.GetBuffer(nBufSize);

	for (i=nStartLine; i<=nEndLine; ++i)
	{
		int soffset = (i==nStartLine ? nStartChar : 0);
		int eoffset = (i==nEndLine ? nEndChar : GetFullLineLength(i));
		int chars = eoffset - soffset;
		// (Exclude ghost lines, also exclude last line if at position 0)
		if (chars>0)
		{
			LPCTSTR szLine = m_aLines[i].m_pcLine + soffset;
			CopyMemory(pszBuf, szLine, chars * sizeof(TCHAR));
			pszBuf += chars;
			if (i!=GetLineCount()-1 && GetLineLength(i)==GetFullLineLength(i))
			{
				// Oops, line lacks EOL
				// (If this happens, editor probably has bug)
				CString sEol = crlfs[nCrlfStyle];
				CopyMemory(pszBuf, sEol, sEol.GetLength());
				pszBuf += sEol.GetLength();
			}
		}
	}
	pszBuf[0] = 0;
	text.ReleaseBuffer();
	text.FreeExtra();
	return text.GetLength();
}


// Try to determine current CRLF mode based on first line
int CMergeDoc::CDiffTextBuffer::DetermineCRLFStyle(LPVOID lpBuf, DWORD dwLength)
{
	WORD wLoopSize = 0xffff;
	int nCrlfStyle = CRLF_STYLE_DOS;	// Default
	TCHAR * lpBuffer = (TCHAR *)lpBuf;

	if (dwLength < 0xffff)
		wLoopSize = dwLength;

	// Find first linebreak
	for (DWORD i = 0; i < wLoopSize; i++, lpBuffer++)
	{
		if ((*lpBuffer == _T('\r')) || (*lpBuffer == _T('\n')))
			break;
	}

	// By default (or in the case of empty file) use DOS style
	if (i < wLoopSize)
	{
		//  Otherwise, analyse the first occurance of line-feed character
		if (*lpBuffer == _T('\n'))
			nCrlfStyle = CRLF_STYLE_UNIX;
		else
		{
			// Found '\r', CRLF is DOS ('\r\n') or MAC ('\r')
			if (i < wLoopSize - 1 && *(lpBuffer+1) == _T('\n'))
				nCrlfStyle = CRLF_STYLE_DOS;
			else
				nCrlfStyle = CRLF_STYLE_MAC;
		}
	}
	return nCrlfStyle;
}

// Reads one line from filebuffer and inserts to textbuffer
void CMergeDoc::CDiffTextBuffer::ReadLineFromBuffer(TCHAR *lpLineBegin, DWORD dwLineLen /* =0 */)
{
	if (m_nSourceEncoding >= 0)
		iconvert (lpLineBegin, m_nSourceEncoding, 1, m_nSourceEncoding == 15);
	InsertLine(lpLineBegin, dwLineLen);
}

void CMergeDoc::CDiffTextBuffer::SetTempPath(CString path)
{
	m_strTempPath = path;
}

BOOL CMergeDoc::CDiffTextBuffer::LoadFromFile(LPCTSTR pszFileName,
		int nCrlfStyle /*= CRLF_STYLE_AUTOMATIC*/)
{
	ASSERT(!m_bInit);
	ASSERT(m_aLines.GetSize() == 0);
	MAPPEDFILEDATA fileData = {0};
	CString sExt;
	BOOL bSuccess = TRUE;

	// Set encoding based on extension, if we know one
	SplitFilename(pszFileName, NULL, NULL, &sExt);
	CCrystalTextView::TextDefinition *def = 
			CCrystalTextView::GetTextType(sExt);
	if (def && def->encoding != -1)
		m_nSourceEncoding = def->encoding;
	
	// Init filedata struct and open file as memory mapped 
	_tcsncpy(fileData.fileName, pszFileName, sizeof(fileData.fileName));
	fileData.bWritable = FALSE;
	fileData.dwOpenFlags = OPEN_EXISTING;
	bSuccess = files_openFileMapped(&fileData);

	if (bSuccess)
	{
		//Try to determine current CRLF mode based on first line
		if (nCrlfStyle == CRLF_STYLE_AUTOMATIC)
			nCrlfStyle = DetermineCRLFStyle(fileData.pMapBase, fileData.dwSize);

		ASSERT(nCrlfStyle >= 0 && nCrlfStyle <= 2);
		SetCRLFMode(nCrlfStyle);
		
		m_aLines.SetSize(0, 4096);
		
		DWORD dwBytesRead = 0;
		TCHAR *lpChar = (TCHAR *)fileData.pMapBase;
		TCHAR *lpLineBegin = lpChar;
		int eolChars = 0;
		while (dwBytesRead < fileData.dwSize)
		{
			TCHAR c = *lpChar;
			
			if (c == '\n' || c == '\r')
			{
				// Might be an EOL, but check for leading \r of \r\n
				if (dwBytesRead+1 < fileData.dwSize && lpChar[0]=='\r' && lpChar[1]=='\n')
				{
					// This is a \r followed by a \n, so treat it as regular char
				}
				else
				{
					lpChar++;
					dwBytesRead++;

					// This is an EOL
					ReadLineFromBuffer(lpLineBegin, lpChar - lpLineBegin);
					lpLineBegin = lpChar;
					continue;
				}
			}
			lpChar++;
			dwBytesRead++;
		}
		// Handle case where file ended with line without EOL
		if (lpChar > lpLineBegin)
			// no ghost line, because that would append an EOL to the last line
			ReadLineFromBuffer(lpLineBegin, lpChar - lpLineBegin);
		else
			// Last line had EOL, so append succeeding ghost line
			InsertLine(_T(""), 0);
		FinishLoading();
		ASSERT(m_aLines.GetSize() > 0);   //  At least one empty line must present
		
		m_bInit = TRUE;
		m_bModified = FALSE;
		m_bUndoGroup = m_bUndoBeginGroup = FALSE;
		m_nUndoBufSize = 1024; // crystaltextbuffer.cpp - UNDO_BUF_SIZE;
		m_nSyncPosition = m_nUndoPosition = 0;
		ASSERT(m_aUndoBuf.GetSize() == 0);
		
		// This is needed: Syntax hilighting does not work when
		// automatically scroll to first diff is enabled
		RetypeViews(pszFileName);

		UpdateViews(NULL, NULL, UPDATE_RESET);
		m_ptLastChange.x = m_ptLastChange.y = -1;
		bSuccess = TRUE;
	}
	
	files_closeFileMapped(&fileData, 0xFFFFFFFF, FALSE);
	return bSuccess;
}

BOOL CMergeDoc::CDiffTextBuffer::SaveToFile (LPCTSTR pszFileName,
		BOOL bTempFile, int nCrlfStyle /*= CRLF_STYLE_AUTOMATIC*/ ,
		BOOL bClearModifiedFlag /*= TRUE*/ )
{
	ASSERT (nCrlfStyle == CRLF_STYLE_AUTOMATIC || nCrlfStyle == CRLF_STYLE_DOS ||
		nCrlfStyle == CRLF_STYLE_UNIX || nCrlfStyle == CRLF_STYLE_MAC);
	ASSERT (m_bInit);
	MAPPEDFILEDATA fileData = {0};
	TCHAR szTempFileDir[_MAX_PATH] = {0};
	TCHAR szTempFileName[_MAX_PATH] = {0} ;
	BOOL bSuccess = FALSE;
	TCHAR drive[_MAX_PATH] = {0};
	TCHAR dir[_MAX_PATH] = {0};

	if (nCrlfStyle == CRLF_STYLE_AUTOMATIC)
		nCrlfStyle = GetCRLFMode();
	
	// HACK for MAC files support.  Temp files for diff must
	// be in unix format
	if (bTempFile && nCrlfStyle == CRLF_STYLE_MAC)
		nCrlfStyle = CRLF_STYLE_UNIX;

	ASSERT(nCrlfStyle >= 0 && nCrlfStyle <= 2);
	LPCTSTR pszCRLF = crlfs[nCrlfStyle];
	int nCRLFLength = _tcslen(pszCRLF);
	int nLineCount = m_aLines.GetSize();
	CString text;			
	int nLastLength = GetFullLineLength(nLineCount-1);
		
	UINT nBufSize = GetTextWithoutEmptys(0, 0, nLineCount - 1,
		nLastLength, text, m_bIsLeft, nCrlfStyle);
	
	if (!pszFileName)
		return FALSE;	// No filename, cannot save...

	if (!::GetTempFileName(m_strTempPath, _T("MRG"), 0, szTempFileName))
		return FALSE;  //Nothing to do if even tempfile name fails

	// Init filedata struct and open file as memory mapped 
	_tcsncpy(fileData.fileName, szTempFileName, sizeof(fileData.fileName));
	fileData.bWritable = TRUE;
	fileData.dwOpenFlags = CREATE_ALWAYS;
	fileData.dwSize = nBufSize;
	bSuccess = files_openFileMapped(&fileData);

	if (bSuccess)
	{
		if (m_nSourceEncoding >= 0)
		{
			LPTSTR pszBuf;
			iconvert_new((LPCTSTR)text, &pszBuf, 1,
					m_nSourceEncoding, m_nSourceEncoding == 15);
			CopyMemory(fileData.pMapBase, pszBuf, nBufSize);
			free(pszBuf);
		}
		else
			CopyMemory(fileData.pMapBase, (void *)(LPCTSTR)text, nBufSize);

		// Force flushing of file buffers for user files
		if (bTempFile)
			files_closeFileMapped(&fileData, nBufSize, FALSE);
		else
			files_closeFileMapped(&fileData, nBufSize, TRUE);
		
		// Write tempfile over original file
		if (::CopyFile(szTempFileName, pszFileName, FALSE))
		{
			::DeleteFile(szTempFileName);
			if (bClearModifiedFlag)
			{
				SetModified(FALSE);
				m_nSyncPosition = m_nUndoPosition;
			}
			bSuccess = TRUE;
		}
	}
	return bSuccess;
}

// Replace text of line (no change to eol)
void CMergeDoc::CDiffTextBuffer::ReplaceLine(int nLine, const CString &strText)
{
	if (GetLineLength(nLine)>0)
		DeleteText(NULL, nLine, 0, nLine, GetLineLength(nLine));
	int endl,endc;
	InsertText(NULL, nLine, 0, strText, endl,endc);
}
// return pointer to the eol chars of this string, or pointer to empty string if none
LPCTSTR getEol(const CString &str)
{
	if (str.GetLength()>1 && str[str.GetLength()-2]=='\r' && str[str.GetLength()-1]=='\n')
		return (LPCTSTR)str + str.GetLength()-2;
	if (str.GetLength()>0 && (str[str.GetLength()-1]=='\r' || str[str.GetLength()-1]=='\n'))
		return (LPCTSTR)str + str.GetLength()-1;
	return _T("");
}

// Replace line (removing any eol, and only including one if in strText)
void CMergeDoc::CDiffTextBuffer::ReplaceFullLine(int nLine, const CString &strText)
{
	if (GetLineEol(nLine) == getEol(strText))
	{
		// (optimization) eols are the same, so just replace text inside line
		ReplaceLine(nLine, strText);
		return;
	}
	if (GetFullLineLength(nLine))
		DeleteText(NULL, nLine, 0, nLine+1, 0); 
	int endl,endc;
	InsertText(NULL, nLine, 0, strText, endl,endc);
}

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

void TraceLastErrorMessage()
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);
	// Process any inserts in lpMsgBuf.
	// ...
	// Display the string.
	//MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
	TRACE((LPCTSTR)lpMsgBuf);
	// Free the buffer.
	LocalFree( lpMsgBuf );
 
}

void CMergeDoc::CleanupTempFiles()
{
	if (!m_strTempLeftFile.IsEmpty())
	{
		if (::DeleteFile(m_strTempLeftFile))
			m_strTempLeftFile = _T("");
		else
			TraceLastErrorMessage();
	}
	if (!m_strTempRightFile.IsEmpty())
	{
		if (::DeleteFile(m_strTempRightFile))
			m_strTempRightFile = _T("");
		else
			TraceLastErrorMessage();
	}
}

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
	if(curView)
	{
		curView->PushCursor();
		nRescanResult = Rescan(bForced);
		UpdateAllViews(NULL);
		curView->PopCursor();
		// Show possible error after updating screen
		if (nRescanResult != RESCAN_OK &&
				nRescanResult != RESCAN_SUPPRESSED)
			ShowRescanError(nRescanResult);
	}
	else
	{
		nRescanResult = Rescan(bForced);
		UpdateAllViews(NULL);
		// Show possible error after updating screen
		if (nRescanResult != RESCAN_OK &&
				nRescanResult != RESCAN_SUPPRESSED)
			ShowRescanError(nRescanResult);
	}
}

void CMergeDoc::OnFileSave() 
{
	BOOL bLSaveSuccess = FALSE;
	BOOL bRSaveSuccess = FALSE;
	BOOL bLModified = FALSE;
	BOOL bRModified = FALSE;

	if (m_ltBuf.IsModified())
	{
		bLModified = TRUE;
		DoSave(m_strLeftFile, bLSaveSuccess, TRUE );
	}

	if (m_rtBuf.IsModified())
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
			m_pDirDoc->UpdateItemTimes(m_strLeftFile,
				m_strRightFile);
			if (m_nDiffs == 0)
				m_pDirDoc->UpdateItemStatus(m_strLeftFile,
					m_strRightFile, FILE_SAME);
		}
	}
}

void CMergeDoc::OnUpdateStatusNum(CCmdUI* pCmdUI) 
{
	CString sIdx,sCnt,s;
	if (GetCurrentDiff() < 0 &&  m_nDiffs <= 0)
		s = _T("");
	else if (GetCurrentDiff() < 0)
	{
		sCnt.Format(_T("%ld"), m_nDiffs);
		AfxFormatString1(s, IDS_NO_DIFF_SEL_FMT, sCnt); 
	}
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
	CStdioFile file;
	UINT LeftExtras=0;   // extra lines added to view
	UINT RightExtras=0;   // extra lines added to view

	// walk the diff stack and flag the line codes
	SetCurrentDiff(-1);
	for (int nDiff=0; nDiff < static_cast<int>(m_nDiffs); ++nDiff)
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
					m_ltBuf.SetLineFlag(i, LF_LEFT_ONLY, TRUE, FALSE, FALSE);
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
					m_rtBuf.SetLineFlag(i, LF_LEFT_ONLY, TRUE, FALSE, FALSE);
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
					m_rtBuf.SetLineFlag(i, LF_RIGHT_ONLY, TRUE, FALSE, FALSE);
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
					m_ltBuf.SetLineFlag(i, LF_RIGHT_ONLY, TRUE, FALSE, FALSE);
					++LeftExtras;
				}
			}
			break;
		case OP_DIFF:
			// left side
			{
				// just flag the lines
				curDiff.dbegin0 = curDiff.begin0+LeftExtras;
				curDiff.dend0 = curDiff.end0+LeftExtras;
				for (UINT i=curDiff.dbegin0; i <= curDiff.dend0; i++)
				{
					m_ltBuf.SetLineFlag(i, LF_DIFF, TRUE, FALSE, FALSE);
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
						m_ltBuf.SetLineFlag(idx, LF_RIGHT_ONLY, TRUE, FALSE, FALSE);
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
					m_rtBuf.SetLineFlag(i, LF_DIFF, TRUE, FALSE, FALSE);
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
						m_rtBuf.SetLineFlag(idx, LF_LEFT_ONLY, TRUE, FALSE, FALSE);
						curDiff.dend1++;
						++RightExtras;
					}
				}
			}
			break;
		}
	}

	m_ltBuf.SetReadOnly(FALSE);
	m_ltBuf.FinishLoading();
	m_rtBuf.SetReadOnly(FALSE);
	m_rtBuf.FinishLoading();

}

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
			m_pDirDoc->UpdateItemTimes(m_strLeftFile, m_strRightFile);
			if (m_nDiffs == 0)
				m_pDirDoc->UpdateItemStatus(m_strLeftFile,
					m_strRightFile, FILE_SAME);
		}
	}
	return result;
}

// View requests we rescan when convenient
void CMergeDoc::SetNeedRescan()
{
	m_bNeedIdleRescan = TRUE;
}

void CMergeDoc::RescanIfNeeded()
{
	// Rescan if we were asked for a rescan when convenient
	// AND if we've not rescanned in at least a second
	if (m_bNeedIdleRescan)
	{
		m_bNeedIdleRescan = FALSE;
		COleDateTimeSpan elapsed = COleDateTime::GetCurrentTime() - m_LastRescan;
		if (elapsed.GetTotalSeconds() > 1)
		{
			WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_RESCANNING));
			FlushAndRescan();
		}
	}
}

// We have two child views (left & right), so we keep pointers directly
// at them (the MFC view list doesn't have them both)
void CMergeDoc::SetMergeViews(CMergeEditView * pLeft, CMergeEditView * pRight)
{
	ASSERT(pLeft && !m_pLeftView);
	m_pLeftView = pLeft;
	ASSERT(pRight && !m_pRightView);
	m_pRightView = pRight;
}

// DirDoc gives us its identity just after it creates us
void CMergeDoc::SetDirDoc(CDirDoc * pDirDoc)
{
	ASSERT(pDirDoc && !m_pDirDoc);
	m_pDirDoc = pDirDoc;
}

CChildFrame * CMergeDoc::GetParentFrame() 
{
	return dynamic_cast<CChildFrame *>(m_pLeftView->GetParentFrame()); 
}

// DirDoc is closing
void CMergeDoc::DirDocClosing(CDirDoc * pDirDoc)
{
	ASSERT(m_pDirDoc == pDirDoc);
	m_pDirDoc = 0;
	// TODO (Perry 2003-03-30): perhaps merge doc should close now ?
}

// DirDoc commanding us to close
BOOL CMergeDoc::CloseNow()
{
	if (!SaveHelper())
		return FALSE;

	GetParentFrame()->CloseNow();
	return TRUE;
}
