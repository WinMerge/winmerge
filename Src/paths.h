#ifndef paths_h_included
#define paths_h_included

typedef enum { DOES_NOT_EXIST, IS_EXISTING_FILE, IS_EXISTING_DIR } PATH_EXISTENCE;

PATH_EXISTENCE paths_DoesPathExist(LPCTSTR szPath);
void paths_normalize(CString & sPath);
CString paths_GetLongPath(const CString & sPath);
TCHAR paths_GetCurrentDrive();

#endif // paths_h_included
