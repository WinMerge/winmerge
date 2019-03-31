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
#include "SelectUnpackerDlg.h"
#include "OptionsDef.h"
#include "MainFrm.h"
#include "OptionsMgr.h"
#include "FileOrFolderSelect.h"
#include "7zCommon.h"
#include "Constants.h"
#include "Picture.h"
#include "DropHandler.h"
#include "FileFilterHelper.h"
#include "Plugins.h"
#include "BCMenu.h"
#include "LanguageSelect.h"
#include "Win_VersionHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef BCN_DROPDOWN
#define BCN_DROPDOWN            (BCN_FIRST + 0x0002)
#endif

// Timer ID and timeout for delaying path validity check
const UINT IDT_CHECKFILES = 1;
const UINT CHECKFILES_TIMEOUT = 1000; // milliseconds
static const TCHAR EMPTY_EXTENSION[] = _T(".*");

/** @brief Location for Open-dialog specific help to open. */
static TCHAR OpenDlgHelpLocation[] = _T("::/htmlhelp/Open_paths.html");

// COpenView

IMPLEMENT_DYNCREATE(COpenView, CFormView)

BEGIN_MESSAGE_MAP(COpenView, CFormView)
	//{{AFX_MSG_MAP(COpenView)
	ON_BN_CLICKED(IDC_PATH0_BUTTON, OnPathButton<0>)
	ON_BN_CLICKED(IDC_PATH1_BUTTON, OnPathButton<1>)
	ON_BN_CLICKED(IDC_PATH2_BUTTON, OnPathButton<2>)
	ON_BN_CLICKED(IDC_SWAP01_BUTTON, (OnSwapButton<IDC_PATH0_COMBO, IDC_PATH1_COMBO>))
	ON_BN_CLICKED(IDC_SWAP12_BUTTON, (OnSwapButton<IDC_PATH1_COMBO, IDC_PATH2_COMBO>))
	ON_BN_CLICKED(IDC_SWAP02_BUTTON, (OnSwapButton<IDC_PATH0_COMBO, IDC_PATH2_COMBO>))
	ON_CBN_SELCHANGE(IDC_PATH0_COMBO, OnSelchangePathCombo<0>)
	ON_CBN_SELCHANGE(IDC_PATH1_COMBO, OnSelchangePathCombo<1>)
	ON_CBN_SELCHANGE(IDC_PATH2_COMBO, OnSelchangePathCombo<2>)
	ON_CBN_EDITCHANGE(IDC_PATH0_COMBO, OnEditEvent<0>)
	ON_CBN_EDITCHANGE(IDC_PATH1_COMBO, OnEditEvent<1>)
	ON_CBN_EDITCHANGE(IDC_PATH2_COMBO, OnEditEvent<2>)
	ON_BN_CLICKED(IDC_SELECT_UNPACKER, OnSelectUnpacker)
	ON_CBN_SELENDCANCEL(IDC_PATH0_COMBO, UpdateButtonStates)
	ON_CBN_SELENDCANCEL(IDC_PATH1_COMBO, UpdateButtonStates)
	ON_CBN_SELENDCANCEL(IDC_PATH2_COMBO, UpdateButtonStates)
	ON_NOTIFY_RANGE(CBEN_BEGINEDIT, IDC_PATH0_COMBO, IDC_PATH2_COMBO, OnSetfocusPathCombo)
	ON_NOTIFY_RANGE(CBEN_DRAGBEGIN, IDC_PATH0_COMBO, IDC_PATH2_COMBO, OnDragBeginPathCombo)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SELECT_FILTER, OnSelectFilter)
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	ON_NOTIFY(BCN_DROPDOWN, IDC_OPTIONS, OnDropDownOptions)
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_LOAD_PROJECT, OnLoadProject)
	ON_COMMAND(ID_SAVE_PROJECT, OnSaveProject)
	ON_NOTIFY(BCN_DROPDOWN, ID_SAVE_PROJECT, OnDropDownSaveProject)
	ON_COMMAND(IDOK, OnOK)
	ON_COMMAND(IDCANCEL, OnCancel)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_EDIT_COPY, OnEditAction<WM_COPY>)
	ON_COMMAND(ID_EDIT_PASTE, OnEditAction<WM_PASTE>)
	ON_COMMAND(ID_EDIT_CUT, OnEditAction<WM_CUT>)
	ON_COMMAND(ID_EDIT_UNDO, OnEditAction<WM_UNDO>)
	ON_COMMAND(ID_EDIT_SELECT_ALL, (OnEditAction<EM_SETSEL, 0, -1>))
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
{
}

