#if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0500)

//=============================================================================
//
// MULTIMON
// stub module that "stubs" multiple monitor APIs on pre-Memphis Win32 OSes
//
// By using this header your code will work unchanged on Win95,
// you will get back correct values from GetSystemMetrics() for new metrics
// and the new APIs will act like only one display is present.
//
// exactly one source must include this with COMPILE_MULTIMON_STUBS defined
//
//=============================================================================

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

//
// if we are building on Win95/NT4 headers we need to declare this stuff ourselves
//
#ifndef SM_CMONITORS

#define SM_XVIRTUALSCREEN       76
#define SM_YVIRTUALSCREEN       77
#define SM_CXVIRTUALSCREEN      78
#define SM_CYVIRTUALSCREEN      79
#define SM_CMONITORS            80
#define SM_SAMEDISPLAYFORMAT    81

DECLARE_HANDLE(HMONITOR);

#define MONITOR_DEFAULTTONULL       0x00000000
#define MONITOR_DEFAULTTOPRIMARY    0x00000001
#define MONITOR_DEFAULTTONEAREST    0x00000002

#define MONITORINFOF_PRIMARY        0x00000001

typedef struct tagMONITORINFO
{
    DWORD   cbSize;
    RECT    rcMonitor;
    RECT    rcWork;
    DWORD   dwFlags;
} MONITORINFO, *LPMONITORINFO;

#define CCHDEVICENAME 32

#ifdef __cplusplus
typedef struct tagMONITORINFOEX : public tagMONITORINFO
{
  TCHAR       szDevice[CCHDEVICENAME];
} MONITORINFOEX, *LPMONITORINFOEX;
#else
typedef struct
{
  MONITORINFO;
  TCHAR       szDevice[CCHDEVICENAME];
} MONITORINFOEX, *LPMONITORINFOEX;
#endif

typedef BOOL (CALLBACK* MONITORENUMPROC)(HMONITOR, HDC, LPRECT, LPARAM);

#endif  // SM_CMONITORS

#ifndef DISPLAY_DEVICE_ATTACHED_TO_DESKTOP

typedef struct {
    DWORD  cb;
    CHAR   DeviceName[32];
    CHAR   DeviceString[128];
    DWORD  StateFlags;
} DISPLAY_DEVICE;

#define DISPLAY_DEVICE_ATTACHED_TO_DESKTOP 0x00000001
#define DISPLAY_DEVICE_MULTI_DRIVER        0x00000002
#define DISPLAY_DEVICE_PRIMARY_DEVICE      0x00000004
#define DISPLAY_DEVICE_MIRRORING_DRIVER    0x00000008

#endif
#define DISPLAY_DEVICE_VGA                 0x00000010

#ifndef ENUM_CURRENT_SETTINGS
#define ENUM_CURRENT_SETTINGS       ((DWORD)-1)
#define ENUM_REGISTRY_SETTINGS      ((DWORD)-2)
#endif

#undef GetMonitorInfo
#undef GetSystemMetrics
#undef MonitorFromWindow
#undef MonitorFromRect
#undef MonitorFromPoint
#undef EnumDisplayMonitors
#undef EnumDisplayDevices

