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
 * @file  Registry.h
 *
 * @brief Frhed registry functions.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: Registry.h 159 2008-11-14 18:16:58Z kimmov $

#ifndef _REGISTRY_H_
#define _REGISTRY_H_

BOOL contextpresent();
BOOL linkspresent();
BOOL defaultpresent();
BOOL unknownpresent();
BOOL oldpresent();
BOOL frhedpresent();

BOOL registry_RemoveFrhed(HWND);

#endif // _REGISTRY_H_
