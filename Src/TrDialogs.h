#pragma once

#include "DDXHelper.h"

#undef GetDlgItemText
#undef SetDlgItemText

template<class T>
class DlgUtils
{
	T *dlg() { return static_cast<T *>(this); }

public:
	void EnableDlgItem(unsigned id, bool enabled)
	{
		dlg()->GetDlgItem(id)->EnableWindow(enabled);
	}

	void SetDlgItemFocus(unsigned id)
	{
		dlg()->GetDlgItem(id)->SetFocus();
	}

	unsigned GetDlgItemText(unsigned id, String& text)
	{
		CString cstrText = text.c_str();
		unsigned uResult = dlg()->GetDlgItemTextW(id, cstrText);
		text = cstrText;
		return uResult;
	}

	void SetDlgItemText(unsigned id, const String& text)
	{
		return dlg()->SetDlgItemTextW(id, text.c_str());
	}
};

class CTrDialog : public CDialog, public DlgUtils<CTrDialog>
{
	DECLARE_DYNAMIC(CTrDialog)
public:
	CTrDialog() : CDialog() {}
	CTrDialog(UINT nIDTemplate, CWnd *pParent = NULL) : CDialog(nIDTemplate, pParent) {}
	CTrDialog(LPCTSTR lpszTemplateName, CWnd *pParentWnd = NULL) : CDialog(lpszTemplateName, pParentWnd) {}

	virtual BOOL OnInitDialog();
};

class CTrPropertyPage : public CPropertyPage, public DlgUtils<CTrPropertyPage>
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

class CTrDialogBar : public CDialogBar, public DlgUtils<CTrDialogBar>
{
	DECLARE_DYNAMIC(CTrDialogBar)
public:
	virtual BOOL Create(CWnd* pParentWnd, LPCTSTR lpszTemplateName,
		UINT nStyle, UINT nID);
	virtual BOOL Create(CWnd* pParentWnd, UINT nIDTemplate,
		UINT nStyle, UINT nID);
};
