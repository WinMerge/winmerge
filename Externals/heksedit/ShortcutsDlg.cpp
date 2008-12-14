#include "precomp.h"
#include "shtools.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

class TraverseFolders
{
public:
	HWND hwlist;
	TCHAR rn[MAX_PATH];
	TCHAR vn[50];
	LVITEM li;
	LVFINDINFO fi;
	BOOL cr;
	TraverseFolders(HWND);
	void Recurse();
};

TraverseFolders::TraverseFolders(HWND h)
{
	ZeroMemory(this, sizeof *this);
	hwlist = h;
	li.mask = LVIF_TEXT;//Using the text only
	li.iItem = ListView_GetItemCount(hwlist);//Initial insert pos
	fi.flags = LVFI_STRING;//will need to check for duplicates
	fi.psz = li.pszText = rn;//Positions don't change beween files (Absolute path is entered into rn)
}

static HANDLE NTAPI FindFile(HANDLE h, LPCTSTR path, WIN32_FIND_DATA *fd)
{
	if (h == INVALID_HANDLE_VALUE)
	{
		h = FindFirstFile(path, fd);
	}
	else if (fd->dwFileAttributes == INVALID_FILE_ATTRIBUTES || !FindNextFile(h, fd))
	{
		FindClose(h);
		h = INVALID_HANDLE_VALUE;
	}
	return h;
}

