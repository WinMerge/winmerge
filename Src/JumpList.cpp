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

IShellLinkW *CreateShellLink(const std::wstring& app_path, const std::wstring& params, const std::wstring& title, const std::wstring& desc, const std::wstring& icon_path, int icon_index)
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
	std::wstring icon_path2(icon_path);
	if (icon_path.empty())
	{
		if (g_exe_path[0] == '\0')
			GetModuleFileNameW(nullptr, g_exe_path, sizeof(g_exe_path)/sizeof(g_exe_path[0]));
		icon_path2 = g_exe_path;
	}
	pShellLink->SetPath(app_path2.c_str());
	pShellLink->SetIconLocation(icon_path2.c_str(), icon_index);
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

static std::vector<JumpList::Item> GetList(IObjectArray *pObjectArray)
{
	std::vector<JumpList::Item> list;
	UINT nObjects;
	if (SUCCEEDED(pObjectArray->GetCount(&nObjects)))
	{
		for (UINT i = 0; i < nObjects; ++i)
		{
			IShellLinkW *pShellLink;
			if (SUCCEEDED(pObjectArray->GetAt(i, IID_IShellLinkW, (void **)&pShellLink)))
			{
				wchar_t szPath[MAX_PATH];
				wchar_t szPathIcon[MAX_PATH];
				wchar_t szDescription[MAX_PATH];
				wchar_t szArguments[MAX_PATH * 6];
				int icon_index = 0;
				pShellLink->GetPath(szPath, sizeof(szPath) / sizeof(szPath[0]), nullptr, SLGP_RAWPATH);
				pShellLink->GetDescription(szDescription, sizeof(szDescription) / sizeof(szDescription[0]));
				pShellLink->GetArguments(szArguments, sizeof(szArguments) / sizeof(szArguments[0]));
				pShellLink->GetIconLocation(szPathIcon, sizeof(szPathIcon) / sizeof(szPathIcon[0]), &icon_index);
				IPropertyStore *pPS = nullptr;
				if (SUCCEEDED(pShellLink->QueryInterface(IID_IPropertyStore, (void **)&pPS)))
				{
					PROPVARIANT pv;
					PropVariantInit(&pv);
					if (SUCCEEDED(pPS->GetValue(PKEY_Title, &pv)))
					{
						if (pv.vt == VT_LPWSTR && pv.bstrVal)
							list.push_back(JumpList::Item(ucr::toTString(szPath), ucr::toTString(szArguments), ucr::toTString(pv.bstrVal), ucr::toTString(szDescription), ucr::toTString(szPathIcon), icon_index));
						PropVariantClear(&pv);
					}
					pPS->Release();
				}
				pShellLink->Release();
			}
		}
	}
	return list;
}

static HRESULT CreateApplicationDocumentLists(IApplicationDocumentLists** ppDocumentLists)
{
	HRESULT hr = CoCreateInstance(CLSID_ApplicationDocumentLists, nullptr, CLSCTX_INPROC_SERVER,
		IID_IApplicationDocumentLists, (void**)ppDocumentLists);
	if (FAILED(hr))
		return hr;
	hr = (*ppDocumentLists)->SetAppID(g_appid.c_str());
	if (FAILED(hr))
	{
		(*ppDocumentLists)->Release();
		return hr;
	}
	return hr;
}

static HRESULT CreateApplicationDestinations(IApplicationDestinations** ppApplicationDestinations)
{
	HRESULT hr = CoCreateInstance(CLSID_ApplicationDestinations, nullptr, CLSCTX_INPROC_SERVER,
		IID_IApplicationDestinations, (void**)ppApplicationDestinations);
	if (FAILED(hr))
		return hr;
	hr = (*ppApplicationDestinations)->SetAppID(g_appid.c_str());
	if (FAILED(hr))
	{
		(*ppApplicationDestinations)->Release();
		return hr;
	}
	return hr;
}

