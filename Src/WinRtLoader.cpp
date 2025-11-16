/** 
 * @file  WinRTLoader.cpp
 * @brief Implementation of dynamic loading for WinRT functions
 */
#include "StdAfx.h"

struct HSTRING__;
using HSTRING = HSTRING__ *;

typedef HRESULT(WINAPI* PFN_RoGetActivationFactory)(HSTRING, REFIID, void**);
typedef HRESULT(WINAPI* PFN_RoOriginateLanguageException)(HRESULT, HSTRING, IUnknown*);

namespace WinRtLoader
{
	PFN_RoGetActivationFactory pRoGetActivationFactory = nullptr;
	PFN_RoOriginateLanguageException pRoOriginateLanguageException = nullptr;
	HMODULE hCombase = nullptr;

	bool Initialize()
	{
		if (hCombase)
			return true;
		hCombase = LoadLibraryW(L"combase.dll");
		if (!hCombase)
			return false;
		pRoGetActivationFactory = reinterpret_cast<PFN_RoGetActivationFactory>(GetProcAddress(hCombase, "RoGetActivationFactory"));
		pRoOriginateLanguageException = reinterpret_cast<PFN_RoOriginateLanguageException>(GetProcAddress(hCombase, "RoOriginateLanguageException"));
		return pRoGetActivationFactory != nullptr;
	}
}

extern "C"
{
	HRESULT WINAPI WINRT_IMPL_RoGetActivationFactory(HSTRING classId, REFIID iid, void** factory)
	{
		if (!WinRtLoader::Initialize() || !WinRtLoader::pRoGetActivationFactory)
			return E_NOTIMPL;
		return WinRtLoader::pRoGetActivationFactory(classId, iid, factory);
	}

	HRESULT WINAPI WINRT_IMPL_RoOriginateLanguageException(HRESULT error, HSTRING message, IUnknown* exception)
	{
		if (!WinRtLoader::Initialize() || !WinRtLoader::pRoOriginateLanguageException)
			return E_NOTIMPL;
		return WinRtLoader::pRoOriginateLanguageException(error, message, exception);
	}
}