//Thanks to Raihan for the code this was based on - see his web page
void TraverseFolders::Recurse()
{
	WIN32_FIND_DATA F;
	//First find all the links
	HANDLE h = INVALID_HANDLE_VALUE;
	while ((h = FindFile(h, _T("*.lnk"), &F)) != INVALID_HANDLE_VALUE)
	{
		if (F.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;
		if (FAILED(ResolveIt(NULL, F.cFileName, rn)))
			continue;
		int si;
		if (cr)
		{//findnfix
			PathStripPath(rn);//strip to file name
			si = _tcsicmp(rn, _T("frhed.exe"));
		}
		else
		{
			si = PathsEqual(rn);//update
		}
		if (si == 0)
		{
			LPTSTR fp = 0;
			GetFullPathName(F.cFileName, MAX_PATH, rn, &fp);
			DeleteFile(rn);//get rid of the file if we are fixing (in case of 2 links to frhed in same dir & links with the wrong name)
			PathRemoveFileSpec(rn);//strip the file name (& '\\') off
			if (-1 == ListView_FindItem(hwlist, -1, &fi)) //Not present yet
			{
				//Insert the item
				ListView_InsertItem(hwlist, &li);
				//Add to the Registry
				_itot(li.iItem, vn, 10);
				SHSetValue(HKEY_CURRENT_USER,
					_T("Software\\frhed\\") FRHED_SETTINGS _T("\\links"),
					vn, REG_SZ, rn, _tcslen(rn) * sizeof(TCHAR));
				li.iItem++;
			}
			PathAppend(rn, _T("frhed.lnk"));//put the name backon
			CreateLink(rn);//create the new link
		}
	}
	//Then find all the subdirs
	while ((h = FindFile(h, _T("*.*"), &F)) != INVALID_HANDLE_VALUE)
	{
		if (!(F.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			continue;
		if (_tcsstr(_T(".."), F.cFileName))//except "." && ".."
			continue;
		SetCurrentDirectory(F.cFileName);
		Recurse();
		SetCurrentDirectory(_T(".."));
	}
}

BOOL ShortcutsDlg::OnInitDialog(HWND hw)
{
	//Add a column
	LVCOLUMN col;
	ZeroMemory(&col, sizeof col);
	col.mask = LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.pszText = _T("link names are \"frhed.lnk\"");
	col.cx = 153;
	HWND list = GetDlgItem(hw, IDC_LIST);
	ListView_InsertColumn(list, 0, &col);
	//Load links from the registry
	//Tricky-tricky
	SendMessage(hw, WM_COMMAND, MAKEWPARAM(IDC_RELOAD, BN_CLICKED), (LPARAM)GetDlgItem(hw,IDC_RELOAD));
	return TRUE;
}

int CALLBACK ShortcutsDlg::BrowseCallbackProc(HWND hw, UINT m, LPARAM l, LPARAM)
{
	//If the folder exists & doesn't contain a link then enabled
	// Set the status window to the currently selected path.
	if (m == BFFM_SELCHANGED)
	{
		TCHAR szDir[MAX_PATH];
		if (SHGetPathFromIDList((LPITEMIDLIST) l ,szDir) && PathIsDirectory(szDir))
		{
			PathAppend(szDir, _T("frhed.lnk"));
			if (PathFileExists(szDir))
			{
				SendMessage(hw, BFFM_ENABLEOK, 0, 0);//Disable
				SendMessage(hw, BFFM_SETSTATUSTEXT, 0, (LPARAM)_T("This folder already contains a file called \"frhed.lnk\""));
			}
			else
			{
				//If there is any other (faster/easier) way to test whether the file-system is writeable or not Please let me know - Pabs
				int fh = _tcreat(szDir, _S_IWRITE);
				if (fh != -1)
				{
					_close(fh);
					_tremove(szDir);
					SendMessage(hw, BFFM_ENABLEOK, 0, 1);//Enable
					SendMessage(hw, BFFM_SETSTATUSTEXT, 0, (LPARAM)_T("\"frhed.lnk\" can be added to this folder"));
				}
				else
				{
					SendMessage(hw,BFFM_ENABLEOK,0,0);//Disable
					SendMessage(hw,BFFM_SETSTATUSTEXT,0,(LPARAM)"\"frhed.lnk\" cannot be added to this folder");
				}
			}
		}
		else
		{
			SendMessage(hw,BFFM_ENABLEOK,0,0);//Disable
			SendMessage(hw,BFFM_SETSTATUSTEXT,0,(LPARAM)"\"frhed.lnk\" cannot be added to this folder");
		}
	}
	return 0;
}

int CALLBACK ShortcutsDlg::SearchCallbackProc(HWND hw, UINT m, LPARAM l, LPARAM)
{
	if (m == BFFM_SELCHANGED)
	{
		TCHAR szDir[MAX_PATH];
		BOOL ret = SHGetPathFromIDList((LPITEMIDLIST)l, szDir) && PathIsDirectory(szDir);
		SendMessage(hw, BFFM_ENABLEOK, 0, ret);//Enable/Disable
		SendMessage(hw, BFFM_SETSTATUSTEXT, 0, LPARAM(
			ret ? _T("frhed can start searching here") : _T("frhed cannot start the search from here")));
	}
	return 0;
}

BOOL ShortcutsDlg::OnCommand(HWND hw, WPARAM w, LPARAM l)
{
	switch (LOWORD(w))
	{
	case IDCANCEL:
	case IDOK:
		{
			//Delete all values
			RegDeleteKey(HKEY_CURRENT_USER, _T("Software\\frhed\\") FRHED_SETTINGS _T("\\links"));
			//Save links (from the list box) to the registry & file system
			HKEY hk;
			if (ERROR_SUCCESS == RegCreateKey(HKEY_CURRENT_USER, _T("Software\\frhed\\") FRHED_SETTINGS _T("\\links"), &hk))
			{
				TCHAR buf[MAX_PATH]; // location of the link (all links named frhed.lnk)
				HWND list = GetDlgItem(hw, IDC_LIST);//get the list
				int num = ListView_GetItemCount(list);//get the # items in the list
				for (int i = 0 ; i < num ; i++)
				{
					TCHAR valnam[50]; // value name
					_itot(i, valnam, 10);
					ListView_GetItemText(list, i, 0, buf, MAX_PATH);
					SHSetValue(hk, 0, valnam, REG_SZ, buf, _tcslen(buf) * sizeof(TCHAR));
					//Just in case
					PathAppend(buf, _T("frhed.lnk"));
					CreateLink(buf);
				}//end of the loop
				RegCloseKey(hk);
			}
			else MessageBox(hw, _T("Could not Save shortcut entries"), _T("Shortcuts"), MB_OK);

			//If the key is empty after this kill it (to prevent re-enabling of "Remove frhed")
			SHDeleteEmptyKey(HKEY_CURRENT_USER, _T("Software\\frhed\\") FRHED_SETTINGS _T("\\links"));
			SHDeleteEmptyKey(HKEY_CURRENT_USER, _T("Software\\frhed\\") FRHED_SETTINGS);

			EndDialog(hw, 0);
			return TRUE;
		}

	case IDC_MOVE:
	case IDC_ADD:
		{
			//Call the filesystem dialog box to browse to a folder
			//& add it
			TCHAR szDir[MAX_PATH];
			LPITEMIDLIST pidl;
			LPMALLOC pMalloc;
			HWND list = GetDlgItem(hw, IDC_LIST);
			int di = -1;
			HKEY hk;

			if (LOWORD(w)==IDC_MOVE)
			{
				di = ListView_GetSelectedCount(list);
				if (di > 1)
				{
					MessageBox(hw, _T("Can't move more than 1 link at a time"), _T("Move link"), MB_OK);
					return TRUE;
				}
				else if(di !=1 )
				{
					MessageBox(hw, _T("No link selected to move"), _T("Move link"), MB_OK);
					return TRUE;
				}
				di = ListView_GetNextItem(list, (UINT)-1, LVNI_SELECTED);
				if (di == -1)
				{
					MessageBox(hw, _T("Couldn't find the selected item"), _T("Move link"), MB_OK);
					return TRUE;
				}
			}
			if (SUCCEEDED(SHGetMalloc(&pMalloc)))
			{
				BROWSEINFO bi;
				ZeroMemory(&bi,sizeof(bi));
				bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT ;
				bi.hwndOwner = hw;
				bi.lpfn = BrowseCallbackProc;
				if (LOWORD(w) == IDC_ADD)
					bi.lpszTitle = _T("Place a link to frhed in...");
				else if (LOWORD(w) == IDC_MOVE)
					bi.lpszTitle = _T("Move the link to frhed to...");

				pidl = SHBrowseForFolder(&bi);
				if (pidl)
				{
					if (SHGetPathFromIDList(pidl, szDir))
					{
						//Check if the item is already in the list
						int num = ListView_GetItemCount(list);//get the # items in the list
						int done = 0;
						TCHAR path[MAX_PATH];
						TCHAR buf[MAX_PATH];
						_tcscpy(path, szDir);
						_tcsupr(path);
						for (int i = 0 ; i < num ; i++)
						{//loop num times
							ListView_GetItemText(list, i, 0, buf, MAX_PATH);//get the string
							_tcsupr(buf);
							if (_tcscmp(buf, path) == 0)
							{
								done = 1;
								break;
							}
						}//end of the loop
						if (done)
						{
							MessageBox(hw, _T("There is already a link in that folder"), _T("Add/Move"), MB_OK);
							//Just in case
							PathAppend(szDir, _T("frhed.lnk"));
							CreateLink(szDir);
						}
						else
						{
							if (LOWORD(w) == IDC_ADD)
							{
								//Add to the list
								LVITEM item;
								ZeroMemory(&item, sizeof item);
								item.mask = LVIF_TEXT;
								item.pszText = szDir;
								item.iItem = num;
								ListView_InsertItem(list, &item);
								if (ERROR_SUCCESS == RegCreateKey(HKEY_CURRENT_USER, _T("Software\\frhed\\") FRHED_SETTINGS _T("\\links"), &hk))
								{
									//Find a value name that does not exist
									TCHAR valnam[MAX_PATH];
									DWORD i = 0;
									do _itot(i++, valnam, 10);
										while (ERROR_FILE_NOT_FOUND != SHGetValue(hk, 0, valnam, 0, 0, 0));
									//Add the value to the registry
									SHSetValue(hk, 0, valnam, REG_SZ, szDir, _tcslen(szDir) * sizeof(TCHAR));
									RegCloseKey(hk);
									//Add to the filesystem
									PathAppend(szDir, _T("frhed.lnk"));
									CreateLink(szDir);
								}
							}
							else if (LOWORD(w) == IDC_MOVE)
							{
								//Move the old one to the new loc
								TCHAR valnam[MAX_PATH];
								DWORD valnamsize, typ;
								TCHAR valbuf[MAX_PATH];
								DWORD valbufsize, ret;
								TCHAR cursel[MAX_PATH];
								//Get the old path
								ListView_GetItemText(list, di, 0, cursel, MAX_PATH);//get the string
								_tcsupr(cursel);
								//Set the new path in the registry
								if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\frhed\\") FRHED_SETTINGS _T("\\links"), 0, KEY_ALL_ACCESS, &hk))
								{
									for (DWORD i = 0 ;; i++)
									{
										typ = 0;
										valbufsize = sizeof valbuf;
										valnamsize = sizeof valnam;
										valbuf[0] = valnam[0] = 0;
										ret = RegEnumValue(hk, i, valnam, &valnamsize, 0, &typ, (BYTE*) valbuf, &valbufsize);
										_tcsupr(valbuf);
										if (typ == REG_SZ && valbuf[0]!=0 && !_tcscmp(valbuf,cursel))
										{
											SHSetValue(hk, 0, valnam, REG_SZ, szDir, _tcslen(szDir) * sizeof(TCHAR));
											break;
										}
										if (ERROR_NO_MORE_ITEMS == ret)
											break;
									}
									RegCloseKey(hk);
									//Set the new path
									ListView_SetItemText(list, di, 0, szDir);
									//Move the actual file
									PathAppend(szDir, _T("frhed.lnk"));
									PathAppend(cursel, _T("frhed.lnk"));
									CreateLink(cursel);//Just in case
									_trename(cursel, szDir);
								}
							}
						}
					}
					pMalloc->Free(pidl);
				}
				pMalloc->Release();
			}
			return TRUE;
		}
	case IDC_FIND_AND_FIX:
		//Go through the file system searching for links to frhed.exe and fix them so they point to this exe
	case IDC_UPDATE:
		{
			//Go through the file system searching for links to this exe
			//-Thanks to Raihan for the traversing code this was based on.
			if (LOWORD(w) == IDC_FIND_AND_FIX && IDNO == MessageBox(hw, _T("Existing links to old versions of frhed will be updated to this version\nAre you sure you want to continue"), _T("Find & fix"), MB_YESNO))
				return TRUE;
			//Find a spot to start from
			LPMALLOC pMalloc;

			if (SUCCEEDED(SHGetMalloc(&pMalloc)))
			{
				BROWSEINFO bi;
				ZeroMemory(&bi, sizeof bi);
				bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
				bi.hwndOwner = hw;
				bi.lpfn = SearchCallbackProc;
				bi.lpszTitle = _T("Pick a folder to start searching in.");

				LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
				if (pidl)
				{
					TCHAR szDir[MAX_PATH];
					if (SHGetPathFromIDList(pidl, szDir))
					{
						WaitCursor wc;//Wait until finished
						SetCurrentDirectory(szDir);//Set the dir to start searching in
						TraverseFolders tf = GetDlgItem(hw, IDC_LIST);//Set the list hwnd;
						tf.cr = LOWORD(w) == IDC_FIND_AND_FIX;//any frhed.exe if 1 else _pgmptr
						tf.Recurse();//Search
					}
					pMalloc->Free(pidl);
				}
				pMalloc->Release();
			}
			return TRUE;
		}
	case IDC_DELETE:
		{
			HWND list = GetDlgItem (hw, IDC_LIST);//get the list
			//Delete the selected links from the registry & the filesystem
			int di = ListView_GetSelectedCount(list);
			if (di == 0)
			{
				MessageBox(hw, _T("No links selected to delete"), _T("Delete links"), MB_OK);
				return TRUE;
			}
			while ((di = ListView_GetNextItem(list, (UINT)-1, LVNI_SELECTED)) != -1)
			{
				TCHAR valnam[MAX_PATH];
				DWORD valnamsize, typ;
				TCHAR valbuf[MAX_PATH];
				DWORD valbufsize, ret;
				TCHAR delbuf[MAX_PATH];
				ListView_GetItemText(list, di, 0, delbuf, MAX_PATH);//get the string
				_tcsupr(delbuf);
				ListView_DeleteItem(list, di);
				HKEY hk;
				if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\frhed\\") FRHED_SETTINGS _T("\\links"), 0, KEY_ALL_ACCESS, &hk))
				{
					for (DWORD i = 0 ;; i++)
					{
						typ = 0;
						valnamsize = sizeof valnam;
						valbufsize = sizeof valbuf;
						valbuf[0] = valnam[0] = 0;
						ret = RegEnumValue(hk, i, valnam, &valnamsize, 0, &typ, (BYTE*) valbuf, &valbufsize);
						_tcsupr(valbuf);
						if (typ == REG_SZ && valbuf[0] != 0 && !_tcscmp(valbuf, delbuf))
						{
							SHDeleteValue(hk, 0, valnam);
							break;
						}
						if (ERROR_NO_MORE_ITEMS == ret)
							break;
					}
					RegCloseKey(hk);
					PathAppend(delbuf, _T("frhed.lnk"));
					_tremove(delbuf);
				}
				SHDeleteEmptyKey(HKEY_CURRENT_USER, _T("Software\\frhed\\") FRHED_SETTINGS _T("\\links"));
			}
			return TRUE;
		}
	case IDC_RELOAD:
		{
			//Reload links from the registry frhed\subreleaseno\links\ all values loaded & tested
			HKEY hk;
			TCHAR valnam[MAX_PATH];
			DWORD valnamsize, typ;
			TCHAR valbuf[MAX_PATH];
			DWORD valbufsize, ret;
			HWND list = GetDlgItem(hw, IDC_LIST);
			//Delete list
			ListView_DeleteAllItems(list);
			LVITEM item;
			ZeroMemory(&item, sizeof item);
			item.mask = LVIF_TEXT;
			item.pszText = valbuf;
			if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\frhed\\v") SHARPEN(FRHED_VERSION_3) _T("\\links"), 0, KEY_ALL_ACCESS, &hk))
			{
				//Load all the string values
				for (DWORD i = 0 ;; i++)
				{
					typ = 0;
					valnamsize = sizeof valnam;
					valbufsize = sizeof valbuf;
					valbuf[0] = valnam[0] = 0;
					ret = RegEnumValue(hk, i, valnam, &valnamsize, 0, &typ, (BYTE*) valbuf, &valbufsize);
					if (typ == REG_SZ && valbuf[0] != 0 && PathIsDirectory(valbuf))//Valid dir
					{
						//Add the string
						item.iItem = i;
						ListView_InsertItem(list, &item);
						PathAppend(valbuf, _T("frhed.lnk"));
						CreateLink(valbuf);
					}
					if (ERROR_NO_MORE_ITEMS == ret)
						break;
				}
				RegCloseKey(hk);
			}
			return TRUE;
		}
	case IDC_START:
	case IDC_PROGRAMS:
	case IDC_SENDTO:
	case IDC_DESKTOP:
		{
			//Create links in
			//HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders\Start Menu = C:\WINDOWS\Start Menu on my computer
			//HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders\Programs = C:\WINDOWS\Start Menu\Programs on my computer
			//HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders\SendTo = C:\WINDOWS\SendTo on my computer
			//HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders\Desktop = C:\WINDOWS\Desktop on my computer
			HKEY hk;
			if (ERROR_SUCCESS != RegOpenKey(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), &hk))
				break;
			TCHAR szDir[MAX_PATH];
			szDir[0] = L'\0';
			DWORD len = sizeof szDir;
			//Get the path from the registry
			LPCTSTR subkey = 0;
			switch (LOWORD(w))
			{
			case IDC_START:
				subkey = _T("Start Menu");
				break;
			case IDC_PROGRAMS:
				subkey = _T("Programs");
				break;
			case IDC_SENDTO:
				subkey = _T("SendTo");
				break;
			case IDC_DESKTOP:
				subkey = _T("Desktop");
				break;
			}
			DWORD ret = SHGetValue(hk, 0, subkey, 0, szDir, &len);
			RegCloseKey(hk);
			if (ERROR_SUCCESS != ret)
				break;
			HWND list = GetDlgItem(hw, IDC_LIST);
			int num = ListView_GetItemCount(list);//get the # items in the list
			int done = 0;
			TCHAR path[MAX_PATH];
			TCHAR buf[MAX_PATH];
			_tcscpy(path, szDir);
			_tcsupr(path);
			for(int i=0;i<num;i++)
			{//loop num times
				ListView_GetItemText(list, i, 0, buf, MAX_PATH);
				_tcsupr(buf);//convert to upper since strcmp is case sensitive & Win32 is not
				if (!_tcscmp(buf, path))
				{
					done = 1;
					break;
				}
			}//end of the loop
			if (done)
			{
				MessageBox(hw, _T("There is already a link in that folder"), _T("Add"), MB_OK);
				//Just in case
				PathAppend(szDir, _T("frhed.lnk"));
				CreateLink(szDir);
			}
			else
			{
				LVITEM item;
				ZeroMemory(&item, sizeof item);
				item.mask = LVIF_TEXT;
				item.pszText = szDir;
				item.iItem = num;
				ListView_InsertItem(list, &item);
				if (ERROR_SUCCESS == RegCreateKey(HKEY_CURRENT_USER, _T("Software\\frhed\\v") SHARPEN(FRHED_VERSION_3) _T("\\links"), &hk))
				{
					//Find a value name that does not exist
					TCHAR valnam[MAX_PATH];
					DWORD i = 0;
					do _itot(i++, valnam, 10);
						while (ERROR_FILE_NOT_FOUND != SHGetValue(hk, 0, valnam, 0, 0, 0));
					//Add the value to the registry
					SHSetValue(hk, 0, valnam, REG_SZ, szDir, _tcslen(szDir) * sizeof(TCHAR));
					RegCloseKey(hk);
					//Add to the filesystem
					PathAppend(szDir, _T("frhed.lnk"));
					CreateLink(szDir);
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ShortcutsDlg::OnNotify(HWND hw, WPARAM w, LPARAM l)
{
	NMLVKEYDOWN *nmh = ((NMLVKEYDOWN*)l);
	if (nmh->hdr.idFrom == IDC_LIST &&
		nmh->hdr.code == LVN_KEYDOWN &&
		nmh->wVKey == VK_DELETE)
	{
		OnCommand(hw, IDC_DELETE, 0);
	}
	return TRUE;
}

INT_PTR ShortcutsDlg::DlgProc(HWND hw, UINT m, WPARAM w, LPARAM l)
{
	switch (m)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hw);
	case WM_COMMAND:
		return OnCommand(hw, w, l);
	case WM_NOTIFY:
		return OnNotify(hw, w, l);
	}
	return FALSE;
}
