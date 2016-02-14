#pragma once

#include "DDXHelper.h"

namespace DlgUtils
{
	template <class T>
	void EnableDlgItem(T* dlg, unsigned id, bool enabled)
	{
		dlg->GetDlgItem(id)->EnableWindow(enabled);
	}

	template <class T>
	void SetDlgItemFocus(T* dlg, unsigned id)
	{
		dlg->GetDlgItem(id)->SetFocus();
	}

	template <class T>
	unsigned GetDlgItemText(T* dlg, unsigned id, String& text)
	{
		return static_cast<CWnd *>(dlg)->GetDlgItemText(id, PopString(text));
	}

	template <class T>
	void SetDlgItemText(T* dlg, unsigned id, const String& text)
	{
		return static_cast<CWnd *>(dlg)->SetDlgItemText(id, text.c_str());
	}
}

class CTrDialog : public CDialog
{
	DECLARE_DYNAMIC(CTrDialog)
public:
	CTrDialog() : CDialog() {}
	CTrDialog(UINT nIDTemplate, CWnd *pParent = NULL) : CDialog(nIDTemplate, pParent) {}
	CTrDialog(LPCTSTR lpszTemplateName, CWnd *pParentWnd = NULL) : CDialog(lpszTemplateName, pParentWnd) {}

	virtual BOOL OnInitDialog();
	void EnableDlgItem(unsigned id, bool enabled) { DlgUtils::EnableDlgItem(this, id, enabled); }
	void SetDlgItemFocus(unsigned id) { DlgUtils::SetDlgItemFocus(this, id); }
	unsigned GetDlgItemText(unsigned id, String& text) { return DlgUtils::GetDlgItemText(this, id, text); }
	void SetDlgItemText(unsigned id, const String& text) { DlgUtils::SetDlgItemText(this, id, text); }
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
	void EnableDlgItem(unsigned id, bool enabled) { DlgUtils::EnableDlgItem(this, id, enabled); }
	void SetDlgItemFocus(unsigned id) { DlgUtils::SetDlgItemFocus(this, id); }
	unsigned GetDlgItemText(unsigned id, String& text) { return DlgUtils::GetDlgItemText(this, id, text); }
	void SetDlgItemText(unsigned id, const String& text) { DlgUtils::SetDlgItemText(this, id, text); }
};

class CTrDialogBar : public CDialogBar
{
	DECLARE_DYNAMIC(CTrDialogBar)
public:
	virtual BOOL Create(CWnd* pParentWnd, LPCTSTR lpszTemplateName,
		UINT nStyle, UINT nID);
	virtual BOOL Create(CWnd* pParentWnd, UINT nIDTemplate,
		UINT nStyle, UINT nID);
	void EnableDlgItem(unsigned id, bool enabled) { DlgUtils::EnableDlgItem(this, id, enabled); }
	void SetDlgItemFocus(unsigned id) { DlgUtils::SetDlgItemFocus(this, id); }
	unsigned GetDlgItemText(unsigned id, String& text) { return DlgUtils::GetDlgItemText(this, id, text); }
	void SetDlgItemText(unsigned id, const String& text) { DlgUtils::SetDlgItemText(this, id, text); }
};
