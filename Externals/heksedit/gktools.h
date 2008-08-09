#ifndef gktools_h
#define gktools_h

BOOL WINAPI GetDllExportNames( LPCSTR pszFilename, ULONG* lpulOffset, ULONG* lpulSize );
BOOL WINAPI GetDllImportNames( LPCSTR pszFilename, ULONG* lpulOffset, ULONG* lpulSize );

struct s_MEMORY_ENCODING;

typedef void (WINAPI* LPFNEncodeMemoryFunction)( s_MEMORY_ENCODING* p );

typedef struct s_MEMORY_ENCODING
{
	LPBYTE lpbMemory;
	DWORD dwSize;
	LPCSTR lpszArguments;
	BOOL bEncode;
	LPFNEncodeMemoryFunction fpEncodeFunc;
} MEMORY_CODING, *LPMEMORY_CODING;

typedef struct _MEMORY_CODING_DESCRIPTION
{
	LPCSTR lpszDescription;
	LPFNEncodeMemoryFunction fpEncodeFunc;
} MEMORY_CODING_DESCRIPTION, *LPMEMORY_CODING_DESCRIPTION;

EXTERN_C LPMEMORY_CODING_DESCRIPTION WINAPI GetMemoryCodings();
typedef LPMEMORY_CODING_DESCRIPTION (WINAPI* LPFNGetMemoryCodings)();

extern PartitionInfo* SelectedPartitionInfo;

BOOL WINAPI GetMemoryCoding( HINSTANCE hInstance, HWND hParent, LPMEMORY_CODING p, LPCSTR lpszDlls );

class EncodeDecodeDialog : public HexEditorWindow
{
public:
	enum { IDD = IDD_ENCODE_DECODE_DIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
};

class OpenDriveDialog : public HexEditorWindow
{
public:
	enum { IDD = IDD_OPEN_DRIVE_DIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
};

class GotoTrackDialog : public HexEditorWindow
{
public:
	enum { IDD = IDD_GOTO_TRACK_DIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
};


#endif // gktools_h
