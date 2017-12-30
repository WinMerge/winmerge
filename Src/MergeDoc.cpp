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

#include "StdAfx.h"
#include "MergeDoc.h"
#include <cstdint>
#include <io.h>
#include <Poco/Timestamp.h>
#include "UnicodeString.h"
#include "Merge.h"
#include "MainFrm.h"
#include "DiffTextBuffer.h"
#include "Environment.h"
#include "MovedLines.h"
#include "MergeEditView.h"
#include "ChildFrm.h"
#include "DirDoc.h"
#include "files.h"
#include "FileTransform.h"
#include "unicoder.h"
#include "UniFile.h"
#include "OptionsDef.h"
#include "DiffFileInfo.h"
#include "SaveClosingDlg.h"
#include "DiffList.h"
#include "paths.h"
#include "OptionsMgr.h"
#include "OptionsDiffOptions.h"
#include "MergeLineFlags.h"
#include "FileOrFolderSelect.h"
#include "LineFiltersList.h"
#include "TempFile.h"
#include "codepage_detect.h"
#include "SelectUnpackerDlg.h"
#include "EncodingErrorBar.h"
#include "MergeCmdLineInfo.h"
#include "TFile.h"
#include "Constants.h"
#include "Merge7zFormatMergePluginImpl.h"
#include "7zCommon.h"
#include "PatchTool.h"
#include "charsets.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using std::swap;

/** @brief Max len of path in caption. */
static const UINT CAPTION_PATH_MAX = 50;

int CMergeDoc::m_nBuffersTemp = 2;

/** @brief EOL types */
static LPCTSTR crlfs[] =
{
	_T ("\x0d\x0a"), //  DOS/Windows style
	_T ("\x0a"),     //  UNIX style
	_T ("\x0d")      //  Macintosh style
};

static void SaveBuffForDiff(CDiffTextBuffer & buf, const String& filepath, bool bForceUTF8, int nStartLine = 0, int nLines = -1);

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc

IMPLEMENT_DYNCREATE(CMergeDoc, CDocument)

BEGIN_MESSAGE_MAP(CMergeDoc, CDocument)
	//{{AFX_MSG_MAP(CMergeDoc)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_LEFT, OnFileSaveLeft)
	ON_COMMAND(ID_FILE_SAVE_MIDDLE, OnFileSaveMiddle)
	ON_COMMAND(ID_FILE_SAVE_RIGHT, OnFileSaveRight)
	ON_COMMAND(ID_FILE_SAVEAS_LEFT, OnFileSaveAsLeft)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEAS_MIDDLE, OnUpdateFileSaveAsMiddle)
	ON_COMMAND(ID_FILE_SAVEAS_MIDDLE, OnFileSaveAsMiddle)
	ON_COMMAND(ID_FILE_SAVEAS_RIGHT, OnFileSaveAsRight)
	ON_UPDATE_COMMAND_UI(ID_STATUS_DIFFNUM, OnUpdateStatusNum)
	ON_UPDATE_COMMAND_UI(ID_STATUS_PLUGIN, OnUpdatePluginName)
	ON_COMMAND(ID_TOOLS_GENERATEREPORT, OnToolsGenerateReport)
	ON_COMMAND(ID_TOOLS_GENERATEPATCH, OnToolsGeneratePatch)
	ON_COMMAND(ID_RESCAN, OnFileReload)
	ON_UPDATE_COMMAND_UI(ID_RESCAN, OnUpdateFileReload)
	ON_COMMAND(ID_FILE_ENCODING, OnFileEncoding)
	ON_UPDATE_COMMAND_UI(ID_FILE_ENCODING, OnUpdateFileEncoding)
	ON_COMMAND_RANGE(ID_VIEW_DIFFCONTEXT_ALL, ID_VIEW_DIFFCONTEXT_TOGGLE, OnDiffContext)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_DIFFCONTEXT_ALL, ID_VIEW_DIFFCONTEXT_TOGGLE, OnUpdateDiffContext)
	ON_COMMAND(ID_POPUP_OPEN_WITH_UNPACKER, OnCtxtOpenWithUnpacker)
	ON_UPDATE_COMMAND_UI(ID_POPUP_OPEN_WITH_UNPACKER, OnUpdateCtxtOpenWithUnpacker)
	ON_BN_CLICKED(IDC_FILEENCODING, OnBnClickedFileEncoding)
	ON_BN_CLICKED(IDC_PLUGIN, OnBnClickedPlugin)
	ON_BN_CLICKED(IDC_HEXVIEW, OnBnClickedHexView)
	ON_COMMAND(IDOK, OnOK)
	ON_COMMAND(ID_MERGE_COMPARE_XML, OnFileRecompareAsXML)
	ON_COMMAND(ID_MERGE_COMPARE_HEX, OnFileRecompareAsBinary)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc construction/destruction

/**
 * @brief Constructor.
 */
CMergeDoc::CMergeDoc()
: m_bEnableRescan(true)
, m_nCurDiff(-1)
, m_pDirDoc(NULL)
, m_bMixedEol(false)
, m_pInfoUnpacker(new PackingInfo)
, m_pEncodingErrorBar(nullptr)
, m_bHasSyncPoints(false)
, m_bAutoMerged(false)
{
	DIFFOPTIONS options = {0};

	m_nBuffers = m_nBuffersTemp;
	m_filePaths.SetSize(m_nBuffers);

	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		m_ptBuf[nBuffer].reset(new CDiffTextBuffer(this, nBuffer));
		m_pSaveFileInfo[nBuffer].reset(new DiffFileInfo());
		m_pRescanFileInfo[nBuffer].reset(new DiffFileInfo());
		m_pView[nBuffer] = NULL;
		m_pDetailView[nBuffer] = NULL;
		m_nBufferType[nBuffer] = BUFFER_NORMAL;
		m_bEditAfterRescan[nBuffer] = false;
	}

	m_nCurDiff=-1;
	m_bEnableRescan = true;
	// COleDateTime m_LastRescan
	curUndo = undoTgt.begin();
	m_nDiffContext = GetOptionsMgr()->GetInt(OPT_DIFF_CONTEXT);

	m_diffWrapper.SetDetectMovedBlocks(GetOptionsMgr()->GetBool(OPT_CMP_MOVED_BLOCKS));
	Options::DiffOptions::Load(GetOptionsMgr(), options);

	m_diffWrapper.SetOptions(&options);
	m_diffWrapper.SetPrediffer(NULL);
}

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
}

/**
 * @brief Deleted data associated with doc before closing.
 */
void CMergeDoc::DeleteContents ()
{
	CDocument::DeleteContents ();
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		m_ptBuf[nBuffer]->FreeAll ();
	m_tempFiles[0].Delete();
	m_tempFiles[1].Delete();
}

/**
 * @brief Called when new document is created.
 *
 * Initialises buffers.
 */
BOOL CMergeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return false;

	SetTitle(_("File Comparison").c_str());
	
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		m_ptBuf[nBuffer]->InitNew ();
	return true;
}

/**
 * @brief Determines currently active view.
 * @return one of MERGEVIEW_INDEX_TYPE values or -1 in error.
 * @todo Detect location pane and return != 1 for it.
 */
int CMergeDoc::GetActiveMergeViewIndexType() const
{
	CMergeDoc * pThis = const_cast<CMergeDoc *>(this);
	// Get active view pointer
	CView * pActiveView = pThis->GetParentFrame()->GetActiveView();
	// Cast it to common base of all our views
	CCrystalTextView* curView = dynamic_cast<CCrystalTextView*> (pActiveView);
	// Now test it against all our views to see which it is
	if (curView == GetView(0))
		return MERGEVIEW_PANE0;
	else if (curView == GetView(1))
		return MERGEVIEW_PANE1;
	else if (m_nBuffers == 3 && curView == GetView(2))
		return MERGEVIEW_PANE2;
	else if (curView == GetDetailView(0))
		return MERGEVIEW_PANE0_DETAIL;
	else if (curView == GetDetailView(1))
		return MERGEVIEW_PANE1_DETAIL;
	else if (m_nBuffers == 3 && curView == GetDetailView(2))
		return MERGEVIEW_PANE2_DETAIL;

	// This assert fired when location pane caused refresh.
	// We can't detect location pane activity, so disable the assert.
	//_RPTF0(_CRT_ERROR, "Invalid view pointer!");
	return -1;
}

/**
 * @brief Return active merge edit view (or left one if neither active)
 */
CMergeEditView * CMergeDoc::GetActiveMergeView()
{
	CView * pActiveView = GetParentFrame()->GetActiveView();
	CMergeEditView * pMergeEditView = dynamic_cast<CMergeEditView *>(pActiveView);
	if (!pMergeEditView)
		pMergeEditView = GetView(0); // default to left view (in case some location or detail view active)
	return pMergeEditView;
}

void CMergeDoc::SetUnpacker(const PackingInfo * infoNewHandler)
{
	if (infoNewHandler)
	{
		*m_pInfoUnpacker = *infoNewHandler;
	}
}

void CMergeDoc::SetPrediffer(const PrediffingInfo * infoPrediffer)
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
 *   buffer  -> save as UTF-8 -> Unicode plugins -> convert to UTF-8 -> diffutils
 * (the plugins are optional, not the conversion)
 * @todo Show SaveToFile() errors?
 */
static void SaveBuffForDiff(CDiffTextBuffer & buf, const String& filepath, bool bForceUTF8, int nStartLine, int nLines)
{
	ASSERT(buf.m_nSourceEncoding == buf.m_nDefaultEncoding);  
	int orig_codepage = buf.getCodepage();
	ucr::UNICODESET orig_unicoding = buf.getUnicoding();
	bool orig_bHasBOM = buf.getHasBom();

	// If file was in Unicode
	if (orig_unicoding != ucr::NONE || bForceUTF8)
	{
	// we subvert the buffer's memory of the original file encoding
		buf.setUnicoding(ucr::UTF8);  // write as UTF-8 (for preprocessing)
		buf.setCodepage(ucr::CP_UTF_8); // should not matter
		buf.setHasBom(false);
	}

	// and we don't repack the file
	PackingInfo * tempPacker = NULL;

	// write buffer out to temporary file
	String sError;
	int retVal = buf.SaveToFile(filepath, true, sError, tempPacker,
		CRLF_STYLE_AUTOMATIC, false, nStartLine, nLines);

	// restore memory of encoding of original file
	buf.setUnicoding(orig_unicoding);
	buf.setCodepage(orig_codepage);
	buf.setHasBom(orig_bHasBOM);
}

/**
 * @brief Save files to temp files & compare again.
 *
 * @param bBinary [in,out] [in] If true, compare two binary files
 * [out] If true binary file was detected.
 * @param bIdentical [out] If true files were identical
 * @param bForced [in] If true, suppressing is ignored and rescan
 * is done always
 * @return Tells if rescan was successfully, was suppressed, or
 * error happened
 * If this code is OK, Rescan has detached the views temporarily
 * (positions of cursors have been lost)
 * @note Rescan() ALWAYS compares temp files. Actual user files are not
 * touched by Rescan().
 * @sa CDiffWrapper::RunFileDiff()
 */
