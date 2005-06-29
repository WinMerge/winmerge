/** 
 * @file  DirCompStateBar.cpp
 *
 * @brief Implementation file for Directory compare statuspanel class
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "merge.h"
#include "DirCompStateBar.h"
#include "DiffContext.h"
#include "DirDoc.h"
#include "DirFrame.h"

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
static const UINT UPDATE_INTERVAL = 500;

/**
 * @brief Reset all UI fields to zero.
 */
void CDirCompStateBar::ClearStat()
{
	SetDlgItemInt(IDC_COUNT_LFILE, 0);
	SetDlgItemInt(IDC_COUNT_RFILE, 0);
	SetDlgItemInt(IDC_COUNT_NOTEQUAL, 0);
	SetDlgItemInt(IDC_COUNT_EQUAL, 0);
	SetDlgItemInt(IDC_COUNT_BINARYSAME, 0);
	SetDlgItemInt(IDC_COUNT_BINARYDIFF, 0);
	SetDlgItemInt(IDC_COUNT_LFOLDER, 0);
	SetDlgItemInt(IDC_COUNT_RFOLDER, 0);
	SetDlgItemInt(IDC_COUNT_FILESKIP, 0);
	SetDlgItemInt(IDC_COUNT_FOLDERSKIP, 0);
	SetDlgItemInt(IDC_COUNT_FOLDER, 0);
	SetDlgItemInt(IDC_COUNT_ERROR, 0);

	CProgressCtrl *pProg = (CProgressCtrl*) GetDlgItem(IDC_PROGRESSCOMPARE);
	pProg->SetPos(0);

	CStatic *pCompared = (CStatic *) GetDlgItem(IDC_ITEMSCOMPARED);
	CStatic *pTotal = (CStatic *) GetDlgItem(IDC_ITEMSTOTAL);
	pCompared->SetWindowText(_T("0"));
	pTotal->SetWindowText(_T("0"));

	m_bTimerFired = FALSE;
	m_prevState = CompareStats::STATE_IDLE;
}

/**
 * @brief Constructor.
 */
CDirCompStateBar::CDirCompStateBar(CWnd* pParent /*=NULL*/)
: m_bStopText(TRUE)
, m_bTimerFired(FALSE)
, m_prevState(CompareStats::STATE_IDLE)
{
}

/**
 * @brief Loads strings from resource.
 */
BOOL CDirCompStateBar::Create(CWnd* pParentWnd)
{
	VERIFY(strAbort.LoadString(IDC_COMPARISON_STOP));
	VERIFY(strClose.LoadString(IDC_COMPARISON_CLOSE));

	if (! CDialogBar::Create(pParentWnd, CDirCompStateBar::IDD, 
			WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_FLYBY, 
			CDirCompStateBar::IDD))
		return FALSE;

	ClearStat();
	return TRUE;
};

void CDirCompStateBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDirCompStateBar)
	DDX_Control(pDX, IDC_COMPARISON_STOP, m_ctlStop);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDirCompStateBar, CDialogBar)
	//{{AFX_MSG_MAP(CDirCompStateBar)
	ON_COMMAND(IDC_COMPARISON_STOP, OnStop)
	ON_UPDATE_COMMAND_UI(IDC_COMPARISON_STOP, OnUpdateStop)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirCompStateBar size
// m_sizeDefault is the size of the dialog

CSize CDirCompStateBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	if (IsFloating())
		return m_sizeDefault;
	else
		return CDialogBar::CalcFixedLayout(bStretch, bHorz);
}

BOOL CDirCompStateBar::GetDefaultRect( LPRECT lpRect ) const
{
	lpRect->top = 0;
	lpRect->bottom = m_sizeDefault.cy;
	lpRect->left = 0;
	lpRect->right = m_sizeDefault.cx;
	return TRUE;
}

/**
 * @brief User selects to stop compare.
 *
 * Aborts compare and hides status panel.
 * @todo Couldn't we close this bar without using frame code?
 */
void CDirCompStateBar::OnStop()
{
	// use GetOwner and not GetParentFrame
	// because FloatControlBar assigns the bar to a new parent frame
	CFrameWnd * pFrameWnd = static_cast<CFrameWnd*> (GetOwner());
	CDirDoc * pDirDoc = dynamic_cast<CDirDoc*>(pFrameWnd->GetActiveDocument());
	ASSERT(pDirDoc != NULL);

	if (pDirDoc->IsCurrentScanAbortable())
		pDirDoc->AbortCurrentScan();

	m_pCompareStats->SetCompareState(CompareStats::STATE_IDLE);

	CDirFrame * pDirFrame = static_cast<CDirFrame*>(pFrameWnd);
	ASSERT(pDirFrame != NULL);
	pDirFrame->ShowProcessingBar(FALSE);
}

