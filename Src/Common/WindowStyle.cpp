/** 
 * @file  WindowStyle.cpp
 *
 * @brief Utilities (trivial) for manipulating window style
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "StdAfx.h"
#include "WindowStyle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/**
 * @brief Add requested style(s) to windows current style
 */
void
WindowStyle_Add(CWnd * pwnd, DWORD newStyle)
{
	DWORD style = pwnd->GetStyle();
	style |= newStyle;
	SetWindowLong(pwnd->m_hWnd, GWL_STYLE, style);
}
