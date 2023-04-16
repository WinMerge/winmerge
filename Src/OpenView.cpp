/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  OpenView.cpp
 *
 * @brief Implementation of the COpenView class
 */

#include "stdafx.h"
#include "OpenView.h"
#include <vector>
#include <sys/stat.h>
#include "UnicodeString.h"
#include "Merge.h"
#include "OpenDoc.h"
#include "ProjectFile.h"
#include "paths.h"
#include "SelectPluginDlg.h"
#include "OptionsDef.h"
#include "MainFrm.h"
#include "OptionsMgr.h"
#include "FileOrFolderSelect.h"
#include "7zCommon.h"
#include "Constants.h"
#include "Bitmap.h"
#include "DropHandler.h"
#include "FileFilterHelper.h"
#include "Plugins.h"
#include "MergeAppCOMClass.h"
#include "BCMenu.h"
#include "LanguageSelect.h"
#include "Win_VersionHelper.h"
#include "OptionsProject.h"
#include "Merge7zFormatMergePluginImpl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef BCN_DROPDOWN
#define BCN_DROPDOWN            (BCN_FIRST + 0x0002)
#endif

// Timer ID and timeout for delaying path validity check
const UINT IDT_CHECKFILES = 1;
const UINT IDT_RETRY = 2;
const UINT CHECKFILES_TIMEOUT = 1000; // milliseconds
const int RETRY_MAX = 3;

// COpenView

IMPLEMENT_DYNCREATE(COpenView, CFormView)

BEGIN_MESSAGE_MAP(COpenView, CFormView)
	//{{AFX_MSG_MAP(COpenView)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_PATH0_BUTTON, IDC_PATH2_BUTTON, OnPathButton)
	ON_BN_CLICKED(IDC_SWAP01_BUTTON, (OnSwapButton<IDC_PATH0_COMBO, IDC_PATH1_COMBO>))
	ON_BN_CLICKED(IDC_SWAP12_BUTTON, (OnSwapButton<IDC_PATH1_COMBO, IDC_PATH2_COMBO>))
	ON_BN_CLICKED(IDC_SWAP02_BUTTON, (OnSwapButton<IDC_PATH0_COMBO, IDC_PATH2_COMBO>))
	ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_PATH0_COMBO, IDC_PATH2_COMBO, OnSelchangePathCombo)
	ON_CONTROL_RANGE(CBN_EDITCHANGE, IDC_PATH0_COMBO, IDC_PATH2_COMBO, OnEditEvent)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_SELECT_UNPACKER, IDC_SELECT_PREDIFFER, OnSelectPlugin)
	ON_CBN_SELENDCANCEL(IDC_PATH0_COMBO, UpdateButtonStates)
	ON_CBN_SELENDCANCEL(IDC_PATH1_COMBO, UpdateButtonStates)
	ON_CBN_SELENDCANCEL(IDC_PATH2_COMBO, UpdateButtonStates)
	ON_NOTIFY_RANGE(CBEN_BEGINEDIT, IDC_PATH0_COMBO, IDC_PATH2_COMBO, OnSetfocusPathCombo)
	ON_NOTIFY_RANGE(CBEN_DRAGBEGIN, IDC_PATH0_COMBO, IDC_PATH2_COMBO, OnDragBeginPathCombo)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SELECT_FILTER, OnSelectFilter)
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	ON_NOTIFY(BCN_DROPDOWN, IDC_OPTIONS, (OnDropDown<IDC_OPTIONS, IDR_POPUP_PROJECT_DIFF_OPTIONS>))
	ON_COMMAND_RANGE(ID_PROJECT_DIFF_OPTIONS_WHITESPACE_COMPARE, ID_PROJECT_DIFF_OPTIONS_WHITESPACE_IGNOREALL, OnDiffWhitespace)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PROJECT_DIFF_OPTIONS_WHITESPACE_COMPARE, ID_PROJECT_DIFF_OPTIONS_WHITESPACE_IGNOREALL, OnUpdateDiffWhitespace)
	ON_COMMAND(ID_PROJECT_DIFF_OPTIONS_IGNORE_BLANKLINES, OnDiffIgnoreBlankLines)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_DIFF_OPTIONS_IGNORE_BLANKLINES, OnUpdateDiffIgnoreBlankLines)
	ON_COMMAND(ID_PROJECT_DIFF_OPTIONS_IGNORE_CASE, OnDiffIgnoreCase)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_DIFF_OPTIONS_IGNORE_CASE, OnUpdateDiffIgnoreCase)
	ON_COMMAND(ID_PROJECT_DIFF_OPTIONS_IGNORE_EOL, OnDiffIgnoreEOL)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_DIFF_OPTIONS_IGNORE_EOL, OnUpdateDiffIgnoreEOL)
	ON_COMMAND(ID_PROJECT_DIFF_OPTIONS_IGNORE_NUMBERS, OnDiffIgnoreNumbers)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_DIFF_OPTIONS_IGNORE_NUMBERS, OnUpdateDiffIgnoreNumbers)
	ON_COMMAND(ID_PROJECT_DIFF_OPTIONS_IGNORE_CODEPAGE, OnDiffIgnoreCP)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_DIFF_OPTIONS_IGNORE_CODEPAGE, OnUpdateDiffIgnoreCP)
	ON_COMMAND(ID_PROJECT_DIFF_OPTIONS_IGNORE_COMMENTS, OnDiffIgnoreComments)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_DIFF_OPTIONS_IGNORE_COMMENTS, OnUpdateDiffIgnoreComments)
	ON_COMMAND_RANGE(ID_PROJECT_DIFF_OPTIONS_COMPMETHOD_FULL_CONTENTS, ID_PROJECT_DIFF_OPTIONS_COMPMETHOD_SIZE, OnCompareMethod)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PROJECT_DIFF_OPTIONS_COMPMETHOD_FULL_CONTENTS, ID_PROJECT_DIFF_OPTIONS_COMPMETHOD_SIZE, OnUpdateCompareMethod)
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_LOAD_PROJECT, OnLoadProject)
	ON_COMMAND(ID_SAVE_PROJECT, OnSaveProject)
	ON_COMMAND(ID_FILE_SAVE, OnSaveProject)
	ON_NOTIFY(BCN_DROPDOWN, ID_SAVE_PROJECT, (OnDropDown<ID_SAVE_PROJECT, IDR_POPUP_PROJECT>))
	ON_COMMAND(IDOK, OnOK)
	ON_NOTIFY(BCN_DROPDOWN, IDOK, (OnDropDown<IDOK, IDR_POPUP_COMPARE>))
	ON_COMMAND(IDCANCEL, OnCancel)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_EDIT_COPY, OnEditAction<WM_COPY>)
	ON_COMMAND(ID_EDIT_PASTE, OnEditAction<WM_PASTE>)
	ON_COMMAND(ID_EDIT_CUT, OnEditAction<WM_CUT>)
	ON_COMMAND(ID_EDIT_UNDO, OnEditAction<WM_UNDO>)
	ON_COMMAND(ID_EDIT_SELECT_ALL, (OnEditAction<EM_SETSEL, 0, -1>))
	ON_COMMAND_RANGE(ID_MERGE_COMPARE_TEXT, ID_MERGE_COMPARE_WEBPAGE, OnCompare)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MERGE_COMPARE_TEXT, ID_MERGE_COMPARE_WEBPAGE, OnUpdateCompare)
	ON_COMMAND_RANGE(ID_UNPACKERS_FIRST, ID_UNPACKERS_LAST, OnCompare)
	ON_COMMAND_RANGE(ID_OPEN_WITH_UNPACKER, ID_OPEN_WITH_UNPACKER, OnCompare)
	ON_MESSAGE(WM_USER + 1, OnUpdateStatus)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_NCHITTEST()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// COpenView construction/destruction

COpenView::COpenView()
	: CFormView(COpenView::IDD)
	, m_pUpdateButtonStatusThread(nullptr)
	, m_bRecurse(false)
	, m_pDropHandler(nullptr)
	, m_dwFlags()
	, m_bAutoCompleteReady()
	, m_bReadOnly {false, false, false}
	, m_hIconRotate(theApp.LoadIcon(IDI_ROTATE2))
	, m_hCursorNo(LoadCursor(nullptr, IDC_NO))
	, m_retryCount(0)
	, m_nIgnoreWhite(0)
	, m_bIgnoreBlankLines(false)
	, m_bIgnoreCase(false)
	, m_bIgnoreEol(false)
	, m_bIgnoreNumbers(false)
	, m_bIgnoreCodepage(false)
	, m_bFilterCommentsLines(false)
	, m_nCompareMethod(0)
{
	// CWnd::EnableScrollBarCtrl() called inside CScrollView::UpdateBars() is quite slow.
	// Therefore, set m_bInsideUpdate = TRUE so that CScrollView::UpdateBars() does almost nothing.
	m_bInsideUpdate = TRUE;
}

COpenView::~COpenView()
{
	TerminateThreadIfRunning();
}

