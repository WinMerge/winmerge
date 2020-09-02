#include <StdAfx.h>
#include "Merge.h"
#include "TrDialogs.h"

IMPLEMENT_DYNAMIC(CTrDialog, CDialog)
IMPLEMENT_DYNAMIC(CTrPropertyPage, CPropertyPage)
IMPLEMENT_DYNAMIC(CTrDialogBar, CDialogBar)

BEGIN_MESSAGE_MAP(CTrDialog, CDialog)
	ON_MESSAGE(0x02E0/*WM_DPICHANGED*/, OnDpiChanged)
END_MESSAGE_MAP()

void StaticDlgUtils::WildcardRemoveDuplicatePatterns(String& patterns)
{
	size_t i = 0, j = 0, k = 0;
	while ((j = patterns.find_first_of(L"; ", i)) != String::npos &&
		(k = patterns.find_last_of(L"; ", j) + 1) != patterns.length())
	{
		TCHAR const sep = patterns[j];
		patterns[j] = L'\0';
		if (PathMatchSpec(patterns.c_str() + i, patterns.c_str() + k))
		{
			patterns.erase(i, k - i);
		}
		else
		{
			patterns[j] = sep;
			i = k;
		}
	}
}

BOOL CTrDialog::OnInitDialog()
{
	theApp.TranslateDialog(m_hWnd);
	CDialog::OnInitDialog();
	return TRUE;
}

LRESULT CTrDialog::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	int dpi = HIWORD(wParam);
	theApp.ChangeDialogFont(m_hWnd, dpi);
	RECT* const prcNew = (RECT*)lParam;
	SetWindowPos(nullptr, 
		prcNew->left, prcNew->top, prcNew->right - prcNew->left, prcNew->bottom - prcNew->top,
		SWP_NOZORDER | SWP_NOACTIVATE);
	return 0;
}

BOOL CTrPropertyPage::OnInitDialog()
{
	theApp.TranslateDialog(m_hWnd);
	CPropertyPage::OnInitDialog();
	return TRUE;
}

BOOL CTrDialogBar::Create(CWnd* pParentWnd, LPCTSTR lpszTemplateName,
	UINT nStyle, UINT nID)
{
	BOOL bSucceeded = CDialogBar::Create(pParentWnd, lpszTemplateName, nStyle, nID);
	if (bSucceeded)
		theApp.TranslateDialog(m_hWnd);
	return bSucceeded;
}

BOOL CTrDialogBar::Create(CWnd* pParentWnd, UINT nIDTemplate,
	UINT nStyle, UINT nID)
{
	BOOL bSucceeded = CDialogBar::Create(pParentWnd, nIDTemplate, nStyle, nID);
	if (bSucceeded)
		theApp.TranslateDialog(m_hWnd);
	return bSucceeded;
}
