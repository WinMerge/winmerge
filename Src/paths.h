#ifndef paths_h_included
#define paths_h_included

typedef enum { DOES_NOT_EXIST, IS_EXISTING_FILE, IS_EXISTING_DIR } PATH_EXISTENCE;
typedef enum { DIRSLASH, NODIRSLASH } DIRSLASH_TYPE;

PATH_EXISTENCE paths_DoesPathExist(LPCTSTR szPath);
void paths_normalize(CString & sPath);
CString paths_GetLongPath(const CString & sPath, DIRSLASH_TYPE dst = NODIRSLASH);
CString paths_GetCurrentDirectory();
TCHAR paths_GetCurrentDriveUpper();
PATH_EXISTENCE GetPairComparability(LPCTSTR pszLeft, LPCTSTR pszRight);
CString ExpandShortcut(CString &inFile);
CString paths_ConcatPath(const CString & path, const CString & subpath);

#endif // paths_h_included