void COpenView::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenView)
	DDX_Control(pDX, IDC_EXT_COMBO, m_ctlExt);
	DDX_Control(pDX, IDC_PATH0_COMBO, m_ctlPath[0]);
	DDX_Control(pDX, IDC_PATH1_COMBO, m_ctlPath[1]);
	DDX_Control(pDX, IDC_PATH2_COMBO, m_ctlPath[2]);
	DDX_Control(pDX, IDC_UNPACKER_COMBO, m_ctlUnpackerPipeline);
	DDX_Control(pDX, IDC_PREDIFFER_COMBO, m_ctlPredifferPipeline);
	DDX_CBStringExact(pDX, IDC_PATH0_COMBO, m_strPath[0]);
	DDX_CBStringExact(pDX, IDC_PATH1_COMBO, m_strPath[1]);
	DDX_CBStringExact(pDX, IDC_PATH2_COMBO, m_strPath[2]);
	DDX_Check(pDX, IDC_PATH0_READONLY, m_bReadOnly[0]);
	DDX_Check(pDX, IDC_PATH1_READONLY, m_bReadOnly[1]);
	DDX_Check(pDX, IDC_PATH2_READONLY, m_bReadOnly[2]);
	DDX_Check(pDX, IDC_RECURS_CHECK, m_bRecurse);
	DDX_CBStringExact(pDX, IDC_EXT_COMBO, m_strExt);
	DDX_CBStringExact(pDX, IDC_UNPACKER_COMBO, m_strUnpackerPipeline);
	DDX_CBStringExact(pDX, IDC_PREDIFFER_COMBO, m_strPredifferPipeline);
	//}}AFX_DATA_MAP
}

BOOL COpenView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style &= ~WS_BORDER;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	return __super::PreCreateWindow(cs);
}

void COpenView::OnInitialUpdate()
{
	if (!IsVista_OrGreater())
	{
		// fallback for XP 
		SendDlgItemMessage(IDC_OPTIONS, BM_SETSTYLE, BS_PUSHBUTTON, TRUE);
		SendDlgItemMessage(ID_SAVE_PROJECT, BM_SETSTYLE, BS_PUSHBUTTON, TRUE);
		SendDlgItemMessage(IDOK, BM_SETSTYLE, BS_PUSHBUTTON, TRUE);
	}

	m_sizeOrig = GetTotalSize();

	theApp.TranslateDialog(m_hWnd);

	if (!LoadImageFromResource(m_image, MAKEINTRESOURCE(IDR_LOGO), _T("IMAGE")))
	{
		// FIXME: LoadImageFromResource() seems to fail when running on Wine 5.0.
		m_image.Create(1, 1, 24, 0);
	}

	__super::OnInitialUpdate();

	// set caption to "swap paths" button
	LOGFONT lf;
	GetDlgItem(IDC_SWAP01_BUTTON)->GetFont()->GetObject(sizeof(lf), &lf);
	lf.lfCharSet = SYMBOL_CHARSET;
	lstrcpy(lf.lfFaceName, _T("Wingdings"));
	m_fontSwapButton.CreateFontIndirect(&lf);
	const int ids[] = {IDC_SWAP01_BUTTON, IDC_SWAP12_BUTTON, IDC_SWAP02_BUTTON};
	for (int i = 0; i < sizeof(ids)/sizeof(ids[0]); ++i)
	{
		GetDlgItem(ids[i])->SetFont(&m_fontSwapButton);
		SetDlgItemText(ids[i], _T("\xf4"));
	}

	m_constraint.InitializeCurrentSize(this);
	m_constraint.InitializeSpecificSize(this, m_sizeOrig.cx, m_sizeOrig.cy);
	m_constraint.SetMaxSizePixels(-1, m_sizeOrig.cy);
	m_constraint.SetScrollScale(this, 1.0, 1.0);
	m_constraint.SetSizeGrip(prdlg::CMoveConstraint::SG_NONE);
	m_constraint.DisallowHeightGrowth();
	//m_constraint.SubclassWnd(); // install subclassing

	m_constraint.LoadPosition(_T("ResizeableDialogs"), _T("OpenView"), false); // persist size via registry
	m_constraint.UpdateSizes();

	COpenDoc *pDoc = GetDocument();

	CString strTitle;
	GetWindowText(strTitle);
	pDoc->SetTitle(strTitle);

	m_files = pDoc->m_files;
	m_bRecurse = pDoc->m_bRecurse;
	m_strExt = pDoc->m_strExt;
	m_strUnpackerPipeline = pDoc->m_strUnpackerPipeline;
	m_strPredifferPipeline = pDoc->m_strPredifferPipeline;
	m_dwFlags[0] = pDoc->m_dwFlags[0];
	m_dwFlags[1] = pDoc->m_dwFlags[1];
	m_dwFlags[2] = pDoc->m_dwFlags[2];

	m_ctlPath[0].SetFileControlStates();
	m_ctlPath[1].SetFileControlStates(true);
	m_ctlPath[2].SetFileControlStates(true);
	m_ctlUnpackerPipeline.SetFileControlStates(true);
	m_ctlPredifferPipeline.SetFileControlStates(true);

	for (int file = 0; file < m_files.GetSize(); file++)
	{
		m_strPath[file] = m_files[file];
		m_ctlPath[file].SetWindowText(m_files[file].c_str());
		m_bReadOnly[file] = (m_dwFlags[file] & FFILEOPEN_READONLY) != 0;
	}

	m_ctlPath[0].AttachSystemImageList();
	m_ctlPath[1].AttachSystemImageList();
	m_ctlPath[2].AttachSystemImageList();
	LoadComboboxStates();

	m_ctlUnpackerPipeline.SetWindowText(m_strUnpackerPipeline.c_str());
	m_ctlPredifferPipeline.SetWindowText(m_strPredifferPipeline.c_str());

	bool bDoUpdateData = true;
	for (auto& strPath: m_strPath)
	{
		if (!strPath.empty())
			bDoUpdateData = false;
	}
	UpdateData(bDoUpdateData);

	auto* pGlobalFileFilter = theApp.GetGlobalFileFilter();
	String filterNameOrMask = pGlobalFileFilter->GetFilterNameOrMask();
	bool bMask = pGlobalFileFilter->IsUsingMask();

	if (!bMask)
	{
		String filterPrefix = _("[F] ");
		filterNameOrMask = filterPrefix + filterNameOrMask;
	}

	int ind = m_ctlExt.FindStringExact(0, filterNameOrMask.c_str());
	if (ind != CB_ERR)
		m_ctlExt.SetCurSel(ind);
	else
	{
		ind = m_ctlExt.InsertString(0, filterNameOrMask.c_str());
		if (ind != CB_ERR)
			m_ctlExt.SetCurSel(ind);
		else
			LogErrorString(_T("Failed to add string to filters combo list!"));
	}

	if (!GetOptionsMgr()->GetBool(OPT_VERIFY_OPEN_PATHS))
	{
		EnableDlgItem(IDOK, true);
		EnableDlgItem(IDC_UNPACKER_COMBO, true);
		EnableDlgItem(IDC_SELECT_UNPACKER, true);
	}

	UpdateButtonStates();

	bool bOverwriteRecursive = false;
	if (m_dwFlags[0] & FFILEOPEN_PROJECT || m_dwFlags[1] & FFILEOPEN_PROJECT)
		bOverwriteRecursive = true;
	if (m_dwFlags[0] & FFILEOPEN_CMDLINE || m_dwFlags[1] & FFILEOPEN_CMDLINE)
		bOverwriteRecursive = true;
	if (!bOverwriteRecursive)
		m_bRecurse = GetOptionsMgr()->GetBool(OPT_CMP_INCLUDE_SUBDIRS);

	m_nIgnoreWhite = GetOptionsMgr()->GetInt(OPT_CMP_IGNORE_WHITESPACE);
	m_bIgnoreBlankLines = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_BLANKLINES);
	m_bIgnoreCase = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CASE);
	m_bIgnoreEol = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_EOL);
	m_bIgnoreNumbers = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_NUMBERS);
	m_bIgnoreCodepage = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CODEPAGE);
	m_bFilterCommentsLines = GetOptionsMgr()->GetBool(OPT_CMP_FILTER_COMMENTLINES);
	m_nCompareMethod = GetOptionsMgr()->GetInt(OPT_CMP_METHOD);

	UpdateData(FALSE);
	SetStatus(IDS_OPEN_FILESDIRS);

	m_pDropHandler = new DropHandler(std::bind(&COpenView::OnDropFiles, this, std::placeholders::_1));
	RegisterDragDrop(m_hWnd, m_pDropHandler);
}

