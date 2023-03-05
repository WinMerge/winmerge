/** 
 * @file  Merge7zFormatShellImpl.cpp
 *
 * @brief Implementation file for Merge7zFormatShellImpl class
 */
#include "pch.h"
#include "Merge7zFormatShellImpl.h"
#pragma warning (push)			// prevent "warning C4091: 'typedef ': ignored on left of 'tagGPFIDL_FLAGS' when no variable is declared"
#pragma warning (disable:4091)	// VC bug when using XP enabled toolsets.
#include <shlobj.h>
#pragma warning (pop)
#include <ShObjIdl.h>
#include <comip.h>
#include <vector>
#include "unicoder.h"
#include "Merge7zFormatRegister.h"
#include "paths.h"

typedef _com_ptr_t<_com_IIID<IFileOperation, &__uuidof(IFileOperation)>> IFileOperationPtr;
typedef _com_ptr_t<_com_IIID<IShellItem, &__uuidof(IShellItem)>> IShellItemPtr;
typedef _com_ptr_t<_com_IIID<IEnumShellItems, &__uuidof(IEnumShellItems)>> IEnumShellItemsPtr;

static Merge7zFormatRegister g_autoregister(&Merge7zFormatShellImpl::GuessFormat);
static Merge7zFormatShellImpl g_shellformat;

static HRESULT MySHCreateShellItemFromPath(PCWSTR pszPath, IShellItem **ppShellItem)
{
	PIDLIST_ABSOLUTE pidl;
	HRESULT hr = SHParseDisplayName(pszPath, nullptr, &pidl, 0, nullptr);
	if (FAILED(hr))
		return hr;
	hr = SHCreateShellItem(nullptr, nullptr, pidl, ppShellItem);
	ILFree(pidl);
	return hr;
}

struct IEnumShellItems;

static HRESULT MySHCreateEnumShellItemsFromPath(PCWSTR pszPath, IEnumShellItems **ppEnumShellItems)
{
	IShellItemPtr pShellItem;
	HRESULT hr = MySHCreateShellItemFromPath(pszPath, &pShellItem);
	if (FAILED(hr))
		return hr;
	return pShellItem->BindToHandler(nullptr, BHID_EnumItems, IID_PPV_ARGS(ppEnumShellItems));
}

Merge7z::Format *Merge7zFormatShellImpl::GuessFormat(const String& path)
{
	if (!paths::IsURL(path))
	{
		int i;
		static const tchar_t *exts[] = {_T(".zip"), _T(".lzh"), _T("::{")};
		for (i = 0; i < sizeof(exts) / sizeof(exts[0]); ++i)
		{
			if (path.find(exts[i]) != String::npos)
				break;
		}
		if (i == sizeof(exts)/sizeof(exts[0]))
			return nullptr;
	}
	else
	{
		if (path.find(_T("ftp://")) == String::npos)
			return nullptr;
	}
	IEnumShellItemsPtr pEnumShellItems;
	if (FAILED(MySHCreateEnumShellItemsFromPath(ucr::toUTF16(path).c_str(), &pEnumShellItems)))
		return nullptr;
	return &g_shellformat;
}

HRESULT Merge7zFormatShellImpl::DeCompressArchive(HWND, LPCTSTR path, LPCTSTR folder)
{
	IFileOperationPtr pFileOperation;
	HRESULT hr = pFileOperation.CreateInstance(CLSID_FileOperation, nullptr, CLSCTX_ALL);
	if (FAILED(hr))
		return hr;

	pFileOperation->SetOperationFlags(0);

	IShellItemPtr pShellItemDest, pShellItem;
	IEnumShellItemsPtr pEnumShellItems;
	ULONG ulRetNo;

	hr = MySHCreateShellItemFromPath(ucr::toUTF16(folder).c_str(), &pShellItemDest);
	if (FAILED(hr))
		return hr;
	hr = MySHCreateEnumShellItemsFromPath(ucr::toUTF16(path).c_str(), &pEnumShellItems);
	if (FAILED(hr))
		return hr;

	while ((hr = pEnumShellItems->Next(1, &pShellItem, &ulRetNo)) == NOERROR)
	{
		pFileOperation->CopyItem(pShellItem, pShellItemDest, nullptr, nullptr);
		pShellItem.Release();
	}
	if (FAILED(hr))
		return hr;

	return pFileOperation->PerformOperations();
}

HRESULT Merge7zFormatShellImpl::CompressArchive(HWND, LPCTSTR path, Merge7z::DirItemEnumerator *)
{
	return E_FAIL;
}

Merge7z::Format::Inspector *Merge7zFormatShellImpl::Open(HWND, LPCTSTR) { return nullptr; }
Merge7z::Format::Updater *Merge7zFormatShellImpl::Update(HWND, LPCTSTR) { return nullptr; }
HRESULT Merge7zFormatShellImpl::GetHandlerProperty(HWND, PROPID, PROPVARIANT *, VARTYPE) { return E_FAIL; }
BSTR Merge7zFormatShellImpl::GetHandlerName(HWND) { return nullptr; }
BSTR Merge7zFormatShellImpl::GetHandlerClassID(HWND) { return nullptr; }
BSTR Merge7zFormatShellImpl::GetHandlerExtension(HWND) { return nullptr; }
BSTR Merge7zFormatShellImpl::GetHandlerAddExtension(HWND) { return nullptr; }
VARIANT_BOOL Merge7zFormatShellImpl::GetHandlerUpdate(HWND) { return VARIANT_FALSE; }
VARIANT_BOOL Merge7zFormatShellImpl::GetHandlerKeepName(HWND) { return VARIANT_FALSE; }

BSTR Merge7zFormatShellImpl::GetDefaultName(HWND, LPCTSTR path)
{
	return SysAllocString(L"");
}
