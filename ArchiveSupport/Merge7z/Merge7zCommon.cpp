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
2003-12-09	Jochen Tucht		Created
2003-12-16	Jochen Tucht		GuessFormat() now checks for directory
2004-03-18	Jochen Tucht		Experimental DllGetVersion() based on rcsid.
2004-10-10	Jochen Tucht		DllGetVersion() based on new REVISION.TXT
2005-01-15	Jochen Tucht		Changed as explained in revision.txt
2005-02-26	Jochen Tucht		Changed as explained in revision.txt
2005-03-19	Jochen Tucht		Changed as explained in revision.txt
2005-06-22	Jochen Tucht		Treat .ear and .war like .zip
2005-07-05	Jochen Tucht		Add missing .tbz2
2005-08-20	Jochen Tucht		Option to guess archive format by signature.
								EnumerateDirectory() in EnumDirItems.cpp has
								somewhat changed so I can no longer use it.
2005-10-02	Jochen Tucht		Add CHM format
2005-11-19	Jochen Tucht		Minor changes to build against 7z430 beta
2006-06-28	Jochen Neubeck		Add ISO format (introduced with 7z436 beta)
								Add NSIS format (introduced with 7z440 beta)
2007-01-27	Jochen Neubeck		Unassociate .exe filename extension from NSIS
								format due to undesired side effect on WinMerge
2007-04-20	Jochen Neubeck		Cope with 7z445's revised plugin system
2007-07-13	Jochen Neubeck		Pass MSI files to CAB handler
2007-08-25	Jochen Neubeck		Add COM format (introduced with 7z452 beta)
								This format also handles MSI files, which are
								therefore no longer passed to the CAB handler.
2007-09-01	Jochen Neubeck		No longer #include "LangUtils.h", which has
								moved to a different location as of 7z453 beta.
2007-12-22	Jochen Neubeck		Unassociate .001 filename extension
2008-08-03	Jochen Neubeck		Add LZMA format (introduced with 7z458 beta)
2010-04-24	Jochen Neubeck		New formats introduced with 7z459 beta:
								XAR, MUB, HFS, DMG, ELF (not sure if they work)
*/

#include "stdafx.h"
#include "Merge7zCommon.h"

bool g_DisableUserQuestions;

void ReloadLang(); //Implemented in /*/FileManager/LangUtils.cpp

using namespace NWindows;
using namespace NFile;

void AddDirFileInfo(
	const UString &prefix, 
	const UString &fullPathName,
	NFind::CFileInfo &fileInfo, 
	CDirItems &dirItems)
{
	CDirItem item;
	item.Attrib = fileInfo.Attrib;
	item.Size = fileInfo.Size;
	item.CTime = fileInfo.CTime;
	item.ATime = fileInfo.ATime;
	item.MTime = fileInfo.MTime;
	//UString
	item.LogParent = dirItems.AddPrefix(-1, -1, prefix + fileInfo.Name);
	item.PhyParent = dirItems.AddPrefix(-1, -1, fullPathName);
	//item.Name = prefix + fileInfo.Name;
	//item.FullPath = fullPathName;
	dirItems.Items.Add(item);
}

