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

	CMDIChildWnd::ActivateFrame(nCmdShow);
}

void CMergeFrameCommon::SaveWindowState()
{
	// If we are active, save the restored/maximized state
	// If we are not, do nothing and let the active frame do the job.
 	if (GetParentFrame()->GetActiveFrame() == this)
	{
		WINDOWPLACEMENT wp = {};
		wp.length = sizeof(WINDOWPLACEMENT);
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

void CMergeFrameCommon::ShowIdenticalMessage(const PathContext& paths, bool bIdenticalAll, std::function<int(LPCTSTR, unsigned, unsigned)> fnMessageBox)
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
	PackingInfo *pInfoUnpacker, PrediffingInfo *pInfoPrediffer)
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
	String plugin;
	if (pInfoUnpacker && !pInfoUnpacker->GetPluginPipeline().empty())
		plugin += _T("U");
	if (pInfoPrediffer && !pInfoPrediffer->GetPluginPipeline().empty())
		plugin += _T("P");
	return sTitle + (plugin.empty() ? _T("") : (_T(" (") + plugin + _T(")")));
}

void CMergeFrameCommon::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CMDIChildWnd::OnGetMinMaxInfo(lpMMI);
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
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	if (bActivate)
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WMU_CHILDFRAMEACTIVATED, 0, reinterpret_cast<LPARAM>(this));
}
