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
#include "UniFile.h"
#include "locality.h"
#include "OptionsDef.h"
#include "DiffFileInfo.h"
#include "SaveClosingDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CLogFile gLog;

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
	ON_UPDATE_COMMAND_UI(ID_STATUS_DIFFNUM, OnUpdateStatusNum)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc construction/destruction


#pragma warning(disable:4355)

/**
 * @brief Constructor.
 */
CMergeDoc::CMergeDoc() : m_ltBuf(this,TRUE), m_rtBuf(this,FALSE)
{
	DIFFOPTIONS options = {0};

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
	m_nLeftBufferType = BUFFER_NORMAL;
	m_nRightBufferType = BUFFER_NORMAL;
	m_bMergingMode = mf->m_options.GetInt(OPT_MERGE_MODE);
	m_bLeftEditAfterRescan = FALSE;
	m_bRightEditAfterRescan = FALSE;

	m_diffWrapper.SetDetectMovedBlocks(mf->m_options.GetInt(OPT_CMP_MOVED_BLOCKS));
	options.nIgnoreWhitespace = mf->m_options.GetInt(OPT_CMP_IGNORE_WHITESPACE);
	options.bIgnoreBlankLines = mf->m_options.GetInt(OPT_CMP_IGNORE_BLANKLINES);
	options.bIgnoreCase = mf->m_options.GetInt(OPT_CMP_IGNORE_CASE);
	options.bEolSensitive = mf->m_options.GetInt(OPT_CMP_EOL_SENSITIVE);

	m_diffWrapper.SetOptions(&options);
	m_diffWrapper.SetPrediffer(NULL);
}

#pragma warning(default:4355)

/**
 * @brief Destructor.
 *
 * Informs associated dirdoc that mergedoc is closing.
 */
CMergeDoc::~CMergeDoc()
{	
	if (m_pDirDoc)
	{
		m_pDirDoc->MergeDocClosing(this);
		m_pDirDoc = NULL;
	}

	delete m_pInfoUnpacker;
}

/**
 * @brief Deleted data associated with doc before closing.
 */
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

/**
 * @brief Called when new document is created.
 *
 * Initialises buffers.
 */
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

void CMergeDoc::SetPrediffer(PrediffingInfo * infoPrediffer)
{
	m_diffWrapper.SetPrediffer(infoPrediffer);
}
void CMergeDoc::GetPrediffer(PrediffingInfo * infoPrediffer)
{
	m_diffWrapper.GetPrediffer(infoPrediffer);
}

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc serialization

void CMergeDoc::Serialize(CArchive& ar)
{
	ASSERT(0); // we do not use CDocument serialization
}

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc commands

/**
 * @brief Save an editor text buffer to a file for prediffing (make UCS-2LE if appropriate)
 *
 * @note 
 * original file is Ansi : 
 *   buffer  -> save as Ansi -> Ansi plugins -> diffutils
 * original file is Unicode (UCS2-LE, UCS2-BE, UTF-8) :
 *   buffer  -> save as UCS2-LE -> Unicode plugins -> convert to UTF-8 -> diffutils
 * (the plugins are optional, not the conversion)
 * @todo Show SaveToFile() errors?
 */
static void SaveBuffForDiff(CMergeDoc::CDiffTextBuffer & buf, const CString & filepath)
{
	ASSERT(buf.m_nSourceEncoding == buf.m_nDefaultEncoding);  
	int orig_codepage = buf.getCodepage();
	int orig_unicoding = buf.getUnicoding();	

	// If file was in Unicode
	if (orig_unicoding!=ucr::NONE)
	{
	// we subvert the buffer's memory of the original file encoding
		buf.setUnicoding(ucr::UCS2LE);  // write as UCS-2LE (for preprocessing)
		buf.setCodepage(0); // should not matter
	}

	// and we don't repack the file
	PackingInfo * tempPacker = NULL;

	// write buffer out to temporary file
	CString sError;
	int retVal = buf.SaveToFile(filepath, TRUE, sError, tempPacker,
		CRLF_STYLE_AUTOMATIC, FALSE);

	// restore memory of encoding of original file
	buf.setUnicoding(orig_unicoding);
	buf.setCodepage(orig_codepage);
}

/**
 * @brief Save files to temp files & compare again.
 *
 * @param bBinary [in,out] [in] If TRUE, compare two binary files
 * [out] If TRUE binary file was detected.
 * @param bIdentical [out] If TRUE files were identical
 * @param bForced [in] If TRUE, suppressing is ignored and rescan
 * is done always
 * @return Tells if rescan was successfully, was suppressed, or
 * error happened
 * If this code is OK, Rescan has detached the views temporarily
 * (positions of cursors have been lost)
 * @note Rescan() ALWAYS compares temp files. Actual user files are not
 * touched by Rescan().
 * @sa CDiffWrapper::RunFileDiff()
 */
int CMergeDoc::Rescan(BOOL &bBinary, BOOL &bIdentical,
		BOOL bForced /* =FALSE */)
{
	DIFFOPTIONS diffOptions = {0};
	DIFFSTATUS status = {0};
	DiffFileInfo fileInfo;
	BOOL diffSuccess;
	int nResult = RESCAN_OK;
	BOOL bLeftFileChanged = FALSE;
	BOOL bRightFileChanged = FALSE;

	if (!bForced)
	{
		if (!m_bEnableRescan)
			return RESCAN_SUPPRESSED;
	}

	bLeftFileChanged = IsFileChangedOnDisk(m_strLeftFile, fileInfo,
		FALSE, TRUE);
	bRightFileChanged = IsFileChangedOnDisk(m_strRightFile, fileInfo,
		FALSE, FALSE);
	m_LastRescan = COleDateTime::GetCurrentTime();

	// get the desired files to temp locations so we can edit them dynamically
	if (!TempFilesExist())
	{
		if (!InitTempFiles(m_strLeftFile, m_strRightFile))
			return RESCAN_TEMP_ERR;
	}

	// output buffers to temp files (in UTF-8 if TCHAR=wchar_t or buffer was Unicode)
	if (bBinary == FALSE)
	{
		SaveBuffForDiff(m_ltBuf, m_strTempLeftFile);
		SaveBuffForDiff(m_rtBuf, m_strTempRightFile);
	}

	// Set up DiffWrapper
	m_diffWrapper.SetCompareFiles(m_strTempLeftFile, m_strTempRightFile);
	m_diffWrapper.SetDiffList(&m_diffs);
	m_diffWrapper.SetUseDiffList(TRUE);		// Add diffs to list
	m_diffWrapper.GetOptions(&diffOptions);
	
	// Clear diff list
	m_diffs.RemoveAll();
	m_nDiffs = 0;
	m_nCurDiff = -1;
	// Clear moved lines lists
	m_diffWrapper.ClearMovedLists();

	// Run diff
	diffSuccess = m_diffWrapper.RunFileDiff();

	// Read diff-status
	m_diffWrapper.GetDiffStatus(&status);
	if (bBinary) // believe caller if we were told these are binaries
		status.bBinaries = TRUE;
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
	{
		bBinary = TRUE;
		if (status.bBinariesIdentical)
			bIdentical = TRUE;
	}
	else
	{
		// Now update views and buffers for ghost lines

		// Prevent displaying views during this update 
		// BTW, this solves the problem of double asserts
		// (during the display of an assert message box, a second assert in one of the 
		//  display functions happens, and hides the first assert)
		m_pLeftView->DetachFromBuffer();
		m_pRightView->DetachFromBuffer();
		m_pLeftDetailView->DetachFromBuffer();
		m_pRightDetailView->DetachFromBuffer();

		// Remove blank lines and clear winmerge flags
		// this operation does not change the modified flag
		m_ltBuf.prepareForRescan();
		m_rtBuf.prepareForRescan();

		// Analyse diff-list (updating real line-numbers)
		// this operation does not change the modified flag
		PrimeTextBuffers();

		// Apply flags to lines that moved, to differentiate from appeared/disappeared lines
		FlagMovedLines(m_diffWrapper.GetMoved0(), &m_ltBuf);
		FlagMovedLines(m_diffWrapper.GetMoved1(), &m_rtBuf);
		
		// After PrimeTextBuffers() we know amount of real diffs
		// (m_nDiffs) and trivial diffs (m_nTrivialDiffs)

		// Identical files are also updated
		if (m_nDiffs == 0)
			bIdentical = TRUE;

		// just apply some options to the views
		m_pLeftView->PrimeListWithFile();
		m_pRightView->PrimeListWithFile();
		m_pLeftDetailView->PrimeListWithFile();
		m_pRightDetailView->PrimeListWithFile();

		// Now buffers data are valid
		m_pLeftView->ReAttachToBuffer();
		m_pRightView->ReAttachToBuffer();
		m_pLeftDetailView->ReAttachToBuffer();
		m_pRightDetailView->ReAttachToBuffer();

		m_bLeftEditAfterRescan = FALSE;
		m_bRightEditAfterRescan = FALSE;
	}

	m_leftRescanFileInfo.Update(m_strLeftFile);
	m_rightRescanFileInfo.Update(m_strRightFile);

	if (bLeftFileChanged)
	{
		CString msg;
		AfxFormatString1(msg, IDS_FILECHANGED_RESCAN, m_strLeftFile);
		AfxMessageBox(msg, MB_OK | MB_ICONWARNING);
	}
	else if (bRightFileChanged)
	{
		CString msg;
		AfxFormatString1(msg, IDS_FILECHANGED_RESCAN, m_strRightFile);
		AfxMessageBox(msg, MB_OK | MB_ICONWARNING);
	}
	return nResult;
}

/** @brief Adjust all different lines that were detected as actually matching moved lines */
void CMergeDoc::FlagMovedLines(const CMap<int, int, int, int> * movedLines, CDiffTextBuffer * pBuffer)
{
	int i;
	for (i=0; i<pBuffer->GetLineCount(); ++i)
	{
		int j=-1;
		if (movedLines->Lookup(i, j))
		{
			TRACE(_T("%d->%d\n"), i, j);
			ASSERT(j>=0);
			// We only flag lines that are already marked as being different
			int apparent = pBuffer->ComputeApparentLine(i);
			if (pBuffer->FlagIsSet(apparent, LF_DIFF))
			{
				pBuffer->SetLineFlag(apparent, LF_MOVED, TRUE, FALSE, FALSE);
			}
		}
	}
	// TODO: Need to record actual moved information
}

/**
 * @brief Prints (error) message by rescan status.
 *
 * @param nRescanResult [in] Resultcocode from rescan().
 * @param bBinary [in] Were files binaries?.
 * @param bIdentical [in] Were files identical?.
 * @sa CMergeDoc::Rescan()
 */
