HRESULT CreateLink(LPCSTR lpszPathObj, LPCSTR lpszPathLink);
HRESULT ResolveIt(HWND hwnd, LPCSTR lpszLinkFile, LPSTR lpszPath);
HRESULT PathsEqual(LPCSTR, LPCSTR);
void GetLongPathNameWin32(LPCSTR lpszShortPath, LPSTR lpszLongPath);
