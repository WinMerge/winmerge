/** 
 * @file PropertyPageHost.cpp
 *
 * @brief Implementation file for CPropertypageHost
 *
 * @note This code originates from AbstractSpoon / TodoList
 * (http://www.abstractspoon.com/) but is modified to use in
 * WinMerge.
 */

#include "stdafx.h"
#include "PropertyPageHost.h"

#if _MSC_VER > 1200
#ifdef _WIN64
#	include <../src/mfc/occimpl.h>
#else // _WIN64
#	include <afxocc.h>
#endif // _WIN64
#else
#	include <..\src\occimpl.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageHost

CPropertyPageHost::CPropertyPageHost() : m_nSelIndex(-1)
{
}

CPropertyPageHost::~CPropertyPageHost()
{
}


BEGIN_MESSAGE_MAP(CPropertyPageHost, CWnd)
	//{{AFX_MSG_MAP(CPropertyPageHost)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(PSM_GETCURRENTPAGEHWND, OnGetCurrentPageHwnd)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertyPageHost message handlers

BOOL CPropertyPageHost::Create(UINT nRefCtrlID, CWnd* pParent, UINT uCtrlID)
{
	ASSERT_VALID(pParent);

	if (!pParent || !::IsWindow(*pParent))
		return FALSE;

	CWnd* pRefCtrl = pParent->GetDlgItem(nRefCtrlID);

	ASSERT_VALID(pRefCtrl);

	if (!pRefCtrl || !::IsWindow(*pRefCtrl))
		return FALSE;

	CRect rHost;
	pRefCtrl->GetWindowRect(rHost);
	pParent->ScreenToClient(rHost);

	if (Create(rHost, pParent, uCtrlID))
	{
		// move us after nRefCtrlID in the z-order
		SetWindowPos(pRefCtrl, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		return TRUE;
	}

	return FALSE;
}

BOOL CPropertyPageHost::Create(LPRECT lpRect, CWnd* pParent, UINT uCtrlID)
{
	ASSERT_VALID(pParent);

	if (!pParent || !::IsWindow(*pParent))
		return FALSE;

	if (!m_aPages.GetSize())
		return FALSE;

	if (!CWnd::Create(NULL, NULL, WS_CHILD | WS_VISIBLE, *lpRect, pParent, uCtrlID))
		return FALSE;

	// create the first page
	return SetActivePage(0);
}

int CPropertyPageHost::GetActiveIndex()
{
	return m_nSelIndex;
}

CPropertyPage* CPropertyPageHost::GetActivePage()
{
	if (m_nSelIndex < 0)
		return NULL;

	CPropertyPage* pPage = m_aPages[m_nSelIndex].pPage;

	if (!pPage || !pPage->GetSafeHwnd())
		return NULL;

	return pPage;
}

LRESULT CPropertyPageHost::OnGetCurrentPageHwnd(WPARAM wParam, LPARAM lParam)
{
	CWnd* pActive = GetActivePage();

	return pActive ? (LRESULT)pActive->GetSafeHwnd() : NULL;
}

BOOL CPropertyPageHost::SetActivePage(int nIndex, BOOL bAndFocus)
{
	if (nIndex < 0 || nIndex >= m_aPages.GetSize())
		return FALSE;

	CPropertyPage* pPage = m_aPages[nIndex].pPage;

	if (!pPage)
		return FALSE;

	CWnd* pFocus = GetFocus();

	if (!pPage->GetSafeHwnd() && !pPage->Create(pPage->m_psp.pszTemplate, this))
		return FALSE;

	// make sure we can into/out of the page
	pPage->ModifyStyleEx(0, WS_EX_CONTROLPARENT | DS_CONTROL);

	// make sure the page is a child and modify it if necessary
	pPage->ModifyStyle(WS_POPUPWINDOW | WS_OVERLAPPEDWINDOW, 0);

	if (!(pPage->GetStyle() & WS_CHILD))
	{
		pPage->ModifyStyle(0, WS_CHILD);
		pPage->SetParent(this);
		ASSERT (pPage->GetParent() == this);
	}


	CRect rClient;
	GetClientRect(rClient);

	pPage->SetParent(this);
	pPage->MoveWindow(rClient);

	if (m_nSelIndex != -1)
	{
		CPropertyPage* pPage = GetActivePage();
		ASSERT (pPage);

		if (pPage)
		{
			pPage->ShowWindow(SW_HIDE);
			pPage->OnKillActive();
		}
	}

	pPage->OnSetActive();
	pPage->ShowWindow(SW_SHOW);

	// move the focus to the first dlg ctrl
	if (bAndFocus)
	{
		CWnd* pCtrl = pPage->GetNextDlgTabItem(NULL);

		if (pCtrl)
			pCtrl->SetFocus();
	}
	else
		pFocus->SetFocus();

	m_nSelIndex = nIndex;
	return TRUE;
}

BOOL CPropertyPageHost::SetActivePage(CPropertyPage* pPage, BOOL bAndFocus)
{
	return SetActivePage(FindPage(pPage));
}

BOOL CPropertyPageHost::AddPage(CPropertyPage* pPage, LPCTSTR szTitle, DWORD dwItemData)
{
	if (!pPage || !pPage->IsKindOf(RUNTIME_CLASS(CPropertyPage)))
		return FALSE;

	if (FindPage(pPage) == -1)
	{
		PAGEITEM pi(pPage, szTitle, dwItemData);
		int nPage = m_aPages.Add(pi);

		if (szTitle)
		{
			pPage->m_psp.pszTitle = m_aPages[nPage].sTitle; // the final string address
			pPage->m_psp.dwFlags |= PSP_USETITLE;
		}
	}

	return TRUE;
}

int CPropertyPageHost::FindPage(CPropertyPage* pPage)
{
	int nPage = m_aPages.GetSize();

	while (nPage--)
	{
		if (m_aPages[nPage].pPage == pPage)
			break;
	}

	return nPage;
}

BOOL CPropertyPageHost::PreTranslateMessage(MSG* pMsg) 
{
	CWnd* pActive = GetActivePage();

	if (pActive)
	{
		if (pActive->PreTranslateMessage(pMsg))
			return TRUE;
		
		else if (pActive->IsDialogMessage(pMsg)) 
			return TRUE;
	}
	
	return FALSE;
}

// unfortunately there is a bug in MFC that can cause an infinite loop
// when the property page is hosting an OLE control so we fix
// it by providing a replacement to the buggy code
BOOL COccManager::IsDialogMessage(CWnd* pWndDlg, LPMSG lpMsg)
{
	CWnd* pWndFocus = CWnd::GetFocus();
	HWND hWndFocus = pWndFocus->GetSafeHwnd();
	HWND hWndDlg = pWndDlg->GetSafeHwnd();
	UINT uMsg = lpMsg->message;

	if (((uMsg >= WM_KEYFIRST) && (uMsg <= WM_KEYLAST)) ||
		((uMsg >= WM_MOUSEFIRST) && (uMsg <= WM_MOUSELAST)))
	{
		CWnd* pWndCtrl = pWndFocus;

		// Walk up the parent chain, until we find an OLE control.
		while ((pWndCtrl != NULL) && (pWndCtrl->m_pCtrlSite == NULL) &&
			(pWndCtrl->GetParent() != pWndDlg))
		{
			pWndCtrl = pWndCtrl->GetParent();
		}

		// let the control attempt to translate the message
		if (pWndCtrl != NULL && pWndCtrl->m_pCtrlSite != NULL &&
			pWndCtrl->m_pCtrlSite->m_pActiveObject != NULL &&
			pWndCtrl->m_pCtrlSite->m_pActiveObject->TranslateAccelerator(lpMsg) == S_OK)
		{
			return TRUE;
		}

		// handle CTRLINFO_EATS_RETURN and CTRLINFO_EATS_ESCAPE flags
		if ((uMsg == WM_KEYUP || uMsg == WM_KEYDOWN || uMsg == WM_CHAR) &&
			pWndCtrl != NULL && pWndCtrl->m_pCtrlSite != NULL &&
			((LOWORD(lpMsg->wParam) == VK_RETURN &&
			 (pWndCtrl->m_pCtrlSite->m_ctlInfo.dwFlags & CTRLINFO_EATS_RETURN)) ||
			(LOWORD(lpMsg->wParam) == VK_ESCAPE &&
			 (pWndCtrl->m_pCtrlSite->m_ctlInfo.dwFlags & CTRLINFO_EATS_ESCAPE))))
		{
			return FALSE;
		}
	}

	// else simple default
	// because this is where the MFC bug existed
	return ::IsDialogMessage(pWndDlg->GetSafeHwnd(), lpMsg);
}

void CPropertyPageHost::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	// resize active page
	if (m_nSelIndex != -1)
	{
		CPropertyPage* pPage = GetActivePage();
		ASSERT (pPage);

		if (pPage)
			pPage->MoveWindow(0, 0, cx, cy, TRUE);
	}
}

