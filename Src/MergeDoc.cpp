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
#include "getopt.h"
#include "fnmatch.h"
#include "coretools.h"
#include "VssPrompt.h"
#include "MergeEditView.h"
#include "cs2cs.h"
#include "childFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	m_pView=NULL;
	m_nCurDiff=-1;
	m_strTempLeftFile=_T("");
	m_strTempRightFile=_T("");
	curUndo = undoTgt.begin();
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
	mf->m_pMergeDoc = NULL;
  CleanupTempFiles();
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

BOOL CMergeDoc::Rescan()
{
	// store modified status
	BOOL ltMod = m_ltBuf.IsModified();
	BOOL rtMod = m_rtBuf.IsModified();

	// remove blank lines and clear winmerge flags
	int ct;
	for(ct=m_ltBuf.GetLineCount()-1; ct>=0; --ct)
	{
		if(m_ltBuf.GetLineFlags(ct) & LF_RIGHT_ONLY)
		{
			m_ltBuf.DeleteLine(ct);
		}
	}
	for(ct=m_rtBuf.GetLineCount()-1; ct>=0; --ct)
	{
		if(m_rtBuf.GetLineFlags(ct) & LF_LEFT_ONLY)
		{
			m_rtBuf.DeleteLine(ct);
		}
	}

	// restore modified status
	m_ltBuf.SetModified(ltMod);
	m_rtBuf.SetModified(rtMod);

	// output to temp file
	m_ltBuf.SaveToFile(m_strTempLeftFile, CRLF_STYLE_AUTOMATIC, FALSE);
	m_rtBuf.SaveToFile(m_strTempRightFile, CRLF_STYLE_AUTOMATIC, FALSE);

	// perform rescan
	struct file_data inf[2];
	char *free0=NULL,*free1=NULL;
	char dir0[MAX_PATH],dir1[MAX_PATH], name0[MAX_PATH], name1[MAX_PATH];
	int val,failed=0, depth=0;
	bool same_files=FALSE;
	struct change *e, *p;
	struct change *script=NULL;
	BOOL bResult=FALSE;
//	int nResumeTopLine=0;

	CWaitCursor wait;

	// get the desired files to temp locations so we can edit them dynamically
	if (!TempFilesExist())
	{
		if (!InitTempFiles(m_strLeftFile, m_strRightFile))
		{
			return FALSE;
		}
	}
	else
	{
		// find the top line to scroll back to
//		nResumeTopLine = mf->m_pLeft->GetScrollPos(SB_VERT)+1;
	}

	m_diffs.RemoveAll();
	m_nDiffs=0;
	m_nCurDiff=-1;
	
	split_filename(m_strTempLeftFile, dir0, name0, NULL);
	split_filename(m_strTempRightFile, dir1, name1, NULL); 
	memset(&inf[0], 0,sizeof(inf[0]));
	memset(&inf[1], 0,sizeof(inf[1]));
	
	/* Both exist and neither is a directory.  */
	int o_binary = always_text_flag ? 0:O_BINARY;
	
	/* Open the files and record their descriptors.  */
	inf[0].name = *dir0 == 0 ? name0 : (free0 = dir_file_pathname (dir0, name0));
	inf[0].desc = -2;
	inf[1].name = *dir1 == 0 ? name1 : (free1 = dir_file_pathname (dir1, name1));
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
				mf->m_pLeft->PrimeListWithFile();
				mf->m_pRight->PrimeListWithFile();

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
				bResult=TRUE;
			}
			else if (diff_flag)
			{
				CString s;
				VERIFY(s.LoadString(IDS_BIN_FILES_DIFF));
				AfxMessageBox(s, MB_ICONINFORMATION);
			}
			else
			{
				CString s;
				VERIFY(s.LoadString(IDS_FILESSAME));
				AfxMessageBox(s, MB_ICONINFORMATION);
			}
		}
	}
	
	if (free0)
		free (free0);
	if (free1)
		free (free1);