int CMergeDoc::Rescan(bool &bBinary, IDENTLEVEL &identical,
		bool bForced /* =false */)
{
	DIFFOPTIONS diffOptions = {0};
	DiffFileInfo fileInfo;
	bool diffSuccess = false;
	int nResult = RESCAN_OK;
	FileChange FileChanged[3] = {FileNoChange, FileNoChange, FileNoChange};
	int nBuffer;

	if (!bForced)
	{
		if (!m_bEnableRescan)
			return RESCAN_SUPPRESSED;
	}

	ClearWordDiffCache();

	if (GetOptionsMgr()->GetBool(OPT_LINEFILTER_ENABLED))
	{
		m_diffWrapper.SetFilterList(theApp.m_pLineFilters->GetAsString());
	}
	else
	{
		m_diffWrapper.SetFilterList(_T(""));
	}
	m_diffWrapper.SetFilterCommentsManager(theApp.m_pFilterCommentsManager.get());

	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		// Check if files have been modified since last rescan
		// Ignore checking in case of scratchpads (empty filenames)
		if (!m_filePaths[nBuffer].empty())
		{
			FileChanged[nBuffer] = IsFileChangedOnDisk(m_filePaths[nBuffer].c_str(),
					fileInfo, false, nBuffer);
		}
	}
	m_LastRescan = COleDateTime::GetCurrentTime();

	LPCTSTR tnames[] = {_T("t0_wmdoc"), _T("t1_wmdoc"), _T("t2_wmdoc")};
	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		if (FileChanged[nBuffer] == FileRemoved)
		{
			String msg = strutils::format_string1(_("The file\n%1\nhas disappeared. Please save a copy of the file to continue."), m_filePaths[nBuffer]);
			AfxMessageBox(msg.c_str(), MB_ICONWARNING);
			bool bSaveResult = false;
			bool ok = DoSaveAs(m_filePaths[nBuffer].c_str(), bSaveResult, nBuffer);
			if (!ok || !bSaveResult)
			{
				return RESCAN_FILE_ERR;
			}
		}

		String temp = m_tempFiles[nBuffer].GetPath();
		if (temp.empty())
		{
			temp = m_tempFiles[nBuffer].CreateFromFile(m_filePaths.GetPath(nBuffer),
				tnames[nBuffer]);
		}
		if (temp.empty())
			return RESCAN_TEMP_ERR;
	}

	CheckFileChanged();

	String tempPath = env::GetTemporaryPath();

	// Set up DiffWrapper
	m_diffWrapper.GetOptions(&diffOptions);

	bool bForceUTF8 = diffOptions.bIgnoreCase;
	IF_IS_TRUE_ALL (
		m_ptBuf[0]->getCodepage() == m_ptBuf[nBuffer]->getCodepage() && m_ptBuf[nBuffer]->getUnicoding() == ucr::NONE,
		nBuffer, m_nBuffers) {}
	else
		bForceUTF8 = true;

	// Clear diff list
	m_diffList.Clear();
	m_nCurDiff = -1;
	// Clear moved lines lists
	if (m_diffWrapper.GetDetectMovedBlocks())
	{
		for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
			m_diffWrapper.GetMovedLines(nBuffer)->Clear();
	}

	// Set paths for diffing and run diff
	m_diffWrapper.EnablePlugins(GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED));
	if (m_nBuffers < 3)
		m_diffWrapper.SetPaths(PathContext(m_tempFiles[0].GetPath(), m_tempFiles[1].GetPath()), true);
	else
		m_diffWrapper.SetPaths(PathContext(m_tempFiles[0].GetPath(), m_tempFiles[1].GetPath(), m_tempFiles[2].GetPath()), true);
	m_diffWrapper.SetCompareFiles(m_filePaths);
	m_diffWrapper.SetCodepage(bForceUTF8 ? ucr::CP_UTF_8 : (m_ptBuf[0]->m_encoding.m_unicoding ? CP_UTF8 : m_ptBuf[0]->m_encoding.m_codepage));
	m_diffWrapper.SetCodepage(m_ptBuf[0]->m_encoding.m_unicoding ?
			CP_UTF8 : m_ptBuf[0]->m_encoding.m_codepage);

	DIFFSTATUS status;

	if (!HasSyncPoints())
	{
		// Save text buffer to file
		for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			m_ptBuf[nBuffer]->SetTempPath(tempPath);
			SaveBuffForDiff(*m_ptBuf[nBuffer], m_tempFiles[nBuffer].GetPath(), bForceUTF8);
		}

		m_diffWrapper.SetCreateDiffList(&m_diffList);
		diffSuccess = !!m_diffWrapper.RunFileDiff();

		// Read diff-status
		m_diffWrapper.GetDiffStatus(&status);
		if (bBinary) // believe caller if we were told these are binaries
			status.bBinaries = true;
	}
	else
	{
		const std::vector<std::vector<int> > syncpoints = GetSyncPointList();	
		int nStartLine[3] = {0};
		int nLines[3], nRealLine[3];
		for (size_t i = 0; i <= syncpoints.size(); ++i)
		{
			// Save text buffer to file
			for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
			{
				nLines[nBuffer] = (i >= syncpoints.size()) ? -1 : syncpoints[i][nBuffer] - nStartLine[nBuffer];
				m_ptBuf[nBuffer]->SetTempPath(tempPath);
				SaveBuffForDiff(*m_ptBuf[nBuffer], m_tempFiles[nBuffer].GetPath(), bForceUTF8,
					nStartLine[nBuffer], nLines[nBuffer]);
			}
			DiffList templist;
			templist.Clear();
			m_diffWrapper.SetCreateDiffList(&templist);
			diffSuccess = !!m_diffWrapper.RunFileDiff();
			for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
				nRealLine[nBuffer] = m_ptBuf[nBuffer]->ComputeRealLine(nStartLine[nBuffer]);
			m_diffList.AppendDiffList(templist, nRealLine);
			for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
				nStartLine[nBuffer] += nLines[nBuffer];

			// Read diff-status
			DIFFSTATUS status_part;
			m_diffWrapper.GetDiffStatus(&status_part);
			if (bBinary) // believe caller if we were told these are binaries
				status.bBinaries = true;
			status.MergeStatus(status_part);
		}
		m_diffWrapper.SetCreateDiffList(&m_diffList);
	}

	// If comparing whitespaces and
	// other file has EOL before EOF and other not...
	if (!diffOptions.nIgnoreWhitespace && !diffOptions.bIgnoreBlankLines)
	{
		if (std::count(status.bMissingNL, status.bMissingNL + m_nBuffers, status.bMissingNL[0]) < m_nBuffers)
		{
			// ..lasf DIFFRANGE of file which has EOL must be
			// fixed to contain last line too
			int lineCount[3];
			for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
				lineCount[nBuffer] = m_ptBuf[nBuffer]->GetLineCount();
			m_diffWrapper.FixLastDiffRange(m_nBuffers, lineCount, status.bMissingNL, diffOptions.bIgnoreBlankLines);
		}
	}

	// set identical/diff result as recorded by diffutils
	identical = status.Identical;

	// Determine errors and binary file compares
	if (!diffSuccess)
		nResult = RESCAN_FILE_ERR;
	else if (status.bBinaries)
	{
		bBinary = true;
	}
	else
	{
		// Now update views and buffers for ghost lines

		// Prevent displaying views during this update 
		// BTW, this solves the problem of double asserts
		// (during the display of an assert message box, a second assert in one of the 
		//  display functions happens, and hides the first assert)
		for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			m_pView[nBuffer]->DetachFromBuffer();
			m_pDetailView[nBuffer]->DetachFromBuffer();
		}

		// Remove blank lines and clear winmerge flags
		// this operation does not change the modified flag
		for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
			m_ptBuf[nBuffer]->prepareForRescan();

		// Divide diff blocks to match lines.
		if (GetOptionsMgr()->GetBool(OPT_CMP_MATCH_SIMILAR_LINES) && m_nBuffers < 3)
			AdjustDiffBlocks();

		// Analyse diff-list (updating real line-numbers)
		// this operation does not change the modified flag
		PrimeTextBuffers();

		// Hide identical lines if diff-context is not 'All'
		HideLines();

		// Apply flags to lines that moved, to differentiate from appeared/disappeared lines
		if (m_diffWrapper.GetDetectMovedBlocks())
			FlagMovedLines();
		
		// After PrimeTextBuffers() we know amount of real diffs
		// (m_nDiffs) and trivial diffs (m_nTrivialDiffs)

		// Identical files are also updated
		if (!m_diffList.HasSignificantDiffs())
			identical = IDENTLEVEL_ALL;

		for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			// just apply some options to the views
			m_pView[nBuffer]->PrimeListWithFile();
			m_pDetailView[nBuffer]->PrimeListWithFile();

			// Now buffers data are valid
			m_pView[nBuffer]->ReAttachToBuffer();
			m_pDetailView[nBuffer]->ReAttachToBuffer();

			m_bEditAfterRescan[nBuffer] = false;
		}
	}

	if (!GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CODEPAGE) &&
		identical == IDENTLEVEL_ALL &&
		std::any_of(m_ptBuf, m_ptBuf + m_nBuffers,
			[&](std::unique_ptr<CDiffTextBuffer>& buf) { return buf->getEncoding() != m_ptBuf[0]->getEncoding(); }))
		identical = IDENTLEVEL_NONE;

	GetParentFrame()->SetLastCompareResult(identical != IDENTLEVEL_ALL ? 1 : 0);

	return nResult;
}

void CMergeDoc::CheckFileChanged(void)
{
	int nBuffer;
	DiffFileInfo fileInfo;
	FileChange FileChange[3];

	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		FileChange[nBuffer] = IsFileChangedOnDisk(m_filePaths[nBuffer].c_str(), fileInfo,
			false, nBuffer);

		m_pRescanFileInfo[nBuffer]->Update((LPCTSTR)m_filePaths[nBuffer].c_str());
	}

	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		if (FileChange[nBuffer] == FileChanged)
		{
			String msg = strutils::format_string1(_("Another application has updated file\n%1\nsince WinMerge scanned it last time.\n\nDo you want to reload the file?"), m_filePaths[nBuffer]);
			if (AfxMessageBox(msg.c_str(), MB_YESNO | MB_ICONWARNING) == IDYES)
			{
				OnFileReload();
			}
			break;
		}
	}
}


/** @brief Adjust all different lines that were detected as actually matching moved lines */
void CMergeDoc::FlagMovedLines(void)
{
	int i;
	MovedLines *pMovedLines;

	pMovedLines = m_diffWrapper.GetMovedLines(0);
	for (i = 0; i < m_ptBuf[0]->GetLineCount(); ++i)
	{
		int j = pMovedLines->LineInBlock(i, MovedLines::SIDE_RIGHT);
		if (j != -1)
		{
			TRACE(_T("%d->%d\n"), i, j);
			ASSERT(j>=0);
			// We only flag lines that are already marked as being different
			int apparent = m_ptBuf[0]->ComputeApparentLine(i);
			if (m_ptBuf[0]->FlagIsSet(apparent, LF_DIFF))
			{
				m_ptBuf[0]->SetLineFlag(apparent, LF_MOVED, true, false, false);
			}
		}
	}

	pMovedLines = m_diffWrapper.GetMovedLines(1);
	for (i=0; i<m_ptBuf[1]->GetLineCount(); ++i)
	{
		int j = pMovedLines->LineInBlock(i, MovedLines::SIDE_LEFT);
		if (j != -1)
		{
			TRACE(_T("%d->%d\n"), i, j);
			ASSERT(j>=0);
			// We only flag lines that are already marked as being different
			int apparent = m_ptBuf[1]->ComputeApparentLine(i);
			if (m_ptBuf[1]->FlagIsSet(apparent, LF_DIFF))
			{
				m_ptBuf[1]->SetLineFlag(apparent, LF_MOVED, true, false, false);
			}
		}
	}

	if (m_nBuffers < 3)
		return;

	pMovedLines = m_diffWrapper.GetMovedLines(1);
	for (i=0; i<m_ptBuf[1]->GetLineCount(); ++i)
	{
		int j = pMovedLines->LineInBlock(i, MovedLines::SIDE_RIGHT);
		if (j != -1)
		{
			TRACE(_T("%d->%d\n"), i, j);
			ASSERT(j>=0);
			// We only flag lines that are already marked as being different
			int apparent = m_ptBuf[1]->ComputeApparentLine(i);
			if (m_ptBuf[1]->FlagIsSet(apparent, LF_DIFF))
			{
				m_ptBuf[1]->SetLineFlag(apparent, LF_MOVED, true, false, false);
			}
		}
	}

	pMovedLines = m_diffWrapper.GetMovedLines(2);
	for (i=0; i<m_ptBuf[2]->GetLineCount(); ++i)
	{
		int j = pMovedLines->LineInBlock(i, MovedLines::SIDE_LEFT);
		if (j != -1)
		{
			TRACE(_T("%d->%d\n"), i, j);
			ASSERT(j>=0);
			// We only flag lines that are already marked as being different
			int apparent = m_ptBuf[2]->ComputeApparentLine(i);
			if (m_ptBuf[2]->FlagIsSet(apparent, LF_DIFF))
			{
				m_ptBuf[2]->SetLineFlag(apparent, LF_MOVED, true, false, false);
			}
		}
	}

	// todo: Need to record actual moved information
}

/**
 * @brief Prints (error) message by rescan status.
 *
 * @param nRescanResult [in] Resultcocode from rescan().
 * @param bIdentical [in] Were files identical?.
 * @sa CMergeDoc::Rescan()
 */
void CMergeDoc::ShowRescanError(int nRescanResult, IDENTLEVEL identical)
{
	// Rescan was suppressed, there is no sensible status
	if (nRescanResult == RESCAN_SUPPRESSED)
		return;

	String s;

	if (nRescanResult == RESCAN_FILE_ERR)
	{
		s = _("An error occurred while comparing the files.");
		LogErrorString(s);
		AfxMessageBox(s.c_str(), MB_ICONSTOP);
		return;
	}

	if (nRescanResult == RESCAN_TEMP_ERR)
	{
		s = _("Temporary files could not be created. Check your temporary path settings.");
		LogErrorString(s);
		AfxMessageBox(s.c_str(), MB_ICONSTOP);
		return;
	}

	// Files are not binaries, but they are identical
	if (identical != IDENTLEVEL_NONE)
	{
		if (!m_filePaths.GetLeft().empty() && !m_filePaths.GetMiddle().empty() && !m_filePaths.GetRight().empty() && 
			m_filePaths.GetLeft() == m_filePaths.GetRight() && m_filePaths.GetMiddle() == m_filePaths.GetRight())
		{
			// compare file to itself, a custom message so user may hide the message in this case only
			s = _("The same file is opened in both panels.");
			AfxMessageBox(s.c_str(), MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN, IDS_FILE_TO_ITSELF);
		}
		else if (identical == IDENTLEVEL_ALL)
		{
			UINT nFlags = MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN;

			if (theApp.m_bExitIfNoDiff == MergeCmdLineInfo::Exit)
			{
				// Show the "files are identical" for basic "exit no diff" flag
				// If user don't want to see the message one uses the quiet version
				// of the "exit no diff".
				nFlags &= ~MB_DONT_DISPLAY_AGAIN;
			}

			if (theApp.m_bExitIfNoDiff != MergeCmdLineInfo::ExitQuiet)
			{
				GetMainFrame()->SendMessageToDescendants(WM_IDLEUPDATECMDUI, (WPARAM)TRUE, 0, TRUE, TRUE);
				LangMessageBox(IDS_FILESSAME, nFlags);
			}

			// Exit application if files are identical.
			if (theApp.m_bExitIfNoDiff == MergeCmdLineInfo::Exit ||
				theApp.m_bExitIfNoDiff == MergeCmdLineInfo::ExitQuiet)
			{
				AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT);
			}
		}
	}
}

bool CMergeDoc::Undo()
{
	return false;
}

/**
 * @brief An instance of RescanSuppress prevents rescan during its lifetime
 * (or until its Clear method is called, which ends its effect).
 */
class RescanSuppress
{
public:
	explicit RescanSuppress(CMergeDoc & doc) : m_doc(doc)
	{
		m_bSuppress = true;
		m_bPrev = doc.m_bEnableRescan;
		m_doc.m_bEnableRescan = false;
	}
	void Clear() 
	{
		if (m_bSuppress)
		{
			m_bSuppress = false;
			m_doc.m_bEnableRescan = m_bPrev;
		}
	}
	~RescanSuppress()
	{
		Clear();
	}
private:
	CMergeDoc & m_doc;
	bool m_bPrev;
	bool m_bSuppress;
};

/**
 * @brief Copy all diffs from one side to side.
 * @param [in] srcPane Source side from which diff is copied
 * @param [in] dstPane Destination side
 */
void CMergeDoc::CopyAllList(int srcPane, int dstPane)
{
	CopyMultipleList(srcPane, dstPane, 0, m_diffList.GetSize() - 1);
}

/**
 * @brief Copy range of diffs from one side to side.
 * This function copies given range of differences from side to another.
 * Ignored differences are skipped, and not copied.
 * @param [in] srcPane Source side from which diff is copied
 * @param [in] dstPane Destination side
 * @param [in] firstDiff First diff copied (0-based index)
 * @param [in] lastDiff Last diff copied (0-based index)
 */
void CMergeDoc::CopyMultipleList(int srcPane, int dstPane, int firstDiff, int lastDiff)
{
#ifdef _DEBUG
	if (firstDiff > lastDiff)
		_RPTF0(_CRT_ERROR, "Invalid diff range (firstDiff > lastDiff)!");
	if (firstDiff < 0)
		_RPTF0(_CRT_ERROR, "Invalid diff range (firstDiff < 0)!");
	if (lastDiff > m_diffList.GetSize() - 1)
		_RPTF0(_CRT_ERROR, "Invalid diff range (lastDiff < diffcount)!");
#endif

	lastDiff = min(m_diffList.GetSize() - 1, lastDiff);
	firstDiff = max(0, firstDiff);
	if (firstDiff > lastDiff)
		return;
	
	RescanSuppress suppressRescan(*this);

	// Note we don't care about m_nDiffs count to become zero,
	// because we don't rescan() so it does not change

	SetCurrentDiff(lastDiff);

	bool bGroupWithPrevious = false;
	if (!ListCopy(srcPane, dstPane, -1, bGroupWithPrevious, true))
		return; // sync failure

	SetEditedAfterRescan(dstPane);

	CPoint currentPosSrc = m_pView[srcPane]->GetCursorPos();
	currentPosSrc.x = 0;
	CPoint currentPosDst = m_pView[dstPane]->GetCursorPos();
	currentPosDst.x = 0;

	CPoint pt(0, 0);
	m_pView[dstPane]->SetCursorPos(pt);
	m_pView[dstPane]->SetNewSelection(pt, pt, false);
	m_pView[dstPane]->SetNewAnchor(pt);

	// copy from bottom up is more efficient
	for (int i = lastDiff - 1; i >= firstDiff; --i)
	{
		if (m_diffList.IsDiffSignificant(i))
		{
			SetCurrentDiff(i);
			const DIFFRANGE *pdi = m_diffList.DiffRangeAt(i);
			if (currentPosDst.y > pdi->dend)
			{
				if (pdi->blank[dstPane] >= 0)
					currentPosDst.y -= pdi->dend - pdi->blank[dstPane] + 1;
				else if (pdi->blank[srcPane] >= 0)
					currentPosDst.y -= pdi->dend - pdi->blank[srcPane] + 1;
			}			
			// Group merge with previous (merge undo data to one action)
			bGroupWithPrevious = true;
			if (!ListCopy(srcPane, dstPane, -1, bGroupWithPrevious, false))
				break; // sync failure
		}
	}

	m_pView[dstPane]->SetCursorPos(currentPosDst);
	m_pView[dstPane]->SetNewSelection(currentPosDst, currentPosDst, false);
	m_pView[dstPane]->SetNewAnchor(currentPosDst);
	m_pDetailView[dstPane]->SetCursorPos(currentPosDst);
	m_pDetailView[dstPane]->SetNewSelection(currentPosDst, currentPosDst, false);
	m_pDetailView[dstPane]->SetNewAnchor(currentPosDst);

	suppressRescan.Clear(); // done suppress Rescan
	FlushAndRescan();
}

