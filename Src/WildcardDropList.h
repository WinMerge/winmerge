// WildcardDropList.h
// Copyright (c) datadiode
// SPDX-License-Identifier: WTFPL

class WildcardDropList
{
private:
	static LRESULT CALLBACK LbWndProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK LvWndProc(HWND, UINT, WPARAM, LPARAM);
public:
	static void OnDropDown(HWND, int columns, LPCTSTR, bool allowUserAddedPatterns = true);
	static bool OnCloseUp(HWND);
	static void OnItemActivate(HWND hLv, int iItem, int iSubItem, int columns, LPCTSTR fixedPatterns, bool allowUserAddedPatterns = true, int limitTextSize = 0);
};
