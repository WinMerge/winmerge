/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  MergeDoc.cpp
 *
 * @brief Implementation file for CMergeDoc
 *
 */

#include "StdAfx.h"
#include "MergeDoc.h"
#include <Poco/Timestamp.h>
#include "UnicodeString.h"
#include "Merge.h"
#include "MainFrm.h"
#include "DiffTextBuffer.h"
#include "Environment.h"
#include "MovedLines.h"
#include "MergeEditView.h"
#include "MergeEditFrm.h"
#include "IDirDoc.h"
#include "FileLoadResult.h"
#include "FileTransform.h"
#include "Plugins.h"
#include "unicoder.h"
#include "UniFile.h"
#include "OptionsDef.h"
#include "DiffFileInfo.h"
#include "SaveClosingDlg.h"
#include "OpenTableDlg.h"
#include "DiffList.h"
#include "paths.h"
#include "OptionsMgr.h"
#include "OptionsDiffOptions.h"
#include "MergeLineFlags.h"
#include "FileOrFolderSelect.h"
#include "LineFiltersList.h"
#include "SubstitutionFiltersList.h"
#include "FileFilterHelper.h"
#include "TempFile.h"
#include "codepage_detect.h"
#include "SelectPluginDlg.h"
#include "EncodingErrorBar.h"
#include "MergeCmdLineInfo.h"
#include "TFile.h"
#include "PatchTool.h"
#include "charsets.h"
#include "markdown.h"
#include "stringdiffs.h"
#include "Logger.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using std::swap;

int CMergeDoc::m_nBuffersTemp = 2;

static int SaveBuffForDiff(CDiffTextBuffer & buf, const String& filepath, int nStartLine = 0, int nLines = -1);

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc

IMPLEMENT_DYNCREATE(CMergeDoc, CDocument)

BEGIN_MESSAGE_MAP(CMergeDoc, CDocument)
	//{{AFX_MSG_MAP(CMergeDoc)
	// [File] menu
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_FILE_SAVE_LEFT, OnFileSaveLeft)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_LEFT, OnUpdateFileSaveLeft)
	ON_COMMAND(ID_FILE_SAVE_MIDDLE, OnFileSaveMiddle)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_MIDDLE, OnUpdateFileSaveMiddle)
	ON_COMMAND(ID_FILE_SAVE_RIGHT, OnFileSaveRight)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_RIGHT, OnUpdateFileSaveRight)
	ON_COMMAND(ID_FILE_SAVEAS_LEFT, OnFileSaveAsLeft)
	ON_COMMAND(ID_FILE_SAVEAS_MIDDLE, OnFileSaveAsMiddle)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEAS_MIDDLE, OnUpdateFileSaveAsMiddle)
	ON_COMMAND(ID_FILE_SAVEAS_RIGHT, OnFileSaveAsRight)
	ON_COMMAND(ID_FILE_LEFT_READONLY, OnFileReadOnlyLeft)
	ON_UPDATE_COMMAND_UI(ID_FILE_LEFT_READONLY, OnUpdateFileReadOnlyLeft)
	ON_COMMAND(ID_FILE_MIDDLE_READONLY, OnFileReadOnlyMiddle)
	ON_UPDATE_COMMAND_UI(ID_FILE_MIDDLE_READONLY, OnUpdateFileReadOnlyMiddle)
	ON_COMMAND(ID_FILE_RIGHT_READONLY, OnFileReadOnlyRight)
	ON_UPDATE_COMMAND_UI(ID_FILE_RIGHT_READONLY, OnUpdateFileReadOnlyRight)
	ON_COMMAND(ID_RESCAN, OnFileReload)
	ON_COMMAND(ID_FILE_ENCODING, OnFileEncoding)
	ON_COMMAND(ID_MERGE_COMPARE_TEXT, OnFileRecompareAsText)
	ON_UPDATE_COMMAND_UI(ID_MERGE_COMPARE_TEXT, OnUpdateFileRecompareAsText)
	ON_COMMAND(ID_MERGE_COMPARE_TABLE, OnFileRecompareAsTable)
	ON_UPDATE_COMMAND_UI(ID_MERGE_COMPARE_TABLE, OnUpdateFileRecompareAsTable)
	ON_COMMAND_RANGE(ID_MERGE_COMPARE_HEX, ID_MERGE_COMPARE_WEBPAGE, OnFileRecompareAs)
	ON_COMMAND_RANGE(ID_UNPACKERS_FIRST, ID_UNPACKERS_LAST, OnFileRecompareAs)
	// [View] menu
	ON_COMMAND_RANGE(ID_VIEW_DIFFCONTEXT_ALL, ID_VIEW_DIFFCONTEXT_INVERT, OnDiffContext)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_DIFFCONTEXT_ALL, ID_VIEW_DIFFCONTEXT_INVERT, OnUpdateDiffContext)
	ON_COMMAND(ID_SWAPPANES_SWAP12, (OnViewSwapPanes<0, 1>))
	ON_COMMAND(ID_SWAPPANES_SWAP23, (OnViewSwapPanes<1, 2>))
	ON_COMMAND(ID_SWAPPANES_SWAP13, (OnViewSwapPanes<0, 2>))
	ON_UPDATE_COMMAND_UI_RANGE(ID_SWAPPANES_SWAP23, ID_SWAPPANES_SWAP13, OnUpdateSwapContext)
	ON_COMMAND(ID_REFRESH, OnRefresh)
	// [Tools] menu
	ON_COMMAND(ID_TOOLS_GENERATEREPORT, OnToolsGenerateReport)
	ON_COMMAND(ID_TOOLS_GENERATEPATCH, OnToolsGeneratePatch)
	// [Plugins] menu
	ON_COMMAND(ID_OPEN_WITH_UNPACKER, OnOpenWithUnpacker)
	ON_COMMAND(ID_APPLY_PREDIFFER, OnApplyPrediffer)
	ON_COMMAND_RANGE(ID_NO_PREDIFFER, ID_NO_PREDIFFER, OnPrediffer)
	ON_COMMAND_RANGE(ID_PREDIFFERS_FIRST, ID_PREDIFFERS_LAST, OnPrediffer)
	ON_UPDATE_COMMAND_UI(ID_NO_PREDIFFER, OnUpdatePrediffer)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PREDIFFERS_FIRST, ID_PREDIFFERS_LAST, OnUpdatePrediffer)
	ON_COMMAND_RANGE(ID_SCRIPT_FOR_COPYING_FIRST, ID_SCRIPT_FOR_COPYING_LAST, OnScriptsForCopying)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SCRIPT_FOR_COPYING_FIRST, ID_SCRIPT_FOR_COPYING_LAST, OnUpdateScriptsForCopying)
	ON_COMMAND(ID_SELECT_EDITOR_SCRIPT_FOR_COPYING, OnSelectEditorScriptForCopying)
	// Encoding Error dialog
	ON_BN_CLICKED(IDC_FILEENCODING, OnBnClickedFileEncoding)
	ON_BN_CLICKED(IDC_PLUGIN, OnBnClickedPlugin)
	ON_BN_CLICKED(IDC_HEXVIEW, OnBnClickedHexView)
	ON_COMMAND(IDOK, OnOK)	
	// Status bar
	ON_UPDATE_COMMAND_UI(ID_STATUS_PANE0FILE_RO, OnUpdateStatusRO)
	ON_UPDATE_COMMAND_UI(ID_STATUS_PANE1FILE_RO, OnUpdateStatusRO)
	ON_UPDATE_COMMAND_UI(ID_STATUS_PANE2FILE_RO, OnUpdateStatusRO)
	ON_UPDATE_COMMAND_UI(ID_STATUS_DIFFNUM, OnUpdateStatusNum)
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
, m_CurWordDiff{ -1, static_cast<size_t>(-1), -1 }
, m_pDirDoc(nullptr)
, m_bMixedEol(false)
, m_pEncodingErrorBar(nullptr)
, m_bHasSyncPoints(false)
, m_bAutoMerged(false)
, m_nGroups(0)
, m_pView{nullptr}
, m_bAutomaticRescan(false)
, m_CurrentPredifferID(0)
, m_CurrentEditorScriptID(ID_SCRIPT_FOR_COPYING_NONE)
, m_bChangedSchemeManually(false)
, m_editorScriptInfo(_T(""))
{
	DIFFOPTIONS options = {0};

	m_nBuffers = m_nBuffersTemp;
	m_filePaths.SetSize(m_nBuffers);

	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		m_ptBuf[nBuffer].reset(new CDiffTextBuffer(this, nBuffer));
		m_pSaveFileInfo[nBuffer].reset(new DiffFileInfo());
		m_pRescanFileInfo[nBuffer].reset(new DiffFileInfo());
		m_nBufferType[nBuffer] = BUFFERTYPE::NORMAL;
		m_bEditAfterRescan[nBuffer] = false;
	}

	m_bEnableRescan = true;
	m_bAutomaticRescan = GetOptionsMgr()->GetBool(OPT_AUTOMATIC_RESCAN);

	// COleDateTime m_LastRescan
	curUndo = undoTgt.begin();
	m_nDiffContext = GetOptionsMgr()->GetInt(OPT_DIFF_CONTEXT);
	m_bInvertDiffContext = GetOptionsMgr()->GetBool(OPT_INVERT_DIFF_CONTEXT);

	m_diffWrapper.SetDetectMovedBlocks(GetOptionsMgr()->GetBool(OPT_CMP_MOVED_BLOCKS));
	Options::DiffOptions::Load(GetOptionsMgr(), options);

	m_diffWrapper.SetOptions(&options);
	m_diffWrapper.SetPrediffer(nullptr);
}

/**
 * @brief Destructor.
 *
 * Informs associated dirdoc that mergedoc is closing.
 */
CMergeDoc::~CMergeDoc()
{	
	GetMainFrame()->UnwatchDocuments(this);

	if (m_pDirDoc != nullptr)
	{
		m_pDirDoc->MergeDocClosing(this);
		m_pDirDoc = nullptr;
	}
}

/**
 * @brief Deleted data associated with doc before closing.
 */
void CMergeDoc::DeleteContents ()
{
	CDocument::DeleteContents ();
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		m_ptBuf[nBuffer]->FreeAll ();
		m_tempFiles[nBuffer].Delete();
	}
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
 * @brief Return active merge edit view (or left one if neither active)
 */
CMergeEditView * CMergeDoc::GetActiveMergeView()
{
	CView * pActiveView = GetParentFrame() ? GetParentFrame()->GetActiveView() : nullptr;
	CMergeEditView * pMergeEditView = dynamic_cast<CMergeEditView *>(pActiveView);
	if (pMergeEditView == nullptr)
		pMergeEditView = GetView(0, 0); // default to left view (in case some location or detail view active)
	return pMergeEditView;
}

CMergeEditView * CMergeDoc::GetActiveMergeGroupView(int nBuffer)
{
	return m_pView[GetActiveMergeView()->m_nThisGroup][nBuffer];
}

void CMergeDoc::SetUnpacker(const PackingInfo * infoNewHandler)
{
	if (infoNewHandler != nullptr)
	{
		m_infoUnpacker = *infoNewHandler;
	}
}

void CMergeDoc::SetPrediffer(const PrediffingInfo * infoPrediffer)
{
	m_diffWrapper.SetPrediffer(infoPrediffer);
}

void CMergeDoc::GetPrediffer(PrediffingInfo * infoPrediffer) const
{
	m_diffWrapper.GetPrediffer(infoPrediffer);
}

const PrediffingInfo* CMergeDoc::GetPrediffer() const
{
	static PrediffingInfo infoPrediffer;
	m_diffWrapper.GetPrediffer(&infoPrediffer);
	return &infoPrediffer;
}

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc serialization

