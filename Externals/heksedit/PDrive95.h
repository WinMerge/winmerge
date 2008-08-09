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
};

#endif // PDrive95_h
