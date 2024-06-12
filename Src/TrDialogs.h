#pragma once

#include "DDXHelper.h"
#include "utils/DpiAware.h"

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

void DpiChangedImplHelper(HWND hwnd, int olddpi, int newdpi);

class CTrDialog
	: public DpiAware::CDpiAwareDialog<CDialog>
	, public DlgUtils<CTrDialog>
{
	DECLARE_DYNAMIC(CTrDialog)
public:
	using DpiAware::CDpiAwareDialog<CDialog>::CDpiAwareDialog;

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam)
	{
		int olddpi = m_dpi;
		LRESULT result = __super::OnDpiChanged(wParam, lParam);
		DpiChangedImplHelper(m_hWnd, olddpi, m_dpi);
		return result;
	}
};

class CTrPropertyPage
	: public DpiAware::CDpiAwareDialog<CPropertyPage>
	, public DlgUtils<CTrPropertyPage>
{
	DECLARE_DYNAMIC(CTrPropertyPage)
public:
	using DpiAware::CDpiAwareDialog<CPropertyPage>::CDpiAwareDialog;

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDpiChangedBeforeParent(WPARAM wParam, LPARAM lParam) {
		int olddpi = m_dpi;
		LRESULT result = __super::OnDpiChangedBeforeParent(wParam, lParam);
		DpiChangedImplHelper(m_hWnd, olddpi, m_dpi);
		return result;
	}
};

class CTrDialogBar
	: public DpiAware::CDpiAwareDialog<CDialogBar>
	, public DlgUtils<CTrDialogBar>
{
	DECLARE_DYNAMIC(CTrDialogBar)
public:
	virtual BOOL Create(CWnd* pParentWnd, LPCTSTR lpszTemplateName,
		UINT nStyle, UINT nID);
	virtual BOOL Create(CWnd* pParentWnd, UINT nIDTemplate,
		UINT nStyle, UINT nID);

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDpiChangedBeforeParent(WPARAM wParam, LPARAM lParam)
	{
		int olddpi = m_dpi;
		LRESULT result = __super::OnDpiChangedBeforeParent(wParam, lParam);
		DpiChangedImplHelper(m_hWnd, olddpi, m_dpi);
		return result;
	}
};
