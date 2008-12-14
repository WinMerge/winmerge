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
 * @file  precomp.h
 *
 * @brief Precompiled header files listing.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: precomp.h 45 2008-09-04 14:05:26Z jtuc $

// Needed to include
// DISK_GEOMETRY_EX, PARTITION_INFORMATION_EX
// and couple of others from WinIoCtl.h
#define _WIN32_WINNT 0x0501

#include <windows.h>
#include <windowsx.h>
#include <crtdbg.h>
#include <assert.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <direct.h>
#include <string.h>
#include <stdio.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <limits.h>
#include "gtools.h"
