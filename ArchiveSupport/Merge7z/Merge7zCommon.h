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
#define DllBuild_Merge7z 10
#define DLLPSTUB /##/
#include "Merge7z.h"

using namespace NWindows;

extern HINSTANCE g_hInstance;
extern DWORD g_dwFlags;
extern CHAR g_cPath7z[MAX_PATH];

typedef UINT32 (WINAPI * CreateObjectFunc)(
	const GUID *clsID, 
	const GUID *interfaceID, 
	void **outObject);

typedef UINT32 (WINAPI * GetHandlerPropertyFunc)(
	PROPID propID, PROPVARIANT *value);

struct Format7zDLL
{
	HMODULE origin;
	CreateObjectFunc CreateObject;
	GetHandlerPropertyFunc GetHandlerProperty;
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
	void GetDefaultName(HWND, UString &);
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
		virtual BSTR GetDefaultName();
		Format7zDLL::Interface *const format;
		IInArchive *archive;
		CInFileStream *file;
		IArchiveOpenCallback *callback;
		CSysString const path;
		UString ustrDefaultName;
		NFile::NFind::CFileInfo fileInfo;
		Inspector(Format7zDLL::Interface *, LPCTSTR);
		void Init(HWND);
	};
	virtual Merge7z::Format::Inspector *Open(HWND, LPCTSTR);
	interface Updater : Merge7z::Format::Updater
	{
		virtual void Free();
		virtual UINT32 Add(Merge7z::DirItemEnumerator::Item &);
		virtual HRESULT Commit(HWND);
		Format7zDLL::Interface *const format;
		IOutArchive *outArchive;
		COutFileStream *file;
		CSysString const path;
		CObjectVector<CDirItem> dirItems;
		CObjectVector<CArchiveItem> archiveItems;
		Updater(Format7zDLL::Interface *, LPCTSTR);
		void Init(HWND);
	};
	virtual Merge7z::Format::Updater *Update(HWND, LPCTSTR);
	virtual HRESULT GetHandlerProperty(HWND, PROPID, PROPVARIANT *, VARTYPE);
	virtual BSTR GetHandlerName(HWND);
	virtual BSTR GetHandlerClassID(HWND);
	virtual BSTR GetHandlerExtension(HWND);
	virtual BSTR GetHandlerAddExtension(HWND);
	virtual VARIANT_BOOL GetHandlerUpdate(HWND);
	virtual VARIANT_BOOL GetHandlerKeepName(HWND);
	virtual BSTR GetDefaultName(HWND, LPCTSTR);
};
