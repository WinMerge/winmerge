/** 
 * @file  DirStatusBar.cpp
 *
 * @brief Implementation file for CDirStatusBar class
 */

#include "stdafx.h"
#include "DirStatusBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CDirStatusBar, CStatusBar)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

BOOL CDirStatusBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    ::SetCursor (::LoadCursor (nullptr, IDC_HAND));
	return TRUE;
}

