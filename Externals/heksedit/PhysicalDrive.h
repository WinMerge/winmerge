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
 * @file  PhysicalDrive.h
 *
 * @brief Drive/partition information class declarations.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: PhysicalDrive.h 45 2008-09-04 14:05:26Z jtuc $

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


