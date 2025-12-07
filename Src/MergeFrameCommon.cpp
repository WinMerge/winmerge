/** 
 * @file  MergeFrameCommon.cpp
 *
 * @brief Implementation file for CMergeFrameCommon
 *
 */
#include "StdAfx.h"
#include "MergeFrameCommon.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "paths.h"
#include "Merge.h"
#include "FileTransform.h"
#include "FileLocation.h"
#include "Logger.h"
#include "CompareStats.h"
#include "IMergeDoc.h"
#include "cepoint.h"
#include "DiffItem.h"
#include "CompareEngines/BinaryCompare.h"
#include "MessageBoxDialog.h"
#include "IAbortable.h"
#include "IAsyncTask.h"
#include <../src/mfc/afximpl.h>

IMPLEMENT_DYNCREATE(CMergeFrameCommon, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CMergeFrameCommon, CMDIChildWnd)
	//{{AFX_MSG_MAP(CMergeFrameCommon)
	ON_WM_GETMINMAXINFO()
	ON_WM_DESTROY()
	ON_WM_MDIACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/**
 * @class AsyncCompareTask
 * @brief An asynchronous task to perform exact binary comparison.
 */
class AsyncCompareTask : public IAsyncTask, public IAbortable
{
public:
	AsyncCompareTask(const PathContext& paths) : m_paths(paths), m_pCancelFlag(nullptr)
	{
	}

	/**
	 * @brief Check whether the task should be aborted.
	 */
	bool ShouldAbort() const override
	{
		return *m_pCancelFlag;
	};

	/**
	 * @brief Run the binary comparison and get the result message.
	 */
	String RunAndGetMessage(std::atomic_bool& cancelFlag) override
	{
		m_pCancelFlag = &cancelFlag;
		DIFFITEM di;
		for (int i = 0; i < m_paths.GetSize(); ++i)
		{
			di.diffFileInfo[i].SetFile(m_paths[i]);
			if (di.diffFileInfo[i].Update(m_paths[i]))
				di.diffcode.setSideFlag(i);
		}
		if (m_paths.GetSize() == 3)
			di.diffcode.diffcode |= DIFFCODE::THREEWAY;
		CompareEngines::BinaryCompare binaryCompare;
		binaryCompare.SetAbortable(this);
		di.diffcode.diffcode |= binaryCompare.CompareFiles(m_paths, di);
		if (di.diffcode.isResultError())
			return _("Selected files are identical (with current settings).\r\nBut binary comparison failed.");
		return di.diffcode.isResultSame()
			? _("Selected files are identical (binary match).")
			: _("Selected files are identical (with current settings).\r\nBut differ at the binary level.");
	}

private:
	std::atomic_bool* m_pCancelFlag;
	PathContext m_paths;
};

CMergeFrameCommon::CMergeFrameCommon(int nIdenticalIcon, int nDifferentIcon)
	: m_hIdentical(nIdenticalIcon < 0 ? nullptr : AfxGetApp()->LoadIcon(nIdenticalIcon))
	, m_hDifferent(nDifferentIcon < 0 ? nullptr : AfxGetApp()->LoadIcon(nDifferentIcon))
	, m_hCurrent((HICON)-1)
	, m_bActivated(false)
	, m_nLastSplitPos{0}
{
}

CMergeFrameCommon::~CMergeFrameCommon()
{
}

void CMergeFrameCommon::ActivateFrame(int nCmdShow)
{
	if (!m_bActivated) 
	{
		m_bActivated = true;

		// get the active child frame, and a flag whether it is maximized
		BOOL bMaximized = FALSE;
		CMDIChildWnd * oldActiveFrame = GetMDIFrame()->MDIGetActive(&bMaximized);
		if (oldActiveFrame == nullptr)
			// for the first frame, get the restored/maximized state from the registry
			bMaximized = GetOptionsMgr()->GetBool(OPT_ACTIVE_FRAME_MAX);
		if (bMaximized)
			nCmdShow = SW_SHOWMAXIMIZED;
		else
			nCmdShow = SW_SHOWNORMAL;
	}

	__super::ActivateFrame(nCmdShow);
}

