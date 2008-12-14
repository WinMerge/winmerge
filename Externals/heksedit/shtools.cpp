#include "precomp.h"
#include "shtools.h"
#include "AnsiConvert.h"
/*Pabs inserted func
Create link files for registry menu
Copied from compiler documentation. - had to fix up some stuff - totally wrong parameters used - all freaky compile time errors
CreateLink - uses the shell's IShellLink and IPersistFile interfaces to create and store a shortcut to the specified object.
Returns the result of calling the member functions of the interfaces.
lpszPathObj - address of a buffer containing the path of the object.
lpszPathLink - address of a buffer containing the path where the shell link is to be stored.*/
HRESULT NTAPI CreateLink(LPCTSTR lpszPathObj, LPCTSTR lpszPathLink)
{
	HRESULT hres;
	IShellLink *psl;

	// Get a pointer to the IShellLink interface.
	hres = CoCreateInstance(CLSID_ShellLink, NULL,
		CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&psl);
	if (SUCCEEDED(hres))
	{
		IPersistFile *ppf;
		// Set the path to the shortcut target
		psl->SetPath(lpszPathObj);
		// Query IShellLink for the IPersistFile interface for saving the
		// shortcut in persistent storage.
		hres = psl->QueryInterface(IID_IPersistFile, (void **)&ppf);
		if (SUCCEEDED(hres))
		{
			// Create the dir before saving the file because IPersistFile::Save won't
			if (LPTSTR tmp = StrRChr(lpszPathLink, 0, _T('\\')))
			{
				*tmp = _T('\0'); //Remove filename
				CreateDirectory(lpszPathLink, 0);
				*tmp = _T('\\');
			}
			// Save the link by calling IPersistFile::Save.
			hres = ppf->Save(static_cast<T2W>(lpszPathLink), TRUE);
			ppf->Release();
		}
		psl->Release();
	}
	return hres;
}

HRESULT NTAPI CreateLink(LPCTSTR lpszPathLink)
{
	TCHAR exepath[MAX_PATH];
	GetModuleFileName(0, exepath, MAX_PATH);
	return CreateLink(exepath, lpszPathLink);
}

HRESULT NTAPI ResolveIt(HWND hwnd, LPCTSTR lpszLinkFile, LPTSTR lpszPath)
{
	HRESULT hres;
	IShellLink *psl;
	WIN32_FIND_DATA fd;
	*lpszPath = _T('\0'); // assume failure

	// Get a pointer to the IShellLink interface.
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&psl);
	if (SUCCEEDED(hres))
	{
		IPersistFile* ppf;
		// Get a pointer to the IPersistFile interface.
		hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
		if (SUCCEEDED(hres))
		{
			// Load the shortcut.
			hres = ppf->Load(static_cast<T2W>(lpszLinkFile), STGM_READ);
			if (SUCCEEDED(hres))
			{
				// Resolve the link.
				hres = psl->Resolve(hwnd, SLR_ANY_MATCH);
				if (SUCCEEDED(hres))
				{
					// Get the path to the link target.
					hres = psl->GetPath(lpszPath, MAX_PATH, &fd, SLGP_SHORTPATH);
				}
			}
			// Release the pointer to the IPersistFile interface.
			ppf->Release();
		}
		// Release the pointer to the IShellLink interface.
		psl->Release();
	}
	return hres;
}

//Pabs inserted
//Parts copied from compiler docs - search for ITEMIDLIST in title in msdn
//Adapted from Q132750:"HOWTO: Convert a File Path to an ITEMIDLIST" in the Knowledge Base
HRESULT NTAPI PathsEqual(LPCTSTR p0, LPCTSTR p1)
{
	LPSHELLFOLDER pFolder;
	HRESULT hr;
	if (SUCCEEDED(hr = SHGetDesktopFolder(&pFolder)))
	{
		LPITEMIDLIST pidl[2];
		ULONG chEaten;//only needed by parse dispname
		// Convert the paths to ITEMIDLISTs.
		if (SUCCEEDED(hr = pFolder->ParseDisplayName(NULL, NULL,
				const_cast<LPOLESTR>(&*static_cast<T2W>(p0)), &chEaten, &pidl[0], NULL)) &&
			SUCCEEDED(hr = pFolder->ParseDisplayName(NULL, NULL,
				const_cast<LPOLESTR>(&*static_cast<T2W>(p1)), &chEaten, &pidl[1], NULL)))
		{
			hr = pFolder->CompareIDs(0, pidl[0], pidl[1]);
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

HRESULT NTAPI PathsEqual(LPCTSTR path)
{
	TCHAR exepath[MAX_PATH];
	GetModuleFileName(0, exepath, MAX_PATH);
	return PathsEqual(exepath, path);
}

//This gets a fully qualified long absolute filename from any other type of file name ON ANY Win32 PLATFORM damn stupid Micro$uck$ & bloody GetLongPathName
//It was copied and enhanced from an article by Jeffrey Richter available in the MSDN under "Periodicals\Periodicals 1997\Microsoft Systems Journal\May\Win32 Q & A" (search for GetLongPathName & choose the last entry)
void NTAPI GetLongPathNameWin32(LPCTSTR lpszShortPath, LPTSTR lpszLongPath)
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
	LPTSTR lpszFilePart = 0;
	if (GetFullPathName(lpszShortPath, MAX_PATH, lpszLongPath, &lpszFilePart) <= MAX_PATH)
	{
		// Get the Desktop's shell folder interface
		LPSHELLFOLDER psfDesktop = 0;
		HRESULT hr = SHGetDesktopFolder(&psfDesktop);
		if (SUCCEEDED(hr))
		{
			// Request an ID list (relative to the desktop) for the short pathname
			ULONG chEaten = 0;
			LPITEMIDLIST pidlShellItem = 0;
			hr = psfDesktop->ParseDisplayName(0, 0, const_cast<LPWSTR>(&*static_cast<T2W>(lpszLongPath)), &chEaten, &pidlShellItem, 0);
			if (SUCCEEDED(hr))
			{
				// We did get an ID list, convert it to a long pathname
				SHGetPathFromIDList(pidlShellItem, lpszLongPath);
				// Free the ID list allocated by ParseDisplayName
				LPMALLOC pMalloc = NULL;
				SHGetMalloc(&pMalloc);
				pMalloc->Free(pidlShellItem);
				pMalloc->Release();
			}
			psfDesktop->Release(); // Release the desktop's IShellFolder
		}
	}
	else
	{
		lstrcpy(lpszLongPath, lpszShortPath);
	}
}
