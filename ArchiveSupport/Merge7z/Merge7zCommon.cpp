/* File:	Merge7zCommon.cpp
 * Author:	Jochen Tucht 2003/12/09
 *			Copyright (C) Jochen Tucht
 *
 * Purpose:	Provide a handy C++ interface to access 7Zip services
 *
 * Remarks:	This file contains the presumably version-independent parts of
 *			Merge7z code. Version specific code resides in Merge7zXXX.cpp.
 *
 *	*** SECURITY ALERT ***
 *	Be aware of 2. a) of the GNU General Public License. Please log your changes
 *	at the end of this comment.
 *
 * License:	This program is free software; you can redistribute it and/or modify
 *			it under the terms of the GNU General Public License as published by
 *			the Free Software Foundation; either version 2 of the License, or
 *			(at your option) any later version.
 *
 *			This program is distributed in the hope that it will be useful,
 *			but WITHOUT ANY WARRANTY; without even the implied warranty of
 *			MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *			GNU General Public License for more details.
 *
 *			You should have received a copy of the GNU General Public License
 *			along with this program; if not, write to the Free Software
 *			Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *

DATE:		BY:					DESCRIPTION:
==========	==================	================================================
2003/12/16	Jochen Tucht		GuessFormat() now checks for directory

*/

#include "stdafx.h"

#define INITGUID
#include <initguid.h>

#include "Merge7zCommon.h"

HINSTANCE g_hInstance;
DWORD g_dwFlags;
CHAR g_cPath7z[MAX_PATH];

/**
 * @brief Dll entry point
 */
BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD, LPVOID)
{
	g_hInstance = hModule;
	return TRUE;
}

/**
 * @brief Load a dll and import a number of functions.
 */
static HMODULE DllProxyHelper(LPCSTR *export, ...)
{
	HMODULE handle = 0;
	if (LPCSTR format = *export)
	{
		char path[MAX_PATH];
		FormatMessageA
		(
			FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY,
			format,
			0,
			0,
			path,
			sizeof path,
			(va_list *)(&export + 1)
		);
		handle = LoadLibraryA(path);
		if COMPLAIN(handle == 0)
		{
			ComplainNotFound(GetSystemString(path));
		}
		*export = 0;
		while (LPCSTR name = *++export)
		{
			*export = (LPCSTR)GetProcAddress(handle, name);
			if COMPLAIN(*export == 0)
			{
				ComplainNotFound(GetSystemString(name));
			}
		}
		*export = (LPCSTR)handle;
	}
	return handle;
}

/**
 * @brief Access archiver dll functions through proxy.
 */
struct Format7zDLL *Format7zDLL::Proxy::operator->()
{
	DllProxyHelper(Format7zDLL, g_cPath7z);
	return (struct Format7zDLL *)Format7zDLL;
}

/**
 * @brief Ask archiver dll for an instance of IInArchive.
 */
IInArchive *Format7zDLL::Interface::GetInArchive()
{
	void *pv;
	if COMPLAIN(proxy->CreateObject(proxy.clsid, &IID_IInArchive, &pv) != S_OK)
	{
		ComplainCreateObject(proxy.handle, _T("IInArchive"));
	}
	return static_cast<IInArchive *>(pv);
}

/**
 * @brief Ask archiver dll for an instance of IOutArchive.
 */
IOutArchive *Format7zDLL::Interface::GetOutArchive()
{
	void *pv;
	if COMPLAIN(proxy->CreateObject(proxy.clsid, &IID_IOutArchive, &pv) != S_OK)
	{
		ComplainCreateObject(proxy.handle, _T("IOutArchive"));
	}
	return static_cast<IOutArchive *>(pv);
}

/**
 * @brief Initialize the library.
 */
int Merge7z::Initialize(DWORD dwFlags)
{
	g_dwFlags = dwFlags;
	if (dwFlags & Initialize::Local7z)
	{
		GetModuleFileNameA(g_hInstance, g_cPath7z, sizeof g_cPath7z);
		PathRemoveFileSpecA(g_cPath7z);
	}
	else
	{
		DWORD type = 0;
		DWORD size = sizeof g_cPath7z;
		SHGetValueA(HKEY_LOCAL_MACHINE, "Software\\7-Zip", "Path", &type, g_cPath7z, &size);
	}
	PathAppendA(g_cPath7z, "Formats");
	return 0;
}