void COpenView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	m_bRecurse = GetDocument()->m_bRecurse;

	m_nIgnoreWhite = GetOptionsMgr()->GetInt(OPT_CMP_IGNORE_WHITESPACE);
	m_bIgnoreBlankLines = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_BLANKLINES);
	m_bIgnoreCase = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CASE);
	m_bIgnoreEol = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_EOL);
	m_bIgnoreNumbers = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_NUMBERS);
	m_bIgnoreCodepage = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CODEPAGE);
	m_bFilterCommentsLines = GetOptionsMgr()->GetBool(OPT_CMP_FILTER_COMMENTLINES);
	m_nCompareMethod = GetOptionsMgr()->GetInt(OPT_CMP_METHOD);

	UpdateData(FALSE);
}

// COpenView diagnostics

#ifdef _DEBUG
COpenDoc* COpenView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COpenDoc)));
	return (COpenDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COpenView message handlers

void COpenView::OnPaint()
{
	CPaintDC dc(this);
	CRect rc;
	GetClientRect(&rc);

	// Draw the logo image
	CSize size{ m_image.GetWidth(), m_image.GetHeight() };
	CRect rcImage(0, 0, size.cx * GetSystemMetrics(SM_CXSMICON) / 16, size.cy * GetSystemMetrics(SM_CYSMICON) / 16);
	m_image.Draw(dc.m_hDC, rcImage, Gdiplus::InterpolationModeBicubic);
	// And extend it to the Right boundary
	dc.PatBlt(rcImage.Width(), 0, rc.Width() - rcImage.Width(), rcImage.Height(), PATCOPY);

	// Draw the resize gripper in the Lower Right corner.
	CRect rcGrip = rc;
	rcGrip.left = rc.right - GetSystemMetrics(SM_CXVSCROLL);
	rcGrip.top = rc.bottom - GetSystemMetrics(SM_CYHSCROLL);
	dc.DrawFrameControl(&rcGrip, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);

	// Draw a line to separate the Status Line
	CPen newPen(PS_SOLID, 1, RGB(208, 208, 208));	// a very light gray
	CPen* oldpen = (CPen*)dc.SelectObject(&newPen);

	CRect rcStatus;
	GetDlgItem(IDC_OPEN_STATUS)->GetWindowRect(&rcStatus);
	ScreenToClient(&rcStatus);
	dc.MoveTo(0, rcStatus.top - 3);
	dc.LineTo(rc.right, rcStatus.top - 3);
	dc.SelectObject(oldpen);

	__super::OnPaint();
}

void COpenView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (::GetCapture() == m_hWnd)
	{
		if (CWnd *const pwndHit = ChildWindowFromPoint(point,
			CWP_SKIPINVISIBLE | CWP_SKIPDISABLED | CWP_SKIPTRANSPARENT))
		{
			switch (int const id1 = pwndHit->GetDlgCtrlID())
			{
			case IDC_PATH0_COMBO:
			case IDC_PATH1_COMBO:
			case IDC_PATH2_COMBO:
				int id2 = 0;
				CWnd *pwndChild = GetFocus();
				if (pwndChild && IsChild(pwndChild) && !pwndHit->IsChild(pwndChild)) do
				{
					id2 = pwndChild->GetDlgCtrlID();
					pwndChild = pwndChild->GetParent();
				} while (pwndChild != this);
				switch (id2)
				{
				case IDC_PATH0_COMBO:
				case IDC_PATH1_COMBO:
				case IDC_PATH2_COMBO:
					String s1, s2;
					GetDlgItemText(id1, s1);
					GetDlgItemText(id2, s2);
					SetDlgItemText(id1, s2);
					SetDlgItemText(id2, s1);
					pwndHit->SetFocus();
					break;
				}
				break;
			}
		}
		ReleaseCapture();
	}
}

void COpenView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (::GetCapture() == m_hWnd)
	{
		if (CWnd *const pwndHit = ChildWindowFromPoint(point,
			CWP_SKIPINVISIBLE | CWP_SKIPDISABLED | CWP_SKIPTRANSPARENT))
		{
			switch (pwndHit->GetDlgCtrlID())
			{
			case IDC_PATH0_COMBO:
			case IDC_PATH1_COMBO:
			case IDC_PATH2_COMBO:
				if (!pwndHit->IsChild(GetFocus()))
				{
					SetCursor(m_hIconRotate);
					break;
				}
				[[fallthrough]];
			default:
				SetCursor(m_hCursorNo);
				break;
			}
		}
	}
}

void COpenView::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if ((lpwndpos->flags & (SWP_NOMOVE | SWP_NOSIZE)) == 0)
	{
		CFrameWnd *const pFrameWnd = GetParentFrame();
		if (pFrameWnd == GetTopLevelFrame()->GetActiveFrame())
		{
			CRect rc;
			pFrameWnd->GetClientRect(&rc);
			lpwndpos->flags |= SWP_FRAMECHANGED | SWP_SHOWWINDOW;
			lpwndpos->cy = m_sizeOrig.cy;
			if (lpwndpos->flags & SWP_NOOWNERZORDER)
			{
				lpwndpos->x = rc.right - (lpwndpos->x + lpwndpos->cx);
				lpwndpos->cx = rc.right - 2 * lpwndpos->x;
				lpwndpos->y = (rc.bottom - lpwndpos->cy) / 2;
				if (lpwndpos->y < 0)
					lpwndpos->y = 0;
			}
			else if (pFrameWnd->IsZoomed())
			{
				lpwndpos->cx = m_totalLog.cx;
				lpwndpos->y = (rc.bottom - lpwndpos->cy) / 2;
				if (lpwndpos->y < 0)
					lpwndpos->y = 0;
			}
			if (lpwndpos->cx > rc.Width())
				lpwndpos->cx = rc.Width();
			if (lpwndpos->cx < m_sizeOrig.cx)
				lpwndpos->cx = m_sizeOrig.cx;
			lpwndpos->x = (rc.right - lpwndpos->cx) / 2;
			if (lpwndpos->x < 0)
				lpwndpos->x = 0;
		}
	}
}

void COpenView::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	if (lpwndpos->flags & SWP_FRAMECHANGED)
	{
		m_constraint.UpdateSizes();
		CFrameWnd *const pFrameWnd = GetParentFrame();
		if (pFrameWnd == GetTopLevelFrame()->GetActiveFrame())
		{
			m_constraint.Persist(true, false);
			WINDOWPLACEMENT wp = { sizeof wp };
			pFrameWnd->GetWindowPlacement(&wp);
			CRect rc;
			GetWindowRect(&rc);
			pFrameWnd->CalcWindowRect(&rc, CWnd::adjustOutside);
			wp.rcNormalPosition.right = wp.rcNormalPosition.left + rc.Width();
			wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + rc.Height();
			pFrameWnd->SetWindowPlacement(&wp);
		}
	}
	__super::OnWindowPosChanged(lpwndpos);
}

void COpenView::OnDestroy()
{
	if (m_pDropHandler != nullptr)
		RevokeDragDrop(m_hWnd);

	__super::OnDestroy();
}

LRESULT COpenView::OnNcHitTest(CPoint point)
{
	if (GetParentFrame()->IsZoomed())
	{
		CRect rc;
		GetWindowRect(&rc);
		rc.left = rc.right - GetSystemMetrics(SM_CXVSCROLL);
		rc.top = rc.bottom - GetSystemMetrics(SM_CYHSCROLL);
		if (PtInRect(&rc, point))
			return HTRIGHT;
	}
	return __super::OnNcHitTest(point);
}

/** 
 * @brief Called when "Browse..." button is selected for N path.
 */
void COpenView::OnPathButton(UINT nId)
{
	const int index = nId - IDC_PATH0_BUTTON;
	String s;
	String sfolder;
	UpdateData(TRUE); 

	paths::PATH_EXISTENCE existence = paths::DoesPathExist(m_strPath[index]);
	switch (existence)
	{
	case paths::IS_EXISTING_DIR:
		sfolder = m_strPath[index];
		break;
	case paths::IS_EXISTING_FILE:
		sfolder = paths::GetPathOnly(m_strPath[index]);
		break;
	case paths::DOES_NOT_EXIST:
		if (!m_strPath[index].empty())
			sfolder = paths::GetParentPath(m_strPath[index]);
		break;
	default:
		_RPTF0(_CRT_ERROR, "Invalid return value from paths::DoesPathExist()");
		break;
	}

	if (SelectFileOrFolder(GetSafeHwnd(), s, sfolder.c_str()))
	{
		m_strPath[index] = s;
		m_strBrowsePath[index] = std::move(s);
		UpdateData(FALSE);
		UpdateButtonStates();
	}	
}

void COpenView::OnSwapButton(int id1, int id2)
{
	String s1, s2;
	GetDlgItemText(id1, s1);
	GetDlgItemText(id2, s2);
	std::swap(s1, s2);
	SetDlgItemText(id1, s1);
	SetDlgItemText(id2, s2);
}

template<int id1, int id2>
void COpenView::OnSwapButton() 
{
	OnSwapButton(id1, id2);
}