//
// define this to compile the stubs
// otherwise you get the declarations
//
#ifdef COMPILE_MULTIMON_STUBS

    //---------------------------------------------------------------------------
    //
    // Implement the API stubs.
    //
    //---------------------------------------------------------------------------

    int      (WINAPI* g_pfnGetSystemMetrics)(int);
    HMONITOR (WINAPI* g_pfnMonitorFromWindow)(HWND, BOOL);
    HMONITOR (WINAPI* g_pfnMonitorFromRect)(LPCRECT, BOOL);
    HMONITOR (WINAPI* g_pfnMonitorFromPoint)(POINT, BOOL);
    BOOL     (WINAPI* g_pfnGetMonitorInfo)(HMONITOR, LPMONITORINFO);
    BOOL     (WINAPI* g_pfnEnumDisplayMonitors)(HDC, LPCRECT,
              MONITORENUMPROC, LPARAM);
    BOOL     (WINAPI *g_pfnEnumDisplayDevices)(LPVOID, int, 
              DISPLAY_DEVICE *,DWORD);

    BOOL InitMultipleMonitorStubs(void)
    {
        HMODULE hUser32;
        static BOOL fInitDone;

        if (fInitDone)
        {
            return g_pfnGetMonitorInfo != NULL;
        }

        if ((hUser32 = GetModuleHandle(TEXT("USER32"))) &&
            (*(FARPROC*)&g_pfnGetSystemMetrics    = GetProcAddress(hUser32,"GetSystemMetrics")) &&
            (*(FARPROC*)&g_pfnMonitorFromWindow   = GetProcAddress(hUser32,"MonitorFromWindow")) &&
            (*(FARPROC*)&g_pfnMonitorFromRect     = GetProcAddress(hUser32,"MonitorFromRect")) &&
            (*(FARPROC*)&g_pfnMonitorFromPoint    = GetProcAddress(hUser32,"MonitorFromPoint")) &&
            (*(FARPROC*)&g_pfnEnumDisplayMonitors = GetProcAddress(hUser32,"EnumDisplayMonitors")) &&
    #ifdef UNICODE
            (*(FARPROC*)&g_pfnGetMonitorInfo      = GetProcAddress(hUser32,"GetMonitorInfoW")) &&
            (*(FARPROC*)&g_pfnEnumDisplayDevices  = GetProcAddress(hUser32,"EnumDisplayDevicesW")) &&
    #else
            (*(FARPROC*)&g_pfnGetMonitorInfo      = GetProcAddress(hUser32,"GetMonitorInfoA")) &&
            (*(FARPROC*)&g_pfnEnumDisplayDevices  = GetProcAddress(hUser32,"EnumDisplayDevicesA")) &&
    #endif
            (GetSystemMetrics(SM_CXVIRTUALSCREEN) >= GetSystemMetrics(SM_CXSCREEN)) &&
            (GetSystemMetrics(SM_CYVIRTUALSCREEN) >= GetSystemMetrics(SM_CYSCREEN)) )
        {
            fInitDone = TRUE;
            return TRUE;
        }
        else
        {
            g_pfnGetSystemMetrics    = NULL;
            g_pfnMonitorFromWindow   = NULL;
            g_pfnMonitorFromRect     = NULL;
            g_pfnMonitorFromPoint    = NULL;
            g_pfnGetMonitorInfo      = NULL;
            g_pfnEnumDisplayMonitors = NULL;
            g_pfnEnumDisplayDevices  = NULL;

            fInitDone = TRUE;
            return FALSE;
        }
    }

    //---------------------------------------------------------------------------
    //
    // "stubbed" implementations of Monitor APIs that work with the primary         //  display
    //
    //---------------------------------------------------------------------------

    int WINAPI
    xGetSystemMetrics(int nIndex)
    {
        if (InitMultipleMonitorStubs())
            return g_pfnGetSystemMetrics(nIndex);

        switch (nIndex)
        {
        case SM_CMONITORS:
        case SM_SAMEDISPLAYFORMAT:
            return 1;

        case SM_XVIRTUALSCREEN:
        case SM_YVIRTUALSCREEN:
            return 0;

        case SM_CXVIRTUALSCREEN:
            nIndex = SM_CXSCREEN;
            break;

        case SM_CYVIRTUALSCREEN:
            nIndex = SM_CYSCREEN;
            break;
        }

        return GetSystemMetrics(nIndex);
    }

    #define xPRIMARY_MONITOR ((HMONITOR)0x42)

    HMONITOR WINAPI
    xMonitorFromRect(LPCRECT lprcScreenCoords, UINT uFlags)
    {
        if (InitMultipleMonitorStubs())
            return g_pfnMonitorFromRect(lprcScreenCoords, uFlags);

        if ((uFlags & (MONITOR_DEFAULTTOPRIMARY | MONITOR_DEFAULTTONEAREST)) ||
            ((lprcScreenCoords->right > 0) &&
            (lprcScreenCoords->bottom > 0) &&
            (lprcScreenCoords->left < GetSystemMetrics(SM_CXSCREEN)) &&
            (lprcScreenCoords->top < GetSystemMetrics(SM_CYSCREEN))))
        {
            return xPRIMARY_MONITOR;
        }

        return NULL;
    }

    HMONITOR WINAPI
    xMonitorFromWindow(HWND hWnd, UINT uFlags)
    {
        RECT rc;

        if (InitMultipleMonitorStubs())
            return g_pfnMonitorFromWindow(hWnd, uFlags);

        if (uFlags & (MONITOR_DEFAULTTOPRIMARY | MONITOR_DEFAULTTONEAREST))
            return xPRIMARY_MONITOR;

        if (GetWindowRect(hWnd, &rc))
            return xMonitorFromRect(&rc, uFlags);

        return NULL;
    }

    HMONITOR WINAPI
    xMonitorFromPoint(POINT ptScreenCoords, UINT uFlags)
    {
        if (InitMultipleMonitorStubs())
            return g_pfnMonitorFromPoint(ptScreenCoords, uFlags);

        if ((uFlags & (MONITOR_DEFAULTTOPRIMARY | MONITOR_DEFAULTTONEAREST)) ||
            ((ptScreenCoords.x >= 0) &&
            (ptScreenCoords.x < GetSystemMetrics(SM_CXSCREEN)) &&
            (ptScreenCoords.y >= 0) &&
            (ptScreenCoords.y < GetSystemMetrics(SM_CYSCREEN))))
        {
            return xPRIMARY_MONITOR;
        }

        return NULL;
    }

    BOOL WINAPI
    xGetMonitorInfo(HMONITOR hMonitor, LPMONITORINFO lpMonitorInfo)
    {
        RECT rcWork;

        if (InitMultipleMonitorStubs())
            return g_pfnGetMonitorInfo(hMonitor, lpMonitorInfo);

        if ((hMonitor == xPRIMARY_MONITOR) && lpMonitorInfo &&
            (lpMonitorInfo->cbSize >= sizeof(MONITORINFO)) &&
            SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0))
        {
            lpMonitorInfo->rcMonitor.left = 0;
            lpMonitorInfo->rcMonitor.top  = 0;
            lpMonitorInfo->rcMonitor.right  = GetSystemMetrics(SM_CXSCREEN);
            lpMonitorInfo->rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
            lpMonitorInfo->rcWork = rcWork;
            lpMonitorInfo->dwFlags = MONITORINFOF_PRIMARY;

            if (lpMonitorInfo->cbSize >= sizeof(MONITORINFOEX))
                lstrcpy(((MONITORINFOEX*)lpMonitorInfo)->szDevice,
                TEXT("DISPLAY"));

            return TRUE;
        }

        return FALSE;
    }

    BOOL WINAPI
    xEnumDisplayMonitors(
        HDC hdc,
        LPCRECT lprcIntersect, 
        MONITORENUMPROC lpfnEnumProc,
        LPARAM lData)
    {
        RECT rcCallback, rcLimit;

        if (InitMultipleMonitorStubs())
            return g_pfnEnumDisplayMonitors(hdc, lprcIntersect, lpfnEnumProc,
                                           lData);
    
        if (!lpfnEnumProc)
            return FALSE;

        rcLimit.left   = 0;
        rcLimit.top    = 0;
        rcLimit.right  = GetSystemMetrics(SM_CXSCREEN);
        rcLimit.bottom = GetSystemMetrics(SM_CYSCREEN);

        if (hdc)
        {
            RECT rcClip;
            HWND hWnd;

            if ((hWnd = WindowFromDC(hdc)) == NULL)
                return FALSE;

            switch (GetClipBox(hdc, &rcClip))
            {
            default:
                MapWindowPoints(NULL, hWnd, (LPPOINT)&rcLimit, 2);
                if (IntersectRect(&rcCallback, &rcClip, &rcLimit))
                    break;
                //fall thru
            case NULLREGION:
                 return TRUE;
            case ERROR:
                 return FALSE;
            }

            rcLimit = rcCallback;
        }

        if (!lprcIntersect ||
            IntersectRect(&rcCallback, lprcIntersect, &rcLimit))
        {
            lpfnEnumProc(xPRIMARY_MONITOR, hdc, &rcCallback, lData);
        }

        return TRUE;
    }

    BOOL WINAPI
    xEnumDisplayDevices(LPVOID lpReserved, int iDeviceNum, 
DISPLAY_DEVICE * pDisplayDevice, DWORD dwFlags)
    {
        if (InitMultipleMonitorStubs())
            return g_pfnEnumDisplayDevices(lpReserved, iDeviceNum,
                                           pDisplayDevice, dwFlags);
    
        return FALSE;
    }

    #undef xPRIMARY_MONITOR
    #undef COMPILE_MULTIMON_STUBS

