/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
// stdafx.cpp : source file that includes just the standard includes
//	Merge.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

int
xisspecial (int c)
{
  return (unsigned) c > (unsigned) _T ('\x7f') || c == _T ('_');
//  return _tcschr (_T ("ìšèøıáíéóúùïò¾àåœäëöüÌŠÈØİÁÍÉ´OÚÙÏÒ¼ÀÅŒÄËÖÜ§"), c) != NULL;
}

int
xisalpha (int c)
{
  return isalpha (c) || xisspecial (c);
}

int
xisalnum (int c)
{
  return isalnum (c) || xisspecial (c);
}