void COpenView::OnCompare(UINT nID)
{
	int pathsType; // enum from paths::PATH_EXISTENCE in paths.h
	const String filterPrefix = _("[F] ");
	auto* pGlobalFileFilter = theApp.GetGlobalFileFilter();

	UpdateData(TRUE);
	TrimPaths();

	int nFiles = 0;
	for (auto& strPath : m_strPath)
	{
		if (nFiles >= 1 && strPath.empty())
			break;
		m_files.SetSize(nFiles + 1);
		m_files[nFiles] = strPath;
		m_dwFlags[nFiles] &= ~FFILEOPEN_READONLY;
		m_dwFlags[nFiles] |= m_bReadOnly[nFiles] ? FFILEOPEN_READONLY : 0;
		nFiles++;
	}
	// If left path is a project-file, load it
	String ext;
	paths::SplitFilename(m_strPath[0], nullptr, nullptr, &ext);
	if (nFiles == 1)
	{
		if (strutils::compare_nocase(ext, ProjectFile::PROJECTFILE_EXT) == 0)
		{
			theApp.LoadAndOpenProjectFile(m_strPath[0]);
		}
		else if (!paths::IsDirectory(m_strPath[0]))
		{
			PackingInfo tmpPackingInfo(m_strUnpackerPipeline);
			if (ID_UNPACKERS_FIRST <= nID && nID <= ID_UNPACKERS_LAST)
			{
				tmpPackingInfo.SetPluginPipeline(CMainFrame::GetPluginPipelineByMenuId(nID, FileTransform::UnpackerEventNames, ID_UNPACKERS_FIRST));
				nID = 0;
			}
			PrediffingInfo tmpPrediffingInfo(m_strPredifferPipeline);
			GetMainFrame()->DoSelfCompare(nID, m_strPath[0], nullptr, &tmpPackingInfo, &tmpPrediffingInfo);
		}
		return;
	}

	PackingInfo tmpPackingInfo(m_strUnpackerPipeline);
	PrediffingInfo tmpPrediffingInfo(m_strPredifferPipeline);
	{
		Merge7zFormatMergePluginScope scope(&tmpPackingInfo);

		pathsType = paths::GetPairComparability(m_files, IsArchiveFile);

		if (pathsType == paths::DOES_NOT_EXIST &&
			!std::any_of(m_files.begin(), m_files.end(), [](const auto& path) { return paths::IsURL(path); }))
		{
			LangMessageBox(IDS_ERROR_INCOMPARABLE, MB_ICONSTOP);
			return;
		}
	}

	for (int index = 0; index < nFiles; index++)
	{
		// If user has edited path by hand, expand environment variables
		bool bExpand = false;
		if (strutils::compare_nocase(m_strBrowsePath[index], m_files[index]) != 0)
			bExpand = true;

		if (!paths::IsURLorCLSID(m_files[index]))
		{
			m_files[index] = paths::GetLongPath(m_files[index], bExpand);
	
			// Add trailing '\' for directories if its missing
			if (paths::DoesPathExist(m_files[index]) == paths::IS_EXISTING_DIR && !IsArchiveFile(m_files[index]))
				m_files[index] = paths::AddTrailingSlash(m_files[index]);
			m_strPath[index] = m_files[index];
		}
	}

	UpdateData(FALSE);
	KillTimer(IDT_CHECKFILES);
	KillTimer(IDT_RETRY);

	String filter(strutils::trim_ws(m_strExt));

	// If prefix found from start..
	if (filter.substr(0, filterPrefix.length()) == filterPrefix)
	{
		// Remove prefix + space
		filter.erase(0, filterPrefix.length());
		if (!pGlobalFileFilter->SetFilter(filter))
		{
			// If filtername is not found use default *.* mask
			pGlobalFileFilter->SetFilter(_T("*.*"));
			filter = _T("*.*");
		}
		GetOptionsMgr()->SaveOption(OPT_FILEFILTER_CURRENT, filter);
	}
	else
	{
		bool bFilterSet = pGlobalFileFilter->SetFilter(filter);
		if (!bFilterSet)
			m_strExt = pGlobalFileFilter->GetFilterNameOrMask();
		GetOptionsMgr()->SaveOption(OPT_FILEFILTER_CURRENT, filter);
	}

	SaveComboboxStates();
	GetOptionsMgr()->SaveOption(OPT_CMP_INCLUDE_SUBDIRS, m_bRecurse);
	LoadComboboxStates();

	GetOptionsMgr()->SaveOption(OPT_CMP_IGNORE_WHITESPACE, m_nIgnoreWhite);
	GetOptionsMgr()->SaveOption(OPT_CMP_IGNORE_BLANKLINES, m_bIgnoreBlankLines);
	GetOptionsMgr()->SaveOption(OPT_CMP_IGNORE_CASE, m_bIgnoreCase);
	GetOptionsMgr()->SaveOption(OPT_CMP_IGNORE_EOL, m_bIgnoreEol);
	GetOptionsMgr()->SaveOption(OPT_CMP_IGNORE_NUMBERS, m_bIgnoreNumbers);
	GetOptionsMgr()->SaveOption(OPT_CMP_IGNORE_CODEPAGE, m_bIgnoreCodepage);
	GetOptionsMgr()->SaveOption(OPT_CMP_FILTER_COMMENTLINES, m_bFilterCommentsLines);
	GetOptionsMgr()->SaveOption(OPT_CMP_METHOD, m_nCompareMethod);

	m_constraint.Persist(true, false);

	COpenDoc *pDoc = GetDocument();
	pDoc->m_files = m_files;
	pDoc->m_bRecurse = m_bRecurse;
	pDoc->m_strExt = m_strExt;
	pDoc->m_strUnpackerPipeline = m_strUnpackerPipeline;
	pDoc->m_strPredifferPipeline = m_strPredifferPipeline;
	pDoc->m_dwFlags[0] = m_dwFlags[0];
	pDoc->m_dwFlags[1] = m_dwFlags[1];
	pDoc->m_dwFlags[2] = m_dwFlags[2];

	if (GetOptionsMgr()->GetBool(OPT_CLOSE_WITH_OK))
		GetParentFrame()->PostMessage(WM_CLOSE);

	// Copy the values in pDoc as it will be invalid when COpenFrame is closed. 
	PathContext tmpPathContext(pDoc->m_files);
	std::array<fileopenflags_t, 3> dwFlags = pDoc->m_dwFlags;
	bool recurse = pDoc->m_bRecurse;
	std::unique_ptr<CMainFrame::OpenFolderParams> pOpenFolderParams;
	if (!pDoc->m_hiddenItems.empty())
	{
		pOpenFolderParams = std::make_unique<CMainFrame::OpenFolderParams>();
		pOpenFolderParams->m_hiddenItems = pDoc->m_hiddenItems;
	}
	if (nID == IDOK)
	{
		GetMainFrame()->DoFileOrFolderOpen(
			&tmpPathContext, dwFlags.data(),
			nullptr, _T(""), recurse, nullptr, &tmpPackingInfo, &tmpPrediffingInfo, 0, pOpenFolderParams.get());
	}
	else if (ID_UNPACKERS_FIRST <= nID && nID <= ID_UNPACKERS_LAST)
	{
		tmpPackingInfo.SetPluginPipeline(CMainFrame::GetPluginPipelineByMenuId(nID, FileTransform::UnpackerEventNames, ID_UNPACKERS_FIRST));
		GetMainFrame()->DoFileOrFolderOpen(
			&tmpPathContext, dwFlags.data(),
			nullptr, _T(""), recurse, nullptr, &tmpPackingInfo, &tmpPrediffingInfo, 0, pOpenFolderParams.get());
	}
	else if (nID == ID_OPEN_WITH_UNPACKER)
	{
		CSelectPluginDlg dlg(pDoc->m_strUnpackerPipeline, tmpPathContext[0], 
			CSelectPluginDlg::PluginType::Unpacker, false, this);
		if (dlg.DoModal() == IDOK)
		{
			tmpPackingInfo.SetPluginPipeline(dlg.GetPluginPipeline());
			GetMainFrame()->DoFileOrFolderOpen(
				&tmpPathContext, dwFlags.data(),
				nullptr, _T(""), recurse, nullptr, &tmpPackingInfo, &tmpPrediffingInfo, 0, pOpenFolderParams.get());
		}
	}
	else
	{
		GetMainFrame()->DoFileOpen(nID, &tmpPathContext, dwFlags.data(), nullptr, _T(""), &tmpPackingInfo, &tmpPrediffingInfo, pOpenFolderParams.get());
	}
}

void COpenView::OnUpdateCompare(CCmdUI *pCmdUI)
{
	bool bFile = GetDlgItem(IDC_UNPACKER_COMBO)->IsWindowEnabled();
	if (!bFile)
	{
		UpdateData(true);
		PathContext paths = PathContext(std::vector<String>(&m_strPath[0], &m_strPath[m_strPath[2].empty() ? 2 : 3]));
		bFile = std::all_of(paths.begin(), paths.end(), [](const String& path) {
				return paths::DoesPathExist(path) == paths::IS_EXISTING_FILE;
			});
	}
	pCmdUI->Enable(bFile);
}

