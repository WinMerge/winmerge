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

/////////////////////////////////////////////////////////////////////////////
// CDirCompStateBar construction destruction

/**
 * @brief Set all stats to zero
 */
void CDirCompStateBar::ClearStat()
{
	//{{AFX_DATA_INIT(CDirCompStateBar)
	m_nBinaryDiff = 0;
	m_nBinarySame = 0;
	m_nEqual = 0;
	m_nFileSkip = 0;
	m_nFolderSkip = 0;
	m_nLFile = 0;
	m_nLFolder = 0;
	m_nNotEqual = 0;
	m_nRFile = 0;
	m_nRFolder = 0;
	m_nUnknown = 0;
	//}}AFX_DATA_INIT
}

CDirCompStateBar::CDirCompStateBar(CWnd* pParent /*=NULL*/)
{
	ClearStat();
	m_bFirstUpdate = FALSE;

	VERIFY(strAbort.LoadString(IDC_COMPARISON_STOP));
	VERIFY(strClose.LoadString(IDC_COMPARISON_CLOSE));
}

BOOL CDirCompStateBar::Create(CWnd* pParentWnd)
{
	if (! CDialogBar::Create(pParentWnd, CDirCompStateBar::IDD, 
			WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_FLYBY, 
			CDirCompStateBar::IDD))
		return FALSE;

	return TRUE;
};

void CDirCompStateBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDirCompStateBar)
	DDX_Text(pDX, IDC_COUNT_BINARYDIFF, m_nBinaryDiff);
	DDX_Text(pDX, IDC_COUNT_BINARYSAME, m_nBinarySame);
	DDX_Text(pDX, IDC_COUNT_EQUAL, m_nEqual);
	DDX_Text(pDX, IDC_COUNT_FILESKIP, m_nFileSkip);
	DDX_Text(pDX, IDC_COUNT_FOLDERSKIP, m_nFolderSkip);
	DDX_Text(pDX, IDC_COUNT_LFILE, m_nLFile);
	DDX_Text(pDX, IDC_COUNT_LFOLDER, m_nLFolder);
	DDX_Text(pDX, IDC_COUNT_NOTEQUAL, m_nNotEqual);
	DDX_Text(pDX, IDC_COUNT_RFILE, m_nRFile);
	DDX_Text(pDX, IDC_COUNT_RFOLDER, m_nRFolder);
	DDX_Text(pDX, IDC_COUNT_UNKNOWN, m_nUnknown);
	DDX_Control(pDX, IDC_COMPARISON_STOP, m_ctlStop);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDirCompStateBar, CDialogBar)
	//{{AFX_MSG_MAP(CDirCompStateBar)
	ON_COMMAND(IDC_COMPARISON_STOP, OnStop)
	ON_UPDATE_COMMAND_UI(IDC_COMPARISON_STOP, OnUpdateStop)
	ON_WM_WINDOWPOSCHANGING()
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

    

/////////////////////////////////////////////////////////////////////////////
// CDirCompStateBar message handlers

void CDirCompStateBar::OnStop()
{
	// use GetOwner and not GetParentFrame
	// because FloatControlBar assigns the bar to a new parent frame
	CFrameWnd * pFrameWnd = static_cast<CFrameWnd*> (GetOwner());
	CDirDoc * pDirDoc = dynamic_cast<CDirDoc*>(pFrameWnd->GetActiveDocument());
	ASSERT(pDirDoc != NULL);

	if (pDirDoc->IsCurrentScanAbortable())
		pDirDoc->AbortCurrentScan();

	CDirFrame * pDirFrame = static_cast<CDirFrame*>(pFrameWnd);
	ASSERT(pDirFrame != NULL);
	pDirFrame->ShowProcessingBar(FALSE);
}

void CDirCompStateBar::OnUpdateStop(CCmdUI* pCmdUI)
{
	// use GetOwner and not GetParentFrame
	// because FloatControlBar assigns the bar to a new parent frame
	CFrameWnd * pFrameWnd = static_cast<CFrameWnd*> (GetOwner());
	CDirDoc * pDirDoc = dynamic_cast<CDirDoc*>(pFrameWnd->GetActiveDocument());
	ASSERT(pDirDoc != NULL);

	// adapt the text of button 
	if (m_ctlStop.GetSafeHwnd())
	{
		if (pDirDoc->IsCurrentScanAbortable())
			m_ctlStop.SetWindowText(strAbort);
		else 
		{
			CString text;
			m_ctlStop.GetWindowText(text);
			if (text != strClose) 
				m_ctlStop.SetWindowText(strClose);
			// also give the focus to the button (PreTranslateMessage needs it)
			m_ctlStop.SetFocus();
		}
	}

	pCmdUI->Enable(TRUE);
}
 
