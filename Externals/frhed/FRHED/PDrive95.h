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
#ifndef PDrive95_h
#define PDrive95_h

#include "RAWIO32.h"
#include "physicaldrive.h"

/*
For more info on partition types, please refer to
http://www.win.tue.nl/~aeb/partitions/partition_types-1.html
*/

class P9xPartitionInfo : public PNode
{
public:
	P9xPartitionInfo(PARTITION_INFORMATION* pi):m_pi(*pi){}

	PARTITION_INFORMATION m_pi;
};

class P9xPhysicalDrive : public IPhysicalDrive
{
public:
	P9xPhysicalDrive();
	virtual ~P9xPhysicalDrive();

	//Path must look like this: "\\.\PhysicalDrive0" (of course, \ maps to \\, and \\ to \\\\)
	BOOL Open( int iDrive );
	void Close();
	BOOL GetDriveGeometry( DISK_GEOMETRY* lpDG );
	BOOL GetDriveGeometryEx( DISK_GEOMETRY_EX* lpDG, DWORD dwSize );
	BOOL GetDriveLayout( LPBYTE lpbMemory, DWORD dwSize );
	BOOL GetDriveLayoutEx( LPBYTE lpbMemory, DWORD dwSize );
	BOOL ReadAbsolute( LPBYTE lpbMemory, DWORD dwSize, INT64 Sector );
	BOOL IsOpen();
	BOOL ReadPartitionInfoRecursive(DWORD dwSector,INT64 TotalOffset,int depth = 0);

	HANDLE m_hDevice;
	BYTE m_bDriveNumber;
	PList m_PartitionInfo;
private:
	static struct RAWIO32 *RAWIO32;
};

#endif // PDrive95_h
