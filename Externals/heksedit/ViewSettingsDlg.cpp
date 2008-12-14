/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  ViewSettingsDlg.cpp
 *
 * @brief Implementation of the View settings -dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: ViewSettingsDlg.cpp 198 2008-12-09 19:53:33Z kimmov $

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "LangArray.h"

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
			::SetWindowPos(hLb, 0, rcLb.left, rcLb.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
		}
		break;
	}
	return CallWindowProc(DefWndProcDroppedComboBox, hWnd, uMsg, wParam, lParam);
}

HWND ViewSettingsDlg::hCbLang;

BOOL ViewSettingsDlg::EnumLocalesProc(LPTSTR lpLocaleString)
{
	TCHAR path[MAX_PATH];
	GetModuleFileName(hMainInstance, path, MAX_PATH);
	PathRemoveFileSpec(path);
	PathAppend(path, _T("heksedit.lng"));
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
			int k = SendMessage(hCbLang, CB_ADDSTRING, 0, MAKELONG(langid, f));
			if (langid == (langArray.m_langid ? langArray.m_langid : LangArray::DefLangId))
				SendMessage(hCbLang, CB_SETCURSEL, k, 0);
		}
	}
	return TRUE;
}

int ViewSettingsDlg::FormatLangId(LPTSTR buf, LANGID langid, int verbose)
{
	//char bufA[200];
	int i = LangArray::LangCodeMajor(langid, buf);
	if (i)
	{
		buf[i - 1] = '-';
		i += LangArray::LangCodeMinor(langid, buf + i);
		if (verbose)
		{
			buf[i - 1] = '\t';
			i += GetLocaleInfo(langid, LOCALE_SNATIVELANGNAME|LOCALE_USE_CP_ACP, buf + i, 40);
			buf[i - 1] = '\t';
			i += GetLocaleInfo(langid, LOCALE_SNATIVECTRYNAME|LOCALE_USE_CP_ACP, buf + i, 40);
			buf[i - 1] = '\t';
			i += GetLocaleInfo(langid, LOCALE_SENGLANGUAGE, buf + i, 40);
			buf[i - 1] = '\t';
			i += GetLocaleInfo(langid, LOCALE_SENGCOUNTRY, buf + i, 40);
		}
		--i;
	}
	else
	{
		i = _stprintf(buf, _T("%04x"), langid);
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
	TCHAR buffer[200];
	int length = FormatLangId(buffer, LOWORD(pdis->itemData), TRUE);
	LPTSTR p = buffer;
	while (LPTSTR q = StrChr(p, _T('\t')))
	{
		ExtTextOut(pdis->hDC, x, y, flags, &pdis->rcItem, p, q - p, 0);
		x += *pcx ? *pcx++ : 100;
		p = q + 1;
		flags = 0;
	}
	ExtTextOut(pdis->hDC, x, y, flags, &pdis->rcItem, p, length - (p - buffer), 0);
	if (pdis->itemState & ODS_FOCUS)
	{
		DrawFocusRect(pdis->hDC, &pdis->rcItem);
	}
}

INT_PTR ViewSettingsDlg::OnCompareitemLangId(COMPAREITEMSTRUCT *pcis)
{
	TCHAR name1[20];
	FormatLangId(name1, LOWORD(pcis->itemData1));
	TCHAR name2[20];
	FormatLangId(name2, LOWORD(pcis->itemData2));
	int cmp = lstrcmpi(name1, name2);
	return cmp < 0 ? -1 : cmp > 0 ? +1 : 0;
	//Code below would yield numeric sort order by first PRIMARYLANGID, then SUBLANGID
	/*WORD w1 = LOWORD(pcis->itemData1);
	w1 = w1 << 10 | w1 >> 10;
	WORD w2 = LOWORD(pcis->itemData2);
	w2 = w2 << 10 | w2 >> 10;
	return w1 < w2 ? -1 : w1 > w2 ? 1 : 0;*/
}

BOOL ViewSettingsDlg::OnInitDialog(HWND hDlg)
{
	SetDlgItemInt(hDlg, IDC_SETTINGS_BYTESPERLINE, iBytesPerLine, TRUE);
	SetDlgItemInt(hDlg, IDC_SETTINGS_OFFSETLEN, iMinOffsetLen, TRUE);
	CheckDlgButton(hDlg, IDC_SETTINGS_ADJUST_BYTELINE, iAutomaticBPL);
	CheckDlgButton(hDlg, bUnsignedView ? IDC_SETTINGS_CARETUNSIGN :
			IDC_SETTINGS_CARETSIGN, BST_CHECKED);
	CheckDlgButton(hDlg, IDC_SETTINGS_OPENRO, bOpenReadOnly);
	CheckDlgButton(hDlg, IDC_SETTINGS_ADJOFFSET, bAutoOffsetLen);
	SetDlgItemText(hDlg, IDC_SETTINGS_EDITOR, TexteditorName);
	hCbLang = GetDlgItem(hDlg, IDC_SETTINGS_LANGUAGE);
	SendMessage(hCbLang, CB_SETDROPPEDWIDTH, 698, 0);
	RECT rc;
	GetWindowRect(hCbLang, &rc);
	int cyItem = SendMessage(hCbLang, CB_GETITEMHEIGHT, -1, 0) - 2;
	int cyScreen = ::GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(hCbLang, 0, 0, 0, rc.right - rc.left, (cyScreen - 10) / 2 / cyItem * cyItem + 10, SWP_NOMOVE | SWP_NOZORDER);
	EnumSystemLocales(EnumLocalesProc, LCID_SUPPORTED);
	return TRUE;
}

BOOL ViewSettingsDlg::Apply(HWND hDlg)
{
	iBytesPerLine = GetDlgItemInt(hDlg, IDC_SETTINGS_BYTESPERLINE, 0, TRUE);
	if (iBytesPerLine < 1)
		iBytesPerLine = 1;
	iMinOffsetLen = GetDlgItemInt(hDlg, IDC_SETTINGS_OFFSETLEN, 0, TRUE);
	if (iMinOffsetLen < 1)
		iMinOffsetLen = 1;
	// Get the text editor path and name.
	GetDlgItemText(hDlg, IDC_SETTINGS_EDITOR, TexteditorName, RTL_NUMBER_OF(TexteditorName));
	iAutomaticBPL = IsDlgButtonChecked(hDlg, IDC_SETTINGS_ADJUST_BYTELINE);
	bAutoOffsetLen = IsDlgButtonChecked(hDlg, IDC_SETTINGS_ADJOFFSET);
	bUnsignedView = IsDlgButtonChecked(hDlg, IDC_SETTINGS_CARETUNSIGN);
	bOpenReadOnly = IsDlgButtonChecked(hDlg, IDC_SETTINGS_OPENRO);
	int i = SendMessage(hCbLang, CB_GETCURSEL, 0, 0);
	if (i != -1)
	{
		DWORD itemData = (DWORD)SendMessage(hCbLang, CB_GETITEMDATA, i, 0);
		load_lang((LANGID)itemData);
	}
	save_ini_data();
	resize_window();
	return TRUE;
}

INT_PTR ViewSettingsDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hDlg);
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			if (Apply(hDlg))
			{
			case IDCANCEL:
				EndDialog(hDlg, wParam);
			}
			return TRUE;
		case MAKEWPARAM(IDC_SETTINGS_LANGUAGE, CBN_DROPDOWN):
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
	}
	return FALSE;
}