COpenView::~COpenView()
{
	TerminateThreadIfRunning();
}

void COpenView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenView)
	DDX_Control(pDX, IDC_EXT_COMBO, m_ctlExt);
	DDX_Control(pDX, IDC_PATH0_COMBO, m_ctlPath[0]);
	DDX_Control(pDX, IDC_PATH1_COMBO, m_ctlPath[1]);
	DDX_Control(pDX, IDC_PATH2_COMBO, m_ctlPath[2]);
	DDX_CBStringExact(pDX, IDC_PATH0_COMBO, m_strPath[0]);
	DDX_CBStringExact(pDX, IDC_PATH1_COMBO, m_strPath[1]);
	DDX_CBStringExact(pDX, IDC_PATH2_COMBO, m_strPath[2]);
	DDX_Check(pDX, IDC_PATH0_READONLY, m_bReadOnly[0]);
	DDX_Check(pDX, IDC_PATH1_READONLY, m_bReadOnly[1]);
	DDX_Check(pDX, IDC_PATH2_READONLY, m_bReadOnly[2]);
	DDX_Check(pDX, IDC_RECURS_CHECK, m_bRecurse);
	DDX_CBStringExact(pDX, IDC_EXT_COMBO, m_strExt);
	DDX_Text(pDX, IDC_UNPACKER_EDIT, m_strUnpacker);
	//}}AFX_DATA_MAP
}

BOOL COpenView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style &= ~WS_BORDER;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	return CFormView::PreCreateWindow(cs);
}

void COpenView::OnInitialUpdate()
{
	if (!IsVista_OrGreater())
	{
		// fallback for XP 
		SendDlgItemMessage(IDC_OPTIONS, BM_SETSTYLE, BS_PUSHBUTTON, TRUE);
		SendDlgItemMessage(ID_SAVE_PROJECT, BM_SETSTYLE, BS_PUSHBUTTON, TRUE);
	}

	m_sizeOrig = GetTotalSize();

	theApp.TranslateDialog(m_hWnd);

	if (!m_picture.Load(IDR_LOGO))
		return;

	CFormView::OnInitialUpdate();

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
	// configure how individual controls adjust when dialog resizes
	m_constraint.ConstrainItem(IDC_PATH0_COMBO, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_PATH1_COMBO, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_PATH2_COMBO, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_EXT_COMBO, 0, 0.5, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_UNPACKER_EDIT, 0.5, 0.5, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_FILES_DIRS_GROUP0, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_FILES_DIRS_GROUP1, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_FILES_DIRS_GROUP2, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_FILES_DIRS_GROUP3X, 0, 0.5, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_FILES_DIRS_GROUP4, 0.5, 0, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_FILES_DIRS_GROUP4X, 0.5, 0.5, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_PATH0_BUTTON, 1, 0, 0, 0); // slides right
	m_constraint.ConstrainItem(IDC_PATH1_BUTTON, 1, 0, 0, 0); // slides right
	m_constraint.ConstrainItem(IDC_PATH2_BUTTON, 1, 0, 0, 0); // slides right
	m_constraint.ConstrainItem(IDC_SELECT_UNPACKER, 1, 0, 0, 0); // slides right
	m_constraint.ConstrainItem(IDC_OPEN_STATUS, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_SELECT_FILTER, 0.5, 0, 0, 0); // slides right
	m_constraint.ConstrainItem(IDC_OPTIONS, 1, 0, 0, 0); // slides right
	m_constraint.ConstrainItem(IDOK, 1, 0, 0, 0); // slides right
	m_constraint.ConstrainItem(IDCANCEL, 1, 0, 0, 0); // slides right
	m_constraint.ConstrainItem(ID_HELP, 1, 0, 0, 0); // slides right
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
	m_strUnpacker = pDoc->m_strUnpacker;
	m_infoHandler = pDoc->m_infoHandler;
	m_dwFlags[0] = pDoc->m_dwFlags[0];
	m_dwFlags[1] = pDoc->m_dwFlags[1];
	m_dwFlags[2] = pDoc->m_dwFlags[2];

	m_ctlPath[0].SetFileControlStates();
	m_ctlPath[1].SetFileControlStates();
	m_ctlPath[2].SetFileControlStates(true);

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

	bool bDoUpdateData = true;
	for (int index = 0; index < countof(m_strPath); index++)
	{
		if (!m_strPath[index].empty())
			bDoUpdateData = false;
	}
	UpdateData(bDoUpdateData);

	String filterNameOrMask = theApp.m_pGlobalFileFilter->GetFilterNameOrMask();
	bool bMask = theApp.m_pGlobalFileFilter->IsUsingMask();

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
		EnableDlgItem(IDC_UNPACKER_EDIT, true);
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

	m_strUnpacker = m_infoHandler.m_PluginName;
	UpdateData(FALSE);
	SetStatus(IDS_OPEN_FILESDIRS);
	SetUnpackerStatus(IDS_USERCHOICE_NONE); 

	m_pDropHandler = new DropHandler(std::bind(&COpenView::OnDropFiles, this, std::placeholders::_1));
	RegisterDragDrop(m_hWnd, m_pDropHandler);
}

