#include <StdAfx.h>
#include "Merge.h"
#include "TrDialogs.h"
#include "Logger.h"

IMPLEMENT_DYNAMIC(CTrDialog, CDialog)
IMPLEMENT_DYNAMIC(CTrPropertyPage, CPropertyPage)
IMPLEMENT_DYNAMIC(CTrDialogBar, CDialogBar)

void StaticDlgUtils::WildcardRemoveDuplicatePatterns(String& patterns)
{
	size_t i = 0, j = 0, k = 0;
	while ((j = patterns.find_first_of(L"; ", i)) != String::npos &&
		(k = patterns.find_last_of(L"; ", j) + 1) != patterns.length())
	{
		tchar_t const sep = patterns[j];
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
	__super::OnInitDialog();
	return TRUE;
}

void CTrDialog::OnOK()
{
	RootLogger::Info(GetTitleText() + _T(": ") + _("OK"));
	__super::OnOK();
}

void CTrDialog::OnCancel()
{
	RootLogger::Info(GetTitleText() + _T(": ") + _("Cancel"));
	__super::OnCancel();
}

BOOL CTrPropertyPage::OnInitDialog()
{
	theApp.TranslateDialog(m_hWnd);
	__super::OnInitDialog();
	return TRUE;
}

void CTrPropertyPage::OnOK()
{
	RootLogger::Info(GetTitleText() + _T(": ") + _("OK"));
	__super::OnOK();
}

void CTrPropertyPage::OnCancel()
{
	RootLogger::Info(GetTitleText() + _T(": ") + _("Cancel"));
	__super::OnCancel();
}

BOOL CTrDialogBar::Create(CWnd* pParentWnd, LPCTSTR lpszTemplateName,
	UINT nStyle, UINT nID)
{
	BOOL bSucceeded = __super::Create(pParentWnd, lpszTemplateName, nStyle, nID);
	if (bSucceeded)
		theApp.TranslateDialog(m_hWnd);
	return bSucceeded;
}

BOOL CTrDialogBar::Create(CWnd* pParentWnd, UINT nIDTemplate,
	UINT nStyle, UINT nID)
{
	BOOL bSucceeded = __super::Create(pParentWnd, nIDTemplate, nStyle, nID);
	if (bSucceeded)
		theApp.TranslateDialog(m_hWnd);
	return bSucceeded;
}
