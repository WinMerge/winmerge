/*
Frhed - Free hex editor
Copyright (C) 2000 Raihan Kibria

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

Last change: 2013-02-24 by Jochen Neubeck
*/
/** 
 * @file  ViewSettingsDlg.cpp
 *
 * @brief Implementation of the View settings -dialog.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "LangArray.h"
#include "StringTable.h"
#include "Constants.h"
#include "AnsiConvert.h"

static WNDPROC DefWndProcDroppedComboBox = 0;

static LRESULT CALLBACK WndProcDroppedComboBox(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CTLCOLORLISTBOX:
		{
			int cxScreen = GetSystemMetrics(SM_CXSCREEN);
			HWND hLb = (HWND)lParam;
			RECT rcLb;
			::GetWindowRect(hLb, &rcLb);
			if (rcLb.right > cxScreen)
				rcLb.left -= rcLb.right - cxScreen;
			if (rcLb.left < 0)
				rcLb.left = 0;
			::SetWindowPos(hLb, 0, rcLb.left, rcLb.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}
		break;
	}
	return CallWindowProc(DefWndProcDroppedComboBox, hWnd, uMsg, wParam, lParam);
}

HComboBox *ViewSettingsDlg::pCbLang;

static BOOL fAllLanguages = FALSE;

/**
 * @brief Available and installed languages enumerator.
 * This method enumerates all available locales. Then it checks if there is a
 * matching language file available. If the language file is available the
 * language is added to the Viewsettings dialog.
 * @param [in] lpLocaleString Locale identifier string.
 * @return TRUE to continue enumeration, FALSE otherwise.
 */
BOOL ViewSettingsDlg::EnumLocalesProc(LPTSTR lpLocaleString)
{
	TCHAR path[MAX_PATH];
	GetModuleFileName(hMainInstance, path, MAX_PATH);
	PathRemoveFileSpec(path);
	PathAppend(path, static_cast<W2T>(LangFileSubFolder));
	LPTSTR name = PathAddBackslash(path);
	LCID lcid = 0;
	if (_stscanf(lpLocaleString, _T("%x"), &lcid) == 1)
	{
		LANGID langid = (LANGID)lcid;
		if (int i = GetLocaleInfo(langid, LOCALE_SISO639LANGNAME, name, 4))
		{
			int j = GetLocaleInfo(langid, LOCALE_SISO3166CTRYNAME, name + i, 4);
			name[--i] = '-';
			BOOL f = langid == LangArray::DefLangId;
			while (f == 0)
			{
				_tcscpy(name + i + j, _T(".po"));
				f = PathFileExists(path);
				if (j == 0 || SUBLANGID(langid) != SUBLANG_DEFAULT)
					break;
				j = 0;
			}
			// If SHIFT is pressed while opening the dropdown list, throw in whatever we can possibly support.
			if (f || fAllLanguages)
			{
				int k = pCbLang->AddString((LPCTSTR)MAKELONG(langid, f));
				if (langid == (langArray.m_langid ? langArray.m_langid : LangArray::DefLangId))
					pCbLang->SetCurSel(k);
			}
		}
	}
	return TRUE;
}

int ViewSettingsDlg::FormatLangId(LPWSTR bufW, LANGID langid, bool verbose)
{
#ifdef UNICODE
	int i = LangArray::LangCodeMajor(langid, bufW);
#else
	CHAR bufA[200];
	int i = LangArray::LangCodeMajor(langid, bufA);
#endif
	if (i)
	{
#ifdef UNICODE
		bufW[i - 1] = '-';
		i += LangArray::LangCodeMinor(langid, bufW + i);
		if (verbose)
		{
			bufW[i - 1] = '\t';
			i += GetLocaleInfoW(langid, LOCALE_SNATIVELANGNAME|LOCALE_USE_CP_ACP, bufW + i, 40);
			bufW[i - 1] = '\t';
			i += GetLocaleInfoW(langid, LOCALE_SNATIVECTRYNAME|LOCALE_USE_CP_ACP, bufW + i, 40);
			bufW[i - 1] = '\t';
			i += GetLocaleInfoW(langid, LOCALE_SENGLANGUAGE, bufW + i, 40);
			bufW[i - 1] = '\t';
			i += GetLocaleInfoW(langid, LOCALE_SENGCOUNTRY, bufW + i, 40);
		}
#else
		bufA[i - 1] = '-';
		i += LangArray::LangCodeMinor(langid, bufA + i);
		if (verbose)
		{
			bufA[i - 1] = '\t';
			i += GetLocaleInfoA(langid, LOCALE_SNATIVELANGNAME|LOCALE_USE_CP_ACP, bufA + i, 40);
			bufA[i - 1] = '\t';
			i += GetLocaleInfoA(langid, LOCALE_SNATIVECTRYNAME|LOCALE_USE_CP_ACP, bufA + i, 40);
			bufA[i - 1] = '\t';
			i += GetLocaleInfoA(langid, LOCALE_SENGLANGUAGE, bufA + i, 40);
			bufA[i - 1] = '\t';
			i += GetLocaleInfoA(langid, LOCALE_SENGCOUNTRY, bufA + i, 40);
		}
		MultiByteToWideChar(CP_ACP, 0, bufA, i, bufW, i);
#endif
		--i;
	}
	else
	{
		i = swprintf(bufW, L"%04x", langid);
	}
	return i;
}

