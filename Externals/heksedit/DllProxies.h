struct DllProxy
{
	LPCSTR Names[1];
	LPVOID Load() throw();
	LPVOID EnsureLoad();
	void FormatMessage(LPTSTR);
	template<class T> struct Instance;
};

template<class T> struct DllProxy::Instance
{
	union
	{
		struct
		{
			LPCSTR Names[sizeof(T) / sizeof(LPCSTR)];
		} Names;
		DllProxy Proxy;
	};
	HMODULE H;
	operator T *() throw()
	{
		return (T *)Proxy.Load();
	}
	T *operator->()
	{
		return (T *)Proxy.EnsureLoad();
	}
};

/**
 * @brief RAWIO32 dll proxy
 */
struct SectorInfo;
struct ExtDriveInfo;
struct BlockInfo;

struct RAWIO32
{
	DWORD(WINAPI*ResetDisk)(SectorInfo *);
	DWORD(WINAPI*ReadPhysicalSector)(SectorInfo *, LPBYTE, DWORD);
	DWORD(WINAPI*WritePhysicalSector)(SectorInfo *, LPBYTE, DWORD);
	DWORD(WINAPI*ReadDiskGeometry)(SectorInfo *);
	DWORD(WINAPI*EI13GetDriveParameters)(ExtDriveInfo *);
	DWORD(WINAPI*EI13ReadSector)(BlockInfo *, LPBYTE, DWORD);
	DWORD(WINAPI*EI13WriteSector)(BlockInfo *, LPBYTE, DWORD);
	HMODULE H;
};

extern DllProxy::Instance<struct RAWIO32> RAWIO32;

/**
 * @brief IMAGEHLP dll proxy
 */
typedef struct _LOADED_IMAGE *PLOADED_IMAGE;

struct IMAGEHLP
{
	BOOL(NTAPI*UnMapAndLoad)(PLOADED_IMAGE);
	PVOID(NTAPI*ImageRvaToVa)(PIMAGE_NT_HEADERS, PVOID, ULONG, PIMAGE_SECTION_HEADER);
	BOOL(NTAPI*MapAndLoad)(PSTR, PSTR, PLOADED_IMAGE, BOOL, BOOL);
	HMODULE H;
};

extern DllProxy::Instance<struct IMAGEHLP> IMAGEHLP;
