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
 * @file  PDriveNT.h
 *
 * @brief Declaration of PNtPhysicalDrive class.
 *
 */
#ifndef PDriveNT_h
#define PDriveNT_h

#include "physicaldrive.h"

class PNtPhysicalDrive : public IPhysicalDrive
{
public:
	PNtPhysicalDrive();
	virtual ~PNtPhysicalDrive();

	// path must look like this: "\\.\PhysicalDrive0" (of course, \ maps to \\, and \\ to \\\\)
	BOOL Open(int iDrive);
	void Close();
	BOOL GetDriveGeometry(DISK_GEOMETRY* lpDG);
	BOOL GetDriveGeometryEx(DISK_GEOMETRY_EX* lpDG, DWORD dwSize);
	BOOL GetDriveLayout(LPBYTE lpbMemory, DWORD dwSize);
	BOOL GetDriveLayoutEx(LPBYTE lpbMemory, DWORD dwSize);
	BOOL ReadAbsolute(LPBYTE lpbMemory, DWORD dwSize, INT64 Sector);
	BOOL IsOpen();

	HANDLE m_hDevice;
	INT64 m_BytesPerSector;
};

#endif // PDriveNT_h


