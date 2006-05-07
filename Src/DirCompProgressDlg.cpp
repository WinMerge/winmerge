/** 
 * @file  DirCompProgressDlg.cpp
 *
 * @brief Implementation file for Directory compare state dialog
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "merge.h"
#include "DirCompProgressDlg.h"
#include "DiffContext.h"
#include "DirDoc.h"
#include "DirFrame.h"
#include "CompareStats.h"

#ifdef COMPILE_MULTIMON_STUBS
#undef COMPILE_MULTIMON_STUBS
#endif
#include <multimon.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 * @brief ID for timer updating UI.
 */
static const UINT IDT_UPDATE = 1;

/**
 * @brief Interval (in milliseconds) for UI updates.
 */
static const UINT UPDATE_INTERVAL = 400;

/**
 * @brief Reset all UI fields to zero.
 */
void DirCompProgressDlg::ClearStat()
{
	CProgressCtrl *pProg = (CProgressCtrl*) GetDlgItem(IDC_PROGRESSCOMPARE);
	pProg->SetPos(0);

	CStatic *pCompared = (CStatic *) GetDlgItem(IDC_ITEMSCOMPARED);
	CStatic *pTotal = (CStatic *) GetDlgItem(IDC_ITEMSTOTAL);
	pCompared->SetWindowText(_T("0"));
	pTotal->SetWindowText(_T("0"));

	m_prevState = CompareStats::STATE_IDLE;
}

IMPLEMENT_DYNAMIC(DirCompProgressDlg, CDialog)

/**
 * @brief Constructor.
 */
DirCompProgressDlg::DirCompProgressDlg(CWnd* pParent /*=NULL*/)
: m_bCompareReady(FALSE)
, m_prevState(CompareStats::STATE_IDLE)
, m_pDirDoc(NULL)
{
}

/**
 * @brief Loads strings from resource.
 */
BOOL DirCompProgressDlg::Create(UINT nIDTemplate, CWnd* pParentWnd)
{
	return CDialog::Create(nIDTemplate, pParentWnd);
};

void DirCompProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DirCompProgressDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DirCompProgressDlg, CDialog)
	//{{AFX_MSG_MAP(DirCompProgressDlg)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_COMPARISON_STOP, OnBnClickedComparisonStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL DirCompProgressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Handle WM_KEYDOWN messages before normal processing
 * to allow any key close stateDlg.
 */
BOOL DirCompProgressDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{		
		// Allow ESC to stop compare
		if (pMsg->wParam == VK_ESCAPE)
		{
			OnBnClickedComparisonStop();
			return TRUE;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

/**
 * @brief Set pointer to compare stats.
 */
void DirCompProgressDlg::SetCompareStat(CompareStats * pCompareStats)
{
	m_pCompareStats = pCompareStats;
}

/**
 * @brief Timer message received.
 */
void DirCompProgressDlg::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == IDT_UPDATE)
	{
		CProgressCtrl *pProg = (CProgressCtrl*) GetDlgItem(IDC_PROGRESSCOMPARE);
		CStatic *pCompared = (CStatic *) GetDlgItem(IDC_ITEMSCOMPARED);
		CStatic *pTotal = (CStatic *) GetDlgItem(IDC_ITEMSTOTAL);
		const CompareStats::CMP_STATE state = m_pCompareStats->GetCompareState();
		
		// New compare started
		if (m_prevState == CompareStats::STATE_IDLE &&
			state == CompareStats::STATE_COLLECT)
		{
			m_prevState = CompareStats::STATE_COLLECT;
		}
		// Collecting items to compare
		else if (m_prevState == CompareStats::STATE_COLLECT &&
			state == CompareStats::STATE_COLLECT)
		{
			TCHAR num[15] = {0};
			_itot(m_pCompareStats->GetTotalItems(), num, 10);
			pTotal->SetWindowText(num);
		}
		// Started comparing items
		else if ((m_prevState == CompareStats::STATE_COLLECT ||
				m_prevState == CompareStats::STATE_IDLE) &&
				state == CompareStats::STATE_COMPARE)
		{
			TCHAR num[15] = {0};

			// Start comparing, init progressDlg
			int totalItems = m_pCompareStats->GetTotalItems();
			_itot(totalItems, num, 10);
			pTotal->SetWindowText(num);

			pProg->SetRange32(0, totalItems);
			int comparedItems = m_pCompareStats->GetComparedItems();
			_itot(comparedItems, num, 10);
			pCompared->SetWindowText(num);
			pProg->SetPos(comparedItems);
			m_prevState = CompareStats::STATE_COMPARE;
		}
		// Comparing items
		else if (m_prevState == CompareStats::STATE_COMPARE &&
				state == CompareStats::STATE_COMPARE)
		{
			TCHAR num[15] = {0};
			int comparedItems = m_pCompareStats->GetComparedItems();
			_itot(comparedItems, num, 10);
			pCompared->SetWindowText(num);
			pProg->SetPos(comparedItems);
		}
		// Compare is ready
		// Update total items too since we might get only this one state
		// when compare is fast.
		else if (state == CompareStats::STATE_IDLE &&
			m_bCompareReady == FALSE && m_pCompareStats->IsCompareDone() )
		{
			TCHAR num[15] = {0};
			const int totalItems = m_pCompareStats->GetTotalItems();
			const int comparedItems = m_pCompareStats->GetComparedItems();
			_itot(comparedItems, num, 10);
			pCompared->SetWindowText(num);
			_itot(totalItems, num, 10);
			pTotal->SetWindowText(num);
			pProg->SetRange32(0, totalItems);
			pProg->SetPos(comparedItems);
			EndUpdating();
			m_prevState = CompareStats::STATE_COMPARE;
			m_bCompareReady = TRUE;
		}
	}
	else
		CDialog::OnTimer(nIDEvent);
}

/**
 * @brief Start timer for UI updating.
 */
void DirCompProgressDlg::StartUpdating()
{
	ClearStat();
	SetTimer(IDT_UPDATE, UPDATE_INTERVAL, NULL);
}

/**
 * @brief Stop timer updating UI.
 */
void DirCompProgressDlg::EndUpdating()
{
	KillTimer(IDT_UPDATE);
}

/**
 * @brief User selects to stop compare.
 *
 * Tells document to abort current compare.
 * @note We don't close this dialog in this function. Instead we tell
 * compare thread to abort. Which in turn causes message sent to CDirView
 * about compare ready/abort. And CDirView calls CloseDialog().
 * Yes, it could be simpler, but this assures we don't close dialog while
 * thread is still comparing items.
 */
void DirCompProgressDlg::OnBnClickedComparisonStop()
{
	ASSERT(m_pDirDoc); // Must be set using SetDirDoc()
	EndUpdating();
	m_pDirDoc->AbortCurrentScan();
}

/** 
 * @brief Closes the dialog.
 */
void DirCompProgressDlg::CloseDialog()
{
	EndUpdating();
	DestroyWindow();
}

/** 
 * @brief Set pointer to DirDoc.
 *
 * We need pointer to DirDoc for aborting the scan when Stop
 * button is clicked.
 */
void DirCompProgressDlg::SetDirDoc(CDirDoc *pDirDoc)
{
	m_pDirDoc = pDirDoc;
}
