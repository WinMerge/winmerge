#pragma once

#include "DDXHelper.h"

#undef GetDlgItemText
#undef SetDlgItemText

class StaticDlgUtils
{
protected:
	static void WildcardRemoveDuplicatePatterns(String& patterns);
};

template<class T>
class DlgUtils : public StaticDlgUtils
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
	using CDialog::CDialog;

	virtual BOOL OnInitDialog();
};

class CTrPropertyPage : public CPropertyPage, public DlgUtils<CTrPropertyPage>
{
	DECLARE_DYNAMIC(CTrPropertyPage)
public:
	using CPropertyPage::CPropertyPage;

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