/** 
 * @brief Called when dialog is closed with "OK".
 *
 * Checks that paths are valid and sets filters.
 */
void COpenView::OnOK() 
{
	OnCompare(IDOK);
}

/** 
 * @brief Called when dialog is closed via Cancel.
 *
 * Open-dialog is closed when `Cancel` button is selected or the
 * `Esc` key is pressed.  Save combobox states, since user may have
 * removed items from them (with `shift-del`) and doesn't want them 
 * to re-appear.
 * This is *not* called when the program is terminated, even if the 
 * dialog is visible at the time.
 */
void COpenView::OnCancel()
{
	SaveComboboxStates();
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_CLOSE);
}

/** 
 * @brief Called when Open-button for project file is selected.
 */
void COpenView::OnLoadProject()
{
	UpdateData(TRUE);

	String fileName = AskProjectFileName(true);
	if (fileName.empty())
		return;

	ProjectFile project;
	if (!theApp.LoadProjectFile(fileName, project))
		return;
	if (project.Items().size() == 0)
		return;
	PathContext paths;
	ProjectFileItem& projItem = *project.Items().begin();
	bool bRecurse = m_bRecurse;
	projItem.GetPaths(paths, bRecurse);
	if (Options::Project::Get(GetOptionsMgr(), Options::Project::Operation::Load, Options::Project::Item::IncludeSubfolders))
		m_bRecurse = bRecurse;
	if (paths.GetSize() < 3)
	{
		m_strPath[0] = paths[0];
		m_strPath[1] = paths[1];
		m_strPath[2].clear();
		m_bReadOnly[0] = projItem.GetLeftReadOnly();
		m_bReadOnly[1] = projItem.GetRightReadOnly();
		m_bReadOnly[2] = false;
	}
	else
	{
		m_strPath[0] = paths[0];
		m_strPath[1] = paths[1];
		m_strPath[2] = paths[2];
		m_bReadOnly[0] = projItem.GetLeftReadOnly();
		m_bReadOnly[1] = projItem.GetMiddleReadOnly();
		m_bReadOnly[2] = projItem.GetRightReadOnly();
	}
	if (Options::Project::Get(GetOptionsMgr(), Options::Project::Operation::Load, Options::Project::Item::FileFilter) && projItem.HasFilter())
		m_strExt = projItem.GetFilter();
	if (Options::Project::Get(GetOptionsMgr(), Options::Project::Operation::Load, Options::Project::Item::Plugin))
	{
		if (projItem.HasUnpacker())
			m_strUnpackerPipeline = projItem.GetUnpacker();
		if (projItem.HasPrediffer())
			m_strPredifferPipeline = projItem.GetPrediffer();
	}

	if (projItem.HasWindowType())
		GetDocument()->m_nWindowType = projItem.GetWindowType();
	if (projItem.HasTableDelimiter())
		GetDocument()->m_cTableDelimiter = projItem.GetTableDelimiter();
	if (projItem.HasTableQuote())
		GetDocument()->m_cTableQuote = projItem.GetTableQuote();
	if (projItem.HasTableAllowNewLinesInQuotes())
		GetDocument()->m_bTableAllowNewLinesInQuotes = projItem.GetTableAllowNewLinesInQuotes();

	if (Options::Project::Get(GetOptionsMgr(), Options::Project::Operation::Load, Options::Project::Item::CompareOptions))
	{
		if (projItem.HasIgnoreWhite())
			m_nIgnoreWhite = projItem.GetIgnoreWhite();
		if (projItem.HasIgnoreBlankLines())
			m_bIgnoreBlankLines = projItem.GetIgnoreBlankLines();
		if (projItem.HasIgnoreCase())
			m_bIgnoreCase = projItem.GetIgnoreCase();
		if (projItem.HasIgnoreEol())
			m_bIgnoreEol = projItem.GetIgnoreEol();
		if (projItem.HasIgnoreNumbers())
			m_bIgnoreNumbers = projItem.GetIgnoreNumbers();
		if (projItem.HasIgnoreCodepage())
			m_bIgnoreCodepage = projItem.GetIgnoreCodepage();
		if (projItem.HasFilterCommentsLines())
			m_bFilterCommentsLines = projItem.GetFilterCommentsLines();
		if (projItem.HasCompareMethod())
			m_nCompareMethod = projItem.GetCompareMethod();
	}

	if ((Options::Project::Get(GetOptionsMgr(), Options::Project::Operation::Load, Options::Project::Item::HiddenItems)) && projItem.HasHiddenItems())
	{
		GetDocument()->m_hiddenItems = projItem.GetHiddenItems();
	}
	UpdateData(FALSE);
	UpdateButtonStates();
	LangMessageBox(IDS_PROJFILE_LOAD_SUCCESS, MB_ICONINFORMATION);
}

/** 
 * @brief Called when Save-button for project file is selected.
 */
void COpenView::OnSaveProject()
{
	UpdateData(TRUE);

	String fileName = AskProjectFileName(false);
	if (fileName.empty())
		return;

	ProjectFile project;
	ProjectFileItem projItem;

	bool bSaveFileFilter = Options::Project::Get(GetOptionsMgr(), Options::Project::Operation::Save, Options::Project::Item::FileFilter);
	bool bSaveIncludeSubfolders = Options::Project::Get(GetOptionsMgr(), Options::Project::Operation::Save, Options::Project::Item::IncludeSubfolders);
	bool bSavePlugin = Options::Project::Get(GetOptionsMgr(), Options::Project::Operation::Save, Options::Project::Item::Plugin);
	bool bSaveCompareOptions = Options::Project::Get(GetOptionsMgr(), Options::Project::Operation::Save, Options::Project::Item::CompareOptions);
	bool bSaveHiddenItems = Options::Project::Get(GetOptionsMgr(), Options::Project::Operation::Save, Options::Project::Item::HiddenItems);

	projItem.SetSaveFilter(bSaveFileFilter);
	projItem.SetSaveSubfolders(bSaveIncludeSubfolders);
	projItem.SetSaveUnpacker(bSavePlugin);
	projItem.SetSavePrediffer(bSavePlugin);
	projItem.SetSaveIgnoreWhite(bSaveCompareOptions);
	projItem.SetSaveIgnoreBlankLines(bSaveCompareOptions);
	projItem.SetSaveIgnoreCase(bSaveCompareOptions);
	projItem.SetSaveIgnoreEol(bSaveCompareOptions);
	projItem.SetSaveIgnoreNumbers(bSaveCompareOptions);
	projItem.SetSaveIgnoreCodepage(bSaveCompareOptions);
	projItem.SetSaveFilterCommentsLines(bSaveCompareOptions);
	projItem.SetSaveCompareMethod(bSaveCompareOptions);
	projItem.SetSaveHiddenItems(bSaveHiddenItems);

	if (!m_strPath[0].empty())
		projItem.SetLeft(m_strPath[0], &m_bReadOnly[0]);
	if (!GetDocument()->m_strDesc[0].empty())
		projItem.SetLeftDesc(GetDocument()->m_strDesc[0]);
	if (m_strPath[2].empty())
	{
		if (!m_strPath[1].empty())
			projItem.SetRight(m_strPath[1], &m_bReadOnly[1]);
		if (!GetDocument()->m_strDesc[1].empty())
			projItem.SetRightDesc(GetDocument()->m_strDesc[1]);
	}
	else
	{
		if (!m_strPath[1].empty())
			projItem.SetMiddle(m_strPath[1], &m_bReadOnly[1]);
		if (!m_strPath[2].empty())
			projItem.SetRight(m_strPath[2], &m_bReadOnly[2]);
		if (!GetDocument()->m_strDesc[1].empty())
			projItem.SetMiddleDesc(GetDocument()->m_strDesc[1]);
		if (!GetDocument()->m_strDesc[2].empty())
			projItem.SetRightDesc(GetDocument()->m_strDesc[2]);
	}
	if (bSaveFileFilter && !m_strExt.empty())
	{
		// Remove possbile prefix from the filter name
		String prefix = _("[F] ");
		String strExt = m_strExt;
		size_t ind = strExt.find(prefix, 0);
		if (ind == 0)
		{
			strExt.erase(0, prefix.length());
		}
		strExt = strutils::trim_ws_begin(strExt);
		projItem.SetFilter(strExt);
	}
	if (bSaveIncludeSubfolders)
		projItem.SetSubfolders(m_bRecurse);
	if (bSavePlugin)
	{
		if (!m_strUnpackerPipeline.empty())
			projItem.SetUnpacker(m_strUnpackerPipeline);
		if (!m_strPredifferPipeline.empty())
			projItem.SetPrediffer(m_strPredifferPipeline);
	}
	if (GetDocument()->m_nWindowType != -1)
		projItem.SetWindowType(GetDocument()->m_nWindowType);
	if (GetDocument()->m_nWindowType == 2 /* table */)
	{
		projItem.SetTableDelimiter(GetDocument()->m_cTableDelimiter);
		projItem.SetTableQuote(GetDocument()->m_cTableQuote);
		projItem.SetTableAllowNewLinesInQuotes(GetDocument()->m_bTableAllowNewLinesInQuotes);
	}

	if (bSaveCompareOptions)
	{
		projItem.SetIgnoreWhite(m_nIgnoreWhite);
		projItem.SetIgnoreBlankLines(m_bIgnoreBlankLines);
		projItem.SetIgnoreCase(m_bIgnoreCase);
		projItem.SetIgnoreEol(m_bIgnoreEol);
		projItem.SetIgnoreNumbers(m_bIgnoreNumbers);
		projItem.SetIgnoreCodepage(m_bIgnoreCodepage);
		projItem.SetFilterCommentsLines(m_bFilterCommentsLines);
		projItem.SetCompareMethod(m_nCompareMethod);
	}

	if (bSaveHiddenItems)
		projItem.SetHiddenItems(GetDocument()->m_hiddenItems);

	project.Items().push_back(projItem);

	if (!theApp.SaveProjectFile(fileName, project))
		return;

	LangMessageBox(IDS_PROJFILE_SAVE_SUCCESS, MB_ICONINFORMATION);
}

