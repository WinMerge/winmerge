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
 * @file  LangArray.h
 *
 * @brief Declaration of translated text array class.
 *
 */
#include "Simparr.h"

class LangArray : public SimpleArray<char *>
{
public:
	static const LANGID DefLangId;
	struct StringData
	{
		int refcount;
		char data[1];
		static StringData *Create(const char *, size_t);
		char *Share();
		static void Unshare(char *);
	};
	HMODULE m_hLangDll;
	unsigned m_codepage;
	LANGID m_langid;
	LangArray();
	~LangArray();
	void ExpandToSize();
	void ClearAll();
	BOOL Load(HINSTANCE hMainInstance, LANGID langid, LPCTSTR langdir);
	PTSTR TranslateString(int line);
	void TranslateDialog(HWND);
	void TranslateMenu(HMENU);
	HMENU LoadMenu(HINSTANCE, LPTSTR);
	static int LangCodeMajor(LANGID, LPTSTR);
	static int LangCodeMinor(LANGID, LPTSTR);
};