void CMergeFrameCommon::SaveWindowState()
{
	// If we are active, save the restored/maximized state
	// If we are not, do nothing and let the active frame do the job.
 	if (GetParentFrame()->GetActiveFrame() == this)
	{
		WINDOWPLACEMENT wp = { sizeof(WINDOWPLACEMENT) };
		GetWindowPlacement(&wp);
		GetOptionsMgr()->SaveOption(OPT_ACTIVE_FRAME_MAX, (wp.showCmd == SW_MAXIMIZE));
	}
}

/**
 * @brief Reflect comparison result in window's icon.
 * @param nResult [in] Last comparison result which the application returns.
 */
void CMergeFrameCommon::SetLastCompareResult(int nResult)
{
	HICON hReplace = (nResult == 0) ? m_hIdentical : ((nResult < 0) ? nullptr : m_hDifferent);

	if (m_hCurrent != hReplace)
	{
		SetIcon(hReplace, TRUE);

		AfxGetMainWnd()->SetTimer(IDT_UPDATEMAINMENU, 500, nullptr);

		m_hCurrent = hReplace;
	}

	theApp.SetLastCompareResult(nResult);
}

void CMergeFrameCommon::ShowIdenticalMessage(const PathContext& paths, bool bIdenticalAll, bool bExactCompareAsync)
{
	String s;
	if (theApp.m_bExitIfNoDiff != MergeCmdLineInfo::ExitQuiet)
	{
		UINT nFlags = MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN;

		if (theApp.m_bExitIfNoDiff == MergeCmdLineInfo::Exit)
		{
			// Show the "files are identical" for basic "exit no diff" flag
			// If user don't want to see the message one uses the quiet version
			// of the "exit no diff".
			nFlags &= ~MB_DONT_DISPLAY_AGAIN;
		}
		if ((paths.GetSize() == 2 && !paths.GetLeft().empty() && !paths.GetRight().empty() &&
			 strutils::compare_nocase(paths.GetLeft(), paths.GetRight()) == 0) ||
			(paths.GetSize() == 3 && !paths.GetLeft().empty() && !paths.GetMiddle().empty() && !paths.GetRight().empty() &&
			 (strutils::compare_nocase(paths.GetLeft(), paths.GetRight()) == 0 ||
			  strutils::compare_nocase(paths.GetMiddle(), paths.GetRight()) == 0 ||
			  strutils::compare_nocase(paths.GetLeft(), paths.GetMiddle()) == 0)))
		{
			// compare file to itself, a custom message so user may hide the message in this case only
			s = _("Same file is opened in both panes.");
			AfxMessageBox(s.c_str(), nFlags, IDS_FILE_TO_ITSELF);
		}
		else if (bIdenticalAll)
		{
			s = _("Selected files are identical.");
			if (bExactCompareAsync)
			{
				if (theApp.GetNonInteractive())
				{
					theApp.OutputConsole(s + _T(": Cancel"));
					return;
				}
				s = _("Selected files are identical (with current settings).\r\nChecking binary identity...");
				CMessageBoxDialog dlgMessageBox(nullptr, s.c_str(), _T(""), nFlags, IDS_FILESSAME);
				dlgMessageBox.SetAsyncTask(std::make_shared<AsyncCompareTask>(paths));
				dlgMessageBox.DoModal();
			}
			else
			{
				AfxMessageBox(s.c_str(), nFlags, IDS_FILESSAME);
			}
		}
	}

	if (bIdenticalAll)
	{
		// Exit application if files are identical.
		if (theApp.m_bExitIfNoDiff == MergeCmdLineInfo::Exit ||
			theApp.m_bExitIfNoDiff == MergeCmdLineInfo::ExitQuiet)
		{
			AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT);
		}
	}
}

void CMergeFrameCommon::LogComparisonStart(int nFiles, const FileLocation ifileloc[], const String descs[], const PackingInfo* infoUnpacker, const PrediffingInfo* infoPrediffer)
{
	String str[3];
	for (int i = 0; i < nFiles; ++i)
	{
		str[i] = ifileloc[i].filepath;
		if (descs && !descs[i].empty())
			str[i] += _T("(") + descs[i] + _T(")");
	}
	String s = (nFiles < 3 ?
		strutils::format_string2(_("Comparing %1 with %2"), str[0], str[1]) :
		strutils::format_string3(_("Comparing %1 with %2 and %3"), str[0], str[1], str[2])
		);
	RootLogger::Info(s + GetPluginInfoString(infoUnpacker, infoPrediffer));
}

