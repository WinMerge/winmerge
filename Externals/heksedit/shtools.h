HRESULT NTAPI CreateLink(LPCTSTR lpszPathObj, LPCTSTR lpszPathLink);
HRESULT NTAPI CreateLink(LPCTSTR lpszPathLink);
HRESULT NTAPI ResolveIt(HWND hwnd, LPCTSTR lpszLinkFile, LPTSTR lpszPath);
HRESULT NTAPI PathsEqual(LPCTSTR, LPCTSTR);
HRESULT NTAPI PathsEqual(LPCTSTR);
void NTAPI GetLongPathNameWin32(LPCTSTR lpszShortPath, LPTSTR lpszLongPath);
