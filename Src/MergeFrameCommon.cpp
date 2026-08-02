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
#include "DiffContext.h"
#include "DiffWrapper.h"
#include "DiffItem.h"
#include "CompareEngines/BinaryCompare.h"
#include "MessageBoxDialog.h"
#include "IAbortable.h"
#include "IAsyncTask.h"
#include "EditorFilePathBar.h"
#include "ShellContextMenu.h"
#include <../src/mfc/afximpl.h>

IMPLEMENT_DYNCREATE(CMergeFrameCommon, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CMergeFrameCommon, CMDIChildWnd)
	//{{AFX_MSG_MAP(CMergeFrameCommon)
	ON_WM_GETMINMAXINFO()
	ON_WM_DESTROY()
	ON_WM_MDIACTIVATE()
	ON_COMMAND(ID_EDITOR_EDIT_PATH, OnEditorEditPath)
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
		PathContext paths;
		for (int i = 0; i < m_paths.GetSize(); ++i)
		{
			paths.SetPath(i, paths::GetParentPath(m_paths[i]));
			di.diffFileInfo[i].path = _T("");
			di.diffFileInfo[i].filename = paths::FindFileName(m_paths[i]);
			if (di.diffFileInfo[i].Update(m_paths[i]))
				di.diffcode.setSideFlag(i);
		}
		if (m_paths.GetSize() == 3)
			di.diffcode.diffcode |= DIFFCODE::THREEWAY;
		CDiffContext ctxt(paths, CMP_BINARY_CONTENT);
		ctxt.SetAbortable(this);
		CompareEngines::BinaryCompare binaryCompare(ctxt);
		binaryCompare.CompareFiles(di);
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

void CMergeFrameCommon::ShowShellMenu(CWnd* pWnd, const String& path)
{
	CFrameWnd *pFrame = pWnd->GetTopLevelFrame();
	ASSERT(pFrame != nullptr);
	BOOL bAutoMenuEnableOld = pFrame->m_bAutoMenuEnable;
	pFrame->m_bAutoMenuEnable = FALSE;

	auto pContextMenu = std::make_unique<CShellContextMenu>(CShellContextMenu(0x9000, 0x9FFF));
	pContextMenu->Initialize();
	pContextMenu->AddItem(path);
	pContextMenu->RequeryShellContextMenu();
	CPoint point;
	::GetCursorPos(&point);
	HWND hWnd = pWnd->GetSafeHwnd();
	BOOL nCmd = TrackPopupMenu(pContextMenu->GetHMENU(), TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x, point.y, 0, hWnd, nullptr);
	if (nCmd)
		pContextMenu->InvokeCommand(nCmd, hWnd);
	pContextMenu->ReleaseShellContextMenu();

	pFrame->m_bAutoMenuEnable = bAutoMenuEnableOld;
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

void CMergeFrameCommon::OnEditorEditPath()
{
	if (GetHeaderInterface())
		GetHeaderInterface()->EditActivePanePath();
}
