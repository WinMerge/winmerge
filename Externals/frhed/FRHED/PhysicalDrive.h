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
 * @file  PhysicalDrive.h
 *
 * @brief Drive/partition information class declarations.
 *
 */
#ifndef PNtPhysicalDrive_h
#define PNtPhysicalDrive_h

#include <WinIoCtl.h>

class PartitionInfo : public PNode
{
public:
	DWORD m_dwDrive;
	DWORD m_dwPartition;
	BOOL m_bIsPartition;
	DWORD m_dwBytesPerSector;
	INT64 m_NumberOfSectors;
	INT64 m_StartingOffset;
	INT64 m_StartingSector;
	INT64 m_PartitionLength;

	LPTSTR GetNameAsString(PFormat * = PString<80>());
	LPTSTR GetSizeAsString(PFormat * = PString<80>());
};


//This is an abstract class for a physical drive layout
class IPhysicalDrive
{
public:
	virtual BOOL Open( int iDrive ) = 0;
	virtual void Close() = 0;
	virtual BOOL GetDriveGeometry( DISK_GEOMETRY* lpDG ) = 0;
	virtual BOOL GetDriveGeometryEx( DISK_GEOMETRY_EX* lpDG, DWORD dwSize ) = 0;
	virtual BOOL GetDriveLayout( LPBYTE lpbMemory, DWORD dwSize ) = 0;
	virtual BOOL GetDriveLayoutEx( LPBYTE lpbMemory, DWORD dwSize ) = 0;
	virtual BOOL ReadAbsolute( LPBYTE lpbMemory, DWORD dwSize, INT64 Sector ) = 0;
	virtual BOOL IsOpen() = 0;

	//Creates a list of PartitionInfo elements
	virtual void GetPartitionInfo(PList* lpList);
};

IPhysicalDrive* CreatePhysicalDriveInstance();

#endif // PNtPhysicalDrive_h


