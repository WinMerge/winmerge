struct DllProxy_ModuleState
{
//	error messages exposed here just in case anybody desires localization...
	struct
	{
		LPCSTR LoadLibrary;		// the DLL could not be loaded
		LPCSTR GetProcAddress;	// the DLL is lacking a function
		LPCSTR Invoke;			// attempt to call a missing function
	} Complain;
	VOID(NTAPI*Load)(HMODULE*);
	FARPROC Unresolved;			// placeholder for missing functions
};

#ifndef POLARITY
#define POLARITY
#endif

EXTERN_C struct DllProxy_ModuleState POLARITY DllProxy_ModuleState;

#define DLLPROXY(NAME) (DllProxy_ModuleState.Load(&NAME.DLL),(struct NAME*)&NAME)

#define EXPORT_DLLPROXY(NAME,MODULE,P) \
	struct NAME{HMODULE DLL;P}; \
	struct {HMODULE DLL;CHAR SIG[sizeof#P MODULE + (sizeof MODULE == 1 ? MAX_PATH : 0)];} NAME = {0, #P MODULE};

#define IMPORT_DLLPROXY(NAME,MODULE,P) \
	struct NAME{HMODULE DLL;P}; \
	extern struct {HMODULE DLL;CHAR SIG[sizeof#P MODULE + (sizeof MODULE == 1 ? MAX_PATH : 0)];} NAME;

#define DLL_EXPORT_DLLPROXY(NAME,MODULE,P) \
	struct NAME {HMODULE DLL;P}; \
	struct {HMODULE DLL;CHAR SIG[sizeof#P MODULE + (sizeof MODULE == 1 ? MAX_PATH : 0)];} __declspec(dllexport) NAME = {0, #P MODULE};

#define DLL_IMPORT_DLLPROXY(NAME,MODULE,P) \
	struct NAME{HMODULE DLL;P}; \
	extern struct {HMODULE DLL;CHAR SIG[sizeof#P MODULE + (sizeof MODULE == 1 ? MAX_PATH : 0)];} __declspec(dllimport) NAME;

#undef POLARITY
