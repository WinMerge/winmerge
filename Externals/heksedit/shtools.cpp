#include "precomp.h"
#include "shtools.h"
/*Pabs inserted func
Create link files for registry menu
Copied from compiler documentation. - had to fix up some stuff - totally wrong parameters used - all freaky compile time errors
CreateLink - uses the shell's IShellLink and IPersistFile interfaces to create and store a shortcut to the specified object.
Returns the result of calling the member functions of the interfaces.
lpszPathObj - address of a buffer containing the path of the object.
lpszPathLink - address of a buffer containing the path where the shell link is to be stored.*/
HRESULT CreateLink(LPCSTR lpszPathObj, LPCSTR lpszPathLink)
{
	HRESULT hres;
	IShellLink* psl;

	// Get a pointer to the IShellLink interface.

	hres = CoCreateInstance(CLSID_ShellLink, NULL,
		CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&psl);
	if (SUCCEEDED(hres)) {
		IPersistFile* ppf;

		// Set the path to the shortcut target
		psl->SetPath(lpszPathObj);

		// Query IShellLink for the IPersistFile interface for saving the
		// shortcut in persistent storage.
		hres = psl->QueryInterface(IID_IPersistFile,(void **)&ppf);

		if (SUCCEEDED(hres)) {
			WCHAR wsz[MAX_PATH];

			//Bugfix - create the dir before saving the file because IPersistFile::Save won't
			hres = 1;
			char* tmp;
			*(tmp = PathFindFileName(lpszPathLink)-1) = 0;//Remove filename
			if(!PathIsDirectory(lpszPathLink))hres = CreateDirectory(lpszPathLink,NULL);
			*tmp = '\\';

			if (hres) {

				// Ensure that the string is ANSI.
				MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1,wsz, MAX_PATH);

				// Save the link by calling IPersistFile::Save.
				hres = ppf->Save(wsz, TRUE);

			}

			ppf->Release();
		}
		psl->Release();
	}
	return hres;
}

HRESULT ResolveIt(HWND hwnd, LPCSTR lpszLinkFile, LPSTR lpszPath)
{
	HRESULT hres;
	IShellLink* psl;
	char szGotPath[MAX_PATH];
	char szDescription[MAX_PATH];
	WIN32_FIND_DATA wfd;

	*lpszPath = 0; // assume failure

	// Get a pointer to the IShellLink interface.
	hres = CoCreateInstance( CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&psl );
	if (SUCCEEDED(hres))
	{
		IPersistFile* ppf;

		// Get a pointer to the IPersistFile interface.
		hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
		if (SUCCEEDED(hres))
		{
			WCHAR wsz[MAX_PATH];

			// Ensure that the string is Unicode.
			MultiByteToWideChar(CP_ACP, 0, lpszLinkFile, -1, wsz, MAX_PATH);

			// Load the shortcut.
			hres = ppf->Load(wsz, STGM_READ);
			if (SUCCEEDED(hres))
			{
				// Resolve the link.
				hres = psl->Resolve(hwnd, SLR_ANY_MATCH);
				if (SUCCEEDED(hres))
				{
					// Get the path to the link target.
					hres = psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA *)&wfd, SLGP_SHORTPATH );
					if (!SUCCEEDED(hres) )
					{
						// application-defined function
					}

					// Get the description of the target.
					hres = psl->GetDescription(szDescription, MAX_PATH);
					if (!SUCCEEDED(hres))
					{
						// HandleErr(hres);
					}
					lstrcpy(lpszPath, szGotPath);
				}
			}
			// Release the pointer to the IPersistFile interface.
			ppf->Release();
		}
		// Release the pointer to the IShellLink interface.
		psl->Release();
	}
	else
	{
	}
	return hres;
}