void COpenView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	m_bRecurse = GetDocument()->m_bRecurse;
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
	CSize size = m_picture.GetImageSize(&dc);
	CRect rcImage(0, 0, size.cx * GetSystemMetrics(SM_CXSMICON) / 16, size.cy * GetSystemMetrics(SM_CYSMICON) / 16);
	m_picture.Render(&dc, rcImage);
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

	CFormView::OnPaint();
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
				if (IsChild(pwndChild) && !pwndHit->IsChild(pwndChild)) do
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
				// fall through
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
			WINDOWPLACEMENT wp;
			wp.length = sizeof wp;
			pFrameWnd->GetWindowPlacement(&wp);
			CRect rc;
			GetWindowRect(&rc);
			pFrameWnd->CalcWindowRect(&rc, CWnd::adjustOutside);
			wp.rcNormalPosition.right = wp.rcNormalPosition.left + rc.Width();
			wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + rc.Height();
			pFrameWnd->SetWindowPlacement(&wp);
		}
	}
	CFormView::OnWindowPosChanged(lpwndpos);
}

void COpenView::OnDestroy()
{
	if (m_pDropHandler != nullptr)
		RevokeDragDrop(m_hWnd);

	CFormView::OnDestroy();
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
	return CFormView::OnNcHitTest(point);
}

void COpenView::OnButton(int index)
{
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
		// Do nothing, empty foldername will be passed to dialog
		break;
	default:
		_RPTF0(_CRT_ERROR, "Invalid return value from paths::DoesPathExist()");
		break;
	}

	if (SelectFileOrFolder(GetSafeHwnd(), s, sfolder.c_str()))
	{
		m_strPath[index] = s;
		m_strBrowsePath[index] = s;
		UpdateData(FALSE);
		UpdateButtonStates();
	}	
}

/** 
 * @brief Called when "Browse..." button is selected for N path.
 */
template <int N>
void COpenView::OnPathButton()
{
	OnButton(N);
}

template<int id1, int id2>
void COpenView::OnSwapButton() 
{
	String s1, s2;
	GetDlgItemText(id1, s1);
	GetDlgItemText(id2, s2);
	std::swap(s1, s2);
	SetDlgItemText(id1, s1);
	SetDlgItemText(id2, s2);
}

/** 
 * @brief Called when dialog is closed with "OK".
 *
 * Checks that paths are valid and sets filters.
 */