void CMergeDoc::Serialize(CArchive& ar)
{
	ASSERT(false); // we do not use CDocument serialization
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
static int SaveBuffForDiff(CDiffTextBuffer & buf, const String& filepath, int nStartLine, int nLines)
{
	// and we don't repack the file
	PackingInfo tempPacker(false);

	// write buffer out to temporary file
	String sError;
	return buf.SaveToFile(filepath, true, sError, tempPacker,
		CRLFSTYLE::AUTOMATIC, false, nStartLine, nLines);
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
	FileChange Changed[3] = {FileChange::NoChange, FileChange::NoChange, FileChange::NoChange};
	int nBuffer;

	if (!bForced)
	{
		if (!m_bEnableRescan)
			return RESCAN_SUPPRESSED;
	}

	ClearWordDiffCache();

	m_diffWrapper.SetFilterList(
		GetOptionsMgr()->GetBool(OPT_LINEFILTER_ENABLED) ?
		theApp.m_pLineFilters->MakeFilterList() : nullptr);
	m_diffWrapper.SetSubstitutionList(
		(theApp.m_pSubstitutionFiltersList && theApp.m_pSubstitutionFiltersList->GetEnabled()) ?
		theApp.m_pSubstitutionFiltersList->MakeSubstitutionList() : nullptr);

	if (GetView(0, 0)->m_CurSourceDef->type != 0)
		m_diffWrapper.SetFilterCommentsSourceDef(GetView(0, 0)->m_CurSourceDef);
	else
		m_diffWrapper.SetFilterCommentsSourceDef(GetFileExt(m_ptBuf[0]->m_strTempFileName.c_str(), m_strDesc[0].c_str()));

	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		// Check if files have been modified since last rescan
		// Ignore checking in case of scratchpads (empty filenames)
		if (!m_filePaths[nBuffer].empty() && !paths::IsURL(m_filePaths[nBuffer]))
		{
			Changed[nBuffer] = IsFileChangedOnDisk(m_filePaths[nBuffer].c_str(),
					fileInfo, false, nBuffer);
		}
	}
	m_LastRescan = COleDateTime::GetCurrentTime();

	const tchar_t* tnames[] = {_T("t0_wmdoc"), _T("t1_wmdoc"), _T("t2_wmdoc")};
	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		if (Changed[nBuffer] == FileChange::Removed)
		{
			String msg = strutils::format_string1(_("File\n%1\nnot found. Save a copy to continue."), m_filePaths[nBuffer]);
			ShowMessageBox(msg, MB_ICONWARNING);
			bool bSaveResult = false;
			bool ok = DoSaveAs(m_filePaths[nBuffer].c_str(), bSaveResult, nBuffer);
			if (!ok || !bSaveResult)
			{
				return RESCAN_FILE_ERR;
			}
		}

		String temp = m_tempFiles[nBuffer].GetPath();
		if (temp.empty())
			temp = m_tempFiles[nBuffer].Create(tnames[nBuffer]);
		if (temp.empty())
			return RESCAN_TEMP_ERR;
	}

	CheckFileChanged();

	String tempPath = env::GetTemporaryPath();

	// Set up DiffWrapper
	m_diffWrapper.GetOptions(&diffOptions);

	// Clear diff list
	m_diffList.Clear();
	m_nCurDiff = -1;
	m_CurWordDiff = { -1, static_cast<size_t>(-1), -1 };
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

	DIFFSTATUS status;

	if (!HasSyncPoints())
	{
		// Save text buffer to file
		for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			m_ptBuf[nBuffer]->SetTempPath(tempPath);
			SaveBuffForDiff(*m_ptBuf[nBuffer], m_tempFiles[nBuffer].GetPath());
		}

		m_diffWrapper.SetCreateDiffList(&m_diffList);
		diffSuccess = m_diffWrapper.RunFileDiff();

		// Read diff-status
		m_diffWrapper.GetDiffStatus(&status);
		if (bBinary) // believe caller if we were told these are binaries
			status.bBinaries = true;
	}
	else
	{
		const std::vector<std::vector<int> > syncpoints = GetSyncPointList();	
		int nStartLine[3]{};
		int nLines[3]{}, nRealLine[3]{};
		for (size_t i = 0; i <= syncpoints.size(); ++i)
		{
			// Save text buffer to file
			for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
			{
				nLines[nBuffer] = (i >= syncpoints.size()) ? -1 : syncpoints[i][nBuffer] - nStartLine[nBuffer];
				m_ptBuf[nBuffer]->SetTempPath(tempPath);
				SaveBuffForDiff(*m_ptBuf[nBuffer], m_tempFiles[nBuffer].GetPath(), 
					nStartLine[nBuffer], nLines[nBuffer]);
			}
			DiffList templist;
			templist.Clear();
			m_diffWrapper.SetCreateDiffList(&templist);
			diffSuccess = m_diffWrapper.RunFileDiff();
			for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
				nRealLine[nBuffer] = m_ptBuf[nBuffer]->ComputeRealLine(nStartLine[nBuffer]);

			// Correct the comparison results made by diffutils if the first file separated by the sync point is an empty file.
			if (i == 0 && templist.GetSize() > 0)
			{
				for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
				{
					if (nStartLine[nBuffer] == 0)
					{
						bool isEmptyFile = true;
						for (int j = 0; j < nLines[nBuffer]; j++)
						{
							if (!(m_ptBuf[nBuffer]->GetLineFlags(nStartLine[nBuffer] + j) & LF_GHOST))
							{
								isEmptyFile = false;
								break;
							}
						}
						if (isEmptyFile)
						{
							DIFFRANGE di;
							templist.GetDiff(0, di);
							if (di.begin[nBuffer] == 0 && di.end[nBuffer] == 0)
							{
								di.end[nBuffer] = -1;
								templist.SetDiff(0, di);
							}
						}
					}
				}
			}

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

	// If one file has EOL before EOF and other not...
	if (std::count(status.bMissingNL, status.bMissingNL + m_nBuffers, status.bMissingNL[0]) < m_nBuffers)
	{
		// ..last DIFFRANGE of file which has EOL must be
		// fixed to contain last line too
		int lineCount[3] = { 0,0,0 };
		for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
			lineCount[nBuffer] = m_ptBuf[nBuffer]->GetLineCount();
		m_diffWrapper.FixLastDiffRange(m_nBuffers, lineCount, status.bMissingNL, diffOptions.bIgnoreBlankLines);
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
		ForEachView([](auto& pView) { pView->DetachFromBuffer(); });

		// Remove blank lines and clear winmerge flags
		// this operation does not change the modified flag
		for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
			m_ptBuf[nBuffer]->prepareForRescan();

		// Divide diff blocks to align similar lines.
		if (GetOptionsMgr()->GetBool(OPT_CMP_ALIGN_SIMILAR_LINES))
		{
			if (m_nBuffers < 3)
				AdjustDiffBlocks();
			else
				AdjustDiffBlocks3way();
		}

		// Analyse diff-list (updating real line-numbers)
		// this operation does not change the modified flag
		PrimeTextBuffers();

		// Hide identical lines if diff-context is not 'All'
		HideLines();

		// Apply flags to lines that are trivial
		PrediffingInfo infoPrediffer;
		GetPrediffer(&infoPrediffer);
		if (!infoPrediffer.GetPluginPipeline().empty())
			FlagTrivialLines();
		
		// Apply flags to lines that moved, to differentiate from appeared/disappeared lines
		if (m_diffWrapper.GetDetectMovedBlocks())
			FlagMovedLines();
		
		// After PrimeTextBuffers() we know amount of real diffs
		// (m_nDiffs) and trivial diffs (m_nTrivialDiffs)

		// Identical files are also updated
		if (!m_diffList.HasSignificantDiffs())
			identical = IDENTLEVEL::ALL;

		ForEachView([](auto& pView) {
			// just apply some options to the views
			pView->PrimeListWithFile();
			// Now buffers data are valid
			pView->ReAttachToBuffer();
		});
		for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			m_bEditAfterRescan[nBuffer] = false;
		}
	}

	if (!GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CODEPAGE) &&
		identical == IDENTLEVEL::ALL &&
		std::any_of(m_ptBuf, m_ptBuf + m_nBuffers,
			[&](std::unique_ptr<CDiffTextBuffer>& buf) { return buf->getEncoding() != m_ptBuf[0]->getEncoding(); }))
		identical = IDENTLEVEL::NONE;

	GetParentFrame()->SetLastCompareResult(identical != IDENTLEVEL::ALL ? 1 : 0);

	return nResult;
}

void CMergeDoc::CheckFileChanged(void)
{
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		DiffFileInfo fileInfo;
		if (IsFileChangedOnDisk(m_filePaths[nBuffer].c_str(), fileInfo, false, nBuffer)
			 == FileChange::Changed)
		{
			String msg = strutils::format_string1(_("Another application updated\n%1\nsince last scan.\n\nReload?"), m_filePaths[nBuffer]);
			if (ShowMessageBox(msg, MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN, IDS_FILECHANGED_RESCAN) == IDYES)
			{
				OnFileReload();
			}
			break;
		}
	}
}

/** @brief Apply flags to lines that are trivial */
void CMergeDoc::FlagTrivialLines(void)
{
	for (int i = 0; i < m_ptBuf[0]->GetLineCount(); ++i)
	{
		if ((m_ptBuf[0]->GetLineFlags(i) & LF_NONTRIVIAL_DIFF) == 0)
		{
			String str[3];
			for (int file = 0; file < m_nBuffers; ++file)
			{
				const tchar_t *p = m_ptBuf[file]->GetLineChars(i);
				str[file] = p ? String(p, m_ptBuf[file]->GetFullLineLength(i)) : _T("");
			}

			if (std::count(str + 1, str + m_nBuffers, str[0]) != m_nBuffers - 1)
			{
				DIFFOPTIONS diffOptions = {0};
				m_diffWrapper.GetOptions(&diffOptions);

				strdiff::EolCompareMode eolMode = diffOptions.bIgnoreLineBreaks ? strdiff::EOL_AS_SPACE :
					diffOptions.bIgnoreEol ? strdiff::EOL_IGNORE : strdiff::EOL_STRICT;

				// Make the call to stringdiffs, which does all the hard & tedious computations
				int result = strdiff::Compare(str[0], str[1],
					!diffOptions.bIgnoreCase,
					eolMode,
					diffOptions.nIgnoreWhitespace,
					diffOptions.bIgnoreNumbers);
				if (m_nBuffers >= 2 && result == 0)
				{
					result = strdiff::Compare(str[1], str[2],
						!diffOptions.bIgnoreCase,
						eolMode,
						diffOptions.nIgnoreWhitespace,
						diffOptions.bIgnoreNumbers);
				}
				if (result != 0)
				{
					for (int file = 0; file < m_nBuffers; ++file)
						m_ptBuf[file]->SetLineFlag(i, LF_TRIVIAL, true, false, false);
					++m_nTrivialDiffs;
				}
			}
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
		int j = pMovedLines->LineInBlock(i, MovedLines::SIDE::RIGHT);
		if (j != -1)
		{
			TRACE(_T("%d->%d\n"), i, j);
			ASSERT(j>=0);
			// We only flag lines that are already marked as being different
			int apparent = m_ptBuf[0]->ComputeApparentLine(i);
			if (m_ptBuf[0]->FlagIsSet(apparent, LF_DIFF))
			{
				m_ptBuf[0]->SetLineFlag(apparent, LF_MOVED, true, false, false);
				if (m_ptBuf[1]->FlagIsSet(apparent, LF_GHOST))
				{
					int apparentJ = m_ptBuf[1]->ComputeApparentLine(j);
					if (m_ptBuf[0]->FlagIsSet(apparentJ, LF_GHOST))
						m_ptBuf[1]->SetLineFlag(apparent, LF_MOVED, true, false, false);
				}
			}
		}
	}

	pMovedLines = m_diffWrapper.GetMovedLines(1);
	for (i=0; i<m_ptBuf[1]->GetLineCount(); ++i)
	{
		int j = pMovedLines->LineInBlock(i, MovedLines::SIDE::LEFT);
		if (j != -1)
		{
			TRACE(_T("%d->%d\n"), i, j);
			ASSERT(j>=0);
			// We only flag lines that are already marked as being different
			int apparent = m_ptBuf[1]->ComputeApparentLine(i);
			if (m_ptBuf[1]->FlagIsSet(apparent, LF_DIFF))
			{
				m_ptBuf[1]->SetLineFlag(apparent, LF_MOVED, true, false, false);
				if (m_ptBuf[0]->FlagIsSet(apparent, LF_GHOST))
				{
					int apparentJ = m_ptBuf[0]->ComputeApparentLine(j);
					if (m_ptBuf[1]->FlagIsSet(apparentJ, LF_GHOST))
						m_ptBuf[0]->SetLineFlag(apparent, LF_MOVED, true, false, false);
				}
			}
		}
	}

	if (m_nBuffers < 3)
		return;

	pMovedLines = m_diffWrapper.GetMovedLines(1);
	for (i=0; i<m_ptBuf[1]->GetLineCount(); ++i)
	{
		int j = pMovedLines->LineInBlock(i, MovedLines::SIDE::RIGHT);
		if (j != -1)
		{
			TRACE(_T("%d->%d\n"), i, j);
			ASSERT(j>=0);
			// We only flag lines that are already marked as being different
			int apparent = m_ptBuf[1]->ComputeApparentLine(i);
			if (m_ptBuf[1]->FlagIsSet(apparent, LF_DIFF))
			{
				m_ptBuf[1]->SetLineFlag(apparent, LF_MOVED, true, false, false);
				if (m_ptBuf[2]->FlagIsSet(apparent, LF_GHOST))
				{
					int apparentJ = m_ptBuf[2]->ComputeApparentLine(j);
					if (m_ptBuf[1]->FlagIsSet(apparentJ, LF_GHOST))
						m_ptBuf[2]->SetLineFlag(apparent, LF_MOVED, true, false, false);
				}
			}
		}
	}

	pMovedLines = m_diffWrapper.GetMovedLines(2);
	for (i=0; i<m_ptBuf[2]->GetLineCount(); ++i)
	{
		int j = pMovedLines->LineInBlock(i, MovedLines::SIDE::LEFT);
		if (j != -1)
		{
			TRACE(_T("%d->%d\n"), i, j);
			ASSERT(j>=0);
			// We only flag lines that are already marked as being different
			int apparent = m_ptBuf[2]->ComputeApparentLine(i);
			if (m_ptBuf[2]->FlagIsSet(apparent, LF_DIFF))
			{
				m_ptBuf[2]->SetLineFlag(apparent, LF_MOVED, true, false, false);
				if (m_ptBuf[1]->FlagIsSet(apparent, LF_GHOST))
				{
					int apparentJ = m_ptBuf[1]->ComputeApparentLine(j);
					if (m_ptBuf[2]->FlagIsSet(apparentJ, LF_GHOST))
						m_ptBuf[1]->SetLineFlag(apparent, LF_MOVED, true, false, false);
				}
			}
		}
	}

	// todo: Need to record actual moved information
}

int CMergeDoc::ShowMessageBox(const String& sText, unsigned nType, unsigned nIDHelp)
{
	if (m_pView[0][0] && m_pView[0][0]->IsTextBufferInitialized() && !GetParentFrame()->IsActivated())
	{
		GetParentFrame()->InitialUpdateFrame(this, true);
		GetParentFrame()->SendMessageToDescendants(WM_IDLEUPDATECMDUI, static_cast<WPARAM>(true), 0, true, true);
	}
	return AfxMessageBox(sText.c_str(), nType, nIDHelp);
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
		s = _("An error occurred while comparing files.");
		ShowMessageBox(s, MB_ICONSTOP);
		return;
	}

	if (nRescanResult == RESCAN_TEMP_ERR)
	{
		s = _("Could not create temporary files. Check your temporary path settings.");
		ShowMessageBox(s, MB_ICONSTOP);
		return;
	}

	// Files are not binaries, but they are identical
	if (identical != IDENTLEVEL::NONE)
	{
		if (m_pView[0][0] && m_pView[0][0]->IsTextBufferInitialized() && !GetParentFrame()->IsActivated())
		{
			GetParentFrame()->InitialUpdateFrame(this, true);
			GetParentFrame()->SendMessageToDescendants(WM_IDLEUPDATECMDUI, static_cast<WPARAM>(true), 0, true, true);
		}
		CMergeFrameCommon::ShowIdenticalMessage(m_filePaths, identical == IDENTLEVEL::ALL, !IsModified());
	}
}

