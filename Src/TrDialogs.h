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

	void ShowDlgItem(unsigned id, bool show)
	{
		dlg()->GetDlgItem(id)->ShowWindow(show ? SW_SHOW : SW_HIDE);
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
		dlg()->SetDlgItemTextW(id, text.c_str());
	}

	void* GetDlgItemDataCurSel(unsigned id)
	{
		CComboBox * cbo = (CComboBox *)dlg()->GetDlgItem(id);
		if (!cbo)
			return nullptr;
		return cbo->GetItemDataPtr(cbo->GetCurSel());
	}

	void SetDlgItemComboBoxList(unsigned id, const std::initializer_list<String>& list)
	{
		CComboBox * cbo = (CComboBox *)dlg()->GetDlgItem(id);
		if (!cbo)
			return;
		cbo->SetRedraw(false);
		for (const auto& item : list)
			cbo->AddString(item.c_str());
		cbo->SetRedraw(true);
	}

	void SetDlgItemComboBoxList(unsigned id, const std::initializer_list<std::pair<String, const wchar_t*>>& list, const String& sel)
	{
		CComboBox * cbo = (CComboBox *)dlg()->GetDlgItem(id);
		if (!cbo)
			return;
		int i = 0;
		cbo->SetRedraw(false);
		for (const auto& item : list)
		{
			cbo->AddString(item.first.c_str());
			if (item.second == sel)
				cbo->SetCurSel(i);
			cbo->SetItemDataPtr(i++, reinterpret_cast<void*>(const_cast<wchar_t*>(item.second)));
		}
		cbo->SetRedraw(true);
	}

	String GetTitleText()
	{
		CString title;
		dlg()->GetWindowText(title);
		return title.GetString();
	}

	void SetTitleText(const String& text)
	{
		dlg()->SetWindowText(text.c_str());
	}
};

class CTrDialog : public CDialog, public DlgUtils<CTrDialog>
{
	DECLARE_DYNAMIC(CTrDialog)
public:
	using CDialog::CDialog;

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
};

class CTrPropertyPage : public CPropertyPage, public DlgUtils<CTrPropertyPage>
{
	DECLARE_DYNAMIC(CTrPropertyPage)
public:
	using CPropertyPage::CPropertyPage;

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
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
