/*
Frhed - Free hex editor
Copyright (C) 2000 Raihan Kibria

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

Last change: 2013-02-24 by Jochen Neubeck
*/
/** 
 * @file  ShortcutsDlg.cpp
 *
 * @brief Implementation of the Shortcuts dialog.
 *
 */
#include "precomp.h"
#include "Constants.h"
#include "shtools.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "StringTable.h"

static const TCHAR FrhedLink[] = _T("Frhed.lnk");

class TraverseFolders
{
public:
	HListView *const list;
	TCHAR rn[MAX_PATH];
	TCHAR vn[50];
	LVITEM li;
	LVFINDINFO fi;
	BOOL cr;

	TraverseFolders(HListView *);
	void Recurse();
};

TraverseFolders::TraverseFolders(HListView *list) 
	: cr(FALSE), list(list)
{
	ZeroMemory(rn, sizeof(rn));
	ZeroMemory(vn, sizeof(vn));
	ZeroMemory(&li, sizeof(li));
	ZeroMemory(&fi, sizeof(fi));

	li.mask = LVIF_TEXT;//Using the text only
	li.iItem = list->GetItemCount();//Initial insert pos
	fi.flags = LVFI_STRING;//will need to check for duplicates
	fi.psz = li.pszText = rn;//Positions don't change beween files (Absolute path is entered into rn)
}

//Thanks to Raihan for the code this was based on - see his web page
void TraverseFolders::Recurse()
{
	_tfinddata_t F;
	intptr_t S;
	//First find all the links
	if ((S = _tfindfirst(_T("*.lnk"), &F)) != -1)
	{
		do if ( !( F.attrib & _A_SUBDIR ) && !ResolveIt(NULL, F.name, rn) )
		{
			int si;
			if (cr)
			{//findnfix
				PathStripPath(rn);//strip to file name
				si = _tcsicmp(rn, _T("frhed.exe"));
			}
			else
			{
				si = PathPointsToMe(rn);//update
			}
			if (si == 0)
			{
				_tfullpath(rn, F.name, MAX_PATH);
				_tremove(rn);//get rid of the file if we are fixing (in case of 2 links to frhed in same dir & links with the wrong name)
				TCHAR *fnam = PathFindFileName(rn);
				fnam[-1] = 0; //strip the file name (& '\\') off
				if (-1 == list->FindItem(&fi)) //Not present yet
				{
					//Insert the item
					list->InsertItem(&li);
					//Add to the Registry
					_stprintf(vn, _T("%d"), li.iItem);
					TCHAR keyname[64];
					_sntprintf(keyname, RTL_NUMBER_OF(keyname), _T("%s\\links"),
						OptionsRegistrySettingsPath);
					SHSetValue(HKEY_CURRENT_USER, keyname, vn, REG_SZ, rn,
						static_cast<DWORD>(fnam - rn));
					li.iItem++;
				}
				PathAppend(rn, FrhedLink);//put the name backon
				CreateLinkToMe(rn);//create the new link
			}
		} while (_tfindnext(S, &F) == 0);
		_findclose(S);
	}
	//Then find all the subdirs
	if ((S = _tfindfirst(_T("*"), &F)) != -1)
	{
		//except "." && ".."
		do if (F.attrib & _A_SUBDIR && _tcsstr(_T(".."), F.name) == 0)
		{
			_tchdir(F.name);
			Recurse();
			_tchdir(_T(".."));
		} while (_tfindnext(S, &F) == 0);
		_findclose(S);
	}
}

BOOL ShortcutsDlg::OnInitDialog(HWindow *pDlg)
{
	//Add a column
	LVCOLUMN col;
	ZeroMemory(&col, sizeof col);
	col.mask = LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.pszText = GetLangString(IDS_SCUT_LINKNAMES);
	col.cx = 153;
	static_cast<HListView *>(pDlg->GetDlgItem(IDC_SHORTCUT_LINKS))->InsertColumn(0, &col);
	//Load links from the registry
	//Tricky-tricky
	pDlg->SendMessage(WM_COMMAND,
		MAKEWPARAM(IDC_RELOAD, BN_CLICKED),
		(LPARAM)pDlg->GetDlgItem(IDC_RELOAD));
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
			PathAddBackslash(szDir);
			_tcsncat(szDir, FrhedLink, RTL_NUMBER_OF(szDir) - _tcslen(szDir));
			if (PathFileExists(szDir))
			{
				SendMessage(hw, BFFM_ENABLEOK, 0, 0);//Disable
				SendMessage(hw, BFFM_SETSTATUSTEXT, 0, (LPARAM)GetLangString(IDS_SCUT_ALREADY_CONTAINS));
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
					SendMessage(hw, BFFM_SETSTATUSTEXT, 0, (LPARAM)GetLangString(IDS_SCUT_CAN_ADD));
				}
				else
				{
					SendMessage(hw, BFFM_ENABLEOK, 0, 0);//Disable
					SendMessage(hw, BFFM_SETSTATUSTEXT, 0, (LPARAM)GetLangString(IDS_SCUT_CANNOT_ADD));
				}
			}
		}
		else
		{
			SendMessage(hw, BFFM_ENABLEOK, 0, 0);//Disable
			SendMessage(hw, BFFM_SETSTATUSTEXT, 0, (LPARAM)GetLangString(IDS_SCUT_CANNOT_ADD));
		}
	}
	return 0;
}

