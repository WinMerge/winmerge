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
					NeedFindFile = Attributes|CreationTime|LastAccessTime|LastWriteTime|Size|Name
				};
			} *operator->() {return 0;}
			UINT32 Mask;
			UINT32 Attributes;
			FILETIME CreationTime;
			FILETIME LastAccessTime;
			FILETIME LastWriteTime;
			UINT64 Size;
			LPCTSTR Name;
			LPCTSTR FullPath;
		};
		virtual UINT Open() = 0;
		virtual Envelope *Enum(Item &) = 0;
	};
	interface Format
	{
		virtual HRESULT DeCompressArchive(HWND, LPCTSTR path, LPCTSTR folder) = 0;
		virtual HRESULT CompressArchive(HWND, LPCTSTR path, DirItemEnumerator *) = 0;
	};
	Merge7z();
	Format &Format7z;
	Format &ZipHandler;
	Format &RarHandler;
	Format &BZip2Handler;
	Format &TarHandler;
	virtual Format *GuessFormat(LPCTSTR path);
};

