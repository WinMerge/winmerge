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
#include "DiffView.h"

#include "diff.h"
#include "getopt.h"
#include "fnmatch.h"
#include "coretools.h"
#include "VssPrompt.h"

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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc construction/destruction

CMergeDoc::CMergeDoc()
{
	m_diffs.SetSize(64);
	m_nDiffs=0;
	m_pView=NULL;
}

CMergeDoc::~CMergeDoc()
{	
	CUndoItem *pitem;
	while (!m_undoList.IsEmpty())
	{
		pitem = (CUndoItem*)m_undoList.RemoveHead();
		delete pitem;
	}
	mf->m_pMergeDoc = NULL;
}

BOOL CMergeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CString s;
	VERIFY(s.LoadString(IDS_FILE_COMPARISON_TITLE));
	SetTitle(s);

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
	struct file_data inf[2];
	char *free0=NULL,*free1=NULL;
	char dir0[MAX_PATH],dir1[MAX_PATH], name0[MAX_PATH], name1[MAX_PATH];
	int val,failed=0, depth=0;
	bool same_files=FALSE;
	struct change *e, *p;
	struct change *script=NULL;
	BOOL bResult=FALSE;

	BeginWaitCursor();

	m_diffs.RemoveAll();
	m_nDiffs=0;
	
	split_filename(m_strLeftFile, dir0, name0, NULL);
	split_filename(m_strRightFile, dir1, name1, NULL); 
	memset(&inf[0], 0,sizeof(inf[0]));
	memset(&inf[1], 0,sizeof(inf[1]));
	
	/* Both exist and neither is a directory.  */
	int o_binary = always_text_flag ? O_BINARY : 0;
	
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
			
			script = diff_2_files (inf, depth, NULL);

			// throw the diff into a temp file
			CString path = GetModulePath(NULL) + _T("\\Diff.txt");
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

			// test to see if we have a binary & text file
			BOOL b1 = FALSE;
			BOOL b2 = FALSE;
			if (m_nDiffs==0)
			{
				b1 = FileIsBinary(inf[0].desc);
				b2 = FileIsBinary(inf[1].desc);
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
				mf->m_pLeft->PrimeListWithFile(m_strLeftFile);
				mf->m_pRight->PrimeListWithFile(m_strRightFile);

				int lcnt = mf->m_pLeft->m_pList->GetItemCount();
				int rcnt = mf->m_pRight->m_pList->GetItemCount();
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
				}
				bResult=TRUE;
			}
			else if ((b1 && !b2)
				|| (!b1 && b2))
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

	EndWaitCursor();
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


void CMergeDoc::AddUndoAction(UINT nBegin, UINT nEnd, UINT nDiff, int nBlanks, BOOL bInsert, CDiffView *pList)
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
				CString s = pitem->m_pList->m_pList->GetItemText(i, 0);
				pitem->list.AddTail(s);
			}

		m_undoList.AddHead(pitem);
	}
}

BOOL CMergeDoc::Undo()
{
	if (!m_undoList.IsEmpty())
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
	}
	return FALSE;
}


void CMergeDoc::ListCopy(CDiffView * pSrcList, CDiffView * pDestList)
{
	CString s;
	int sel;
	int begin,end=-1;
	begin = sel = pSrcList->m_pList->GetNextItem(-1,LVNI_SELECTED);
	if (sel==-1)
		return;
	while ((sel=pSrcList->m_pList->GetNextItem(sel,LVNI_SELECTED)) != -1)
	{
		end=sel;
	}
	if (end==-1)
		end=begin;
		
	int diff = LineToDiff(begin);
	if (pDestList->m_bIsLeft)
	{
		AddUndoAction(begin, end, diff, m_diffs[diff].blank0, TRUE, pDestList);
		m_diffs[diff].blank0 = m_diffs[diff].blank1;
	}
	else 
	{
		AddUndoAction(begin, end, diff, m_diffs[diff].blank1, TRUE, pDestList);
		m_diffs[diff].blank1 = m_diffs[diff].blank0;
	}
	sel=-1;
	while ((sel=pSrcList->m_pList->GetNextItem(sel,LVNI_SELECTED)) != -1)
	{
		s = pSrcList->m_pList->GetItemText(sel,0);
		pDestList->m_pList->SetItemText(sel,0,s);
	}
	pDestList->AddMod();
	pDestList->UpdateWindow();
}



BOOL CMergeDoc::DoSave(LPCTSTR szPath, CListCtrl * pList, BOOL bLeft)
{
	HANDLE hf;
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

	if ((hf=FOPEN(strSavePath, GENERIC_WRITE, CREATE_ALWAYS)) != INVALID_HANDLE_VALUE)
	{
		CString s;
		int idx,cnt = pList->GetItemCount();
		int blank0,blank1;
		for (int i=0; i < cnt; i++)
		{
			idx=LineToDiff(i);
			s = pList->GetItemText(i,0);
			if (idx!=-1)
			{
				blank0 = m_diffs[idx].blank0;
				blank1 = m_diffs[idx].blank1;
				if ((bLeft && (blank0 < 0 || i < blank0))
				|| (!bLeft && (blank1 < 0 || i < blank1)))
				{
					FPRINTF(hf, "%s\r\n", s);
					//FPRINTF(hf, "%s\r\n", Tabify(s));
				}
			}
			else
				FPRINTF(hf, "%s\r\n", s);
			//FPRINTF(hf, "%s\r\n", Tabify(s));
		}
		CloseHandle(hf);
		return TRUE;
	}
	return FALSE;
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

BOOL CMergeDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
	if (mf->m_pLeft)
		if (!mf->m_pLeft->SaveHelper())
			return FALSE;

	
	BOOL result = CDocument::CanCloseFrame(pFrame);
	if (result)
		mf->m_pLeft = mf->m_pRight = NULL;
	return result;
}
