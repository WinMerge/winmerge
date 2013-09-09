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
 * @file  PhysicalDrive.cpp
 *
 * @brief Drive/partition information class implementations.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "physicaldrive.h"
#include "pdrive95.h"
#include "pdrivent.h"
#include "LangArray.h"
#include "StringTable.h"

IPhysicalDrive *CreatePhysicalDriveInstance()
{
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof osvi);
	osvi.dwOSVersionInfoSize = sizeof osvi;
	if (!GetVersionEx(&osvi))
		osvi.dwPlatformId = 0;
	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		return new PNtPhysicalDrive;
	case VER_PLATFORM_WIN32_WINDOWS:
		return new P9xPhysicalDrive;
	}
	return 0;
}

LPTSTR PartitionInfo::GetNameAsString(PFormat *pFormat)
{
	if (m_bIsPartition)
	{
		pFormat->Format(GetLangString(IDS_DRIVES_DRIVE_PART), m_dwDrive + 1, m_dwPartition + 1, GetSizeAsString());
	}
	else
	{
		pFormat->Format(GetLangString(IDS_DRIVES_DRIVE_ONLY), m_dwDrive + 1, GetSizeAsString());
	}
	return pFormat->buffer;
}

LPTSTR PartitionInfo::GetSizeAsString(PFormat *pFormat)
{
	const double sizeInMB = (double) m_PartitionLength / (1024 * 1024);
	if (sizeInMB < 1024.0)
	{
		//fmt += GetLangString(IDS_ISO_MB);
		pFormat->Format(_T("%.2f MB"), sizeInMB);
	}
	else
	{
		const double sizeInGB = sizeInMB / 1024;
		if (sizeInGB < 1024.0)
		{
			//fmt += GetLangString(IDS_ISO_GB);
			pFormat->Format(_T("%.2f GB"), sizeInGB);
		}
		else
		{
			//fmt += GetLangString(IDS_ISO_TB);
			pFormat->Format(_T("%.2f TB"), sizeInGB / 1024);
		}
	}
	return pFormat->buffer;
}

void IPhysicalDrive::GetPartitionInfo(PList* lpList)
{
	lpList->DeleteContents();

	BYTE bLayoutInfo[20240];
	DISK_GEOMETRY dg;

	for( int iDrive = 0; iDrive < 8; iDrive++ )
	{
		if( !Open(iDrive) )
			continue;

		if( GetDriveGeometryEx( (DISK_GEOMETRY_EX*) bLayoutInfo, sizeof(bLayoutInfo) ) )
		{
			DISK_GEOMETRY& dgref = (((DISK_GEOMETRY_EX*)bLayoutInfo)->Geometry);
			dg = dgref;
			PartitionInfo* p = new PartitionInfo();
			p->m_dwDrive = (DWORD) iDrive;
			p->m_dwPartition = 0;
			p->m_bIsPartition = TRUE;
			p->m_dwBytesPerSector = dg.BytesPerSector;
			p->m_NumberOfSectors = dg.Cylinders.QuadPart;
			p->m_NumberOfSectors *= dg.SectorsPerTrack;
			p->m_NumberOfSectors *= dg.TracksPerCylinder;
			p->m_StartingOffset = 0;
			p->m_StartingSector = 0;
			p->m_PartitionLength = p->m_NumberOfSectors;
			p->m_PartitionLength *= dg.BytesPerSector;

			lpList->AddTail(p);
			if( GetDriveLayoutEx( bLayoutInfo, sizeof(bLayoutInfo) ) )
			{
				PDRIVE_LAYOUT_INFORMATION_EX pLI = (PDRIVE_LAYOUT_INFORMATION_EX)bLayoutInfo;
				for( DWORD iPartition = 0; iPartition < pLI->PartitionCount; iPartition++ )
				{
					PARTITION_INFORMATION_EX* pi = &(pLI->PartitionEntry[iPartition]);

					PartitionInfo* p = new PartitionInfo();
					p->m_dwDrive = (DWORD) iDrive;
					p->m_dwPartition = (DWORD) iPartition;
					p->m_bIsPartition = TRUE;
					p->m_dwBytesPerSector = dg.BytesPerSector;
					p->m_NumberOfSectors = pi->PartitionLength.QuadPart;
					p->m_NumberOfSectors /= dg.BytesPerSector;
					p->m_StartingOffset = pi->StartingOffset.QuadPart;
					p->m_StartingSector = p->m_StartingOffset;
					p->m_StartingSector /= dg.BytesPerSector;
					p->m_PartitionLength = pi->PartitionLength.QuadPart;
					lpList->AddTail(p);
				}
			}
		}
		else
		{
			if( GetDriveGeometry( &dg ) )
			{
				PartitionInfo* p = new PartitionInfo();
				p->m_dwDrive = (DWORD) iDrive;
				p->m_dwPartition = 0;
				p->m_bIsPartition = FALSE;
				p->m_dwBytesPerSector = dg.BytesPerSector;
				p->m_NumberOfSectors = dg.Cylinders.QuadPart;
				p->m_NumberOfSectors *= dg.SectorsPerTrack;
				p->m_NumberOfSectors *= dg.TracksPerCylinder;
				p->m_StartingOffset = 0;
				p->m_StartingSector = 0;
				p->m_PartitionLength = p->m_NumberOfSectors;
				p->m_PartitionLength *= dg.BytesPerSector;

				lpList->AddTail(p);

				if( GetDriveLayout( bLayoutInfo, sizeof(bLayoutInfo) ) )
				{
					PDRIVE_LAYOUT_INFORMATION pLI = (PDRIVE_LAYOUT_INFORMATION)bLayoutInfo;
					for( DWORD iPartition = 0; iPartition < pLI->PartitionCount; iPartition++ )
					{
						PARTITION_INFORMATION* pi = &(pLI->PartitionEntry[iPartition]);

						if( !pi->PartitionLength.QuadPart )
							continue;

						PartitionInfo* p = new PartitionInfo();
						p->m_dwDrive = (DWORD) iDrive;
						p->m_dwPartition = (DWORD) iPartition;
						p->m_bIsPartition = TRUE;
						p->m_dwBytesPerSector = dg.BytesPerSector;
						p->m_NumberOfSectors = pi->PartitionLength.QuadPart;
						p->m_NumberOfSectors /= dg.BytesPerSector;
						p->m_StartingOffset = pi->StartingOffset.QuadPart;
						p->m_StartingSector = p->m_StartingOffset;
						p->m_StartingSector /= dg.BytesPerSector;
						p->m_PartitionLength = pi->PartitionLength.QuadPart;
						lpList->AddTail(p);
					}
				}
			}
		}
		Close();
	}
}