void CMergeFrameCommon::LogComparisonStart(const PathContext& paths, const String descs[], const PackingInfo* infoUnpacker, const PrediffingInfo* infoPrediffer)
{
	String str[3];
	for (int i = 0; i < paths.GetSize(); ++i)
	{
		str[i] = paths[i];
		if (descs && !descs[i].empty())
			str[i] += _T("(") + descs[i] + _T(")");
	}
	String s = (paths.GetSize() < 3) ?
			strutils::format_string2(_("Comparing %1 with %2"), str[0], str[1]) : 
			strutils::format_string3(_("Comparing %1 with %2 and %3"), str[0], str[1], str[2]);
	RootLogger::Info(s + GetPluginInfoString(infoUnpacker, infoPrediffer));
}

String CMergeFrameCommon::GetDiffStatusString(int curDiffIndex, int diffCount)
{
	if (diffCount == 0)
		return _("Identical");
	if (diffCount < 0)
		return _("Different");

	if (curDiffIndex < 0)
		return diffCount == 1 ? _("1 Difference Found") :
			  strutils::format_string1(_("%1 Differences Found"), strutils::to_str(diffCount));

	tchar_t sCnt[32] {};
	tchar_t sIdx[32] {};
	String s = I18n::LoadString(IDS_DIFF_NUMBER_STATUS_FMT);
	const int signInd = curDiffIndex;
	_itot_s(signInd + 1, sIdx, 10);
	strutils::replace(s, _T("%1"), sIdx);
	_itot_s(diffCount, sCnt, 10);
	strutils::replace(s, _T("%2"), sCnt);
	return s;
}

static String GetTitleStringFlags(const IMergeDoc& mergeDoc)
{
	const PackingInfo* pInfoUnpacker = mergeDoc.GetUnpacker();
	const PrediffingInfo* pInfoPrediffer = mergeDoc.GetPrediffer();
	const bool hasTrivialDiffs = mergeDoc.GetTrivialCount();
	String flags;
	if (pInfoUnpacker && !pInfoUnpacker->GetPluginPipeline().empty())
		flags += _T("U");
	if (pInfoPrediffer && !pInfoPrediffer->GetPluginPipeline().empty())
		flags += _T("P");
	if (hasTrivialDiffs)
		flags += _T("F");
	return (flags.empty() ? _T("") : (_T("(") + flags + _T(") ")));
}

String CMergeFrameCommon::GetTitleString(const IMergeDoc& mergeDoc)
{
	PathContext paths;
	const int nBuffers = mergeDoc.GetFileCount();
	String sFileName[3];
	String sTitle;
	for (int nBuffer = 0; nBuffer < nBuffers; nBuffer++)
	{
		const String desc = mergeDoc.GetDescription(nBuffer);
		sFileName[nBuffer] = !desc.empty() ? desc : paths::FindFileName(mergeDoc.GetPath(nBuffer));
	}
	if (std::count(&sFileName[0], &sFileName[0] + nBuffers, sFileName[0]) == nBuffers)
		sTitle = sFileName[0] + strutils::format(_T(" x %d"), nBuffers);
	else
		sTitle = strutils::join(&sFileName[0], &sFileName[0] + nBuffers, _T(" - "));
	return GetTitleStringFlags(mergeDoc) + sTitle;
}

String CMergeFrameCommon::GetTooltipString(const IMergeDoc& mergeDoc)
{
	PathContext paths;
	String desc[3];
	const int nBuffers = mergeDoc.GetFileCount();
	for (int i = 0; i < nBuffers; ++i)
	{
		desc[i] = mergeDoc.GetDescription(i);
		paths.SetPath(i, mergeDoc.GetPath(i), false);
	}
	return GetTooltipString(paths, desc, mergeDoc.GetUnpacker(), mergeDoc.GetPrediffer(), mergeDoc.GetTrivialCount() > 0);
}