int CALLBACK ShortcutsDlg::SearchCallbackProc(HWND hw, UINT m, LPARAM l, LPARAM)
{
	if (m == BFFM_SELCHANGED)
	{
		TCHAR szDir[MAX_PATH];
		BOOL ret = SHGetPathFromIDList((LPITEMIDLIST) l ,szDir) && PathIsDirectory(szDir);
		SendMessage(hw, BFFM_ENABLEOK, 0, ret);//Enable/Disable
		SendMessage(hw, BFFM_SETSTATUSTEXT, 0,
			(LPARAM)(ret ? GetLangString(IDS_SCUT_CAN_SEARCH) : GetLangString(IDS_SCUT_CANNOT_SEARCH)));
	}
	return 0;
}

BOOL ShortcutsDlg::OnCommand(HWindow *pDlg, WPARAM w, LPARAM l)
{
	switch (LOWORD(w))
	{
	case IDCANCEL:
	case IDOK:
		{
			TCHAR keyname[64];
			_sntprintf(keyname, RTL_NUMBER_OF(keyname), _T("%s\\links"), OptionsRegistrySettingsPath);
			//Delete all values
			RegDeleteKey(HKEY_CURRENT_USER, keyname);
			//Save links (from the list box) to the registry & file system
			HKEY hk;
			if (ERROR_SUCCESS == RegCreateKey(HKEY_CURRENT_USER, keyname, &hk))
			{
				HListView *list = static_cast<HListView *>(pDlg->GetDlgItem(IDC_SHORTCUT_LINKS));//get the list
				int num = list->GetItemCount(); //get the # items in the list
				for (int i = 0; i < num; i++)
				{
					TCHAR valnam[50]; //value name
					_sntprintf(valnam, RTL_NUMBER_OF(valnam), _T("%d"), i);
					TCHAR buf[MAX_PATH]; //location of the link (all links named frhed.lnk)
					list->GetItemText(i, 0, buf, MAX_PATH);
					RegSetValueEx(hk, valnam, 0, REG_SZ, (BYTE*)buf,
						static_cast<DWORD>(_tcslen(buf) + 1) * sizeof(TCHAR));
					//Just in case
					PathAppend(buf, FrhedLink);
					CreateLinkToMe(buf);
				}
				RegCloseKey(hk);
			}
			else
			{
				MessageBox(pDlg, GetLangString(IDS_SCUT_CANNOT_SAVE), MB_OK);
			}

			//If the key is empty after this kill it (to prevent re-enabling of "Remove frhed")
			SHDeleteEmptyKey(HKEY_CURRENT_USER, keyname);
			SHDeleteEmptyKey(HKEY_CURRENT_USER, OptionsRegistrySettingsPath);

			pDlg->EndDialog(0);
			return TRUE;
		}

	case IDC_MOVE:
	case IDC_ADD:
		{
			//Call the filesystem dialog box to browse to a folder
			//& add it

			BROWSEINFO bi;
			TCHAR szDir[MAX_PATH] = {0};
			LPITEMIDLIST pidl;
			LPMALLOC pMalloc;
			HListView *list = static_cast<HListView *>(pDlg->GetDlgItem(IDC_SHORTCUT_LINKS));
			int di = -1;
			HKEY hk;

			if (LOWORD(w) == IDC_MOVE)
			{
				di = list->GetSelectedCount();
				if (di > 1)
				{
					MessageBox(pDlg, GetLangString(IDS_SCUT_CANNOT_MOVE), MB_OK);
					return TRUE;
				}
				if (di != 1)
				{
					MessageBox(pDlg, GetLangString(IDS_SCUT_NO_LINK_MOVE), MB_OK);
					return TRUE;
				}
				di = list->GetNextItem(-1, LVNI_SELECTED);
				if (di == -1)
				{
					MessageBox(pDlg, GetLangString(IDS_SCUT_CANNOT_FIND), MB_OK);
					return TRUE;
				}
			}
			if (SUCCEEDED(SHGetMalloc(&pMalloc)))
			{
				ZeroMemory(&bi, sizeof(bi));
				bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT ;
				bi.hwndOwner = pDlg->m_hWnd;
				bi.lpfn = BrowseCallbackProc;
				if (LOWORD(w) == IDC_ADD)
					bi.lpszTitle = GetLangString(IDS_SCUT_PLACE_LINK);
				else if (LOWORD(w) == IDC_MOVE)
					bi.lpszTitle = GetLangString(IDS_SCUT_MOVE_LINK);

				pidl = SHBrowseForFolder(&bi);
				if (pidl)
				{
					if (SHGetPathFromIDList(pidl, szDir))
					{
						//Check if the item is already in the list
						int num = list->GetItemCount();//get the # items in the list
						int done = 0;
						TCHAR path[MAX_PATH];
						TCHAR buf[MAX_PATH];

						_tcsncpy(path, szDir, RTL_NUMBER_OF(path));
						_tcsupr(path);
						for (int i = 0; i < num; i++)
						{
							list->GetItemText(i, 0, buf, MAX_PATH);//get the string
							_tcsupr(buf);
							if (_tcscmp(buf, path) == 0)
							{
								done = 1;
								break;
							}
						}//end of the loop
						TCHAR valnam[MAX_PATH];
						if (done)
						{
							MessageBox(pDlg, GetLangString(IDS_SCUT_ALREADY_LINK), MB_OK);
							//Just in case
							PathAppend(szDir, FrhedLink);
							CreateLinkToMe(szDir);
						}
						else
						{
							TCHAR keyname[64];
							_sntprintf(keyname, RTL_NUMBER_OF(keyname), _T("%s\\links"),
									OptionsRegistrySettingsPath);

							if (LOWORD(w) == IDC_ADD)
							{
								//Add to the list
								LVITEM item;
								ZeroMemory(&item, sizeof(item));
								item.mask = LVIF_TEXT;
								item.pszText = szDir;
								item.iItem = num;
								list->InsertItem(&item);
								//Add to the registry (find a string name that doesn't exist first)
								if (ERROR_SUCCESS == RegCreateKey(HKEY_CURRENT_USER, keyname, &hk))
								{
									for (DWORD i = 0 ; ; i++)
									{
										_sntprintf(valnam, RTL_NUMBER_OF(valnam) - 1, _T("%d"), i);
										if (ERROR_FILE_NOT_FOUND == RegQueryValueEx(hk, valnam, 0, NULL, NULL, NULL))
										{
											RegSetValueEx(hk, valnam, 0, REG_SZ, (BYTE*)szDir,
												static_cast<DWORD>(_tcslen(szDir) + 1) * sizeof(TCHAR));
											break;
										}
									}
									RegCloseKey(hk);
									//Add to the filesystem
									PathAppend(szDir, FrhedLink);
									CreateLinkToMe(szDir);
								}
							}
							else if (LOWORD(w) == IDC_MOVE)
							{
								//Move the old one to the new loc
								DWORD valnamsize, typ;
								TCHAR valbuf[MAX_PATH];
								DWORD valbufsize, ret;
								TCHAR cursel[MAX_PATH];
								//Get the old path
								list->GetItemText(di, 0, cursel, MAX_PATH);
								_tcsupr(cursel);
								//Set the new path in the registry
								if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, keyname, 0, KEY_ALL_ACCESS, &hk))
								{
									for (DWORD i = 0; ; i++)
									{
										typ = 0;
										valnamsize = sizeof valnam;
										valbufsize = sizeof valbuf;
										valbuf[0] = valnam[0] = 0;
										ret = RegEnumValue(hk, i, valnam, &valnamsize, 0, &typ, (BYTE*) valbuf, &valbufsize);
										_tcsupr(valbuf);
										if (typ == REG_SZ && valbuf[0] != 0 && !_tcscmp(valbuf ,cursel))
										{
											RegSetValueEx(hk, valnam, 0, REG_SZ, (BYTE*)szDir,
												static_cast<DWORD>(_tcslen(szDir) + 1) * sizeof(TCHAR));
											break;
										}
										if (ERROR_NO_MORE_ITEMS == ret)
											break;
									}
									RegCloseKey(hk);
									//Set the new path
									list->SetItemText(di, 0, szDir);
									//Move the actual file
									PathAppend(szDir, FrhedLink);
									PathAppend(cursel, FrhedLink);
									CreateLinkToMe(cursel);//Just in case
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
			if (LOWORD(w) == IDC_FIND_AND_FIX &&
					IDNO == MessageBox(pDlg, GetLangString(IDS_SCUT_UPDATE_LINKS), MB_YESNO))
				return TRUE;
			//Find a spot to start from
			LPMALLOC pMalloc;

			if (SUCCEEDED(SHGetMalloc(&pMalloc)))
			{
				BROWSEINFO bi;
				LPITEMIDLIST pidl;
				ZeroMemory(&bi, sizeof(bi));
				bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT ;
				bi.hwndOwner = pDlg->m_hWnd;
				bi.lpfn = SearchCallbackProc;
				bi.lpszTitle = GetLangString(IDS_SCUT_SEARCH_FOLDER);

				pidl = SHBrowseForFolder(&bi);
				if (pidl)
				{
					TCHAR szDir[MAX_PATH] = {0};
					if (SHGetPathFromIDList(pidl, szDir))
					{
						WaitCursor wc;//Wait until finished
						_tchdir(szDir);//Set the dir to start searching in
						TraverseFolders tf = static_cast<HListView *>(
							pDlg->GetDlgItem(IDC_SHORTCUT_LINKS));//Set the list hwnd;
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
			TCHAR keyname[64];
			_sntprintf(keyname, RTL_NUMBER_OF(keyname), _T("%s\\links"),
					OptionsRegistrySettingsPath);
			HListView *list = static_cast<HListView *>(pDlg->GetDlgItem(IDC_SHORTCUT_LINKS));//get the list
			//Delete the selected links from the registry & the filesystem
			int di = list->GetSelectedCount();
			if (di == 0)
			{
				MessageBox(pDlg, GetLangString(IDS_SCUT_NO_SELECT_DEL), MB_OK);
				return TRUE;
			}
			for( ; ; )
			{
				di = list->GetNextItem(-1, LVNI_SELECTED);
				if (di == -1)
					break;
				TCHAR valnam[MAX_PATH];
				TCHAR valbuf[MAX_PATH];
				TCHAR delbuf[MAX_PATH];
				list->GetItemText(di, 0, delbuf, MAX_PATH);
				_tcsupr(delbuf);
				list->DeleteItem(di);
				HKEY hk;
				if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, keyname, 0, KEY_ALL_ACCESS, &hk))
				{
					for (DWORD i = 0; ; i++)
					{
						DWORD typ = 0;
						DWORD valnamsize = sizeof valnam;
						DWORD valbufsize = sizeof valbuf;
						valbuf[0] = valnam[0] = _T('\0');
						DWORD ret = RegEnumValue(hk, i, valnam, &valnamsize, 0, &typ, (BYTE*) valbuf, &valbufsize);
						_tcsupr(valbuf);
						if (typ == REG_SZ && valbuf[0] != 0 && !_tcscmp(valbuf, delbuf))
						{
							RegDeleteValue(hk, valnam);
							break;
						}
						if (ERROR_NO_MORE_ITEMS == ret)
							break;
					}
					RegCloseKey(hk);
					PathAddBackslash(delbuf);
					_tcsncat(delbuf, FrhedLink, RTL_NUMBER_OF(delbuf) - _tcslen(delbuf));
					_tremove(delbuf);
				}
				SHDeleteEmptyKey(HKEY_CURRENT_USER, keyname);
			}
			return TRUE;
		}
	case IDC_RELOAD:
		{
			//Reload links from the registry frhed\subreleaseno\links\ all values loaded & tested
			TCHAR keyname[64] = {0};
			_sntprintf(keyname, RTL_NUMBER_OF(keyname), _T("%s\\links"),
					OptionsRegistrySettingsPath);
			HListView *list = static_cast<HListView *>(pDlg->GetDlgItem(IDC_SHORTCUT_LINKS));//get the list
			//Delete list
			list->DeleteAllItems();
			HKEY hk;
			if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, keyname, 0, KEY_ALL_ACCESS, &hk))
			{
				//Load all the string values
				for (DWORD i = 0 ; ; i++)
				{
					TCHAR valnam[MAX_PATH];
					DWORD valnamsize = sizeof valnam;
					DWORD typ = 0;
					TCHAR valbuf[MAX_PATH];
					DWORD valbufsize = sizeof valbuf;
					DWORD ret = RegEnumValue(hk, i, valnam, &valnamsize, 0, &typ, (BYTE*) valbuf, &valbufsize);
					if (typ == REG_SZ && valbuf[0] != 0 && PathIsDirectory(valbuf))
					{//Valid dir
						//Add the string
						LVITEM item;
						item.iItem = i;
						item.iSubItem = 0;
						item.mask = LVIF_TEXT;
						item.pszText = valbuf;
						list->InsertItem(&item);
						PathAddBackslash(valbuf);
						_tcsncat(valbuf, FrhedLink, RTL_NUMBER_OF(valbuf) - _tcslen(valbuf));
						CreateLinkToMe(valbuf);
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
			TCHAR keyname[64];
			_sntprintf(keyname, RTL_NUMBER_OF(keyname), _T("%s\\links"),
					OptionsRegistrySettingsPath);
			HKEY hk;
			if (ERROR_SUCCESS == RegOpenKey(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), &hk))
			{
				TCHAR szDir[MAX_PATH];
				DWORD len = sizeof szDir;
				//Get the path from the registry
				switch (LOWORD(w))
				{
				case IDC_START:
					RegQueryValueEx(hk, _T("Start Menu"), 0, NULL, (BYTE*)szDir, &len);
					break;
				case IDC_PROGRAMS:
					RegQueryValueEx(hk, _T("Programs"), 0, NULL, (BYTE*)szDir, &len);
					break;
				case IDC_SENDTO:
					RegQueryValueEx(hk, _T("SendTo"), 0, NULL, (BYTE*)szDir, &len);
					break;
				case IDC_DESKTOP:
					RegQueryValueEx(hk, _T("Desktop"), 0, NULL, (BYTE*)szDir, &len);
					break;
				}
				RegCloseKey(hk);

				HListView *list = static_cast<HListView *>(pDlg->GetDlgItem(IDC_SHORTCUT_LINKS));//get the list
				int num = list->GetItemCount();//get the # items in the list
				int done = 0;
				TCHAR path[MAX_PATH];
				TCHAR buf[MAX_PATH];
				_tcsnccpy(path, szDir, RTL_NUMBER_OF(path));
				_tcsupr(path);
				for (int i = 0; i < num; i++)
				{//loop num times
					list->GetItemText(i, 0, buf, MAX_PATH);//get the string
					_tcsupr(buf);//convert to upper since strcmp is case sensitive & Win32 is not
					if (_tcscmp(buf, path) == 0)
					{
						done = 1;
						break;
					}
				}//end of the loop
				if (done)
				{
					MessageBox(pDlg, GetLangString(IDS_SCUT_ALREADY_LINK), MB_OK);
					//Just in case
					PathAppend(szDir, FrhedLink);
					CreateLinkToMe(szDir);
				}
				else
				{
					LVITEM item;
					item.mask = LVIF_TEXT;
					item.pszText = szDir;
					item.iItem = num;
					item.iSubItem = 0;
					list->InsertItem(&item);
					TCHAR valnam[MAX_PATH];
					if (ERROR_SUCCESS == RegCreateKey(HKEY_CURRENT_USER, keyname, &hk))
					{
						//Find a value name that does not exist
						for (DWORD i = 0; ; i++)
						{
							_sntprintf(valnam, RTL_NUMBER_OF(valnam), _T("%d"), i);
							if (ERROR_FILE_NOT_FOUND == RegQueryValueEx(hk, valnam, 0, NULL, NULL, NULL))
							{
								//Add the value to the registry
								RegSetValueEx(hk, valnam, 0, REG_SZ, (BYTE*)szDir,
									static_cast<DWORD>(_tcslen(szDir) + 1) * sizeof(TCHAR));
								break;
							}
						}
						RegCloseKey(hk);
						PathAppend(szDir, FrhedLink);
						CreateLinkToMe(szDir);
					}
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ShortcutsDlg::OnNotify(HWindow *pDlg, WPARAM w, LPARAM l)
{
	NMLVKEYDOWN *nmh = ((NMLVKEYDOWN*)l);
	if (nmh->hdr.idFrom == IDC_SHORTCUT_LINKS &&
		nmh->hdr.code == LVN_KEYDOWN &&
		nmh->wVKey == VK_DELETE)
	{
		OnCommand(pDlg, IDC_DELETE, 0);
	}
	return TRUE;
}

INT_PTR ShortcutsDlg::DlgProc(HWindow *pDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(pDlg);
	case WM_COMMAND:
		return OnCommand(pDlg, wParam, lParam);
	case WM_NOTIFY:
		return OnNotify(pDlg, wParam, lParam);

	case WM_HELP:
		OnHelp(pDlg);
		break;
	}
	return FALSE;
}