/**
 * @brief Update button text (Stop/Close).
 *
 * Changes button text after compare is ready.
 */
void CDirCompStateBar::OnUpdateStop(CCmdUI* pCmdUI)
{
	// use GetOwner and not GetParentFrame
	// because FloatControlBar assigns the bar to a new parent frame
	CFrameWnd * pFrameWnd = static_cast<CFrameWnd*> (GetOwner());
	CDirDoc * pDirDoc = dynamic_cast<CDirDoc*>(pFrameWnd->GetActiveDocument());
	ASSERT(pDirDoc != NULL);

	// Change button text if scan is finished and it hasn't been yet changed. 
	if (m_ctlStop.GetSafeHwnd())
	{
		if (!pDirDoc->IsCurrentScanAbortable())
		{
			if (m_bStopText)
			{
				m_ctlStop.SetWindowText(strClose);
				m_bStopText = FALSE;
				m_lElapsed += ::GetTickCount();
				CString text;
				text.Format(IDS_ELAPSED_TIME, m_lElapsed);
				pFrameWnd->SetMessageText(text);
			}
		}
	}
	pCmdUI->Enable(TRUE);
}
 
/**
 * @brief Return control ID for result code.
 */
UINT CDirCompStateBar::GetIDFromResult(CompareStats::RESULT res)
{
	UINT resID = 0;

	switch ((int)res)
	{
	case CompareStats::RESULT_LUNIQUE:
		resID = IDC_COUNT_LFILE;
		break;
	case CompareStats::RESULT_RUNIQUE:
		resID = IDC_COUNT_RFILE;
		break;
	case CompareStats::RESULT_DIFF:
		resID = IDC_COUNT_NOTEQUAL;
		break;
	case CompareStats::RESULT_SAME:
		resID = IDC_COUNT_EQUAL;
		break;
	case CompareStats::RESULT_BINSAME:
		resID = IDC_COUNT_BINARYSAME;
		break;
	case CompareStats::RESULT_BINDIFF:
		resID = IDC_COUNT_BINARYDIFF;
		break;
	case CompareStats::RESULT_LDIRUNIQUE:
		resID = IDC_COUNT_LFOLDER;
		break;
	case CompareStats::RESULT_RDIRUNIQUE:
		resID = IDC_COUNT_RFOLDER;
		break;
	case CompareStats::RESULT_SKIP:
		resID = IDC_COUNT_FILESKIP;
		break;
	case CompareStats::RESULT_DIRSKIP:
		resID = IDC_COUNT_FOLDERSKIP;
		break;
	case CompareStats::RESULT_DIR:
		resID = IDC_COUNT_FOLDER;
		break;
	case CompareStats::RESULT_ERROR:
		resID = IDC_COUNT_ERROR;
		break;
	}
	return resID;
}

/**
 * @brief Update all status counts at current values.
 */
void CDirCompStateBar::UpdateElements()
{
	for (int i = 0; i < CompareStats::RESULT_COUNT; i++)
	{
		CompareStats::RESULT resnum = static_cast<CompareStats::RESULT>(i);
		UINT resID = GetIDFromResult(resnum);
		int count = m_pCompareStats->GetCount(resnum);
		SetDlgItemInt(resID, count);
	}
}

/**
 * @brief Handle WM_KEYDOWN messages before normal processing
 * to allow any key close statebar.
 */
BOOL CDirCompStateBar::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{		
		// Allow ESC to stop compare
		if (pMsg->wParam == VK_ESCAPE)
		{
			OnStop();
			return TRUE;
		}

		// When the scan is finished, any key will hide the bar
		if (m_pCompareStats->GetCompareState() == CompareStats::STATE_READY)
		{
			OnStop();
			return TRUE;
		}
	}
	
	return CDialogBar::PreTranslateMessage(pMsg);
}

/**
 * @brief Hook : this message is received when the window gets hidden. 
 * It is the only message we get when the user clicks on the upper right x.
 * (the message WM_CLOSE is processed in the parent CMiniFrameWnd)
 */
void CDirCompStateBar::OnWindowPosChanging( WINDOWPOS* lpwndpos )
{
	CDialogBar::OnWindowPosChanging(lpwndpos);
	if (lpwndpos->flags & SWP_HIDEWINDOW)
	{
		// need to notify CDirFrame
		CDirFrame * pDirFrame = static_cast<CDirFrame*>(GetOwner());
		ASSERT(pDirFrame != NULL);
		pDirFrame->NotifyHideStateBar();
	}
}

