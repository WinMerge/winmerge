/* dirtools.cpp : implementation of the ::SHBrowseForFolder function

PROTOTYPES:
	ChooseFile
	GetUNCPath
	ChooseFolderFromMyComputer
	ChooseFolderBelowCurrentFolder
	ChooseFolderStartCurrentFolder 
	ChooseFolderBelowProgramsFolder

DEPENDENCIES:
	mpr.lib
*/

#include "stdafx.h"
#include "dirtools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL ChooseFile( CString& strResult, 
				 LPCTSTR szStartPath /* = NULL */, 
				 LPCTSTR szCaption /* = "Open" */, 
				 LPCTSTR szFilter /* = "All Files (*.*)|*.*||" */, 
				 BOOL bOpenDlg /* = TRUE */) 
// displays a shell file selector
{
	CFileDialog dlg(bOpenDlg, NULL, NULL, 
				    OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | (bOpenDlg? OFN_FILEMUSTEXIST:0) , szFilter);
	dlg.m_ofn.lpstrTitle = (LPTSTR)szCaption;
	dlg.m_ofn.lpstrInitialDir = (LPTSTR)szStartPath;
	if (dlg.DoModal()==IDOK)
	{
	 	strResult = dlg.GetPathName(); 
		return TRUE;
	}
	strResult = _T("");
	return FALSE;	   
}


BOOL GetUNCPath(LPCTSTR szDrivePath, LPTSTR szUNC)
// converts a filename from x:\... style to \\computer\share... style
{
	TCHAR pBuf[MAX_PATH];
	DWORD dwSize = sizeof(pBuf);

	switch (WNetGetUniversalName(szDrivePath, UNIVERSAL_NAME_INFO_LEVEL, pBuf, &dwSize))
	{
    case  NO_ERROR:
		_tcscpy(szUNC, ((UNIVERSAL_NAME_INFO*)pBuf)->lpUniversalName);
		TRACE(_T("UNC name = %s\n"),  ((UNIVERSAL_NAME_INFO*)pBuf)->lpUniversalName);
		break;
    case ERROR_NOT_CONNECTED: // Local
		TRACE(_T("GetUNCPath: Path is not redirected\n"));
		*szUNC = _T('\0');
		break;
    default:
		TRACE(_T("GetUNCPath: Error\n"));
		*szUNC = _T('\0');
		break;
	}
	return (*szUNC != _T('\0'));
}




int CALLBACK BrowseCallback(HWND hWnd, UINT uMsg, LPARAM /*lParam*/, LPARAM lpData)
{
    if (uMsg == BFFM_INITIALIZED)
    {
        SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lpData);
    }
    return 0;
}



////////////////////////////////////////////////
// basic browse from Listing A in article
////////////////////////////////////////////////
BOOL ChooseFolderFromMyComputer(LPTSTR szBuf, LPCTSTR szTitle /*=NULL*/) 
{
    // Retrieve the task memory allocator.
    LPMALLOC pIMalloc;
    if (!SUCCEEDED(::SHGetMalloc(&pIMalloc)))
	{
		TRACE(_T("SHGetMalloc failed.\n"));
		return FALSE;
	}

    // Initialize a BROWSEINFO structure,
    BROWSEINFO brInfo;
    ::ZeroMemory(&brInfo, sizeof(brInfo));

    brInfo.hwndOwner =
                  AfxGetMainWnd()->GetSafeHwnd();
    brInfo.pidlRoot = NULL;
    brInfo.pszDisplayName = szBuf;
    brInfo.lpszTitle = szTitle;
	
	// only want folders (no printers, etc.)
    brInfo.ulFlags = BIF_RETURNONLYFSDIRS;

    // Display the browser.
    ITEMIDLIST* browseList = NULL;
    browseList = ::SHBrowseForFolder(&brInfo);

    // if the user selected a folder . . .
    if (browseList)
    {
        // Convert the item ID to a pathname, 
        //char szBuf[MAX_PATH];
        if(::SHGetPathFromIDList(browseList, 
                                 szBuf))
        {
            //AfxMessageBox(szBuf);
            TRACE1("You chose==>%s\n",szBuf);

        }

        // Free the PIDL
        pIMalloc->Free(browseList);
    }
    else
    {
		*szBuf=_T('\0');
        //AfxMessageBox("Canceled browse.");
    }

    // Decrement ref count on the allocator
    pIMalloc->Release();
	return (*szBuf != _T('\0')); 
}