enum MergeResult { NoMergeNeeded, Merged, Conflict };

template<class Type>
static std::pair<MergeResult, Type> DoMergeValue(Type left, Type middle, Type right, int dstPane)
{
	bool equal_all = middle == left && middle == right && left == right;
	if (equal_all)
		return std::pair<MergeResult, Type>(NoMergeNeeded, left);
	bool conflict = middle != left && middle != right && left != right;
	if (conflict)
		return std::pair<MergeResult, Type>(Conflict, left);
	switch (dstPane)
	{
	case 0:
		if (left == right)
			return std::pair<MergeResult, Type>(Merged, middle);
		break;
	case 1:
		if (left == middle)
			return std::pair<MergeResult, Type>(Merged, right);
		else
			return std::pair<MergeResult, Type>(Merged, left);
		break;
	case 2:
		if (left == right)
			return std::pair<MergeResult, Type>(Merged, middle);
		break;
	}
	return std::pair<MergeResult, Type>(NoMergeNeeded, left);
}

/**
 * @brief Do auto-merge.
 * @param [in] dstPane Destination side
 */
void CMergeDoc::DoAutoMerge(int dstPane)
{
	const int lastDiff = m_diffList.GetSize() - 1;
	const int firstDiff = 0;
	bool bGroupWithPrevious = false;
	int autoMergedCount = 0;
	int unresolvedConflictCount = 0;

	std::pair<MergeResult, FileTextEncoding> mergedEncoding = 
		DoMergeValue(m_ptBuf[0]->getEncoding(), m_ptBuf[1]->getEncoding(), m_ptBuf[2]->getEncoding(), dstPane);
	if (mergedEncoding.first == Merged)
	{
		LangMessageBox(IDS_CODEPAGE_MERGED, MB_ICONINFORMATION);
		m_ptBuf[dstPane]->setEncoding(mergedEncoding.second);
	}
	else if (mergedEncoding.first == Conflict)
		LangMessageBox(IDS_CODEPAGE_CONFLICT, MB_ICONINFORMATION);

	std::pair<MergeResult, CRLFSTYLE> mergedEOLStyle = 
		DoMergeValue(m_ptBuf[0]->GetCRLFMode(), m_ptBuf[1]->GetCRLFMode(), m_ptBuf[2]->GetCRLFMode(), dstPane);
	if (mergedEOLStyle.first == Merged)
	{
		LangMessageBox(IDS_EOL_MERGED, MB_ICONINFORMATION);
		m_ptBuf[dstPane]->SetCRLFMode(mergedEOLStyle.second);
	}
	else if (mergedEOLStyle.first == Conflict)
		LangMessageBox(IDS_EOL_CONFLICT, MB_ICONINFORMATION);

	RescanSuppress suppressRescan(*this);

	// Note we don't care about m_nDiffs count to become zero,
	// because we don't rescan() so it does not change

	SetCurrentDiff(lastDiff);

	SetEditedAfterRescan(dstPane);

	CPoint currentPosDst = m_pView[dstPane]->GetCursorPos();
	currentPosDst.x = 0;

	CPoint pt(0, 0);
	m_pView[dstPane]->SetCursorPos(pt);
	m_pView[dstPane]->SetNewSelection(pt, pt, false);
	m_pView[dstPane]->SetNewAnchor(pt);

	// copy from bottom up is more efficient
	for (int i = lastDiff; i >= firstDiff; --i)
	{
		const DIFFRANGE *pdi = m_diffList.DiffRangeAt(i);
		const int srcPane = m_diffList.GetMergeableSrcIndex(i, dstPane);
		if (srcPane != -1)
		{
			SetCurrentDiff(i);
			if (currentPosDst.y > pdi->dend)
			{
				if (pdi->blank[dstPane] >= 0)
					currentPosDst.y -= pdi->dend - pdi->blank[dstPane] + 1;
				else if (pdi->blank[srcPane] >= 0)
					currentPosDst.y -= pdi->dend - pdi->blank[srcPane] + 1;
			}			
			// Group merge with previous (merge undo data to one action)
			if (!ListCopy(srcPane, dstPane, -1, bGroupWithPrevious, false))
				break; // sync failure
			if (!bGroupWithPrevious)
				bGroupWithPrevious = true;
			++autoMergedCount;
		}
		if (pdi->op == OP_DIFF)
			++unresolvedConflictCount;
	}

	m_pView[dstPane]->SetCursorPos(currentPosDst);
	m_pView[dstPane]->SetNewSelection(currentPosDst, currentPosDst, false);
	m_pView[dstPane]->SetNewAnchor(currentPosDst);
	m_pDetailView[dstPane]->SetCursorPos(currentPosDst);
	m_pDetailView[dstPane]->SetNewSelection(currentPosDst, currentPosDst, false);
	m_pDetailView[dstPane]->SetNewAnchor(currentPosDst);

	suppressRescan.Clear(); // done suppress Rescan
	FlushAndRescan();
	UpdateHeaderPath(dstPane);

	if (autoMergedCount > 0)
		m_bAutoMerged = true;

	// move to first conflict 
	const int nDiff = m_diffList.FirstSignificant3wayDiff(THREEWAYDIFFTYPE_CONFLICT);
	if (nDiff != -1)
		m_pView[dstPane]->SelectDiff(nDiff, true, false);

	AfxMessageBox(
		strutils::format_string2(
			_T("The number of automatically merged changes: %1\nThe number of unresolved conflicts: %2"), 
			strutils::format(_T("%d"), autoMergedCount),
			strutils::format(_T("%d"), unresolvedConflictCount)).c_str(),
		MB_ICONINFORMATION);
}

/**
 * @brief Sanity check difference.
 *
 * Checks that lines in difference are inside difference in both files.
 * If file is edited, lines added or removed diff lines get out of sync and
 * merging fails miserably.
 *
 * @param [in] dr Difference to check.
 * @return true if difference lines match, false otherwise.
 */
bool CMergeDoc::SanityCheckDiff(DIFFRANGE dr) const
{
	const int cd_dbegin = dr.dbegin;
	const int cd_dend = dr.dend;

	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		// Must ensure line number is in range before getting line flags
		if (cd_dend >= m_ptBuf[nBuffer]->GetLineCount())
			return false;

		// Optimization - check last line first so we don't need to
		// check whole diff for obvious cases
		DWORD dwFlags = m_ptBuf[nBuffer]->GetLineFlags(cd_dend);
		if (!(dwFlags & LF_WINMERGE_FLAGS))
			return false;
	}

	for (int line = cd_dbegin; line < cd_dend; line++)
	{
		for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			DWORD dwFlags = m_ptBuf[nBuffer]->GetLineFlags(cd_dend);
			if (!(dwFlags & LF_WINMERGE_FLAGS))
				return false;
		}
	}
	return true;
}

/**
 * @brief Copy selected (=current) difference from from side to side.
 * @param [in] srcPane Source side from which diff is copied
 * @param [in] dstPane Destination side
 * @param [in] nDiff Diff to copy, if -1 function determines it.
 * @param [in] bGroupWithPrevious Adds diff to same undo group with
 * @return true if ok, false if sync failure & need to abort copy
 * previous action (allows one undo for copy all)
 */
