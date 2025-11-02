/**
 * @file MyColorDialog.cpp
 *
 * @brief Implementation of a custom color dialog that supports dark mode.
 */
#include "StdAfx.h"
#include "MyColorDialog.h"
#include "DarkModeLib.h"

CMyColorDialog::CMyColorDialog(COLORREF clrInit, DWORD dwFlags, CWnd* pParentWnd) : 
	CColorDialog(clrInit, dwFlags, pParentWnd)
{
	if (DarkMode::isEnabled())
	{
		m_cc.Flags |= CC_FLAGS_DARK;
		m_cc.lpfnHook = static_cast<LPCCHOOKPROC>(DarkMode::HookDlgProc);
	}
}