BOOL CPropertyPageHost::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CPropertyPageHost::OnOK()
{
	int nPage = m_aPages.GetSize();

	while (nPage--)
	{
		CPropertyPage* pPage = m_aPages[nPage].pPage;

		if (pPage && pPage->GetSafeHwnd())
		{
			pPage->UpdateData();
			pPage->OnOK();
		}
	}
}

int CPropertyPageHost::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// *************************************************************************
	// EXTREMELY IMPORTANT
	// set these styles because otherwise propsheet
	// goes into infinite loop when focus changes
	ModifyStyle(0, DS_CONTROL, 0);
	ModifyStyleEx(0, WS_EX_CONTROLPARENT);
	// *************************************************************************
	
	return 0;
}

CString CPropertyPageHost::GetPageTitle(int nIndex)
{
	if (nIndex < 0 || nIndex > m_aPages.GetSize())
		return "";

	return m_aPages[nIndex].sTitle;
}

DWORD CPropertyPageHost::GetPageItemData(int nIndex)
{
	if (nIndex < 0 || nIndex > m_aPages.GetSize())
		return 0;

	return m_aPages[nIndex].dwItemData;
}

CPropertyPage* CPropertyPageHost::GetPage(int nIndex)
{
	if (nIndex < 0 || nIndex > m_aPages.GetSize())
		return NULL;

	return m_aPages[nIndex].pPage;
}

CPropertyPage* CPropertyPageHost::FindPage(DWORD dwItemData)
{
	int nPage = m_aPages.GetSize();

	while (nPage--)
	{
		if (dwItemData && dwItemData == m_aPages[nPage].dwItemData)
			return m_aPages[nPage].pPage;
	}

	return NULL;
}
