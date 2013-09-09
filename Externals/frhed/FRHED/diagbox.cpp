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
#include "precomp.h"

//============================================================================================

#define NumBx(var) NumBox( #var , var, "NumBox" )
#define NBx(capt, var) NumBox( #var , var, capt )
void NumBox (char* varname, int x, char* caption)
{
	char buf[100];
	sprintf (buf, "%s = %d = 0x%x", varname, x, x);
	MessageBox (NULL, buf, caption, MB_OK);
}

#define TxtBx(var) TxtBox( #var , var)
void TxtBox (char* varname, char* s)
{
	char buf[100];
	sprintf (buf, "%s = %s", varname, s);
	MessageBox (NULL, buf, "TxtBox", MB_OK);
}

#define ChrBx(var) CharBox( #var , var)
void CharBox (char* varname, char c)
{
	char buf[100];
	sprintf (buf, "%s = %c", varname, c);
	MessageBox (NULL, buf, "CharBox", MB_OK);
}
