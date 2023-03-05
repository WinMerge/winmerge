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
#include <../src/mfc/afximpl.h>

IMPLEMENT_DYNCREATE(CMergeFrameCommon, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CMergeFrameCommon, CMDIChildWnd)
	//{{AFX_MSG_MAP(CMergeFrameCommon)
	ON_WM_GETMINMAXINFO()
	ON_WM_DESTROY()
	ON_WM_MDIACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMergeFrameCommon::CMergeFrameCommon(int nIdenticalIcon, int nDifferentIcon)
	: m_hIdentical(nIdenticalIcon < 0 ? nullptr : AfxGetApp()->LoadIcon(nIdenticalIcon))
	, m_hDifferent(nDifferentIcon < 0 ? nullptr : AfxGetApp()->LoadIcon(nDifferentIcon))
	, m_hCurrent((HICON)-1)
	, m_bActivated(false)
	, m_nLastSplitPos{0}
{
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WMU_CHILDFRAMEADDED, 0, reinterpret_cast<LPARAM>(this));
}

CMergeFrameCommon::~CMergeFrameCommon()
{
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WMU_CHILDFRAMEREMOVED, 0, reinterpret_cast<LPARAM>(this));
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

void CMergeFrameCommon::RemoveBarBorder()
{
	afxData.cxBorder2 = 0;
	afxData.cyBorder2 = 0;
	for (int i = 0; i < 4; ++i)
	{
		CControlBar* pBar = GetControlBar(i + AFX_IDW_DOCKBAR_TOP);
		pBar->SetBarStyle(pBar->GetBarStyle() & ~(CBRS_BORDER_ANY | CBRS_BORDER_3D));
	}
}

/**
 * @brief Reflect comparison result in window's icon.
 * @param nResult [in] Last comparison result which the application returns.
 */
void CMergeFrameCommon::SetLastCompareResult(int nResult)
{
	HICON hReplace = (nResult == 0) ? m_hIdentical : m_hDifferent;

	if (m_hCurrent != hReplace)
	{
		SetIcon(hReplace, TRUE);

		AfxGetMainWnd()->SetTimer(IDT_UPDATEMAINMENU, 500, nullptr);

		m_hCurrent = hReplace;
	}

	theApp.SetLastCompareResult(nResult);
}

void CMergeFrameCommon::ShowIdenticalMessage(const PathContext& paths, bool bIdenticalAll, std::function<int(const tchar_t*, unsigned, unsigned)> fnMessageBox)
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
			s = _("The same file is opened in both panels.");
			fnMessageBox(s.c_str(), nFlags, IDS_FILE_TO_ITSELF);
		}
		else if (bIdenticalAll)
		{
			s = _("The selected files are identical.");
			fnMessageBox(s.c_str(), nFlags, IDS_FILESSAME);
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

String CMergeFrameCommon::GetTitleString(const PathContext& paths, const String desc[],
	const PackingInfo *pInfoUnpacker, const PrediffingInfo *pInfoPrediffer, bool hasTrivialDiffs)
{
	const int nBuffers = paths.GetSize();
	String sFileName[3];
	String sTitle;
	for (int nBuffer = 0; nBuffer < paths.GetSize(); nBuffer++)
		sFileName[nBuffer] = !desc[nBuffer].empty() ? desc[nBuffer] : paths::FindFileName(paths[nBuffer]);
	if (std::count(&sFileName[0], &sFileName[0] + nBuffers, sFileName[0]) == nBuffers)
		sTitle = sFileName[0] + strutils::format(_T(" x %d"), nBuffers);
	else
		sTitle = strutils::join(&sFileName[0], &sFileName[0] + nBuffers, _T(" - "));
	String flags;
	if (pInfoUnpacker && !pInfoUnpacker->GetPluginPipeline().empty())
		flags += _T("U");
	if (pInfoPrediffer && !pInfoPrediffer->GetPluginPipeline().empty())
		flags += _T("P");
	if (hasTrivialDiffs)
		flags += _T("F");
	return (flags.empty() ? _T("") : (_T("(") + flags + _T(") "))) + sTitle;
}

String CMergeFrameCommon::GetTooltipString(const PathContext& paths, const String desc[],
	const PackingInfo *pInfoUnpacker, const PrediffingInfo *pInfoPrediffer, bool hasTrivialDiffs)
{
	const int nBuffers = paths.GetSize();
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
#if _MFC_VER >= 0x0800
	lpMMI->ptMaxTrackSize.x = max(lpMMI->ptMaxTrackSize.x, lpMMI->ptMaxSize.x);
	lpMMI->ptMaxTrackSize.y = max(lpMMI->ptMaxTrackSize.y, lpMMI->ptMaxSize.y);
#endif
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

	if (bActivate)
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WMU_CHILDFRAMEACTIVATED, 0, reinterpret_cast<LPARAM>(this));
}