void COpenView::OnOK() 
{
	int pathsType; // enum from paths::PATH_EXISTENCE in paths.h
	const String filterPrefix = _("[F] ");

	UpdateData(TRUE);
	TrimPaths();

	int index;
	int nFiles = 0;
	for (index = 0; index < countof(m_strPath); index++)
	{
		if (index == 2 && m_strPath[index].empty())
			break;
		m_files.SetSize(nFiles + 1);
		m_files[nFiles] = m_strPath[index];
		m_dwFlags[nFiles] &= ~FFILEOPEN_READONLY;
		m_dwFlags[nFiles] |= m_bReadOnly[index] ? FFILEOPEN_READONLY : 0;
		nFiles++;
	}
	// If left path is a project-file, load it
	String ext;
	paths::SplitFilename(m_strPath[0], nullptr, nullptr, &ext);
	if (m_strPath[1].empty() && strutils::compare_nocase(ext, ProjectFile::PROJECTFILE_EXT) == 0)
		LoadProjectFile(m_strPath[0]);

	pathsType = paths::GetPairComparability(m_files, IsArchiveFile);

	if (pathsType == paths::DOES_NOT_EXIST)
	{
		LangMessageBox(IDS_ERROR_INCOMPARABLE, MB_ICONSTOP);
		return;
	}

	for (index = 0; index < nFiles; index++)
	{
		// If user has edited path by hand, expand environment variables
		bool bExpand = false;
		if (strutils::compare_nocase(m_strBrowsePath[index], m_files[index]) != 0)
			bExpand = true;

		if (!paths::IsURLorCLSID(m_files[index]))
		{
			m_files[index] = paths::GetLongPath(m_files[index], bExpand);
	
			// Add trailing '\' for directories if its missing
			if (paths::DoesPathExist(m_files[index]) == paths::IS_EXISTING_DIR)
				m_files[index] = paths::AddTrailingSlash(m_files[index]);
			m_strPath[index] = m_files[index];
		}
	}

	UpdateData(FALSE);
	KillTimer(IDT_CHECKFILES);

	String filter(strutils::trim_ws(m_strExt));

	// If prefix found from start..
	if (filter.substr(0, filterPrefix.length()) == filterPrefix)
	{
		// Remove prefix + space
		filter.erase(0, filterPrefix.length());
		if (!theApp.m_pGlobalFileFilter->SetFilter(filter))
		{
			// If filtername is not found use default *.* mask
			theApp.m_pGlobalFileFilter->SetFilter(_T("*.*"));
			filter = _T("*.*");
		}
		GetOptionsMgr()->SaveOption(OPT_FILEFILTER_CURRENT, filter);
	}
	else
	{
		bool bFilterSet = theApp.m_pGlobalFileFilter->SetFilter(filter);
		if (!bFilterSet)
			m_strExt = theApp.m_pGlobalFileFilter->GetFilterNameOrMask();
		GetOptionsMgr()->SaveOption(OPT_FILEFILTER_CURRENT, filter);
	}

	SaveComboboxStates();
	GetOptionsMgr()->SaveOption(OPT_CMP_INCLUDE_SUBDIRS, m_bRecurse);
	LoadComboboxStates();

	m_constraint.Persist(true, false);

	COpenDoc *pDoc = GetDocument();
	pDoc->m_files = m_files;
	pDoc->m_bRecurse = m_bRecurse;
	pDoc->m_strExt = m_strExt;
	pDoc->m_strUnpacker = m_strUnpacker;
	pDoc->m_infoHandler = m_infoHandler;
	pDoc->m_dwFlags[0] = m_dwFlags[0];
	pDoc->m_dwFlags[1] = m_dwFlags[1];
	pDoc->m_dwFlags[2] = m_dwFlags[2];

	if (GetOptionsMgr()->GetBool(OPT_CLOSE_WITH_OK))
		GetParentFrame()->PostMessage(WM_CLOSE);

	PathContext tmpPathContext(pDoc->m_files);
	PackingInfo tmpPackingInfo(pDoc->m_infoHandler);
	GetMainFrame()->DoFileOpen(
		&tmpPathContext, std::array<DWORD, 3>(pDoc->m_dwFlags).data(), 
		nullptr, _T(""), pDoc->m_bRecurse, nullptr, _T(""), &tmpPackingInfo);
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
 * @brief Callled when Open-button for project file is selected.
 */
void COpenView::OnLoadProject()
{
	String fileName = AskProjectFileName(true);
	if (fileName.empty())
		return;

	ProjectFile project;
	if (!theApp.LoadProjectFile(fileName, project))
		return;

	PathContext paths;
	project.GetPaths(paths, m_bRecurse);
	project.GetLeftReadOnly();
	if (paths.size() < 3)
	{
		m_strPath[0] = paths[0];
		m_strPath[1] = paths[1];
		m_strPath[2] = _T("");
		m_bReadOnly[0] = project.GetLeftReadOnly();
		m_bReadOnly[1] = project.GetRightReadOnly();
		m_bReadOnly[2] = false;
	}
	else
	{
		m_strPath[0] = paths[0];
		m_strPath[1] = paths[1];
		m_strPath[2] = paths[2];
		m_bReadOnly[0] = project.GetLeftReadOnly();
		m_bReadOnly[1] = project.GetMiddleReadOnly();
		m_bReadOnly[2] = project.GetRightReadOnly();
	}
	m_strExt = project.GetFilter();

	UpdateData(FALSE);
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

	if (!m_strPath[0].empty())
		project.SetLeft(m_strPath[0], &m_bReadOnly[0]);
	if (m_strPath[2].empty())
	{
		if (!m_strPath[1].empty())
			project.SetRight(m_strPath[1], &m_bReadOnly[1]);
	}
	else
	{
		if (!m_strPath[1].empty())
			project.SetMiddle(m_strPath[1], &m_bReadOnly[1]);
		if (!m_strPath[2].empty())
			project.SetRight(m_strPath[2], &m_bReadOnly[2]);
	}
	if (!m_strExt.empty())
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
		project.SetFilter(strExt);
	}
	project.SetSubfolders(m_bRecurse);

	if (!theApp.SaveProjectFile(fileName, project))
		return;

	LangMessageBox(IDS_PROJFILE_SAVE_SUCCESS, MB_ICONINFORMATION);
}