void CMergeDoc::ShowRescanError(int nRescanResult,
	BOOL bBinary, BOOL bIdentical)
{
	CString s;

	// Rescan was suppressed, there is no sensible status
	if (nRescanResult == RESCAN_SUPPRESSED)
		return;

	if (nRescanResult == RESCAN_FILE_ERR)
	{
		VERIFY(s.LoadString(IDS_FILEERROR));
		LogErrorString(s);
		AfxMessageBox(s, MB_ICONSTOP);
		return;
	}

	if (nRescanResult == RESCAN_TEMP_ERR)
	{
		VERIFY(s.LoadString(IDS_TEMP_FILEERROR));
		LogErrorString(s);
		AfxMessageBox(s, MB_ICONSTOP);
		return;
	}

	// Binary files tried to load, this can happen when giving filenames
	// from commandline
	if (bBinary)
	{
		VERIFY(s.LoadString(IDS_FILEBINARY));
		AfxMessageBox(s, MB_ICONINFORMATION);
		return;
	}

	// Files are not binaries, but they are identical
	if (bIdentical)
	{
		if (m_strLeftFile == m_strRightFile)
		{
			// compare file to itself, a custom message so user may hide the message in this case only
			VERIFY(s.LoadString(IDS_FILE_TO_ITSELF));
			AfxMessageBox(s, MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN, IDS_FILE_TO_ITSELF);
		}
		else
		{
			VERIFY(s.LoadString(IDS_FILESSAME));
			AfxMessageBox(s, MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN, IDS_FILESSAME);
		}
	}
}

CMergeDoc::CDiffTextBuffer::CDiffTextBuffer(CMergeDoc * pDoc, BOOL bLeft)
{
	m_pOwnerDoc = pDoc;
	m_bIsLeft=bLeft;
	unpackerSubcode = 0;
	m_unicoding = 0;
	m_codepage = 0;
}

BOOL CMergeDoc::CDiffTextBuffer::GetLine(int nLineIndex, CString &strLine)
{ 
	int nLineLength = CCrystalTextBuffer::GetLineLength 
		( nLineIndex ); 
	
	if( nLineLength < 0 ) 
		return FALSE; 
	else if( nLineLength == 0 ) 
		strLine.Empty(); 
	else 
	{ 
		_tcsncpy ( strLine.GetBuffer( nLineLength + 1 ), 
			CCrystalTextBuffer::GetLineChars( nLineIndex ), 
			nLineLength ); 
		strLine.ReleaseBuffer( nLineLength ); 
	} 
	return TRUE; 
}

void CMergeDoc::CDiffTextBuffer::SetModified(BOOL bModified /*= TRUE*/)
{
	CCrystalTextBuffer::SetModified (bModified);
	m_pOwnerDoc->SetModifiedFlag (bModified);
}

BOOL CMergeDoc::CDiffTextBuffer::GetFullLine(int nLineIndex, CString &strLine)
{
	if (!GetFullLineLength(nLineIndex))
		return FALSE;
	strLine = GetLineChars(nLineIndex);
	return TRUE;
}

void CMergeDoc::CDiffTextBuffer::AddUndoRecord(BOOL bInsert, const CPoint & ptStartPos, const CPoint & ptEndPos, LPCTSTR pszText, int nLinesToValidate, int nActionType /*= CE_ACTION_UNKNOWN*/)
{
	CGhostTextBuffer::AddUndoRecord(bInsert, ptStartPos, ptEndPos, pszText, nLinesToValidate, nActionType);
	if (m_aUndoBuf[m_nUndoPosition - 1].m_dwFlags & UNDO_BEGINGROUP)
	{
		m_pOwnerDoc->undoTgt.erase(m_pOwnerDoc->curUndo, m_pOwnerDoc->undoTgt.end());
		m_pOwnerDoc->undoTgt.push_back(m_bIsLeft ? m_pOwnerDoc->m_pLeftView : m_pOwnerDoc->m_pRightView);
		m_pOwnerDoc->curUndo = m_pOwnerDoc->undoTgt.end();
	}
}

BOOL CMergeDoc::Undo()
{
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
	lastDiff = min((signed int)m_nDiffs - 1, lastDiff);
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
 * @brief Save file with new filename.
 *
 * This function is called by CMergeDoc::DoSave() or CMergeDoc::DoSAveAs()
 * to save file with new filename. CMergeDoc::DoSave() calls if saving with
 * normal filename fails, to let user choose another filename/location.
 * Also, if file is unnamed file (e.g. scratchpad) then it must be saved
 * using this function.
 * @param [in, out] strPath 
 * - [in] : Initial path shown to user
 * - [out] : Path to new filename if saving succeeds
 * @param [in, out] nSaveResult 
 * - [in] : Statuscode telling why we ended up here. Maybe the result of
 * previous save.
 * - [out] : Statuscode of this saving try
 * @param [in, out] sError Error string from lower level saving code
 * @return FALSE as long as the user is not satisfied. Calling function
 * should not continue until TRUE is returned.
 * @sa CMergeDoc::DoSave()
 * @sa CMergeDoc::DoSaveAs()
 * @sa CMergeDoc::CDiffTextBuffer::SaveToFile()
 */
BOOL CMergeDoc::TrySaveAs(CString &strPath, int &nSaveResult, CString & sError,
	BOOL bLeft, PackingInfo * pInfoTempUnpacker)
{
	CString s;
	CString strSavePath; // New path for next saving try
	CString title;
	BOOL result = TRUE;
	int answer = IDYES;

	// We shouldn't get here if saving is succeed before
	ASSERT(nSaveResult != SAVE_DONE);

	// Select message based on reason function called
	if (nSaveResult == SAVE_PACK_FAILED)
	{
		AfxFormatString2(s, bLeft ? IDS_FILEPACK_FAILED_LEFT :
			IDS_FILEPACK_FAILED_RIGHT, strPath, pInfoTempUnpacker->pluginName);
		// replace the unpacker with a "do nothing" unpacker
		pInfoTempUnpacker->Initialize(PLUGIN_MANUAL);
	}
	else
	{
		AfxFormatString2(s, IDS_FILESAVE_FAILED, strPath, sError);
	}

	// SAVE_NO_FILENAME is temporarily used for scratchpad.
	// So don't ask about saving in that case.
	if (nSaveResult != SAVE_NO_FILENAME)
		answer = AfxMessageBox(s, MB_YESNO | MB_ICONWARNING);

	switch (answer)
	{
	case IDYES:
		if (bLeft)
			VERIFY(title.LoadString(IDS_SAVE_LEFT_AS));
		else
			VERIFY(title.LoadString(IDS_SAVE_RIGHT_AS));

		if (SelectFile(s, strPath, title, NULL, FALSE))
		{
			CDiffTextBuffer *pBuffer = bLeft ? &m_ltBuf : &m_rtBuf;
			strSavePath = s;
			nSaveResult = pBuffer->SaveToFile(strSavePath, FALSE, sError,
				pInfoTempUnpacker);

			if (nSaveResult == SAVE_DONE)
			{
				// We are saving scratchpad (unnamed file)
				if (strPath.IsEmpty())
				{
					if (bLeft)
					{
						m_nLeftBufferType = BUFFER_UNNAMED_SAVED;
						m_strLeftDesc.Empty();
					}
					else
					{
						m_nRightBufferType = BUFFER_UNNAMED_SAVED;
						m_strRightDesc.Empty();
					}
				}
					
				strPath = strSavePath;
				UpdateHeaderPath(bLeft);
			}
			else
				result = FALSE;
		}
		else
			nSaveResult = SAVE_CANCELLED;
		break;

	case IDNO:
		nSaveResult = SAVE_CANCELLED;
		break;
	}
	return result;
}

/**
 * @brief Save file creating backups etc.
 *
 * Safe top-level file saving function. Checks validity of given path.
 * Creates backup file if wanted to. And if saving to given path fails,
 * allows user to select new location/name for file.
 * @param [in] szPath Path where to save including filename. Can be
 * empty/NULL if new file is created (scratchpad) without filename.
 * @param [out] bSaveSuccess Will contain information about save success with
 * the original name (to determine if file statuses should be changed)
 * @param [in] bLeft If TRUE we are saving left(side) file, else right file
 * @return Tells if caller can continue (no errors happened)
 * @note Return value does not tell if SAVING succeeded. Caller must
 * Check value of bSaveSuccess parameter after calling this function!
 * @note If CMainFrame::m_strSaveAsPath is non-empty, file is saved
 * to directory it points to. If m_strSaveAsPath contains filename,
 * that filename is used.
 * @sa CMergeDoc::TrySaveAs()
 * @sa CMainFrame::CheckSavePath()
 * @sa CMergeDoc::CDiffTextBuffer::SaveToFile()
 */
BOOL CMergeDoc::DoSave(LPCTSTR szPath, BOOL &bSaveSuccess, BOOL bLeft)
{
	DiffFileInfo fileInfo;
	CString strSavePath(szPath);
	CString sError;
	BOOL bFileChanged = FALSE;
	BOOL bApplyToAll = FALSE;	
	int nRetVal = -1;

	bFileChanged = IsFileChangedOnDisk(szPath, fileInfo, TRUE, bLeft);
	if (bFileChanged)
	{
		CString msg;
		AfxFormatString1(msg, IDS_FILECHANGED_ONDISK, szPath);
		if (AfxMessageBox(msg, MB_ICONWARNING | MB_YESNO) == IDNO)
		{
			bSaveSuccess = SAVE_CANCELLED;
			return TRUE;
		}		
	}

	// use a temp packer
	// first copy the m_pInfoUnpacker
	// if an error arises during packing, change and take a "do nothing" packer
	PackingInfo infoTempUnpacker = *m_pInfoUnpacker;

	bSaveSuccess = FALSE;
	
	// Check third arg possibly given from command-line
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

	nRetVal = mf->HandleReadonlySave(strSavePath, FALSE, bApplyToAll);
	if (nRetVal == IDCANCEL)
		return FALSE;

	if (!mf->CreateBackup(strSavePath))
		return FALSE;

	// FALSE as long as the user is not satisfied
	// TRUE if saving succeeds, even with another filename, or if the user cancels
	BOOL result = FALSE;
	// the error code from the latest save operation, 
	// or SAVE_DONE when the save succeeds
	// TODO: Shall we return this code in addition to bSaveSuccess ?
	int nSaveErrorCode = SAVE_DONE;
	CDiffTextBuffer *pBuffer = bLeft ? &m_ltBuf : &m_rtBuf;

	// Assume empty filename means Scratchpad (unnamed file)
	// Todo: This is not needed? - buffer type check should be enough
	if (strSavePath.IsEmpty())
		nSaveErrorCode = SAVE_NO_FILENAME;

	// Handle unnamed buffers
	if (( bLeft && m_nLeftBufferType == BUFFER_UNNAMED) ||
		(!bLeft && m_nRightBufferType == BUFFER_UNNAMED))
			nSaveErrorCode = SAVE_NO_FILENAME;

	if (nSaveErrorCode == SAVE_DONE)
		// We have a filename, just try to save
		nSaveErrorCode = pBuffer->SaveToFile(strSavePath, FALSE, sError, &infoTempUnpacker);

	if (nSaveErrorCode != SAVE_DONE)
	{
		// Saving failed, user may save to another location if wants to
		do
			result = TrySaveAs(strSavePath, nSaveErrorCode, sError, bLeft, &infoTempUnpacker);
		while (!result);
	}

	// Saving succeeded with given/selected filename
	if (nSaveErrorCode == SAVE_DONE)
	{
		if (bLeft)
		{
			m_leftSaveFileInfo.Update(strSavePath);
			m_leftRescanFileInfo.Update(m_strLeftFile);
			m_strLeftFile = strSavePath;
		}
		else
		{
			m_rightSaveFileInfo.Update(strSavePath);
			m_rightRescanFileInfo.Update(m_strRightFile);
			m_strRightFile = strSavePath;
		}
		UpdateHeaderPath(bLeft);
		bSaveSuccess = TRUE;
		result = TRUE;
	}
	return result;
}

/**
 * @brief Save file with different filename.
 *
 * Safe top-level file saving function. Asks user to select filename
 * and path. Does not create backups.
 * @param [in] szPath Path where to save including filename. Can be
 * empty/NULL if new file is created (scratchpad) without filename.
 * @param [out] bSaveSuccess Will contain information about save success with
 * the original name (to determine if file statuses should be changed)
 * @param [in] bLeft If TRUE we are saving left(side) file, else right file
 * @return Tells if caller can continue (no errors happened)
 * @note Return value does not tell if SAVING succeeded. Caller must
 * Check value of bSaveSuccess parameter after calling this function!
 * @sa CMergeDoc::TrySaveAs()
 * @sa CMainFrame::CheckSavePath()
 * @sa CMergeDoc::CDiffTextBuffer::SaveToFile()
 */
BOOL CMergeDoc::DoSaveAs(LPCTSTR szPath, BOOL &bSaveSuccess, BOOL bLeft)
{
	CString strSavePath(szPath);
	CString sError;

	// use a temp packer
	// first copy the m_pInfoUnpacker
	// if an error arises during packing, change and take a "do nothing" packer
	PackingInfo infoTempUnpacker = *m_pInfoUnpacker;

	bSaveSuccess = FALSE;
	// FALSE as long as the user is not satisfied
	// TRUE if saving succeeds, even with another filename, or if the user cancels
	BOOL result = FALSE;
	// the error code from the latest save operation, 
	// or SAVE_DONE when the save succeeds
	// TODO: Shall we return this code in addition to bSaveSuccess ?
	int nSaveErrorCode = SAVE_DONE;

	// Use SAVE_NO_FILENAME to prevent asking about error
	nSaveErrorCode = SAVE_NO_FILENAME;

	// Loop until user succeeds saving or cancels
	do
		result = TrySaveAs(strSavePath, nSaveErrorCode, sError, bLeft, &infoTempUnpacker);
	while (!result);

	// Saving succeeded with given/selected filename
	if (nSaveErrorCode == SAVE_DONE)
	{
		if (bLeft)
		{
			m_leftSaveFileInfo.Update(strSavePath);
			m_leftRescanFileInfo.Update(m_strLeftFile);
			m_strLeftFile = strSavePath;
		}
		else
		{
			m_rightSaveFileInfo.Update(strSavePath);
			m_rightRescanFileInfo.Update(m_strRightFile);
			m_strRightFile = strSavePath;
		}
		UpdateHeaderPath(bLeft);
		bSaveSuccess = TRUE;
		result = TRUE;
	}
	return result;
}

/**
 * @brief Checks if line is inside given diff
 * @param nline Linenumber to text buffer (not "real" number)
 * @param ndiff Index to diff table
 */
BOOL CMergeDoc::LineInDiff(UINT nLine, UINT nDiff)
{
	ASSERT(nDiff <= m_nDiffs);
	if (nLine >= m_diffs[nDiff].dbegin0 &&
			nLine <= m_diffs[nDiff].dend0)
		return TRUE;
	else
		return FALSE;
}

/**
 * @brief Checks if given line is inside diff and
 * @param nLine [in] Linenumber, 0-based.
 * @return Index to diff table, -1 if line no inside any diff.
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

/**
 * @brief Get left->right info for a moved line (apparent line number)
 */
int CMergeDoc::RightLineInMovedBlock(int apparentLeftLine)
{
	if (!(m_ltBuf.GetLineFlags(apparentLeftLine) & LF_MOVED))
		return -1;

	int realLeftLine = m_ltBuf.ComputeRealLine(apparentLeftLine);
	int realRightLine = m_diffWrapper.RightLineInMovedBlock(realLeftLine);
	if (realRightLine != -1)
		return m_rtBuf.ComputeApparentLine(realRightLine);
	else
		return -1;
}

/**
 * @brief Get right->left info for a moved line (apparent line number)
 */
int CMergeDoc::LeftLineInMovedBlock(int apparentRightLine)
{
	if (!(m_rtBuf.GetLineFlags(apparentRightLine) & LF_MOVED))
		return -1;

	int realRightLine = m_rtBuf.ComputeRealLine(apparentRightLine);
	int realLeftLine = m_diffWrapper.LeftLineInMovedBlock(realRightLine);
	if (realLeftLine != -1)
		return m_ltBuf.ComputeApparentLine(realLeftLine);
	else
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
	// Allow user to cancel closing
	if (SaveHelper(TRUE))
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
		SetLineFlag(ct, LF_MOVED, FALSE, FALSE, FALSE);
	}
}