/**
 * @brief Increase given category count by one and update panel
 *
 * Diff code calls this function every time it has compeleted comparing
 * one item.
 * @todo This could be optimised by letting timer call UI update
 * couple of times per second.
 */
void CDirCompStateBar::AddElement(UINT diffcode)
{
	DIFFITEM di;
	
	if (!m_bFirstUpdate)
		FirstUpdate();

	di.diffcode = diffcode;
	if (di.isSideLeft())
	{
		// left-only
		if (di.isDirectory())
		{
			++m_nLFolder;
			CStatic * pCtrl = (CStatic *)GetDlgItem(IDC_COUNT_LFOLDER);
			UpdateText(pCtrl, m_nLFolder);
		}
		else
		{
			++m_nLFile;
			CStatic * pCtrl = (CStatic *)GetDlgItem(IDC_COUNT_LFILE);
			UpdateText(pCtrl, m_nLFile);
		}
	}
	else if (di.isSideRight())
	{
		// right-only
		if (di.isDirectory())
		{
			++m_nRFolder;
			CStatic * pCtrl = (CStatic *)GetDlgItem(IDC_COUNT_RFOLDER);
			UpdateText(pCtrl, m_nRFolder);
		}
		else
		{
			++m_nRFile;
		}
	}
	else if (di.isResultSkipped())
	{
		// skipped
		if (di.isDirectory())
		{
			++m_nFolderSkip;
			CStatic * pCtrl = (CStatic *)GetDlgItem(IDC_COUNT_FOLDERSKIP);
			UpdateText(pCtrl, m_nFolderSkip);
		}
		else
		{
			++m_nFileSkip;
			CStatic * pCtrl = (CStatic *)GetDlgItem(IDC_COUNT_FILESKIP);
			UpdateText(pCtrl, m_nFileSkip);
		}
	}
	else if (di.isResultError())
	{
		// could be directory error ?
		++m_nUnknown;
		CStatic * pCtrl = (CStatic *)GetDlgItem(IDC_COUNT_UNKNOWN);
		UpdateText(pCtrl, m_nUnknown);
	}
	// Now we know it was on both sides & compared!
	else if (di.isResultSame())
	{
		// same
		if (di.isBin())
		{
			++m_nBinarySame;
			CStatic * pCtrl = (CStatic *)GetDlgItem(IDC_COUNT_BINARYSAME);
			UpdateText(pCtrl, m_nBinarySame);
		}
		else
		{
			++m_nEqual;
			CStatic * pCtrl = (CStatic *)GetDlgItem(IDC_COUNT_EQUAL);
			UpdateText(pCtrl, m_nEqual);
		}
	}
	else
	{
		// presumably it is diff
		if (di.isDirectory())
		{
			// this doesn't happen right now, but it will
			// TODO
		}
		else
		{
			if (di.isBin())
			{
				++m_nBinaryDiff;
				CStatic * pCtrl = (CStatic *)GetDlgItem(IDC_COUNT_BINARYDIFF);
				UpdateText(pCtrl, m_nBinaryDiff);
			}
			else
			{
				++m_nNotEqual;
				CStatic * pCtrl = (CStatic *)GetDlgItem(IDC_COUNT_NOTEQUAL);
				UpdateText(pCtrl, m_nNotEqual);
			}
		}
	}
}

BOOL CDirCompStateBar::PreTranslateMessage(MSG* pMsg)
{
	// When the scan is finished, any key will hide the bar
	CFrameWnd * pFrameWnd = static_cast<CFrameWnd*> (GetOwner());
	CDirDoc * pDirDoc = dynamic_cast<CDirDoc*>(pFrameWnd->GetActiveDocument());
	if (pMsg->message == WM_KEYDOWN && !pDirDoc->IsCurrentScanAbortable())
	{
		OnStop();
		return TRUE;
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

void CDirCompStateBar::UpdateText(CStatic * ctrl, int num) const
{
	ASSERT(ctrl != NULL);
	TCHAR strNum[20] = {0};
	
	_itot(num, strNum, 10);
	ctrl->SetWindowText(strNum);
}

void CDirCompStateBar::FirstUpdate()
{
	m_bFirstUpdate = TRUE;
	UpdateData(FALSE);
}
