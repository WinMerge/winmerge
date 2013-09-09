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
 * @file  clipboard.h
 *
 * @brief Clipboard helper functions declaration.
 *
 */
#ifndef _CLIPBOARD_H_
#define _CLIPBOARD_H_

void TextToClipboard(HWindow *pwnd, TCHAR *text);
void TextToClipboard(HWindow *pwnd, TCHAR *text, SIZE_T len);
void MessageCopyBox(HWindow *pwnd, LPTSTR text, UINT type);

#endif // _CLIPBOARD_H_
