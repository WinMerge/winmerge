#ifndef DllBuild_Merge7z
#define DllBuild_Merge7z 0	// traditional interfaces only
#endif

#if DllBuild_Merge7z >= 9
#define DllBuild_Merge7z_9 public
#else
#define DllBuild_Merge7z_9 private
#endif

interface Merge7z
{
	struct Proxy
	{
		LPCSTR Merge7z[2];
		HMODULE handle;
		interface Merge7z *operator->();
	};
	struct Initialize
	{
		enum
		{
			Unicode = 1,
			Local7z = 2,
			Default = sizeof(TCHAR) == sizeof(WCHAR) ? Unicode : 0
		};
	};
	virtual int Initialize(DWORD = Initialize::Default);
	interface Envelope
	{
		virtual void Free() = 0;
	};
	interface DirItemEnumerator
	{
		struct Item
		{
			struct
			{
				enum
				{
					Attributes = 1,
					CreationTime = 2,
					LastAccessTime = 4,
					LastWriteTime = 8,
					Size = 16,
					Name = 32,
					FullPath = 64,
					CheckIfPresent = 128,
					Recurse = 256,
					NeedFindFile = Attributes|CreationTime|LastAccessTime|LastWriteTime|Size|Name
				};
				UINT32 Item;
			} Mask;
			UINT32 Attributes;
			FILETIME CreationTime;
			FILETIME LastAccessTime;
			FILETIME LastWriteTime;
			UINT64 Size;
			LPCTSTR Name;
			LPCTSTR FullPath;
		};
		virtual UINT32 Open() = 0;
		virtual Envelope *Enum(Item &) = 0;
	};
	interface Format
	{
		virtual HRESULT DeCompressArchive(HWND, LPCTSTR path, LPCTSTR folder) = 0;
		virtual HRESULT CompressArchive(HWND, LPCTSTR path, DirItemEnumerator *) = 0;
	DllBuild_Merge7z_9:
		interface Inspector
		{
			virtual void Free() = 0;
			virtual UINT32 Open() = 0;
			virtual HRESULT Extract(HWND, LPCTSTR folder, const UINT32 *indices = 0, UINT32 numItems = -1) = 0;
			virtual HRESULT GetProperty(UINT32, PROPID, PROPVARIANT *, VARTYPE) = 0;
			virtual BSTR GetPath(UINT32) = 0;
			virtual BSTR GetName(UINT32) = 0;
			virtual BSTR GetExtension(UINT32) = 0;
			virtual VARIANT_BOOL IsFolder(UINT32) = 0;
			virtual FILETIME LastWriteTime(UINT32) = 0;
		};
		virtual Inspector *Open(HWND, LPCTSTR) = 0;
		interface Updater
		{
			virtual void Free() = 0;
			virtual UINT32 Add(DirItemEnumerator::Item &) = 0;
			virtual HRESULT Commit(HWND) = 0;
		};
		virtual Updater *Update(HWND, LPCTSTR) = 0;
	};
	Merge7z();
	Format &Format7z;
	Format &ZipHandler;
	Format &RarHandler;
	Format &BZip2Handler;
	Format &TarHandler;
	virtual Format *GuessFormat(LPCTSTR);
DllBuild_Merge7z_9:
	virtual LPCTSTR LoadLang(LPCTSTR);
};