void COpenView::DropDown(NMHDR* pNMHDR, LRESULT* pResult, UINT nID, UINT nPopupID)
{
	CRect rcButton;
	GetDlgItem(nID)->GetWindowRect(&rcButton);
	BCMenu menu;
	VERIFY(menu.LoadMenu(nPopupID));
	theApp.TranslateMenu(menu.m_hMenu);
	CMenu* pPopup = menu.GetSubMenu(0);
	if (pPopup != nullptr)
	{
		if (nID == IDOK && GetDlgItem(IDC_UNPACKER_COMBO)->IsWindowEnabled())
		{
			UpdateData(TRUE);
			String tmpPath[3];
			for (int i = 0; i < 3; i++)
				tmpPath[i] = m_strPath[i].empty() ? _T("|.|") : m_strPath[i];
			String filteredFilenames = strutils::join(std::begin(tmpPath), std::end(tmpPath), _T("|"));
			CMainFrame::AppendPluginMenus(pPopup, filteredFilenames, FileTransform::UnpackerEventNames, true, ID_UNPACKERS_FIRST);
		}
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
			rcButton.left, rcButton.bottom, GetMainFrame());
	}
	*pResult = 0;
}

template<UINT id, UINT popupid>
void COpenView::OnDropDown(NMHDR *pNMHDR, LRESULT *pResult)
{
	DropDown(pNMHDR, pResult, id, popupid);
}

/** 
 * @brief Allow user to select a file to open/save.
 */
String COpenView::AskProjectFileName(bool bOpen)
{
	// get the default projects path
	String strProjectFileName;
	String strProjectPath = GetOptionsMgr()->GetString(OPT_PROJECTS_PATH);

	if (!::SelectFile(GetSafeHwnd(), strProjectFileName, bOpen, strProjectPath.c_str(),
			_T(""), _("WinMerge Project Files (*.WinMerge)|*.WinMerge||"), _T(".WinMerge")))
		return _T("");

	if (strProjectFileName.empty())
		return _T("");

	// get the path part from the filename
	strProjectPath = paths::GetParentPath(strProjectFileName);
	// store this as the new project path
	GetOptionsMgr()->SaveOption(OPT_PROJECTS_PATH, strProjectPath);
	return strProjectFileName;
}

/** 
 * @brief Load File- and filter-combobox states.
 */
void COpenView::LoadComboboxStates()
{
	m_ctlPath[0].LoadState(_T("Files\\Left"));
	m_ctlPath[1].LoadState(_T("Files\\Right"));
	m_ctlPath[2].LoadState(_T("Files\\Option"));
	m_ctlExt.LoadState(_T("Files\\Ext"));
	m_ctlUnpackerPipeline.LoadState(_T("Files\\Unpacker"));
	m_ctlPredifferPipeline.LoadState(_T("Files\\Prediffer"));
}

/** 
 * @brief Save File- and filter-combobox states.
 */
void COpenView::SaveComboboxStates()
{
	m_ctlPath[0].SaveState(_T("Files\\Left"));
	m_ctlPath[1].SaveState(_T("Files\\Right"));
	m_ctlPath[2].SaveState(_T("Files\\Option"));
	m_ctlExt.SaveState(_T("Files\\Ext"));
	m_ctlUnpackerPipeline.SaveState(_T("Files\\Unpacker"));
	m_ctlPredifferPipeline.SaveState(_T("Files\\Prediffer"));
}

struct UpdateButtonStatesThreadParams
{
	HWND m_hWnd;
	PathContext m_paths;
};

