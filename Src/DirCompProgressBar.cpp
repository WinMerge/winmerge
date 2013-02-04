/** 
 * @file  DirCompProgressBar.cpp
 *
 * @brief Implementation file for Directory compare state dialog
 */
// ID line follows -- this is updated by SVN
// $Id: DirCompProgressBar.cpp 4596 2007-10-07 09:44:06Z jtuc $

#include "stdafx.h"
#include "DirCompProgressBar.h"
#include "Merge.h"
#include "DirDoc.h"
#include "CompareStats.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** @brief ID for timer updating UI. */
static const UINT IDT_UPDATE = 1;

/** @brief Interval (in milliseconds) for UI updates. */
static const UINT UPDATE_INTERVAL = 400;

/** @brief Reset all UI fields to zero. */
void DirCompProgressBar::ClearStat()
{
	CProgressCtrl *pProg = (CProgressCtrl*) GetDlgItem(IDC_PROGRESSCOMPARE);
	pProg->SetPos(0);

	CStatic *pCompared = (CStatic *) GetDlgItem(IDC_ITEMSCOMPARED);
	CStatic *pTotal = (CStatic *) GetDlgItem(IDC_ITEMSTOTAL);
	pCompared->SetWindowText(_T("0"));
	pTotal->SetWindowText(_T("0"));

	m_prevState = CompareStats::STATE_IDLE;
}

/**
 * @brief Constructor.
 * @param [in] pParent Parent window for progress dialog.
 */
DirCompProgressBar::DirCompProgressBar()
: m_bCompareReady(FALSE)
, m_prevState(CompareStats::STATE_IDLE)
, m_pCompareStats(NULL)
#if _MSC_VER >= 1600
, m_pTaskbarList(NULL)
#endif
{
}

DirCompProgressBar::~DirCompProgressBar()
{
#if _MSC_VER >= 1600
	if (m_pTaskbarList)
	{
		m_pTaskbarList->SetProgressValue(theApp.GetMainWnd()->m_hWnd, 0, 0);
		m_pTaskbarList->Release();
	}
#endif
}

BEGIN_MESSAGE_MAP(DirCompProgressBar, CDialogBar)
	//{{AFX_MSG_MAP(DirCompProgressBar)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_TIMER()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

/**
 * @brief Initialize the dialog.
 * Center the dialog to main window.
 * @return TRUE (see the comment inside function).
 */
BOOL DirCompProgressBar::Create(CWnd* pParentWnd)
{
	if (! CDialogBar::Create(pParentWnd, DirCompProgressBar::IDD, 
			CBRS_BOTTOM | CBRS_TOOLTIPS | CBRS_FLYBY, DirCompProgressBar::IDD))
		return FALSE; 

#if _MSC_VER >= 1600
	CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL, IID_ITaskbarList3, (void**)&m_pTaskbarList);
#endif
	theApp.TranslateDialog(m_hWnd);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Set pointer to compare stats.
 * @param [in] pCompareStats Pointer to stats.
 */
void DirCompProgressBar::SetCompareStat(CompareStats * pCompareStats)
{
	m_pCompareStats = pCompareStats;
}

void DirCompProgressBar::SetProgressState(int comparedItems, int totalItems)
{
	CProgressCtrl *pProg = (CProgressCtrl*) GetDlgItem(IDC_PROGRESSCOMPARE);
	CStatic *pCompared = (CStatic *) GetDlgItem(IDC_ITEMSCOMPARED);
	CStatic *pTotal = (CStatic *) GetDlgItem(IDC_ITEMSTOTAL);

	TCHAR num[15] = {0};
	_itot(totalItems, num, 10);
	pTotal->SetWindowText(num);

	pProg->SetRange32(0, totalItems);
	_itot(comparedItems, num, 10);
	pCompared->SetWindowText(num);
	pProg->SetPos(comparedItems);

#if _MSC_VER >= 1600
	if (m_pTaskbarList)
		m_pTaskbarList->SetProgressValue(theApp.GetMainWnd()->m_hWnd, comparedItems, totalItems);
#endif
}

/**
 * @brief Timer message received.
 * Handle timer messages. When timer fires, update the dialog.
 * @param [in] nIDEvent ID of the timer that fired.
 */
void DirCompProgressBar::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == IDT_UPDATE)
	{
		const CompareStats::CMP_STATE state = m_pCompareStats->GetCompareState();
		
		// New compare started
		if (m_prevState == CompareStats::STATE_IDLE &&
			state == CompareStats::STATE_START)
		{
			m_prevState = CompareStats::STATE_START;
		}
		// Collecting items to compare
		else if (m_prevState == CompareStats::STATE_START &&
			state == CompareStats::STATE_START)
		{
			SetProgressState(0, m_pCompareStats->GetTotalItems());
		}
		// Started comparing items
		else if ((m_prevState == CompareStats::STATE_START ||
				m_prevState == CompareStats::STATE_IDLE) &&
				state == CompareStats::STATE_COMPARE)
		{
			// Start comparing, init progressDlg
			SetProgressState(m_pCompareStats->GetComparedItems(), m_pCompareStats->GetTotalItems());
			m_prevState = CompareStats::STATE_COMPARE;
		}
		// Comparing items
		else if (m_prevState == CompareStats::STATE_COMPARE &&
				state == CompareStats::STATE_COMPARE)
		{
			SetProgressState(m_pCompareStats->GetComparedItems(), m_pCompareStats->GetTotalItems());
		}
		// Compare is ready
		// Update total items too since we might get only this one state
		// when compare is fast.
		else if (state == CompareStats::STATE_IDLE &&
			m_bCompareReady == FALSE && m_pCompareStats->IsCompareDone() )
		{
			SetProgressState(m_pCompareStats->GetComparedItems(), m_pCompareStats->GetTotalItems());
			EndUpdating();
			m_prevState = CompareStats::STATE_COMPARE;
			m_bCompareReady = TRUE;
		}
	}
	else
		CDialogBar::OnTimer(nIDEvent);
}

void DirCompProgressBar::OnSize(UINT nType, int cx, int cy)
{
	CDialogBar::OnSize(nType, cx, cy);

	CWnd *pwndButton = GetDlgItem(IDC_COMPARISON_STOP);
	CWnd *pwndProgress = GetDlgItem(IDC_PROGRESSCOMPARE);

	if (pwndButton && pwndProgress)
	{
		CRect rectButton, rectProgress;
		pwndButton->GetWindowRect(&rectButton);
		pwndProgress->GetWindowRect(&rectProgress);
		ScreenToClient(&rectButton);
		ScreenToClient(&rectProgress);
		rectButton.left = cx - rectButton.Width() - rectProgress.left;
		rectProgress.right = cx - rectProgress.left;
		pwndButton->SetWindowPos(NULL, rectButton.left, rectButton.top, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
		pwndProgress->SetWindowPos(NULL, 0, 0, rectProgress.Width(), rectProgress.Height(), SWP_NOZORDER|SWP_NOMOVE);
	}
}

/**
 * @brief Start timer for UI updating.
 */
void DirCompProgressBar::StartUpdating()
{
	ClearStat();
	SetTimer(IDT_UPDATE, UPDATE_INTERVAL, NULL);
}

/**
 * @brief Stop timer updating UI.
 */
void DirCompProgressBar::EndUpdating()
{
	KillTimer(IDT_UPDATE);
}

