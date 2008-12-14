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
 * @file  LangArray.h
 *
 * @brief Declaration of translated text array class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: LangArray.h 127 2008-11-09 14:09:53Z jtuc $

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
	BOOL Load(HINSTANCE, LANGID);
	PTSTR TranslateString(int line);
	void TranslateDialog(HWND);
	void TranslateMenu(HMENU);
	HMENU LoadMenu(HINSTANCE, LPTSTR);
	static int LangCodeMajor(LANGID, LPTSTR);
	static int LangCodeMinor(LANGID, LPTSTR);
};