static UINT UpdateButtonStatesThread(LPVOID lpParam)
{
	MSG msg;
	BOOL bRet;

	CoInitialize(nullptr);
	CAssureScriptsForThread scriptsForRescan(new MergeAppCOMClass());

	while( (bRet = GetMessage( &msg, nullptr, 0, 0 )) != 0)
	{ 
		if (bRet == -1)
			break;
		if (msg.message != WM_USER + 2)
			continue;

		bool bIsaFolderCompare = true;
		bool bIsaFileCompare = true;
		bool bInvalid[3] = {false, false, false};
		paths::PATH_EXISTENCE pathType[3] = {paths::DOES_NOT_EXIST, paths::DOES_NOT_EXIST, paths::DOES_NOT_EXIST};
		int iStatusMsgId = IDS_OPEN_FILESDIRS;

		UpdateButtonStatesThreadParams *pParams = reinterpret_cast<UpdateButtonStatesThreadParams *>(msg.wParam);
		PathContext paths = pParams->m_paths;
		HWND hWnd = pParams->m_hWnd;
		delete pParams;

		// Check if we have project file as left side path
		bool bProject = false;
		String ext;
		paths::SplitFilename(paths[0], nullptr, nullptr, &ext);
		if (paths[1].empty() && strutils::compare_nocase(ext, ProjectFile::PROJECTFILE_EXT) == 0)
			bProject = true;

		if (!bProject)
		{
			for (int i = 0; i < paths.GetSize(); ++i)
			{
				pathType[i] = paths::DoesPathExist(paths[i], IsArchiveFile);
				if (pathType[i] == paths::DOES_NOT_EXIST)
				{
					if (paths::IsURL(paths[i]))
						pathType[i] = paths::IS_EXISTING_FILE;
					else
						bInvalid[i] = true;
				}
			}
		}

		// Enable buttons as appropriate
		if (GetOptionsMgr()->GetBool(OPT_VERIFY_OPEN_PATHS))
		{
			paths::PATH_EXISTENCE pathsType = pathType[0];

			if (paths.GetSize() <= 2)
			{
				if (bInvalid[0] && bInvalid[1])
					iStatusMsgId = IDS_OPEN_BOTHINVALID;
				else if (bInvalid[0])
					iStatusMsgId = IDS_OPEN_LEFTINVALID;
				else if (bInvalid[1])
				{
					if (pathType[0] == paths::IS_EXISTING_FILE && (paths.GetSize() == 1 || paths[1].empty()))
						iStatusMsgId = IDS_OPEN_FILESDIRS;
					else
						iStatusMsgId = IDS_OPEN_RIGHTINVALID2;
				}
				else if (!bInvalid[0] && !bInvalid[1])
				{
					if (pathType[0] != pathType[1])
						iStatusMsgId = IDS_OPEN_MISMATCH;
					else
						iStatusMsgId = IDS_OPEN_FILESDIRS;
				}
			}
			else
			{
				if (bInvalid[0] && bInvalid[1] && bInvalid[2])
					iStatusMsgId = IDS_OPEN_ALLINVALID;
				else if (!bInvalid[0] && bInvalid[1] && bInvalid[2])
					iStatusMsgId = IDS_OPEN_MIDDLERIGHTINVALID;
				else if (bInvalid[0] && !bInvalid[1] && bInvalid[2])
					iStatusMsgId = IDS_OPEN_LEFTRIGHTINVALID;
				else if (!bInvalid[0] && !bInvalid[1] && bInvalid[2])
					iStatusMsgId = IDS_OPEN_RIGHTINVALID3;
				else if (bInvalid[0] && bInvalid[1] && !bInvalid[2])
					iStatusMsgId = IDS_OPEN_LEFTMIDDLEINVALID;
				else if (!bInvalid[0] && bInvalid[1] && !bInvalid[2])
					iStatusMsgId = IDS_OPEN_MIDDLEINVALID;
				else if (bInvalid[0] && !bInvalid[1] && !bInvalid[2])
					iStatusMsgId = IDS_OPEN_LEFTINVALID;
				else if (!bInvalid[0] && !bInvalid[1] && !bInvalid[2])
				{
					if (pathType[0] != pathType[1] || pathType[0] != pathType[2])
						iStatusMsgId = IDS_OPEN_MISMATCH;
					else
						iStatusMsgId = IDS_OPEN_FILESDIRS;
				}
			}
			if (iStatusMsgId != IDS_OPEN_FILESDIRS)
				pathsType = paths::DOES_NOT_EXIST;
			bIsaFileCompare = (pathsType == paths::IS_EXISTING_FILE);
			bIsaFolderCompare = (pathsType == paths::IS_EXISTING_DIR);
			// Both will be `false` if incompatibilities or something is missing
			// Both will end up `true` if file validity isn't being checked
		}

		PostMessage(hWnd, WM_USER + 1, MAKEWPARAM(bIsaFolderCompare, bIsaFileCompare), MAKELPARAM(iStatusMsgId, bProject)); 
	}

	CoUninitialize();

	return 0;
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void COpenView::UpdateResources()
{
	theApp.m_pLangDlg->RetranslateDialog(m_hWnd, MAKEINTRESOURCE(IDD_OPEN));
}

/** 
 * @brief Enable/disable components based on validity of paths.
 */
void COpenView::UpdateButtonStates()
{
	UpdateData(TRUE); // load member variables from screen
	KillTimer(IDT_CHECKFILES);
	TrimPaths();
	
	if (m_pUpdateButtonStatusThread == nullptr)
	{
		m_pUpdateButtonStatusThread = AfxBeginThread(
			UpdateButtonStatesThread, nullptr, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		m_pUpdateButtonStatusThread->m_bAutoDelete = FALSE;
		m_pUpdateButtonStatusThread->ResumeThread();
		while (PostThreadMessage(m_pUpdateButtonStatusThread->m_nThreadID, WM_NULL, 0, 0) == FALSE)
			Sleep(1);
	}

	UpdateButtonStatesThreadParams *pParams = new UpdateButtonStatesThreadParams;
	pParams->m_hWnd = this->m_hWnd;
	pParams->m_paths = PathContext(std::vector<String>(&m_strPath[0], &m_strPath[m_strPath[2].empty() ? 2 : 3]));

	PostThreadMessage(m_pUpdateButtonStatusThread->m_nThreadID, WM_USER + 2, (WPARAM)pParams, 0);
}

void COpenView::TerminateThreadIfRunning()
{
	if (m_pUpdateButtonStatusThread == nullptr)
		return;

	PostThreadMessage(m_pUpdateButtonStatusThread->m_nThreadID, WM_QUIT, 0, 0);
	DWORD dwResult = WaitForSingleObject(m_pUpdateButtonStatusThread->m_hThread, 100);
	if (dwResult != WAIT_OBJECT_0)
	{
		m_pUpdateButtonStatusThread->SuspendThread();
		TerminateThread(m_pUpdateButtonStatusThread->m_hThread, 0);
	}
	delete m_pUpdateButtonStatusThread;
	m_pUpdateButtonStatusThread = nullptr;
}

/**
 * @brief Called when user changes selection in left/middle/right path's combo box.
 */
void COpenView::OnSelchangePathCombo(UINT nId) 
{
	const int index = nId - IDC_PATH0_COMBO;
	int sel = m_ctlPath[index].GetCurSel();
	if (sel != CB_ERR)
	{
		CString cstrPath;
		m_ctlPath[index].GetLBText(sel, cstrPath);
		m_strPath[index] = cstrPath;
		m_ctlPath[index].SetWindowText(cstrPath);
		UpdateData(TRUE);
	}
	UpdateButtonStates();
}

void COpenView::OnSetfocusPathCombo(UINT id, NMHDR *pNMHDR, LRESULT *pResult) 
{
	if (!m_bAutoCompleteReady[id - IDC_PATH0_COMBO])
	{
		int nSource = GetOptionsMgr()->GetInt(OPT_AUTO_COMPLETE_SOURCE);
		if (nSource > 0)
			m_ctlPath[id - IDC_PATH0_COMBO].SetAutoComplete(nSource);
		m_bAutoCompleteReady[id - IDC_PATH0_COMBO] = true;
	}
	*pResult = 0;
}

void COpenView::OnDragBeginPathCombo(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	m_ctlPath[id - IDC_PATH0_COMBO].SetFocus();
	SetCapture();
	*pResult = 0;
}

/**
 * @brief Called every time paths are edited.
 */
void COpenView::OnEditEvent(UINT nID)
{
	const int N = nID - IDC_PATH0_COMBO;
	if (CEdit *const edit = m_ctlPath[N].GetEditCtrl())
	{
		int const len = edit->GetWindowTextLength();
		if (edit->GetSel() == MAKEWPARAM(len, len))
		{
			CString text;
			edit->GetWindowText(text);
			// Remove any double quotes
			text.Remove('"');
			if (text.GetLength() != len)
			{
				edit->SetSel(0, len);
				edit->ReplaceSel(text);
			}
		}
	}
	// (Re)start timer to path validity check delay
	// If timer starting fails, update buttonstates immediately
	if (!SetTimer(IDT_CHECKFILES, CHECKFILES_TIMEOUT, nullptr))
		UpdateButtonStates();
}

/**
 * @brief Handle timer events.
 * Checks if paths are valid and sets control states accordingly.
 * @param [in] nIDEvent Timer ID that fired.
 */
void COpenView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == IDT_CHECKFILES || nIDEvent == IDT_RETRY)
		UpdateButtonStates();

	__super::OnTimer(nIDEvent);
}

/**
 * @brief Called when users selects plugin browse button.
 */
void COpenView::OnSelectPlugin(UINT nID)
{
	paths::PATH_EXISTENCE pathsType;
	UpdateData(TRUE);

	int nFiles = 0;
	for (auto& strPath: m_strPath)
	{
		if (nFiles == 2 && strPath.empty())
			break;
		m_files.SetSize(nFiles + 1);
		m_files[nFiles] = strPath;
		nFiles++;
	}
	PathContext tmpFiles = m_files;
	if (tmpFiles.GetSize() == 2 && tmpFiles[1].empty())
		tmpFiles[1] = tmpFiles[0];
	pathsType = paths::GetPairComparability(tmpFiles, IsArchiveFile);

	if (pathsType == paths::IS_EXISTING_DIR || (pathsType == paths::DOES_NOT_EXIST &&
		!std::any_of(m_files.begin(), m_files.end(), [](const auto& path) { return paths::IsURL(path); })))
		return;

	// let the user select a handler
	CSelectPluginDlg dlg(nID == IDC_SELECT_UNPACKER ? m_strUnpackerPipeline : m_strPredifferPipeline, m_files[0], 
		nID == IDC_SELECT_UNPACKER ? CSelectPluginDlg::PluginType::Unpacker : CSelectPluginDlg::PluginType::Prediffer, false, this);
	if (dlg.DoModal() == IDOK)
	{
		if (nID == IDC_SELECT_UNPACKER)
			m_strUnpackerPipeline = dlg.GetPluginPipeline();
		else
			m_strPredifferPipeline = dlg.GetPluginPipeline();
		UpdateData(FALSE);
	}
}

LRESULT COpenView::OnUpdateStatus(WPARAM wParam, LPARAM lParam)
{
	const bool bIsaFolderCompare = LOWORD(wParam) != 0;
	const bool bIsaFileCompare = HIWORD(wParam) != 0;
	const bool bProject = HIWORD(lParam) != 0;
	const int iStatusMsgId = LOWORD(lParam);

	EnableDlgItem(IDOK, bIsaFolderCompare || bIsaFileCompare || bProject);

	for (auto nID : { IDC_FILES_DIRS_GROUP4, IDC_UNPACKER_COMBO, IDC_SELECT_UNPACKER })
	{
		EnableDlgItem(nID, bIsaFileCompare);
	}

	if (GetOptionsMgr()->GetBool(OPT_VERIFY_OPEN_PATHS))
	{
		for (auto nID : { IDC_FILES_DIRS_GROUP5, IDC_PREDIFFER_COMBO, IDC_SELECT_PREDIFFER })
		{
			GetDlgItem(nID)->ShowWindow(bIsaFileCompare ? SW_SHOW : SW_HIDE);
			EnableDlgItem(nID, bIsaFileCompare);
		}

		for (auto nID : { IDC_FILES_DIRS_GROUP3, IDC_EXT_COMBO, IDC_SELECT_FILTER, IDC_RECURS_CHECK })
		{
			GetDlgItem(nID)->ShowWindow((bIsaFolderCompare || !bIsaFileCompare) ? SW_SHOW : SW_HIDE);
			EnableDlgItem(nID, bIsaFolderCompare);
		}
	}
	
	SetStatus(iStatusMsgId);

	if (iStatusMsgId != IDS_OPEN_FILESDIRS && m_retryCount <= RETRY_MAX)
	{
		if (m_retryCount == 0)
			SetTimer(IDT_RETRY, CHECKFILES_TIMEOUT, nullptr);
		m_retryCount++;
	}
	else
	{
		KillTimer(IDT_RETRY);
		m_retryCount = 0;
	}
	return 0;
}