void ViewSettingsDlg::OnDrawitemLangId(DRAWITEMSTRUCT *pdis)
{
	int iColorText = COLOR_WINDOWTEXT;
	int iColorTextBk = COLOR_WINDOW;
	if (pdis->itemState & ODS_SELECTED)
	{
		iColorText = COLOR_HIGHLIGHTTEXT;
		iColorTextBk =  COLOR_HIGHLIGHT;
	}
	if (HIWORD(pdis->itemData) == 0)
	{
		iColorText = COLOR_GRAYTEXT;
	}
	SetTextColor(pdis->hDC, GetSysColor(iColorText));
	SetBkColor(pdis->hDC, GetSysColor(iColorTextBk));
	int x = pdis->rcItem.left + 2;
	int y = pdis->rcItem.top + 2;
	static const int rgcx[] = { 50, 120, 180, 120, 220, 0 };
	const int *pcx = rgcx;
	UINT flags = ETO_OPAQUE;
	WCHAR buffer[200];
	int length = FormatLangId(buffer, LOWORD(pdis->itemData), true);
	LPWSTR p = buffer;
	while (LPWSTR q = StrChrW(p, L'\t'))
	{
		ExtTextOutW(pdis->hDC, x, y, flags, &pdis->rcItem, p, static_cast<UINT>(q - p), 0);
		x += *pcx ? *pcx++ : 100;
		p = q + 1;
		flags = 0;
	}
	ExtTextOutW(pdis->hDC, x, y, flags, &pdis->rcItem, p, length - static_cast<UINT>(p - buffer), 0);
	if (pdis->itemState & ODS_FOCUS)
	{
		DrawFocusRect(pdis->hDC, &pdis->rcItem);
	}
}

INT_PTR ViewSettingsDlg::OnCompareitemLangId(COMPAREITEMSTRUCT *pcis)
{
	WCHAR name1[20];
	FormatLangId(name1, LOWORD(pcis->itemData1));
	WCHAR name2[20];
	FormatLangId(name2, LOWORD(pcis->itemData2));
	int cmp = StrCmpIW(name1, name2);
	return cmp < 0 ? -1 : cmp > 0 ? +1 : 0;
	//Code below would yield numeric sort order by first PRIMARYLANGID, then SUBLANGID
	/*WORD w1 = LOWORD(pcis->itemData1);
	w1 = w1 << 10 | w1 >> 10;
	WORD w2 = LOWORD(pcis->itemData2);
	w2 = w2 << 10 | w2 >> 10;
	return w1 < w2 ? -1 : w1 > w2 ? 1 : 0;*/
}

/**
 * @brief Initialize the dialog controls.
 * This function initializes dialog controls from current option values. Not
 * reading from registry since there are values calculated runtime (min.
 * offset length).
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE.
 */
BOOL ViewSettingsDlg::OnInitDialog(HWindow *pDlg)
{
	pDlg->SetDlgItemInt(IDC_SETTINGS_BYTESPERLINE, iBytesPerLine);
	pDlg->SetDlgItemInt(IDC_SETTINGS_OFFSETLEN, iMinOffsetLen);
	UINT checked = iAutomaticBPL == 0 ? BST_UNCHECKED : BST_CHECKED;
	pDlg->CheckDlgButton(IDC_SETTINGS_ADJUST_BYTELINE, checked);
	pDlg->CheckDlgButton(bUnsignedView ? IDC_SETTINGS_CARETUNSIGN :
			IDC_SETTINGS_CARETSIGN, BST_CHECKED);
	checked = bOpenReadOnly == 0 ? BST_UNCHECKED : BST_CHECKED;
	pDlg->CheckDlgButton(IDC_SETTINGS_OPENRO, checked);
	checked = bAutoOffsetLen == 0 ? BST_UNCHECKED : BST_CHECKED;
	pDlg->CheckDlgButton(IDC_SETTINGS_ADJOFFSET, checked);
	pDlg->SetDlgItemText(IDC_SETTINGS_EDITOR, TexteditorName);
	pCbLang = static_cast<HComboBox *>(pDlg->GetDlgItem(IDC_SETTINGS_LANGUAGE));
	// Adjust dropped control width.
	pCbLang->SetDroppedWidth(698);
	// Adjust dropped control height to about half of screen.
	RECT rc;
	pCbLang->GetWindowRect(&rc);
	int cyScreen = GetSystemMetrics(SM_CYSCREEN);
	int cyEdit = rc.bottom - rc.top;
	int cyItem = pCbLang->GetItemHeight(0);
	pCbLang->SetWindowPos(NULL, 0, 0, rc.right - rc.left, (cyScreen / 2 - cyEdit) / cyItem * cyItem + cyEdit + 2, SWP_NOMOVE | SWP_NOZORDER);
	// Populate the dropdown list.
	fAllLanguages = FALSE;
	EnumSystemLocales(EnumLocalesProc, LCID_SUPPORTED);
	return TRUE;
}

