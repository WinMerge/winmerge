/** 
 * @file  JumpList.cpp
 *
 * @brief Implementation file for JumpList helper functions.
 *
 */
#include "pch.h"
#include "JumpList.h"
#include <vector>
#pragma warning (push)			// prevent "warning C4091: 'typedef ': ignored on left of 'tagGPFIDL_FLAGS' when no variable is declared"
#pragma warning (disable:4091)	// VC bug when using XP enabled toolsets.
#include <shlobj.h>
#pragma warning (pop)
#include <propvarutil.h>
#include <propkey.h>
#include "unicoder.h"

namespace
{

std::wstring g_appid;
wchar_t g_exe_path[260];

IShellLinkW *CreateShellLink(const std::wstring& app_path, const std::wstring& params, const std::wstring& title, const std::wstring& desc, int icon_index)
{
	IShellLinkW *pShellLink = nullptr;
	if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
	                            IID_IShellLinkW, (void **)&pShellLink)))
		return nullptr;

	std::wstring app_path2(app_path);
	if (app_path.empty())
	{
		if (g_exe_path[0] == '\0')
			GetModuleFileNameW(nullptr, g_exe_path, sizeof(g_exe_path)/sizeof(g_exe_path[0]));
		app_path2 = g_exe_path;
	}
	pShellLink->SetPath(app_path2.c_str());
	pShellLink->SetIconLocation(app_path2.c_str(), icon_index);
	pShellLink->SetArguments(params.c_str());
	pShellLink->SetDescription(desc.c_str());

	IPropertyStore *pPS = nullptr;
	if (SUCCEEDED(pShellLink->QueryInterface(IID_IPropertyStore, (void **)&pPS)))
	{
		PROPVARIANT pv;
		if (SUCCEEDED(InitPropVariantFromString(title.c_str(), &pv)))
		{
			pPS->SetValue(PKEY_Title, pv);
			PropVariantClear(&pv);
		}
		pPS->Commit();
		pPS->Release();
	}

	return pShellLink;
}

}

namespace JumpList
{

bool SetCurrentProcessExplicitAppUserModelID(const std::wstring& appid)
{
	g_appid = appid;
	HMODULE hLibrary = GetModuleHandle(_T("shell32.dll"));
	if (hLibrary == nullptr)
		return false;
	HRESULT (__stdcall *pfnSetCurrentProcessExplicitAppUserModelID)(PCWSTR AppID) = 
		(HRESULT (__stdcall *)(PCWSTR))GetProcAddress(hLibrary, "SetCurrentProcessExplicitAppUserModelID");
	if (pfnSetCurrentProcessExplicitAppUserModelID == nullptr)
		return false;
	return pfnSetCurrentProcessExplicitAppUserModelID(appid.c_str()) == S_OK;
}

bool AddToRecentDocs(const String& app_path, const String& params, const String& title, const String& desc, int icon_index)
{
	SHARDAPPIDINFOLINK saiil;
	saiil.pszAppID = g_appid.c_str();
	saiil.psl = CreateShellLink(app_path, params, title, desc, icon_index);
	if (saiil.psl == nullptr)
		return false;
	SHAddToRecentDocs(SHARD_APPIDINFOLINK, &saiil);
	saiil.psl->Release();
	return true;
}

std::vector<Item> GetRecentDocs(size_t nMaxItems)
{
	std::vector<Item> list;
	IApplicationDocumentLists *pDocumentLists = nullptr;
	if (FAILED(CoCreateInstance(CLSID_ApplicationDocumentLists, nullptr, CLSCTX_INPROC_SERVER,
	                            IID_IApplicationDocumentLists, (void **)&pDocumentLists)))
		return list;
	pDocumentLists->SetAppID(g_appid.c_str());

	IObjectArray *pObjectArray;
	if (SUCCEEDED(pDocumentLists->GetList(ADLT_RECENT, static_cast<UINT>(nMaxItems), IID_IObjectArray, (void **)&pObjectArray)))
	{
		UINT nObjects;
		if (SUCCEEDED(pObjectArray->GetCount(&nObjects)))
		{
			for (UINT i = 0; i < nObjects; ++i)
			{
				IShellLinkW *pShellLink;
				if (SUCCEEDED(pObjectArray->GetAt(i, IID_IShellLinkW, (void **)&pShellLink)))
				{
					wchar_t szPath[MAX_PATH];
					wchar_t szDescription[MAX_PATH];
					wchar_t szArguments[MAX_PATH * 6];
					pShellLink->GetPath(szPath, sizeof(szPath) / sizeof(szPath[0]), nullptr, SLGP_RAWPATH);
					pShellLink->GetDescription(szDescription, sizeof(szDescription) / sizeof(szDescription[0]));
					pShellLink->GetArguments(szArguments, sizeof(szArguments) / sizeof(szArguments[0]));
					IPropertyStore *pPS = nullptr;
					if (SUCCEEDED(pShellLink->QueryInterface(IID_IPropertyStore, (void **)&pPS)))
					{
						PROPVARIANT pv;
						if (SUCCEEDED(pPS->GetValue(PKEY_Title, &pv)))
						{
							list.push_back(Item(ucr::toTString(szPath), ucr::toTString(szArguments), ucr::toTString(pv.bstrVal), ucr::toTString(szDescription)));
							PropVariantClear(&pv);
						}
						pPS->Release();
					}
					pShellLink->Release();
				}
			}
		}
		pObjectArray->Release();
	}
	pDocumentLists->Release();
	return list;
}

}