bool CMergeDoc::Undo()
{
	return false;
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
	int nBuffer, PackingInfo& infoTempUnpacker)
{
	String s;
	String str;
	String strSavePath; // New path for next saving try
	String title;
	bool result = true;
	int answer = IDOK; // Set default we use for scratchpads

	// We shouldn't get here if saving is succeed before
	ASSERT(nSaveResult != SAVE_DONE);

	// Select message based on reason function called
	if (nSaveResult == SAVE_PACK_FAILED)
	{
		str = CMergeApp::GetPackingErrorMessage(nBuffer, m_nBuffers, strPath, infoTempUnpacker);
		// replace the unpacker with a "do nothing" unpacker
		infoTempUnpacker.Initialize(false);
	}
	else
	{
		str = strutils::format_string2(_("Saving file failed.\n%1\n%2\n\t- Use different filename (OK)\n\t- Abort (Cancel)?"), strPath, sError);
	}

	// SAVE_NO_FILENAME is temporarily used for scratchpad.
	// So don't ask about saving in that case.
	if (nSaveResult != SAVE_NO_FILENAME)
		answer = ShowMessageBox(str, MB_OKCANCEL | MB_ICONWARNING);

	switch (answer)
	{
	case IDOK:
		if (nBuffer == 0)
			title = _("Save Left File As");
		else if (nBuffer == m_nBuffers - 1)
			title = _("Save Right File As");
		else
			title = _("Save Middle File As");

		if (SelectFile(GetActiveMergeView()->GetSafeHwnd(), s, false, strPath.c_str(), title))
		{
			CDiffTextBuffer *pBuffer = m_ptBuf[nBuffer].get();
			strSavePath = std::move(s);
			nSaveResult = pBuffer->SaveToFile(strSavePath, false, sError,
				infoTempUnpacker);

			if (nSaveResult == SAVE_DONE)
			{
				// We are saving scratchpad (unnamed file)
				if (m_nBufferType[nBuffer] == BUFFERTYPE::UNNAMED)
				{
					m_nBufferType[nBuffer] = BUFFERTYPE::UNNAMED_SAVED;
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
 * empty/`nullptr` if new file is created (scratchpad) without filename.
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
bool CMergeDoc::DoSave(const tchar_t* szPath, bool &bSaveSuccess, int nBuffer)
{
	DiffFileInfo fileInfo;
	String strSavePath(m_strSaveAsPath.empty() ? szPath : m_strSaveAsPath);
	FileChange fileChanged;
	bool bApplyToAll = false;	
	int nRetVal = -1;

	fileChanged = IsFileChangedOnDisk(szPath, fileInfo, true, nBuffer);
	if (fileChanged == FileChange::Changed)
	{
		String msg = strutils::format_string1(_("Another application updated\n%1\nsince WinMerge loaded it.\n\nOverwrite?"), szPath);
		if (ShowMessageBox(msg, MB_ICONWARNING | MB_YESNO) == IDNO)
		{
			bSaveSuccess = true;
			return true;
		}		
	}

	// use a temp packer
	// first copy the m_infoUnpacker
	// if an error arises during packing, change and take a "do nothing" packer
	PackingInfo infoTempUnpacker = m_infoUnpacker;

	bSaveSuccess = false;
	
	nRetVal = CMergeApp::HandleReadonlySave(strSavePath, false, bApplyToAll);
	if (nRetVal == IDCANCEL)
		return false;

	if (!CMergeApp::CreateBackup(false, strSavePath))
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
	if (m_nBufferType[nBuffer] == BUFFERTYPE::UNNAMED)
		nSaveErrorCode = SAVE_NO_FILENAME;

	String sError;
	if (nSaveErrorCode == SAVE_DONE)
		// We have a filename, just try to save
		nSaveErrorCode = pBuffer->SaveToFile(strSavePath, false, sError, infoTempUnpacker);

	if (nSaveErrorCode != SAVE_DONE)
	{
		// Saving failed, user may save to another location if wants to
		do
			result = TrySaveAs(strSavePath, nSaveErrorCode, sError, nBuffer, infoTempUnpacker);
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
		m_pSaveFileInfo[nBuffer]->Update(strSavePath);
		m_filePaths[nBuffer] = std::move(strSavePath);
		m_pRescanFileInfo[nBuffer]->Update(m_filePaths[nBuffer]);
		UpdateHeaderPath(nBuffer);
		bSaveSuccess = true;
		result = true;

		CMergeFrameCommon::LogFileSaved(m_filePaths[nBuffer]);
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
 * empty/`nullptr` if new file is created (scratchpad) without filename.
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
bool CMergeDoc::DoSaveAs(const tchar_t* szPath, bool &bSaveSuccess, int nBuffer)
{
	String strSavePath(szPath);

	// use a temp packer
	// first copy the m_infoUnpacker
	// if an error arises during packing, change and take a "do nothing" packer
	PackingInfo infoTempUnpacker = m_infoUnpacker;

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
		result = TrySaveAs(strSavePath, nSaveErrorCode, sError, nBuffer, infoTempUnpacker);
	while (!result);

	// Saving succeeded with given/selected filename
	if (nSaveErrorCode == SAVE_DONE)
	{
		m_pSaveFileInfo[nBuffer]->Update(strSavePath);
		m_filePaths[nBuffer] = std::move(strSavePath);
		m_pRescanFileInfo[nBuffer]->Update(m_filePaths[nBuffer]);
		UpdateHeaderPath(nBuffer);
		bSaveSuccess = true;
		result = true;

		CMergeFrameCommon::LogFileSaved(m_filePaths[nBuffer]);
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
				MovedLines::SIDE::RIGHT);
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
				MovedLines::SIDE::LEFT);
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

	CMergeEditView *pActiveView = GetActiveMergeView();

	// store cursors and hide caret
	ForEachView([](auto& pView) { pView->PushCursors(); });
	pActiveView->HideCursor();

	bool bBinary = false;
	IDENTLEVEL identical = IDENTLEVEL::NONE;
	int nRescanResult = Rescan(bBinary, identical, bForced);

	// restore cursors and caret
	ForEachView([](auto& pView) { pView->PopCursors(); });
	pActiveView->ShowCursor();

	ForEachView(pActiveView->m_nThisPane, [](auto& pView) {
		// because of ghostlines, m_nTopLine may differ just after Rescan
		// scroll both views to the same top line
		pView->UpdateSiblingScrollPos(false);
	});
	// make sure we see the cursor from the curent view
	pActiveView->EnsureVisible(pActiveView->GetCursorPos());

	// Refresh display
	UpdateAllViews(nullptr);

	// Show possible error after updating screen
	if (nRescanResult != RESCAN_SUPPRESSED)
		ShowRescanError(nRescanResult, identical);
	m_LastRescan = COleDateTime::GetCurrentTime();
	SetTitle(nullptr);
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
		if (m_pDirDoc != nullptr && m_pDirDoc->HasDiffs())
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
		if (m_pDirDoc != nullptr && m_pDirDoc->HasDiffs())
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
 * @brief Called when "Save" item is updated
 */
void CMergeDoc::OnUpdateFileSave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsModified());
}

/**
 * @brief Called when "Save left (as...)" item is updated
 */
void CMergeDoc::OnUpdateFileSaveLeft(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_ptBuf[0]->IsModified());
}

/**
 * @brief Called when "Save middle (as...)" item is updated
 */
void CMergeDoc::OnUpdateFileSaveMiddle(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nBuffers == 3 && m_ptBuf[1]->IsModified());
}

/**
 * @brief Called when "Save right (as...)" item is updated
 */
void CMergeDoc::OnUpdateFileSaveRight(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_ptBuf[m_nBuffers - 1]->IsModified());
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
 * @brief Enable/disable left buffer read-only
 */
void CMergeDoc::OnFileReadOnlyLeft()
{
	bool bReadOnly = m_ptBuf[0]->GetReadOnly();
	m_ptBuf[0]->SetReadOnly(!bReadOnly);
}

/**
 * @brief Called when "Left read-only" item is updated
 */
void CMergeDoc::OnUpdateFileReadOnlyLeft(CCmdUI* pCmdUI)
{
	bool bReadOnly = m_ptBuf[0]->GetReadOnly();
	pCmdUI->Enable(true);
	pCmdUI->SetCheck(bReadOnly);
}

/**
 * @brief Enable/disable middle buffer read-only
 */
void CMergeDoc::OnFileReadOnlyMiddle()
{
	if (m_nBuffers == 3)
	{
		bool bReadOnly = m_ptBuf[1]->GetReadOnly();
		m_ptBuf[1]->SetReadOnly(!bReadOnly);
	}
}

/**
 * @brief Called when "Middle read-only" item is updated
 */
void CMergeDoc::OnUpdateFileReadOnlyMiddle(CCmdUI* pCmdUI)
{
	if (m_nBuffers < 3)
	{
		pCmdUI->Enable(false);
	}
	else
	{
		bool bReadOnly = m_ptBuf[1]->GetReadOnly();
		pCmdUI->Enable(true);
		pCmdUI->SetCheck(bReadOnly);
	}
}

/**
 * @brief Enable/disable right buffer read-only
 */
void CMergeDoc::OnFileReadOnlyRight()
{
	bool bReadOnly = m_ptBuf[m_nBuffers - 1]->GetReadOnly();
	m_ptBuf[m_nBuffers - 1]->SetReadOnly(!bReadOnly);
}

/**
 * @brief Called when "Left read-only" item is updated
 */
void CMergeDoc::OnUpdateFileReadOnlyRight(CCmdUI* pCmdUI)
{
	bool bReadOnly = m_ptBuf[m_nBuffers - 1]->GetReadOnly();
	pCmdUI->Enable(true);
	pCmdUI->SetCheck(bReadOnly);
}

/**
 * @brief Update readonly statusbaritem
 */
void CMergeDoc::OnUpdateStatusRO(CCmdUI* pCmdUI)
{
	bool bRO = m_ptBuf[pCmdUI->m_nID - ID_STATUS_PANE0FILE_RO]->GetReadOnly();
	pCmdUI->Enable(bRO);
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
	const String s = CMergeFrameCommon::GetDiffStatusString(GetCurrentDiff(), m_diffList.GetSignificantDiffs());
	pCmdUI->SetText(s.c_str());
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
	case ID_VIEW_DIFFCONTEXT_INVERT:
		m_bInvertDiffContext = !m_bInvertDiffContext;
		break;
	}
	GetOptionsMgr()->SaveOption(OPT_DIFF_CONTEXT, m_nDiffContext);
	GetOptionsMgr()->SaveOption(OPT_INVERT_DIFF_CONTEXT, m_bInvertDiffContext);
	FlushAndRescan(true);
	ForEachView([](auto& pView) { if (pView->m_bDetailView) pView->EnsureVisible(pView->GetCursorPos()); });
}

/**
 * @brief Swap the positions of the two panes
 */
template<int srcPane, int dstPane>
void CMergeDoc::OnViewSwapPanes()
{
	SwapFiles(srcPane, dstPane);
}

/**
 * @brief Swap context enable for 3 file compares 
 */
void CMergeDoc::OnUpdateSwapContext(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nBuffers > 2);
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
	case ID_VIEW_DIFFCONTEXT_INVERT:
		bCheck = m_bInvertDiffContext; break;
	default:
		bCheck = (m_nDiffContext < 0); break;
	}
	pCmdUI->SetCheck(bCheck);
	pCmdUI->Enable(!(pCmdUI->m_nID == ID_VIEW_DIFFCONTEXT_INVERT && (m_nDiffContext < 0)));
}

