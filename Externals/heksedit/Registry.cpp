/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  Registry.cpp
 *
 * @brief Implementation of Frhed registry functions.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: Registry.cpp 159 2008-11-14 18:16:58Z kimmov $

#include "precomp.h"
#include "version.h"
#include "Registry.h"
#include "shtools.h"
#include "regtools.h"

BOOL contextpresent()
{
	HKEY key1;
	LONG res = RegOpenKeyEx(HKEY_CLASSES_ROOT, _T("*\\shell\\Open in frhed\\command"), 0, KEY_ALL_ACCESS, &key1);
	if (res == ERROR_SUCCESS) //succeeded check if has the required keys & data
	{
		TCHAR stringval[MAX_PATH];
		long len = sizeof stringval;
		RegQueryValue(key1, NULL, stringval, &len);
		PathRemoveArgs(stringval);
		PathUnquoteSpaces(stringval);
		RegCloseKey(key1);
		if (PathsEqual(stringval) == 0)
			return 1;
	}
	return 0;
}

BOOL defaultpresent()
{
	TCHAR stringval[MAX_PATH];
	long len = sizeof stringval;
	LONG res = RegQueryValue(HKEY_CLASSES_ROOT, _T("Unknown\\shell"), stringval, &len);
	return res == ERROR_SUCCESS && _tcscmp(stringval, _T("Open in frhed")) == 0;
}

BOOL unknownpresent()
{
	HKEY key1;
	LONG res = RegOpenKeyEx(HKEY_CLASSES_ROOT, _T("Unknown\\shell\\Open in frhed\\command"), 0, KEY_ALL_ACCESS, &key1);
	if (res == ERROR_SUCCESS) //succeeded check if has the required keys & data
	{
		TCHAR stringval[MAX_PATH];
		long len = sizeof stringval;
		RegQueryValue(key1, NULL, stringval, &len);
		PathRemoveArgs(stringval);
		PathUnquoteSpaces(stringval);
		RegCloseKey(key1);
		if (PathsEqual(stringval) == 0)
			return 1;
	}
	return 0;
}

BOOL oldpresent()
{
	HKEY hk;
	TCHAR keyname[] = _T("Software\\frhed");
	LONG res = RegOpenKeyEx(HKEY_CURRENT_USER, keyname, 0, KEY_ALL_ACCESS, &hk);
	if (res == ERROR_SUCCESS)
	{
		TCHAR subkeynam[MAX_PATH];
		for (DWORD i = 0 ; (res = RegEnumKey(hk, i, subkeynam, sizeof subkeynam)) == ERROR_SUCCESS ; i++)
		{
			if (_tcscmp(subkeynam, FRHED_SETTINGS))
				break;
		}
		RegCloseKey(hk);
	}
	return res == ERROR_SUCCESS;
}

BOOL frhedpresent()
{
	//Check if frhed\subreleaseno exists
	HKEY hk;
	if (ERROR_SUCCESS != RegOpenKey(HKEY_CURRENT_USER, _T("Software\\frhed\\") FRHED_SETTINGS, &hk))
		return FALSE;
	RegCloseKey(hk);
	return TRUE;
}

BOOL linkspresent()
{
	//Check if frhed\subreleaseno\links exists
	HKEY hk;
	if (ERROR_SUCCESS != RegOpenKey(HKEY_CURRENT_USER, _T("Software\\frhed\\") FRHED_SETTINGS _T("\\links"), &hk))
		return FALSE;
	RegCloseKey(hk);
	return TRUE;
}

