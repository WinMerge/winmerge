#ifndef DECORATE_U

/**
 * @brief define the suffixes to decorate TCHAR width specific module/function names
 */

#ifdef _UNICODE
#define DECORATE_A
#define DECORATE_W "W"
#define DECORATE_U "U"
#else
#define DECORATE_A "A"
#define DECORATE_W
#define DECORATE_U
#endif
#define DECORATE_AW DECORATE_A DECORATE_W

/**
 * @brief stub class to help implement DLL proxies
 */
struct DLLPSTUB
{
	DWORD dwMajorVersion;					// Major version
	DWORD dwMinorVersion;					// Minor version
	DWORD dwBuildNumber;					// Build number
	static void Throw(LPCSTR name, HMODULE, DWORD dwError, BOOL bFreeLibrary);
	HMODULE Load();
};

#endif