/**
 * @brief Refresh display using text-buffers
 * @note This DOES NOT reload files!
 */
void CMergeDoc::OnRefresh()
{
	FlushAndRescan(true);
}

/**
 * @brief Build the diff array and prepare buffers accordingly (insert ghost lines, set WinMerge flags)
 *
 * @note After PrimeTextBuffers(), all buffers should have the same length.
 */
void CMergeDoc::PrimeTextBuffers()
{
	SetCurrentDiff(-1);
	m_nTrivialDiffs = 0;
	int nDiff;
	int nDiffCount = m_diffList.GetSize();
	int file;

	// walk the diff list and calculate numbers of extra lines to add
	int extras[3] = {0, 0, 0};   // extra lines added to each view
	m_diffList.GetExtraLinesCounts(m_nBuffers, extras);

	// resize m_aLines once for each view
	UINT lcount[3] = {0, 0, 0};
	UINT lcountnew[3] = {0, 0, 0};
	UINT lcountmax = 0;
	
	for (file = 0; file < m_nBuffers; file++)
	{
		lcount[file] = m_ptBuf[file]->GetLineCount();
		lcountnew[file] = lcount[file] + extras[file];
		lcountmax = (std::max)(lcountmax, lcountnew[file]);
	}
	for (file = 0; file < m_nBuffers; file++)
	{
		m_ptBuf[file]->m_aLines.resize(lcountmax);
	}

	// walk the diff list backward, move existing lines to proper place,
	// add ghost lines, and set flags
	for (nDiff = nDiffCount - 1; nDiff >= 0; nDiff --)
	{
		DIFFRANGE curDiff;
		VERIFY(m_diffList.GetDiff(nDiff, curDiff));

		// move matched lines after curDiff
		int nline[3] = { 0, 0, 0 };
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
			nmaxline = (std::max)(nmaxline, nline[file]);
		}

		for (file = 0; file < m_nBuffers; file++)
		{
			lineflags_t dflag = LF_GHOST;
			if ((file == 0 && curDiff.op == OP_3RDONLY) || (file == 2 && curDiff.op == OP_1STONLY))
				dflag |= LF_SNP;
			m_ptBuf[file]->MoveLine(curDiff.begin[file], curDiff.end[file], lcountnew[file]-nmaxline);
			int nextra = nmaxline - nline[file];
			if (nextra > 0)
			{
				m_ptBuf[file]->SetEmptyLine(lcountnew[file] - nextra, nextra);
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
			[[fallthrough]];
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
							lineflags_t dflag = (curDiff.op == OP_TRIVIAL) ? LF_TRIVIAL : LF_DIFF;
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

#ifdef _DEBUG
	// Note: By this point all `m_ptBuf[]` buffers must have the same  
	//		number of line entries; eventual buffer processing typically
	//		uses the line count from `m_ptBuf[0]` for all buffer processing.

	for (file = 0; file < m_nBuffers; file++)
	{
		ASSERT(m_ptBuf[0]->GetLineCount() == m_ptBuf[file]->GetLineCount());
	}
#endif

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
CMergeDoc::FileChange CMergeDoc::IsFileChangedOnDisk(const tchar_t* szPath, DiffFileInfo &dfi,
	bool bSave, int nBuffer)
{
	DiffFileInfo *fileInfo = nullptr;
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
		return FileChange::Removed;

	int64_t timeDiff = dfi.mtime - fileInfo->mtime;
	if (timeDiff < 0) timeDiff = -timeDiff;
	if ((timeDiff > tolerance * Poco::Timestamp::resolution()) || (dfi.size != fileInfo->size))
	{
		bFileChanged = true;
	}

	if (bFileChanged)
		return FileChange::Changed;
	else
		return FileChange::NoChange;
}

void CMergeDoc::HideLines()
{
	int nLine;
	int file;

	if (m_nDiffContext < 0)
	{
		ForEachView([](auto& pView) { pView->SetEnableHideLines(false); });
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
		bool diff = !!(m_ptBuf[0]->GetLineFlags(nLine) & (LF_DIFF | LF_GHOST));
		if ((!m_bInvertDiffContext && !diff) || (m_bInvertDiffContext && diff))
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
				diff = !!(m_ptBuf[0]->GetLineFlags(nLine) & (LF_DIFF | LF_GHOST));
				if ((!m_bInvertDiffContext && !diff) || (m_bInvertDiffContext && diff))
					break;
				for (file = 0; file < m_nBuffers; file++)
					m_ptBuf[file]->SetLineFlag(nLine, LF_INVISIBLE, false, false, false);
			}

			int nLineEnd2 = (nLine + m_nDiffContext >= nLineCount) ? nLineCount-1 : (nLine + m_nDiffContext);
			for (; nLine < nLineEnd2; nLine++)
			{
				for (file = 0; file < m_nBuffers; file++)
					m_ptBuf[file]->SetLineFlag(nLine, LF_INVISIBLE, false, false, false);
				diff = !!(m_ptBuf[0]->GetLineFlags(nLine) & (LF_DIFF | LF_GHOST));
				if ((!m_bInvertDiffContext && diff) || (m_bInvertDiffContext && !diff))
					nLineEnd2 = (nLine + 1 + m_nDiffContext >= nLineCount) ? nLineCount-1 : (nLine + 1 + m_nDiffContext);
			}
		}
	}

	ForEachView([](auto& pView) { pView->SetEnableHideLines(true); });
}

void CMergeDoc::AddToLineFilters(const String& text)
{
	theApp.m_pLineFilters->AddFilter(strutils::to_regex(text), true);
	theApp.m_pLineFilters->SaveFilters();
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
		dlg.m_sLeftFile = m_strSaveAsPath.empty() ? m_filePaths.GetLeft() : m_strSaveAsPath;
	else
		dlg.m_sLeftFile = m_strSaveAsPath.empty() ? m_strDesc[0] : m_strSaveAsPath;
	if (m_nBuffers == 3)
	{
		if (!m_filePaths.GetMiddle().empty())
			dlg.m_sMiddleFile = m_strSaveAsPath.empty() ? m_filePaths.GetMiddle() : m_strSaveAsPath;
		else
			dlg.m_sMiddleFile = m_strSaveAsPath.empty() ? m_strDesc[1] : m_strSaveAsPath;
	}
	if (!m_filePaths.GetRight().empty())
		dlg.m_sRightFile = m_strSaveAsPath.empty() ?m_filePaths.GetRight() : m_strSaveAsPath;
	else
		dlg.m_sRightFile = m_strSaveAsPath.empty() ? m_strDesc[m_nBuffers - 1] : m_strSaveAsPath;

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
		if (m_pDirDoc != nullptr && m_pDirDoc->HasDiffs())
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
void CMergeDoc::AddMergeViews(CMergeEditSplitterView* pMergeEditSplitterView, CMergeEditView *pView[3])
{
	m_pMergeEditSplitterView[m_nGroups] = pMergeEditSplitterView;
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		m_pView[m_nGroups][nBuffer] = pView[nBuffer];
	++m_nGroups;
}

void CMergeDoc::RemoveMergeViews(CMergeEditSplitterView* pMergeEditSplitterView)
{
	ASSERT(m_nGroups > 0);
	int nGroup;
	for (nGroup = 0; nGroup < m_nGroups; ++nGroup)
	{
		if (m_pMergeEditSplitterView[nGroup] == pMergeEditSplitterView)
			break;
	}
	ASSERT(nGroup < m_nGroups);
	for (; nGroup < m_nGroups - 1; nGroup++)
	{
		for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			m_pView[nGroup][nBuffer] = m_pView[nGroup + 1][nBuffer];
			m_pView[nGroup][nBuffer]->m_nThisGroup = nGroup;
		}
		m_pMergeEditSplitterView[nGroup] = m_pMergeEditSplitterView[nGroup + 1];
	}
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		m_pView[m_nGroups - 1][nBuffer] = nullptr;
	m_pMergeEditSplitterView[m_nGroups - 1] = nullptr;
	--m_nGroups;
}

/**
 * @brief DirDoc gives us its identity just after it creates us
 */
void CMergeDoc::SetDirDoc(IDirDoc * pDirDoc)
{
	ASSERT(pDirDoc != nullptr && m_pDirDoc == nullptr);
	m_pDirDoc = pDirDoc;
}

/**
 * @brief Return pointer to parent frame
 */
CMergeEditFrame * CMergeDoc::GetParentFrame() 
{
	return dynamic_cast<CMergeEditFrame *>(m_pView[0][0] ? m_pView[0][0]->GetParentFrame() : nullptr); 
}

/**
 * @brief DirDoc is closing
 */
void CMergeDoc::DirDocClosing(IDirDoc * pDirDoc)
{
	ASSERT(m_pDirDoc == pDirDoc);
	m_pDirDoc = nullptr;
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

	GetParentFrame()->DestroyWindow();
	return true;
}

/**
 * @brief A string to display as a tooltip for MDITabbar
 */
CString CMergeDoc::GetTooltipString() const
{
	return CMergeFrameCommon::GetTooltipString(*this).c_str();
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
int CMergeDoc::LoadFile(const String& sFileName, int nBuffer, bool & readOnly, const FileTextEncoding & encoding)
{
	String sError;
	FileLoadResult::flags_t retVal = FileLoadResult::FRESULT_ERROR;

	CDiffTextBuffer *pBuf = m_ptBuf[nBuffer].get();
	m_filePaths[nBuffer] = sFileName;

	CRLFSTYLE nCrlfStyle = CRLFSTYLE::AUTOMATIC;
	String sOpenError;
	retVal = pBuf->LoadFromFile(sFileName.c_str(), m_infoUnpacker,
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
		if (!sOpenError.empty())
			sError = strutils::format_string2(_("Cannot open file\n%1\n\n%2"), sFileName, sOpenError);
		else
			sError = strutils::format_string1(_("File not found: %1"), sFileName);
		ShowMessageBox(sError, MB_OK | MB_ICONSTOP | MB_MODELESS);
	}
	else if (FileLoadResult::IsErrorUnpack(retVal))
	{
		sError = strutils::format_string1(_("File not unpacked: %1"), sFileName);
		ShowMessageBox(sError, MB_OK | MB_ICONSTOP | MB_MODELESS);
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
	if (cp == 0) // 0 is our signal value for invalid
		return false;
	return GetEncodingNameFromCodePage(cp) != nullptr;
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
FileLoadResult::flags_t CMergeDoc::LoadOneFile(int index, const String& filename, bool readOnly, const String& strDesc,
		const FileTextEncoding & encoding)
{
	FileLoadResult::flags_t loadSuccess = FileLoadResult::FRESULT_ERROR;;
	
	m_strDesc[index] = strDesc;
	if (!filename.empty() && !paths::IsNullDeviceName(filename))
	{
		if (strDesc.empty())
			m_nBufferType[index] = BUFFERTYPE::NORMAL;
		else
			m_nBufferType[index] = BUFFERTYPE::NORMAL_NAMED;
		m_pSaveFileInfo[index]->Update(filename);
		m_pRescanFileInfo[index]->Update(filename);

		loadSuccess = LoadFile(filename, index, readOnly, encoding);
		if (FileLoadResult::IsLossy(loadSuccess))
		{
			// Determine the file encoding by looking at all the contents of the file, not just part of it
			FileTextEncoding encodingNew = codepage_detect::Guess(filename, GetOptionsMgr()->GetInt(OPT_CP_DETECT), -1);
			if (encoding != encodingNew)
			{
				m_ptBuf[index]->FreeAll();
				loadSuccess = LoadFile(filename, index, readOnly, encodingNew);
			}
		}
	}
	else
	{
		if (m_strDesc[index].empty())
			m_strDesc[index] = (index == 0) ? _("Untitled Left") : ((m_nBuffers < 3 || index == 2) ? _("Untitled Right") : _("Untitled Middle"));
		m_nBufferType[index] = BUFFERTYPE::UNNAMED;
		m_ptBuf[index]->InitNew();
		m_ptBuf[index]->m_encoding = encoding;
		m_ptBuf[index]->FinishLoading(); // should clear GGhostTextBuffer::m_RealityBlock when reloading unnamed buffer 
		loadSuccess = FileLoadResult::FRESULT_OK;
	}
	return loadSuccess;
}

CMergeDoc::TableProps CMergeDoc::MakeTablePropertiesByFileName(const String& path, const std::optional<bool>& enableTableEditing, bool showDialog)
{
	const tchar_t quote = strutils::from_charstr(GetOptionsMgr()->GetString(OPT_CMP_TBL_QUOTE_CHAR));
	FileFilterHelper filterCSV, filterTSV, filterDSV;
	bool allowNewlineIQuotes = GetOptionsMgr()->GetBool(OPT_CMP_TBL_ALLOW_NEWLINES_IN_QUOTES);
	const String& csvFilePattern = GetOptionsMgr()->GetString(OPT_CMP_CSV_FILEPATTERNS);
	if (!csvFilePattern.empty())
	{
		filterCSV.SetMaskOrExpression(csvFilePattern);
		if (filterCSV.includeFile(path))
			return { true, strutils::from_charstr(GetOptionsMgr()->GetString(OPT_CMP_CSV_DELIM_CHAR)), quote, allowNewlineIQuotes };
	}
	const String& tsvFilePattern = GetOptionsMgr()->GetString(OPT_CMP_TSV_FILEPATTERNS);
	if (!tsvFilePattern.empty())
	{
		filterTSV.SetMaskOrExpression(tsvFilePattern);
		if (filterTSV.includeFile(path))
			return { true, '\t', quote, allowNewlineIQuotes };
	}
	const String& dsvFilePattern = GetOptionsMgr()->GetString(OPT_CMP_DSV_FILEPATTERNS);
	if (!dsvFilePattern.empty())
	{
		filterDSV.SetMaskOrExpression(dsvFilePattern);
		if (filterDSV.includeFile(path))
			return { true, strutils::from_charstr(GetOptionsMgr()->GetString(OPT_CMP_DSV_DELIM_CHAR)), quote };
	}
	if (enableTableEditing.value_or(false))
	{
		if (showDialog)
		{
			COpenTableDlg dlg;
			if (dlg.DoModal() == IDOK)
				return { true, strutils::from_charstr(dlg.m_sDelimiterChar), strutils::from_charstr(dlg.m_sQuoteChar), dlg.m_bAllowNewlinesInQuotes };
		}
		else
		{
			return { true, strutils::from_charstr(GetOptionsMgr()->GetString(OPT_CMP_DSV_DELIM_CHAR)), quote };
		}
	}
	return { false, 0, 0, false };
};

void CMergeDoc::SetTableProperties()
{
	TableProps tableProps[3] = { };
	int nTableFileIndex = -1;
	if (m_pTablePropsPrepared)
	{
		nTableFileIndex = 0;
		tableProps[0] = *m_pTablePropsPrepared;
	}
	else
	{
		for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			if (nBuffer == 0 ||
				paths::FindExtension(m_ptBuf[nBuffer - 1]->GetTempFileName()) != paths::FindExtension(m_ptBuf[nBuffer]->GetTempFileName()))
				tableProps[nBuffer] = MakeTablePropertiesByFileName(m_ptBuf[nBuffer]->GetTempFileName(), m_bEnableTableEditing);
			else
				tableProps[nBuffer] = tableProps[nBuffer - 1];
			if (tableProps[nBuffer].istable)
				nTableFileIndex = nBuffer;
		}
	}
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		if (m_bEnableTableEditing.value_or(true) && nTableFileIndex >= 0)
		{
			int i = tableProps[nBuffer].istable ? nBuffer : nTableFileIndex;
			m_ptBuf[nBuffer]->SetTableEditing(true);
			m_ptBuf[nBuffer]->ShareColumnWidths(*m_ptBuf[0]);
			m_ptBuf[nBuffer]->SetAllowNewlinesInQuotes(tableProps[i].allowNewlinesInQuotes);
			m_ptBuf[nBuffer]->SetFieldDelimiter(tableProps[i].delimiter);
			m_ptBuf[nBuffer]->SetFieldEnclosure(tableProps[i].quote);
			m_ptBuf[nBuffer]->JoinLinesForTableEditingMode();
		}
		else
		{
			m_ptBuf[nBuffer]->SetTableEditing(false);
		}
	}
}

void CMergeDoc::SetTextType(int textType)
{
	ForEachView([textType, this](auto& pView) {
		pView->SetTextType(CrystalLineParser::TextType(textType));
		pView->SetDisableBSAtSOL(false);
		m_bChangedSchemeManually = true;
	});
}

void CMergeDoc::SetTextType(const String& ext)
{
	String ext2 = ext;
	strutils::replace(ext2, _T("."), _T(""));
	ForEachView([&ext2, this](auto& pView) {
		pView->SetTextType(ext2.c_str());
		pView->SetDisableBSAtSOL(false);
		m_bChangedSchemeManually = true;
	});
}

/**
 * @brief Loads files and does initial rescan.
 * @param fileloc [in] File to open to left/middle/right side (path & encoding info)
 * @param bRO [in] Is left/middle/right file read-only
 * @return Success/Failure/Binary (failure) per typedef enum OpenDocsResult_TYPE
 * @todo Options are still read from CMainFrame, this will change
 * @sa CMainFrame::ShowTextMergeDoc()
 */
bool CMergeDoc::OpenDocs(int nFiles, const FileLocation ifileloc[],
		const bool bRO[], const String strDesc[])
{
	PrediffingInfo prediffer;
	m_diffWrapper.GetPrediffer(&prediffer);
	CMergeFrameCommon::LogComparisonStart(nFiles, ifileloc, strDesc, &m_infoUnpacker, &prediffer);

	CWaitCursor waitstatus;
	IDENTLEVEL identical = IDENTLEVEL::NONE;
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

	ForEachView([](auto& pView) { pView->DetachFromBuffer(); });

	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		// clear undo buffers
		m_ptBuf[nBuffer]->m_aUndoBuf.clear();

		// free the buffers
		m_ptBuf[nBuffer]->FreeAll ();

		// build the text being filtered, "|" separates files as it is forbidden in filenames
		m_strBothFilenames += fileloc[nBuffer].filepath + _T("|");
	}
	m_strBothFilenames.erase(m_strBothFilenames.length() - 1);

	// Load files
	FileLoadResult::flags_t nSuccess[3] = { FileLoadResult::FRESULT_ERROR,  FileLoadResult::FRESULT_ERROR,  FileLoadResult::FRESULT_ERROR };
	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		nSuccess[nBuffer] = LoadOneFile(nBuffer, fileloc[nBuffer].filepath, bRO[nBuffer], strDesc ? strDesc[nBuffer] : _T(""),
			fileloc[nBuffer].encoding);
		if (!FileLoadResult::IsOk(nSuccess[nBuffer]))
		{
			CMergeEditFrame* pFrame = GetParentFrame();
			if (pFrame != nullptr)
			{
				// Use verify macro to trap possible error in debug.
				VERIFY(pFrame->DestroyWindow());
			}
			return false;
		}
	}

	SetTableProperties();

	const bool bFiltersEnabled = GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED);

	// scratchpad : we don't call LoadFile, so
	// we need to initialize the unpacker as a "do nothing" one
	if (bFiltersEnabled)
	{ 
		if (std::count(m_nBufferType, m_nBufferType + m_nBuffers, BUFFERTYPE::UNNAMED) == m_nBuffers)
		{
			m_infoUnpacker.Initialize(false);
		}
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
		if (m_pEncodingErrorBar == nullptr)
		{
			m_pEncodingErrorBar.reset(new CEncodingErrorBar());
			m_pEncodingErrorBar->Create(GetParentFrame());
		}
		m_pEncodingErrorBar->SetText(I18n::LoadString(idres));
		GetParentFrame()->ShowControlBar(m_pEncodingErrorBar.get(), TRUE, FALSE);
	}

	ForEachView([](auto& pView) {
		// Now buffers data are valid
		pView->AttachToBuffer();
		// Currently there is only one set of syntax colors, which all documents & views share
		pView->SetColorContext(theApp.GetMainSyntaxColors());
		// Currently there is only one set of markers, which all documents & views share
		pView->SetMarkersContext(theApp.GetMainMarkers());
	});
	for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
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
			String s = I18n::LoadString(IDS_SUGGEST_IGNOREEOL);
			if (ShowMessageBox(s, MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN, IDS_SUGGEST_IGNOREEOL) == IDYES)
			{
				diffOptions.bIgnoreEol = true;
				m_diffWrapper.SetOptions(&diffOptions);

				CMessageBoxDialog dlg(nullptr, s.c_str(), _T(""), 0, IDS_SUGGEST_IGNOREEOL);
				const int nFormerResult = dlg.GetFormerResult();
				if (nFormerResult != -1)
				{
					// "Don't ask this question again" checkbox is checked
					GetOptionsMgr()->SaveOption(OPT_CMP_IGNORE_EOL, true);
				}
			}
		}
	}

	// Define the prediffer
	if (bFiltersEnabled)
	{
		m_diffWrapper.SetTextForAutomaticPrediff(m_strBothFilenames);
	}

	bool bBinary = false;
	nRescanResult = Rescan(bBinary, identical);

	// Open filed if rescan succeed and files are not binaries
	if (nRescanResult == RESCAN_OK)
	{
		if (GetOptionsMgr()->GetBool(OPT_SYNTAX_HIGHLIGHT))
		{
			// set the document types
			// Warning : it is the first thing to do (must be done before UpdateView,
			// or any function that calls UpdateView, like SelectDiff)
			// Note: If option enabled, and another side type is not recognized,
			// we use recognized type for unrecognized side too.
			String sext[3];
			bool bTyped[3]{};
			int paneTyped = -1;

			for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
			{
				sext[nBuffer] = GetFileExt(m_ptBuf[nBuffer]->GetTempFileName().c_str(), m_strDesc[nBuffer].c_str());
				ForEachView(nBuffer, [&](auto& pView) {
					bTyped[nBuffer] = pView->SetTextType(sext[nBuffer].c_str());
					if (bTyped[nBuffer])
						paneTyped = nBuffer;
				});
			}

			if (paneTyped == -1)
			{
				String sFirstLine;
				m_ptBuf[0]->GetLine(0, sFirstLine);
				ForEachView([&bTyped, &sFirstLine](auto& pView) {
					bTyped[pView->m_nThisPane] = pView->SetTextTypeByContent(sFirstLine.c_str());
				});
			}
			else
			{
				CrystalLineParser::TextDefinition *enuType = CrystalLineParser::GetTextType(sext[paneTyped].c_str());
				ForEachView([&bTyped, enuType](auto& pView) {
					if (!bTyped[pView->m_nThisPane])
						pView->SetTextType(enuType);
				});
			}
		}

		int nNormalBuffer = 0;
		for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			// set the frame window header
			UpdateHeaderPath(nBuffer);

			ForEachView(nBuffer, [](auto& pView) { pView->DocumentsLoaded(); });
			
			if ((m_nBufferType[nBuffer] == BUFFERTYPE::NORMAL) ||
			    (m_nBufferType[nBuffer] == BUFFERTYPE::NORMAL_NAMED))
			{
				nNormalBuffer++;
			}
			
		}

		CMergeFrameCommon::LogComparisonCompleted(*this);

		// Inform user that files are identical
		// Don't show message if new buffers created
		if (identical == IDENTLEVEL::ALL && nNormalBuffer > 0)
		{
			ShowRescanError(nRescanResult, identical);
		}

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
	if (m_pView[0][0] != nullptr)
		m_pView[0][0]->RepaintLocationPane();

	GetMainFrame()->WatchDocuments(this);

	return true;
}