static void EnumerateDirectory(
	const UString &baseFolderPrefix,
	const UString &directory, 
	const UString &prefix,
	CDirItems &dirItems)
{
	NFind::CEnumerator enumerator;
	enumerator.SetDirPrefix(baseFolderPrefix + directory);
	NFind::CFileInfo fileInfo;
	while (enumerator.Next(fileInfo))
	{ 
		AddDirFileInfo(prefix, directory + fileInfo.Name, fileInfo, dirItems);
		if (fileInfo.IsDir())
		{
			EnumerateDirectory(baseFolderPrefix, directory + fileInfo.Name + WCHAR_PATH_SEPARATOR,
			prefix + fileInfo.Name + WCHAR_PATH_SEPARATOR, dirItems);
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
static HMODULE DllProxyHelper(LPCSTR *proxy, LPCSTR dir)
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
				(va_list *)(&dir)
			);
			handle = LoadLibraryA(path);
			if (handle)
			{
				LPCSTR *p = proxy;
				*proxy = NULL;
				while ((name = *++p) != NULL)
				{
					*p = (LPCSTR)GetProcAddress(handle, name);
					if (*p == NULL)
					{
						*proxy = proxy[1] = name;
						p = proxy + 2;
						break;
					}
				}
				*p = (LPCSTR)handle;
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
 * @brief Ask archiver dll for an interface of given class.
 */
HRESULT Format7zDLL::Interface::CreateObject(const GUID *interfaceID, void **outObject)
{
	PROPVARIANT value;
	PropVariantInit(&value);
	HRESULT result = proxy->GetHandlerProperty(NArchive::NHandlerPropID::kClassID, &value);
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
		if (AutoBSTR(GetHandlerAddExtension(hwndParent)).Len())
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
	Inspector* inspector = nullptr;
	try
	{
		Interface* pFormat = this;
		while (pFormat)
		{
			inspector = new Inspector(pFormat, path);
			try
			{
				inspector->Init(hwndParent);
				break;
			}
			catch (Complain* complain)
			{
				inspector->Free();
				inspector = 0;
				pFormat = pFormat->next;
				if (!pFormat)
					throw complain;
			}
		}
	}
	catch (Complain *complain)
	{
		complain->Alert(hwndParent);
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
	return SUCCEEDED(GetProperty(index, kpidIsDir, &value, VT_BOOL)) ? value.boolVal : 0;
}

FILETIME Format7zDLL::Interface::Inspector::LastWriteTime(UINT32 index)
{
	static const FILETIME invalid = { 0, 0 };
	PROPVARIANT value;
	return SUCCEEDED(GetProperty(index, kpidMTime, &value, VT_FILETIME)) ? value.filetime : invalid;
}

void Format7zDLL::Interface::GetDefaultName(HWND hwndParent, UString &ustrDefaultName)
{
	int dot = ustrDefaultName.ReverseFind(L'.');
	int slash = ustrDefaultName.ReverseFind(L'\\');
	if (dot > slash)
	{
		LPCWSTR pchExtension = ustrDefaultName;
		pchExtension += dot + 1;
		static OLECHAR const wBlank[] = L" ";
		AutoBSTR const bstrHandlerExtension = GetHandlerExtension(hwndParent);
		AutoBSTR const bstrHandlerAddExtension = GetHandlerAddExtension(hwndParent);
		LPWSTR pchHandlerExtension = bstrHandlerExtension.m_str;
		LPWSTR pchHandlerAddExtension = bstrHandlerAddExtension.m_str;
		while (int cchHandlerAddExtension = StrCSpnW(pchHandlerAddExtension += StrSpnW(pchHandlerAddExtension, wBlank), wBlank))
		{
			int cchHandlerExtension = StrCSpnW(pchHandlerExtension += StrSpnW(pchHandlerExtension, wBlank), wBlank);
			if (StrIsIntlEqualW(FALSE, pchExtension, pchHandlerExtension, cchHandlerExtension) && pchExtension[cchHandlerExtension] == 0)
			{
				pchHandlerAddExtension[cchHandlerAddExtension] = L'\0'; // will also stop iteration
				ustrDefaultName.ReleaseBuf_SetEnd(dot);
				if (*pchHandlerAddExtension == L'.') // consider != '*'
				{
					ustrDefaultName += pchHandlerAddExtension;
					dot += cchHandlerAddExtension; // make ReleaseBuffer(dot) below a NOP
				}
			}
			pchHandlerExtension += cchHandlerExtension;
			pchHandlerAddExtension += cchHandlerAddExtension;
		}
		ustrDefaultName.ReleaseBuf_SetEnd(dot);
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
	UString Name = GetUnicodeString(etorItem.Name);
	UString FullPath = GetUnicodeString(etorItem.FullPath);
	if (etorItem.Mask.Item & etorItem.Mask.Name)
		//item.Name = GetUnicodeString(etorItem.Name);
		item.LogParent = dirItems.AddPrefix(-1, -1, Name);
	if (etorItem.Mask.Item & etorItem.Mask.FullPath)
		//item.FullPath = GetUnicodeString(etorItem.FullPath);
		item.PhyParent = dirItems.AddPrefix(-1, -1, FullPath);
	if (etorItem.Mask.Item & etorItem.Mask.Attributes)
		item.Attrib = etorItem.Attributes;
	if (etorItem.Mask.Item & etorItem.Mask.Size)
		item.Size = etorItem.Size;
	if (etorItem.Mask.Item & etorItem.Mask.CreationTime)
		item.CTime = etorItem.CreationTime;
	if (etorItem.Mask.Item & etorItem.Mask.LastAccessTime)
		item.ATime = etorItem.LastAccessTime;
	if (etorItem.Mask.Item & etorItem.Mask.LastWriteTime)
		item.MTime = etorItem.LastWriteTime;
	if (etorItem.Mask.Item && (etorItem.Mask.Item & (etorItem.Mask.NeedFindFile|etorItem.Mask.CheckIfPresent)) != etorItem.Mask.NeedFindFile)
	{
		// Check the info from the disk
		NFile::NFind::CFileInfo fileInfo;
		if (NFile::NFind::CFindFile().FindFirst(FullPath, fileInfo))
		{
			if (!(etorItem.Mask.Item & etorItem.Mask.Name))
				item.Name = fileInfo.Name;
			if (!(etorItem.Mask.Item & etorItem.Mask.Attributes))
				item.Attrib = fileInfo.Attrib;
			if (!(etorItem.Mask.Item & etorItem.Mask.Size))
				item.Size = fileInfo.Size;
			if (!(etorItem.Mask.Item & etorItem.Mask.CreationTime))
				item.CTime = fileInfo.CTime;
			if (!(etorItem.Mask.Item & etorItem.Mask.LastAccessTime))
				item.ATime = fileInfo.ATime;
			if (!(etorItem.Mask.Item & etorItem.Mask.LastWriteTime))
				item.MTime = fileInfo.MTime;
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
		dirItems.Items.Add(item);
		// Recurse into directories (call a function of 7zip)
		if ((etorItem.Mask.Item & etorItem.Mask.Recurse) && (item.Attrib & FILE_ATTRIBUTE_DIRECTORY))
		{
			EnumerateDirectory(UString(), FullPath + L'\\',
					Name + L'\\', dirItems);
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
			if (etorItem.Mask.Item != 0)
			{
				updater->Add(etorItem);
			}
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
	return SUCCEEDED(GetHandlerProperty(hwndParent, NArchive::NHandlerPropID::kName, &value, VT_BSTR)) ? value.bstrVal : 0;
}

/**
 * @brief get ClassID handler property
 */
BSTR Format7zDLL::Interface::GetHandlerClassID(HWND hwndParent)
{
	PROPVARIANT value;
	return SUCCEEDED(GetHandlerProperty(hwndParent, NArchive::NHandlerPropID::kClassID, &value, VT_BSTR)) ? value.bstrVal : 0;
}

/**
 * @brief get Extension handler property
 */
BSTR Format7zDLL::Interface::GetHandlerExtension(HWND hwndParent)
{
	PROPVARIANT value;
	return SUCCEEDED(GetHandlerProperty(hwndParent, NArchive::NHandlerPropID::kExtension, &value, VT_BSTR)) ? value.bstrVal : 0;
}

/**
 * @brief get AddExtension handler property
 */
BSTR Format7zDLL::Interface::GetHandlerAddExtension(HWND hwndParent)
{
	PROPVARIANT value;
	return SUCCEEDED(GetHandlerProperty(hwndParent, NArchive::NHandlerPropID::kAddExtension, &value, VT_BSTR)) ? value.bstrVal : 0;
}

/**
 * @brief get Update handler property
 */
VARIANT_BOOL Format7zDLL::Interface::GetHandlerUpdate(HWND hwndParent)
{
	PROPVARIANT value;
	return SUCCEEDED(GetHandlerProperty(hwndParent, NArchive::NHandlerPropID::kUpdate, &value, VT_BOOL)) ? value.boolVal : 0;
}

/**
 * @brief get KeepName handler property
 */
VARIANT_BOOL Format7zDLL::Interface::GetHandlerKeepName(HWND hwndParent)
{
	PROPVARIANT value;
	return SUCCEEDED(GetHandlerProperty(hwndParent, NArchive::NHandlerPropID::kKeepName, &value, VT_BOOL)) ? value.boolVal : 0;
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

Format7zDLL::Interface *Format7zDLL::Interface::head = NULL;

/**
 * @brief Access archiver dll functions through proxy.
 */

#define CLS_ARC_ID_ITEM(cls) ((cls).Data4[5])

Format7zDLL::Proxy::Handle Format7zDLL::Proxy::handle =
{
	"%1!s!7z.dll",
	"CreateObject",
	"GetHandlerProperty2",
	"GetNumberOfFormats",
	"GetIsArc",
	(HMODULE)0
};

struct Format7zDLL::Proxy *Format7zDLL::Proxy::operator->()
{
	DllProxyHelper(&handle.aModule, g_cPath7z);
	if (formatIndex < 0)
	{
		GUID clsId =
		{
			0x23170F69, 0x40C1, 0x278A,
			0x10, 0x00, 0x00, 0x01, 0x10, (BYTE)-formatIndex, 0x00, 0x00
		};
		UINT32 i = 0;
		handle.GetNumberOfFormats(&i);
		while (i)
		{
			PROPVARIANT value;
			::VariantInit((LPVARIANT)&value);
			if (SUCCEEDED(handle.GetHandlerProperty2(--i, NArchive::NHandlerPropID::kClassID, &value)) &&
				value.vt == VT_BSTR &&
				SysStringByteLen(value.bstrVal) == sizeof(GUID) &&
				IsEqualGUID(clsId, *value.puuid))
			{
				formatIndex = i;
				i = 0;
			}
			::VariantClear((LPVARIANT)&value);
		}
		if (formatIndex < 0)
		{
			TCHAR szArcID[4];
			wsprintf(szArcID, _T("%02x"), (UINT)CLS_ARC_ID_ITEM(clsId));
			Complain(RPC_S_INTERFACE_NOT_FOUND, szArcID, handle);
		}
	}
	return this;
}

STDMETHODIMP Format7zDLL::Proxy::CreateObject(const GUID *clsID, const GUID *interfaceID, void **outObject)
{
	return handle.CreateObject(clsID, interfaceID, outObject);
}

STDMETHODIMP Format7zDLL::Proxy::GetHandlerProperty(PROPID propID, PROPVARIANT *value)
{
	return handle.GetHandlerProperty2(formatIndex, propID, value);
}

STDMETHODIMP Format7zDLL::Proxy::GetIsArc(UINT32 formatIndex, Func_IsArc* isArc)
{
	return handle.GetIsArc(formatIndex, isArc);
}

#define	DEFINE_FORMAT(name, id, extension) \
		Format7zDLL::Proxy PROXY_##name = \
		{ \
			-0x##id, \
			0, \
			0, \
			'@', \
			extension \
		}; \
		Format7zDLL::Interface name = PROXY_##name;

DEFINE_FORMAT(CZipHandler,		01, "zip jar war ear xpi apk");
DEFINE_FORMAT(CBZip2Handler,	02, "bz2 tbz2 tbz");
DEFINE_FORMAT(CRarHandler,		03, "rar");
DEFINE_FORMAT(CArjHandler,		04, "arj");
DEFINE_FORMAT(CZHandler,		05, "z taz");
DEFINE_FORMAT(CLzhHandler,		06, "lzh lha");//"@-l" doesn't work because signature starts at offset 2
DEFINE_FORMAT(CFormat7z,		07, "7z");
DEFINE_FORMAT(CCabHandler,		08, "cab");
DEFINE_FORMAT(CNsisHandler,		09, "");
DEFINE_FORMAT(CLzmaHandler,		0A, "lzma");
DEFINE_FORMAT(CLzma86Handler,	0B, "lzma86");
DEFINE_FORMAT(CXzHandler,		0C, "xz txz");
DEFINE_FORMAT(CPpmdHandler,		0D, "");
DEFINE_FORMAT(CZstdHandler,		0E,  "zst tzst");

DEFINE_FORMAT(CLvmHandler,		BF, "");
DEFINE_FORMAT(CAvbHandler,		C0, "");
DEFINE_FORMAT(CLpHandler,		C1, "");
DEFINE_FORMAT(CSparseHandler,	C2, "");
DEFINE_FORMAT(CApFsHandler,		C3, "");
DEFINE_FORMAT(CVhdxHandler,		C4, "");
DEFINE_FORMAT(CBase64Handler,	C5, "");
DEFINE_FORMAT(CCOFFHandler,		C6, "");
DEFINE_FORMAT(CExtHandler,		C7, "");
DEFINE_FORMAT(CVMDKHandler,		C8, "");
DEFINE_FORMAT(CVDIHandler,		C9, "");
DEFINE_FORMAT(CQcowHandler,		CA, "");
DEFINE_FORMAT(CGPTHandler,		CB, "");
DEFINE_FORMAT(CRar5Handler,		CC, "rar");
DEFINE_FORMAT(CIHEXHandler,		CD, "");
DEFINE_FORMAT(CHxsHandler,		CE, "");
DEFINE_FORMAT(CTEHandler,		CF, "");
DEFINE_FORMAT(CUEFIcHandler,	D0, "");
DEFINE_FORMAT(CUEFIsHandler,	D1, "");
DEFINE_FORMAT(CSquashFSHandler,	D2, "");
DEFINE_FORMAT(CCramFSHandler,	D3, "");
DEFINE_FORMAT(CApmHandler,		D4, "");
DEFINE_FORMAT(CMslzHandler,		D5, "");
DEFINE_FORMAT(CFlvHandler,		D6, "");
DEFINE_FORMAT(CSwfHandler,		D7, "");
DEFINE_FORMAT(CSwfcHandler,		D8, "");
DEFINE_FORMAT(CNtfsHandler,		D9, "");
DEFINE_FORMAT(CFatHandler,		DA, "");
DEFINE_FORMAT(CMbrHandler,		DB, "");
DEFINE_FORMAT(CVhdHandler,		DC, "");
DEFINE_FORMAT(CPeHandler,		DD, "");
DEFINE_FORMAT(CElfHandler,		DE, "elf");
DEFINE_FORMAT(CMachOHandler,	DF, "");
DEFINE_FORMAT(CUdfHandler,		E0, "udf");
DEFINE_FORMAT(CXarHandler,		E1, "xar pkg");
DEFINE_FORMAT(CMubHandler,		E2, "mub");
DEFINE_FORMAT(CHfsHandler,		E3, "hfs");
DEFINE_FORMAT(CDmgHandler,		E4, "dmg");
DEFINE_FORMAT(CComHandler,		E5, "");
DEFINE_FORMAT(CWimHandler,		E6, "wim swm");
DEFINE_FORMAT(CIsoHandler,		E7, "iso");
DEFINE_FORMAT(CSplitHandler,	EA, "");
DEFINE_FORMAT(CChmHandler,		E9, "chm chi chq chw hxs hxi hxr hxq hxw lit");
DEFINE_FORMAT(CRpmHandler,		EB, "rpm");
DEFINE_FORMAT(CDebHandler,		EC, "deb");
DEFINE_FORMAT(CCpioHandler,		ED, "cpio");
DEFINE_FORMAT(CTarHandler,		EE, "tar");
DEFINE_FORMAT(CGZipHandler,		EF, "gz tgz");

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
 * @brief Check signature manually using 7-Zip's signature properties when IsArc is not available.
 */
static bool CheckSignatureManually(Format7zDLL::Proxy* proxy, LPCH sig, int cchSig)
{
	if (!sig || cchSig <= 0)
		return false;

	// Get signature offset
	UInt32 sigOffset = 0;
	PROPVARIANT offsetValue;
	PropVariantInit(&offsetValue);
	if (SUCCEEDED(proxy->GetHandlerProperty(NArchive::NHandlerPropID::kSignatureOffset, &offsetValue)) &&
		offsetValue.vt == VT_UI4)
	{
		sigOffset = offsetValue.ulVal;
	}
	VariantClear((VARIANT *)&offsetValue);

	// Try kSignature first (single signature)
	PROPVARIANT value;
	PropVariantInit(&value);
	proxy->GetHandlerProperty(NArchive::NHandlerPropID::kSignature, &value);

	bool isMultiSignature = false;
	// If kSignature is not available, try kMultiSignature
	if (value.vt != VT_BSTR || !value.bstrVal)
	{
		VariantClear((VARIANT *)&value);
		PropVariantInit(&value);
		proxy->GetHandlerProperty(NArchive::NHandlerPropID::kMultiSignature, &value);
		isMultiSignature = true;
	}

	bool matched = false;
	if (value.vt == VT_BSTR && value.bstrVal)
	{
		UInt32 totalSize = SysStringByteLen(value.bstrVal);
		const BYTE *pchSignature = (const BYTE *)value.bstrVal;

		if (isMultiSignature)
		{
			// Multi-signature format: [length1][signature1][length2][signature2]...
			// Each signature is prefixed with 1 byte indicating its length
			UInt32 pos = 0;

			while (pos < totalSize)
			{
				// Read signature length (1 byte)
				UInt32 sigLen = pchSignature[pos++];

				if (pos + sigLen > totalSize)
					break; // Invalid format

				// Check this signature
				if (cchSig >= (int)(sigOffset + sigLen))
				{
					bool signatureMatch = true;
					for (UInt32 j = 0; j < sigLen; j++)
					{
						if ((BYTE)sig[sigOffset + j] != pchSignature[pos + j])
						{
							signatureMatch = false;
							break;
						}
					}

					if (signatureMatch)
					{
						matched = true;
						break;
					}
				}

				pos += sigLen; // Move to next signature
			}
		}
		else
		{
			// Single signature
			if (cchSig >= (int)(sigOffset + totalSize))
			{
				bool signatureMatch = true;
				for (UInt32 j = 0; j < totalSize; j++)
				{
					if ((BYTE)sig[sigOffset + j] != pchSignature[j])
					{
						signatureMatch = false;
						break;
					}
				}

				if (signatureMatch)
					matched = true;
			}
		}
	}

	VariantClear((VARIANT *)&value);
	return matched;
}

/**
 * @brief Figure out which archiver dll to use for a given archive using pre-read signature.
 * This function receives an already-read signature buffer, unlike GuessFormatBySignature which reads the file itself.
 */
Merge7z::Format* Merge7z::GuessFormatEx(LPCSTR ext, LPCH sig, int cchSig)
{
	// If no signature provided, fall back to extension-only detection
	if (!sig || cchSig <= 0)
	{
		if (!ext || ext[0] == '\0')
			return nullptr;
		Format7zDLL::Interface* pFormat = Format7zDLL::Interface::head;
		while (pFormat)
		{
			static const char aBlank[] = " ";
			LPCSTR pchExtension = pFormat->proxy.extension;
			int cchExtension;
			while ((cchExtension = StrCSpnA(pchExtension += StrSpnA(pchExtension, aBlank), aBlank)) != 0)
			{
				if (StrIsIntlEqualA(FALSE, pchExtension, ext, cchExtension) && ext[cchExtension] == '\0')
					return pFormat;
				pchExtension += cchExtension;
			}
			pFormat = pFormat->next;
		}
		return nullptr;
	}

	// Build list of candidate formats
	Format7zDLL::Interface* candidates[100];
	int candidateCount = 0;
	int extensionMatches = 0;

	// First pass: collect formats matching extension if provided
	Format7zDLL::Interface* pFormat = Format7zDLL::Interface::head;
	bool useExtension = (ext != nullptr && ext[0] != '\0');

	while (pFormat && candidateCount < 100)
	{
		bool matchesExtension = false;

		if (useExtension)
		{
			static const char aBlank[] = " ";
			LPCSTR pchExtension = pFormat->proxy.extension;
			int cchExtension;
			while ((cchExtension = StrCSpnA(pchExtension += StrSpnA(pchExtension, aBlank), aBlank)) != 0)
			{
				if (StrIsIntlEqualA(FALSE, pchExtension, ext, cchExtension) && ext[cchExtension] == '\0')
				{
					matchesExtension = true;
					break;
				}
				pchExtension += cchExtension;
			}
		}

		if (matchesExtension)
		{
			// Insert at beginning (priority for extension matches)
			for (int i = candidateCount; i > extensionMatches; i--)
				candidates[i] = candidates[i - 1];
			candidates[extensionMatches] = pFormat;
			extensionMatches++;
			candidateCount++;
		}
		else
		{
			// Add at end (for signature checking)
			candidates[candidateCount++] = pFormat;
		}

		pFormat = pFormat->next;
	}

	// Use 7-Zip's IsArc functions to check signatures
	for (int i = 0; i < candidateCount; i++)
	{
		pFormat = candidates[i];

		// Get IsArc function for this format
		Func_IsArc isArc = nullptr;
		try
		{
			Format7zDLL::Proxy* pr = pFormat->proxy.operator->();
			if (SUCCEEDED(pr->GetIsArc(pr->formatIndex, &isArc)) && isArc)
			{
				// Call IsArc with the file signature
				UInt32 result = isArc((const Byte*)sig, (size_t)cchSig);
				if (result == k_IsArc_Res_YES ||
					(i < extensionMatches && cchSig == sizeof(CH_SIGNATURE) && result == k_IsArc_Res_NEED_MORE))
					return pFormat;
			}
			else
			{
				// IsArc function not available, fall back to manual signature checking
				if (CheckSignatureManually(pr, sig, cchSig))
					return pFormat;
				// If no signature or signature doesn't match, return format if extension matches
				else if (i < extensionMatches)
					return pFormat;
			}
		}
		catch (Complain*)
		{
			// Ignore unsupported/problematic formats while probing.
		}
	}

	// No signature match found
	return nullptr;
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
 * Just reads the file header; 7-Zip handlers will handle self-extracting archives themselves.
 */
DWORD Merge7z::GetSignature(LPCTSTR path, CH_SIGNATURE sig)
{
	if (sig == NULL)
		return sizeof(CH_SIGNATURE);

	DWORD cchSig = 0;
	HANDLE h = CreateFile(path, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, 
						  0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (h != INVALID_HANDLE_VALUE)
	{
		ReadFile(h, sig, sizeof(CH_SIGNATURE), &cchSig, 0);
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
	g_LangPath += L"Lang\\";
	g_LangPath += langFile;
	int slash = g_LangPath.ReverseFind(L'\\');
	int minus = g_LangPath.ReverseFind(L'-');
	int dot = g_LangPath.ReverseFind(L'.');
	if (dot <= slash)
	{
		dot = g_LangPath.Len();
		g_LangPath += L".txt";
	}
	if (minus > slash && !PathFileExists(g_LangPath))
	{
		// 2nd chance: filename == language code
		CSysString Region = g_LangPath.Mid(minus, dot - minus);
		Region.Replace(L'-', L'\\');
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

void ReadRegLang(UString &langFile)
{
	langFile = GetUnicodeString(g_LangPath);
}

void SaveRegLang(const UString &)
{
}

/**
 * @brief Provide substitutes for Read_ShowPassword()/Save_ShowPassword.
 */
#include "7zip/UI/Common/ZipRegistry.h"

bool NExtract::Read_ShowPassword()
{
	return (g_dwFlags & Merge7z::Initialize::ShowPassword) != 0;
}

void NExtract::Save_ShowPassword(bool)
{
}

UInt32 NExtract::Read_LimitGB()
{
	return (UInt32)(Int32)-1;
}


void NExtract::Save_LimitGB(UInt32 limit_GB)
{
}


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