bool CMergeDoc::ListCopy(int srcPane, int dstPane, int nDiff /* = -1*/,
		bool bGroupWithPrevious /*= false*/, bool bUpdateView /*= true*/)
{
	CCrystalTextView *pSource = bUpdateView ? m_pView[dstPane] : NULL;

	// suppress Rescan during this method
	// (Not only do we not want to rescan a lot of times, but
	// it will wreck the line status array to rescan as we merge)
	RescanSuppress suppressRescan(*this);

	// If diff-number not given, determine it from active view
	if (nDiff == -1)
	{
		nDiff = GetCurrentDiff();

		// No current diff, but maybe cursor is in diff?
		if (nDiff == -1 && (m_pView[srcPane]->IsCursorInDiff() ||
			m_pView[dstPane]->IsCursorInDiff()))
		{
			// Find out diff under cursor
			CPoint ptCursor;
			int nActiveViewIndexType = GetActiveMergeViewIndexType();
			if (nActiveViewIndexType >= MERGEVIEW_PANE0 && nActiveViewIndexType <= MERGEVIEW_PANE2)
				ptCursor = m_pView[nActiveViewIndexType]->GetCursorPos();
			else if (nActiveViewIndexType >= MERGEVIEW_PANE0_DETAIL ||
					nActiveViewIndexType <= MERGEVIEW_PANE2_DETAIL)
			{
				ptCursor = m_pView[nActiveViewIndexType - MERGEVIEW_PANE0_DETAIL]->GetCursorPos();
			}
			nDiff = m_diffList.LineToDiff(ptCursor.y);
		}
	}

	if (nDiff != -1)
	{
		DIFFRANGE cd;
		VERIFY(m_diffList.GetDiff(nDiff, cd));
		CDiffTextBuffer& sbuf = *m_ptBuf[srcPane];
		CDiffTextBuffer& dbuf = *m_ptBuf[dstPane];
		bool bSrcWasMod = sbuf.IsModified();
		const int cd_dbegin = cd.dbegin;
		const int cd_dend = cd.dend;
		const int cd_blank = cd.blank[srcPane];
		bool bInSync = SanityCheckDiff(cd);

		if (!bInSync)
		{
			LangMessageBox(IDS_VIEWS_OUTOFSYNC, MB_ICONSTOP);
			return false; // abort copying
		}

		// If we remove whole diff from current view, we must fix cursor
		// position first. Normally we would move to end of previous line,
		// but we want to move to begin of that line for usability.
		if (bUpdateView)
		{
			CPoint currentPos = m_pView[dstPane]->GetCursorPos();
			currentPos.x = 0;
			if (currentPos.y > cd_dend)
			{
				if (cd.blank[dstPane] >= 0)
					currentPos.y -= cd_dend - cd.blank[dstPane] + 1;
				else if (cd.blank[srcPane] >= 0)
					currentPos.y -= cd_dend - cd.blank[srcPane] + 1;
			}
			m_pView[dstPane]->SetCursorPos(currentPos);
			m_pDetailView[dstPane]->SetCursorPos(currentPos);
		}

		// if the current diff contains missing lines, remove them from both sides
		int limit = cd_dend;

		// curView is the view which is changed, so the opposite of the source view
		dbuf.BeginUndoGroup(bGroupWithPrevious);
		if (cd_blank>=0)
		{
			// text was missing, so delete rest of lines on both sides
			// delete only on destination side since rescan will clear the other side
			if (cd_dend + 1 < dbuf.GetLineCount())
			{
				dbuf.DeleteText(pSource, cd_blank, 0, cd_dend+1, 0, CE_ACTION_MERGE);
			}
			else
			{
				// To removing EOL chars of last line, deletes from the end of the line (cd_blank - 1).
				ASSERT(cd_blank > 0);
				dbuf.DeleteText(pSource, cd_blank-1, dbuf.GetLineLength(cd_blank-1), cd_dend, dbuf.GetLineLength(cd_dend), CE_ACTION_MERGE);
			}

			limit=cd_blank-1;
			dbuf.FlushUndoGroup(pSource);
			dbuf.BeginUndoGroup(true);
		}


		// copy the selected text over
		if (cd_dbegin <= limit)
		{
			// text exists on left side, so just replace
			dbuf.ReplaceFullLines(dbuf, sbuf, pSource, cd_dbegin, limit, CE_ACTION_MERGE);
			dbuf.FlushUndoGroup(pSource);
			dbuf.BeginUndoGroup(true);
		}
		dbuf.FlushUndoGroup(pSource);

		// remove the diff
		SetCurrentDiff(-1);

		// reset the mod status of the source view because we do make some
		// changes, but none that concern the source text
		sbuf.SetModified(bSrcWasMod);
	}

	suppressRescan.Clear(); // done suppress Rescan
	FlushAndRescan();
	return true;
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
 * @param [in] nBuffer Buffer we are saving
 * @return false as long as the user is not satisfied. Calling function
 * should not continue until true is returned.
 * @sa CMergeDoc::DoSave()
 * @sa CMergeDoc::DoSaveAs()
 * @sa CMergeDoc::CDiffTextBuffer::SaveToFile()
 */
bool CMergeDoc::TrySaveAs(String &strPath, int &nSaveResult, String & sError,
	int nBuffer, PackingInfo * pInfoTempUnpacker)
{
	String s;
	String str;
	String strSavePath; // New path for next saving try
	String title;
	bool result = true;
	int answer = IDOK; // Set default we use for scratchpads
	int nActiveViewIndexType = GetActiveMergeViewIndexType();

	if (nActiveViewIndexType == -1)
	{
		// We don't have valid view active, but still tried to save.
		// We don't know which file to save, so just cancel.
		// Possible origin in location pane?
		_RPTF0(_CRT_ERROR, "Save request from invalid view!");
		nSaveResult = SAVE_CANCELLED;
		return true;
	}

	HWND parent = m_pView[nActiveViewIndexType]->GetSafeHwnd();

	// We shouldn't get here if saving is succeed before
	ASSERT(nSaveResult != SAVE_DONE);

	// Select message based on reason function called
	if (nSaveResult == SAVE_PACK_FAILED)
	{
		if (m_nBuffers == 3)
		{
			str = strutils::format_string2(
				nBuffer == 0 ? 
					_("Plugin '%2' cannot pack your changes to the left file back into '%1'.\n\nThe original file will not be changed.\n\nDo you want to save the unpacked version to another file?")
					: (nBuffer == 1 ? 
					_("Plugin '%2' cannot pack your changes to the middle file back into '%1'.\n\nThe original file will not be changed.\n\nDo you want to save the unpacked version to another file?"): 
					_("Plugin '%2' cannot pack your changes to the right file back into '%1'.\n\nThe original file will not be changed.\n\nDo you want to save the unpacked version to another file?")),
				strPath, pInfoTempUnpacker->pluginName);
		}
		else
		{
			str = strutils::format_string2(nBuffer == 0 ? _("Plugin '%2' cannot pack your changes to the left file back into '%1'.\n\nThe original file will not be changed.\n\nDo you want to save the unpacked version to another file?") : 
				_("Plugin '%2' cannot pack your changes to the right file back into '%1'.\n\nThe original file will not be changed.\n\nDo you want to save the unpacked version to another file?"),
				strPath, pInfoTempUnpacker->pluginName);
		}
		// replace the unpacker with a "do nothing" unpacker
		pInfoTempUnpacker->Initialize(PLUGIN_MANUAL);
	}
	else
	{
		str = strutils::format_string2(_("Saving file failed.\n%1\n%2\nDo you want to:\n\t-use a different filename (Press Ok)\n\t-abort the current operation (Press Cancel)?"), strPath, sError);
	}

	// SAVE_NO_FILENAME is temporarily used for scratchpad.
	// So don't ask about saving in that case.
	if (nSaveResult != SAVE_NO_FILENAME)
		answer = AfxMessageBox(str.c_str(), MB_OKCANCEL | MB_ICONWARNING);

	switch (answer)
	{
	case IDOK:
		if (nBuffer == 0)
			title = _("Save Left File As");
		else if (nBuffer == m_nBuffers - 1)
			title = _("Save Right File As");
		else
			title = _("Save Middle File As");

		if (SelectFile(parent, s, strPath.c_str(), title, _T(""), false))
		{
			CDiffTextBuffer *pBuffer = m_ptBuf[nBuffer].get();
			strSavePath = s;
			nSaveResult = pBuffer->SaveToFile(strSavePath, false, sError,
				pInfoTempUnpacker);

			if (nSaveResult == SAVE_DONE)
			{
				// We are saving scratchpad (unnamed file)
				if (strPath.empty())
				{
					m_nBufferType[nBuffer] = BUFFER_UNNAMED_SAVED;
					m_strDesc[nBuffer].erase();
				}
					
				strPath = strSavePath;
				UpdateHeaderPath(nBuffer);
			}
			else
				result = false;
		}
		else
			nSaveResult = SAVE_CANCELLED;
		break;

	case IDCANCEL:
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
 * @param [in] nBuffer Index (0-based) of buffer to save
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
bool CMergeDoc::DoSave(LPCTSTR szPath, bool &bSaveSuccess, int nBuffer)
{
	DiffFileInfo fileInfo;
	String strSavePath(szPath);
	FileChange fileChanged;
	BOOL bApplyToAll = false;	
	int nRetVal = -1;

	fileChanged = IsFileChangedOnDisk(szPath, fileInfo, true, nBuffer);
	if (fileChanged == FileChanged)
	{
		String msg = strutils::format_string1(_("Another application has updated file\n%1\nsince WinMerge loaded it.\n\nOverwrite changed file?"), szPath);
		if (AfxMessageBox(msg.c_str(), MB_ICONWARNING | MB_YESNO) == IDNO)
		{
			bSaveSuccess = true;
			return true;
		}		
	}

	// use a temp packer
	// first copy the m_pInfoUnpacker
	// if an error arises during packing, change and take a "do nothing" packer
	PackingInfo infoTempUnpacker = *m_pInfoUnpacker;

	bSaveSuccess = false;
	
	// Check third arg possibly given from command-line
	if (!theApp.m_strSaveAsPath.empty())
	{
		if (paths::DoesPathExist(theApp.m_strSaveAsPath) == paths::IS_EXISTING_DIR)
		{
			// third arg was a directory, so get append the filename
			String sname;
			paths::SplitFilename(szPath, 0, &sname, 0);
			strSavePath = theApp.m_strSaveAsPath;
			strSavePath = paths::ConcatPath(strSavePath, sname);
		}
		else
			strSavePath = theApp.m_strSaveAsPath;	
	}

	nRetVal = theApp.HandleReadonlySave(strSavePath, false, bApplyToAll);
	if (nRetVal == IDCANCEL)
		return false;

	if (!theApp.CreateBackup(false, strSavePath))
		return false;

	// false as long as the user is not satisfied
	// true if saving succeeds, even with another filename, or if the user cancels
	bool result = false;
	// the error code from the latest save operation, 
	// or SAVE_DONE when the save succeeds
	// TODO: Shall we return this code in addition to bSaveSuccess ?
	int nSaveErrorCode = SAVE_DONE;
	CDiffTextBuffer *pBuffer = m_ptBuf[nBuffer].get();

	// Assume empty filename means Scratchpad (unnamed file)
	// Todo: This is not needed? - buffer type check should be enough
	if (strSavePath.empty())
		nSaveErrorCode = SAVE_NO_FILENAME;

	// Handle unnamed buffers
	if (m_nBufferType[nBuffer] == BUFFER_UNNAMED)
		nSaveErrorCode = SAVE_NO_FILENAME;

	String sError;
	if (nSaveErrorCode == SAVE_DONE)
		// We have a filename, just try to save
		nSaveErrorCode = pBuffer->SaveToFile(strSavePath, false, sError, &infoTempUnpacker);

	if (nSaveErrorCode != SAVE_DONE)
	{
		// Saving failed, user may save to another location if wants to
		do
			result = TrySaveAs(strSavePath, nSaveErrorCode, sError, nBuffer, &infoTempUnpacker);
		while (!result);
	}

	// Saving succeeded with given/selected filename
	if (nSaveErrorCode == SAVE_DONE)
	{
		// Preserve file times if user wants to
		if (GetOptionsMgr()->GetBool(OPT_PRESERVE_FILETIMES))
		{
			fileInfo.SetFile(strSavePath);
			try
			{
				TFile file(strSavePath);
				file.setLastModified(fileInfo.mtime);
			}
			catch (...)
			{
			}
		}

		m_ptBuf[nBuffer]->SetModified(false);
		m_pSaveFileInfo[nBuffer]->Update(strSavePath.c_str());
		m_filePaths[nBuffer] = strSavePath;
		m_pRescanFileInfo[nBuffer]->Update(m_filePaths[nBuffer].c_str());
		UpdateHeaderPath(nBuffer);
		bSaveSuccess = true;
		result = true;
	}
	else if (nSaveErrorCode == SAVE_CANCELLED)
	{
		// User cancelled current operation, lets do what user wanted to do
		result = false;
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
 * @param [in] nBuffer Index (0-based) of buffer to save
 * @return Tells if caller can continue (no errors happened)
 * @note Return value does not tell if SAVING succeeded. Caller must
 * Check value of bSaveSuccess parameter after calling this function!
 * @sa CMergeDoc::TrySaveAs()
 * @sa CMainFrame::CheckSavePath()
 * @sa CMergeDoc::CDiffTextBuffer::SaveToFile()
 */
bool CMergeDoc::DoSaveAs(LPCTSTR szPath, bool &bSaveSuccess, int nBuffer)
{
	String strSavePath(szPath);

	// use a temp packer
	// first copy the m_pInfoUnpacker
	// if an error arises during packing, change and take a "do nothing" packer
	PackingInfo infoTempUnpacker = *m_pInfoUnpacker;

	bSaveSuccess = false;
	// false as long as the user is not satisfied
	// true if saving succeeds, even with another filename, or if the user cancels
	bool result = false;
	// the error code from the latest save operation, 
	// or SAVE_DONE when the save succeeds
	// TODO: Shall we return this code in addition to bSaveSuccess ?
	int nSaveErrorCode = SAVE_DONE;

	// Use SAVE_NO_FILENAME to prevent asking about error
	nSaveErrorCode = SAVE_NO_FILENAME;

	// Loop until user succeeds saving or cancels
	String sError;
	do
		result = TrySaveAs(strSavePath, nSaveErrorCode, sError, nBuffer, &infoTempUnpacker);
	while (!result);

	// Saving succeeded with given/selected filename
	if (nSaveErrorCode == SAVE_DONE)
	{
		m_pSaveFileInfo[nBuffer]->Update(strSavePath);
		m_filePaths[nBuffer] = strSavePath;
		m_pRescanFileInfo[nBuffer]->Update(m_filePaths[nBuffer]);
		UpdateHeaderPath(nBuffer);
		bSaveSuccess = true;
		result = true;
	}
	return result;
}

/**
 * @brief Get left->right info for a moved line (apparent line number)
 */
int CMergeDoc::RightLineInMovedBlock(int nBuffer, int apparentLeftLine)
{
	if (!(m_ptBuf[nBuffer]->GetLineFlags(apparentLeftLine) & LF_MOVED))
		return -1;

	int realLeftLine = m_ptBuf[nBuffer]->ComputeRealLine(apparentLeftLine);
	int realRightLine = -1;
	if (m_diffWrapper.GetDetectMovedBlocks())
	{
		realRightLine = m_diffWrapper.GetMovedLines(nBuffer)->LineInBlock(realLeftLine,
				MovedLines::SIDE_RIGHT);
	}
	if (realRightLine != -1)
		return m_ptBuf[nBuffer + 1]->ComputeApparentLine(realRightLine);
	else
		return -1;
}

/**
 * @brief Get right->left info for a moved line (apparent line number)
 */
int CMergeDoc::LeftLineInMovedBlock(int nBuffer, int apparentRightLine)
{
	if (!(m_ptBuf[nBuffer]->GetLineFlags(apparentRightLine) & LF_MOVED))
		return -1;

	int realRightLine = m_ptBuf[nBuffer]->ComputeRealLine(apparentRightLine);
	int realLeftLine = -1;
	if (m_diffWrapper.GetDetectMovedBlocks())
	{
		realLeftLine = m_diffWrapper.GetMovedLines(nBuffer)->LineInBlock(realRightLine,
				MovedLines::SIDE_LEFT);
	}
	if (realLeftLine != -1)
		return m_ptBuf[nBuffer - 1]->ComputeApparentLine(realLeftLine);
	else
		return -1;
}

/**
 * @brief Save modified documents.
 * This function asks if user wants to save modified documents. We also
 * allow user to cancel the closing.
 *
 * There is a special trick avoiding showing two save-dialogs, as MFC framework
 * sometimes calls this function twice. We use static counter for these calls
 * and if we already have saving in progress (counter == 1) we skip the new
 * saving dialog.
 *
 * @return true if docs are closed, false if closing is cancelled.
 */
BOOL CMergeDoc::SaveModified()
{
	static int counter;
	++counter;
	if (counter > 1)
		return false;

	if (PromptAndSaveIfNeeded(true))
	{
		counter = 0;
		return true;
	}
	else
	{
		counter = 0;
		return false;
	}
}

/**
 * @brief Sets the current difference.
 * @param [in] nDiff Difference to set as current difference.
 */
void CMergeDoc::SetCurrentDiff(int nDiff)
{
	if (nDiff >= 0 && nDiff <= m_diffList.LastSignificantDiff())
		m_nCurDiff = nDiff;
	else
		m_nCurDiff = -1;
}

/**
 * @brief Take care of rescanning document.
 * 
 * Update view and restore cursor and scroll position after
 * rescanning document.
 * @param [in] bForced If true rescan cannot be suppressed
 */
void CMergeDoc::FlushAndRescan(bool bForced /* =false */)
{
	// Ignore suppressing when forced rescan
	if (!bForced)
		if (!m_bEnableRescan) return;

	CWaitCursor waitstatus;

	int nActiveViewIndexType = GetActiveMergeViewIndexType();

	// store cursors and hide caret
	int nBuffer;
	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		m_pView[nBuffer]->PushCursors();
		m_pDetailView[nBuffer]->PushCursors();
	}
	if (nActiveViewIndexType >= MERGEVIEW_PANE0 && nActiveViewIndexType <= MERGEVIEW_PANE2)
		m_pView[nActiveViewIndexType]->HideCursor();

	bool bBinary = false;
	IDENTLEVEL identical = IDENTLEVEL_NONE;
	int nRescanResult = Rescan(bBinary, identical, bForced);

	// restore cursors and caret
	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		m_pView[nBuffer]->PopCursors();
		m_pDetailView[nBuffer]->PopCursors();
	}
	if (nActiveViewIndexType >= MERGEVIEW_PANE0 && nActiveViewIndexType <= MERGEVIEW_PANE2)
		m_pView[nActiveViewIndexType]->ShowCursor();

	// because of ghostlines, m_nTopLine may differ just after Rescan
	// scroll both views to the same top line
	CMergeEditView * fixedView = m_pView[0];
	if (nActiveViewIndexType >= MERGEVIEW_PANE0 && nActiveViewIndexType <= MERGEVIEW_PANE2)
		// only one view needs to scroll so do not scroll the active view
		fixedView = m_pView[nActiveViewIndexType];
	fixedView->UpdateSiblingScrollPos(false);

	// make sure we see the cursor from the curent view
	if (nActiveViewIndexType >= MERGEVIEW_PANE0 && nActiveViewIndexType <= MERGEVIEW_PANE2)
		m_pView[nActiveViewIndexType]->EnsureVisible(m_pView[nActiveViewIndexType]->GetCursorPos());

	// scroll both diff views to the same top line
	CMergeEditView * fixedDetailView = m_pDetailView[0];
	if (nActiveViewIndexType >= MERGEVIEW_PANE0_DETAIL && nActiveViewIndexType <= MERGEVIEW_PANE2_DETAIL)
		// only one view needs to scroll so do not scroll the active view
		fixedDetailView = m_pDetailView[nActiveViewIndexType - MERGEVIEW_PANE0_DETAIL];
	fixedDetailView->UpdateSiblingScrollPos(false);

	// Refresh display
	UpdateAllViews(NULL);

	// Show possible error after updating screen
	if (nRescanResult != RESCAN_SUPPRESSED)
		ShowRescanError(nRescanResult, identical);
	m_LastRescan = COleDateTime::GetCurrentTime();
}

/**
 * @brief Saves both files
 */
void CMergeDoc::OnFileSave() 
{
	// We will need to know if either of the originals actually changed
	// so we know whether to update the diff status
	bool bChangedOriginal = false;

	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		if (m_ptBuf[nBuffer]->IsModified() && !m_ptBuf[nBuffer]->GetReadOnly())
		{
			// (why we don't use return value of DoSave)
			// DoSave will return true if it wrote to something successfully
			// but we have to know if it overwrote the original file
			bool bSaveOriginal = false;
			DoSave(m_filePaths[nBuffer].c_str(), bSaveOriginal, nBuffer );
			if (bSaveOriginal)
				bChangedOriginal = true;
		}
	}

	// If either of the actual source files being compared was changed
	// we need to update status in the dir view
	if (bChangedOriginal)
	{
		// If DirDoc contains diffs
		if (m_pDirDoc && m_pDirDoc->HasDiffs())
		{
			for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
			{
				if (m_bEditAfterRescan[nBuffer])
				{
					FlushAndRescan(false);
					break;
				}
			}

			bool bIdentical = !m_diffList.HasSignificantDiffs(); // True if status should be set to identical
			m_pDirDoc->UpdateChangedItem(m_filePaths, m_diffList.GetSignificantDiffs(),
					m_nTrivialDiffs, bIdentical);
		}
	}
}

void CMergeDoc::DoFileSave(int nBuffer)
{
	bool bSaveSuccess = false;
	bool bModified = false;

	if (m_ptBuf[nBuffer]->IsModified() && !m_ptBuf[nBuffer]->GetReadOnly())
	{
		bModified = true;
		DoSave(m_filePaths[nBuffer].c_str(), bSaveSuccess, nBuffer );
	}

	// If file were modified and saving succeeded,
	// update status on dir view
	if (bModified && bSaveSuccess)
	{
		// If DirDoc contains compare results
		if (m_pDirDoc && m_pDirDoc->HasDiffs())
		{
			for (int nBuffer1 = 0; nBuffer1 < m_nBuffers; nBuffer1++)
			{
				if (m_bEditAfterRescan[nBuffer1])
				{
					FlushAndRescan(false);
					break;
				}
			}

			bool bIdentical = !m_diffList.HasSignificantDiffs(); // True if status should be set to identical
			m_pDirDoc->UpdateChangedItem(m_filePaths, m_diffList.GetSignificantDiffs(),
					m_nTrivialDiffs, bIdentical);
		}
	}
}

/**
 * @brief Saves left-side file
 */
void CMergeDoc::OnFileSaveLeft()
{
	DoFileSave(0);
}

/**
 * @brief Saves middle-side file
 */
void CMergeDoc::OnFileSaveMiddle()
{
	DoFileSave(1);
}

/**
 * @brief Saves right-side file
 */
void CMergeDoc::OnFileSaveRight()
{
	DoFileSave(m_nBuffers - 1);
}

/**
 * @brief Saves left-side file with name asked
 */
void CMergeDoc::OnFileSaveAsLeft()
{
	bool bSaveResult = false;
	DoSaveAs(m_filePaths.GetLeft().c_str(), bSaveResult, 0);
}

/**
 * @brief Called when "Save middle (as...)" item is updated
 */
void CMergeDoc::OnUpdateFileSaveAsMiddle(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nBuffers == 3);
}

/**
 * @brief Saves right-side file with name asked
 */
void CMergeDoc::OnFileSaveAsMiddle()
{
	bool bSaveResult = false;
	DoSaveAs(m_filePaths.GetMiddle().c_str(), bSaveResult, 1);
}

/**
 * @brief Saves right-side file with name asked
 */
void CMergeDoc::OnFileSaveAsRight()
{
	bool bSaveResult = false;
	DoSaveAs(m_filePaths.GetRight().c_str(), bSaveResult, m_nBuffers - 1);
}

/**
 * @brief Update diff-number pane text in file compare.
 * The diff number pane shows selected difference/amount of differences when
 * there is difference selected. If there is no difference selected, then
 * the panel shows amount of differences. If there are ignored differences,
 * those are not count into numbers.
 * @param [in] pCmdUI UI component to update.
 */
void CMergeDoc::OnUpdateStatusNum(CCmdUI* pCmdUI) 
{
	TCHAR sIdx[32];
	TCHAR sCnt[32];
	String s;
	const int nDiffs = m_diffList.GetSignificantDiffs();
	
	// Files are identical - show text "Identical"
	if (nDiffs <= 0)
		s = _("Identical");
	
	// There are differences, but no selected diff
	// - show amount of diffs
	else if (GetCurrentDiff() < 0)
	{
		s = nDiffs == 1 ? _("1 Difference Found") : _("%1 Differences Found");
		_itot_s(nDiffs, sCnt, 10);
		strutils::replace(s, _T("%1"), sCnt);
	}
	
	// There are differences and diff selected
	// - show diff number and amount of diffs
	else
	{
		s = _("Difference %1 of %2");
		const int signInd = m_diffList.GetSignificantIndex(GetCurrentDiff());
		_itot_s(signInd + 1, sIdx, 10);
		strutils::replace(s, _T("%1"), sIdx);
		_itot_s(nDiffs, sCnt, 10);
		strutils::replace(s, _T("%2"), sCnt);
	}
	pCmdUI->SetText(s.c_str());
}

/**
 * @brief Update plugin name
 * @param [in] pCmdUI UI component to update.
 */
void CMergeDoc::OnUpdatePluginName(CCmdUI* pCmdUI)
{
	String pluginNames;
	if (m_pInfoUnpacker && !m_pInfoUnpacker->pluginName.empty())
		pluginNames += m_pInfoUnpacker->pluginName + _T("&");
	PrediffingInfo prediffer;
	GetPrediffer(&prediffer);
	if (!prediffer.pluginName.empty())
		pluginNames += prediffer.pluginName + _T("&");
	pCmdUI->SetText(pluginNames.substr(0, pluginNames.length() - 1).c_str());
}

/**
 * @brief Change number of diff context lines
 */
void CMergeDoc::OnDiffContext(UINT nID)
{
	switch (nID)
	{
	case ID_VIEW_DIFFCONTEXT_0:
		m_nDiffContext = 0; break;
	case ID_VIEW_DIFFCONTEXT_1:
		m_nDiffContext = 1; break;
	case ID_VIEW_DIFFCONTEXT_3:
		m_nDiffContext = 3; break;
	case ID_VIEW_DIFFCONTEXT_5:
		m_nDiffContext = 5; break;
	case ID_VIEW_DIFFCONTEXT_7:
		m_nDiffContext = 7; break;
	case ID_VIEW_DIFFCONTEXT_9:
		m_nDiffContext = 9; break;
	case ID_VIEW_DIFFCONTEXT_TOGGLE:
		m_nDiffContext = -m_nDiffContext - 1; break;
	case ID_VIEW_DIFFCONTEXT_ALL:
		if (m_nDiffContext >= 0)
			m_nDiffContext = -m_nDiffContext - 1;
		break;
	}
	GetOptionsMgr()->SaveOption(OPT_DIFF_CONTEXT, m_nDiffContext);
	FlushAndRescan(true);
}

/**
 * @brief Update number of diff context lines
 */
void CMergeDoc::OnUpdateDiffContext(CCmdUI* pCmdUI)
{
	bool bCheck;
	switch (pCmdUI->m_nID)
	{
	case ID_VIEW_DIFFCONTEXT_0:
		bCheck = (m_nDiffContext == 0); break;
	case ID_VIEW_DIFFCONTEXT_1:
		bCheck = (m_nDiffContext == 1); break;
	case ID_VIEW_DIFFCONTEXT_3:
		bCheck = (m_nDiffContext == 3); break;
	case ID_VIEW_DIFFCONTEXT_5:
		bCheck = (m_nDiffContext == 5); break;
	case ID_VIEW_DIFFCONTEXT_7:
		bCheck = (m_nDiffContext == 7); break;
	case ID_VIEW_DIFFCONTEXT_9:
		bCheck = (m_nDiffContext == 9); break;
	case ID_VIEW_DIFFCONTEXT_TOGGLE:
		bCheck = false; break;
	default:
		bCheck = (m_nDiffContext < 0); break;
	}
	pCmdUI->SetCheck(bCheck);
	pCmdUI->Enable(true);
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
	int nDiffCount = m_diffList.GetSize();
	int file;

	// walk the diff list and calculate numbers of extra lines to add
	int extras[3]={0};   // extra lines added to each view
	m_diffList.GetExtraLinesCounts(m_nBuffers, extras);

	// resize m_aLines once for each view
	UINT lcount[3] = {0};
	UINT lcountnew[3] = {0};
	
	for (file = 0; file < m_nBuffers; file++)
	{
		lcount[file] = m_ptBuf[file]->GetLineCount();
		lcountnew[file] = lcount[file] + extras[file];
		m_ptBuf[file]->m_aLines.resize(lcountnew[file]);
	}
// this ASSERT may be false because of empty last line (see function's note)
//	ASSERT(lcount0new == lcount1new);

	// walk the diff list backward, move existing lines to proper place,
	// add ghost lines, and set flags
	for (nDiff = nDiffCount - 1; nDiff >= 0; nDiff --)
	{
		DIFFRANGE curDiff;
		VERIFY(m_diffList.GetDiff(nDiff, curDiff));

		// move matched lines after curDiff
		int nline[3] = { 0 };
		for (file = 0; file < m_nBuffers; file++)
			nline[file] = lcount[file] - curDiff.end[file] - 1; // #lines on left/middle/right after current diff
		// Matched lines should really match...
		// But matched lines after last diff may differ because of empty last line (see function's note)
		if (nDiff < nDiffCount - 1)
			ASSERT(nline[0] == nline[1]);

		int nmaxline = 0;
		for (file = 0; file < m_nBuffers; file++)
		{
			// Move all lines after current diff down as far as needed
			// for any ghost lines we're about to insert
			m_ptBuf[file]->MoveLine(curDiff.end[file]+1, lcount[file]-1, lcountnew[file]-nline[file]);
			lcountnew[file] -= nline[file];
			lcount[file] -= nline[file];
			// move unmatched lines and add ghost lines
			nline[file] = curDiff.end[file] - curDiff.begin[file] + 1; // #lines in diff on left/middle/right
			nmaxline = max(nmaxline, nline[file]);
		}

		for (file = 0; file < m_nBuffers; file++)
		{
			m_ptBuf[file]->MoveLine(curDiff.begin[file], curDiff.end[file], lcountnew[file]-nmaxline);
			int nextra = nmaxline - nline[file];
			if (nextra > 0)
			{
				m_ptBuf[file]->SetEmptyLine(lcountnew[file] - nextra, nextra);
				DWORD dflag = LF_GHOST;
				if ((file == 0 && curDiff.op == OP_3RDONLY) || (file == 2 && curDiff.op == OP_1STONLY))
					dflag |= LF_SNP;
				for (int i = 1; i <= nextra; i++)
					m_ptBuf[file]->SetLineFlag(lcountnew[file]-i, dflag, true, false, false);
			}
			lcountnew[file] -= nmaxline;

			lcount[file] -= nline[file];

		}
		// set dbegin, dend, blank, and line flags
		curDiff.dbegin = lcountnew[0];

		switch (curDiff.op)
		{
		case OP_TRIVIAL:
			++m_nTrivialDiffs;
			// fall through and handle as diff
		case OP_DIFF:
		case OP_1STONLY:
		case OP_2NDONLY:
		case OP_3RDONLY:
			// set curdiff
			{
				curDiff.dend = lcountnew[0]+nmaxline-1;
				for (file = 0; file < m_nBuffers; file++)
				{
					curDiff.blank[file] = -1;
					int nextra = nmaxline - nline[file];
					if (nmaxline > nline[file])
					{
						// more lines on left, ghost lines on right side
						curDiff.blank[file] = curDiff.dend + 1 - nextra;
					}
				}
			}
			// flag lines
			{
				for (file = 0; file < m_nBuffers; file++)
				{
					// left side
					int i;
					for (i = curDiff.dbegin; i <= curDiff.dend; i++)
					{
						if (curDiff.blank[file] == -1 || (int)i < curDiff.blank[file])
						{
							// set diff or trivial flag
							DWORD dflag = (curDiff.op == OP_TRIVIAL) ? LF_TRIVIAL : LF_DIFF;
							if ((file == 0 && curDiff.op == OP_3RDONLY) || (file == 2 && curDiff.op == OP_1STONLY))
								dflag |= LF_SNP;
							m_ptBuf[file]->SetLineFlag(i, dflag, true, false, false);
m_ptBuf[file]->SetLineFlag(i, LF_INVISIBLE, false, false, false);
						}
						else
						{
							// ghost lines are already inserted (and flagged)
							// ghost lines opposite to trivial lines are ghost and trivial
							if (curDiff.op == OP_TRIVIAL)
								m_ptBuf[file]->SetLineFlag(i, LF_TRIVIAL, true, false, false);
						}
					}
				}
			}
			break;
		}           // switch (curDiff.op)
		VERIFY(m_diffList.SetDiff(nDiff, curDiff));
	}             // for (nDiff = nDiffCount; nDiff-- > 0; )

	m_diffList.ConstructSignificantChain();

	// Used to strip trivial diffs out of the diff chain
	// if m_nTrivialDiffs
	// via copying them all to a new chain, then copying only non-trivials back
	// but now we keep all diffs, including trivial diffs


	for (file = 0; file < m_nBuffers; file++)
		m_ptBuf[file]->FinishLoading();
}

/**
 * @brief Checks if file has changed since last update (save or rescan).
 * @param [in] szPath File to check
 * @param [in] dfi Previous fileinfo of file
 * @param [in] bSave If true Compare to last save-info, else to rescan-info
 * @param [in] nBuffer Index (0-based) of buffer
 * @return true if file is changed.
 */
CMergeDoc::FileChange CMergeDoc::IsFileChangedOnDisk(LPCTSTR szPath, DiffFileInfo &dfi,
	bool bSave, int nBuffer)
{
	DiffFileInfo *fileInfo = NULL;
	bool bFileChanged = false;
	bool bIgnoreSmallDiff = GetOptionsMgr()->GetBool(OPT_IGNORE_SMALL_FILETIME);
	int tolerance = 0;
	if (bIgnoreSmallDiff)
		tolerance = SmallTimeDiff; // From MainFrm.h

	if (bSave)
		fileInfo = m_pSaveFileInfo[nBuffer].get();
	else
		fileInfo = m_pRescanFileInfo[nBuffer].get();

	// We assume file existed, so disappearing means removal
	if (!dfi.Update(szPath))
		return FileRemoved;

	int64_t timeDiff = dfi.mtime - fileInfo->mtime;
	if (timeDiff < 0) timeDiff = -timeDiff;
	if ((timeDiff > tolerance * Poco::Timestamp::resolution()) || (dfi.size != fileInfo->size))
	{
		bFileChanged = true;
	}

	if (bFileChanged)
		return FileChanged;
	else
		return FileNoChange;
}

void CMergeDoc::HideLines()
{
	int nLine;
	int file;

	if (m_nDiffContext < 0)
	{
		for (file = 0; file < m_nBuffers; file++)
			m_pView[file]->SetEnableHideLines(false);
		return;
	}

	int nLineCount = 0x7fffffff;
	for (file = 0; file < m_nBuffers; file++)
	{
		if (nLineCount > m_ptBuf[file]->GetLineCount())
			nLineCount = m_ptBuf[file]->GetLineCount();
	}

	for (nLine =  0; nLine < nLineCount;)
	{
		if (!(m_ptBuf[0]->GetLineFlags(nLine) & (LF_DIFF | LF_GHOST)))
		{
			for (file = 0; file < m_nBuffers; file++)
				m_ptBuf[file]->SetLineFlag(nLine, LF_INVISIBLE, true, false, false);
			nLine++;
		}
		else
		{
			int nLine2 = (nLine - m_nDiffContext < 0) ? 0 : (nLine - m_nDiffContext);
			for (; nLine2 < nLine; nLine2++)
			{
				for (file = 0; file < m_nBuffers; file++)
					m_ptBuf[file]->SetLineFlag(nLine2, LF_INVISIBLE, false, false, false);
			}
		
			for (; nLine < nLineCount; nLine++)
			{
				if (!(m_ptBuf[0]->GetLineFlags(nLine) & (LF_DIFF | LF_GHOST)))
					break;
				for (file = 0; file < m_nBuffers; file++)
					m_ptBuf[file]->SetLineFlag(nLine, LF_INVISIBLE, false, false, false);
			}

			int nLineEnd2 = (nLine + m_nDiffContext >= nLineCount) ? nLineCount-1 : (nLine + m_nDiffContext);
			for (; nLine < nLineEnd2; nLine++)
			{
				for (file = 0; file < m_nBuffers; file++)
					m_ptBuf[file]->SetLineFlag(nLine, LF_INVISIBLE, false, false, false);
				if (m_ptBuf[0]->GetLineFlags(nLine) & (LF_DIFF | LF_GHOST))
					nLineEnd2 = (nLine + 1 + m_nDiffContext >= nLineCount) ? nLineCount-1 : (nLine + 1 + m_nDiffContext);
			}
		}
	}

	for (file = 0; file < m_nBuffers; file++)
		m_pView[file]->SetEnableHideLines(true);
}

/**
 * @brief Asks and then saves modified files.
 *
 * This function saves modified files. Dialog is shown for user to select
 * modified file(s) one wants to save or discard changed. Cancelling of
 * save operation is allowed unless denied by parameter. After successfully
 * save operation file statuses are updated to directory compare.
 * @param [in] bAllowCancel If false "Cancel" button is disabled.
 * @return true if user selected "OK" so next operation can be
 * executed. If false user choosed "Cancel".
 * @note If filename is empty, we assume scratchpads are saved,
 * so instead of filename, description is shown.
 * @todo If we have filename and description for file, what should
 * we do after saving to different filename? Empty description?
 * @todo Parameter @p bAllowCancel is always true in callers - can be removed.
 */
bool CMergeDoc::PromptAndSaveIfNeeded(bool bAllowCancel)
{
	bool bLModified = false, bMModified = false, bRModified = false;
	bool result = true;
	bool bLSaveSuccess = false, bMSaveSuccess = false, bRSaveSuccess = false;

	if (m_nBuffers == 3)
	{
		bLModified = m_ptBuf[0]->IsModified();
		bMModified = m_ptBuf[1]->IsModified();
		bRModified = m_ptBuf[2]->IsModified();
	}
	else
	{
		bLModified = m_ptBuf[0]->IsModified();
		bRModified = m_ptBuf[1]->IsModified();
	}
	if (!bLModified && !bMModified && !bRModified)
		 return true;

	SaveClosingDlg dlg;
	dlg.DoAskFor(bLModified, bMModified, bRModified);
	if (!bAllowCancel)
		dlg.m_bDisableCancel = true;
	if (!m_filePaths.GetLeft().empty())
	{
		if (theApp.m_strSaveAsPath.empty())
			dlg.m_sLeftFile = m_filePaths.GetLeft();
		else
			dlg.m_sLeftFile = theApp.m_strSaveAsPath;
	}
	else
		dlg.m_sLeftFile = m_strDesc[0];
	if (m_nBuffers == 3)
	{
		if (!m_filePaths.GetMiddle().empty())
		{
			if (theApp.m_strSaveAsPath.empty())
				dlg.m_sMiddleFile = m_filePaths.GetMiddle();
			else
				dlg.m_sMiddleFile = theApp.m_strSaveAsPath;
		}
		else
			dlg.m_sMiddleFile = m_strDesc[1];
	}
	if (!m_filePaths.GetRight().empty())
	{
		if (theApp.m_strSaveAsPath.empty())
			dlg.m_sRightFile = m_filePaths.GetRight();
		else
			dlg.m_sRightFile = theApp.m_strSaveAsPath;
	}
	else
		dlg.m_sRightFile = m_strDesc[m_nBuffers - 1];

	if (dlg.DoModal() == IDOK)
	{
		if (bLModified && dlg.m_leftSave == SaveClosingDlg::SAVECLOSING_SAVE)
		{
			if (!DoSave(m_filePaths.GetLeft().c_str(), bLSaveSuccess, 0))
				result = false;
		}

		if (bMModified && dlg.m_middleSave == SaveClosingDlg::SAVECLOSING_SAVE)
		{
			if (!DoSave(m_filePaths.GetMiddle().c_str(), bMSaveSuccess, 1))
				result = false;
		}

		if (bRModified && dlg.m_rightSave == SaveClosingDlg::SAVECLOSING_SAVE)
		{
			if (!DoSave(m_filePaths.GetRight().c_str(), bRSaveSuccess, m_nBuffers - 1))
				result = false;
		}
	}
	else
	{	
		result = false;
	}

	// If file were modified and saving was successfull,
	// update status on dir view
	if ((bLModified && bLSaveSuccess) || 
	     (bMModified && bMSaveSuccess) ||
		 (bRModified && bRSaveSuccess))
	{
		// If directory compare has results
		if (m_pDirDoc && m_pDirDoc->HasDiffs())
		{
			if (m_bEditAfterRescan[0] || m_bEditAfterRescan[1] || (m_nBuffers == 3 && m_bEditAfterRescan[2]))
				FlushAndRescan(false);

			bool bIdentical = !m_diffList.HasSignificantDiffs(); // True if status should be set to identical
			m_pDirDoc->UpdateChangedItem(m_filePaths, m_diffList.GetSignificantDiffs(),
					m_nTrivialDiffs, bIdentical);
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
		// (laoran 08-01-2003) maybe should be FlushAndRescan(true) ??
		FlushAndRescan();
}

/**
 * @brief We have two child views (left & right), so we keep pointers directly
 * at them (the MFC view list doesn't have them both)
 */
void CMergeDoc::SetMergeViews(CMergeEditView *pView[])
{
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		ASSERT(pView[nBuffer] && !m_pView[nBuffer]);
		m_pView[nBuffer] = pView[nBuffer];
	}
}

/**
 * @brief Someone is giving us pointers to our detail views
 */
void CMergeDoc::SetMergeDetailViews(CMergeEditView * pDetailView[])
{
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		ASSERT(pDetailView[nBuffer] && !m_pDetailView[nBuffer]);
		m_pDetailView[nBuffer] = pDetailView[nBuffer];
	}
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
	return dynamic_cast<CChildFrame *>(m_pView[0]->GetParentFrame()); 
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
bool CMergeDoc::CloseNow()
{
	// Allow user to cancel closing
	if (!PromptAndSaveIfNeeded(true))
		return false;

	GetParentFrame()->CloseNow();
	return true;
}

/**
 * @brief Loads file to buffer and shows load-errors
 * @param [in] sFileName File to open
 * @param [in] nBuffer Index (0-based) of buffer to load
 * @param [out] readOnly whether file is read-only
 * @param [in] encoding encoding used
 * @return Tells if files were loaded successfully
 * @sa CMergeDoc::OpenDocs()
 **/
int CMergeDoc::LoadFile(CString sFileName, int nBuffer, bool & readOnly, const FileTextEncoding & encoding)
{
	String sError;
	DWORD retVal = FileLoadResult::FRESULT_ERROR;

	CDiffTextBuffer *pBuf = m_ptBuf[nBuffer].get();
	m_filePaths[nBuffer] = sFileName;

	CRLFSTYLE nCrlfStyle = CRLF_STYLE_AUTOMATIC;
	CString sOpenError;
	retVal = pBuf->LoadFromFile(sFileName, m_pInfoUnpacker.get(),
		m_strBothFilenames.c_str(), readOnly, nCrlfStyle, encoding, sOpenError);

	// if CMergeDoc::CDiffTextBuffer::LoadFromFile failed,
	// it left the pBuf in a valid (but empty) state via a call to InitNew

	if (FileLoadResult::IsOkImpure(retVal))
	{
		// File loaded, and multiple EOL types in this file
		FileLoadResult::SetMainOk(retVal);

		// If mixed EOLs are not enabled, enable them for this doc.
		if (!GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL))
		{
			pBuf->SetMixedEOL(true);
		}
	}

	if (FileLoadResult::IsError(retVal))
	{
		// Error from Unifile/system
		if (!sOpenError.IsEmpty())
			sError = strutils::format_string2(_("Cannot open file\n%1\n\n%2"), (LPCTSTR)sFileName, (LPCTSTR)sOpenError);
		else
			sError = strutils::format_string1(_("File not found: %1"), (LPCTSTR)sFileName);
		AfxMessageBox(sError.c_str(), MB_OK | MB_ICONSTOP | MB_MODELESS);
	}
	else if (FileLoadResult::IsErrorUnpack(retVal))
	{
		sError = strutils::format_string1(_("File not unpacked: %1"), (LPCTSTR)sFileName);
		AfxMessageBox(sError.c_str(), MB_OK | MB_ICONSTOP | MB_MODELESS);
	}
	return retVal;
}

/**
 * @brief Check if specified codepage number is valid for WinMerge Editor.
 * @param [in] cp Codepage number to check.
 * @return true if codepage is valid, false otherwise.
 */
bool CMergeDoc::IsValidCodepageForMergeEditor(unsigned cp) const
{
	if (!cp) // 0 is our signal value for invalid
		return false;
	return GetEncodingNameFromCodePage(cp) != NULL;
}

/**
 * @brief Sanity check file's specified codepage.
 * This function checks if file's specified codepage is valid for WinMerge
 * editor and if not resets the codepage to default.
 * @param [in,out] fileinfo Class containing file's codepage.
 */
void CMergeDoc::SanityCheckCodepage(FileLocation & fileinfo)
{
	if (fileinfo.encoding.m_unicoding == ucr::NONE
		&& !IsValidCodepageForMergeEditor(fileinfo.encoding.m_codepage))
	{
		int cp = ucr::getDefaultCodepage();
		if (!IsValidCodepageForMergeEditor(cp))
			cp = CP_ACP;
		fileinfo.encoding.SetCodepage(cp);
	}
}

/**
 * @brief Loads one file from disk and updates file infos.
 * @param [in] index Index of file in internal buffers.
 * @param [in] filename File's name.
 * @param [in] readOnly Is file read-only?
 * @param [in] encoding File's encoding.
 * @return One of FileLoadResult values.
 */
DWORD CMergeDoc::LoadOneFile(int index, String filename, bool readOnly, const String& strDesc, 
		const FileTextEncoding & encoding)
{
	DWORD loadSuccess = FileLoadResult::FRESULT_ERROR;;
	
	m_strDesc[index] = strDesc;
	if (!filename.empty())
	{
		if (strDesc.empty())
			m_nBufferType[index] = BUFFER_NORMAL;
		else
			m_nBufferType[index] = BUFFER_NORMAL_NAMED;
		m_pSaveFileInfo[index]->Update(filename);
		m_pRescanFileInfo[index]->Update(filename);

		loadSuccess = LoadFile(filename.c_str(), index, readOnly, encoding);
		if (FileLoadResult::IsLossy(loadSuccess))
		{
			m_ptBuf[index]->FreeAll();
			loadSuccess = LoadFile(filename.c_str(), index, readOnly,
				GuessCodepageEncoding(filename, GetOptionsMgr()->GetInt(OPT_CP_DETECT), -1));
		}
	}
	else
	{
		m_nBufferType[index] = BUFFER_UNNAMED;
		m_ptBuf[index]->InitNew();
		m_ptBuf[index]->m_encoding = encoding;
		loadSuccess = FileLoadResult::FRESULT_OK;
	}
	return loadSuccess;
}

/**
 * @brief Loads files and does initial rescan.
 * @param fileloc [in] File to open to left/middle/right side (path & encoding info)
 * @param bRO [in] Is left/middle/right file read-only
 * @param nPane [in] Pane to activate
 * @param nLineIndex [in] Index of line in view to move the cursor to
 * @return Success/Failure/Binary (failure) per typedef enum OpenDocsResult_TYPE
 * @todo Options are still read from CMainFrame, this will change
 * @sa CMainFrame::ShowMergeDoc()
 */
bool CMergeDoc::OpenDocs(int nFiles, const FileLocation ifileloc[],
		const bool bRO[], const String strDesc[], int nPane/* = -1 */, int nLineIndex/* = -1 */)
{
	IDENTLEVEL identical = IDENTLEVEL_NONE;
	int nRescanResult = RESCAN_OK;
	int nBuffer;
	FileLocation fileloc[3];

	std::copy_n(ifileloc, 3, fileloc);

	// Filter out invalid codepages, or editor will display all blank
	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		SanityCheckCodepage(fileloc[nBuffer]);

	// clear undo stack
	undoTgt.clear();
	curUndo = undoTgt.begin();

	// Prevent displaying views during LoadFile
	// Note : attach buffer again only if both loads succeed
	m_strBothFilenames.erase();
	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		m_pView[nBuffer]->DetachFromBuffer();
		m_pDetailView[nBuffer]->DetachFromBuffer();
		
		// clear undo buffers
		m_ptBuf[nBuffer]->m_aUndoBuf.clear();

		// free the buffers
		m_ptBuf[nBuffer]->FreeAll ();

		// build the text being filtered, "|" separates files as it is forbidden in filenames
		m_strBothFilenames += fileloc[nBuffer].filepath + _T("|");
	}
	m_strBothFilenames.erase(m_strBothFilenames.length() - 1);

	// Load files
	DWORD nSuccess[3];
	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		nSuccess[nBuffer] = LoadOneFile(nBuffer, fileloc[nBuffer].filepath, bRO[nBuffer], strDesc ? strDesc[nBuffer] : _T(""),
			fileloc[nBuffer].encoding);
	}
	const bool bFiltersEnabled = GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED);

	// scratchpad : we don't call LoadFile, so
	// we need to initialize the unpacker as a "do nothing" one
	if (bFiltersEnabled)
	{ 
		if (std::count(m_nBufferType, m_nBufferType + m_nBuffers, BUFFER_UNNAMED) == m_nBuffers)
		{
			m_pInfoUnpacker->Initialize(PLUGIN_MANUAL);
		}
	}

	// Bail out if either side failed
	if (std::find_if(nSuccess, nSuccess + m_nBuffers, [](DWORD d){return !FileLoadResult::IsOk(d);} ) != nSuccess + m_nBuffers)
	{
		CChildFrame *pFrame = GetParentFrame();
		if (pFrame)
		{
			// Use verify macro to trap possible error in debug.
			VERIFY(pFrame->DestroyWindow());
		}
		return false;
	}

	// Warn user if file load was lossy (bad encoding)
	int idres=0;
	int nLossyBuffers = 0;
	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		if (FileLoadResult::IsLossy(nSuccess[nBuffer]))
		{
			// TODO: It would be nice to report how many lines were lossy
			// we did calculate those numbers when we loaded the files, in the text stats
	
			idres = IDS_LOSSY_TRANSCODING_FIRST + nBuffer;
			nLossyBuffers++;
		}
	}
	if (nLossyBuffers > 1)
		idres = IDS_LOSSY_TRANSCODING_BOTH; /* FIXEME */
	
	if (nLossyBuffers > 0)
	{
		if (!m_pEncodingErrorBar)
		{
			m_pEncodingErrorBar.reset(new CEncodingErrorBar());
			m_pEncodingErrorBar->Create(this->m_pView[0]->GetParentFrame());
		}
		m_pEncodingErrorBar->SetText(LoadResString(idres));
		m_pView[0]->GetParentFrame()->ShowControlBar(m_pEncodingErrorBar.get(), TRUE, FALSE);
	}

	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		// Now buffers data are valid
		m_pView[nBuffer]->AttachToBuffer();
		m_pDetailView[nBuffer]->AttachToBuffer();

		// Currently there is only one set of syntax colors, which all documents & views share
		m_pView[nBuffer]->SetColorContext(theApp.GetMainSyntaxColors());
		m_pDetailView[nBuffer]->SetColorContext(theApp.GetMainSyntaxColors());

		// Currently there is only one set of markers, which all documents & views share
		m_pView[nBuffer]->SetMarkersContext(theApp.GetMainMarkers());
		m_pDetailView[nBuffer]->SetMarkersContext(theApp.GetMainMarkers());

		// Set read-only statuses
		m_ptBuf[nBuffer]->SetReadOnly(bRO[nBuffer]);
	}

	// Check the EOL sensitivity option (do it before Rescan)
	DIFFOPTIONS diffOptions = {0};
	m_diffWrapper.GetOptions(&diffOptions);
	if (!GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL) && !diffOptions.bIgnoreEol)
	{
		for (nBuffer = 1; nBuffer < m_nBuffers; nBuffer++)
			if (m_ptBuf[0]->GetCRLFMode() != m_ptBuf[nBuffer]->GetCRLFMode())
				break;

		if (nBuffer < m_nBuffers)
		{
			// Options and files not are not compatible :
			// Sensitive to EOL on, allow mixing EOL off, and files have a different EOL style.
			// All lines will differ, that is not very interesting and probably not wanted.
			// Propose to turn off the option 'sensitive to EOL'
			String s = theApp.LoadString(IDS_SUGGEST_IGNOREEOL);
			if (AfxMessageBox(s.c_str(), MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN | MB_IGNORE_IF_SILENCED, IDS_SUGGEST_IGNOREEOL) == IDYES)
			{
				diffOptions.bIgnoreEol = true;
				m_diffWrapper.SetOptions(&diffOptions);
			}
		}
	}

	// Define the prediffer
	PackingInfo * infoUnpacker = 0;
	PrediffingInfo * infoPrediffer = 0;
	if (bFiltersEnabled && m_pDirDoc)
	{
		m_pDirDoc->GetPluginManager().FetchPluginInfos(m_strBothFilenames, &infoUnpacker, &infoPrediffer);
		m_diffWrapper.SetPrediffer(infoPrediffer);
		m_diffWrapper.SetTextForAutomaticPrediff(m_strBothFilenames);
	}

	bool bBinary = false;
	nRescanResult = Rescan(bBinary, identical);

	// Open filed if rescan succeed and files are not binaries
	if (nRescanResult == RESCAN_OK)
	{
		// set the document types
		// Warning : it is the first thing to do (must be done before UpdateView,
		// or any function that calls UpdateView, like SelectDiff)
		// Note: If option enabled, and another side type is not recognized,
		// we use recognized type for unrecognized side too.
		String sext;
		bool bTyped[3];
		int paneTyped = 0;

		for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			if (bFiltersEnabled && m_pInfoUnpacker->textType.length())
				sext = m_pInfoUnpacker->textType;
			else
				sext = GetFileExt(fileloc[nBuffer].filepath.c_str(), m_strDesc[nBuffer].c_str());
			bTyped[nBuffer] = GetView(nBuffer)->SetTextType(sext.c_str());
			GetDetailView(nBuffer)->SetTextType(sext.c_str());
			if (bTyped[nBuffer])
				paneTyped = nBuffer;
		}

		for (nBuffer = 1; nBuffer < m_nBuffers; nBuffer++)
		{
			if (bTyped[0] != bTyped[nBuffer])
				break;
		}

		bool syntaxHLEnabled = GetOptionsMgr()->GetBool(OPT_SYNTAX_HIGHLIGHT);
		if (syntaxHLEnabled && nBuffer < m_nBuffers)
		{
			if (std::count(bTyped, bTyped + m_nBuffers, false) == m_nBuffers)
			{
				CString sFirstLine;
				m_ptBuf[0]->GetLine(0, sFirstLine);
				for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
				{
					bTyped[nBuffer] = GetView(nBuffer)->SetTextTypeByContent(sFirstLine);
				}
			}
		}

		if (syntaxHLEnabled)
		{
			for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
			{
				if (!bTyped[nBuffer])
				{
					CCrystalTextView::TextDefinition *enuType = GetView(paneTyped)->GetTextType(sext.c_str());
					GetView(nBuffer)->SetTextType(enuType);
					GetDetailView(nBuffer)->SetTextType(enuType);
				}
			}
		}

		int nNormalBuffer = 0;
		for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			// set the frame window header
			UpdateHeaderPath(nBuffer);

			GetView(nBuffer)->DocumentsLoaded();
			GetDetailView(nBuffer)->DocumentsLoaded();
			
			if ((m_nBufferType[nBuffer] == BUFFER_NORMAL) ||
			    (m_nBufferType[nBuffer] == BUFFER_NORMAL_NAMED))
			{
				nNormalBuffer++;
			}
			
		}

		// Inform user that files are identical
		// Don't show message if new buffers created
		if (identical == IDENTLEVEL_ALL && nNormalBuffer > 0)
		{
			ShowRescanError(nRescanResult, identical);
		}

		if (nPane < 0)
		{
			nPane = theApp.GetProfileInt(_T("Settings"), _T("ActivePane"), 0);
			if (nPane < 0 || nPane >= m_nBuffers)
			nPane = 0;
		}
		if (nLineIndex == -1)
		{
			// scroll to first diff
			if (GetOptionsMgr()->GetBool(OPT_SCROLL_TO_FIRST) &&
				m_diffList.HasSignificantDiffs())
			{
				int nDiff = m_diffList.FirstSignificantDiff();
				m_pView[nPane]->SelectDiff(nDiff, true, false);
				nLineIndex = m_pView[nPane]->GetCursorPos().y;
			}
			else
			{
				nLineIndex = 0;
			}
		}
		m_pView[nPane]->GotoLine(nLineIndex, false, nPane);

		// Exit if files are identical should only work for the first
		// comparison and must be disabled afterward.
		theApp.m_bExitIfNoDiff = MergeCmdLineInfo::Disabled;
	}
	else
	{
		// CMergeDoc::Rescan fails if files do not exist on both sides 
		// or the really arcane case that the temp files couldn't be created, 
		// which is too obscure to bother reporting if you can't write to 
		// your temp directory, doing nothing is graceful enough for that).
		ShowRescanError(nRescanResult, identical);
		GetParentFrame()->DestroyWindow();
		return false;
	}

	// Force repaint of location pane to update it in case we had some warning
	// dialog visible and it got painted before files were loaded
	if (m_pView[0])
		m_pView[0]->RepaintLocationPane();

	return true;
}