void CMergeDoc::MoveOnLoad(int nPane, int nLineIndex, bool bRealLine, int nCharIndex)
{
	if (nPane < 0)
	{
		nPane = (m_nBufferType[0] != BUFFERTYPE::UNNAMED) ? GetOptionsMgr()->GetInt(OPT_ACTIVE_PANE) : 0;
		if (nPane < 0 || nPane >= m_nBuffers)
			nPane = 0;
	}
	if (nLineIndex == -1)
	{
		if (this->GetCurrentDiff() != -1)
		{
			DIFFRANGE di;
			m_diffList.GetDiff(this->GetCurrentDiff(), di);
			nLineIndex = bRealLine ? di.begin[nPane] : di.dbegin;
		}
		else
		{
			// scroll to first diff
			if (GetOptionsMgr()->GetBool(OPT_SCROLL_TO_FIRST) &&
				m_diffList.HasSignificantDiffs())
			{
				int nDiff = m_diffList.FirstSignificantDiff();
				if (nDiff != -1)
					m_pView[0][nPane]->SelectDiff(nDiff, true, false);
				m_pView[0][nPane]->SetActivePane();
				return;
			}
		}
	}
	m_pView[0][nPane]->GotoLine(nLineIndex < 0 ? 0 : nLineIndex, bRealLine, nPane, true, nCharIndex);
}

