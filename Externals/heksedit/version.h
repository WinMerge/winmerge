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
 * @file  version.h
 *
 * @brief Version numbers for Frhed.
 */
// ID line follows -- this is updated by SVN
// $Id: version.h 158 2008-11-14 18:15:22Z kimmov $

// These set Frhed release version number
#define FRHED_MAJOR_VERSION 1
#define FRHED_MINOR_VERSION 2
#define FRHED_SUB_RELEASE_NO 1
#define FRHED_BUILD_NO 3

// Utility macros for formatting version number to string.
#define SHARPEN_A(X) #X
#define SHARPEN_W(X) L#X

#ifdef UNICODE
#define SHARPEN(X) SHARPEN_W(X)
#define WC_HEKSEDIT _T("hekseditW_") SHARPEN(FRHED_VERSION_4)
#define PE_HEKSEDIT _T("hekseditU.dll")
#else
#define SHARPEN(X) SHARPEN_A(X)
#define WC_HEKSEDIT _T("hekseditA_") SHARPEN(FRHED_VERSION_4)
#define PE_HEKSEDIT _T("heksedit.dll")
#endif

#define FRHED_VERSION_1 FRHED_MAJOR_VERSION
#define FRHED_VERSION_2 FRHED_MAJOR_VERSION.FRHED_MINOR_VERSION
#define FRHED_VERSION_3 FRHED_MAJOR_VERSION.FRHED_MINOR_VERSION.FRHED_SUB_RELEASE_NO
#define FRHED_VERSION_4 FRHED_MAJOR_VERSION.FRHED_MINOR_VERSION.FRHED_SUB_RELEASE_NO.FRHED_BUILD_NO

// #define the registry node to persist Frhed settings.
// Note: _T("v") SHARPEN(FRHED_VERSION_3) would yield traditional per-version settings.
#define FRHED_SETTINGS _T("Settings")