String CMergeFrameCommon::GetTooltipString(const PathContext& paths, const String desc[],
	const PackingInfo *pInfoUnpacker, const PrediffingInfo *pInfoPrediffer, bool hasTrivialDiffs)
{
	String sTitle;
	for (int nBuffer = 0; nBuffer < paths.GetSize(); nBuffer++)
	{
		sTitle += strutils::format(_T("%d: "), nBuffer + 1);
		if (!desc[nBuffer].empty())
		{
			sTitle += desc[nBuffer];
			if (!paths[nBuffer].empty()) 
				sTitle += _T(" (") + paths[nBuffer] + _T(")");
		}
		else
		{
			sTitle += paths[nBuffer];
		}
		sTitle += _T(" - ");
		if (nBuffer == 0)
			sTitle += _("Left");
		else if (nBuffer == 1 && paths.GetSize() > 2)
			sTitle += _("Middle");
		else
			sTitle += _("Right");
		sTitle += _T("\n");
	}
	if (pInfoUnpacker && !pInfoUnpacker->GetPluginPipeline().empty())
		sTitle += strutils::format(_T("%s: %s\n"), _("Unpacker"), pInfoUnpacker->GetPluginPipeline());
	if (pInfoPrediffer && !pInfoPrediffer->GetPluginPipeline().empty())
		sTitle += strutils::format(_T("%s: %s\n"), _("Prediffer"), pInfoPrediffer->GetPluginPipeline());
	if (hasTrivialDiffs)
		sTitle += _("Filter applied") + _T("\n");
	return sTitle;
}

void CMergeFrameCommon::LogComparisonCompleted(const IMergeDoc& mergeDoc)
{
	RootLogger::Info(_("Comparison completed") + _T(": ") + GetTitleStringFlags(mergeDoc) + GetDiffStatusString(-1, mergeDoc.GetDiffCount()));
}

void CMergeFrameCommon::LogComparisonCompleted(const CompareStats& stats)
{
	const int errorCount = stats.GetCount(CompareStats::RESULT_ERROR);
	if (errorCount > 0)
	{
		String s = errorCount == 1 ? _("1 Error Found") :
			  strutils::format_string1(_("%1 Errors Found"), strutils::to_str(errorCount));
		RootLogger::Warn(_("Comparison completed") + _T(": ") + s);
		return;
	}
	int diffCount = 0;
	for (auto type : {
		CompareStats::RESULT_DIFF, CompareStats::RESULT_BINDIFF,
		CompareStats::RESULT_LUNIQUE, CompareStats::RESULT_MUNIQUE, CompareStats::RESULT_RUNIQUE,
		CompareStats::RESULT_LMISSING, CompareStats::RESULT_MMISSING, CompareStats::RESULT_RMISSING
		})
		diffCount += stats.GetCount(type);
	RootLogger::Info(_("Comparison completed") + _T(": ") + GetDiffStatusString(-1, diffCount));
}

void CMergeFrameCommon::LogFileSaved(const String& path)
{
	RootLogger::Info(_("File saved") + _T(": ") + path);
}

void CMergeFrameCommon::LogCopyDiff(int srcPane, int dstPane, int nDiff)
{
	RootLogger::Info(strutils::format(_T("Copy diff: pane %d to %d (hunk %d)"),
		srcPane, dstPane, nDiff));
}

void CMergeFrameCommon::LogCopyLines(int srcPane, int dstPane, int firstLine, int lastLine)
{
	RootLogger::Info(strutils::format(_T("Copy lines: pane %d to %d (vline %d-%d)"),
		srcPane, dstPane, firstLine, lastLine));
}

void CMergeFrameCommon::LogCopyInlineDiffs(int srcPane, int dstPane, int nDiff, int firstWordDiff, int lastWordDiff)
{
	RootLogger::Info(strutils::format(_T("Copy inline diffs: pane %d to %d (hunk %d, wdiff %d-%d)"),
		srcPane, dstPane, nDiff, firstWordDiff, lastWordDiff));
}