void CMergeDoc::ChangeFile(int nBuffer, const String& path, int nLineIndex)
{
	if (!PromptAndSaveIfNeeded(true))
		return;

	FileLocation fileloc[3];
	String strDesc[3];
	bool bRO[3]{};
	for (int pane = 0; pane < m_nBuffers; pane++)
	{
		bRO[pane] = m_ptBuf[pane]->GetReadOnly();
		fileloc[pane].encoding.m_unicoding = m_ptBuf[pane]->getUnicoding();
		fileloc[pane].encoding.m_codepage = m_ptBuf[pane]->getCodepage();
		fileloc[pane].setPath(m_filePaths[pane]);
	}
	std::copy_n(m_strDesc, m_nBuffers, strDesc);

	strDesc[nBuffer].clear();
	fileloc[nBuffer].setPath(path);
	fileloc[nBuffer].encoding = codepage_detect::Guess(path, GetOptionsMgr()->GetInt(OPT_CP_DETECT));

	bool filenameChanged = path != m_filePaths[nBuffer];
	auto columnWidths = m_ptBuf[nBuffer]->GetColumnWidths();
	int nActivePane = GetActiveMergeView()->m_nThisPane;
	
	if (OpenDocs(m_nBuffers, fileloc, bRO, strDesc))
	{
		// Restore column widths and active pane changed by OpenDocs to their previous state
		if (!filenameChanged)
		{
			m_ptBuf[nBuffer]->SetColumnWidths(columnWidths);
		}
		MoveOnLoad(nActivePane, nLineIndex);
	}
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
	
	m_bAutomaticRescan = GetOptionsMgr()->GetBool(OPT_AUTOMATIC_RESCAN);

	m_diffWrapper.SetDetectMovedBlocks(GetOptionsMgr()->GetBool(OPT_CMP_MOVED_BLOCKS));
	Options::DiffOptions::Load(GetOptionsMgr(), options);

	m_diffWrapper.SetOptions(&options);

	// Refresh view options
	ForEachView([](auto& pView) { pView->RefreshOptions(); });
}

/**
 * @brief Write path and filename to headerbar
 * @note SetText() does not repaint unchanged text
 */
void CMergeDoc::UpdateHeaderPath(int pane)
{
	CMergeEditFrame *pf = GetParentFrame();
	ASSERT(pf != nullptr);
	String sText;
	bool bChanges = false;

	if (m_nBufferType[pane] == BUFFERTYPE::UNNAMED ||
		m_nBufferType[pane] == BUFFERTYPE::NORMAL_NAMED)
	{
		sText = m_strDesc[pane];
	}
	else
	{
		sText = m_filePaths[pane];
		if (m_pDirDoc != nullptr)
		{
			m_pDirDoc->ApplyDisplayRoot(pane, sText);
		}
	}
	bChanges = m_ptBuf[pane]->IsModified();

	if (bChanges)
		sText.insert(0, _T("* "));

	if (m_sCurrentHeaderTitle[pane] == sText)
		return;

	m_sCurrentHeaderTitle[pane] = sText;

	pf->GetHeaderInterface()->SetText(pane, sText);

	SetTitle(nullptr);
}

/**
 * @brief Paint differently the headerbar of the active view
 */
void CMergeDoc::UpdateHeaderActivity(int pane, bool bActivate)
{
	CMergeEditFrame *pf = GetParentFrame();
	ASSERT(pf != nullptr);
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

bool CMergeDoc::IsEditedAfterRescan(int nBuffer) const
{
	if (nBuffer >= 0 && nBuffer < m_nBuffers)
		return m_bEditAfterRescan[nBuffer];

	for (nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
	{
		if (m_bEditAfterRescan[nBuffer])
			return true;
	}

	return false;
}

/**
 * @brief Update document filenames to title
 */
void CMergeDoc::SetTitle(LPCTSTR lpszTitle)
{
	String sTitle = (lpszTitle != nullptr) ? lpszTitle : CMergeFrameCommon::GetTitleString(*this);
	CDocument::SetTitle(sTitle.c_str());
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CMergeDoc::UpdateResources()
{
	if (m_nBufferType[0] == BUFFERTYPE::UNNAMED)
		m_strDesc[0] = _("Untitled Left");
	if (m_nBufferType[m_nBuffers - 1] == BUFFERTYPE::UNNAMED)
		m_strDesc[m_nBuffers - 1] = _("Untitled Right");
	if (m_nBuffers == 3 && m_nBufferType[1] == BUFFERTYPE::UNNAMED)
		m_strDesc[1] = _("Untitled Middle");
	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		UpdateHeaderPath(nBuffer);

	GetParentFrame()->UpdateResources();
	ForEachView([](auto& pView) { pView->UpdateResources(); });
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
void CMergeDoc::SwapFiles(int nFromIndex, int nToIndex)
{
	if ((nFromIndex >= 0 && nFromIndex < m_nBuffers) && (nToIndex >= 0 && nToIndex < m_nBuffers))
	{
		// Swap views
		for (int nGroup = 0; nGroup < m_nGroups; ++nGroup)
		{
			int nLeftViewId = m_pView[nGroup][nFromIndex]->GetDlgCtrlID();
			int nRightViewId = m_pView[nGroup][nToIndex]->GetDlgCtrlID();
			m_pView[nGroup][nFromIndex]->SetDlgCtrlID(nRightViewId);
			m_pView[nGroup][nToIndex]->SetDlgCtrlID(nLeftViewId);
		}


		// Swap buffers and so on
		std::swap(m_ptBuf[nFromIndex], m_ptBuf[nToIndex]);
		for (int nGroup = 0; nGroup < m_nGroups; ++nGroup)
			std::swap(m_pView[nGroup][nFromIndex], m_pView[nGroup][nToIndex]);
		std::swap(m_pSaveFileInfo[nFromIndex], m_pSaveFileInfo[nToIndex]);
		std::swap(m_pRescanFileInfo[nFromIndex], m_pRescanFileInfo[nToIndex]);
		std::swap(m_nBufferType[nFromIndex], m_nBufferType[nToIndex]);
		std::swap(m_bEditAfterRescan[nFromIndex], m_bEditAfterRescan[nToIndex]);
		std::swap(m_strDesc[nFromIndex], m_strDesc[nToIndex]);

		m_filePaths.Swap(nFromIndex, nToIndex);
		m_diffList.Swap(nFromIndex, nToIndex);
		for (int nGroup = 0; nGroup < m_nGroups; nGroup++)
			swap(m_pView[nGroup][nFromIndex]->m_piMergeEditStatus, m_pView[nGroup][nToIndex]->m_piMergeEditStatus);

		ClearWordDiffCache();

		for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			m_ptBuf[nBuffer]->m_nThisPane = nBuffer;
			for (int nGroup = 0; nGroup < m_nGroups; nGroup++)
				m_pView[nGroup][nBuffer]->m_nThisPane = nBuffer;

			// Update views
			UpdateHeaderPath(nBuffer);
		}
		GetParentFrame()->UpdateSplitter();
		ForEachView([](auto& pView) { pView->UpdateStatusbar(); });

		UpdateAllViews(nullptr);
	}
}

/**
 * @brief Reloads the opened files
 */
void CMergeDoc::OnFileReload()
{
	if (!PromptAndSaveIfNeeded(true))
		return;
	
	FileLocation fileloc[3];
	bool bRO[3]{};
	for (int pane = 0; pane < m_nBuffers; pane++)
	{
		DiffFileInfo fileInfo;
		bRO[pane] = m_ptBuf[pane]->GetReadOnly();
		if (IsFileChangedOnDisk(m_filePaths[pane].c_str(), fileInfo, false, pane)
			== FileChange::Changed)
		{
			fileloc[pane].encoding = codepage_detect::Guess(m_filePaths[pane], GetOptionsMgr()->GetInt(OPT_CP_DETECT));
		}
		else
		{
			fileloc[pane].encoding.m_unicoding = m_ptBuf[pane]->getUnicoding();
			fileloc[pane].encoding.m_codepage = m_ptBuf[pane]->getCodepage();
		}
		fileloc[pane].setPath(m_filePaths[pane]);
	}
	int nActivePane = GetActiveMergeView()->m_nThisPane;
	CEPoint pt = GetActiveMergeView()->GetCursorPos();
	auto columnWidths = m_ptBuf[0]->GetColumnWidths();
	if (OpenDocs(m_nBuffers, fileloc, bRO, m_strDesc))
	{
		// Restore column widths and active pane changed by OpenDocs to their previous state
		m_ptBuf[0]->SetColumnWidths(columnWidths);
		MoveOnLoad(nActivePane, pt.y);
	}
}

/**
 * @brief Display encodings to user
 */
void CMergeDoc::OnFileEncoding()
{
	DoFileEncodingDialog();
}

void CMergeDoc::OnOpenWithUnpacker()
{
	CSelectPluginDlg dlg(m_infoUnpacker.GetPluginPipeline(),
		strutils::join(m_filePaths.begin(), m_filePaths.end(), _T("|")),
		CSelectPluginDlg::PluginType::Unpacker, false);
	if (dlg.DoModal() != IDOK)
		return;

	if (!PromptAndSaveIfNeeded(true))
		return;

	PackingInfo infoUnpacker(dlg.GetPluginPipeline());
	PathContext paths = m_filePaths;
	fileopenflags_t dwFlags[3] = { FFILEOPEN_NOMRU, FFILEOPEN_NOMRU, FFILEOPEN_NOMRU };
	String strDesc[3] = { m_strDesc[0], m_strDesc[1], m_strDesc[2] };
	int nID = m_ptBuf[0]->GetTableEditing() ? ID_MERGE_COMPARE_TABLE : ID_MERGE_COMPARE_TEXT;
	nID = GetOptionsMgr()->GetBool(OPT_PLUGINS_OPEN_IN_SAME_FRAME_TYPE) ? nID : -nID;

	if (GetMainFrame()->DoFileOrFolderOpen(&paths, dwFlags, strDesc, _T(""),
		GetOptionsMgr()->GetBool(OPT_CMP_INCLUDE_SUBDIRS), nullptr, &infoUnpacker, nullptr, nID))
		GetParentFrame()->DestroyWindow();
}

void CMergeDoc::OnApplyPrediffer() 
{
	PrediffingInfo prediffer;
	GetPrediffer(&prediffer);
	// let the user choose a handler
	CSelectPluginDlg dlg(prediffer.GetPluginPipeline(),
		strutils::join(m_filePaths.begin(), m_filePaths.end(), _T("|")),
		CSelectPluginDlg::PluginType::Prediffer, false);
	if (dlg.DoModal() != IDOK)
		return;
	prediffer.SetPluginPipeline(dlg.GetPluginPipeline());
	SetPrediffer(&prediffer);
	m_CurrentPredifferID = -1;
	FlushAndRescan(true);
}

/**
 * @brief Create the dynamic submenu for prediffers
 *
 * @note The plugins are grouped in (suggested) and (not suggested)
 *       The IDs follow the order of GetAvailableScripts
 *       For example :
 *				suggested 0         ID_1ST + 0 
 *				suggested 1         ID_1ST + 2 
 *				suggested 2         ID_1ST + 5 
 *				not suggested 0     ID_1ST + 1 
 *				not suggested 1     ID_1ST + 3 
 *				not suggested 2     ID_1ST + 4 
 */
HMENU CMergeDoc::createPrediffersSubmenu(HMENU hMenu)
{
	// empty the menu
	int j = GetMenuItemCount(hMenu);
	while (j --)
		DeleteMenu(hMenu, 0, MF_BYPOSITION);

	// title
	AppendMenu(hMenu, MF_STRING, ID_NO_PREDIFFER, _("No Prediffer (Normal)").c_str());
	
	if (!GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED))
		return hMenu;

	m_CurrentPredifferID = -1;

	// compute the m_CurrentPredifferID (to set the radio button)
	PrediffingInfo prediffer;
	GetPrediffer(&prediffer);
	if (prediffer.GetPluginPipeline().empty())
		m_CurrentPredifferID = ID_NO_PREDIFFER;

	// get the scriptlet files
	const auto& [ suggestedPlugins, allPlugins ]= FileTransform::CreatePluginMenuInfos(
		m_strBothFilenames, FileTransform::PredifferEventNames, ID_PREDIFFERS_FIRST);

	// build the menu : first part, Suggested Plugins
	// title
	AppendMenu(hMenu, MF_SEPARATOR, 0, nullptr);
	AppendMenu(hMenu, MF_STRING, ID_SUGGESTED_PLUGINS, _("Suggested Plugins").c_str());

	for (const auto& [caption, name, id, plugin ] : suggestedPlugins)
		AppendMenu(hMenu, MF_STRING, id, caption.c_str());

	// build the menu : second part, others plugins
	// title
	AppendMenu(hMenu, MF_SEPARATOR, 0, nullptr);
	AppendMenu(hMenu, MF_STRING, ID_NOT_SUGGESTED_PLUGINS, _("All Plugins").c_str());

	String lastPluginName;
	String errorMessage;
	auto result = prediffer.ParsePluginPipeline(errorMessage);
	if (result.size() > 0)
		lastPluginName = result.back().name;
	
	for (const auto& [processType, pluginAry] : allPlugins)
	{
		for (const auto& [caption, name, id, plugin] : pluginAry)
		{
			if (!name.empty())
			{
				AppendMenu(hMenu, MF_STRING, id, caption.c_str());
				if (lastPluginName == plugin->m_name)
					m_CurrentPredifferID = id;
			}
		}
	}

	return hMenu;
}

/**
 * @brief Called when an editor script item is updated
 */
void CMergeDoc::OnUpdatePrediffer(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);

	PrediffingInfo prediffer;
	GetPrediffer(&prediffer);

	if (prediffer.GetPluginPipeline().find(_T("<Automatic>")) != String::npos)
	{
		pCmdUI->SetRadio(false);
		return;
	}

	// Detect when CDiffWrapper::RunFileDiff has canceled a buggy prediffer
	if (prediffer.GetPluginPipeline().empty())
		m_CurrentPredifferID = ID_NO_PREDIFFER;

	pCmdUI->SetRadio(pCmdUI->m_nID == static_cast<UINT>(m_CurrentPredifferID));
}

/**
 * @brief Handler for all prediffer choices, including ID_PREDIFF_MANUAL, ID_PREDIFF_AUTO, ID_NO_PREDIFFER, & specific prediffers
 */
void CMergeDoc::OnPrediffer(UINT nID )
{
	SetPredifferByMenu(nID);
	FlushAndRescan(true);
}

/**
 * @brief Handler for all prediffer choices.
 * Prediffer choises include ID_PREDIFF_MANUAL, ID_PREDIFF_AUTO,
 * ID_NO_PREDIFFER, & specific prediffers.
 */
void CMergeDoc::SetPredifferByMenu(UINT nID)
{
	// update data for the radio button
	m_CurrentPredifferID = nID;

	if (nID == ID_NO_PREDIFFER)
	{
		// All flags are set correctly during the construction
		PrediffingInfo infoPrediffer(false);
		SetPrediffer(&infoPrediffer);
		return;
	}

	String pluginName = CMainFrame::GetPluginPipelineByMenuId(nID, FileTransform::PredifferEventNames, ID_PREDIFFERS_FIRST);

	// build a PrediffingInfo structure fom the ID
	PrediffingInfo prediffer(pluginName);
	
	// update the prediffer and rescan
	SetPrediffer(&prediffer);
}

void CMergeDoc::OnScriptsForCopying(UINT nID)
{
	m_CurrentEditorScriptID = nID;
	m_editorScriptInfo.SetPluginPipeline(
		CMainFrame::GetPluginPipelineByMenuId(nID, FileTransform::EditorScriptEventNames, ID_SCRIPT_FOR_COPYING_FIRST));
}

void CMergeDoc::OnUpdateScriptsForCopying(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
	pCmdUI->SetRadio(pCmdUI->m_nID == static_cast<UINT>(m_CurrentEditorScriptID));
}

void CMergeDoc::OnSelectEditorScriptForCopying() 
{
	// let the user choose a handler
	CSelectPluginDlg dlg(m_editorScriptInfo.GetPluginPipeline(),
		strutils::join(m_filePaths.begin(), m_filePaths.end(), _T("|")),
		CSelectPluginDlg::PluginType::EditorScript, false);
	if (dlg.DoModal() != IDOK)
		return;
	m_editorScriptInfo.SetPluginPipeline(dlg.GetPluginPipeline());
	m_CurrentEditorScriptID = 0;
}

void CMergeDoc::OnBnClickedFileEncoding()
{
	if (m_pEncodingErrorBar == nullptr || GetParentFrame() == nullptr)
		return;
	GetParentFrame()->ShowControlBar(m_pEncodingErrorBar.get(), FALSE, FALSE);
	DoFileEncodingDialog();
}

void CMergeDoc::OnBnClickedPlugin()
{
	if (m_pEncodingErrorBar == nullptr || GetParentFrame() == nullptr)
		return;
	GetParentFrame()->ShowControlBar(m_pEncodingErrorBar.get(), FALSE, FALSE);
	OnOpenWithUnpacker();
}

void CMergeDoc::OnBnClickedHexView()
{
	OnFileRecompareAs(ID_MERGE_COMPARE_HEX);
}

void CMergeDoc::OnOK()
{
	if (m_pEncodingErrorBar == nullptr || GetParentFrame() == nullptr)
		return;
	GetParentFrame()->ShowControlBar(m_pEncodingErrorBar.get(), FALSE, FALSE);
}

void CMergeDoc::OnFileRecompareAsText()
{
	m_bEnableTableEditing = false;
	OnFileReload();
}

void CMergeDoc::OnUpdateFileRecompareAsText(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ptBuf[0]->GetTableEditing());
}

