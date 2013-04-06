// WinMerge32BitPluginProxy.cpp : WinMain ‚ÌŽÀ‘•


#include "stdafx.h"
#include "resource.h"
#include "WinMerge32BitPluginProxy_i.h"



class CWinMerge32BitPluginProxyModule : public ATL::CAtlExeModuleT< CWinMerge32BitPluginProxyModule >
	{
public :
	DECLARE_LIBID(LIBID_WinMerge32BitPluginProxyLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_WINMERGE32BITPLUGINPROXY, "{88BC89DA-E4D9-43C1-9474-88FD6A96D9B1}")
	};

CWinMerge32BitPluginProxyModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}

