// includes from 7-Zip sources
#include "7zip/Archive/IArchive.h"
#include "Common/StringConvert.h"
#include "Windows/PropVariant.h"
#include "Windows/PropVariantConversions.h"
#include "Windows/FileDir.h"
#include "Windows/FileFind.h"
#include "Windows/Thread.h"
#include "7zip/UI/Common/DirItem.h"
#include "7zip/Common/FileStreams.h"
// Merge7z includes
#include "tools.h"
#define DllBuild_Merge7z 9
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
	interface Inspector : Merge7z::Format::Inspector
	{
		virtual void Free();
		virtual UINT32 Open();
		virtual HRESULT Extract(HWND, LPCTSTR folder, const UINT32 *indices = 0, UINT32 numItems = -1);
		virtual HRESULT GetProperty(UINT32, PROPID, PROPVARIANT *, VARTYPE);
		virtual BSTR GetPath(UINT32);
		virtual BSTR GetName(UINT32);
		virtual BSTR GetExtension(UINT32);
		virtual VARIANT_BOOL IsFolder(UINT32);
		virtual FILETIME LastWriteTime(UINT32);
		IInArchive *archive;
		CInFileStream *file;
		IArchiveOpenCallback *callback;
		CSysString path;
		NFile::NFind::CFileInfo fileInfo;
		Inspector(Format7zDLL::Interface *, HWND, LPCTSTR);
		void Init();
	};
	virtual Merge7z::Format::Inspector *Open(HWND, LPCTSTR);
	interface Updater : Merge7z::Format::Updater
	{
		virtual void Free();
		virtual UINT32 Add(Merge7z::DirItemEnumerator::Item &);
		virtual HRESULT Commit(HWND);
		IOutArchive *outArchive;
		COutFileStream *file;
		CSysString path;
		CObjectVector<CDirItem> dirItems;
		CObjectVector<CArchiveItem> archiveItems;
		Updater(Format7zDLL::Interface *, HWND, LPCTSTR);
		void Init();
	};
	virtual Merge7z::Format::Updater *Update(HWND, LPCTSTR);
};