void CMergeDoc::ChangeFile(int nBuffer, const String& path)
{
	if (!PromptAndSaveIfNeeded(true))
		return;

	FileLocation fileloc[3];
	String strDesc[3];
	bool bRO[3];
	for (int pane = 0; pane < m_nBuffers; pane++)
	{
		bRO[pane] = m_ptBuf[pane]->GetReadOnly();
		fileloc[pane].encoding.m_unicoding = m_ptBuf[pane]->getUnicoding();
		fileloc[pane].encoding.m_codepage = m_ptBuf[pane]->getCodepage();
		fileloc[pane].setPath(m_filePaths[pane]);
	}
	std::copy_n(m_strDesc, m_nBuffers, strDesc);

	strDesc[nBuffer] = _T("");
	fileloc[nBuffer].setPath(path);
	fileloc[nBuffer].encoding = GuessCodepageEncoding(path, GetOptionsMgr()->GetInt(OPT_CP_DETECT));
	
	OpenDocs(m_nBuffers, fileloc, bRO, strDesc, nBuffer, 0);
}

/**
 * @brief Re-load a document.
 * This methods re-loads the file compare document. The re-loaded document is
 * one side of the file compare.
 * @param [in] index The document to re-load.
 * @return Open result code.
 */
void CMergeDoc::RefreshOptions()
{
	DIFFOPTIONS options = {0};
	
	m_diffWrapper.SetDetectMovedBlocks(GetOptionsMgr()->GetBool(OPT_CMP_MOVED_BLOCKS));
	Options::DiffOptions::Load(GetOptionsMgr(), options);

	m_diffWrapper.SetOptions(&options);

	// Refresh view options
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		m_pView[nBuffer]->RefreshOptions();
		m_pDetailView[nBuffer]->RefreshOptions();
	}
}

