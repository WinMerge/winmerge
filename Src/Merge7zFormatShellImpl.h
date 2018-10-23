/** 
 * @file  Merge7zFormatShellImpl.h
 *
 * @brief Declaration file for Merge7zFormatShellImpl class
 *
 */
#pragma once

#include <Windows.h>

#define DllBuild_Merge7z 10 // Minimum DllBuild of Merge7z plugin required

// We include dllpstub.h for Merge7z.h
// Merge7z::Proxy embeds a DLLPSTUB
#include "dllpstub.h"
#include "../ArchiveSupport/Merge7z/Merge7z.h"
#include "UnicodeString.h"

struct Merge7zFormatShellImpl : public Merge7z::Format
{
	virtual HRESULT DeCompressArchive(HWND, LPCTSTR path, LPCTSTR folder);
	virtual HRESULT CompressArchive(HWND, LPCTSTR path, Merge7z::DirItemEnumerator *);
	virtual Inspector *Open(HWND, LPCTSTR);
	virtual Updater *Update(HWND, LPCTSTR);
	virtual HRESULT GetHandlerProperty(HWND, PROPID, PROPVARIANT *, VARTYPE);
	virtual BSTR GetHandlerName(HWND);
	virtual BSTR GetHandlerClassID(HWND);
	virtual BSTR GetHandlerExtension(HWND);
	virtual BSTR GetHandlerAddExtension(HWND);
	virtual VARIANT_BOOL GetHandlerUpdate(HWND);
	virtual VARIANT_BOOL GetHandlerKeepName(HWND);
	virtual BSTR GetDefaultName(HWND, LPCTSTR);
	static Merge7z::Format *GuessFormat(const String& path);
};
