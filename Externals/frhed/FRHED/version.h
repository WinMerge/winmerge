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

Last change: 2013-04-10 by Jochen Neubeck
*/
/** 
 * @file  version.h
 *
 * @brief Version number (macros) for Frhed.
 *
 */
// These set Frhed release version number
// Note that for releases, these values are read from
// versions.ini file and set here by the SetVersions.py script
#define FRHED_MAJOR_VERSION 0
#define FRHED_MINOR_VERSION 10902
#define FRHED_SUB_RELEASE_NO 2013
#define FRHED_BUILD_NO 0

// Utility macros for formatting version number to string.
#define SHARPEN_A(X) #X
#define SHARPEN_W(X) L#X

#ifdef UNICODE
#define SHARPEN(X) SHARPEN_W(X)
#else
#define SHARPEN(X) SHARPEN_A(X)
#endif

#define FRHED_VERSION_1 FRHED_MAJOR_VERSION
#define FRHED_VERSION_2 FRHED_MAJOR_VERSION.FRHED_MINOR_VERSION
#define FRHED_VERSION_3 FRHED_MAJOR_VERSION.FRHED_MINOR_VERSION.FRHED_SUB_RELEASE_NO
#define FRHED_VERSION_4 FRHED_MAJOR_VERSION.FRHED_MINOR_VERSION.FRHED_SUB_RELEASE_NO.FRHED_BUILD_NO

