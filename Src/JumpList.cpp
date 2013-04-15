/** 
 * @file  JumpList.cpp
 *
 * @brief Implementation file for JumpList helper functions.
 *
 */
#include "JumpList.h"
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
TCHAR g_exe_path[260];

IShellLink *CreateShellLink(const String& app_path, const String& params, const String& title, const String& desc, int icon_index)
{
#if _MSC_VER >= 1600
	IShellLink *pShellLink = NULL;
	if (FAILED(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
	                            IID_IShellLink, (void **)&pShellLink)))
		return NULL;

	String app_path2(app_path);
	if (app_path.empty())
	{
		if (g_exe_path[0] == '\0')
			GetModuleFileName(NULL, g_exe_path, sizeof(g_exe_path));
		app_path2 = g_exe_path;
	}
	pShellLink->SetPath(app_path2.c_str());
	pShellLink->SetIconLocation(app_path2.c_str(), icon_index);
	pShellLink->SetArguments(params.c_str());
	pShellLink->SetDescription(desc.c_str());

	IPropertyStore *pPS = NULL;
	if (SUCCEEDED(pShellLink->QueryInterface(IID_IPropertyStore, (void **)&pPS)))
	{
		std::wstring title2 = ucr::toUTF16((title.empty()) ? params : title);
		PROPVARIANT pv;
		InitPropVariantFromString(title2.c_str(), &pv);
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
	saiil.psl = CreateShellLink(app_path, params, title, desc, icon_index);
	if (!saiil.psl)
		return false;
	SHAddToRecentDocs(SHARD_APPIDINFOLINK, &saiil);
	saiil.psl->Release();
	return true;
#else
	return false;
#endif
}

}
