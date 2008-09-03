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
 * @file  PDriveNT.h
 *
 * @brief Declaration of PNtPhysicalDrive class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: PDriveNT.h 27 2008-08-21 19:52:33Z kimmov $

#ifndef PDriveNT_h
#define PDriveNT_h

#include "physicaldrive.h"

class PNtPhysicalDrive : public IPhysicalDrive
{
public:
	PNtPhysicalDrive();
	virtual ~PNtPhysicalDrive();

	// path must look like this: "\\.\PhysicalDrive0" (of course, \ maps to \\, and \\ to \\\\)
	BOOL Open( int iDrive );
	void Close();
	BOOL GetDriveGeometry( DISK_GEOMETRY* lpDG );
	BOOL GetDriveGeometryEx( DISK_GEOMETRY_EX* lpDG, DWORD dwSize );
	BOOL GetDriveLayout( LPBYTE lpbMemory, DWORD dwSize );
	BOOL GetDriveLayoutEx( LPBYTE lpbMemory, DWORD dwSize );
	BOOL ReadAbsolute( LPBYTE lpbMemory, DWORD dwSize, INT64 Sector );
	BOOL IsOpen();

	HANDLE m_hDevice;
	INT64 m_BytesPerSector;
};

#endif // PDriveNT_h


