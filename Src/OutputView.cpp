// OutputView.cpp : implementation of the COutputView class
//

#include "stdafx.h"
#include "OutputView.h"
#include "OutputDoc.h"
#include "MainFrm.h"
#include "Merge.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "SyntaxColors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputView

IMPLEMENT_DYNCREATE(COutputView, CCrystalTextView)

BEGIN_MESSAGE_MAP(COutputView, CCrystalTextView)
	//{{AFX_MSG_MAP(COutputView)
	ON_WM_CONTEXTMENU()
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_EDIT_CLEAR_ALL, OnClearAll)
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputView construction/destruction

COutputView::COutputView()
{
	// TODO: add construction code here
	SetParser(&m_xParser);
}

COutputView::~COutputView()
{
}

BOOL COutputView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CCrystalTextView::PreCreateWindow(cs);
}

BOOL COutputView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		if (::TranslateAccelerator (m_hWnd, static_cast<CFrameWnd *>(AfxGetMainWnd())->GetDefaultAccelerator(), pMsg))
			return TRUE;
	}
	return __super::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// COutputView diagnostics

#ifdef _DEBUG
void COutputView::AssertValid() const
{
	CCrystalTextView::AssertValid();
}

void COutputView::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}

COutputDoc* COutputView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COutputDoc)));
	return static_cast<COutputDoc*>(m_pDocument);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COutputView message handlers

CCrystalTextBuffer* COutputView::LocateTextBuffer()
{
	return &GetDocument()->m_xTextBuffer;
}

void COutputView::OnInitialUpdate()
{
	__super::OnInitialUpdate();
	SetRevisionMarkWidth(0);
	SetSelectionMargin(false);
	SetFont(theApp.m_lfDiff);
	AttachToBuffer();
	SetColorContext(theApp.GetMainSyntaxColors());
	SetMarkersContext(GetDocument()->m_pMarkers.get());
	if (HWND hSelf = GetSafeHwnd())
		DarkMode::setDarkScrollBar(hSelf);
}

/**
 * @brief In dark mode the output pane uses the system window colors (dark through the color scheme's
 * system color hook) like the rest of the frame, even when the scheme keeps a light text area.
 */
CEColor COutputView::GetColor(int nColorIndex) const
{
	if (GetOptionsMgr()->GetInt(OPT_COLOR_MODE_EFFECTIVE) == 1)
	{
		switch (nColorIndex & ~COLORINDEX_MASK)
		{
		case COLORINDEX_WHITESPACE:
		case COLORINDEX_BKGND:
			return GetSysColor(COLOR_WINDOW);
		case COLORINDEX_NORMALTEXT:
			return GetSysColor(COLOR_WINDOWTEXT);
		case COLORINDEX_SELMARGIN:
			return GetSysColor(COLOR_BTNFACE);
		}
	}
	return __super::GetColor(nColorIndex);
}

void COutputView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_OUTPUTVIEW));
	I18n::TranslateMenu(menu.m_hMenu);

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != nullptr);

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

void COutputView::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	if (WinMergeDarkMode::IsImmersiveColorSet(lpszSection))
		DarkMode::setDarkScrollBar(GetSafeHwnd());
	__super::OnSettingChange(uFlags, lpszSection);
}

void COutputView::OnClearAll()
{
	GetDocument()->ClearAll();
}