void CMergeFrameCommon::LogCopyCharacters(int srcPane, int dstPane,  int nDiff, const CEPoint& ptStart, const CEPoint& ptEnd)
{
	RootLogger::Info(strutils::format(_T("Copy chars: pane %d to %d (hunk %d, vline %d,%d-%d,%d)"),
		srcPane, dstPane, nDiff, ptStart.y, ptStart.x, ptEnd.y, ptEnd.x));
}

void CMergeFrameCommon::LogUndo()
{
	RootLogger::Info(_("Undo"));
}

void CMergeFrameCommon::LogRedo()
{
	RootLogger::Info(_("Redo"));
}

String CMergeFrameCommon::GetPluginInfoString(const PackingInfo* infoUnpacker, const PrediffingInfo* infoPrediffer)
{
	String p;
	if (infoUnpacker && !infoUnpacker->GetPluginPipeline().empty())
		p = _("Unpacker") + _T(": ") + infoUnpacker->GetPluginPipeline();
	if (infoPrediffer && !infoPrediffer->GetPluginPipeline().empty())
	{
		if (!p.empty())
			p += _T(", ");
		p += _("Prediffer") + _T(": ") + infoPrediffer->GetPluginPipeline();
	}
	if (p.empty())
		return _T("");
	return _T(" (") + p + _T(")");
}

void CMergeFrameCommon::ChangeMergeMenuText(int srcPane, int dstPane, CCmdUI* pCmdUI)
{
	String text;
	switch (pCmdUI->m_nID)
	{
	case ID_L2R:
		text = (srcPane == 0 && dstPane == 1) ?
			_("Copy to &Middle\tAlt+Right") : _("Copy to &Right\tAlt+Right");
		break;
	case ID_R2L:
		text = (srcPane == 2 && dstPane == 1) ?
			_("Copy to &Middle\tAlt+Left") : _("Copy to L&eft\tAlt+Left");
		break;
	case ID_COPY_FROM_LEFT:
		text = (srcPane == 1 && dstPane == 2) ?
			_("Copy from Middle\tAlt+Shift+Right") : _("Copy from Left\tAlt+Shift+Right");
		break;
	case ID_COPY_FROM_RIGHT:
		text = (srcPane == 1 && dstPane == 0) ?
			_("Copy from Middle\tAlt+Shift+Left") : _("Copy from Right\tAlt+Shift+Left");
		break;
	case ID_L2RNEXT:
		text = (srcPane == 0 && dstPane == 1) ?
			_("Copy to Middle and Advance\tCtrl+Alt+Right") : _("C&opy to Right and Advance\tCtrl+Alt+Right");
		break;
	case ID_R2LNEXT:
		text = (srcPane == 2 && dstPane == 1) ?
			_("Copy to Middle and Advance\tCtrl+Alt+Left") : _("Copy &to Left and Advance\tCtrl+Alt+Left");
		break;
	case ID_ALL_RIGHT:
		text = (srcPane == 0 && dstPane == 1) ?
			_("Copy All to Middle") : _("Copy &All to Right");
		break;
	case ID_ALL_LEFT:
		text = (srcPane == 2 && dstPane == 1) ?
			_("Copy All to Middle") : _("Cop&y All to Left");
		break;
	}
	if (!text.empty())
		pCmdUI->SetText(text.c_str());
}

