#pragma once

#include <afxwin.h>
#include <afxext.h>

class CMyColorDialog : public CColorDialog
{
public:
	CMyColorDialog(COLORREF clrInit = 0, DWORD dwFlags = 0, CWnd* pParentWnd = NULL);
};