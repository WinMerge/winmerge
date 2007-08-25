// includes from 7-Zip sources

#include "7zip/MyVersion.h"
#include "7zip/Archive/IArchive.h"
#include "Common/StringConvert.h"
#include "Windows/PropVariant.h"
#include "Windows/PropVariantConversions.h"
#include "Windows/FileDir.h"
#include "Windows/FileFind.h"

#include "7zip/UI/Common/DirItem.h"
#include "7zip/Common/FileStreams.h"

// Merge7z includes
#include "tools.h"
#define DllBuild_Merge7z 24
#define DLLPSTUB /##/
#include "Merge7z.h"

typedef char SZ_EXTENSION[8];
typedef char CH_SIGNATURE[64]; //MAX(4 + IMAGE_SIZEOF_FILE_HEADER, 64)

using namespace NWindows;

extern HINSTANCE g_hInstance;
extern DWORD g_dwFlags;
extern CHAR g_cPath7z[MAX_PATH];

struct Format7zDLL
{
	struct Proxy;
	interface Interface;
};

#if MY_VER_MAJOR * 100 + MY_VER_MINOR < 445

struct Format7zDLL::Proxy
{
	const char *aModule;
	union
	{
		const char *aCreateObject;
		HRESULT(STDAPICALLTYPE*CreateObject)(const GUID *clsID, const GUID *interfaceID, void **outObject);
	};
	union
	{
		const char *aGetHandlerProperty;
		HRESULT(STDAPICALLTYPE*GetHandlerProperty)(PROPID propID, PROPVARIANT *value);
	};
	HMODULE handle;
	const char *extension;
	size_t signature;
	struct Proxy *operator->();
};

#else

struct Format7zDLL::Proxy
{
	INT32 formatIndex;
	const char *extension;
	size_t signature;
	STDMETHODIMP CreateObject(const GUID *clsID, const GUID *interfaceID, void **outObject);
	STDMETHODIMP GetHandlerProperty(PROPID propID, PROPVARIANT *value);
	static struct Handle
	{
		const char *aModule;
		union
		{
			const char *aCreateObject;
			HRESULT(STDAPICALLTYPE*CreateObject)(const GUID *clsID, const GUID *interfaceID, void **outObject);
		};
		union
		{
			const char *aGetHandlerProperty2;
			HRESULT(STDAPICALLTYPE*GetHandlerProperty2)(UINT32 formatIndex, PROPID propID, PROPVARIANT *value);
		};
		union
		{
			const char *aGetNumberOfFormats;
			HRESULT(STDAPICALLTYPE*GetNumberOfFormats)(UINT32 *numFormats);
		};
		HMODULE handle;
		operator HMODULE() { return handle; }
	} handle;
	struct Proxy *operator->();
};

#endif

interface Format7zDLL::Interface : Merge7z::Format
{
	Proxy &proxy;
	static Interface *head;
	Interface *next;
	Interface(Proxy &proxy):proxy(proxy), next(head)
	{
		head = this;
	}
	void GetDefaultName(HWND, UString &);
	virtual HRESULT CreateObject(const GUID *interfaceID, void **outObject);
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
		bool passwordIsDefined;
		UString password;
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