/**
 * @brief Write path and filename to headerbar
 * @note SetText() does not repaint unchanged text
 */
void CMergeDoc::UpdateHeaderPath(int pane)
{
	CChildFrame *pf = GetParentFrame();
	ASSERT(pf);
	String sText;
	bool bChanges = false;

	if (m_nBufferType[pane] == BUFFER_UNNAMED ||
		m_nBufferType[pane] == BUFFER_NORMAL_NAMED)
	{
		sText = m_strDesc[pane];
	}
	else
	{
		sText = m_filePaths[pane];
		if (m_pDirDoc)
		{
			m_pDirDoc->ApplyDisplayRoot(pane, sText);
		}
	}
	bChanges = m_ptBuf[pane]->IsModified();

	if (bChanges)
		sText.insert(0, _T("* "));

	pf->GetHeaderInterface()->SetText(pane, sText);

	SetTitle(NULL);
}

/**
 * @brief Paint differently the headerbar of the active view
 */
void CMergeDoc::UpdateHeaderActivity(int pane, bool bActivate)
{
	CChildFrame *pf = GetParentFrame();
	ASSERT(pf);
	pf->GetHeaderInterface()->SetActive(pane, bActivate);
}

/**
 * @brief Set detect/not detect Moved Blocks
 */
void CMergeDoc::SetDetectMovedBlocks(bool bDetectMovedBlocks)
{
	if (bDetectMovedBlocks == m_diffWrapper.GetDetectMovedBlocks())
		return;

	GetOptionsMgr()->SaveOption(OPT_CMP_MOVED_BLOCKS, bDetectMovedBlocks);
	m_diffWrapper.SetDetectMovedBlocks(bDetectMovedBlocks);
	FlushAndRescan();
}

