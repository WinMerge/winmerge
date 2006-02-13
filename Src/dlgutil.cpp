/** 
 * @file  dlgutil.cpp
 *
 * @brief Miscellaneous dialog utility functions
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "dlgutil.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 * @brief Show WinMerge icon as dialog icon
 */
void
dlgutil_SetMainIcon(CDialog * dlg)
{
	CMainFrame::SetMainIcon(dlg);
}

/**
 * @brief Display new dialog in center of WinMerge main window
 */
void
dlgutil_CenterToMainFrame(CDialog * dlg)
{
	CMainFrame::CenterToMainFrame(dlg);
}

