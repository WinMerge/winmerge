/** 
 * @file  Satellites.h
 *
 * @date  Created: 2004-06-06 (Perry)
 * @date  Edited:  2004-06-06 (Perry)
 * @brief Code to load list of satellite resource dlls
 */

#ifndef Satellites_h_included
#define Satellites_h_included

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif

struct LangDll 
{
	WORD m_lang; 
	CString dllpath; 
	LangDll(WORD lang, LPCTSTR path) : m_lang(lang), dllpath(path) { }
	LangDll() : m_lang(0) { }
};

typedef CArray<LangDll, LangDll> LangDllArray;

void Sats_LoadList(const CString& sDir, const CString & PathSpec, LangDllArray & langdlls);
bool Sats_GetSatLang(const CString& dll, WORD & lang);



#endif // Satellites_h_included