/**
 * @brief Check if given buffer has mixed EOL style.
 * @param [in] nBuffer Buffer to check.
 * @return true if buffer's EOL style is mixed, false otherwise.
 */
bool CMergeDoc::IsMixedEOL(int nBuffer) const
{
	CDiffTextBuffer *pBuf = m_ptBuf[nBuffer].get();
	return pBuf->IsMixedEOL();
}

void CMergeDoc::SetEditedAfterRescan(int nBuffer)
{
	m_bEditAfterRescan[nBuffer] = true;
}

/**
 * @brief Update document filenames to title
 */
void CMergeDoc::SetTitle(LPCTSTR lpszTitle)
{
	String sTitle;
	String sFileName[3];

	if (lpszTitle)
		sTitle = lpszTitle;
	else
	{
		for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
			sFileName[nBuffer] = !m_strDesc[nBuffer].empty() ? m_strDesc[nBuffer] : paths::FindFileName(m_filePaths[nBuffer]);
		if (std::count(&sFileName[0], &sFileName[0] + m_nBuffers, sFileName[0]) == m_nBuffers)
			sTitle = sFileName[0] + strutils::format(_T(" x %d"), m_nBuffers);
		else
			sTitle = strutils::join(&sFileName[0], &sFileName[0] + m_nBuffers, _T(" - "));
	}
	CDocument::SetTitle(sTitle.c_str());
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CMergeDoc::UpdateResources()
{
	CString str;
	int nBuffer;

	m_strDesc[0] = _("Untitled left");
	m_strDesc[m_nBuffers - 1] = _("Untitled right");
	if (m_nBuffers == 3)
		m_strDesc[1] = _("Untitled middle");
	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		UpdateHeaderPath(nBuffer);

	GetParentFrame()->UpdateResources();
	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		GetView(nBuffer)->UpdateResources();
}

// Return current word breaking break type setting (whitespace only or include punctuation)
bool CMergeDoc::GetBreakType() const
{
	bool breakType = !!GetOptionsMgr()->GetInt(OPT_BREAK_TYPE);
	return breakType;
}

// Return true to do line diff colors at the byte level (false to do them at word level)
bool CMergeDoc::GetByteColoringOption() const
{
	// color at byte level if 'break_on_words' option not set
	bool breakWords = GetOptionsMgr()->GetBool(OPT_BREAK_ON_WORDS);
	return !breakWords;
}

/// Swap files and update views
void CMergeDoc::SwapFiles()
{
	// Swap views
	int nLeftViewId = m_pView[0]->GetDlgCtrlID();
	int nRightViewId = m_pView[m_nBuffers - 1]->GetDlgCtrlID();
	m_pView[0]->SetDlgCtrlID(nRightViewId);
	m_pView[m_nBuffers - 1]->SetDlgCtrlID(nLeftViewId);

	int nLeftDetailViewId = m_pDetailView[0]->GetDlgCtrlID();
	int nRightDetailViewId = m_pDetailView[m_nBuffers - 1]->GetDlgCtrlID();
	m_pDetailView[0]->SetDlgCtrlID(nRightDetailViewId);
	m_pDetailView[m_nBuffers - 1]->SetDlgCtrlID(nLeftDetailViewId);

	// Swap buffers and so on
	std::swap(m_ptBuf[0], m_ptBuf[m_nBuffers - 1]);
	std::swap(m_pView[0], m_pView[m_nBuffers - 1]);
	std::swap(m_pDetailView[0], m_pDetailView[m_nBuffers - 1]);
	std::swap(m_pSaveFileInfo[0], m_pSaveFileInfo[m_nBuffers - 1]);
	std::swap(m_pRescanFileInfo[0], m_pRescanFileInfo[m_nBuffers - 1]);
	std::swap(m_nBufferType[0], m_nBufferType[m_nBuffers - 1]);
	std::swap(m_bEditAfterRescan[0], m_bEditAfterRescan[m_nBuffers - 1]);
	std::swap(m_strDesc[0], m_strDesc[m_nBuffers - 1]);
	m_strDesc[0].swap(m_strDesc[1]);

	m_filePaths.Swap();
	m_diffList.Swap(0, m_nBuffers - 1);
	swap(m_pView[0]->m_piMergeEditStatus, m_pView[m_nBuffers - 1]->m_piMergeEditStatus);

	ClearWordDiffCache();

	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		m_ptBuf[nBuffer]->m_nThisPane = nBuffer;
		m_pView[nBuffer]->m_nThisPane = nBuffer;
		m_pDetailView[nBuffer]->m_nThisPane = nBuffer;

		// Update views
		UpdateHeaderPath(nBuffer);
	}
	GetParentFrame()->UpdateSplitter();
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		m_pView[nBuffer]->UpdateStatusbar();

	UpdateAllViews(NULL);
}

/**
 * @brief Display unpacker dialog to user & handle user's choices
 */