#define	DEFINE_FORMAT(name, dll, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
		EXTERN_C const GUID CLSID_##name \
				= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }; \
		Format7zDLL::Proxy PROXY_##name = \
		{ \
			"%1\\" dll, \
			"CreateObject", \
			(HMODULE)0, \
			&CLSID_##name \
		}; \
		Format7zDLL::Interface name = PROXY_##name;
	
/* this is how DEFINE_FORMAT expands:
DEFINE_GUID(CLSID_CFormat7z,
	0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x05, 0x00, 0x00);

Format7zDLL::Proxy PROXY_CFormat7z =
{
	"%1\\7Z.DLL",
	"CreateObject",
	(HMODULE)0,
	&CLSID_CFormat7z
};

Format7zDLL::Interface CFormat7z = PROXY_CFormat7z;/**/

DEFINE_FORMAT(CFormat7z, "7Z.DLL",
	0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x05, 0x00, 0x00);
DEFINE_FORMAT(CArjHandler, "ARJ.DLL",
	0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x0A, 0x00, 0x00);
DEFINE_FORMAT(CBZip2Handler, "BZ2.DLL",
	0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00);
DEFINE_FORMAT(CCabHandler, "CAB.DLL",
	0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x06, 0x00, 0x00);
DEFINE_FORMAT(CCpioHandler, "CPIO.DLL",
	0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x08, 0x00, 0x00);
DEFINE_FORMAT(CDebHandler, "DEB.DLL",
	0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x0C, 0x00, 0x00);
DEFINE_FORMAT(CGZipHandler, "GZ.DLL",
	0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x03, 0x00, 0x00);
DEFINE_FORMAT(CRarHandler, "RAR.DLL",
	0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x02, 0x00, 0x00);
DEFINE_FORMAT(CRpmHandler, "RPM.DLL",
	0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x09, 0x00, 0x00);
DEFINE_FORMAT(CSplitHandler, "SPLIT.DLL",
	0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x0B, 0x00, 0x00);
DEFINE_FORMAT(CTarHandler, "TAR.DLL",
	0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x04, 0x00, 0x00);
DEFINE_FORMAT(CZipHandler, "ZIP.DLL",
	0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x01, 0x00, 0x00);

/**
 * @brief Construct Merge7z interface.
 */
Merge7z::Merge7z():
Format7z(CFormat7z),
ZipHandler(CZipHandler),
RarHandler(CRarHandler),
BZip2Handler(CBZip2Handler),
TarHandler(CTarHandler)
{
}

/**
 * @brief Figure out which archiver dll to use for a given archive.
 */
Merge7z::Format *Merge7z::GuessFormat(LPCTSTR path)
{
	if (PathIsDirectory(path))
		return 0;
	ENUM_LIST
	(
		EnumList,
		_ENUM(7Z)
		ENUM(ZIP)
		ENUM(JAR)
		ENUM(XPI)
		ENUM(RAR)
		ENUM(BZ2)
		ENUM(TAR)
		ENUM(GZ)
		ENUM(TGZ)
		ENUM(CAB)
		ENUM(ARJ)
		ENUM(CPIO)
		ENUM(DEB)
		ENUM(RPM)
		_ENUM(001)
	);
	Format *pFormat = 0;
	switch (EnumList->Find(PathFindExtension(path), FALSE))
	{
	case EnumList::_7Z:
		pFormat = &CFormat7z;
		break;
	case EnumList::ZIP:
	case EnumList::JAR:
	case EnumList::XPI:
		pFormat = &CZipHandler;
		break;
	case EnumList::RAR:
		pFormat = &CRarHandler;
		break;
	case EnumList::BZ2:
		pFormat = &CBZip2Handler;
		break;
	case EnumList::TAR:
		pFormat = &CTarHandler;
		break;
	case EnumList::GZ:
	case EnumList::TGZ:
		pFormat = &CGZipHandler;
		break;
	case EnumList::CAB:
		pFormat = &CCabHandler;
		break;
	case EnumList::ARJ:
		pFormat = &CArjHandler;
		break;
	case EnumList::CPIO:
		pFormat = &CCpioHandler;
		break;
	case EnumList::DEB:
		pFormat = &CDebHandler;
		break;
	case EnumList::RPM:
		pFormat = &CRpmHandler;
		break;
	case EnumList::_001:
		pFormat = &CSplitHandler;
		break;
	}
	return pFormat;
}

/**
 * @brief Export instance of Merge7z interface.
 */
EXTERN_C
{
	__declspec(dllexport) Merge7z Merge7z;
}
