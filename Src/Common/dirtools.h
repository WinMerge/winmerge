// dirtools.h : 
//
/////////////////////////////////////////////////////////////////////////////
#ifndef DIRTOOLS_H
#define DIRTOOLS_H
#include <shlobj.h>

BOOL ChooseFile( CString& strResult, 
				 LPCTSTR szStartPath = NULL, 
				 LPCTSTR szCaption = _T("Open"), 
				 LPCTSTR szFilter = _T("All Files (*.*)|*.*||"), 
				 BOOL bOpenDlg = TRUE);

BOOL GetUNCPath(LPCTSTR szDrivePath, LPTSTR szUNC);

BOOL ChooseFolderFromMyComputer(LPTSTR szBuf, LPCTSTR szTitle =NULL);
BOOL ChooseFolderBelowCurrentFolder(LPTSTR szBuf, LPCTSTR szTitle =NULL); 
BOOL ChooseFolderStartCurrentFolder(LPTSTR szBuf, LPCTSTR szTitle =NULL);  
BOOL ChooseBelowSpecialFolder(LPTSTR szBuf, int idFolder, LPCTSTR szTitle =NULL);

#endif

/////////////////////////////////////////////////////////////////////////////
