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
 * @file  regtools.h
 *
 * @brief Registry helper functions implementation.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: regtools.cpp 19 2008-08-18 15:03:12Z kimmov $

#include "precomp.h"
#include "regtools.h"
/*Recursively delete key for WinNT
Don't use this under Win9x
Don't use this to delete keys you know will/should have no subkeys
This recursively deletes subkeys of the key and then
returns the return value of RegDeleteKey(basekey,keynam)*/
LONG RegDeleteWinNTKey(HKEY basekey, LPCTSTR keynam)
{
	HKEY tmp;
	LONG res;
	res = RegOpenKeyEx(basekey, keynam, 0, KEY_READ, &tmp);
	if (res == ERROR_SUCCESS)
	{
		TCHAR subkeynam[MAX_PATH];
		for (DWORD i = 0;; i++ )//Delete subkeys for WinNT
		{
			subkeynam[0] = 0;
			res = RegEnumKey(tmp, i, subkeynam, MAX_PATH);
			if (res == ERROR_NO_MORE_ITEMS)
				break;
			DWORD numsub;
			res = RegQueryInfoKey(tmp,NULL,NULL,NULL,&numsub,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
			if (res == ERROR_SUCCESS && numsub > 0)
				RegDeleteWinNTKey(tmp, subkeynam);//Recursively delete
			RegDeleteKey(tmp, subkeynam);
		}
		RegCloseKey(tmp);
	}
	return RegDeleteKey(basekey,keynam);
}

LONG RegCopyValues(HKEY src, LPCTSTR skey, HKEY dst, LPCTSTR dkey)
{
	HKEY sk,dk;
	LONG res;

	res = RegCreateKey(dst,dkey,&dk);
	if(res==ERROR_SUCCESS)res = RegOpenKeyEx(src,skey,0,KEY_READ,&sk);
	else RegCloseKey(dk);

	if (res == ERROR_SUCCESS)
	{
		TCHAR valnam[MAX_PATH];
		DWORD valnamsize,typ;
		TCHAR valbuf[MAX_PATH];
		DWORD valbufsize;
		for(DWORD i = 0;;i++)
		{
			valnamsize = sizeof valnam;
            valbufsize = sizeof valbuf;
			valbuf[0]=valnam[0]=0;
			res = RegEnumValue(sk,i,valnam,&valnamsize,0,&typ,(BYTE*) valbuf,&valbufsize);
			if(ERROR_NO_MORE_ITEMS==res)break;
			if(res==ERROR_SUCCESS)
				RegSetValueEx(dk,valnam,0,typ,(BYTE*)valbuf,valbufsize);
		}
		RegCloseKey(dk);
		RegCloseKey(sk);
	}
	return res;
}

