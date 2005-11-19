/* Merge7zCommon.cpp: Provide a handy C++ interface to access 7Zip services
 * Copyright (c) 2003 Jochen Tucht
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
 * Remarks:	This file contains the presumably version-independent parts of
 *			Merge7z code. Version specific code resides in Merge7zXXX.cpp.

Please mind 2. a) of the GNU General Public License, and log your changes below.

DATE:		BY:					DESCRIPTION:
==========	==================	================================================
2003/12/09	Jochen Tucht		Created
2003/12/16	Jochen Tucht		GuessFormat() now checks for directory
2004/03/18	Jochen Tucht		Experimental DllGetVersion() based on rcsid.
2004/10/10	Jochen Tucht		DllGetVersion() based on new REVISION.TXT
2005/01/15	Jochen Tucht		Changed as explained in revision.txt
2005/02/26	Jochen Tucht		Changed as explained in revision.txt
2005/03/19	Jochen Tucht		Changed as explained in revision.txt
2005/06/22	Jochen Tucht		Treat .ear and .war like .zip
2005/07/05	Jochen Tucht		Add missing .tbz2
2005/08/20	Jochen Tucht		Option to guess archive format by signature.
								EnumerateDirectory() in EnumDirItems.cpp has
								somewhat changed so I can no longer use it.
2005/10/02	Jochen Tucht		Add CHM format
2005/11/19	Jochen Tucht		Minor changes to build against 7z430 beta
*/

#include "stdafx.h"
#include "Merge7zCommon.h"
#include "7zip/FileManager/LangUtils.h"

//#include "7zip/UI/Common/EnumDirItems.cpp" // defines static void EnumerateDirectory()
using namespace NWindows;
using namespace NFile;
using namespace NName;

void AddDirFileInfo(
	const UString &prefix, 
	const UString &fullPathName,
	NFind::CFileInfoW &fileInfo, 
	CObjectVector<CDirItem> &dirItems)
{
	CDirItem item;
	item.Attributes = fileInfo.Attributes;
	item.Size = fileInfo.Size;
	item.CreationTime = fileInfo.CreationTime;
	item.LastAccessTime = fileInfo.LastAccessTime;
	item.LastWriteTime = fileInfo.LastWriteTime;
	item.Name = prefix + fileInfo.Name;
	item.FullPath = fullPathName;
	dirItems.Add(item);
}

static void EnumerateDirectory(
	const UString &baseFolderPrefix,
	const UString &directory, 
	const UString &prefix,
	CObjectVector<CDirItem> &dirItems)
{
	NFind::CEnumeratorW enumerator(baseFolderPrefix + directory + wchar_t(kAnyStringWildcard));
	NFind::CFileInfoW fileInfo;
	while (enumerator.Next(fileInfo))
	{ 
		AddDirFileInfo(prefix, directory + fileInfo.Name, fileInfo, dirItems);
		if (fileInfo.IsDirectory())
		{
			EnumerateDirectory(baseFolderPrefix, directory + fileInfo.Name + wchar_t(kDirDelimiter),
			prefix + fileInfo.Name + wchar_t(kDirDelimiter), dirItems);
		}
	}
}

HINSTANCE g_hInstance;
#ifndef _UNICODE
bool g_IsNT = false;
static bool IsItWindowsNT()
{
  OSVERSIONINFO versionInfo;
  versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
  if (!::GetVersionEx(&versionInfo)) 
    return false;
  return (versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT);
}
#endif

DWORD g_dwFlags;
CHAR g_cPath7z[MAX_PATH];

/**
 * @brief Dll entry point
 */
BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		g_hInstance = hInstance;
#		ifndef _UNICODE
		g_IsNT = IsItWindowsNT();
#		endif
	}
	return TRUE;
}

/**
 * @brief Load a dll and import a number of functions.
 */
