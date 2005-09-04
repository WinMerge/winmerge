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
 *
 * A DLLPSTUB must be embedded in an object followed immediately by a 
 * char array name of the DLL to load
 * Eg
 *  ...
 *  DLLPSTUB stub;
 *  char dllname[50];
 *
 * If dll is not found, DLLPSTUB::Load will throw an exception
 * If any of dwMajorVersion, dwMinorVersion, dwBuildNumber are non-zero
 * the DLLPSTUB::Load will throw an exception (CO_S_NOTALLINTERFACES) unless
 * the dll exports DllGetVersion and reports a version at least as high as
 * requested by these members
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
