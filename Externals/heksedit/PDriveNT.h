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


