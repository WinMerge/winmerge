#include "precomp.h"
#include "pdrive95.h"

#define SIZE_OF_ONE_BLOCK 512
#define SIZE_OF_ONE_BLOCKL 512L

#define PARTITION_TYPE_EXTENDED 5

typedef struct {
	BYTE DH; // Bit 7 is the active partition flag, bits 6-0 are zero.
	BYTE StartS; // Starting CHS in INT 13 call format.
	BYTE StartH;
	BYTE StartC;
	BYTE Type; // Partition type byte.
	BYTE StopS; // Starting CHS in INT 13 call format.
	BYTE StopH;
	BYTE StopC;
	DWORD LBA; // Starting LBA.
	DWORD SIZE; // Size in sectors.
} PARTITIONINFO, *LPPARTITIONINFO;

typedef struct
{
	PARTITIONINFO pi[4];
	WORD wSignature;
} MBR;

// RAW functions
LPFNResetDisk m_ResetDisk = NULL;
LPFNReadPhysicalSector m_ReadPhysicalSector = NULL;
LPFNWritePhysicalSector m_WritePhysicalSector = NULL;
LPFNReadDiskGeometry m_ReadDiskGeometry = NULL;
LPFNEI13GetDriveParameters m_EI13GetDriveParameters = NULL;
LPFNEI13ReadSector m_EI13ReadSector = NULL;
LPFNEI13WriteSector m_EI13WriteSector = NULL;

P9xPhysicalDrive::P9xPhysicalDrive()
{
	m_hDevice = 0;
}

P9xPhysicalDrive::~P9xPhysicalDrive()
{
}

BOOL P9xPhysicalDrive::ReadPartitionInfoRecursive(DWORD dwSector,INT64 TotalOffset,int depth )
{
	CHAR szHeader[80];
	if(depth)
	memset(szHeader,'\t',depth);
	szHeader[depth] = 0;

	TRACE("%sReading sector %d, offset %I64d\n", szHeader,dwSector,TotalOffset );

	BYTE mbr[SIZE_OF_ONE_BLOCK];
	if( ReadAbsolute(mbr,sizeof(mbr),dwSector) )
	{
		MBR* pMBR = (MBR*) &(mbr[446]);
		TRACE("%sSignature = 0x%04hx\n", szHeader,pMBR->wSignature);
		if( pMBR->wSignature != 0xaa55 )
			return FALSE;

		INT64 ScheissOffset = 0;

		for( int i = 0; i < 4; i++ )
		{
			PARTITIONINFO* source = &(pMBR->pi[i]);

			if( !source->SIZE && !source->LBA )
				continue;

			TRACE("%s-------------------- Partition %d --------------\n", szHeader, i );
			TRACE("%sDH=%d\n", szHeader, (int) source->DH);
			TRACE("%sSOURCE = %ld, LBA = %ld\n", szHeader, source->LBA, source->SIZE );

			PARTITION_INFORMATION pi;
			ZeroMemory(&pi,sizeof(pi));

			pi.PartitionLength.QuadPart = source->SIZE;
			pi.PartitionLength.QuadPart *= SIZE_OF_ONE_BLOCKL;
			pi.PartitionType = source->Type;

			if( i == 0 )
			{
				pi.StartingOffset.QuadPart = source->LBA;
				pi.StartingOffset.QuadPart *= SIZE_OF_ONE_BLOCKL;
				pi.StartingOffset.QuadPart += TotalOffset;
				ScheissOffset = pi.StartingOffset.QuadPart;
			}
			else
			{
				pi.StartingOffset.QuadPart = ScheissOffset;
			}
			ScheissOffset += pi.PartitionLength.QuadPart;

			TRACE("%sPartitionLength = %I64d\n", szHeader, pi.PartitionLength.QuadPart );
			TRACE("%sStartingOffset = %I64d\n", szHeader, pi.StartingOffset.QuadPart );

			P9xPartitionInfo* p9pi = new P9xPartitionInfo(&pi);
			m_PartitionInfo.AddTail( p9pi );
			if( pi.PartitionType == PARTITION_TYPE_EXTENDED )
			{
				if( !ReadPartitionInfoRecursive(dwSector + source->LBA,pi.StartingOffset.QuadPart,depth+1) )
				{
					p9pi->m_pi.StartingOffset.QuadPart += 63*512;
				}
			}

		}
	}
	return TRUE;
}

