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
 * @file  DragDropDlg.h
 *
 * @brief Drag and drop -dialog declaration.
 *
 */
#ifndef _DRAG_DROP_DLG_H_
#define _DRAG_DROP_DLG_H_

#include "resource.h"

/**
 * @brief A dialog for Drag & Drop.
 * This dialog is shown when user d&d data (not file) from another application
 * to Frhed view. User needs to select the format of the d&d'ed data.
 */
class DragDropDlg
{
public:
	enum { IDD = IDD_DRAG_DROP };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);

	DWORD allowable_effects; /**< Allowed operations bitmask. */
	bool effect; /**< Copy (true) or move (false) the data from other app? */
	UINT numformatetcs;
	FORMATETC *formatetcs;
	UINT numformats;
	UINT *formats;

protected:
	BOOL OnInitDialog(HWindow *);
	BOOL OnCommand(HWindow *, WPARAM, LPARAM);
};

#endif // _DRAG_DROP_DLG_H_
