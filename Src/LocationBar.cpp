//////////////////////////////////////////////////////////////////////
/** 
 * @file  LocationBar.cpp
 *
 * @brief Implementation file for CLocationBar
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Merge.h"

#include "ChildFrm.h"
#include "LocationBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CLocationBar, TViewBarBase);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLocationBar::CLocationBar()
{
}


CLocationBar::~CLocationBar()
{
}


BEGIN_MESSAGE_MAP(CLocationBar, TViewBarBase)
	//{{AFX_MSG_MAP(CRegBar)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocationBar message handlers

/**
* @brief Just create ourself
*
* @note The control are created in the parent frame CChildFrame
*
*/
BOOL CLocationBar::Create(
	CWnd* pParentWnd,
	LPCTSTR lpszWindowName,
	DWORD dwStyle,
	UINT nID)
{
	return TViewBarBase::Create(
		lpszWindowName,
		pParentWnd,
		nID,
		dwStyle);
}


///	Create the frame window associated with the view bar. */
int CLocationBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (TViewBarBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetSCBStyle(SCBS_EDGELEFT | SCBS_EDGERIGHT | SCBS_SIZECHILD);

	extendBar(CPoint(40,0));
	
	return 0;
}

/**
* @note The window must always be docked after movement
* there are too much troubles if we get reparented to some minidockbar 
*
*/
void CLocationBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	TViewBarBase::OnLButtonDown(nFlags, point);
  if (m_pDockBar != NULL)
  {
    if (IsVertDocked() == FALSE)
			m_pDockContext->ToggleDocking();
	}
}

void CLocationBar::OnSize(UINT nType, int cx, int cy) 
{
	TViewBarBase::OnSize(nType, cx, cy);
}