/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm

/* GNU DIFF main routine.
   Copyright (C) 1988, 1989, 1992, 1993 Free Software Foundation, Inc.

This file is part of GNU DIFF.

GNU DIFF is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU DIFF is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU DIFF; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* GNU DIFF was written by Mike Haertel, David Hayes,
   Richard Stallman, Len Tower, and Paul Eggert.  */

#include "stdafx.h"
#define GDIFF_MAIN
#include "diff.h" 
#include "getopt.h"
#include "fnmatch.h"
#include "io.h"
#include "diffwrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// reduce some noise produced with the MSVC compiler
#if defined (_AFXDLL)
#pragma warning(disable : 4706)
#endif



#ifndef DEFAULT_WIDTH
#define DEFAULT_WIDTH 130
#endif

#ifndef GUTTER_WIDTH_MINIMUM
#define GUTTER_WIDTH_MINIMUM 3
#endif



/* Nonzero for -r: if comparing two directories,
   compare their common subdirectories recursively.  */

int recursive;

