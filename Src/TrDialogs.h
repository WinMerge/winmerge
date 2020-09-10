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

void DpiChangedImplHelper(HWND hwnd, int olddpi, int newdpi);

template <class T>
class DpiChangedImpl
{
public:
	LRESULT OnDpiChangedImpl(WPARAM wParam, LPARAM lParam)
	{
		T* pwnd = static_cast<T*>(this);
		int olddpi = pwnd->m_dpi;
		pwnd->UpdateDpi();
		pwnd->Default();
		if (olddpi != pwnd->m_dpi)
			DpiChangedImplHelper(pwnd->m_hWnd, olddpi, pwnd->m_dpi);
		return 0;
	}
};

class CTrDialog
	: public DpiAware::PerMonitorDpiAwareCWnd<CDialog>
	, public DlgUtils<CTrDialog>
	, public DpiChangedImpl<CTrDialog>
{
	friend DpiChangedImpl;
	DECLARE_DYNAMIC(CTrDialog)
public:
	using DpiAware::PerMonitorDpiAwareCWnd<CDialog>::PerMonitorDpiAwareCWnd;

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam)
	{
		return OnDpiChangedImpl(wParam, lParam);
	}
};

class CTrPropertyPage
	: public DpiAware::PerMonitorDpiAwareCWnd<CPropertyPage>
	, public DlgUtils<CTrPropertyPage>
	, public DpiChangedImpl<CTrPropertyPage>
{
	friend DpiChangedImpl;
	DECLARE_DYNAMIC(CTrPropertyPage)
public:
	using DpiAware::PerMonitorDpiAwareCWnd<CPropertyPage>::PerMonitorDpiAwareCWnd;

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDpiChangedBeforeParent(WPARAM wParam, LPARAM lParam) {
		return OnDpiChangedImpl(wParam, lParam);
	}
};

class CTrDialogBar
	: public DpiAware::PerMonitorDpiAwareCWnd<CDialogBar>
	, public DlgUtils<CTrDialogBar>
	, public DpiChangedImpl<CTrDialogBar>
{
	friend DpiChangedImpl;
	DECLARE_DYNAMIC(CTrDialogBar)
public:
	virtual BOOL Create(CWnd* pParentWnd, LPCTSTR lpszTemplateName,
		UINT nStyle, UINT nID);
	virtual BOOL Create(CWnd* pParentWnd, UINT nIDTemplate,
		UINT nStyle, UINT nID);

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnDpiChangedBeforeParent(WPARAM wParam, LPARAM lParam) { return OnDpiChangedImpl(wParam, lParam); }
};
