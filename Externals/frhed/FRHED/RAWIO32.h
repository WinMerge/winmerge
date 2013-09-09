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
#ifndef diskio32_H
#define diskio32_H

#ifdef __cplusplus
extern "C" {
#endif

/*
    Interface of the RAWIO32.DLL
    
    */
struct SectorInfo
{
	 BYTE bDrive;
	 WORD wCylinder;
	 BYTE bHead;
	 BYTE bSector;
	 BYTE bCount;
};

typedef SectorInfo far *LPSectorInfo;

struct ExtDriveInfo
{
	BYTE drive;
	DWORD cylinders;
	DWORD heads;
	DWORD sectors;
	DWORD sectorsLo;
	DWORD sectorsHi;
	WORD bytesPerSector;
};

typedef struct ExtDriveInfo* LPExtDriveInfo;

/*
	Data structure used for EI13 functions
*/

struct BlockInfo
{
	BYTE drive;
	DWORD scheiss[2];
	WORD count;
};

typedef BlockInfo far *LPBlockInfo;


struct DiskAddressPacket
{
	BYTE size;
	BYTE reserved;
	WORD count;
	DWORD buffer;
	DWORD startLo;
	DWORD startHi;
};

struct DriveParameters
{
	WORD size;
	WORD flags;
	DWORD cylinders;
	DWORD heads;
	DWORD sectors;
	DWORD sectorsLo;
	DWORD sectorsHi;
	WORD bytesPerSector;
//---v2.0+ ---
	DWORD EDDptr;
//---v3.0 ---
	WORD signature;
	BYTE v3size;
	BYTE reserved[3];
	BYTE bus[4];
	BYTE _interface[8];
	BYTE interfacePath[8];
	BYTE devicePath[8];
	BYTE reserved2;
	BYTE checksum;
};

DWORD WINAPI ResetDisk(LPSectorInfo s);
DWORD WINAPI ReadPhysicalSector (LPSectorInfo s,  LPBYTE lpBuffer, DWORD  cbBuffSize);
DWORD WINAPI WritePhysicalSector (LPSectorInfo s, LPBYTE lpBuffer, DWORD  cbBuffSize);
DWORD WINAPI ReadDiskGeometry (LPSectorInfo s);
DWORD WINAPI EI13GetDriveParameters(LPExtDriveInfo b);
DWORD WINAPI EI13ReadSector (LPBlockInfo b, LPBYTE lpBuffer, DWORD bufferSize);
DWORD WINAPI EI13WriteSector(LPBlockInfo b, LPBYTE lpBuffer, DWORD bufferSize);

typedef DWORD (WINAPI* LPFNResetDisk)(LPSectorInfo s);
typedef DWORD (WINAPI* LPFNReadPhysicalSector)(LPSectorInfo s,  LPBYTE lpBuffer, DWORD  cbBuffSize);
typedef DWORD (WINAPI* LPFNWritePhysicalSector)(LPSectorInfo s, LPBYTE lpBuffer, DWORD  cbBuffSize);
typedef DWORD (WINAPI* LPFNReadDiskGeometry)(LPSectorInfo s);
typedef DWORD (WINAPI* LPFNEI13GetDriveParameters)(LPExtDriveInfo b);
typedef DWORD (WINAPI* LPFNEI13ReadSector)(LPBlockInfo b, LPBYTE lpBuffer, DWORD bufferSize);
typedef DWORD (WINAPI* LPFNEI13WriteSector)(LPBlockInfo b, LPBYTE lpBuffer, DWORD bufferSize);


#ifdef __cplusplus
} // extern "C" 
#endif

#endif // diskio32_H
