// includes from 7-Zip sources
#include "7zip/Archive/IArchive.h"
#include "Common/StringConvert.h"
#include "Windows/PropVariant.h"
#include "Windows/PropVariantConversions.h"
#include "Windows/FileDir.h"
#include "Windows/Thread.h"
// Merge7z includes
#include "tools.h"
#include "Merge7z.h"

using namespace NWindows;

extern HINSTANCE g_hInstance;
extern DWORD g_dwFlags;
extern CHAR g_cPath7z[MAX_PATH];

typedef UINT32 (WINAPI * CreateObjectFunc)(
    const GUID *clsID, 
    const GUID *interfaceID, 
    void **outObject);

struct Format7zDLL
{
	HMODULE origin;
	CreateObjectFunc CreateObject;
	HMODULE handle;
	const CLSID *clsid;
	struct Proxy;
	interface Interface;
};

struct Format7zDLL::Proxy
{
	LPCSTR Format7zDLL
	[
		&((struct Format7zDLL *)0)->handle
	-	&((struct Format7zDLL *)0)->origin
	];
	HMODULE handle;
	const CLSID *clsid;
	struct Format7zDLL *operator->();
};

interface Format7zDLL::Interface : Merge7z::Format
{
	Proxy &proxy;
	Interface(Proxy &proxy):proxy(proxy)
	{
	}
	virtual IInArchive *GetInArchive();
	virtual HRESULT DeCompressArchive(HWND, LPCTSTR path, LPCTSTR folder);
	virtual IOutArchive *GetOutArchive();
	virtual HRESULT CompressArchive(HWND, LPCTSTR path, Merge7z::DirItemEnumerator *);
};