std::pair<int, int> CMergeFrameCommon::MenuIDtoXY(UINT nID, int nActivePane, int nBuffers)
{
	if (nActivePane < 0 || nActivePane >= nBuffers)
		return { -1, -1 };

	int srcPane = -1, dstPane = -1;
	switch (nID)
	{
	case ID_L2R:
	case ID_L2RNEXT:
	case ID_ALL_RIGHT:
	case ID_LINES_L2R:
		dstPane = (nActivePane < nBuffers - 1) ? nActivePane + 1 : nBuffers - 1;
		srcPane = dstPane - 1;
		break;
	case ID_R2L:
	case ID_R2LNEXT:
	case ID_ALL_LEFT:
	case ID_LINES_R2L:
		dstPane = (nActivePane > 0) ? nActivePane - 1 : 0;
		srcPane = dstPane + 1;
		break;
	case ID_COPY_FROM_LEFT:
	case ID_COPY_LINES_FROM_LEFT:
		if (nActivePane > 0)
		{
			dstPane = nActivePane;
			srcPane = nActivePane - 1;
		}
		break;
	case ID_COPY_FROM_RIGHT:
	case ID_COPY_LINES_FROM_RIGHT:
		if (nActivePane < nBuffers - 1)
		{
			dstPane = nActivePane;
			srcPane = nActivePane + 1;
		}
		break;
	case ID_COPY_TO_MIDDLE_L:
	case ID_COPY_LINES_TO_MIDDLE_L:
		if (nBuffers > 2)
		{
			srcPane = 0;
			dstPane = 1;
		}
		break;
	case ID_COPY_TO_RIGHT_L:
	case ID_COPY_LINES_TO_RIGHT_L:
		srcPane = 0; dstPane = nBuffers - 1; break;
	case ID_COPY_FROM_MIDDLE_L:
	case ID_COPY_LINES_FROM_MIDDLE_L:
		if (nBuffers > 2)
		{
			srcPane = 1;
			dstPane = 0;
		}
		break;
	case ID_COPY_FROM_RIGHT_L:
	case ID_COPY_LINES_FROM_RIGHT_L:
		srcPane = nBuffers - 1; dstPane = 0; break;
	case ID_COPY_TO_LEFT_M:
	case ID_COPY_LINES_TO_LEFT_M:
		if (nBuffers > 2)
		{
			srcPane = 1;
			dstPane = 0;
		}
		break;
	case ID_COPY_TO_RIGHT_M:
	case ID_COPY_LINES_TO_RIGHT_M:
		if (nBuffers > 2)
		{
			srcPane = 1;
			dstPane = nBuffers - 1;
		}
		break;
	case ID_COPY_FROM_LEFT_M:
	case ID_COPY_LINES_FROM_LEFT_M:
		if (nBuffers > 2)
		{
			srcPane = 0;
			dstPane = 1;
		}
		break;
	case ID_COPY_FROM_RIGHT_M:
	case ID_COPY_LINES_FROM_RIGHT_M:
		if (nBuffers > 2)
		{
			srcPane = nBuffers - 1;
			dstPane = 1;
		}
		break;
	case ID_COPY_TO_MIDDLE_R:
	case ID_COPY_LINES_TO_MIDDLE_R:
		if (nBuffers > 2)
		{
			srcPane = nBuffers - 1;
			dstPane = 1;
		}
		break;
	case ID_COPY_TO_LEFT_R:
	case ID_COPY_LINES_TO_LEFT_R:
		srcPane = nBuffers - 1; dstPane = 0; break;
	case ID_COPY_FROM_MIDDLE_R:
	case ID_COPY_LINES_FROM_MIDDLE_R:
		if (nBuffers > 2)
		{
			srcPane = 1;
			dstPane = nBuffers - 1;
		}
		break;
	case ID_COPY_FROM_LEFT_R:
	case ID_COPY_LINES_FROM_LEFT_R:
		srcPane = 0; dstPane = nBuffers - 1; break;
	default:
		return { -1, -1 };
	}
	return { srcPane, dstPane };
}

void CMergeFrameCommon::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	__super::OnGetMinMaxInfo(lpMMI);
	// [Fix for MFC 8.0 MDI Maximizing Child Window bug on Vista]
	// https://groups.google.com/forum/#!topic/microsoft.public.vc.mfc/iajCdW5DzTM
	lpMMI->ptMaxTrackSize.x = (std::max)(lpMMI->ptMaxTrackSize.x, lpMMI->ptMaxSize.x);
	lpMMI->ptMaxTrackSize.y = (std::max)(lpMMI->ptMaxTrackSize.y, lpMMI->ptMaxSize.y);
}

void CMergeFrameCommon::OnDestroy()
{
	// https://stackoverflow.com/questions/35553955/getting-rid-of-3d-look-of-mdi-frame-window
	CFrameWnd::OnDestroy();
}

void CMergeFrameCommon::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	// call the base class to let standard processing switch to
	// the top-level menu associated with this window
	__super::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
}
