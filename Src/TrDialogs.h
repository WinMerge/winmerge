#pragma once

#include "DDXHelper.h"

class CTrDialog : public CDialog
{
	DECLARE_DYNAMIC(CTrDialog)
public:
	CTrDialog() : CDialog() {}
	CTrDialog(UINT nIDTemplate, CWnd *pParent = NULL) : CDialog(nIDTemplate, pParent) {}
	CTrDialog(LPCTSTR lpszTemplateName, CWnd *pParentWnd = NULL) : CDialog(lpszTemplateName, pParentWnd) {}

	virtual BOOL OnInitDialog();
};

class CTrPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CTrPropertyPage)
public:
	CTrPropertyPage() : CPropertyPage() {}
	explicit CTrPropertyPage(UINT nIDTemplate, UINT nIDCaption = 0, DWORD dwSize = sizeof(PROPSHEETPAGE))
		: CPropertyPage(nIDTemplate, nIDCaption, dwSize) {}
	explicit CTrPropertyPage(LPCTSTR lpszTemplateName, UINT nIDCaption = 0, DWORD dwSize = sizeof(PROPSHEETPAGE))
		: CPropertyPage(lpszTemplateName, nIDCaption, dwSize) {}

	virtual BOOL OnInitDialog();
};

class CTrDialogBar : public CDialogBar
{
	DECLARE_DYNAMIC(CTrDialogBar)
public:
	virtual BOOL Create(CWnd* pParentWnd, LPCTSTR lpszTemplateName,
		UINT nStyle, UINT nID);
	virtual BOOL Create(CWnd* pParentWnd, UINT nIDTemplate,
		UINT nStyle, UINT nID);
};