static HMODULE DllProxyHelper(LPCSTR *proxy, ...)
{
	HMODULE handle = NULL;
	if (LPCSTR name = *proxy)
	{
		if (proxy[1] && proxy[1] != name)
		{
			char path[MAX_PATH];
			FormatMessageA
			(
				FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ARGUMENT_ARRAY,
				name,
				0,
				0,
				path,
				sizeof path,
				(va_list *)(&proxy + 1)
			);
			handle = LoadLibraryA(path);
			if (handle)
			{
				LPCSTR *export = proxy;
				*proxy = NULL;
				while ((name = *++export) != NULL)
				{
					*export = (LPCSTR)GetProcAddress(handle, name);
					if (*export == NULL)
					{
						*proxy = proxy[1] = name;
						export = proxy + 2;
						break;
					}
				}
				*export = (LPCSTR)handle;
			}
		}
		if ((name = *proxy) != NULL)
		{
			DWORD dwError = ERROR_MOD_NOT_FOUND;
			HMODULE hContext = NULL;
			if (proxy[1] == name)
			{
				dwError = ERROR_PROC_NOT_FOUND;
				hContext = (HMODULE)proxy[2];
			}
			Complain(dwError, GetSystemString(name), hContext);
		}
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
 * @brief Ask archiver dll for an interface of given class.
 */
HRESULT Format7zDLL::Interface::CreateObject(const GUID *interfaceID, void **outObject)
{
	PROPVARIANT value;
	HRESULT result = proxy->GetHandlerProperty(NArchive::kClassID, &value);
	if SUCCEEDED(result)
	{
		if (value.vt != VT_BSTR || SysStringByteLen(value.bstrVal) != sizeof(GUID))
		{
			result = DISP_E_TYPEMISMATCH;
		}
		else
		{
			result = proxy->CreateObject((const CLSID *)value.bstrVal, interfaceID, outObject);
		}
		VariantClear((VARIANT *)&value);
	}
	return result;
}

/**
 * @brief Ask archiver dll for an instance of IInArchive.
 */
IInArchive *Format7zDLL::Interface::GetInArchive()
{
	void *pv;
	if COMPLAIN(CreateObject(&IID_IInArchive, &pv) != S_OK)
	{
		Complain(RPC_S_INTERFACE_NOT_FOUND, _T("IInArchive"), proxy.handle);
	}
	return static_cast<IInArchive *>(pv);
}

/**
 * @brief Ask archiver dll for an instance of IOutArchive.
 */
IOutArchive *Format7zDLL::Interface::GetOutArchive()
{
	void *pv;
	if COMPLAIN(CreateObject(&IID_IOutArchive, &pv) != S_OK)
	{
		Complain(RPC_S_INTERFACE_NOT_FOUND, _T("IOutArchive"), proxy.handle);
	}
	return static_cast<IOutArchive *>(pv);
}

/**
 * @brief Extraction method accessible from outside
 */
HRESULT Format7zDLL::Interface::DeCompressArchive(HWND hwndParent, LPCTSTR path, LPCTSTR folder)
{
	HRESULT result = E_FAIL;
	if (Merge7z::Format::Inspector *inspector = Open(hwndParent, path))
	{
		if (CMyComBSTR(GetHandlerAddExtension(hwndParent)).Length())
		{
			//Most handlers seem to be happy with missing index array, but rpm
			//handler doesn't know how to "extract all" and needs index array
			//even for the one and only file inside (which is .cpio.gz).
			static const UINT32 indices[1] = {0};
			result = inspector->Extract(hwndParent, folder, indices, 1);
		}
		else
		{
			result = inspector->Extract(hwndParent, folder);
		}
		inspector->Free();
	}
	return result;
}

/**
 * @brief Open archive for inspection.
 */
Merge7z::Format::Inspector *Format7zDLL::Interface::Open(HWND hwndParent, LPCTSTR path)
{
	Inspector *inspector = new Inspector(this, path);
	try
	{
		inspector->Init(hwndParent);
	}
	catch (Complain *complain)
	{
		complain->Alert(hwndParent);
		inspector->Free();
		inspector = 0;
	}
	return inspector;
}

/**
 * @brief Prepare inspection interface for iteration.
 */
UINT32 Format7zDLL::Interface::Inspector::Open()
{
	UINT32 numItems = 0;
	archive->GetNumberOfItems(&numItems);
	return numItems;
}

/**
 * @brief free inspection interface.
 */
void Format7zDLL::Interface::Inspector::Free()
{
	Release(archive);
	Release(static_cast<IInStream *>(file));
	Release(callback);
	delete this;
}

HRESULT Format7zDLL::Interface::Inspector::GetProperty(UINT32 index, PROPID propID, PROPVARIANT *value, VARTYPE vt)
{
	VariantInit((VARIANT *)value);
	HRESULT result = archive->GetProperty(index, propID, value);
	if (SUCCEEDED(result) && value->vt != vt)
	{
		VariantClear((VARIANT *)value);
		result = DISP_E_TYPEMISMATCH;
	}
	return result;
}

BSTR Format7zDLL::Interface::Inspector::GetPath(UINT32 index)
{
	PROPVARIANT value;
	return SUCCEEDED(GetProperty(index, kpidPath, &value, VT_BSTR)) ? value.bstrVal : 0;
}

BSTR Format7zDLL::Interface::Inspector::GetName(UINT32 index)
{
	PROPVARIANT value;
	return SUCCEEDED(GetProperty(index, kpidName, &value, VT_BSTR)) ? value.bstrVal : 0;
}

BSTR Format7zDLL::Interface::Inspector::GetExtension(UINT32 index)
{
	PROPVARIANT value;
	return SUCCEEDED(GetProperty(index, kpidExtension, &value, VT_BSTR)) ? value.bstrVal : 0;
}

VARIANT_BOOL Format7zDLL::Interface::Inspector::IsFolder(UINT32 index)
{
	PROPVARIANT value;
	return SUCCEEDED(GetProperty(index, kpidIsFolder, &value, VT_BOOL)) ? value.boolVal : 0;
}

FILETIME Format7zDLL::Interface::Inspector::LastWriteTime(UINT32 index)
{
	static const FILETIME invalid = { 0, 0 };
	PROPVARIANT value;
	return SUCCEEDED(GetProperty(index, kpidLastWriteTime, &value, VT_FILETIME)) ? value.filetime : invalid;
}

void Format7zDLL::Interface::GetDefaultName(HWND hwndParent, UString &ustrDefaultName)
{
	int dot = ustrDefaultName.ReverseFind('.');
	int slash = ustrDefaultName.ReverseFind('\\');
	if (dot > slash)
	{
		LPCWSTR pchExtension = ustrDefaultName;
		pchExtension += dot + 1;
		static const OLECHAR wBlank[] = L" ";
		CMyComBSTR bstrHandlerExtension = GetHandlerExtension(hwndParent);
		CMyComBSTR bstrHandlerAddExtension = GetHandlerAddExtension(hwndParent);
		LPWSTR pchHandlerExtension = bstrHandlerExtension.m_str;
		LPWSTR pchHandlerAddExtension = bstrHandlerAddExtension.m_str;
		while (int cchHandlerAddExtension = StrCSpnW(pchHandlerAddExtension += StrSpnW(pchHandlerAddExtension, wBlank), wBlank))
		{
			int cchHandlerExtension = StrCSpnW(pchHandlerExtension += StrSpnW(pchHandlerExtension, wBlank), wBlank);
			if (StrIsIntlEqualW(FALSE, pchExtension, pchHandlerExtension, cchHandlerExtension) && pchExtension[cchHandlerExtension] == 0)
			{
				pchHandlerAddExtension[cchHandlerAddExtension] = '\0'; // will also stop iteration
				ustrDefaultName.ReleaseBuffer(dot);
				if (*pchHandlerAddExtension == '.') // consider != '*'
				{
					ustrDefaultName += pchHandlerAddExtension;
					dot += cchHandlerAddExtension; // make ReleaseBuffer(dot) below a NOP
				}
			}
			pchHandlerExtension += cchHandlerExtension;
			pchHandlerAddExtension += cchHandlerAddExtension;
		}
		ustrDefaultName.ReleaseBuffer(dot);
		ustrDefaultName.Delete(0, slash + 1);
	}
	else
	{
		ustrDefaultName = L"noname";
	}
}

BSTR Format7zDLL::Interface::GetDefaultName(HWND hwndParent, LPCTSTR path)
{
	UString ustrDefaultName = GetUnicodeString(path);
	GetDefaultName(hwndParent, ustrDefaultName);
	return SysAllocString(ustrDefaultName);
}

BSTR Format7zDLL::Interface::Inspector::GetDefaultName()
{
	//UString ustrDefaultName = GetUnicodeString(path);
	return SysAllocString(ustrDefaultName);
}

/**
 * @brief Open archive for update.
 */
Merge7z::Format::Updater *Format7zDLL::Interface::Update(HWND hwndParent, LPCTSTR path)
{
	Updater *updater = new Updater(this, path);
	try
	{
		updater->Init(hwndParent);
	}
	catch (Complain *complain)
	{
		complain->Alert(hwndParent);
		updater->Free();
		updater = 0;
	}
	return updater;
}

/**
 * @brief Add item to updater
 */
UINT32 Format7zDLL::Interface::Updater::Add(Merge7z::DirItemEnumerator::Item &etorItem)
{
	// fill in the default values from the enumerator
	CDirItem item;
	if (etorItem.Mask.Item & etorItem.Mask.Name)
		item.Name = GetUnicodeString(etorItem.Name);
	if (etorItem.Mask.Item & etorItem.Mask.FullPath)
		item.FullPath = GetUnicodeString(etorItem.FullPath);
	if (etorItem.Mask.Item & etorItem.Mask.Attributes)
		item.Attributes = etorItem.Attributes;
	if (etorItem.Mask.Item & etorItem.Mask.Size)
		item.Size = etorItem.Size;
	if (etorItem.Mask.Item & etorItem.Mask.CreationTime)
		item.CreationTime = etorItem.CreationTime;
	if (etorItem.Mask.Item & etorItem.Mask.LastAccessTime)
		item.LastAccessTime = etorItem.LastAccessTime;
	if (etorItem.Mask.Item & etorItem.Mask.LastWriteTime)
		item.LastWriteTime = etorItem.LastWriteTime;
	if (etorItem.Mask.Item && (etorItem.Mask.Item & (etorItem.Mask.NeedFindFile|etorItem.Mask.CheckIfPresent)) != etorItem.Mask.NeedFindFile)
	{
		// Check the info from the disk
		NFile::NFind::CFileInfoW fileInfo;
		if (NFile::NFind::FindFile(item.FullPath, fileInfo))
		{
			if (!(etorItem.Mask.Item & etorItem.Mask.Name))
				item.Name = fileInfo.Name;
			if (!(etorItem.Mask.Item & etorItem.Mask.Attributes))
				item.Attributes = fileInfo.Attributes;
			if (!(etorItem.Mask.Item & etorItem.Mask.Size))
				item.Size = fileInfo.Size;
			if (!(etorItem.Mask.Item & etorItem.Mask.CreationTime))
				item.CreationTime = fileInfo.CreationTime;
			if (!(etorItem.Mask.Item & etorItem.Mask.LastAccessTime))
				item.LastAccessTime = fileInfo.LastAccessTime;
			if (!(etorItem.Mask.Item & etorItem.Mask.LastWriteTime))
				item.LastWriteTime = fileInfo.LastWriteTime;
		}
		else
		{
			// file not valid, forget it
			etorItem.Mask.Item = 0;
		}
	}
	if (etorItem.Mask.Item)
	{
		// No check from disk, simply use info from enumerators (risky)
		// Why risky? This is not at all obvious.
		dirItems.Add(item);
		// Recurse into directories (call a function of 7zip)
		if ((etorItem.Mask.Item & etorItem.Mask.Recurse) && (item.Attributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			EnumerateDirectory(UString(), item.FullPath + L'\\',
					item.Name + L'\\', dirItems);
		}
	}
	return etorItem.Mask.Item;
}

/**
 * @brief free updater interface.
 */
void Format7zDLL::Interface::Updater::Free()
{
	Release(outArchive);
	Release(static_cast<IOutStream *>(file));
	delete this;
}

/**
 * @brief Compression method accessible from outside
 *
 * @note See CAgent::DoOperation (in 7zip source) for model
 */
HRESULT Format7zDLL::Interface::CompressArchive(HWND hwndParent, LPCTSTR path, Merge7z::DirItemEnumerator *etor)
{
	HRESULT result = E_FAIL;
	if (Merge7z::Format::Updater *updater = Update(hwndParent, path))
	{
		UINT count = etor->Open();
		while (count--)
		{
			Merge7z::DirItemEnumerator::Item etorItem;
			etorItem.Mask.Item = 0;
			Merge7z::Envelope *envelope = etor->Enum(etorItem);
			updater->Add(etorItem);
			if (envelope)
			{
				envelope->Free();
			}
		}
		result = updater->Commit(hwndParent);
		updater->Free();
	}
	return result;
}

/**
 * @brief get handler property identified by given propID
 */
HRESULT Format7zDLL::Interface::GetHandlerProperty(HWND hwndParent, PROPID propID, PROPVARIANT *value, VARTYPE vt)
{
	VariantInit((VARIANT *)value);
	HRESULT result = DISP_E_EXCEPTION;
	try
	{
		result = proxy->GetHandlerProperty(propID, value);
		if (SUCCEEDED(result) && value->vt != vt)
		{
			VariantClear((VARIANT *)value);
			result = DISP_E_TYPEMISMATCH;
		}
	}
	catch (Complain *complain)
	{
		complain->Alert(hwndParent);
	}
	return result;
}

/**
 * @brief get Name handler property
 */
BSTR Format7zDLL::Interface::GetHandlerName(HWND hwndParent)
{
	PROPVARIANT value;
	return SUCCEEDED(GetHandlerProperty(hwndParent, NArchive::kName, &value, VT_BSTR)) ? value.bstrVal : 0;
}

/**
 * @brief get ClassID handler property
 */
BSTR Format7zDLL::Interface::GetHandlerClassID(HWND hwndParent)
{
	PROPVARIANT value;
	return SUCCEEDED(GetHandlerProperty(hwndParent, NArchive::kClassID, &value, VT_BSTR)) ? value.bstrVal : 0;
}

/**
 * @brief get Extension handler property
 */
BSTR Format7zDLL::Interface::GetHandlerExtension(HWND hwndParent)
{
	PROPVARIANT value;
	return SUCCEEDED(GetHandlerProperty(hwndParent, NArchive::kExtension, &value, VT_BSTR)) ? value.bstrVal : 0;
}

/**
 * @brief get AddExtension handler property
 */
BSTR Format7zDLL::Interface::GetHandlerAddExtension(HWND hwndParent)
{
	PROPVARIANT value;
	return SUCCEEDED(GetHandlerProperty(hwndParent, NArchive::kAddExtension, &value, VT_BSTR)) ? value.bstrVal : 0;
}

/**
 * @brief get Update handler property
 */
VARIANT_BOOL Format7zDLL::Interface::GetHandlerUpdate(HWND hwndParent)
{
	PROPVARIANT value;
	return SUCCEEDED(GetHandlerProperty(hwndParent, NArchive::kUpdate, &value, VT_BOOL)) ? value.boolVal : 0;
}

/**
 * @brief get KeepName handler property
 */
VARIANT_BOOL Format7zDLL::Interface::GetHandlerKeepName(HWND hwndParent)
{
	PROPVARIANT value;
	return SUCCEEDED(GetHandlerProperty(hwndParent, NArchive::kKeepName, &value, VT_BOOL)) ? value.boolVal : 0;
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
	PathAddBackslashA(g_cPath7z);
	if (WORD wLangID = HIWORD(dwFlags))
	{
		LoadLang(MAKEINTATOM(wLangID));
	}
	return 0;
}

static const char aCreateObject[] = "CreateObject";
static const char aGetHandlerProperty[] = "GetHandlerProperty";

Format7zDLL::Interface *Format7zDLL::Interface::head = NULL;

#define	DEFINE_FORMAT(name, dll, extension, signature) \
		Format7zDLL::Proxy PROXY_##name = \
		{ \
			"%1Formats\\" dll, \
			aCreateObject, \
			aGetHandlerProperty, \
			(HMODULE)0, \
			signature extension + sizeof signature extension - sizeof extension, \
			sizeof signature extension - sizeof extension \
		}; \
		Format7zDLL::Interface name = PROXY_##name;

DEFINE_FORMAT(CFormat7z,		"7Z.DLL",		"7z", "@7z\xBC\xAF\x27\x1C");
DEFINE_FORMAT(CArjHandler,		"ARJ.DLL",		"arj", "@\x60\xEA");
DEFINE_FORMAT(CBZip2Handler,	"BZ2.DLL",		"bz2 tbz2", "@BZh");
DEFINE_FORMAT(CCabHandler,		"CAB.DLL",		"cab", "@MSCF");
DEFINE_FORMAT(CCpioHandler,		"CPIO.DLL",		"cpio", "");
DEFINE_FORMAT(CDebHandler,		"DEB.DLL",		"deb", "@!<arch>\n");
DEFINE_FORMAT(CLzhHandler,		"LZH.DLL",		"lzh lha", "@@@-l@@-");//"@-l" doesn't work because signature starts at offset 2
DEFINE_FORMAT(CGZipHandler,		"GZ.DLL",		"gz tgz", "@\x1F\x8B");
DEFINE_FORMAT(CRarHandler,		"RAR.DLL",		"rar", "@Rar!\x1a\x07\x00");
DEFINE_FORMAT(CRpmHandler,		"RPM.DLL",		"rpm", "");
DEFINE_FORMAT(CSplitHandler,	"SPLIT.DLL",	"001", "");
DEFINE_FORMAT(CTarHandler,		"TAR.DLL",		"tar", "");
DEFINE_FORMAT(CZHandler,		"Z.DLL",		"z", "@\x1F\x9D");
DEFINE_FORMAT(CZipHandler,		"ZIP.DLL",		"zip jar war ear xpi", "@PK\x03\x04");
DEFINE_FORMAT(CChmHandler,		"CHM.DLL",		"chm chi chq chw hxs hxi hxr hxq hxw lit", "@ITSF");

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
	if (g_dwFlags & Initialize::GuessFormatBySignature)
		return GuessFormatBySignature(path, g_dwFlags & Initialize::GuessFormatByExtension ? path : 0);
	return GuessFormatByExtension(path);
}

/**
 * @brief Figure out which archiver dll to use for a given archive.
 */
Merge7z::Format *Merge7z::GuessFormatByExtension(LPCTSTR path)
{
	SZ_EXTENSION ext;
	if (PathIsDirectory(path))
		return 0;
	return GuessFormatEx(GetExtension(path, ext), 0, 0);
}

/**
 * @brief Figure out which archiver dll to use for a given archive.
 */
Merge7z::Format *Merge7z::GuessFormatBySignature(LPCTSTR path, LPCTSTR extension)
{
	SZ_EXTENSION ext;
	CH_SIGNATURE sig;
	if (PathIsDirectory(path))
		return 0;
	return GuessFormatEx(GetExtension(extension, ext), sig, GetSignature(path, sig));
}

/**
 * @brief Figure out which archiver dll to use for a given archive.
 */
Merge7z::Format *Merge7z::GuessFormatEx(LPCSTR ext, LPCH sig, int cchSig)
{
	Format7zDLL::Interface *pFormat = Format7zDLL::Interface::head;
	Format7zDLL::Interface *pFormatByExtension = 0;
	while (pFormat)
	{
		static const char aBlank[] = " ";
		LPCSTR pchExtension = pFormat->proxy.extension;
		int cchExtension = pFormat->proxy.signature;
		if (cchSig > 0 && cchExtension)
		{
			LPCSTR pchSignature = pchExtension - cchExtension;
			char joker = *pchSignature++;
			--cchExtension;
			if (cchSig >= cchExtension)
			{
				while (cchExtension--)
				{
					char expected = pchSignature[cchExtension];
					if (expected != joker && sig[cchExtension] != expected)
						break;
				}
				if (cchExtension == -1)
					return pFormat;
			}
		}
		else while
		(
			ext
		&&	pFormatByExtension == 0
		&&	(cchExtension = StrCSpnA(pchExtension += StrSpnA(pchExtension, aBlank), aBlank)) != 0
		)
		{
			if (StrIsIntlEqualA(FALSE, pchExtension, ext, cchExtension) && ext[cchExtension] == '\0')
			{
				pFormatByExtension = pFormat;
			}
			pchExtension += cchExtension;
		}
		pFormat = pFormat->next;
	}
	return pFormat ? pFormat : pFormatByExtension;
}

/**
 * @brief Get filename extension as ANSI characters.
 */
LPCSTR Merge7z::GetExtension(LPCTSTR path, SZ_EXTENSION ext)
{
	if (path == NULL)
		return NULL;
	path = PathFindExtension(path);
#ifdef UNICODE
	return WideCharToMultiByte(CP_ACP, 0, path, -1, ext, sizeof(SZ_EXTENSION), 0, 0) > 1 ? ext + 1 : 0;
#else
	ext[sizeof(SZ_EXTENSION) - 2] = '\0';
	lstrcpynA(ext, path, sizeof(SZ_EXTENSION));
	return ext[0] != '\0' && ext[sizeof(SZ_EXTENSION) - 2] == '\0' ? ext + 1 : 0;
#endif
}

/**
 * @brief Read start signature from given file.
 */
DWORD Merge7z::GetSignature(LPCTSTR path, CH_SIGNATURE sig)
{
	if (sig == NULL)
		return sizeof(CH_SIGNATURE);
	DWORD cchSig = 0;
	HANDLE h = CreateFile(path, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (h != INVALID_HANDLE_VALUE)
	{
		ReadFile(h, sig, sizeof(CH_SIGNATURE), &cchSig, 0);
		if (cchSig >= 64 && MAKEWORD(sig[0],sig[1]) == IMAGE_DOS_SIGNATURE)
		{
			DWORD offset = 
			(
				LPDWORD(sig)[5]	//DOS CS:IP
			?	512UL * (LPWORD(sig)[1] ? LPWORD(sig)[2] - 1 : LPWORD(sig)[2]) + LPWORD(sig)[1]
			:	LPDWORD(sig)[15]
			);
			if (SetFilePointer(h, offset, 0, FILE_BEGIN) == offset)
			{
				ReadFile(h, sig, sizeof(CH_SIGNATURE), &cchSig, 0);
				if (cchSig >= 4 + sizeof(IMAGE_FILE_HEADER) && MAKELONG(MAKEWORD(sig[0],sig[1]), MAKEWORD(sig[2],sig[3])) == MAKELONG(MAKEWORD('P','E'), 0))
				{
					cchSig = 0;
					IMAGE_FILE_HEADER *pImageFileHeader = (IMAGE_FILE_HEADER *) (sig + 4);
					offset += 4 + sizeof(IMAGE_FILE_HEADER) + pImageFileHeader->SizeOfOptionalHeader;
					if (SetFilePointer(h, offset, 0, FILE_BEGIN) == offset)
					{
						int iSection = pImageFileHeader->NumberOfSections;
						while (iSection--)
						{
							IMAGE_SECTION_HEADER ImageSectionHeader;
							DWORD cbImageSectionHeader = 0;
							ReadFile(h, &ImageSectionHeader, sizeof ImageSectionHeader, &cbImageSectionHeader, 0);
							if (cbImageSectionHeader != sizeof ImageSectionHeader)
								break;
							if (memcmp(ImageSectionHeader.Name, "_winzip_", 8) == 0)
							{
								// looks like WinZip Self-Extractor
								memcpy(sig, "PK\x03\x04", cchSig = 4);
								break;
							}
							DWORD ahead = ImageSectionHeader.PointerToRawData + ImageSectionHeader.SizeOfRawData;
							if (offset < ahead)
								offset = ahead;
						}
						if (iSection == -1 && SetFilePointer(h, offset, 0, FILE_BEGIN) == offset)
						{
							ReadFile(h, sig, sizeof(CH_SIGNATURE), &cchSig, 0);
						}
					}
				}
			}
		}
		CloseHandle(h);
	}
	return cchSig;
}

/**
 * @brief Load language file for 7-Zip UIs.
 */

static CSysString g_LangPath;

LPCTSTR Merge7z::LoadLang(LPCTSTR langFile)
{
	TCHAR langFileGuess[8];
	if (HIWORD(langFile) == 0)
	{
		langFileGuess[0] = '\0';
		if (int cchLng = GetLocaleInfo(LCID(langFile), LOCALE_SISO639LANGNAME, langFileGuess, 4))
		{
			langFileGuess[cchLng - 1] = '-';
			GetLocaleInfo(LCID(langFile), LOCALE_SISO3166CTRYNAME, langFileGuess + cchLng, 4);
			langFile = langFileGuess;
		}
	}
	g_LangPath = GetSystemString(g_cPath7z);
	g_LangPath += TEXT("Lang\\");
	g_LangPath += langFile;
	int slash = g_LangPath.ReverseFind('\\');
	int minus = g_LangPath.ReverseFind('-');
	int dot = g_LangPath.ReverseFind('.');
	if (dot <= slash)
	{
		dot = g_LangPath.Length();
		g_LangPath += TEXT(".txt");
	}
	if (minus > slash && !PathFileExists(g_LangPath))
	{
		// 2nd chance: filename == language code
		CSysString Region = g_LangPath.Mid(minus, dot - minus);
		Region.Replace('-', '\\');
		g_LangPath.Delete(minus, dot - minus);
		if (!PathFileExists(g_LangPath))
		{
			// 3rd chance: filename == region code (Norwegian)
			g_LangPath.Delete(slash, minus - slash);
			g_LangPath.Insert(slash, Region);
		}
	}
	ReloadLang();
	return g_LangPath;
}

/**
 * @brief Override ReadRegLang to return path set by Merge7z::LoadLang().
 * This is global 7-Zip function otherwise defined in RegistryUtils.cpp.
 * Exclude RegistryUtils.cpp from build to avoid link-time collision.
 */
void ReadRegLang(CSysString &langFile)
{
	langFile = g_LangPath;
}

/**
 * @brief 7-Zip 4.26: ReloadLangSmart() wants this #ifdef _UNICODE.
 * We certainly don't want to write 7-Zip's registry so we make it a NOP.
 */
void SaveRegLang(const CSysString &langFile)
{
}

#ifndef _UNICODE
void ReadRegLang(UString &langFile)
{
	langFile = GetUnicodeString(g_LangPath);
}

void SaveRegLang(const UString &langFile)
{
}
#endif

/**
 * @brief Export instance of Merge7z interface.
 */
EXTERN_C
{
	__declspec(dllexport) Merge7z Merge7z;
}

EXTERN_C HRESULT CALLBACK DllGetVersion(DLLVERSIONINFO *pdvi)
{
	// Compute dwBuild from revision.txt
	static const DWORD dwBuild =
	(
		sizeof""
#		define VERSION(MAJOR,MINOR)
#		include "revision.txt"
#		undef VERSION
	);
	C_ASSERT(dwBuild == DllBuild_Merge7z);
	// Compute dwVersion from revision.txt
	static const DWORD dwVersion =
	(
		0*sizeof""
#		define VERSION(MAJOR,MINOR) , MAKELONG(MINOR,MAJOR) + 0*sizeof""
#		include "revision.txt"
#		undef VERSION
	);
	static const DLLVERSIONINFO dvi =
	{
		sizeof dvi,
		HIWORD(dwVersion), LOWORD(dwVersion), dwBuild, DLLVER_PLATFORM_WINDOWS
	};
	CopyMemory(pdvi, &dvi, pdvi->cbSize < dvi.cbSize ? pdvi->cbSize : dvi.cbSize);
	return S_OK;
}
