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
 * @file  clipboard.h
 *
 * @brief Clipboard helper functions declaration.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: clipboard.h 18 2008-08-18 14:46:22Z kimmov $

#ifndef _CLIPBOARD_H_
#define _CLIPBOARD_H_

void TextToClipboard(HWND hwnd, char *text);
void TextToClipboard(HWND hwnd, char *text, int len);
void MessageCopyBox(HWND hwnd, LPTSTR text, LPCTSTR caption, UINT type);

#endif // _CLIPBOARD_H_