void CMergeDoc::CDiffTextBuffer::OnNotifyLineHasBeenEdited(int nLine)
{
	SetLineFlag(nLine, LF_DIFF, FALSE, FALSE, FALSE);
	SetLineFlag(nLine, LF_TRIVIAL, FALSE, FALSE, FALSE);
	SetLineFlag(nLine, LF_MOVED, FALSE, FALSE, FALSE);
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
int GetTextFileStyle(const UniMemFile::txtstats & stats)
{
	if (stats.ncrlfs >= stats.nlfs)
	{
		if (stats.ncrlfs >= stats.ncrs)
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
		if (stats.nlfs >= stats.ncrs)
		{
			return CRLF_STYLE_UNIX;
		}
		else
		{
			return CRLF_STYLE_MAC;
		}
	}
}
/**
 * @brief Examine statistics in textFileStats and tell if the file has only one EOL type
 */
int IsTextFileStylePure(const UniMemFile::txtstats & stats)
{
	int nType = 0;
	nType += (stats.ncrlfs > 0);
	nType += (stats.ncrs > 0);
	nType += (stats.nlfs > 0);
	return (nType <= 1);
}


/**
 * @brief Return a string giving #lines and #bytes and how much time elapsed.
 */
static CString
GetLineByteTimeReport(UINT lines, UINT bytes, const COleDateTime & start)
{
	CString sLines = locality::NumToLocaleStr(lines);
	CString sBytes = locality::NumToLocaleStr(bytes);
	COleDateTimeSpan duration = COleDateTime::GetCurrentTime() - start;
	CString sMinutes = locality::NumToLocaleStr((int)duration.GetTotalMinutes());
	CString str;
	str.Format(_T("%s lines (%s byte) saved in %sm%02ds")
		, sLines, sBytes, sMinutes
		, duration.GetSeconds()
		);
	return str;
}

/**
 * @brief Load file from disk into buffer
 *
 * @param [in] pszFileNameInit File to load
 * @param [in] infoUnpacker Unpacker plugin
 * @param [in] sToFindUnpacker String for finding unpacker plugin
 * @param [out] readOnly Loading was lossy so file should be read-only
 * @param [in] nCrlfStyle EOL style used
 * @param [in] codepage Codepage used
 * @param [out] sError Error message returned
 * @return FRESULT_OK when loading succeed or (list in files.h):
 * - FRESULT_OK_IMPURE : load OK, but the EOL are of different types
 * - FRESULT_ERROR_UNPACK : plugin failed to unpack
 * - FRESULT_ERROR : loading failed, sError contains error message
 * - FRESULT_BINARY : file is binary file
 * @note If this method fails, it calls InitNew so the CDiffTextBuffer is in a valid state
 */
int CMergeDoc::CDiffTextBuffer::LoadFromFile(LPCTSTR pszFileNameInit,
		PackingInfo * infoUnpacker, CString sToFindUnpacker, BOOL & readOnly,
		int nCrlfStyle, int codepage, CString &sError)
{
	ASSERT(!m_bInit);
	ASSERT(m_aLines.GetSize() == 0);

	// Unpacking the file here, save the result in a temporary file
	CString sFileName = pszFileNameInit;
	if (infoUnpacker->bToBeScanned)
	{
		if (!FileTransform_Unpacking(sFileName, sToFindUnpacker, infoUnpacker, &unpackerSubcode))
		{
			InitNew(); // leave crystal editor in valid, empty state
			return FRESULT_ERROR_UNPACK;
		}
	}
	else
	{
		if (!FileTransform_Unpacking(sFileName, infoUnpacker, &unpackerSubcode))
		{
			InitNew(); // leave crystal editor in valid, empty state
			return FRESULT_ERROR_UNPACK;
		}
	}
	// we use the same unpacker for both files, so it must be defined after first file
	ASSERT(infoUnpacker->bToBeScanned == FALSE);
	// we will load the transformed file
	LPCTSTR pszFileName = sFileName;

	CString sExt;
	int nRetVal = FRESULT_OK;

	// Set encoding based on extension, if we know one
	SplitFilename(pszFileName, NULL, NULL, &sExt);
	CCrystalTextView::TextDefinition *def = 
			CCrystalTextView::GetTextType(sExt);
	if (def && def->encoding != -1)
		m_nSourceEncoding = def->encoding;
	
	UniMemFile ufile;
	UniFile * pufile = &ufile;

	// Now we only use the UniFile interface
	// which is something we could implement for HTTP and/or FTP files

	if (!pufile->OpenReadOnly(pszFileName))
	{
		nRetVal = FRESULT_ERROR;
		UniFile::UniError uniErr = pufile->GetLastUniError();
		if (uniErr.hasError())
		{
			if (uniErr.apiname.IsEmpty())
				sError = uniErr.desc;
			else
				sError = GetSysError(uniErr.syserrnum);
		}
		InitNew(); // leave crystal editor in valid, empty state
		goto LoadFromFileExit;
	}
	else
	{
		// Recognize Unicode files with BOM (byte order mark)
		// or else, use the codepage we were given to interpret the 8-bit characters
		if (!pufile->ReadBom())
			pufile->SetCodepage(codepage);
		UINT lineno = 0;
		CString line, eol, preveol;
		bool done = false;
		int next_line_report = 100; // for trace messages
		int next_line_multiple = 5; // for trace messages
		COleDateTime start = COleDateTime::GetCurrentTime(); // for trace messages

		// Manually grow line array exponentially
		int arraysize = 500;
		m_aLines.SetSize(arraysize);
		
		// preveol must be initialized for empty files
		preveol = "\n";
		
		do {
			done = !pufile->ReadString(line, eol);


			if (pufile->GetTxtStats().nzeros)
			{
				nRetVal = FRESULT_BINARY;
				ResetInit(); // leave crystal editor in valid, empty state
				goto LoadFromFileExit;
			}

			// if last line had no eol, we can quit
			if (done && preveol.IsEmpty())
				break;
			// but if last line had eol, we add an extra (empty) line to buffer

			// Manually grow line array exponentially
			if (lineno == arraysize)
			{
				arraysize *= 2;
				m_aLines.SetSize(arraysize);
			}

			line += eol;
			AppendLine(lineno, line, line.GetLength());
			++lineno;
			preveol = eol;

			// send occasional line counts to trace
			// (at 100, 500, 1000, 5000, etc)
			if (lineno == next_line_report)
			{
				__int64 dwBytesRead = pufile->GetPosition();
				COleDateTimeSpan duration = COleDateTime::GetCurrentTime() - start;
				if (duration.GetTotalMinutes() > 0)
				{
					CString strace = GetLineByteTimeReport(lineno, dwBytesRead, start);
					TRACE(_T("%s\n"), (LPCTSTR)strace);
				}
				next_line_report = next_line_multiple * next_line_report;
				next_line_multiple = (next_line_multiple == 5) ? 2 : 5;
			}
		} while (!done);

		// Send report of duration to trace (if it took a while)
		COleDateTime end = COleDateTime::GetCurrentTime();
		COleDateTimeSpan duration = end - start;
		if (duration.GetTotalMinutes() > 0)
		{
			__int64 dwBytesRead = pufile->GetPosition();
			CString strace = GetLineByteTimeReport(lineno, dwBytesRead, start);
			TRACE(_T("%s\n"), (LPCTSTR)strace);
		}

		// fix array size (due to our manual exponential growth
		m_aLines.SetSize(lineno);
	
		
		//Try to determine current CRLF mode (most frequent)
		if (nCrlfStyle == CRLF_STYLE_AUTOMATIC)
		{
			nCrlfStyle = GetTextFileStyle(pufile->GetTxtStats());
		}
		ASSERT(nCrlfStyle >= 0 && nCrlfStyle <= 2);
		SetCRLFMode(nCrlfStyle);
		
		//  At least one empty line must present
		// (view does not work for empty buffers)
		ASSERT(m_aLines.GetSize() > 0);
		
		m_bInit = TRUE;
		m_bModified = FALSE;
		m_bUndoGroup = m_bUndoBeginGroup = FALSE;
		m_nUndoBufSize = 1024; // crystaltextbuffer.cpp - UNDO_BUF_SIZE;
		m_nSyncPosition = m_nUndoPosition = 0;
		ASSERT(m_aUndoBuf.GetSize() == 0);
		m_ptLastChange.x = m_ptLastChange.y = -1;
		
		FinishLoading();
		// flags don't need initialization because 0 is the default value

		// Set the return value : OK + info if the file is impure
		// A pure file is a file where EOL are consistent (all DOS, or all UNIX, or all MAC)
		// An impure file is a file with several EOL types
		// WinMerge may display impure files, but the default option is to unify the EOL
		// We return this info to the caller, so it may display a confirmation box
		if (IsTextFileStylePure(pufile->GetTxtStats()))
			nRetVal = FRESULT_OK;
		else
			nRetVal = FRESULT_OK_IMPURE;

		// stash original encoding away
		m_unicoding = pufile->GetUnicoding();
		m_codepage = pufile->GetCodepage();

		if (pufile->GetTxtStats().nlosses)
			readOnly = TRUE;
	}
	
LoadFromFileExit:
	// close the file now to free the handle
	pufile->Close();

	// delete the file that unpacking may have created
	if (_tcscmp(pszFileNameInit, pszFileName) != 0)
		if (!::DeleteFile(pszFileName))
		{
			LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
				pszFileName, GetSysError(GetLastError())));
		}

	return nRetVal;
}

