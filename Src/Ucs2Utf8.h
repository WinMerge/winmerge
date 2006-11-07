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
 * @file  Ucs2Utf8.h
 *
 * @brief Declaration for UCS-2 <--> UTF-8 conversions functions.
 *
 */

#ifndef _UCS2UTF8_H_
#define _UCS2UTF8_H_

UINT TransformUcs2ToUtf8(LPCWSTR psUcs, UINT nUcs, LPSTR pcsUtf, UINT nUtf);
UINT TransformUtf8ToUcs2(LPCSTR pcsUtf, UINT nUtf, LPWSTR psUcs, UINT nUcs);

#endif // _UCS2UTF8_H_