bool CMergeDoc::OpenWithUnpackerDialog()
{
	// let the user choose a handler
	CSelectUnpackerDlg dlg(m_filePaths[0], NULL);
	// create now a new infoUnpacker to initialize the manual/automatic flag
	PackingInfo infoUnpacker(PLUGIN_AUTO);
	dlg.SetInitialInfoHandler(&infoUnpacker);

	if (dlg.DoModal() == IDOK)
	{
		infoUnpacker = dlg.GetInfoHandler();
		Merge7zFormatMergePluginScope scope(&infoUnpacker);
		if (HasZipSupport() && std::count_if(m_filePaths.begin(), m_filePaths.end(), ArchiveGuessFormat) == m_nBuffers)
		{
			DWORD dwFlags[3] = {FFILEOPEN_NOMRU, FFILEOPEN_NOMRU, FFILEOPEN_NOMRU};
			GetMainFrame()->DoFileOpen(&m_filePaths, dwFlags, m_strDesc, _T(""), 
				GetOptionsMgr()->GetBool(OPT_CMP_INCLUDE_SUBDIRS), NULL, _T(""), &infoUnpacker);
			CloseNow();
		}
		else
		{
			SetUnpacker(&infoUnpacker);
			OnFileReload();
		}
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief Update "Reload" item
 */
void CMergeDoc::OnUpdateFileReload(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(true);
}

/**
 * @brief Reloads the opened files
 */
void CMergeDoc::OnFileReload()
{
	if (!PromptAndSaveIfNeeded(true))
		return;
	
	FileLocation fileloc[3];
	bool bRO[3];
	for (int pane = 0; pane < m_nBuffers; pane++)
	{
		bRO[pane] = m_ptBuf[pane]->GetReadOnly();
		fileloc[pane].encoding.m_unicoding = m_ptBuf[pane]->getUnicoding();
		fileloc[pane].encoding.m_codepage = m_ptBuf[pane]->getCodepage();
		fileloc[pane].setPath(m_filePaths[pane]);
	}
	int nActivePane = GetActiveMergeView()->m_nThisPane;
	CPoint pt = m_pView[nActivePane]->GetCursorPos();
	OpenDocs(m_nBuffers, fileloc, bRO, m_strDesc, nActivePane, pt.y);
}

/**
 * @brief Display encodings to user
 */
void CMergeDoc::OnFileEncoding()
{
	DoFileEncodingDialog();
}

/**
 * @brief Update "File Encoding" item
 */
void CMergeDoc::OnUpdateFileEncoding(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(true);
}

void CMergeDoc::OnCtxtOpenWithUnpacker() 
{
	OpenWithUnpackerDialog();
}

void CMergeDoc::OnUpdateCtxtOpenWithUnpacker(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(true);
}

void CMergeDoc::OnBnClickedFileEncoding()
{
	m_pView[0]->GetParentFrame()->ShowControlBar(m_pEncodingErrorBar.get(), FALSE, FALSE);
	DoFileEncodingDialog();
}

void CMergeDoc::OnBnClickedPlugin()
{
	m_pView[0]->GetParentFrame()->ShowControlBar(m_pEncodingErrorBar.get(), FALSE, FALSE);
	OpenWithUnpackerDialog();
}

void CMergeDoc::OnBnClickedHexView()
{
	DWORD dwFlags[3] = { 0 };
	FileLocation fileloc[3];
	for (int pane = 0; pane < m_nBuffers; pane++)
	{
		fileloc[pane].setPath(m_filePaths[pane]);
		dwFlags[pane] |= FFILEOPEN_NOMRU | (m_ptBuf[pane]->GetReadOnly() ? FFILEOPEN_READONLY : 0);
	}
	if (m_pEncodingErrorBar && m_pEncodingErrorBar->IsWindowVisible())
		m_pView[0]->GetParentFrame()->ShowControlBar(m_pEncodingErrorBar.get(), FALSE, FALSE);
	GetMainFrame()->ShowHexMergeDoc(m_pDirDoc, m_nBuffers, fileloc, dwFlags, m_strDesc);
	GetParentFrame()->ShowWindow(SW_RESTORE);
	GetParentFrame()->DestroyWindow();
}

void CMergeDoc::OnOK()
{
	m_pView[0]->GetParentFrame()->ShowControlBar(m_pEncodingErrorBar.get(), FALSE, FALSE);
}

void CMergeDoc::OnFileRecompareAsXML()
{
	PackingInfo infoUnpacker(PLUGIN_BUILTIN_XML);
	SetUnpacker(&infoUnpacker);
	OnFileReload();
}

void CMergeDoc::OnFileRecompareAsBinary()
{
	OnBnClickedHexView();
}

// Return file extension either from file name or file description (if WinMerge is used as an
// external Rational ClearCase tool.
String CMergeDoc::GetFileExt(LPCTSTR sFileName, LPCTSTR sDescription) const
{
	String sExt;
	paths::SplitFilename(sFileName, NULL, NULL, &sExt);

	if (theApp.m_bClearCaseTool)
	{
		// If no extension found in real file name.
		if (sExt.empty())
		{
			paths::SplitViewName(sFileName, NULL, NULL, &sExt);
		}
		// If no extension found in repository file name.
		if (true == sExt.empty())
		{
			paths::SplitViewName(sDescription, NULL, NULL, &sExt);
		}
	}
	return sExt;
}

/**
 * @brief Generate report from file compare results.
 */
bool CMergeDoc::GenerateReport(const String& sFileName) const
{
	// calculate HTML font size
	LOGFONT lf;
	CDC dc;
	dc.CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	m_pView[0]->GetFont(lf);
	int nFontSize = -MulDiv (lf.lfHeight, 72, dc.GetDeviceCaps (LOGPIXELSY));

	// create HTML report
	UniStdioFile file;
	if (!file.Open(sFileName, _T("wt")))
	{
		String errMsg = GetSysError(GetLastError());
		String msg = strutils::format_string1(
			_("Error creating the report:\n%1"), errMsg);
		AfxMessageBox(msg.c_str(), MB_OK | MB_ICONSTOP);
		return false;
	}

	file.SetCodepage(ucr::CP_UTF_8);

	CString headerText =
		_T("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n")
		_T("\t\"http://www.w3.org/TR/html4/loose.dtd\">\n")
		_T("<html>\n")
		_T("<head>\n")
		_T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n")
		_T("<title>WinMerge File Compare Report</title>\n")
		_T("<style type=\"text/css\">\n")
		_T("<!--\n")
		_T("td,th {word-break: break-all; font-size: %dpt;}\n")
		_T("tr { vertical-align: top; }\n")
		_T(".border { border-radius: 6px; border: 1px #a0a0a0 solid; box-shadow: 1px 1px 2px rgba(0, 0, 0, 0.15); overflow: hidden; }\n")
		_T(".ln {text-align: right; word-break: normal; background-color: lightgrey; box-shadow: inset 1px 0px 0px rgba(0, 0, 0, 0.10);}\n")
		_T(".title {color: white; background-color: blue; vertical-align: top; padding: 4px 4px; background: linear-gradient(mediumblue, darkblue);}\n")
		_T("%s")
		_T("-->\n")
		_T("</style>\n")
		_T("</head>\n")
		_T("<body>\n")
		_T("<div class=\"border\">")
		_T("<table cellspacing=\"0\" cellpadding=\"0\" style=\"width: 100%%; margin: 0; border: none;\">\n")
		_T("<thead>\n")
		_T("<tr>\n");
	String header = 
		strutils::format((LPCTSTR)headerText, nFontSize, (LPCTSTR)m_pView[0]->GetHTMLStyles());
	file.WriteString(header);

	// Get paths
	// If archive, use archive path + folder + filename inside archive
	// If desc text given, use it
	PathContext paths = m_filePaths;
	if (m_pDirDoc && m_pDirDoc->IsArchiveFolders())
	{
		for (int i = 0; i < paths.GetSize(); i++)
			m_pDirDoc->ApplyDisplayRoot(i, paths[i]);
	}
	else
	{
		for (int i = 0; i < paths.GetSize(); i++)
		{
			if (!m_strDesc[i].empty())
				paths[i] = m_strDesc[i];
		}
	}

	// left and right title
	int nBuffer;
	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		int nLineNumberColumnWidth = 1;
		String data = strutils::format(_T("<th class=\"title\" style=\"width:%d%%\"></th>"), 
			nLineNumberColumnWidth);
		file.WriteString(data);
		data = strutils::format(_T("<th class=\"title\" style=\"width:%f%%\">"),
			(double)(100 - nLineNumberColumnWidth * m_nBuffers) / m_nBuffers);
		file.WriteString(data);
		file.WriteString(paths[nBuffer].c_str());
		file.WriteString(_T("</th>\n"));
	}
	file.WriteString(
		_T("</tr>\n")
		_T("</thead>\n")
		_T("<tbody>\n"));

	// write the body of the report
	int idx[3] = {0};
	int nLineCount[3] = {0};
	int nDiff = 0;
	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		nLineCount[nBuffer] = m_ptBuf[nBuffer]->GetLineCount();

	for (;;)
	{
		file.WriteString(_T("<tr>\n"));
		for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			for (; idx[nBuffer] < nLineCount[nBuffer]; idx[nBuffer]++)
			{
				if (m_pView[nBuffer]->GetLineVisible(idx[nBuffer]))
					break;
			}
				
			if (idx[nBuffer] < nLineCount[nBuffer])
			{
				// line number
				String tdtag = _T("<td class=\"ln\">");
				DWORD dwFlags = m_ptBuf[nBuffer]->GetLineFlags(idx[nBuffer]);
				if (nBuffer == 0 && 
				     (dwFlags & (LF_DIFF | LF_GHOST)) && (idx[nBuffer] == 0 || 
				    !(m_ptBuf[nBuffer]->GetLineFlags(idx[nBuffer] - 1) & (LF_DIFF | LF_GHOST))))
				{
					++nDiff;
					tdtag += strutils::format(_T("<a name=\"d%d\" href=\"#d%d\">.</a>"), nDiff, nDiff);
				}
				if (!(dwFlags & LF_GHOST) && m_pView[nBuffer]->GetViewLineNumbers())
					tdtag += strutils::format(_T("%d</td>"), m_ptBuf[nBuffer]->ComputeRealLine(idx[nBuffer]) + 1);
				else
					tdtag += _T("</td>");
				file.WriteString(tdtag);
				// write a line on left/right side
				file.WriteString((LPCTSTR)m_pView[nBuffer]->GetHTMLLine(idx[nBuffer], _T("td")));
				idx[nBuffer]++;
			}
			else
				file.WriteString(_T("<td class=\"ln\"></td><td></td>"));
			file.WriteString(_T("\n"));
		}
		file.WriteString(_T("</tr>\n"));

		bool bBorderLine = false;
		for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			if (idx[nBuffer] < nLineCount[nBuffer] && !m_pView[nBuffer]->GetLineVisible(idx[nBuffer]))
				bBorderLine = true;
		}

		if (bBorderLine)
		{
			file.WriteString(_T("<tr height=1>"));
			for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
			{
				if (idx[nBuffer] < nLineCount[nBuffer] && !m_pView[nBuffer]->GetLineVisible(idx[nBuffer]))
					file.WriteString(_T("<td style=\"background-color: black\"></td><td style=\"background-color: black\"></td>"));
				else
					file.WriteString(_T("<td></td><td></td>"));
			}
			file.WriteString(_T("</tr>\n"));
		}

		if (idx[0] >= nLineCount[0] && idx[1] >= nLineCount[1] && (m_nBuffers < 3 || idx[2] >= nLineCount[2]))
			break;
	}
	file.WriteString(
		_T("</tbody>\n")
		_T("</table>\n")
		_T("</div>")
		_T("</body>\n")
		_T("</html>\n"));

	file.Close();

	return true;
}

/**
 * @brief Generate report from file compare results.
 */
void CMergeDoc::OnToolsGenerateReport()
{
	String s;
	CString folder;

	if (!SelectFile(AfxGetMainWnd()->GetSafeHwnd(), s, folder, _("Save As"), _("HTML Files (*.htm,*.html)|*.htm;*.html|All Files (*.*)|*.*||"), false, _T("htm")))
		return;

	GenerateReport(s.c_str());

	LangMessageBox(IDS_REPORT_SUCCESS, MB_OK | MB_ICONINFORMATION);
}

/**
 * @brief Generate patch from files selected.
 *
 * Creates a patch from selected files in active directory compare, or
 * active file compare. Files in file compare must be saved before
 * creating a patch.
 */
void CMergeDoc::OnToolsGeneratePatch()
{
	// If there are changes in files, tell user to save them first
	if (IsModified())
	{
		LangMessageBox(IDS_SAVEFILES_FORPATCH, MB_ICONSTOP);
		return;
	}

	CPatchTool patcher;
	patcher.AddFiles(m_filePaths.GetLeft(),
			m_filePaths.GetRight());
	patcher.CreatePatch();
}

/**
 * @brief Add synchronization point
 */
void CMergeDoc::AddSyncPoint()
{
	int nLine[3];
	for (int nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
	{
		 int tmp = m_pView[nBuffer]->GetCursorPos().y;
		 nLine[nBuffer] = m_ptBuf[nBuffer]->ComputeApparentLine(m_ptBuf[nBuffer]->ComputeRealLine(tmp));

		if (m_ptBuf[nBuffer]->GetLineFlags(nLine[nBuffer]) & LF_INVALID_BREAKPOINT)
			DeleteSyncPoint(nBuffer, nLine[nBuffer], false);
	}
	
	for (int nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
		m_ptBuf[nBuffer]->SetLineFlag(nLine[nBuffer], LF_INVALID_BREAKPOINT, true, false);

	m_bHasSyncPoints = true;

	for (int nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
		m_pView[nBuffer]->SetSelectionMargin(true);

	FlushAndRescan(true);
}

/**
 * @brief Delete a synchronization point
 */
bool CMergeDoc::DeleteSyncPoint(int pane, int nLine, bool bRescan)
{
	const auto syncpoints = GetSyncPointList();	
	for (auto syncpnt : syncpoints)
	{
		if (syncpnt[pane] == nLine)
		{
			for (int nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
				m_ptBuf[nBuffer]->SetLineFlag(syncpnt[nBuffer], LF_INVALID_BREAKPOINT, false, false);
		}
	}

	if (syncpoints.size() == 1)
		m_bHasSyncPoints = false;

	if (bRescan)
		FlushAndRescan(true);
	return true;
}

/**
 * @brief Clear Synchronization points
 */
void CMergeDoc::ClearSyncPoints()
{
	if (!m_bHasSyncPoints)
		return;

	for (int nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
	{
		int nLineCount = m_ptBuf[nBuffer]->GetLineCount();
		for (int nLine = 0; nLine < nLineCount; ++nLine)
		{
			if (m_ptBuf[nBuffer]->GetLineFlags(nLine) & LF_INVALID_BREAKPOINT)
				m_ptBuf[nBuffer]->SetLineFlag(nLine, LF_INVALID_BREAKPOINT, false, false);
		}
	}
	
	m_bHasSyncPoints = false;

	FlushAndRescan(true);
}

/**
 * @brief return true if there are synchronization points
 */
bool CMergeDoc::HasSyncPoints()
{
	return m_bHasSyncPoints;
}

std::vector<std::vector<int> > CMergeDoc::GetSyncPointList()
{
	std::vector<std::vector<int> > list;
	if (!m_bHasSyncPoints)
		return list;
	int idx[3] = {-1, -1, -1};
	std::vector<int> points(m_nBuffers);
	for (int nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
		points[nBuffer] = m_ptBuf[nBuffer]->GetLineCount() - 1;
	for (int nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
	{
		int nLineCount = m_ptBuf[nBuffer]->GetLineCount();
		for (int nLine = 0; nLine < nLineCount; ++nLine)
		{
			if (m_ptBuf[nBuffer]->GetLineFlags(nLine) & LF_INVALID_BREAKPOINT)
			{
				idx[nBuffer]++;
				if (static_cast<int>(list.size()) <= idx[nBuffer])
					list.push_back(points);
				list[idx[nBuffer]][nBuffer] = nLine;
			}
		}
	}
	return list;
}