/**
 * @brief Resets itemcounts
 */
void CDirCompStateBar::Reset()
{
	ClearStat();
	m_lElapsed = 0;
	m_lElapsed -= ::GetTickCount();
	UpdateData(FALSE);
	m_ctlStop.SetWindowText(strAbort);
	m_bStopText = TRUE;
	// also give the focus to the button (PreTranslateMessage needs it)
	m_ctlStop.SetFocus();
}

/**
 * @brief Set pointer to compare stats.
 */
void CDirCompStateBar::SetCompareStat(CompareStats * pCompareStats)
{
	m_pCompareStats = pCompareStats;
}

/**
 * @brief Timer message received.
 */
void CDirCompStateBar::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == IDT_UPDATE)
	{
		CProgressCtrl *pProg = (CProgressCtrl*) GetDlgItem(IDC_PROGRESSCOMPARE);
		CStatic *pCompared = (CStatic *) GetDlgItem(IDC_ITEMSCOMPARED);
		CStatic *pTotal = (CStatic *) GetDlgItem(IDC_ITEMSTOTAL);
		CompareStats::CMP_STATE state = m_pCompareStats->GetCompareState();
		
		if (m_prevState == CompareStats::STATE_IDLE &&
			state == CompareStats::STATE_COLLECT)
		{
			m_prevState = CompareStats::STATE_COLLECT;
		}
		else if (m_prevState == CompareStats::STATE_COLLECT &&
			state == CompareStats::STATE_COLLECT)
		{
			TCHAR num[15] = {0};
			_itot(m_pCompareStats->GetTotalItems(), num, 10);
			pTotal->SetWindowText(num);
		}
		else if ((m_prevState == CompareStats::STATE_COLLECT ||
				m_prevState == CompareStats::STATE_IDLE) &&
				state == CompareStats::STATE_COMPARE)
		{
			TCHAR num[15] = {0};

			// Start comparing, init progressbar
			int totalItems = m_pCompareStats->GetTotalItems();
			_itot(totalItems, num, 10);
			pTotal->SetWindowText(num);

			pProg->SetRange32(0, totalItems);
			int comparedItems = m_pCompareStats->GetComparedItems();
			_itot(comparedItems, num, 10);
			pCompared->SetWindowText(num);
			pProg->SetPos(comparedItems);
			UpdateElements();
			m_prevState = CompareStats::STATE_COMPARE;
		}
		else if (m_prevState == CompareStats::STATE_COMPARE &&
				state == CompareStats::STATE_COMPARE)
		{
			TCHAR num[15] = {0};
			int comparedItems = m_pCompareStats->GetComparedItems();
			_itot(comparedItems, num, 10);
			pCompared->SetWindowText(num);
			pProg->SetPos(comparedItems);
			UpdateElements();
			m_bTimerFired = TRUE;
		}
		else if (m_prevState == CompareStats::STATE_COMPARE &&
			state == CompareStats::STATE_READY)
		{
			TCHAR num[15] = {0};
			int comparedItems = m_pCompareStats->GetComparedItems();
			_itot(comparedItems, num, 10);
			pCompared->SetWindowText(num);
			pProg->SetPos(comparedItems);
			UpdateElements();
			EndUpdating();
			m_prevState = CompareStats::STATE_READY;
			m_bTimerFired = TRUE;
		}
		else if (state == CompareStats::STATE_READY)
		{
			if (!m_bTimerFired)
			{
				TCHAR num[15] = {0};
				int totalItems = m_pCompareStats->GetTotalItems();
				_itot(totalItems, num, 10);
				pTotal->SetWindowText(num);
				int comparedItems = m_pCompareStats->GetComparedItems();
				_itot(comparedItems, num, 10);
				pCompared->SetWindowText(num);
				pProg->SetPos(comparedItems);
				UpdateElements();
				EndUpdating();
				m_prevState = CompareStats::STATE_READY;
				m_bTimerFired = TRUE;
			}
		}
	}
	else
		CDialogBar::OnTimer(nIDEvent);

}

/**
 * @brief Start timer for UI updating.
 */
void CDirCompStateBar::StartUpdating()
{
	UpdateElements();
	SetTimer(IDT_UPDATE, UPDATE_INTERVAL, NULL);
}

/**
 * @brief Stop timer updating UI.
 */
void CDirCompStateBar::EndUpdating()
{
	KillTimer(IDT_UPDATE);
}
