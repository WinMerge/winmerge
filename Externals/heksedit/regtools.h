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
 * @brief Registry helper functions declaration.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: regtools.h 19 2008-08-18 15:03:12Z kimmov $

#ifndef _REGTOOLS_H_
#define _REGTOOLS_H_

/*Recursively delete key for WinNT
Don't use this under Win9x
Don't use this to delete keys you know will have no subkeys or should not have subkeys
This recursively deletes subkeys of the key and then
returns the return value of RegDeleteKey(basekey,keynam)*/
LONG RegDeleteWinNTKey(HKEY basekey, LPCTSTR keynam);
LONG RegCopyValues(HKEY src, LPCTSTR skey, HKEY dst, LPCTSTR dkey);

#endif // _REGTOOLS_H_