/**
 * @brief Save the settings in dialog controls to registry.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE.
 */
BOOL ViewSettingsDlg::Apply(HWindow *pDlg)
{
	iBytesPerLine = pDlg->GetDlgItemInt(IDC_SETTINGS_BYTESPERLINE, 0);
	if (iBytesPerLine < 1)
		iBytesPerLine = 1;
	iMinOffsetLen = pDlg->GetDlgItemInt(IDC_SETTINGS_OFFSETLEN, 0);
	if (iMinOffsetLen < 1)
		iMinOffsetLen = 1;
	// Get the text editor path and name.
	pDlg->GetDlgItemText(IDC_SETTINGS_EDITOR, TexteditorName, RTL_NUMBER_OF(TexteditorName));
	
	UINT res = pDlg->IsDlgButtonChecked(IDC_SETTINGS_ADJUST_BYTELINE);
	if (res == BST_CHECKED)
		iAutomaticBPL = 1;
	else
		iAutomaticBPL = 0;

	res = pDlg->IsDlgButtonChecked(IDC_SETTINGS_ADJOFFSET);
	if (res == BST_CHECKED)
		bAutoOffsetLen = 1;
	else
		bAutoOffsetLen = 0;

	res = pDlg->IsDlgButtonChecked(IDC_SETTINGS_CARETUNSIGN);
	if (res == BST_CHECKED)
		bUnsignedView = true;
	else if (res == BST_UNCHECKED)
		bUnsignedView = false;

	bOpenReadOnly = pDlg->IsDlgButtonChecked(IDC_SETTINGS_OPENRO);
	int i = pCbLang->GetCurSel();
	if (i != -1)
	{
		DWORD itemData = pCbLang->GetItemData(i);
		load_lang((LANGID)itemData);
	}
	save_ini_data();
	resize_window();
	return TRUE;
}

/**
 * @brief Show the file selection dialog to select text editor executable.
 * @param [in] hDlg Handle to the view settings dialog.
 */
void ViewSettingsDlg::SelectEditor(HWindow *pDlg)
{
	TCHAR szFileName[MAX_PATH];
	szFileName[0] = '\0';
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hwndOwner = pwnd->m_hWnd;
	ofn.lpstrFilter = GetLangString(IDS_OPEN_ALL_FILES);
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
	if (GetOpenFileName(&ofn))
	{
		_tcsncpy(TexteditorName, szFileName, MAX_PATH);
		pDlg->SetDlgItemText(IDC_SETTINGS_EDITOR, TexteditorName);
	}
}

/**
 * @brief Handle dialog messages.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg The message.
 * @param [in] wParam The command in the message.
 * @param [in] lParam The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
 */
INT_PTR ViewSettingsDlg::DlgProc(HWindow *pDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(pDlg);
	case WM_COMMAND:
		switch (wParam)
		{
		case IDC_SETTINGS_EDITORSELECT:
			SelectEditor(pDlg);
			return TRUE;
		case IDOK:
			if (Apply(pDlg))
			{
			case IDCANCEL:
				pDlg->EndDialog(wParam);
			}
			return TRUE;
		case MAKEWPARAM(IDC_SETTINGS_LANGUAGE, CBN_DROPDOWN):
			if (fAllLanguages == FALSE && GetKeyState(VK_SHIFT) < 0)
			{
				fAllLanguages = TRUE;
				SendMessage((HWND)lParam, CB_RESETCONTENT, 0, 0);
				EnumSystemLocales(EnumLocalesProc, LCID_SUPPORTED);
			}
			if (DefWndProcDroppedComboBox == 0)
			{
				DefWndProcDroppedComboBox = SubclassWindow((HWND)lParam, WndProcDroppedComboBox);
			}
			return TRUE;
		case MAKEWPARAM(IDC_SETTINGS_LANGUAGE, CBN_CLOSEUP):
			if (DefWndProcDroppedComboBox)
			{
				SubclassWindow((HWND)lParam, DefWndProcDroppedComboBox);
				DefWndProcDroppedComboBox = 0;
			}
			return TRUE;
		}
		break;
	case WM_DRAWITEM:
		switch (wParam)
		{
		case IDC_SETTINGS_LANGUAGE:
			OnDrawitemLangId(reinterpret_cast<DRAWITEMSTRUCT *>(lParam));
			return TRUE;
		}
		break;
	case WM_COMPAREITEM:
		switch (wParam)
		{
		case IDC_SETTINGS_LANGUAGE:
			return OnCompareitemLangId(reinterpret_cast<COMPAREITEMSTRUCT *>(lParam));
		}
		break;

	case WM_HELP:
		OnHelp(pDlg);
		break;
	}
	return FALSE;
}