/**
 * @brief Saves file from buffer to disk
 *
 * @param bTempFile : FALSE if we are saving user files and
 * TRUE if we are saving workin-temp-files for diff-engine
 *
 * @return SAVE_DONE or an error code (list in MergeDoc.h)
 */
int CMergeDoc::CDiffTextBuffer::SaveToFile (LPCTSTR pszFileName,
		BOOL bTempFile, CString & sError, PackingInfo * infoUnpacker /*= NULL*/,
		int nCrlfStyle /*= CRLF_STYLE_AUTOMATIC*/,
		BOOL bClearModifiedFlag /*= TRUE*/ )
{
	ASSERT (nCrlfStyle == CRLF_STYLE_AUTOMATIC || nCrlfStyle == CRLF_STYLE_DOS ||
		nCrlfStyle == CRLF_STYLE_UNIX || nCrlfStyle == CRLF_STYLE_MAC);
	ASSERT (m_bInit);

	if (!pszFileName || _tcslen(pszFileName) == 0)
		return SAVE_FAILED;	// No filename, cannot save...

	if (nCrlfStyle == CRLF_STYLE_AUTOMATIC &&
		!mf->m_options.GetInt(OPT_ALLOW_MIXED_EOL))
	{
			// get the default nCrlfStyle of the CDiffTextBuffer
		nCrlfStyle = GetCRLFMode();
		ASSERT(nCrlfStyle >= 0 && nCrlfStyle <= 2);
	}

	BOOL bOpenSuccess = TRUE;
	BOOL bSaveSuccess = FALSE;

	UniStdioFile file;
	file.SetUnicoding(m_unicoding);
	file.SetCodepage(m_codepage);

	CString sIntermediateFilename; // used when !bTempFile

	if (bTempFile)
	{
		bOpenSuccess = !!file.OpenCreate(pszFileName);
	}
	else
	{
		LPTSTR intermedBuffer = sIntermediateFilename.GetBuffer(_MAX_PATH);
		if (!::GetTempFileName(m_strTempPath, _T("MRG"), 0, intermedBuffer))
			return SAVE_FAILED;  //Nothing to do if even tempfile name fails
		sIntermediateFilename.ReleaseBuffer();
		bOpenSuccess = !!file.OpenCreate(sIntermediateFilename);
	}

	if (!bOpenSuccess)
	{	
		UniFile::UniError uniErr = file.GetLastUniError();
		if (uniErr.hasError())
		{
			if (uniErr.apiname.IsEmpty())
				sError = uniErr.desc;
			else
				sError = GetSysError(uniErr.syserrnum);
			if (bTempFile)
				LogErrorString(Fmt(_T("Opening file %s failed: %s"),
					pszFileName, sError));
			else
				LogErrorString(Fmt(_T("Opening file %s failed: %s"),
					sIntermediateFilename, sError));
		}
		return SAVE_FAILED;
	}

	file.WriteBom();

	// line loop : get each real line and write it in the file
	CString sLine;
	CString sEol = GetStringEol(nCrlfStyle);
	int nLineCount = m_aLines.GetSize();
	for (int line=0; line<nLineCount; ++line)
	{
		if (GetLineFlags(line) & LF_GHOST)
			continue;

		// get the characters of the line (excluding EOL)
		if (GetLineLength(line) > 0)
			GetText(line, 0, line, GetLineLength(line), sLine, 0);
		else
			sLine = _T("");

		// last real line ?
		if (line == ApparentLastRealLine())
		{
			// last real line is never EOL terminated
			ASSERT (_tcslen(GetLineEol(line)) == 0);
			// write the line and exit loop
			file.WriteString(sLine);
			break;
		}

		// normal real line : append an EOL
		if (nCrlfStyle == CRLF_STYLE_AUTOMATIC)
		{
			// either the EOL of the line (when preserve original EOL chars is on)
			sLine += GetLineEol(line);
		}
		else
		{
			// or the default EOL for this file
			sLine += sEol;
		}

		// write this line to the file (codeset or unicode conversions are done there)
		file.WriteString(sLine);
	}
	file.Close();


	if (!bTempFile)
	{
		// If we are saving user files
		// we need an unpacker/packer, at least a "do nothing" one
		ASSERT(infoUnpacker != NULL);
		// repack the file here, overwrite the temporary file we did save in
		CString csTempFileName = sIntermediateFilename;
		infoUnpacker->subcode = unpackerSubcode;
		if (!FileTransform_Packing(csTempFileName, *infoUnpacker))
		{
			if (!::DeleteFile(sIntermediateFilename))
			{
				LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
					sIntermediateFilename, GetSysError(GetLastError())));
			}
			// returns now, don't overwrite the original file
			return SAVE_PACK_FAILED;
		}
		// the temp filename may have changed during packing
		if (csTempFileName != sIntermediateFilename)
		{
			if (!::DeleteFile(sIntermediateFilename))
			{
				LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
					sIntermediateFilename, GetSysError(GetLastError())));
			}
			sIntermediateFilename = csTempFileName;
		}

		// Write tempfile over original file
		if (::CopyFile(sIntermediateFilename, pszFileName, FALSE))
		{
			if (!::DeleteFile(sIntermediateFilename))
			{
				LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
					sIntermediateFilename, GetSysError(GetLastError())));
			}
			if (bClearModifiedFlag)
			{
				SetModified(FALSE);
				m_nSyncPosition = m_nUndoPosition;
			}
			bSaveSuccess = TRUE;
		}
		else
		{
			sError = GetSysError(GetLastError());
			LogErrorString(Fmt(_T("CopyFile(%s, %s) failed: %s"),
				sIntermediateFilename, pszFileName, sError));
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

/**
 * @brief Get path and name for temporary files and init those files.
 *
 * This function gets temp file path from system and generates new
 * unique temp filenames. User files are then copied to temp files.
 * @param srcPathL [in] Left-side userfile path
 * @param strPathR [in] Right-side userfile path
 * return TRUE if tempfiles creates successfully.
 */
BOOL CMergeDoc::InitTempFiles(const CString& strPathL, const CString& strPathR)
{
	TCHAR strTempPath[MAX_PATH] = {0};

	if (!::GetTempPath(MAX_PATH, strTempPath))
	{
		LogErrorString(Fmt(_T("GetTempPath() failed: %s"),
			GetSysError(GetLastError())));
		return FALSE;
	}

	// Set temp paths for buffers
	m_ltBuf.SetTempPath(strTempPath);
	m_rtBuf.SetTempPath(strTempPath);

	if (m_strTempLeftFile.IsEmpty())
	{
		TCHAR name[MAX_PATH];
		if (!::GetTempFileName(strTempPath, _T("_LT"), 0, name))
		{
			LogErrorString(Fmt(_T("GetTempFileName() for left-side failed: %s"),
				GetSysError(GetLastError())));
			return FALSE;
		}
		m_strTempLeftFile = name;

		if (!strPathL.IsEmpty())
		{
			if (!::CopyFile(strPathL, m_strTempLeftFile, FALSE))
			{
				LogErrorString(Fmt(_T("CopyFile() (copy left-side temp file) failed: %s"),
					GetSysError(GetLastError())));
				return FALSE;
			}
		}
		::SetFileAttributes(m_strTempLeftFile, FILE_ATTRIBUTE_NORMAL);
	}
	
	if (m_strTempRightFile.IsEmpty())
	{
		TCHAR name[MAX_PATH];
		if (!::GetTempFileName(strTempPath, _T("_RT"), 0, name))
		{
			LogErrorString(Fmt(_T("GetTempFileName() for right-side failed: %s"),
				strTempPath, GetSysError(GetLastError())));
			return FALSE;
		}
		m_strTempRightFile = name;

		if (!strPathR.IsEmpty())
		{
			if (!::CopyFile(strPathR, m_strTempRightFile, FALSE))
			{
				LogErrorString(Fmt(_T("CopyFile() (copy right-side temp file) failed: %s"),
					GetSysError(GetLastError())));
				return FALSE;
			}
		}
		::SetFileAttributes(m_strTempRightFile, FILE_ATTRIBUTE_NORMAL);
	}
	return TRUE;
}

/**
 * @brief Remove temporary files.
 * @note Set tempfilenames empty even if deleting fails.
 * So we can create new files when needed. This causes leak
 * of temp files but at least WinMerge works...
 */
void CMergeDoc::CleanupTempFiles()
{
	if (!m_strTempLeftFile.IsEmpty())
	{
		if (!::DeleteFile(m_strTempLeftFile))
		{
			LogErrorString(Fmt(_T("DeleteFile(%s) (deleting left-side temp file) failed: %s"),
				m_strTempLeftFile, GetSysError(GetLastError())));
		}
		m_strTempLeftFile.Empty();

	}
	if (!m_strTempRightFile.IsEmpty())
	{
		if (!::DeleteFile(m_strTempRightFile))
		{
			LogErrorString(Fmt(_T("DeleteFile(%s) (deleting right-side temp file) failed: %s"),
				m_strTempRightFile, GetSysError(GetLastError())));
		}
		m_strTempRightFile.Empty();
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

/**
 * @brief Take care of rescanning document.
 * 
 * Update view and restore cursor and scroll position after
 * rescanning document.
 * @param bForced If TRUE rescan cannot be suppressed
 */
void CMergeDoc::FlushAndRescan(BOOL bForced /* =FALSE */)
{
	// Ignore suppressing when forced rescan
	if (!bForced)
		if (!m_bEnableRescan) return;

	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_RESCANNING));

	CCrystalTextView* curView = dynamic_cast<CCrystalTextView*> (GetParentFrame()->GetActiveView());

	// store cursors and hide caret
	m_pLeftView->PushCursors();
	m_pRightView->PushCursors();
	m_pLeftDetailView->PushCursors();
	m_pRightDetailView->PushCursors();
	if (curView)
		curView->HideCursor();

	BOOL bBinary = FALSE;
	BOOL bIdentical = FALSE;
	int nRescanResult = Rescan(bBinary, bIdentical, bForced);

	// restore cursors and caret
	m_pLeftView->PopCursors();
	m_pRightView->PopCursors();
	m_pLeftDetailView->PopCursors();
	m_pRightDetailView->PopCursors();
	if (curView)
		curView->ShowCursor();

	// because of ghostlines, m_nTopLine may differ just after Rescan
	// scroll both views to the same top line
	CMergeEditView * fixedView = m_pLeftView;
	if (curView == m_pLeftView || curView == m_pRightView)
		// only one view needs to scroll so do not scroll the active view
		fixedView = (CMergeEditView*) curView;
	fixedView->UpdateSiblingScrollPos(FALSE);

	// make sure we see the cursor from the curent view
	if (curView == m_pRightView || curView == m_pLeftView)
		curView->EnsureVisible(curView->GetCursorPos());

	// scroll both diff views to the same top line
	CMergeDiffDetailView * fixedDetailView = m_pLeftDetailView;
	if (curView == m_pLeftDetailView || curView == m_pRightDetailView)
		// only one view needs to scroll so do not scroll the active view
		fixedDetailView = (CMergeDiffDetailView*) curView;
	fixedDetailView->UpdateSiblingScrollPos(FALSE);

	// Refresh display
	UpdateAllViews(NULL);

	// Show possible error after updating screen
	if (nRescanResult != RESCAN_SUPPRESSED)
		ShowRescanError(nRescanResult, bBinary, bIdentical);
}

/**
 * @brief Saves both files
 */
void CMergeDoc::OnFileSave() 
{
	// We will need to know if either of the originals actually changed
	// so we know whether to update the diff status
	BOOL bLChangedOriginal = FALSE;
	BOOL bRChangedOriginal = FALSE;

	if (m_ltBuf.IsModified() && !m_ltBuf.GetReadOnly())
	{
		// (why we don't use return value of DoSave)
		// DoSave will return TRUE if it wrote to something successfully
		// but we have to know if it overwrote the original file
		BOOL bSaveOriginal = FALSE;
		DoSave(m_strLeftFile, bSaveOriginal, TRUE );
		if (bSaveOriginal)
			bLChangedOriginal = TRUE;
	}

	if (m_rtBuf.IsModified() && !m_rtBuf.GetReadOnly())
	{
		// See comments above for left case
		BOOL bSaveOriginal = FALSE;
		DoSave(m_strRightFile, bSaveOriginal, FALSE);
		if (bSaveOriginal)
			bRChangedOriginal = TRUE;
	}

	// If either of the actual source files being compared was changed
	// we need to update status in the dir view
	if (bLChangedOriginal || bRChangedOriginal)
	{
		// If DirDoc contains diffs
		if (m_pDirDoc->m_pCtxt)
		{
			if (m_bLeftEditAfterRescan || m_bRightEditAfterRescan)
				FlushAndRescan(FALSE);

			BOOL bIdentical = (m_nDiffs == 0); // True if status should be set to identical
			m_pDirDoc->UpdateChangedItem(m_strLeftFile, m_strRightFile,
				m_nDiffs, m_nTrivialDiffs, bIdentical);
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
		// If DirDoc contains compare results
		if (m_pDirDoc->m_pCtxt)
		{
			if (m_bLeftEditAfterRescan || m_bRightEditAfterRescan)
				FlushAndRescan(FALSE);

			BOOL bIdentical = (m_nDiffs == 0); // True if status should be set to identical
			m_pDirDoc->UpdateChangedItem(m_strLeftFile, m_strRightFile,
				m_nDiffs, m_nTrivialDiffs, bIdentical);
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
		// If DirDoc contains compare results
		if (m_pDirDoc->m_pCtxt)
		{
			if (m_bLeftEditAfterRescan || m_bRightEditAfterRescan)
				FlushAndRescan(FALSE);

			BOOL bIdentical = (m_nDiffs == 0); // True if status should be set to identical
			m_pDirDoc->UpdateChangedItem(m_strLeftFile, m_strRightFile,
				m_nDiffs, m_nTrivialDiffs, bIdentical);
		}
	}
}

/**
 * @brief Saves left-side file with name asked
 */
void CMergeDoc::OnFileSaveAsLeft()
{
	CString s;
	BOOL bSaveResult = FALSE;
	
	DoSaveAs(s, bSaveResult, TRUE);
}

/**
 * @brief Saves right-side file with name asked
 */
void CMergeDoc::OnFileSaveAsRight()
{
	CString s;
	BOOL bSaveResult = FALSE;
	
	DoSaveAs(s, bSaveResult, FALSE);
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
		if (m_nDiffs == 1)
			VERIFY(s.LoadString(IDS_1_DIFF_FOUND));
		else
		{
			sCnt.Format(_T("%ld"), m_nDiffs);
			AfxFormatString1(s, IDS_NO_DIFF_SEL_FMT, sCnt);
		}
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


/**
 * @brief Build the diff array and prepare buffers accordingly (insert ghost lines, set WinMerge flags)
 *
 * @note Buffers may have different length after PrimeTextBuffers. Indeed, no
 * synchronization is needed after the last line. So no ghost line will be created
 * to face an ignored difference in the last line (typically : 'ignore blank lines' 
 * + empty last line on one side).
 * If you fell that different length buffers are really strange, CHANGE FIRST
 * the last diff to take into account the empty last line.
 */
void CMergeDoc::PrimeTextBuffers()
{
	SetCurrentDiff(-1);
	m_nTrivialDiffs = 0;
	int nDiff;

	// walk the diff list and calculate numbers of extra lines to add
	UINT LeftExtras=0;   // extra lines added to left view
	UINT RightExtras=0;   // extra lines added to right view
	for (nDiff = 0; nDiff < m_nDiffs; ++ nDiff)
	{
		DIFFRANGE &curDiff = m_diffs[nDiff];

		// this guarantees that all the diffs are synchronized
		ASSERT(curDiff.begin0+LeftExtras == curDiff.begin1+RightExtras);
		int nline0 = curDiff.end0-curDiff.begin0+1;
		int nline1 = curDiff.end1-curDiff.begin1+1;
		int nextra = nline0-nline1;

		if (nextra > 0)
			RightExtras += nextra;
		else
			LeftExtras -= nextra;
	}

	// resize m_aLines once for each view
	UINT lcount0 = m_ltBuf.GetLineCount();
	UINT lcount1 = m_rtBuf.GetLineCount();
	UINT lcount0new = lcount0 + LeftExtras;
	UINT lcount1new = lcount1 + RightExtras;
// this ASSERT may be false because of empty last line (see function's note)
//	ASSERT(lcount0new == lcount1new);
	m_ltBuf.m_aLines.SetSize(lcount0new);
	m_rtBuf.m_aLines.SetSize(lcount1new);

	// walk the diff list backward, move existing lines to proper place,
	// add ghost lines, and set flags
	for (nDiff = m_nDiffs - 1; nDiff >= 0; nDiff --)
	{
		DIFFRANGE &curDiff = m_diffs[nDiff];

		// move matched lines after curDiff
		int nline0 = lcount0 - curDiff.end0 - 1;
		int nline1 = lcount1 - curDiff.end1 - 1;
		// Matched lines should really match...
		// But matched lines after last diff may differ because of empty last line (see function's note)
		if (nDiff < m_nDiffs - 1)
			ASSERT(nline0 == nline1);
		m_ltBuf.MoveLine(curDiff.end0+1, lcount0-1, lcount0new-nline0);
		m_rtBuf.MoveLine(curDiff.end1+1, lcount1-1, lcount1new-nline1);
		lcount0new -= nline0;
		lcount1new -= nline1;
		lcount0 -= nline0;
		lcount1 -= nline1;

		// move unmatched lines and add ghost lines
		nline0 = curDiff.end0 - curDiff.begin0 + 1;
		nline1 = curDiff.end1 - curDiff.begin1 + 1;
		int nextra = nline0-nline1;
		int nextraAbs = (nextra >= 0) ? nextra : -nextra;

		if (nextra > 0) 
		{
			// more lines on the left
			m_ltBuf.MoveLine(curDiff.begin0, curDiff.end0, lcount0new-nline0);
			m_rtBuf.MoveLine(curDiff.begin1, curDiff.end1, lcount1new-nline0);
			m_rtBuf.SetEmptyLine(lcount1new - nextraAbs, nextraAbs);
			for (int i = 1; i <= nextraAbs; i++)
				m_rtBuf.SetLineFlag(lcount1new-i, LF_GHOST, TRUE, FALSE, FALSE);
			lcount0new -= nline0;
			lcount1new -= nline0;
		}
		else if (nextra < 0) 
		{
			// more lines on the right
			m_ltBuf.MoveLine(curDiff.begin0, curDiff.end0, lcount0new-nline1);
			m_rtBuf.MoveLine(curDiff.begin1, curDiff.end1, lcount1new-nline1);
			m_ltBuf.SetEmptyLine(lcount0new - nextraAbs, nextraAbs);
			for (int i = 1; i <= nextraAbs; i++)
				m_ltBuf.SetLineFlag(lcount0new-i, LF_GHOST, TRUE, FALSE, FALSE);
			lcount0new -= nline1;
			lcount1new -= nline1;
		}
		else 
		{
			// same number of lines
			m_ltBuf.MoveLine(curDiff.begin0, curDiff.end0, lcount0new-nline0);
			m_rtBuf.MoveLine(curDiff.begin1, curDiff.end1, lcount1new-nline1);
			lcount0new -= nline0;
			lcount1new -= nline1;
		}
		lcount0 -= nline0;
		lcount1 -= nline1;

		// set dbegin, dend, blank, and line flags
		curDiff.dbegin0 = lcount0new;
		curDiff.dbegin1 = lcount1new;

		switch (curDiff.op)
		{
		case OP_LEFTONLY:
			// set curdiff
			{
				// left side
				curDiff.dend0 = lcount0new+nline0-1;
				curDiff.blank0 = -1;
				// right side
				curDiff.dend1 = lcount1new+nline0-1;
				curDiff.blank1 = curDiff.dbegin1;
			}
			// flag lines
			{
				for (UINT i = curDiff.dbegin0 ; i <= curDiff.dend0; i++)
					m_ltBuf.SetLineFlag(i, LF_DIFF, TRUE, FALSE, FALSE);
				// blanks are already inserted (and flagged) to compensate for diff on other side
			}
			break;
		case OP_RIGHTONLY:
			// set curdiff
			{
				// left side
				curDiff.dend0 = lcount0new+nline1-1;
				curDiff.blank0 = curDiff.dbegin0;
				// right side
				curDiff.dend1 = lcount1new+nline1-1;
				curDiff.blank1 = -1;
			}
			// flag lines
			{
				for (UINT i = curDiff.dbegin1 ; i <= curDiff.dend1 ; i++)
					m_rtBuf.SetLineFlag(i, LF_DIFF, TRUE, FALSE, FALSE);
				// blanks are already inserted (and flagged) to compensate for diff on other side
			}
			break;
		case OP_TRIVIAL:
			++m_nTrivialDiffs;
			// fall through and handle as diff
		case OP_DIFF:
			// set curdiff
			{
				int nline = max(nline0, nline1);
				// left side
				curDiff.dend0 = lcount0new+nline-1;
				curDiff.blank0 = -1;
				// right side
				curDiff.dend1 = lcount1new+nline-1;
				curDiff.blank1 = -1;
				if (nline0 > nline1)
					// more lines on left, ghost lines on right side
					curDiff.blank1 = curDiff.dend1+1 - nextraAbs;
				else if (nline0 < nline1)
					// more lines on right, ghost lines on left side
					curDiff.blank0 = curDiff.dend0+1 - nextraAbs;
			}
			// flag lines
			{
				// left side
				for (UINT i = curDiff.dbegin0; i <= curDiff.dend0 ; i++)
				{
					if (curDiff.blank0 == -1 || (int)i < curDiff.blank0)
					{
						// set diff or trivial flag
						DWORD dflag = (curDiff.op == OP_DIFF) ? LF_DIFF : LF_TRIVIAL;
						m_ltBuf.SetLineFlag(i, dflag, TRUE, FALSE, FALSE);
					}
					else
					{
						// ghost lines are already inserted (and flagged)
						// ghost lines opposite to trivial lines are ghost and trivial
						if (curDiff.op == OP_TRIVIAL)
							m_ltBuf.SetLineFlag(i, LF_TRIVIAL, TRUE, FALSE, FALSE);
					}
				}
				// right side
				for (i = curDiff.dbegin1; i <= curDiff.dend1 ; i++)
				{
					if (curDiff.blank1 == -1 || (int)i < curDiff.blank1)
					{
						// set diff or trivial flag
						DWORD dflag = (curDiff.op == OP_DIFF) ? LF_DIFF : LF_TRIVIAL;
						m_rtBuf.SetLineFlag(i, dflag, TRUE, FALSE, FALSE);
					}
					else
					{
						// ghost lines are already inserted (and flagged)
						// ghost lines opposite to trivial lines are ghost and trivial
						if (curDiff.op == OP_TRIVIAL)
							m_rtBuf.SetLineFlag(i, LF_TRIVIAL, TRUE, FALSE, FALSE);
					}
				}
			}
			break;
		}           // switch (curDiff.op)
	}             // for (nDiff = m_nDiffs; nDiff-- > 0; )

	if (m_nTrivialDiffs)
	{
		// The following code deletes all trivial changes
		//
		// #1) Copy nontrivial diffs into new array
		CArray<DIFFRANGE,DIFFRANGE> newdiffs;
		newdiffs.SetSize(m_diffs.GetSize()-m_nTrivialDiffs);
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
 * @brief Checks if file has changed since last update (save or rescan).
 * @param [in] szPath File to check
 * @param [in] dfi Previous fileinfo of file
 * @param [in] bSave If TRUE Compare to last save-info, else to rescan-info
 * @param [in] bLeft If TRUE, compare left file, else right file
 * @return TRUE if file is changed.
 */
BOOL CMergeDoc::IsFileChangedOnDisk(LPCTSTR szPath, DiffFileInfo &dfi,
	BOOL bSave, BOOL bLeft)
{
	DiffFileInfo *fileInfo = NULL;
	BOOL bFileChanged = FALSE;

	if (bLeft)
	{
		if (bSave)
			fileInfo = &m_leftSaveFileInfo;
		else
			fileInfo = &m_leftRescanFileInfo;
	}
	else
	{
		if (bSave)
			fileInfo = &m_rightSaveFileInfo;
		else
			fileInfo = &m_rightRescanFileInfo;
	}

	dfi.Update(szPath);

	
	if (dfi.mtime != fileInfo->mtime ||
		dfi.size != fileInfo->size)
	{
		bFileChanged = TRUE;
	}
	return bFileChanged;
}

/**
 * @brief Asks and then saves modified files.
 *
 * This function saves modified files. Dialog is shown for user to select
 * modified file(s) one wants to save or discard changed. Cancelling of
 * save operation is allowed unless denied by parameter. After successfully
 * save operation file statuses are updated to directory compare.
 * @param [in] bAllowCancel If FALSE "Cancel" button is disabled.
 * @return TRUE if user selected "OK" so next operation can be
 * executed. If FALSE user choosed "Cancel".
 * @note If filename is empty, we assume scratchpads are saved,
 * so instead of filename, description is shown.
 * @todo If we have filename and description for file, what should
 * we do after saving to different filename? Empty description?
 */
BOOL CMergeDoc::SaveHelper(BOOL bAllowCancel)
{
	const BOOL bLModified = m_ltBuf.IsModified();
	const BOOL bRModified = m_rtBuf.IsModified();
	BOOL result = TRUE;
	BOOL bLSaveSuccess = FALSE;
	BOOL bRSaveSuccess = FALSE;
	SaveClosingDlg dlg;

	if (!bLModified && !bRModified) //Both files unmodified
		return TRUE;

	dlg.DoAskFor(bLModified, bRModified);
	if (!bAllowCancel)
		dlg.m_bDisableCancel = TRUE;
	if (!m_strLeftFile.IsEmpty())
		dlg.m_sLeftFile = m_strLeftFile;
	else
		dlg.m_sLeftFile = m_strLeftDesc;
	if (!m_strRightFile.IsEmpty())
		dlg.m_sRightFile = m_strRightFile;
	else
		dlg.m_sRightFile = m_strRightDesc;

	if (dlg.DoModal() == IDOK)
	{
		if (bLModified && dlg.m_leftSave == 0)
		{
			if (!DoSave(m_strLeftFile, bLSaveSuccess, TRUE))
				result = FALSE;
		}

		if (bRModified && dlg.m_rightSave == 0)
		{
			if (!DoSave(m_strRightFile, bRSaveSuccess, FALSE))
				result = FALSE;
		}
	}
	else
		result = FALSE;

	// If file were modified and saving was successfull,
	// update status on dir view
	if ((bLModified && bLSaveSuccess) ||
		 (bRModified && bRSaveSuccess))
	{
		// If directory compare has results
		if (m_pDirDoc->m_pCtxt)
		{
			if (m_bLeftEditAfterRescan || m_bRightEditAfterRescan)
				FlushAndRescan(FALSE);

			BOOL bIdentical = (m_nDiffs == 0); // True if status should be set to identical
			m_pDirDoc->UpdateChangedItem(m_strLeftFile, m_strRightFile,
				m_nDiffs, m_nTrivialDiffs, bIdentical);
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
	// Allow user to cancel closing
	if (!SaveHelper(TRUE))
		return FALSE;

	GetParentFrame()->CloseNow();
	return TRUE;
}

/**
 * @brief Loads file to buffer and shows load-errors
 * @param [in] sFileName File to open
 * @param [in] bLeft Left/right-side file
 * @param [out] readOnly whether file is read-only
 * @param [in] codepage relevant 8-bit codepage if any (0 if none or unknown)
 * @return Tells if files were loaded succesfully
 * @sa CMergeDoc::OpenDocs()
 **/
int CMergeDoc::LoadFile(CString sFileName, BOOL bLeft, BOOL & readOnly, int codepage)
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

	int nCrlfStyle = CRLF_STYLE_AUTOMATIC;
	CString sOpenError;
	retVal = pBuf->LoadFromFile(sFileName, m_pInfoUnpacker,
		m_strBothFilenames, readOnly, nCrlfStyle, codepage, sOpenError);

	// if CMergeDoc::CDiffTextBuffer::LoadFromFile failed,
	// it left the pBuf in a valid (but empty) state via a call to InitNew

	if (retVal == FRESULT_OK_IMPURE)
	{
		// File loaded, and multiple EOL types in this file
		retVal = FRESULT_OK;
		// By default, WinMerge unifies EOL to the most used type (when diffing or saving)
		// As some info are lost, we request a confirmation from the user
		if (!mf->m_options.GetInt(OPT_ALLOW_MIXED_EOL))
		{
			CString s;
			AfxFormatString1(s, IDS_SUGGEST_PRESERVEEOL, sFileName); 
			if (AfxMessageBox(s, MB_YESNO|MB_ICONQUESTION|MB_DONT_ASK_AGAIN, IDS_SUGGEST_PRESERVEEOL) == IDYES)
				// the user wants to keep the original chars
				mf->SetEOLMixed(TRUE);
		}
	}

	if (retVal == FRESULT_ERROR)
	{
		// Error from Unifile/system
		if (!sOpenError.IsEmpty())
			AfxFormatString2(sError, IDS_ERROR_FILEOPEN, sFileName, sOpenError);
		else
			AfxFormatString1(sError, IDS_ERROR_FILE_NOT_FOUND, sFileName);
		AfxMessageBox(sError, MB_OK | MB_ICONSTOP);
	}
	else if (retVal == FRESULT_ERROR_UNPACK)
	{
		AfxFormatString1(sError, IDS_ERROR_FILE_NOT_UNPACKED, sFileName);
		AfxMessageBox(sError, MB_OK | MB_ICONSTOP);
	}
	return retVal;
}

/**
 * @brief Loads files and does initial rescan
 * @param sLeftFile [in] File to open to left side
 * @param sRightFile [in] File to open to right side
 * @param bROLeft [in] Is left file read-only
 * @param bRORight [in] Is right file read-only
 * @param cpleft [in] Is left file's 8-bit codepage (eg, 1252) if applicable (0 is unknown or N/A)
 * @param cpright [in] Is right file's 8-bit codepage (eg, 1252) if applicable (0 is unknown or N/A)
 * @return Tells if files were loaded and scanned succesfully
 * @todo Options are still read from CMainFrame, this will change
 * @sa CMainFrame::ShowMergeDoc()
 */
BOOL CMergeDoc::OpenDocs(CString sLeftFile, CString sRightFile,
		BOOL bROLeft, BOOL bRORight, int cpleft, int cpright)
{
	BOOL bBinary = FALSE;
	BOOL bIdentical = FALSE;
	int nRescanResult = RESCAN_OK;

	// clear undo stack
	undoTgt.clear();
	curUndo = undoTgt.begin();

	// Prevent displaying views during LoadFile
	// Note : attach buffer again only if both loads succeed
	m_pLeftView->DetachFromBuffer();
	m_pRightView->DetachFromBuffer();
	m_pLeftDetailView->DetachFromBuffer();
	m_pRightDetailView->DetachFromBuffer();

	// free the buffers
	m_ltBuf.FreeAll ();
	m_rtBuf.FreeAll ();

	// build the text being filtered, "|" separates files as it is forbidden in filenames
	m_strBothFilenames = sLeftFile + _T("|") + sRightFile;

	// Load left side file
	int nLeftSuccess = FRESULT_ERROR;
	if (!sLeftFile.IsEmpty())
	{
		if (mf->m_strLeftDesc.IsEmpty())
			m_nLeftBufferType = BUFFER_NORMAL;
		else
		{
			m_nLeftBufferType = BUFFER_NORMAL_NAMED;
			m_strLeftDesc = mf->m_strLeftDesc;
			mf->m_strLeftDesc.Empty();
		}

		m_leftSaveFileInfo.Update(sLeftFile);
		m_leftRescanFileInfo.Update(sLeftFile);

		// Load left side file
		nLeftSuccess = LoadFile(sLeftFile, TRUE, bROLeft, cpleft);
	}
	else
	{
		m_nLeftBufferType = BUFFER_UNNAMED;

		m_ltBuf.InitNew();
		m_strLeftDesc = mf->m_strLeftDesc;
		nLeftSuccess = FRESULT_OK;
	}
	
	// Load right side only if left side was succesfully loaded
	int nRightSuccess = FRESULT_ERROR;
	if (!sRightFile.IsEmpty())
	{
		if (mf->m_strRightDesc.IsEmpty())
			m_nRightBufferType = BUFFER_NORMAL;
		else
		{
			m_nRightBufferType = BUFFER_NORMAL_NAMED;
			m_strRightDesc = mf->m_strRightDesc;
			mf->m_strRightDesc.Empty();
		}

		m_rightSaveFileInfo.Update(sRightFile);
		m_rightRescanFileInfo.Update(sRightFile);
		if (nLeftSuccess == FRESULT_OK || nLeftSuccess == FRESULT_BINARY)
			nRightSuccess = LoadFile(sRightFile, FALSE, bRORight, cpright);
	}
	else
	{
		m_nRightBufferType = BUFFER_UNNAMED;

		m_rtBuf.InitNew();
		m_strRightDesc = mf->m_strRightDesc;
		nRightSuccess = FRESULT_OK;
	}

	// scratchpad : we don't call LoadFile, so
	// we need to initialize the unpacker as a "do nothing" one
	if (m_nLeftBufferType == BUFFER_UNNAMED && m_nRightBufferType == BUFFER_UNNAMED)
		m_pInfoUnpacker->Initialize(PLUGIN_MANUAL);

	// Bail out if either side failed
	if (nLeftSuccess != FRESULT_OK || nRightSuccess != FRESULT_OK)
	{
		if (nLeftSuccess == FRESULT_BINARY || nRightSuccess == FRESULT_BINARY)
			CompareBinaries(sLeftFile, sRightFile, nLeftSuccess, nRightSuccess);
		return FALSE;
	}

	// Now buffers data are valid
	m_pLeftView->AttachToBuffer();
	m_pRightView->AttachToBuffer();
	m_pLeftDetailView->AttachToBuffer();
	m_pRightDetailView->AttachToBuffer();
		
	// Set read-only statuses
	m_ltBuf.SetReadOnly(bROLeft);
	m_rtBuf.SetReadOnly(bRORight);

	// Check the EOL sensitivity option (do it before Rescan)
	DIFFOPTIONS diffOptions = {0};
	m_diffWrapper.GetOptions(&diffOptions);
	if (m_ltBuf.GetCRLFMode() != m_rtBuf.GetCRLFMode() &&
		!mf->m_options.GetInt(OPT_ALLOW_MIXED_EOL) && diffOptions.bEolSensitive)
	{
		// Options and files not are not compatible :
		// Sensitive to EOL on, allow mixing EOL off, and files have a different EOL style.
		// All lines will differ, that is not very interesting and probably not wanted.
		// Propose to turn off the option 'sensitive to EOL'
		CString sOptionName;
		VERIFY(sOptionName.LoadString(IDC_MIXED_EOL));
		CString s;
		AfxFormatString1(s, IDS_SUGGEST_IGNOREEOL, sOptionName); 
		if (AfxMessageBox(s, MB_YESNO|MB_ICONQUESTION|MB_DONT_ASK_AGAIN, IDS_SUGGEST_IGNOREEOL) == IDYES)
		{
			diffOptions.bEolSensitive = FALSE;
			m_diffWrapper.SetOptions(&diffOptions);
		}
	}

	// Define the prediffer
	PackingInfo * infoUnpacker = 0;
	PrediffingInfo * infoPrediffer = 0;
	m_pDirDoc->FetchPluginInfos(m_strBothFilenames, &infoUnpacker, &infoPrediffer);
	m_diffWrapper.SetPrediffer(infoPrediffer);
	m_diffWrapper.SetTextForAutomaticPrediff(m_strBothFilenames);
	
	nRescanResult = Rescan(bBinary, bIdentical);

	// recreate the sub menu (to fill the "selected prediffers")
	// keep after Rescan (in automatic mode, prediffer is set during the first Rescan)
	mf->UpdatePrediffersMenu();

	// Open filed if rescan succeed and files are not binaries
	if (nRescanResult == RESCAN_OK && bBinary == FALSE)
	{
		// prepare the four views
		CMergeEditView * pLeft = GetLeftView();
		CMergeEditView * pRight = GetRightView();
		CMergeDiffDetailView * pLeftDetail = GetLeftDetailView();
		CMergeDiffDetailView * pRightDetail = GetRightDetailView();
		
		// set the document types
		// Warning : it is the first thing to do (must be done before UpdateView,
		// or any function that calls UpdateView, like SelectDiff)
		// Note: If option enabled, and another side type is not recognized,
		// we use recognized type for unrecognized side too.
		CString sextL, sextR;
		SplitFilename(sLeftFile, 0, 0, &sextL);
		BOOL bLeftTyped = pLeft->SetTextType(sextL);
		pLeftDetail->SetTextType(sextL);
		SplitFilename(sRightFile, 0, 0, &sextR);
		BOOL bRightTyped = pRight->SetTextType(sextR);
		pRightDetail->SetTextType(sextR);

		if (bLeftTyped != bRightTyped &&
			mf->m_options.GetInt(OPT_UNREC_APPLYSYNTAX))
		{
			CCrystalTextView::TextDefinition *enuType;

			if (bLeftTyped)
			{
				enuType = pLeft->GetTextType(sextL);
				pRight->SetTextType(enuType);
				pRightDetail->SetTextType(enuType);
			}
			else
			{
				enuType = pRight->GetTextType(sextR);
				pLeft->SetTextType(enuType);
				pLeftDetail->SetTextType(enuType);
			}
		}

		// scroll to first diff
		if (mf->m_options.GetInt(OPT_SCROLL_TO_FIRST) &&
			m_diffs.GetSize() != 0)
		{
			pLeft->SelectDiff(0, TRUE, FALSE);
		}

		// Enable/disable automatic rescan (rescanning after edit)
		pLeft->EnableRescan(mf->m_options.GetInt(OPT_AUTOMATIC_RESCAN));
		pRight->EnableRescan(mf->m_options.GetInt(OPT_AUTOMATIC_RESCAN));

		// SetTextType will revert to language dependent defaults for tab
		pLeft->SetTabSize(mf->m_options.GetInt(OPT_TAB_SIZE));
		pRight->SetTabSize(mf->m_options.GetInt(OPT_TAB_SIZE));
		pLeft->SetViewTabs(mf->m_options.GetInt(OPT_VIEW_WHITESPACE));
		pRight->SetViewTabs(mf->m_options.GetInt(OPT_VIEW_WHITESPACE));
		pLeft->SetViewEols(mf->m_options.GetInt(OPT_VIEW_WHITESPACE),
			mf->m_options.GetInt(OPT_ALLOW_MIXED_EOL));
		pRight->SetViewEols(mf->m_options.GetInt(OPT_VIEW_WHITESPACE),
			mf->m_options.GetInt(OPT_ALLOW_MIXED_EOL));
		pLeft->SetWordWrapping(FALSE);
		pRight->SetWordWrapping(FALSE);

		pLeftDetail->SetTabSize(mf->m_options.GetInt(OPT_TAB_SIZE));
		pRightDetail->SetTabSize(mf->m_options.GetInt(OPT_TAB_SIZE));
		pLeftDetail->SetViewTabs(mf->m_options.GetInt(OPT_VIEW_WHITESPACE));
		pRightDetail->SetViewTabs(mf->m_options.GetInt(OPT_VIEW_WHITESPACE));
		pLeftDetail->SetViewEols(mf->m_options.GetInt(OPT_VIEW_WHITESPACE),
			mf->m_options.GetInt(OPT_ALLOW_MIXED_EOL));
		pRightDetail->SetViewEols(mf->m_options.GetInt(OPT_VIEW_WHITESPACE),
			mf->m_options.GetInt(OPT_ALLOW_MIXED_EOL));
		pLeftDetail->SetWordWrapping(FALSE);
		pRightDetail->SetWordWrapping(FALSE);

		// Enable Backspace at beginning of line
		pLeft->SetDisableBSAtSOL(FALSE);
		pRight->SetDisableBSAtSOL(FALSE);
		
		// set the frame window header
		UpdateHeaderPath(TRUE);
		UpdateHeaderPath(FALSE);

		// Set tab type (tabs/spaces)
		BOOL bInsertTabs = (mf->m_options.GetInt(OPT_TAB_TYPE) == 0);
		pLeft->SetInsertTabs(bInsertTabs);
		pRight->SetInsertTabs(bInsertTabs);

		// Inform user that files are identical
		// Don't show message if new buffers created
		if (bIdentical && (m_nLeftBufferType == BUFFER_NORMAL ||
			m_nRightBufferType == BUFFER_NORMAL))
		{
			ShowRescanError(nRescanResult, bBinary, bIdentical);
		}
	}
	else
	{
		// CMergeDoc::Rescan fails if files do not exist on both sides 
		// or the really arcane case that the temp files couldn't be created, 
		// which is too obscure to bother reporting if you can't write to 
		// your temp directory, doing nothing is graceful enough for that).
		ShowRescanError(nRescanResult, bBinary, bIdentical);
		GetParentFrame()->DestroyWindow();
		return FALSE;
	}
	return TRUE;
}

/**
 * @brief Compare binary files and print results to user.
 *
 * @param sLeftFile [in] Full path to left file
 * @param sRightFile [in] Full path to right file
 * @param nLeftSuccess [in] Returnvalue from file load for leftside
 * @param nRightSuccess [in] Returnvalue from file load for rightside
 * @sa CMergeDoc::OpenDocs()
 * @sa CMergeDoc::Rescan()
 */
void CMergeDoc::CompareBinaries(CString sLeftFile, CString sRightFile, int nLeftSuccess, int nRightSuccess)
{
	int nRescanResult = RESCAN_OK;
	BOOL bBinary = FALSE;
	BOOL bIdentical = FALSE;

	// Compare binary files
	if (nLeftSuccess == FRESULT_BINARY && nRightSuccess == FRESULT_BINARY)
	{
		bBinary = TRUE; // Compare binary files
		int nRescanResult = Rescan(bBinary, bIdentical);
	}

	if (nRescanResult == RESCAN_OK)
	{
		// Format message shown to user: both files are binaries
		if (nLeftSuccess == FRESULT_BINARY && nRightSuccess == FRESULT_BINARY)
		{
			CString msg;
			CString msg2;
			if (bIdentical)
				AfxFormatString2(msg, IDS_BINFILES_IDENTICAL, sLeftFile, sRightFile);
			else
				AfxFormatString2(msg, IDS_BINFILES_DIFFERENT, sLeftFile, sRightFile);
			msg += _T("\n\n");
			VERIFY(msg2.LoadString(IDS_FILEBINARY));
			msg += msg2;
			AfxMessageBox(msg, MB_ICONINFORMATION);
		}
		else if (nLeftSuccess == FRESULT_BINARY || nRightSuccess == FRESULT_BINARY)
		{
			// Other file binary, other text
			CString msg;
			CString msg2;
			if (nLeftSuccess == FRESULT_BINARY)
				AfxFormatString1(msg, IDS_OTHER_IS_BINARY, sLeftFile);
			else
				AfxFormatString1(msg, IDS_OTHER_IS_BINARY, sRightFile);

			AfxMessageBox(msg, MB_ICONSTOP);
		}
	}
	else
		ShowRescanError(nRescanResult, bBinary, bIdentical);
}

/**
 * @brief Refresh cached options.
 *
 * For compare speed, we have to cache some frequently needed options,
 * instead of getting option value every time from OptionsMgr. This
 * function must be called every time options are changed to OptionsMgr.
 */
void CMergeDoc::RefreshOptions()
{
	DIFFOPTIONS options = {0};
	
	m_diffWrapper.SetDetectMovedBlocks(mf->m_options.GetInt(OPT_CMP_MOVED_BLOCKS));
	options.nIgnoreWhitespace = mf->m_options.GetInt(OPT_CMP_IGNORE_WHITESPACE);
	options.bIgnoreBlankLines = mf->m_options.GetInt(OPT_CMP_IGNORE_BLANKLINES);
	options.bIgnoreCase = mf->m_options.GetInt(OPT_CMP_IGNORE_CASE);
	options.bEolSensitive = mf->m_options.GetInt(OPT_CMP_EOL_SENSITIVE);

	m_diffWrapper.SetOptions(&options);

	// Refresh view options
	m_pLeftView->RefreshOptions();
	m_pRightView->RefreshOptions();
}

/**
 * @brief Write path and filename to headerbar
 * @note SetText() does not repaint unchanged text
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
		if (m_nLeftBufferType == BUFFER_UNNAMED ||
			m_nLeftBufferType == BUFFER_NORMAL_NAMED)
		{
			sText = m_strLeftDesc;
		}
		else
			sText = m_strLeftFile;
		bChanges = m_ltBuf.IsModified();
		nPane = 0;
	}
	else
	{
		if (m_nRightBufferType == BUFFER_UNNAMED ||
			m_nRightBufferType == BUFFER_NORMAL_NAMED)
		{
			sText = m_strRightDesc;
		}
		else
			sText = m_strRightFile;
		bChanges = m_rtBuf.IsModified();
		nPane = 1;
	}

	if (bChanges)
		sText.Insert(0, _T("* "));

	pf->GetHeaderInterface()->SetText(nPane, sText);
}

/**
 * @brief Paint differently the headerbar of the active view
 */
void CMergeDoc::UpdateHeaderActivity(BOOL bLeft, BOOL bActivate)
{
	CChildFrame *pf = GetParentFrame();
	ASSERT(pf);
	int nPane = (bLeft) ? 0 : 1;
	pf->GetHeaderInterface()->SetActive(nPane, bActivate);
}

/**
 * @brief Return next diff from given line.
 * @param nLine [in] First line searched.
 * @param nDiff [out] Index of diff found.
 * @return TRUE if line is inside diff, FALSE otherwise.
 */
BOOL CMergeDoc::GetNextDiff(int nLine, int &nDiff)
{
	BOOL bInDiff = TRUE;
	int numDiff = LineToDiff(nLine);

	// Line not inside diff
	if (numDiff == -1)
	{
		bInDiff = FALSE;
		for (UINT i = 0; i < m_nDiffs; i++)
		{
			if ((int)m_diffs[i].dbegin0 >= nLine)
			{
				numDiff = i;
				break;
			}
		}
	}
	nDiff = numDiff;
	return bInDiff;
}

/**
 * @brief Returns copy of DIFFITEM from diff-list.
 * @param nDiff [in] Index of DIFFITEM to return.
 * @param di [out] DIFFITEM returned (empty if error)
 * @return TRUE if DIFFITEM found from given index.
 */
BOOL CMergeDoc::GetDiff(int nDiff, DIFFRANGE &di) const
{
	DIFFRANGE diff = {0};
	if (nDiff >= 0 && nDiff < m_nDiffs)
	{
		di = m_diffs[nDiff];
		return TRUE;
	}
	else
	{
		di = diff;
		return FALSE;
	}
}

/**
 * @brief Return if doc is in Merging/Editing mode
 */
BOOL CMergeDoc::GetMergingMode() const
{
	return m_bMergingMode;
}

/**
 * @brief Set doc to Merging/Editing mode
 */
void CMergeDoc::SetMergingMode(BOOL bMergingMode)
{
	m_bMergingMode = bMergingMode;
	mf->m_options.SaveOption(OPT_MERGE_MODE, m_bMergingMode);
}

/**
 * @brief Set detect/not detect Moved Blocks
 */
void CMergeDoc::SetDetectMovedBlocks(BOOL bDetectMovedBlocks)
{
	if (bDetectMovedBlocks == m_diffWrapper.GetDetectMovedBlocks())
		return;

	mf->m_options.SaveOption(OPT_CMP_MOVED_BLOCKS, bDetectMovedBlocks);
	m_diffWrapper.SetDetectMovedBlocks(bDetectMovedBlocks);
	FlushAndRescan();
}

void CMergeDoc::SetEditedAfterRescan(BOOL bLeft)
{
	if (bLeft)
		m_bLeftEditAfterRescan = TRUE;
	else
		m_bRightEditAfterRescan = TRUE;
}