BOOL P9xPhysicalDrive::Open( int iDrive )
{
	Close();
	if( m_ResetDisk == NULL )
	{
		HINSTANCE hLibrary = (HINSTANCE)LoadLibrary( "RAWIO32.DLL" );
		if( !hLibrary )
		{
			printf("ERROR %s, unable to load RAWIO32.DLL.\n", (LPCSTR) GetLastErrorString() );
			return FALSE;
		}

	#define GETPROC(__NAME__) \
		m_##__NAME__ = (LPFN##__NAME__) GetProcAddress(hLibrary,#__NAME__); \
		if( !m_##__NAME__ ) { \
			printf("ERROR, missing export " #__NAME__ " IN DISKIO32.DLL\n" ); \
			return FALSE; \
		}

		GETPROC( ResetDisk )
		GETPROC( ReadPhysicalSector )
		GETPROC( WritePhysicalSector )
		GETPROC( ReadDiskGeometry )
		GETPROC( EI13GetDriveParameters )
		GETPROC( EI13ReadSector )
		GETPROC( EI13WriteSector )
	}

	TRACE("About to get geometry\n");
	m_bDriveNumber = (BYTE)(128 + iDrive);

	DISK_GEOMETRY dg;
	if( GetDriveGeometry(&dg) )
	{
		TRACE("Cylinders = %I64d\n", dg.Cylinders );
		TRACE("TracksPerCylinder = %d\n", dg.TracksPerCylinder );
		TRACE("SectorsPerTrack = %d\n", dg.SectorsPerTrack );
		TRACE("BytesPerSector = %d\n", dg.BytesPerSector );

		INT64 TotalSize = dg.Cylinders.QuadPart;
		TotalSize *= dg.TracksPerCylinder;
		TotalSize *= dg.SectorsPerTrack;
		TotalSize *= dg.BytesPerSector;
		TRACE( "Total Size In Bytes = %I64d\n", TotalSize );
		TotalSize /= 1024L;
		TotalSize /= 1024L;
		TRACE( "Total Size In Megabytes = %I64d\n", TotalSize );

		ReadPartitionInfoRecursive(0,0);

		m_hDevice = (HANDLE) 1;
		return TRUE;
	}
	return FALSE;
}

BOOL P9xPhysicalDrive::IsOpen()
{
	return (BOOL) m_hDevice;
}

void P9xPhysicalDrive::Close()
{
	m_hDevice = 0;
}

BOOL P9xPhysicalDrive::GetDriveGeometry( DISK_GEOMETRY* lpDG )
{
	lpDG->MediaType = Unknown;
	lpDG->BytesPerSector = SIZE_OF_ONE_BLOCKL;

	ExtDriveInfo edi;
	ZeroMemory(&edi,sizeof(edi));
	edi.drive = m_bDriveNumber;

	if( m_EI13GetDriveParameters(&edi) > 0 )
	{
		lpDG->Cylinders.QuadPart = *(INT64*)&(edi.sectorsLo);
		lpDG->TracksPerCylinder = 1; //edi.heads;
		lpDG->SectorsPerTrack = 1; //edi.cylinders;
		return TRUE;
	}

	SectorInfo si;
	ZeroMemory(&si,sizeof(si));
	si.bDrive = m_bDriveNumber;

	if( m_ReadDiskGeometry(&si) > 0 )
	{
		lpDG->Cylinders.QuadPart = si.wCylinder;
		lpDG->TracksPerCylinder = si.bHead;
		lpDG->SectorsPerTrack = si.bSector;
		return TRUE;
	}
	return FALSE;
}

BOOL P9xPhysicalDrive::ReadAbsolute( LPBYTE lpbMemory, DWORD dwSize, INT64 Sector )
{
	BlockInfo bi;
	bi.drive = m_bDriveNumber;
	*((INT64*)&(bi.scheiss[0])) = Sector;
	bi.count = (WORD) (dwSize / SIZE_OF_ONE_BLOCK);

	if( m_EI13ReadSector (&bi, lpbMemory, dwSize) > 0 )
		return TRUE;

/* ***** NOT SUPPORTED *******
struct SectorInfo
{
	BYTE bDrive;
	WORD wCylinder;
	BYTE bHead;
	BYTE bSector;
	BYTE bCount;
};

	if( ReadPhysicalSector(&bi, lpbMemory, dwSize) > 0 )
		return TRUE;
*/

	return FALSE;
}

BOOL P9xPhysicalDrive::GetDriveGeometryEx( DISK_GEOMETRY_EX* lpDG, DWORD dwSize )
{
	UNREFERENCED_PARAMETER( lpDG );
	UNREFERENCED_PARAMETER( dwSize );
	return FALSE;
}

BOOL P9xPhysicalDrive::GetDriveLayoutEx( LPBYTE lpbMemory, DWORD dwSize )
{
	UNREFERENCED_PARAMETER( lpbMemory );
	UNREFERENCED_PARAMETER( dwSize );
	return FALSE;
}

BOOL P9xPhysicalDrive::GetDriveLayout( LPBYTE lpbMemory, DWORD dwSize )
{
	DWORD dwBytesRequired = sizeof(DRIVE_LAYOUT_INFORMATION) + sizeof(PARTITION_INFORMATION)*(m_PartitionInfo.m_lCount-1);

	if( dwSize < dwBytesRequired )
		return FALSE;

	PDRIVE_LAYOUT_INFORMATION pli = (PDRIVE_LAYOUT_INFORMATION) lpbMemory;
	pli->PartitionCount = m_PartitionInfo.m_lCount;
	pli->Signature = 0;
	int index = 0;
	ENUMERATE( &m_PartitionInfo, P9xPartitionInfo, pI )
	{
		pli->PartitionEntry[index++] = pI->m_pi;
	}
	return TRUE;
}