/**
 * @brief Sets the path status text.
 * The open dialog shows a status text of selected paths. This function
 * is used to set that status text.
 * @param [in] msgID Resource ID of status text to set.
 */
void COpenView::SetStatus(UINT msgID)
{
	String msg = theApp.LoadString(msgID);
	SetDlgItemText(IDC_OPEN_STATUS, msg);
}

/** 
 * @brief Called when "Select..." button for filters is selected.
 */
void COpenView::OnSelectFilter()
{
	String curFilter;
	auto* pGlobalFileFilter = theApp.GetGlobalFileFilter();

	const bool bUseMask = pGlobalFileFilter->IsUsingMask();
	GetDlgItemText(IDC_EXT_COMBO, curFilter);
	curFilter = strutils::trim_ws(curFilter);

	GetMainFrame()->SelectFilter();
	
	String filterNameOrMask = pGlobalFileFilter->GetFilterNameOrMask();
	if (pGlobalFileFilter->IsUsingMask())
	{
		// If we had filter chosen and now has mask we can overwrite filter
		if (!bUseMask || curFilter[0] != '*')
		{
			SetDlgItemText(IDC_EXT_COMBO, filterNameOrMask);
		}
	}
	else
	{
		String filterPrefix = _("[F] ");
		filterNameOrMask = filterPrefix + filterNameOrMask;
		SetDlgItemText(IDC_EXT_COMBO, filterNameOrMask);
	}
}

void COpenView::OnOptions()
{
	GetMainFrame()->PostMessage(WM_COMMAND, ID_OPTIONS);
}

/**
 * @brief Set "Whitespaces" setting.
 * @param [in] nID Menu ID of the selected item
 */
void COpenView::OnDiffWhitespace(UINT nID)
{
	assert(nID >= ID_PROJECT_DIFF_OPTIONS_WHITESPACE_COMPARE && nID <= ID_PROJECT_DIFF_OPTIONS_WHITESPACE_IGNOREALL);

	m_nIgnoreWhite = nID - ID_PROJECT_DIFF_OPTIONS_WHITESPACE_COMPARE;
}

/**
 * @brief Update "Whitespaces" state.
 * @param [in] pCmdUI UI component to update.
 */
void COpenView::OnUpdateDiffWhitespace(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio((pCmdUI->m_nID - ID_PROJECT_DIFF_OPTIONS_WHITESPACE_COMPARE) == static_cast<UINT>(m_nIgnoreWhite));
}

/**
 * @brief Toggle "Ignore blank lines" setting.
 */
void COpenView::OnDiffIgnoreBlankLines()
{
	m_bIgnoreBlankLines = !m_bIgnoreBlankLines;
}

/**
 * @brief Update "Ignore blank lines" state.
 * @param [in] pCmdUI UI component to update.
 */
void COpenView::OnUpdateDiffIgnoreBlankLines(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bIgnoreBlankLines);
}

/**
 * @brief Toggle "Ignore case" setting.
 */
void COpenView::OnDiffIgnoreCase()
{
	m_bIgnoreCase = !m_bIgnoreCase;
}

/**
 * @brief Update "Ignore case" state.
 * @param [in] pCmdUI UI component to update.
 */
void COpenView::OnUpdateDiffIgnoreCase(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bIgnoreCase);
}

/**
 * @brief Toggle "Ignore carriage return differences" setting.
 */
void COpenView::OnDiffIgnoreEOL()
{
	m_bIgnoreEol = !m_bIgnoreEol;
}

/**
 * @brief Update "Ignore carriage return differences" state.
 * @param [in] pCmdUI UI component to update.
 */
void COpenView::OnUpdateDiffIgnoreEOL(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bIgnoreEol);
}

/**
 * @brief Toggle "Ignore numbers" setting.
 */
void COpenView::OnDiffIgnoreNumbers()
{
	m_bIgnoreNumbers = !m_bIgnoreNumbers;
}

/**
 * @brief Update "Ignore numbers" state.
 * @param [in] pCmdUI UI component to update.
 */
void COpenView::OnUpdateDiffIgnoreNumbers(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bIgnoreNumbers);
}

/**
 * @brief Toggle "Ignore codepage differences" setting.
 */
void COpenView::OnDiffIgnoreCP()
{
	m_bIgnoreCodepage = !m_bIgnoreCodepage;
}

/**
 * @brief Update "Ignore codepage differences" state.
 * @param [in] pCmdUI UI component to update.
 */
void COpenView::OnUpdateDiffIgnoreCP(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bIgnoreCodepage);
}

/**
 * @brief Toggle "Ignore comment differences" setting.
 */
void COpenView::OnDiffIgnoreComments()
{
	m_bFilterCommentsLines = !m_bFilterCommentsLines;
}

/**
 * @brief Update "Ignore comment differences" state.
 * @param [in] pCmdUI UI component to update.
 */
void COpenView::OnUpdateDiffIgnoreComments(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bFilterCommentsLines);
}

/**
 * @brief Set "Compare method" setting.
 * @param [in] nID Menu ID of the selected item
 */
void COpenView::OnCompareMethod(UINT nID)
{
	assert(nID >= ID_PROJECT_DIFF_OPTIONS_COMPMETHOD_FULL_CONTENTS && nID <= ID_PROJECT_DIFF_OPTIONS_COMPMETHOD_SIZE);

	m_nCompareMethod = nID - ID_PROJECT_DIFF_OPTIONS_COMPMETHOD_FULL_CONTENTS;
}

/**
 * @brief Update "Compare method" state.
 * @param [in] pCmdUI UI component to update.
 */
void COpenView::OnUpdateCompareMethod(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio((pCmdUI->m_nID - ID_PROJECT_DIFF_OPTIONS_COMPMETHOD_FULL_CONTENTS) == static_cast<UINT>(m_nCompareMethod));
}

/** 
 * @brief Removes whitespaces from left and right paths
 * @note Assumes UpdateData(TRUE) is called before this function.
 */
void COpenView::TrimPaths()
{
	for (auto& strPath: m_strPath)
		strPath = strutils::trim_ws(strPath);
}

/** 
 * @brief Update control states when dialog is activated.
 *
 * Update control states when user re-activates dialog. User might have
 * switched for other program to e.g. update files/folders and then
 * swiches back to WinMerge. Its nice to see WinMerge detects updated
 * files/folders.
 */
void COpenView::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	__super::OnActivate(nState, pWndOther, bMinimized);

	if (nState == WA_ACTIVE || nState == WA_CLICKACTIVE)
		UpdateButtonStates();
}

void COpenView::OnEditAction(int msg, WPARAM wParam, LPARAM lParam)
{
	CWnd *pCtl = GetFocus();
	if (pCtl != nullptr)
		pCtl->PostMessage(msg, wParam, lParam);
}

/**
 * @brief Open help from mainframe when user presses F1.
 */
void COpenView::OnHelp()
{
	theApp.ShowHelp(OpenDlgHelpLocation);
}

/////////////////////////////////////////////////////////////////////////////
//
//	OnDropFiles code from CDropEdit
//	Copyright 1997 Chris Losinger
//
//	shortcut expansion code modified from :
//	CShortcut, 1996 Rob Warner
//

/**
 * @brief Drop paths(s) to the dialog.
 * One or two paths can be dropped to the dialog. The behaviour is:
 *   If 1 file:
 *     - drop to empty path edit box (check left first)
 *     - if both boxes have a path, drop to left path
 *   If two files:
 *    - overwrite both paths, empty or not
 * @param [in] dropInfo Dropped data, including paths.
 */
void COpenView::OnDropFiles(const std::vector<String>& files)
{
	const size_t fileCount = files.size();

	// Add dropped paths to the dialog
	UpdateData(TRUE);
	if (fileCount == 3)
	{
		m_strPath[0] = files[0];
		m_strPath[1] = files[1];
		m_strPath[2] = files[2];
		UpdateData(FALSE);
		UpdateButtonStates();
	}
	else if (fileCount == 2)
	{
		m_strPath[0] = files[0];
		m_strPath[1] = files[1];
		UpdateData(FALSE);
		UpdateButtonStates();
	}
	else if (fileCount == 1)
	{
		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		if (CWnd *const pwndHit = ChildWindowFromPoint(point,
			CWP_SKIPINVISIBLE | CWP_SKIPDISABLED | CWP_SKIPTRANSPARENT))
		{
			switch (int const id = pwndHit->GetDlgCtrlID())
			{
			case IDC_PATH0_COMBO:
			case IDC_PATH1_COMBO:
			case IDC_PATH2_COMBO:
				m_strPath[id - IDC_PATH0_COMBO] = files[0];
				break;
			default:
				if (m_strPath[0].empty())
					m_strPath[0] = files[0];
				else if (m_strPath[1].empty())
					m_strPath[1] = files[0];
				else if (m_strPath[2].empty())
					m_strPath[2] = files[0];
				else
					m_strPath[0] = files[0];
				break;
			}
		}
		UpdateData(FALSE);
		UpdateButtonStates();
	}
}
