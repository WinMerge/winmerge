#ifndef dllver_h_included
#define dllver_h_included

#ifndef PACKVERSION
#define PACKVERSION(major,minor) MAKELONG(minor,major)
#endif

DWORD getFixedModuleVersion(HMODULE hmod);
DWORD GetDllVersion_Raw(LPCTSTR lpszDllName); // checks very time
DWORD GetDllVersion(LPCTSTR lpszDllName); // checks once & caches

#endif // dllver_h_included