BOOL registry_RemoveFrhed(HWND hwnd)
{
	BOOL bRemoveRegistryEntries = FALSE;
	int res, r, r0;
	r = r0 = 0;//r&r0 used to determine if the user has removed all frhed data
	res = MessageBox(hwnd, _T("Are you sure you want to remove frhed?"), _T("Remove frhed"), MB_YESNO);
	if (res != IDYES)
		return FALSE;
	//Can assume registry data exists
	res = linkspresent();
	if (res)
	{
		r0++;
		res = MessageBox(hwnd, _T("Remove known links?"), _T("Remove frhed"), MB_YESNO);
		if (res == IDYES)
		{
			r++;
			//Remove known links & registry entries of those links
			HKEY hk;
			TCHAR valnam[MAX_PATH];
			DWORD valnamsize, typ;
			TCHAR valbuf[MAX_PATH];
			DWORD valbufsize, ret;
			if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\frhed\\") FRHED_SETTINGS _T("\\links"), 0, KEY_ALL_ACCESS, &hk))
			{
				for(DWORD i = 0;;i++)
				{
					typ=0;
					valnamsize = sizeof valnam;
					valbufsize = sizeof valbuf;
					valbuf[0] = valnam[0] = 0;
					ret = RegEnumValue(hk,i,valnam,&valnamsize,0,&typ,(BYTE*) valbuf,&valbufsize);
					if(typ==REG_SZ && valbuf[0]!=0 && PathIsDirectory(valbuf))
					{
						PathAppend(valbuf, _T("frhed.lnk"));
						_tremove(valbuf);
					}
					if (ERROR_NO_MORE_ITEMS == ret)
						break;
				}
				RegCloseKey(hk);
			}
			RegDeleteKey(HKEY_CURRENT_USER, _T("Software\\frhed\\") FRHED_SETTINGS _T("\\links"));
		}
	}
	res = contextpresent() || unknownpresent();
	if (res)
	{
		r0++;
		res = MessageBox(hwnd, _T("Remove 'Open in frhed' command(s) ?"), _T("Remove frhed"), MB_YESNO);
		if (res == IDYES)
		{
			r++;
			//Remove 'Open in frhed' command registry entries
			RegDeleteKey(HKEY_CLASSES_ROOT, _T("*\\shell\\Open in frhed\\command")); //WinNT requires the key to have no subkeys
			RegDeleteKey(HKEY_CLASSES_ROOT, _T("*\\shell\\Open in frhed"));
			RegDeleteKey(HKEY_CLASSES_ROOT, _T("Unknown\\shell\\Open in frhed\\command")); //WinNT requires the key to have no subkeys
			RegDeleteKey(HKEY_CLASSES_ROOT, _T("Unknown\\shell\\Open in frhed"));
			TCHAR stringval[MAX_PATH];
			long len = _MAX_PATH;
			RegQueryValue(HKEY_CLASSES_ROOT, _T("Unknown\\shell"), stringval, &len);
			if (!_tcscmp(stringval, _T("Open in frhed")))
			{
				HKEY hk;
				if (ERROR_SUCCESS == RegOpenKey(HKEY_CLASSES_ROOT, _T("Unknown\\shell"), &hk))
				{
					RegDeleteValue(hk, NULL);
					RegCloseKey(hk);
				}
			}
		}
	}
	HKEY tmp;
	res = RegOpenKey(HKEY_CURRENT_USER, _T("Software\\frhed\\") FRHED_SETTINGS, &tmp);
	if (res == ERROR_SUCCESS)
	{
		RegCloseKey(tmp);
		r0++;
		res = MessageBox(hwnd, _T("Remove registry entries?"), _T("Remove frhed"), MB_YESNO);
		if (res == IDYES)
		{
			r++;
			bRemoveRegistryEntries = TRUE;//Don't save ini data when the user quits (and hope other instances are not running now (they will write new data)
			OSVERSIONINFO ver;
			ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx(&ver);
			if (ver.dwPlatformId == VER_PLATFORM_WIN32_NT)
				RegDeleteWinNTKey(HKEY_CURRENT_USER, _T("Software\\frhed\\") FRHED_SETTINGS);
			else
				RegDeleteKey(HKEY_CURRENT_USER, _T("Software\\frhed\\") FRHED_SETTINGS);
			res = oldpresent();
			if (res)
			{
				res = MessageBox(hwnd, _T("Registry entries from previous versions of frhed were found\n")
					_T("Should they all be removed?"), _T("Remove frhed"), MB_YESNO);
				if (res == IDYES)
				{
					if (ver.dwPlatformId == VER_PLATFORM_WIN32_NT)
						RegDeleteWinNTKey(HKEY_CURRENT_USER, _T("Software\\frhed"));
					else
						RegDeleteKey(HKEY_CURRENT_USER, _T("Software\\frhed"));
				}
			}
		}
	}
#if 0
	for(;;i++){
		RegEnumKey (HKEY_USERS,i,buf,MAX_PATH+1);
		if(res==ERROR_NO_MORE_ITEMS)break;
		if(!stricmp(buf,".Default")){
			if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_USERS,buf,0,KEY_ALL_ACCESS,&hk)){
				removefrhedfromuser(hk);
				RegCloseKey(hk);
			}
		}
	}
#endif // 0
	if (r == r0)
	{
		MessageBox(hwnd,
			_T("Now all that remains to remove frhed from this computer is to:\n")
			_T("1. Quit all other instances of frhed(after turning off \"Save ini...\" in each one)\n")
			_T("2. Quit this instance of frhed\n")
			_T("3. Check that the registry data was removed (just in case)\n")
			_T("4. Delete the directory where frhed currently resides\n")
			_T("5. If you have an email account please take the time to\n")
			_T("    email the author/s and give the reason/s why you have\n")
			_T("    removed frhed from your computer"),
			_T("Remove frhed"), MB_OK);
	}
	return bRemoveRegistryEntries;
}
