/////////////////////////////////////////////////////////////////////////////
//
//    expat_maps.cpp:  a plugin to the expat parser, to handle character encodings
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


#include "StdAfx.h"
#include <stdio.h>
#include "expat.h"
#include "expat_maps.h"
#include "map_data_8859_1.h"
#include "map_data_8859_2.h"
#include "map_data_8859_3.h"
#include "map_data_8859_4.h"
#include "map_data_8859_5.h"
#include "map_data_8859_6.h"
#include "map_data_8859_7.h"
#include "map_data_8859_8.h"
#include "map_data_8859_9.h"
#include "map_data_8859_10.h"
#include "map_data_8859_11.h"
#include "map_data_8859_13.h"
#include "map_data_8859_14.h"
#include "map_data_8859_15.h"
#include "map_data_8859_16.h"

struct map_info { const char * name; int * mapdata; };

static map_info known_maps[] = {
	{ "ISO-8859-1", &map_data_8859_1[0] }
	,{ "ISO-8859-2", &map_data_8859_2[0] }
	,{ "ISO-8859-3", &map_data_8859_3[0] }
	,{ "ISO-8859-4", &map_data_8859_4[0] }
	,{ "ISO-8859-5", &map_data_8859_5[0] }
	,{ "ISO-8859-6", &map_data_8859_6[0] }
	,{ "ISO-8859-7", &map_data_8859_7[0] }
	,{ "ISO-8859-8", &map_data_8859_8[0] }
	,{ "ISO-8859-9", &map_data_8859_9[0] }
	,{ "ISO-8859-10", &map_data_8859_10[0] }
	,{ "ISO-8859-11", &map_data_8859_11[0] }
	,{ "ISO-8859-13", &map_data_8859_13[0] }
	,{ "ISO-8859-14", &map_data_8859_14[0] }
	,{ "ISO-8859-15", &map_data_8859_15[0] }
	,{ "ISO-8859-16", &map_data_8859_16[0] }
};

static void populate_encoding_info(const map_info * mapinfo, XML_Encoding * info);

int
expat_maps_getMap(const XML_Char *name, XML_Encoding *info)
{
	for (int i=0; i<sizeof(known_maps)/sizeof(known_maps[0]); ++i)
	{
		const map_info * mapinfo = &known_maps[i];
		if (!stricmp(name, mapinfo->name))
		{
			populate_encoding_info(mapinfo, info);
			return 1;
		}
	}
	// Lets allow any encoding at all, and provide ISO-8859-1 map
	for (i=0; i<256; ++i)
	{
		// i->i gives us identity for ASCII, and ISO-8859-1 for remainder
		// because Unicode character set is numbered exactly like ISO-8859-1
		info->map[i] = i;
	}
	return 0;
}

static void
populate_encoding_info(const map_info * mapinfo, XML_Encoding * info)
{
	info->data = 0;
	info->convert = 0;
	info->release = 0;
	// Populate identity mapping for ASCII bytes
	for (int i=0; i<256; ++i)
	{
		info->map[i] = (i<127 ? i : 0);
	}
	// Populate code bytes as given in our map table
	for (i=0; mapinfo->mapdata[i] != -1; i += 2)
	{
		int src = mapinfo->mapdata[i];
		int dest = mapinfo->mapdata[i+1];
		info->map[src] = dest;
	}
}