#else    // COMPILE_MULTIMON_STUBS

    extern int    WINAPI xGetSystemMetrics(int);
    extern HMONITOR WINAPI xMonitorFromWindow(HWND, UINT);
    extern HMONITOR WINAPI xMonitorFromRect(LPCRECT, UINT);
    extern HMONITOR WINAPI xMonitorFromPoint(POINT, UINT);
    extern BOOL    WINAPI xGetMonitorInfo(HMONITOR, LPMONITORINFO);
    extern BOOL    WINAPI xEnumDisplayMonitors(HDC, LPCRECT, MONITORENUMPROC,
                                               LPARAM);
    extern BOOL WINAPI xEnumDisplayDevices(LPVOID, int, DISPLAY_DEVICE *,DWORD);

#endif    // COMPILE_MULTIMON_STUBS

//
// build defines that replace the regular APIs with our versions
//
#define GetSystemMetrics    xGetSystemMetrics
#define MonitorFromWindow   xMonitorFromWindow
#define MonitorFromRect     xMonitorFromRect
#define MonitorFromPoint    xMonitorFromPoint
#define GetMonitorInfo      xGetMonitorInfo
#define EnumDisplayMonitors xEnumDisplayMonitors
#define EnumDisplayDevices    xEnumDisplayDevices

#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif  /* !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0500) */