// WildcardDropList.h
// Copyright (c) datadiode
// SPDX-License-Identifier: WTFPL

class WildcardDropList
{
private:
	static LRESULT CALLBACK LbWndProc(HWND, UINT, WPARAM, LPARAM);
public:
	static void OnDropDown(HWND, int columns, LPCTSTR);
	static void OnCloseUp(HWND);
};
