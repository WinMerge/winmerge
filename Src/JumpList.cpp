/** 
 * @file  JumpList.cpp
 *
 * @brief Implementation file for JumpList helper functions.
 *
 */
#include "JumpList.h"
#include <vector>
#include <ObjBase.h>
#include <ShlObj.h>
#if _MSC_VER >= 1600
#include <propvarutil.h>
#include <propkey.h>
#endif
#include "unicoder.h"

namespace
{

std::wstring g_appid;
wchar_t g_exe_path[260];

IShellLinkW *CreateShellLink(const std::wstring& app_path, const std::wstring& params, const std::wstring& title, const std::wstring& desc, int icon_index)
{
#if _MSC_VER >= 1600
	IShellLinkW *pShellLink = NULL;
	if (FAILED(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
	                            IID_IShellLinkW, (void **)&pShellLink)))
		return NULL;

	std::wstring app_path2(app_path);
	if (app_path.empty())
	{
		if (g_exe_path[0] == '\0')
			GetModuleFileNameW(NULL, g_exe_path, sizeof(g_exe_path)/sizeof(g_exe_path[0]));
		app_path2 = g_exe_path;
	}
	pShellLink->SetPath(app_path2.c_str());
	pShellLink->SetIconLocation(app_path2.c_str(), icon_index);
	pShellLink->SetArguments(params.c_str());
	pShellLink->SetDescription(desc.c_str());

	IPropertyStore *pPS = NULL;
	if (SUCCEEDED(pShellLink->QueryInterface(IID_IPropertyStore, (void **)&pPS)))
	{
		PROPVARIANT pv;
		InitPropVariantFromString(title.c_str(), &pv);
		pPS->SetValue(PKEY_Title, pv);
		PropVariantClear(&pv);
		pPS->Commit();
		pPS->Release();
	}

	return pShellLink;
#else
	return NULL;
#endif
}

}

namespace JumpList
{

bool SetCurrentProcessExplicitAppUserModelID(const std::wstring& appid)
{
	g_appid = appid;
	HMODULE hLibrary = GetModuleHandle(_T("shell32.dll"));
	if (!hLibrary)
		return false;
	HRESULT (__stdcall *pfnSetCurrentProcessExplicitAppUserModelID)(PCWSTR AppID) = 
		(HRESULT (__stdcall *)(PCWSTR))GetProcAddress(hLibrary, "SetCurrentProcessExplicitAppUserModelID");
	if (!pfnSetCurrentProcessExplicitAppUserModelID)
		return false;
	return pfnSetCurrentProcessExplicitAppUserModelID(appid.c_str()) == S_OK;
}

bool AddToRecentDocs(const String& app_path, const String& params, const String& title, const String& desc, int icon_index)
{
#if _MSC_VER >= 1600
	SHARDAPPIDINFOLINK saiil;
	saiil.pszAppID = g_appid.c_str();
#ifdef _UNICODE
	saiil.psl = CreateShellLink(app_path, params, title, desc, icon_index);
#else
	saiil.psl = (IShellLink *)CreateShellLink(ucr::toUTF16(app_path), ucr::toUTF16(params), ucr::toUTF16(title), ucr::toUTF16(desc), icon_index);
#endif
	if (!saiil.psl)
		return false;
	SHAddToRecentDocs(SHARD_APPIDINFOLINK, &saiil);
	saiil.psl->Release();
	return true;
#else
	return false;
#endif
}

std::vector<Item> GetRecentDocs(size_t nMaxItems)
{
	std::vector<Item> list;
#if _MSC_VER >= 1600
	IApplicationDocumentLists *pDocumentLists = NULL;
	if (FAILED(CoCreateInstance(CLSID_ApplicationDocumentLists, NULL, CLSCTX_INPROC_SERVER,
	                            IID_IApplicationDocumentLists, (void **)&pDocumentLists)))
		return list;
	pDocumentLists->SetAppID(g_appid.c_str());

	IObjectArray *pObjectArray;
	if (SUCCEEDED(pDocumentLists->GetList(ADLT_RECENT, static_cast<UINT>(nMaxItems), IID_IObjectArray, (void **)&pObjectArray)))
	{
		UINT nObjects;
		pObjectArray->GetCount(&nObjects);
		for (UINT i = 0; i < nObjects; ++i)
		{
			IShellLinkW *pShellLink;
			if (SUCCEEDED(pObjectArray->GetAt(i, IID_IShellLinkW, (void **)&pShellLink)))
			{
				wchar_t szPath[MAX_PATH];
				wchar_t szDescription[MAX_PATH];
				wchar_t szArguments[MAX_PATH * 6];
				pShellLink->GetPath(szPath, sizeof(szPath)/sizeof(szPath[0]), NULL, SLGP_RAWPATH);
				pShellLink->GetDescription(szDescription, sizeof(szDescription)/sizeof(szDescription[0]));
				pShellLink->GetArguments(szArguments, sizeof(szArguments)/sizeof(szArguments[0]));
				IPropertyStore *pPS = NULL;
				PROPVARIANT pv;
				InitPropVariantFromString(L"", &pv);
				if (SUCCEEDED(pShellLink->QueryInterface(IID_IPropertyStore, (void **)&pPS)))
				{
					pPS->GetValue(PKEY_Title, &pv);
					pPS->Release();
				}
				list.push_back(Item(ucr::toTString(szPath), ucr::toTString(szArguments), ucr::toTString(pv.bstrVal), ucr::toTString(szDescription)));
				PropVariantClear(&pv);
				pShellLink->Release();
			}		
		}
		pObjectArray->Release();
	}
	pDocumentLists->Release();
#endif
	return list;
}

}