////////////////////////////////////////////////
// allow user to browse only the current folder 
// and those below it
////////////////////////////////////////////////
BOOL ChooseFolderBelowCurrentFolder(LPTSTR szBuf, LPCTSTR szTitle /*=NULL*/) 
{
    // Retrieve the task memory allocator.
    LPMALLOC pIMalloc;
    if (!SUCCEEDED(::SHGetMalloc(&pIMalloc)))
	{
		TRACE(_T("SHGetMalloc failed.\n"));
		return FALSE;
	}

    // Initialize a BROWSEINFO structure,
    BROWSEINFO brInfo;
    ::ZeroMemory(&brInfo, sizeof(brInfo));

    brInfo.hwndOwner = 
                  AfxGetMainWnd()->GetSafeHwnd();
    brInfo.pidlRoot = NULL;
    brInfo.pszDisplayName = szBuf;
    brInfo.lpszTitle = szTitle;
    brInfo.ulFlags = BIF_RETURNONLYFSDIRS;

    // Prepare to create a PIDL 
    LPSHELLFOLDER pIShell;
    TCHAR    szPath[MAX_PATH];
    OLECHAR szOlePath[MAX_PATH];

    // get dir & convert to unicode
    ::GetCurrentDirectory(MAX_PATH, szPath);
#ifndef _UNICODE
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,
                        szPath, -1, 
                        szOlePath, MAX_PATH);
#else
	_tcscpy(szOlePath, szPath);
#endif

    // Get Desktop's IShellFolder interface
    if(FAILED(::SHGetDesktopFolder(&pIShell)))
    {
        TRACE0("Can't get desktop folder\n");
        return FALSE;
    }
    
    LPITEMIDLIST  pidlRoot=NULL;
    ULONG         pchEaten;
    ULONG         dwAttr;

    // convert the path to an item id list & browse
    if(SUCCEEDED(::SHGetMalloc(&pIMalloc)))
    {
        if(SUCCEEDED(pIShell->ParseDisplayName(
                            AfxGetMainWnd()->GetSafeHwnd(),
                            NULL,
                            szOlePath,
                            &pchEaten,
                            &pidlRoot,
                            &dwAttr)))
        {
            brInfo.pidlRoot = pidlRoot;  
        }
    }

    // Display the browser.
    ITEMIDLIST* browseList = NULL;
    browseList = ::SHBrowseForFolder(&brInfo);

    // if the user selected a folder . . .
    if (browseList)
    {
        // Convert the item ID to a pathname, 
        //char szBuf[MAX_PATH];
        if(::SHGetPathFromIDList(browseList, 
                                 szBuf))
        {
            //AfxMessageBox(szBuf);
            TRACE1("You chose==>%s\n",szBuf);
        }

        // Free the PIDL
        pIMalloc->Free(browseList);
    }
    else
    {
		*szBuf=_T('\0');
        //AfxMessageBox("Canceled browse.");
    }
    
	pIMalloc->Free(pidlRoot);
    pIShell->Release();
    pIMalloc->Release();
	return (*szBuf != _T('\0'));
}


