/////////////////////////////////////////////////////////////////////////////
//
//    expat_maps.h:  a plugin to the expat parser, to handle character encodings
//    Copyright (C) 2005  Perry Rapp
//    Author: Perry Rapp
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

// RCS ID line follows -- this is updated by CVS
// $Id$


#ifndef expat_maps_h_included
#define expat_maps_h_included

int expat_maps_getMap(const XML_Char *name, XML_Encoding *info);
const char * expat_maps_getVersion();

#endif // expat_maps_h_included
