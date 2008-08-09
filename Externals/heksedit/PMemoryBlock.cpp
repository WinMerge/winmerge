#include "precomp.h"
#include "PMemoryBlock.h"


PMemoryBlock::PMemoryBlock()
{
	m_lpbMemory = 0;
	m_dwSize = 0;
} // PMemoryBlock()

PMemoryBlock::PMemoryBlock( DWORD dwSize, LPBYTE lpbSource, DWORD dwPadBytes )
{
	if( dwSize )
	{
		m_lpbMemory = (LPBYTE) malloc( dwSize+dwPadBytes );
		if( m_lpbMemory )
		{
			if( lpbSource )
			{
				CopyMemory( m_lpbMemory, lpbSource, dwSize );
			}
			else
			{
				ZeroMemory( m_lpbMemory, dwSize );
			}
			m_dwSize = dwSize;
		}
		else
		{
			m_dwSize = 0;
		}
	}
	else
	{
		m_lpbMemory = 0;
		m_dwSize = 0;
	}
} // PMemoryBlock()

void PMemoryBlock::Delete()
{
	if( m_lpbMemory )
	{
		free( m_lpbMemory );
		m_lpbMemory = NULL;
		m_dwSize = 0;
	}
} // Delete()

BOOL PMemoryBlock::CreateAligned( DWORD dwUnalignedSize, DWORD dwAlignment, LPBYTE lpbSource, DWORD dwPadBytes )
{
	DWORD dwRest = dwUnalignedSize % dwAlignment;
	if( dwRest )
	{
		dwUnalignedSize -= dwRest;
		dwUnalignedSize += dwAlignment;
	}
	return Create( dwUnalignedSize, lpbSource, dwPadBytes );
}

BOOL PMemoryBlock::Create( DWORD dwSize, LPBYTE lpbSource, DWORD dwPadBytes )
{
	Delete();
	BOOL bSuccess = TRUE;
	if( dwSize )
	{
		m_lpbMemory = (LPBYTE) malloc( dwSize+dwPadBytes );
		if( m_lpbMemory )
		{
			if( lpbSource )
			{
				CopyMemory( m_lpbMemory, lpbSource, dwSize );
			}
			else
			{
				ZeroMemory( m_lpbMemory, dwSize+dwPadBytes );
			}
			m_dwSize = dwSize;
		}
		else
		{
			bSuccess = FALSE;
			m_dwSize = 0;
		}
	}
	else
	{
		m_lpbMemory = 0;
		m_dwSize = 0;
	}
	return bSuccess;
} // PMemoryBlock()

PMemoryBlock::PMemoryBlock( const PMemoryBlock& objectSrc )
{
	if( objectSrc.m_dwSize )
	{
		m_lpbMemory = (LPBYTE) malloc( objectSrc.m_dwSize );
		if( m_lpbMemory )
		{
			if( objectSrc.m_lpbMemory )
			{
				CopyMemory( m_lpbMemory, objectSrc.m_lpbMemory, objectSrc.m_dwSize );
			}
			else
			{
				ZeroMemory( m_lpbMemory, objectSrc.m_dwSize );
			}
			m_dwSize = objectSrc.m_dwSize;
		}
		else
		{
			m_dwSize = 0;
		}
	}
	else
	{
		m_lpbMemory = 0;
		m_dwSize = 0;
	}
} // PMemoryBlock()

PMemoryBlock& PMemoryBlock::operator=( const PMemoryBlock& objectSrc )
{
	if( m_lpbMemory )
	{
		free( m_lpbMemory );
		m_lpbMemory = NULL;
	}
	if( objectSrc.m_dwSize )
	{
		m_lpbMemory = (LPBYTE) malloc( objectSrc.m_dwSize );
		if( m_lpbMemory )
		{
			if( objectSrc.m_lpbMemory )
			{
				CopyMemory( m_lpbMemory, objectSrc.m_lpbMemory, objectSrc.m_dwSize );
			}
			else
			{
				ZeroMemory( m_lpbMemory, objectSrc.m_dwSize );
			}
			m_dwSize = objectSrc.m_dwSize;
		}
		else
		{
			m_dwSize = 0;
		}
	}
	else
	{
		m_lpbMemory = 0;
		m_dwSize = 0;
	}
	return *this;
} // operator=()

PMemoryBlock::~PMemoryBlock()
{
	if( m_lpbMemory )
		free( m_lpbMemory );
} // ~PMemoryBlock()

DWORD PMemoryBlock::GetObjectSize()
{
	return m_dwSize;
} // GetObjectSize()

LPBYTE PMemoryBlock::GetObjectMemory()
{
	return m_lpbMemory;
} // GetObjectMemory()