// start the browse in the current directory
// but allow user to browse entire namespace
BOOL ChooseFolderStartCurrentFolder(LPTSTR szBuf, LPCTSTR szTitle /*=NULL*/)  
{
    // Retrieve the task memory allocator.
    LPMALLOC pIMalloc;
    if (!SUCCEEDED(::SHGetMalloc(&pIMalloc)))
	{
		TRACE(_T("SHGetMalloc failed.\n"));
		return FALSE;
	}

    // Initialize a BROWSEINFO structure,
    BROWSEINFO brInfo;
    ::ZeroMemory(&brInfo, sizeof(brInfo));

    brInfo.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
    brInfo.pidlRoot = NULL;
    brInfo.pszDisplayName = szBuf;
    brInfo.lpszTitle = szTitle;
    brInfo.ulFlags = BIF_RETURNONLYFSDIRS;

    // get current dir
    TCHAR lpszHere[MAX_PATH];
    ::GetCurrentDirectory(MAX_PATH, lpszHere);
    brInfo.lParam = (LPARAM)lpszHere;

    // set callback address
    brInfo.lpfn = BrowseCallback;

    // Display the browser.
    ITEMIDLIST* browseList = NULL;
    browseList = ::SHBrowseForFolder(&brInfo);

    // if the user selected a folder . . .
    if (browseList)
    {
        // Convert the item ID to a pathname, 
        //char szBuf[MAX_PATH];
        if(::SHGetPathFromIDList(browseList, 
                                 szBuf))
        {
            //AfxMessageBox(szBuf);
            TRACE1("You chose==>%s\n",szBuf);
        }

        // Free the PIDL
        pIMalloc->Free(browseList);
    }
    else
    {
		*szBuf=_T('\0');
        //AfxMessageBox("Canceled browse.");
    }
    pIMalloc->Release();
	return (*szBuf != _T('\0')); 
}



////////////////////////////////////////////////
//  Browse a system folder. 
//  Not discussed in article
////////////////////////////////////////////////
BOOL ChooseBelowSpecialFolder(LPTSTR szBuf, int idFolder, LPCTSTR szTitle /*=NULL*/)
// common values for idFolder are CSIDL_DRIVES, CSIDL_NETWORK, CSIDL_PERSONAL
{
    LPMALLOC pIMalloc;
    if(!SUCCEEDED(::SHGetMalloc(&pIMalloc)))
    {
		TRACE(_T("SHGetMalloc failed.\n"));
		return FALSE;
	}

    // Initialize a BROWSEINFO structure,
    BROWSEINFO brInfo;
    ::ZeroMemory(&brInfo, sizeof(brInfo));

    brInfo.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
    brInfo.pidlRoot = NULL;
    brInfo.pszDisplayName = szBuf;
    brInfo.lpszTitle = szTitle;
    brInfo.ulFlags = BIF_RETURNONLYFSDIRS;

    // build a pidl for a system folder
    LPITEMIDLIST pidlRoot;
	if(!SUCCEEDED(SHGetSpecialFolderLocation(
           AfxGetMainWnd()->GetSafeHwnd(),
           idFolder,
           &pidlRoot)))
	{
		TRACE(_T("SHGetSpecialFolderLocation failed.\n"));
		pIMalloc->Release();
		return FALSE;
	}
    brInfo.pidlRoot = pidlRoot;

	    // Display the browser.
    ITEMIDLIST* browseList = NULL;
    browseList = ::SHBrowseForFolder(&brInfo);

    // if the user selected a folder . . .
    if (browseList)
    {
        // Convert the item ID to a pathname, 
        //char szBuf[MAX_PATH];
        if(::SHGetPathFromIDList(browseList, 
                                 szBuf))
        {
            //AfxMessageBox(szBuf);
            TRACE1("You chose==>%s\n",szBuf);
        }

        // Free the PIDL
        pIMalloc->Free(browseList);
    }
    else
    {
		*szBuf=_T('\0');
        //AfxMessageBox("Canceled browse.");
    }

    pIMalloc->Free(pidlRoot);
    pIMalloc->Release();
	return (*szBuf != _T('\0')); 
}