//	if (nResumeTopLine>0)
//	{
//		mf->m_pLeft->GoToLine(nResumeTopLine, FALSE);
//		mf->m_pRight->GoToLine(nResumeTopLine, FALSE);
//	}
	return bResult;
}


void CMergeDoc::AddDiffRange(UINT begin0, UINT end0, UINT begin1, UINT end1, BYTE op)
{
	TRY {
		DIFFRANGE dr;
		memset(&dr, 0, sizeof(dr));
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
		TCHAR msg[1024];
		e->GetErrorMessage(msg, 1024);
		AfxMessageBox(msg, MB_ICONSTOP);
	}
	END_CATCH_ALL;
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


void CMergeDoc::ListCopy(bool bSrcLeft)
{
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
			sbuf.GetLine(i, strLine);
			dbuf.ReplaceLine(i, strLine);
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
		UpdateAllViews(NULL);

		// reset the mod status of the source view because we do make some
		// changes, but none that concern the source text
		sbuf.SetModified(bSrcWasMod);
	}
	// what does this do?
//	pDestList->AddMod();
}



BOOL CMergeDoc::DoSave(LPCTSTR szPath, BOOL bLeft)
{
	CString strSavePath(szPath);

	if (!mf->m_strSaveAsPath.IsEmpty())
	{
		CFileStatus status;
		if (CFile::GetStatus(mf->m_strSaveAsPath, status)
			&& (status.m_attribute & CFile::Attribute::directory))
		{
			// third arg was a directory, so get append the filename
			TCHAR name[MAX_PATH];
			split_filename(szPath, NULL, name, NULL);
			strSavePath = mf->m_strSaveAsPath;
			if (mf->m_strSaveAsPath.Right(1) != _T('\\'))
				strSavePath += _T('\\');
			strSavePath += name;
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
		result = m_ltBuf.SaveToFile(strSavePath);
		if(result)
		{
			m_strLeftFile = strSavePath;
			CChildFrame *parent = dynamic_cast<CChildFrame*>(dynamic_cast<CMDIFrameWnd*>(AfxGetMainWnd())->MDIGetActive());
			if(parent)
			{
				parent->SetHeaderText(0, m_strLeftFile);
			}
		}
	}
	else
	{
		result = m_rtBuf.SaveToFile(strSavePath);
		if(result)
		{
			m_strRightFile = strSavePath;
			CChildFrame *parent = dynamic_cast<CChildFrame*>(dynamic_cast<CMDIFrameWnd*>(AfxGetMainWnd())->MDIGetActive());
			if(parent)
			{
				parent->SetHeaderText(1, m_strRightFile);
			}
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

int CMergeDoc::LineToDiff(UINT nLine)
{
	for (UINT i=0; i < m_nDiffs; i++)
	{
		if (nLine >= m_diffs[i].dbegin0
			&& nLine <= m_diffs[i].dend0)
		{
			return i;
		}
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
	if (!SaveHelper())
		return FALSE;
	
	mf->m_pLeft = mf->m_pRight = NULL;
	return TRUE;
}

void CMergeDoc::SetCurrentDiff(int nDiff)
{
	if (nDiff >= 0 && nDiff < (int)m_nDiffs)
		m_nCurDiff = nDiff;
	else
		m_nCurDiff = -1;
}

#define FLAGSET(f)   ((m_aLines[nLine].m_dwFlags&(f))==(f))

BOOL CMergeDoc::CDiffTextBuffer::SaveToFile (LPCTSTR pszFileName, 
											 int nCrlfStyle /*= CRLF_STYLE_AUTOMATIC*/ , 
											 BOOL bClearModifiedFlag /*= TRUE*/ )
{
  ASSERT (nCrlfStyle == CRLF_STYLE_AUTOMATIC || nCrlfStyle == CRLF_STYLE_DOS ||
          nCrlfStyle == CRLF_STYLE_UNIX || nCrlfStyle == CRLF_STYLE_MAC);
  ASSERT (m_bInit);
  HANDLE hTempFile = INVALID_HANDLE_VALUE;
  HANDLE hSearch = INVALID_HANDLE_VALUE;
  TCHAR szTempFileDir[_MAX_PATH + 1];
  TCHAR szTempFileName[_MAX_PATH + 1];
  TCHAR szBackupFileName[_MAX_PATH + 1];
  BOOL bSuccess = FALSE;
  __try
  {
    TCHAR drive[_MAX_PATH], dir[_MAX_PATH], name[_MAX_PATH], ext[_MAX_PATH];
#ifdef _UNICODE
    _wsplitpath (pszFileName, drive, dir, name, ext);
#else
    _splitpath (pszFileName, drive, dir, name, ext);
#endif
    _tcscpy (szTempFileDir, drive);
    _tcscat (szTempFileDir, dir);
    _tcscpy (szBackupFileName, pszFileName);
    _tcscat (szBackupFileName, _T (".bak"));

    if (::GetTempFileName (szTempFileDir, _T ("MRG"), 0, szTempFileName) == 0)
      __leave;

      hTempFile =::CreateFile (szTempFileName, GENERIC_WRITE, 0, NULL,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
      if (hTempFile == INVALID_HANDLE_VALUE)
        __leave;

        if (nCrlfStyle == CRLF_STYLE_AUTOMATIC)
          nCrlfStyle = m_nCRLFMode;

          ASSERT (nCrlfStyle >= 0 && nCrlfStyle <= 2);
          LPCTSTR pszCRLF = crlfs[nCrlfStyle];
          int nCRLFLength = _tcslen (pszCRLF);

          int nLineCount = m_aLines.GetSize ();
          for (int nLine = 0; nLine < nLineCount; nLine++)
            {
			  // skip blank diff lines
			  if ((!m_bIsLeft && FLAGSET(LF_LEFT_ONLY))
				  || (m_bIsLeft && FLAGSET(LF_RIGHT_ONLY)))
				continue;

              int nLength = m_aLines[nLine].m_nLength;
              DWORD dwWrittenBytes;
              if (nLength > 0)
                {
                  LPCTSTR pszLine = m_aLines[nLine].m_pcLine;
                  if (m_nSourceEncoding >= 0)
                    {
                      LPTSTR pszBuf;
                      iconvert_new (m_aLines[nLine].m_pcLine, &pszBuf, 1, m_nSourceEncoding, m_nSourceEncoding == 15);
                      if (!::WriteFile (hTempFile, pszBuf, nLength, &dwWrittenBytes, NULL))
                        {
                          free (pszBuf);
                          __leave;
                        }
                      free (pszBuf);
                    }
                  else
                    if (!::WriteFile (hTempFile, pszLine, nLength, &dwWrittenBytes, NULL))
                      __leave;
                  if (nLength != (int) dwWrittenBytes)
                    __leave;
                }
              if (nLine < nLineCount - 1)     //  Last line must not end with CRLF

                {
                  if (!::WriteFile (hTempFile, pszCRLF, nCRLFLength, &dwWrittenBytes, NULL))
                    __leave;
                  if (nCRLFLength != (int) dwWrittenBytes)
                    __leave;
                }
            }
    ::CloseHandle (hTempFile);
    hTempFile = INVALID_HANDLE_VALUE;

    if (m_bCreateBackupFile)
      {
        WIN32_FIND_DATA wfd;
        hSearch =::FindFirstFile (pszFileName, &wfd);
        if (hSearch != INVALID_HANDLE_VALUE)
          {
            //  File exist - create backup file
            ::DeleteFile (szBackupFileName);
            if (!::MoveFile (pszFileName, szBackupFileName))
              __leave;
            ::FindClose (hSearch);
            hSearch = INVALID_HANDLE_VALUE;
          }
      }
    else
      {
        ::DeleteFile (pszFileName);
      }

    //  Move temporary file to target name
    if (!::MoveFile (szTempFileName, pszFileName))
      __leave;

      if (bClearModifiedFlag)
        {
          SetModified (FALSE);
          m_nSyncPosition = m_nUndoPosition;
        }
    bSuccess = TRUE;
  }
  __finally
  {
    if (hSearch != INVALID_HANDLE_VALUE)
      ::FindClose (hSearch);
      if (hTempFile != INVALID_HANDLE_VALUE)
        ::CloseHandle (hTempFile);
        ::DeleteFile (szTempFileName);
      }
      return bSuccess;
}

void CMergeDoc::CDiffTextBuffer::ReplaceLine(int nLine, const CString &strText)
{
	if (GetLineLength(nLine)>0)
		DeleteText(NULL, nLine, 0, nLine, GetLineLength(nLine));
	int endl,endc;
	InsertText(NULL, nLine, 0, strText, endl,endc);
}

//DEL void CMergeDoc::CDiffTextBuffer::DeleteLine(int nLine)
//DEL {
//DEL 	if (GetLineLength(nLine)>0)
//DEL 		DeleteText(m_bIsLeft? mf->m_pLeft:mf->m_pRight, nLine, 0, nLine, GetLineLength(nLine));
//DEL }

BOOL CMergeDoc::InitTempFiles(const CString& srcPathL, const CString& strPathR)
{
	CleanupTempFiles();

	CString strPath = GetModulePath(NULL);
	if (m_strTempLeftFile.IsEmpty())
	{
		TCHAR name[MAX_PATH];
		::GetTempFileName (strPath, _T ("_LT"), 0, name);
		m_strTempLeftFile = name;

		if (!::CopyFile(srcPathL, m_strTempLeftFile, FALSE))
			return FALSE;
		::SetFileAttributes(m_strTempLeftFile, FILE_ATTRIBUTE_NORMAL);
	}
	if (m_strTempRightFile.IsEmpty())
	{
		TCHAR name[MAX_PATH];
		::GetTempFileName (strPath, _T ("_RT"), 0, name);
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

void CMergeDoc::FlushAndRescan()
{
	CMDIFrameWnd* mainWnd = dynamic_cast<CMDIFrameWnd*>(AfxGetMainWnd());
	CMDIChildWnd* diffWnd = dynamic_cast<CMDIChildWnd*>(mainWnd->MDIGetActive());
	CCrystalEditView* curView = dynamic_cast<CCrystalEditView*>(diffWnd->GetActiveView());

	if(curView)
	{
		curView->PushCursor();
		Rescan();
		UpdateAllViews(NULL);
		curView->PopCursor();
	}
	else
	{
		Rescan();
		UpdateAllViews(NULL);
	}
}

void CMergeDoc::OnFileSave() 
{
	if (m_ltBuf.IsModified())
	{
		DoSave(m_strLeftFile, TRUE);
	}

	if (m_rtBuf.IsModified())
	{
		DoSave(m_strRightFile, FALSE);
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

	CString blankline(_T(""));
	int blanklen=blankline.GetLength();

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
					m_rtBuf.InsertLine(blankline, blanklen, i);
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
					m_ltBuf.InsertLine(blankline, blanklen, i);
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
						m_ltBuf.InsertLine(blankline, blanklen, idx);
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
						m_rtBuf.InsertLine(blankline, blanklen, idx);
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
	m_rtBuf.SetReadOnly(FALSE);

}

BOOL CMergeDoc::SaveHelper()
{
	BOOL result = TRUE;
	CString s;

	AfxFormatString1(s, IDS_SAVE_FMT, m_strLeftFile); 
	if (m_ltBuf.IsModified())
	{
		switch(AfxMessageBox(s, MB_YESNOCANCEL|MB_ICONQUESTION))
		{
		case IDYES:
			if (!DoSave(m_strLeftFile, TRUE))
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
		switch(AfxMessageBox(s, MB_YESNOCANCEL|MB_ICONQUESTION))
		{
		case IDYES:
			if (!DoSave(m_strRightFile, FALSE))
				result=FALSE;
			break;
		case IDNO:
			break;
		default:  // IDCANCEL
			result=FALSE;
			break;
		}
	}
	return result;
}