static HRESULT CreateCustomDestinationList(ICustomDestinationList** ppCustomDestinationList)
{
	HRESULT hr = CoCreateInstance(CLSID_DestinationList, nullptr, CLSCTX_INPROC_SERVER,
		IID_ICustomDestinationList, (void**)ppCustomDestinationList);
	if (FAILED(hr))
		return hr;
	hr = (*ppCustomDestinationList)->SetAppID(g_appid.c_str());
	if (FAILED(hr))
	{
		(*ppCustomDestinationList)->Release();
		return hr;
	}
	return hr;
}

}

namespace JumpList
{

bool SetCurrentProcessExplicitAppUserModelID(const std::wstring& appid)
{
	g_appid = appid;
#ifdef _WIN64
	return ::SetCurrentProcessExplicitAppUserModelID(appid.c_str()) == S_OK;
#else
	HMODULE hLibrary = GetModuleHandle(_T("shell32.dll"));
	if (hLibrary == nullptr)
		return false;
	HRESULT (__stdcall *pfnSetCurrentProcessExplicitAppUserModelID)(PCWSTR AppID) = 
		(HRESULT (__stdcall *)(PCWSTR))GetProcAddress(hLibrary, "SetCurrentProcessExplicitAppUserModelID");
	if (pfnSetCurrentProcessExplicitAppUserModelID == nullptr)
		return false;
	return pfnSetCurrentProcessExplicitAppUserModelID(appid.c_str()) == S_OK;
#endif
}

bool AddToRecentDocs(const String& app_path, const String& params, const String& title, const String& desc, const String& icon_path, int icon_index)
{
	SHARDAPPIDINFOLINK saiil;
	saiil.pszAppID = g_appid.c_str();
	saiil.psl = CreateShellLink(app_path, params, title, desc, icon_path, icon_index);
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
	if (FAILED(CreateApplicationDocumentLists(&pDocumentLists)))
		return list;

	IObjectArray *pObjectArray;
	if (SUCCEEDED(pDocumentLists->GetList(ADLT_RECENT, static_cast<UINT>(nMaxItems), IID_IObjectArray, (void **)&pObjectArray)))
	{
		list = GetList(pObjectArray);
		pObjectArray->Release();
	}
	pDocumentLists->Release();
	return list;
}

bool RemoveRecentDocs()
{
	IApplicationDestinations* pDestinations = nullptr;
	if (FAILED(CreateApplicationDestinations(&pDestinations)))
		return false;
	HRESULT hr = pDestinations->RemoveAllDestinations();
	pDestinations->Release();
	return SUCCEEDED(hr);
}

bool AddUserTasks(const std::vector<Item>& tasks)
{
	ICustomDestinationList* pDestList = nullptr;
	HRESULT hr = CreateCustomDestinationList(&pDestList);
	if (FAILED(hr))
		return false;
	if (tasks.empty())
	{
		hr = pDestList->DeleteList(nullptr);
		pDestList->Release();
		return SUCCEEDED(hr);
	}
	IObjectCollection* pObjectCollection = nullptr;
	hr = CoCreateInstance(CLSID_EnumerableObjectCollection, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pObjectCollection));
	if (SUCCEEDED(hr))
	{
		for (const auto& task : tasks)
		{
			IShellLinkW* pShellLink = CreateShellLink(task.path, task.params, task.title, task.desc, task.icon_path, task.icon_index);
			if (pShellLink)
			{
				pObjectCollection->AddObject(pShellLink);
				pShellLink->Release();
			}
		}

		IObjectArray* pObjectArray = nullptr;
		hr = pObjectCollection->QueryInterface(IID_PPV_ARGS(&pObjectArray));
		if (SUCCEEDED(hr))
		{
			IObjectArray* pRemovedItems = nullptr;
			UINT minSlots;
			hr = pDestList->BeginList(&minSlots, IID_PPV_ARGS(&pRemovedItems));
			if (SUCCEEDED(hr))
			{
				pRemovedItems->Release();
				hr = pDestList->AddUserTasks(pObjectArray);
				if (SUCCEEDED(hr))
					hr = pDestList->CommitList();
			}
			pObjectArray->Release();
		}
		pObjectCollection->Release();
	}
	pDestList->Release();
	return SUCCEEDED(hr);
}

}
