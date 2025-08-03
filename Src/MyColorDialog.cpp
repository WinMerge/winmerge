#include "StdAfx.h"
#include "MyColorDialog.h"
#include "MergeDarkMode.h"

CMyColorDialog::CMyColorDialog(COLORREF clrInit, DWORD dwFlags, CWnd* pParentWnd) : 
	CColorDialog(clrInit, dwFlags, pParentWnd)
{
#if defined(USE_DARKMODELIB)
	if (DarkMode::isEnabled())
	{
		m_cc.Flags |= CC_FLAGS_DARK;
		m_cc.lpfnHook = static_cast<LPCCHOOKPROC>(DarkMode::HookDlgProc);
	}
#endif
}