void COpenView::OnDropDownSaveProject(NMHDR *pNMHDR, LRESULT *pResult)
{
	CRect rcButton, rcView;
	GetDlgItem(ID_SAVE_PROJECT)->GetWindowRect(&rcButton);
	BCMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_PROJECT));
	theApp.TranslateMenu(menu.m_hMenu);
	CMenu* pPopup = menu.GetSubMenu(0);
	if (pPopup != nullptr)
	{
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
			rcButton.left, rcButton.bottom, GetMainFrame());
	}
	*pResult = 0;
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
	CAssureScriptsForThread scriptsForRescan;

	while( (bRet = GetMessage( &msg, nullptr, 0, 0 )) != 0)
	{ 
		if (bRet == -1)
			break;
		if (msg.message != WM_USER + 2)
			continue;

		bool bIsaFolderCompare = true;
		bool bIsaFileCompare = true;
		bool bInvalid[3] = {false, false, false};
		int iStatusMsgId = 0;
		int iUnpackerStatusMsgId = 0;

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
			if (paths::DoesPathExist(paths[0], IsArchiveFile) == paths::DOES_NOT_EXIST)
				bInvalid[0] = true;
			if (paths::DoesPathExist(paths[1], IsArchiveFile) == paths::DOES_NOT_EXIST)
				bInvalid[1] = true;
			if (paths.GetSize() > 2 && paths::DoesPathExist(paths[2], IsArchiveFile) == paths::DOES_NOT_EXIST)
				bInvalid[2] = true;
		}

		// Enable buttons as appropriate
		if (GetOptionsMgr()->GetBool(OPT_VERIFY_OPEN_PATHS))
		{
			paths::PATH_EXISTENCE pathsType = paths::DOES_NOT_EXIST;

			if (paths.GetSize() <= 2)
			{
				if (bInvalid[0] && bInvalid[1])
					iStatusMsgId = IDS_OPEN_BOTHINVALID;
				else if (bInvalid[0])
					iStatusMsgId = IDS_OPEN_LEFTINVALID;
				else if (bInvalid[1])
					iStatusMsgId = IDS_OPEN_RIGHTINVALID2;
				else if (!bInvalid[0] && !bInvalid[1])
				{
					pathsType = paths::GetPairComparability(paths, IsArchiveFile);
					if (pathsType == paths::DOES_NOT_EXIST)
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
					pathsType = paths::GetPairComparability(paths, IsArchiveFile);
					if (pathsType == paths::DOES_NOT_EXIST)
						iStatusMsgId = IDS_OPEN_MISMATCH;
					else
						iStatusMsgId = IDS_OPEN_FILESDIRS;
				}
			}
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
	if (m_strUnpacker != m_infoHandler.m_PluginName)
		m_strUnpacker = theApp.LoadString(IDS_OPEN_UNPACKERDISABLED);
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
	if (m_strPath[2].empty())
		pParams->m_paths = PathContext(m_strPath[0], m_strPath[1]);
	else
		pParams->m_paths = PathContext(m_strPath[0], m_strPath[1], m_strPath[2]);

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
void COpenView::OnSelchangeCombo(int index) 
{
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

template <int N>
void COpenView::OnSelchangePathCombo() 
{
	OnSelchangeCombo(N);
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
template <int N>
void COpenView::OnEditEvent()
{
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
	if (nIDEvent == IDT_CHECKFILES)
		UpdateButtonStates();

	CFormView::OnTimer(nIDEvent);
}

/**
 * @brief Called when users selects plugin browse button.
 */
void COpenView::OnSelectUnpacker()
{
	paths::PATH_EXISTENCE pathsType;
	UpdateData(TRUE);

	int index;
	int nFiles = 0;
	for (index = 0; index < countof(m_strPath); index++)
	{
		if (index == 2 && m_strPath[index].empty())
			break;
		m_files.SetSize(nFiles + 1);
		m_files[nFiles] = m_strPath[index];
		nFiles++;
	}
	pathsType = paths::GetPairComparability(m_files);

	if (pathsType != paths::IS_EXISTING_FILE) 
		return;

	// let the user select a handler
	CSelectUnpackerDlg dlg(m_files[0], this);
	PackingInfo infoUnpacker(PLUGIN_AUTO);
	dlg.SetInitialInfoHandler(&infoUnpacker);

	if (dlg.DoModal() == IDOK)
	{
		m_infoHandler = dlg.GetInfoHandler();

		m_strUnpacker = m_infoHandler.m_PluginName;

		UpdateData(FALSE);
	}
}

LRESULT COpenView::OnUpdateStatus(WPARAM wParam, LPARAM lParam)
{
	bool bIsaFolderCompare = LOWORD(wParam) != 0;
	bool bIsaFileCompare = HIWORD(wParam) != 0;
	bool bProject = HIWORD(lParam) != 0;

	EnableDlgItem(IDOK, bIsaFolderCompare || bIsaFileCompare || bProject);

	EnableDlgItem(IDC_FILES_DIRS_GROUP4, bIsaFileCompare);
	EnableDlgItem(IDC_UNPACKER_EDIT, bIsaFileCompare);
	EnableDlgItem(IDC_SELECT_UNPACKER, bIsaFileCompare);

	
	EnableDlgItem(IDC_FILES_DIRS_GROUP3,  bIsaFolderCompare);
	EnableDlgItem(IDC_EXT_COMBO, bIsaFolderCompare);
	EnableDlgItem(IDC_SELECT_FILTER, bIsaFolderCompare);
	EnableDlgItem(IDC_RECURS_CHECK, bIsaFolderCompare);
	
	SetStatus(LOWORD(lParam));

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
 * @brief Set the plugin edit box text.
 * Plugin edit box is at the same time a plugin status view. This function
 * sets the status text.
 * @param [in] msgID Resource ID of status text to set.
 */
void COpenView::SetUnpackerStatus(UINT msgID)
{
	String msg = (msgID == 0 ? m_strUnpacker : theApp.LoadString(msgID));
	SetDlgItemText(IDC_UNPACKER_EDIT, msg);
}

/** 
 * @brief Called when "Select..." button for filters is selected.
 */
void COpenView::OnSelectFilter()
{
	String filterPrefix = _("[F] ");
	String curFilter;

	const bool bUseMask = theApp.m_pGlobalFileFilter->IsUsingMask();
	GetDlgItemText(IDC_EXT_COMBO, curFilter);
	curFilter = strutils::trim_ws(curFilter);

	GetMainFrame()->SelectFilter();
	
	String filterNameOrMask = theApp.m_pGlobalFileFilter->GetFilterNameOrMask();
	if (theApp.m_pGlobalFileFilter->IsUsingMask())
	{
		// If we had filter chosen and now has mask we can overwrite filter
		if (!bUseMask || curFilter[0] != '*')
		{
			SetDlgItemText(IDC_EXT_COMBO, filterNameOrMask);
		}
	}
	else
	{
		filterNameOrMask = filterPrefix + filterNameOrMask;
		SetDlgItemText(IDC_EXT_COMBO, filterNameOrMask);
	}
}

void COpenView::OnOptions()
{
	GetMainFrame()->PostMessage(WM_COMMAND, ID_OPTIONS);
}

void COpenView::OnDropDownOptions(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTOOLBAR dropDown = { 0 };
	dropDown.hdr.code = TBN_DROPDOWN;
	dropDown.hdr.hwndFrom = GetMainFrame()->GetDescendantWindow(AFX_IDW_TOOLBAR)->GetSafeHwnd();
	dropDown.hdr.idFrom = AFX_IDW_TOOLBAR;
	GetDlgItem(IDC_OPTIONS)->GetWindowRect(&dropDown.rcButton);
	GetMainFrame()->ScreenToClient(&dropDown.rcButton);
	GetMainFrame()->SendMessage(WM_NOTIFY, dropDown.hdr.idFrom, reinterpret_cast<LPARAM>(&dropDown));
	*pResult = 0;
}

/** 
 * @brief Read paths and filter from project file.
 * Reads the given project file. After the file is read, found paths and
 * filter is updated to dialog GUI. Other possible settings found in the
 * project file are kept in memory and used later when loading paths
 * selected.
 * @param [in] path Path to the project file.
 * @return `true` if the project file was successfully loaded, `false` otherwise.
 */
bool COpenView::LoadProjectFile(const String &path)
{
	String filterPrefix = _("[F] ");
	ProjectFile prj;

	if (!theApp.LoadProjectFile(path, prj))
		return false;

	bool recurse;
	prj.GetPaths(m_files, recurse);
	m_bRecurse = recurse;
	m_dwFlags[0] &= ~FFILEOPEN_READONLY;
	m_dwFlags[0] |= prj.GetLeftReadOnly() ?	FFILEOPEN_READONLY : 0;
	if (m_files.GetSize() < 3)
	{
		m_dwFlags[1] &= ~FFILEOPEN_READONLY;
		m_dwFlags[1] |= prj.GetRightReadOnly() ? FFILEOPEN_READONLY : 0;
	}
	else
	{
		m_dwFlags[1] &= ~FFILEOPEN_READONLY;
		m_dwFlags[1] |= prj.GetMiddleReadOnly() ? FFILEOPEN_READONLY : 0;
		m_dwFlags[2] &= ~FFILEOPEN_READONLY;
		m_dwFlags[2] |= prj.GetRightReadOnly() ? FFILEOPEN_READONLY : 0;
	}
	if (prj.HasFilter())
	{
		m_strExt = strutils::trim_ws(prj.GetFilter());
		if (m_strExt[0] != '*')
			m_strExt.insert(0, filterPrefix);
	}
	return true;
}

/** 
 * @brief Removes whitespaces from left and right paths
 * @note Assumes UpdateData(TRUE) is called before this function.
 */
void COpenView::TrimPaths()
{
	for (int index = 0; index < countof(m_strPath); index++)
		m_strPath[index] = strutils::trim_ws(m_strPath[index]);
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
	CFormView::OnActivate(nState, pWndOther, bMinimized);

	if (nState == WA_ACTIVE || nState == WA_CLICKACTIVE)
		UpdateButtonStates();
}

template <int MSG, int WPARAM, int LPARAM>
void COpenView::OnEditAction()
{
	CWnd *pCtl = GetFocus();
	if (pCtl != nullptr)
		pCtl->PostMessage(MSG, WPARAM, LPARAM);
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