void CMergeDoc::OnFileRecompareAsTable()
{
	if (m_ptBuf[0]->GetTableEditing())
	{
		COpenTableDlg dlg;
		if (dlg.DoModal() != IDOK)
			return;
		m_pTablePropsPrepared.reset(new TableProps{ true, strutils::from_charstr(dlg.m_sDelimiterChar), strutils::from_charstr(dlg.m_sQuoteChar), dlg.m_bAllowNewlinesInQuotes });
	}
	m_bEnableTableEditing = true;
	OnFileReload();
}

void CMergeDoc::OnUpdateFileRecompareAsTable(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(true);
}

void CMergeDoc::OnFileRecompareAs(UINT nID)
{
	if (!PromptAndSaveIfNeeded(true))
		return;
	
	fileopenflags_t dwFlags[3] = { 0 };
	PathContext paths = m_filePaths;
	String strDesc[3];
	PackingInfo infoUnpacker(m_infoUnpacker.GetPluginPipeline());

	for (int pane = 0; pane < m_nBuffers; pane++)
	{
		dwFlags[pane] |= FFILEOPEN_NOMRU | (m_ptBuf[pane]->GetReadOnly() ? FFILEOPEN_READONLY : 0);
		strDesc[pane] = m_strDesc[pane];
	}
	if (ID_UNPACKERS_FIRST <= nID && nID <= ID_UNPACKERS_LAST)
	{
		infoUnpacker.SetPluginPipeline(CMainFrame::GetPluginPipelineByMenuId(nID, FileTransform::UnpackerEventNames, ID_UNPACKERS_FIRST));
		nID = m_ptBuf[0]->GetTableEditing() ? ID_MERGE_COMPARE_TABLE : ID_MERGE_COMPARE_TEXT;
		nID = GetOptionsMgr()->GetBool(OPT_PLUGINS_OPEN_IN_SAME_FRAME_TYPE) ? nID : -static_cast<int>(nID);
	}

	if (GetMainFrame()->DoFileOrFolderOpen(&m_filePaths, dwFlags, strDesc, _T(""),
	    GetOptionsMgr()->GetBool(OPT_CMP_INCLUDE_SUBDIRS), nullptr, &infoUnpacker, nullptr, nID))
		GetParentFrame()->DestroyWindow();
}

