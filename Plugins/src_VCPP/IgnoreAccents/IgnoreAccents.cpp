// SPDX-License-Identifier: GPL-2.0-or-later
// IgnoreAccents.cpp

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "IgnoreAccents.h"
#include "IgnoreAccents_i.c"
#include "WinMergeScript.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
    OBJECT_ENTRY(CLSID_WinMergeScript, CWinMergeScript)
END_OBJECT_MAP()

extern "C"
BOOL WINAPI DllMain(
    HINSTANCE hInstance,
    DWORD dwReason,
    LPVOID)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_IgnoreAccentsLib);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        _Module.Term();
    }

    return TRUE;
}

STDAPI DllCanUnloadNow(void)
{
    return _Module.GetLockCount() == 0 ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(
    REFCLSID rclsid,
    REFIID riid,
    LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

STDAPI DllRegisterServer(void)
{
    return _Module.RegisterServer(TRUE);
}

STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}