//Pabs inserted
//Parts copied from compiler docs - search for ITEMIDLIST in title in msdn
//Adapted from Q132750:"HOWTO: Convert a File Path to an ITEMIDLIST" in the Knowledge Base
HRESULT PathsEqual(LPCSTR p0, LPCSTR p1)
{
	LPSHELLFOLDER pFolder;
	HRESULT hr;
	if (SUCCEEDED(hr = SHGetDesktopFolder(&pFolder)))
	{
		OLECHAR olePath[2][MAX_PATH];
		// IShellFolder::ParseDisplayName requires the file name be in Unicode.
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, p0, -1, olePath[0], MAX_PATH);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, p1, -1, olePath[1], MAX_PATH);
		LPITEMIDLIST pidl[2];
		ULONG chEaten;//only needed by parse dispname
		// Convert the paths to ITEMIDLISTs.
		if (SUCCEEDED(hr = pFolder->ParseDisplayName(NULL, NULL,
				olePath[0], &chEaten, &pidl[0], NULL)) &&
			SUCCEEDED(hr = pFolder->ParseDisplayName(NULL, NULL,
				olePath[1], &chEaten, &pidl[1], NULL)))
		{
			hr = pFolder->CompareIDs(0,pidl[0],pidl[1]);
		}

		//free ITEMIDLISTs
		IMalloc* pm;
		SHGetMalloc(&pm);
		pm->Free(pidl[0]);
		pm->Free(pidl[1]);
		pm->Release();

		pFolder->Release();
	}
	return hr;
}

/**
 * @brief Convert from Ansi to Unicode using given codepage.
 * @param [in] pstr String to convert.
 * @param [in] codepage Codepage to use in conversion.
 * @return Unicode string.
 * @note Caller must CoTaskMemFree().
 */
static LPWSTR AllocTaskString(LPCSTR pstr, UINT codepage)
{
	LPWSTR wstr = 0;
	if (int len = MultiByteToWideChar(codepage, 0, pstr, -1, 0, 0))
	{
		wstr = (LPWSTR)CoTaskMemAlloc(len * sizeof(WCHAR));
		if (wstr)
			MultiByteToWideChar(codepage, 0, pstr, -1, wstr, len);
	}
	return wstr;
}

//This gets a fully qualified long absolute filename from any other type of file name ON ANY Win32 PLATFORM damn stupid Micro$uck$ & bloody GetLongPathName
//It was copied and enhanced from an article by Jeffrey Richter available in the MSDN under "Periodicals\Periodicals 1997\Microsoft Systems Journal\May\Win32 Q & A" (search for GetLongPathName & choose the last entry)
void GetLongPathNameWin32(LPCSTR lpszShortPath, LPSTR lpszLongPath)
{
	/*Alternative methods to consider adding here
	GetLongPathName on Win98/NT5
	Recursive FindFirstFile ... FindClose calls
	Interrupt 21h Function 7160h Minor Code 2h
	- does exactly what we want
		- to the damn letter
		- even resolves SUBST's (or not if you only want the drive letter)
			- remember those from DOS?
	- but the MSDN doesn't say anything about whether or not it is supported
	  by WinNT4 (It is only listed in Win95 docs)*/

	//Make sure it is an absolute path
	if (_fullpath(lpszLongPath, lpszShortPath, MAX_PATH))
	{
		//Allocate memory for the WCHAR version of the short path name
		if (LPWSTR pszShortPathNameW = AllocTaskString(lpszLongPath, CP_ACP))
		{
		// Get the Desktop's shell folder interface
			LPSHELLFOLDER psfDesktop = 0;
			HRESULT hr = SHGetDesktopFolder(&psfDesktop);
			if (SUCCEEDED(hr))
			{
				// Request an ID list (relative to the desktop) for the short pathname
				ULONG chEaten = 0;
				LPITEMIDLIST pidlShellItem = 0;
				hr = psfDesktop->ParseDisplayName(0, 0, pszShortPathNameW, &chEaten, &pidlShellItem, 0);
				if (SUCCEEDED(hr))
				{
					// We did get an ID list, convert it to a long pathname
					SHGetPathFromIDListA(pidlShellItem, lpszLongPath);
					// Free the ID list allocated by ParseDisplayName
					LPMALLOC pMalloc = NULL;
					SHGetMalloc(&pMalloc);
					pMalloc->Free(pidlShellItem);
					pMalloc->Release();
				}
				psfDesktop->Release(); // Release the desktop's IShellFolder
			}
			CoTaskMemFree(pszShortPathNameW);
		}
	}
	else
	{
		strcpy(lpszLongPath, lpszShortPath);
	}
}