// Return file extension either from file name 
String CMergeDoc::GetFileExt(const tchar_t* sFileName, const tchar_t* sDescription) const
{
	String sExt;
	paths::SplitFilename(sFileName, nullptr, nullptr, &sExt);
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
	dc.CreateDC(_T("DISPLAY"), nullptr, nullptr, nullptr);
	m_pView[0][0]->GetFont(lf);
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
	// Get paths
	// If archive, use archive path + folder + filename inside archive
	// If desc text given, use it
	PathContext paths = m_filePaths;
	if (m_pDirDoc != nullptr && m_pDirDoc->IsArchiveFolders())
	{
		for (int i = 0; i < paths.GetSize(); i++)
		{
			if (!m_strDesc[i].empty())
				paths[i] = m_strDesc[i];
			else
				m_pDirDoc->ApplyDisplayRoot(i, paths[i]);
		}
	}
	else
	{
		for (int i = 0; i < paths.GetSize(); i++)
		{
			if (!m_strDesc[i].empty())
				paths[i] = m_strDesc[i];
		}
	}

	if (m_ptBuf[0]->GetTableEditing())
	{
		String headerText =
			_T("<!DOCTYPE html>\n")
			_T("<html>\n")
			_T("<head>\n")
			_T("<meta charset=\"UTF-8\">\n")
			_T("<title>WinMerge File Compare Report</title>\n")
			_T("<style>\n")
			_T("<!--\n")
			_T("table { table-layout: fixed; margin: 0; border: 1px solid #a0a0a0; box-shadow: 1px 1px 2px rgba(0, 0, 0, 0.15); font-size: %dpt;  }\n")
			_T("tr { vertical-align: top; line-height: 1.2 }\n")
			_T("tr:first-child { position: sticky; top: 0; z-index: 99; }\n")
			_T("td,th { word-break: break-all; padding: 0 3px; border: 1px solid #a0a0a0; }\n")
			_T(".ln { position: sticky; left: 0; }\n")
			_T(".title { font-weight: bold; color: white; background-color: blue; vertical-align: top; text-align: center; padding: 4px 4px; background: linear-gradient(mediumblue, darkblue);}\n")
			_T("%s")
			_T("-->\n")
			_T("</style>\n")
			_T("<script>\n")
			_T("<!--\n");
		if (m_nBuffers < 3)
			headerText +=
				_T("window.addEventListener('load', (event) => {\n")
				_T("  const div1 = document.getElementById(\"div1\");\n")
				_T("  const div2 = document.getElementById(\"div2\");\n")
				_T("  div1.addEventListener(\"scroll\", function() { div2.scrollTop = div1.scrollTop; div2.scrollLeft = div1.scrollLeft; });\n")
				_T("  div2.addEventListener(\"scroll\", function() { div1.scrollTop = div2.scrollTop; div1.scrollLeft = div2.scrollLeft; });\n")
				_T("});\n");
		else
			headerText +=
				_T("window.addEventListener('load', (event) => {\n")
				_T("  const div1 = document.getElementById(\"div1\");\n")
				_T("  const div2 = document.getElementById(\"div2\");\n")
				_T("  const div3 = document.getElementById(\"div3\");\n")
				_T("  div1.addEventListener(\"scroll\", function() { div2.scrollTop = div3.scrollTop = div1.scrollTop; div2.scrollLeft = div3.scrollLeft = div1.scrollLeft; });\n")
				_T("  div2.addEventListener(\"scroll\", function() { div1.scrollTop = div3.scrollTop = div2.scrollTop; div1.scrollLeft = div3.scrollLeft = div2.scrollLeft; });\n")
				_T("  div3.addEventListener(\"scroll\", function() { div1.scrollTop = div2.scrollTop = div3.scrollTop; div1.scrollLeft = div2.scrollLeft = div3.scrollLeft; });\n")
				_T("});\n");
		headerText +=
			_T("-->\n")
			_T("</script>\n")
			_T("</head>\n");
		String header = 
			strutils::format(headerText, nFontSize, (const tchar_t*)m_pView[0][0]->GetHTMLStyles());
		file.WriteString(header);

		file.WriteString(
			strutils::format(_T("<body>\n")
			_T("<div style=\"display: grid; grid-template-columns: %s; grid-template-rows: max-content; height: calc(100vh - 16px);\">\n"), 
				m_nBuffers < 3 ? _T("50% 50%") : _T("33.33% 33.33% 33.33%")));

		// titles
		int nBuffer;
		for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			String data = _T("<div class=\"title\">");
			file.WriteString(data);
			file.WriteString(ucr::toTString(CMarkdown::Entities(ucr::toUTF8(paths[nBuffer]))));
			file.WriteString(_T("</div>\n"));
		}

		// write the body of the report
		for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			int nDiff = 0;
			int nLineCount = m_ptBuf[nBuffer]->GetLineCount();
			int nColumnCountMax = m_ptBuf[nBuffer]->GetColumnCountMax();
			file.WriteString(
				strutils::format(
				_T("<div id=\"div%d\" style=\"overflow-x: auto;\">\n")
				_T("<table style=\"width: max-content; border-collapse: collapse;\">\n"), nBuffer + 1));
			file.WriteString(_T("<tr>"));
			String columnHeader = _T("<th class=\"cn\"></th>");
			for (int nColumn = 0; nColumn < nColumnCountMax; nColumn++)
				columnHeader += _T("<th class=\"cn\">") + m_pView[0][nBuffer]->GetColumnName(nColumn) + _T("</th>");
			file.WriteString(columnHeader.c_str());
			file.WriteString(_T("</tr>"));
			for (int nLineIndex = 0; nLineIndex < nLineCount; nLineIndex++)
			{
				if (!m_pView[0][nBuffer]->GetLineVisible(nLineIndex))
						continue;

				const int nSubLineCount = m_pView[0][nBuffer]->GetSubLines(nLineIndex);
				file.WriteString(strutils::format(_T("<tr style=\"height: %.1fem\">"), (nSubLineCount * 1.2 + 0.1)));

				// line number
				int iVisibleLineNumber = 0;
				String tdtag = _T("<td class=\"ln\">");
				lineflags_t dwFlags = m_ptBuf[nBuffer]->GetLineFlags(nLineIndex);
				if ((dwFlags & LF_GHOST) == 0 && m_pView[0][nBuffer]->GetViewLineNumbers())
				{
					iVisibleLineNumber = m_ptBuf[nBuffer]->ComputeRealLine(nLineIndex) + 1;
				}
				if (nBuffer == 0 &&
					(dwFlags & (LF_DIFF | LF_GHOST)) != 0 && (nLineIndex == 0 ||
						(m_ptBuf[nBuffer]->GetLineFlags(nLineIndex - 1) & (LF_DIFF | LF_GHOST)) == 0))
				{
					++nDiff;
					if (iVisibleLineNumber > 0)
					{
						tdtag += strutils::format(_T("<a id=\"d%d\" href=\"#d%d\">%d</a>"), nDiff, nDiff, iVisibleLineNumber);
						iVisibleLineNumber = 0;
					}
					else
						tdtag += strutils::format(_T("<a id=\"d%d\" href=\"#d%d\">.</a>"), nDiff, nDiff);
				}
				if (iVisibleLineNumber > 0)
					tdtag += strutils::format(_T("%d</td>"), iVisibleLineNumber);
				else
					tdtag += _T("</td>");
				file.WriteString(tdtag);
				// line content
				file.WriteString((const tchar_t*)m_pView[0][nBuffer]->GetHTMLLine(nLineIndex, _T("td"), nColumnCountMax));

				file.WriteString(_T("\n"));
				file.WriteString(_T("</tr>\n"));

				bool bBorderLine = nLineIndex + 1 < nLineCount && !m_pView[0][nBuffer]->GetLineVisible(nLineIndex + 1);
				if (bBorderLine)
				{
					file.WriteString(_T("<tr style=\"height: 1px\">"));
					file.WriteString(
						strutils::format(_T("<td colspan=\"%d\" style=\"background-color: black\"></td>")
							, nColumnCountMax + 1));
					file.WriteString(_T("</tr>\n"));
				}
			}
			file.WriteString(_T("</table></div>\n"));
		}
		file.WriteString(
			_T("</div>\n</body>\n"));
	}
	else
	{
		String headerText =
			_T("<!DOCTYPE html>\n")
			_T("<html>\n")
			_T("<head>\n")
			_T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n")
			_T("<title>WinMerge File Compare Report</title>\n")
			_T("<style>\n")
			_T("<!--\n")
			_T("table { table-layout: fixed; margin: 0; border: 1px solid #a0a0a0; box-shadow: 1px 1px 2px rgba(0, 0, 0, 0.15); }\n")
			_T("th { position: sticky; top: 0; }\n")
			_T("td,th { word-break: break-all; font-size: %dpt; padding: 0 3px; }\n")
			_T("tr { vertical-align: top; }\n")
			_T(".title { font-weight: bold; color: white; background-color: blue; vertical-align: top; text-align: center; padding: 4px 4px; background: linear-gradient(mediumblue, darkblue);}\n")
			_T("%s")
			_T("-->\n")
			_T("</style>\n")
			_T("</head>\n");
		String header = 
			strutils::format(headerText, nFontSize, (const tchar_t*)m_pView[0][0]->GetHTMLStyles());
		file.WriteString(header);

		file.WriteString(
			_T("<body>\n")
			_T("<table style=\"width: 100%; border-collapse: collapse;\">\n")
			_T("<colgroup>\n"));
		double marginWidth = m_pView[0][0]->GetViewLineNumbers() ?
			strutils::to_str(m_pView[0][0]->GetLineCount()).length() / 1.5 + 0.5 : 0.5;
		for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			String data = strutils::format(
				_T("<col style=\"width: %.1fem;\" />\n")
				_T("<col style=\"width: calc(100%% / %d - %.1fem);\" />\n"),
				marginWidth, m_nBuffers, marginWidth);
			file.WriteString(data);
		}
		file.WriteString(_T("</colgroup>\n"));

		file.WriteString(
			_T("<thead>\n")
			_T("<tr>\n"));
	
		// titles
		int nBuffer;
		for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			String data = _T("<th colspan=\"2\" class=\"title\">");
			file.WriteString(data);
			file.WriteString(ucr::toTString(CMarkdown::Entities(ucr::toUTF8(paths[nBuffer]))));
			file.WriteString(_T("</th>\n"));
		}
		file.WriteString(
			_T("</tr>\n")
			_T("</thead>\n")
			_T("<tbody>\n"));

		// write the body of the report
		int idx[3]{};
		int nLineCount[3] = {};
		int nDiff = 0;
		int nColumnCountMax[3]{};
		for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			nLineCount[nBuffer] = m_ptBuf[nBuffer]->GetLineCount();
			nColumnCountMax[nBuffer] = m_ptBuf[nBuffer]->GetColumnCountMax();
		}
		for (;;)
		{
			file.WriteString(_T("<tr>\n"));
			for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
			{
				for (; idx[nBuffer] < nLineCount[nBuffer]; idx[nBuffer]++)
				{
					if (m_pView[0][nBuffer]->GetLineVisible(idx[nBuffer]))
						break;
				}

				if (idx[nBuffer] < nLineCount[nBuffer])
				{
					// line number
					int iVisibleLineNumber = 0;
					String tdtag = _T("<td class=\"ln\">");
					lineflags_t dwFlags = m_ptBuf[nBuffer]->GetLineFlags(idx[nBuffer]);
					if ((dwFlags & LF_GHOST) == 0 && m_pView[0][nBuffer]->GetViewLineNumbers())
					{
						iVisibleLineNumber = m_ptBuf[nBuffer]->ComputeRealLine(idx[nBuffer]) + 1;
					}
					if (nBuffer == 0 &&
						(dwFlags & (LF_DIFF | LF_GHOST)) != 0 && (idx[nBuffer] == 0 ||
							(m_ptBuf[nBuffer]->GetLineFlags(idx[nBuffer] - 1) & (LF_DIFF | LF_GHOST)) == 0))
					{
						++nDiff;
						if (iVisibleLineNumber > 0)
						{
							tdtag += strutils::format(_T("<a id=\"d%d\" href=\"#d%d\">%d</a>"), nDiff, nDiff, iVisibleLineNumber);
							iVisibleLineNumber = 0;
						}
						else
							tdtag += strutils::format(_T("<a id=\"d%d\" href=\"#d%d\">.</a>"), nDiff, nDiff);
					}
					if (iVisibleLineNumber > 0)
						tdtag += strutils::format(_T("%d</td>"), iVisibleLineNumber);
					else
						tdtag += _T("</td>");
					file.WriteString(tdtag);
					// line content
					file.WriteString((const tchar_t*)m_pView[0][nBuffer]->GetHTMLLine(idx[nBuffer], _T("td"), nColumnCountMax[nBuffer]));
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
				if (idx[nBuffer] < nLineCount[nBuffer] && !m_pView[0][nBuffer]->GetLineVisible(idx[nBuffer]))
					bBorderLine = true;
			}

			if (bBorderLine)
			{
				file.WriteString(_T("<tr style=\"height: 1px\">"));
				for (nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
				{
					if (idx[nBuffer] < nLineCount[nBuffer] && !m_pView[0][nBuffer]->GetLineVisible(idx[nBuffer]))
						file.WriteString(_T("<td colspan=\"2\" style=\"background-color: black\"></td>"));
					else
						file.WriteString(_T("<td colspan=\"2\"></td>"));
				}
				file.WriteString(_T("</tr>\n"));
			}

			if (idx[0] >= nLineCount[0] && idx[1] >= nLineCount[1] && (m_nBuffers < 3 || idx[2] >= nLineCount[2]))
				break;
		}
		file.WriteString(
			_T("</tbody>\n")
			_T("</table>\n")
			_T("</body>\n"));
	}
	file.WriteString(
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

	if (!SelectFile(AfxGetMainWnd()->GetSafeHwnd(), s, false, nullptr, _T(""), _("HTML Files (*.htm,*.html)|*.htm;*.html|All Files (*.*)|*.*||"), _T("htm")))
		return;

	if (GenerateReport(s))
		I18n::MessageBox(IDS_REPORT_SUCCESS, MB_OK | MB_ICONINFORMATION);
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
		I18n::MessageBox(IDS_SAVEFILES_FORPATCH, MB_ICONSTOP);
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
	int nLine[3]{};
	for (int nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
	{
		int tmp = m_pView[0][nBuffer]->GetCursorPos().y;
		nLine[nBuffer] = m_ptBuf[nBuffer]->ComputeApparentLine(m_ptBuf[nBuffer]->ComputeRealLine(tmp));
	}

	// If adding a sync point by selecting a ghost line that is after the last block, Cancel the process adding a sync point.
	for (int nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
		if (nLine[nBuffer] >= m_ptBuf[nBuffer]->GetLineCount())
		{
			I18n::MessageBox(IDS_SYNCPOINT_LASTBLOCK, MB_ICONSTOP);
			return;
		}

	for (int nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
		if (m_ptBuf[nBuffer]->GetLineFlags(nLine[nBuffer]) & LF_INVALID_BREAKPOINT)
			DeleteSyncPoint(nBuffer, nLine[nBuffer], false);

	for (int nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
		m_ptBuf[nBuffer]->SetLineFlag(nLine[nBuffer], LF_INVALID_BREAKPOINT, true, false);

	m_bHasSyncPoints = true;

	ForEachView([](auto& pView) { pView->SetSelectionMargin(true); });

	FlushAndRescan(true);
}

/**
 * @brief Delete a synchronization point
 */
bool CMergeDoc::DeleteSyncPoint(int pane, int nLine, bool bRescan)
{
	const auto syncpoints = GetSyncPointList();	
	for (const auto& syncpnt : syncpoints)
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

