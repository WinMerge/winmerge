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
 * @file  PMemoryBlock.cpp
 *
 * @brief Implementation of the memory block class.
 *
 */
#include "precomp.h"
#include "PMemoryBlock.h"

/**
 * @brief A constructor.
 */
PMemoryBlock::PMemoryBlock()
: m_lpbMemory(0)
, m_dwSize(0)
{
}

/**
 * @brief A constructor creating a memory block.
 * This constructor creates a new memory block. If existing memory address
 * is given then the data in that address is copied to the new block.
 * @param [in] dwSize Size of the memory block.
 * @param [in] lpbSource Memory address from where to copy data.
 *  If NULL then no data is copied.
 * @param [in] dwPadBytes Number of padding bytes.
 */
PMemoryBlock::PMemoryBlock(DWORD dwSize, LPBYTE lpbSource, DWORD dwPadBytes)
{
	if (dwSize)
	{
		m_lpbMemory = (LPBYTE) malloc(dwSize + dwPadBytes);
		if (m_lpbMemory)
		{
			if (lpbSource)
				CopyMemory(m_lpbMemory, lpbSource, dwSize);
			else
				ZeroMemory(m_lpbMemory, dwSize);
			m_dwSize = dwSize;
		}
		else
			m_dwSize = 0;
	}
	else
	{
		m_lpbMemory = 0;
		m_dwSize = 0;
	}
}

/**
 * @brief Create a memory block as a copy of existing block.
 * @param [in] objectSrc A memory block to copy.
 */
PMemoryBlock::PMemoryBlock(const PMemoryBlock& objectSrc)
{
	if (objectSrc.m_dwSize)
	{
		m_lpbMemory = (LPBYTE) malloc(objectSrc.m_dwSize);
		if (m_lpbMemory)
		{
			if (objectSrc.m_lpbMemory)
				CopyMemory(m_lpbMemory, objectSrc.m_lpbMemory, objectSrc.m_dwSize);
			else
				ZeroMemory(m_lpbMemory, objectSrc.m_dwSize);
			m_dwSize = objectSrc.m_dwSize;
		}
		else
			m_dwSize = 0;
	}
	else
	{
		m_lpbMemory = 0;
		m_dwSize = 0;
	}
}

/**
 * @brief A destructor freeing the memory block.
 */
PMemoryBlock::~PMemoryBlock()
{
	Delete();
}

/**
 * @brief Delete the memory block.
 */
void PMemoryBlock::Delete()
{
	if (m_lpbMemory)
	{
		free(m_lpbMemory);
		m_lpbMemory = NULL;
		m_dwSize = 0;
	}
}

/**
 * @brief Create an aligned memory block.
 * @param [in] dwUnalignedSize Block size without alignment bytes.
 * @param [in] dwAlignment Number of alignment bytes.
 * @param [in] lpbSource Memory address from where to copy data.
 *  If NULL then no data is copied.
 * @param [in] dwPadBytes Number of padding bytes.
 * @return TRUE if the create succeeded, FALSE otherwise.
 */
BOOL PMemoryBlock::CreateAligned(DWORD dwUnalignedSize, DWORD dwAlignment,
		LPBYTE lpbSource, DWORD dwPadBytes)
{
	DWORD dwRest = dwUnalignedSize % dwAlignment;
	if (dwRest)
	{
		dwUnalignedSize -= dwRest;
		dwUnalignedSize += dwAlignment;
	}
	return Create(dwUnalignedSize, lpbSource, dwPadBytes);
}

/**
 * @brief Create unaligned memory block.
 * @param [in] dwSize Size of the memory block in bytes.
 * @param [in] lpbSource Memory address from where to copy data.
 *  If NULL then no data is copied.
 * @param [in] dwPadBytes Number of padding bytes.
 * @return TRUE if the create succeeded, FALSE otherwise.
 */
BOOL PMemoryBlock::Create(DWORD dwSize, LPBYTE lpbSource, DWORD dwPadBytes)
{
	Delete();
	BOOL bSuccess = TRUE;
	if (dwSize)
	{
		m_lpbMemory = (LPBYTE) malloc(dwSize + dwPadBytes);
		if (m_lpbMemory)
		{
			if (lpbSource)
				CopyMemory(m_lpbMemory, lpbSource, dwSize);
			else
				ZeroMemory(m_lpbMemory, dwSize+dwPadBytes);
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
}

/**
 * @brief Assignment operator.
 * @param [in] objectSrc A memory block to copy.
 */
PMemoryBlock& PMemoryBlock::operator=(const PMemoryBlock& objectSrc)
{
	Delete();

	if (objectSrc.m_dwSize)
	{
		m_lpbMemory = (LPBYTE) malloc(objectSrc.m_dwSize);
		if (m_lpbMemory)
		{
			if (objectSrc.m_lpbMemory)
				CopyMemory(m_lpbMemory, objectSrc.m_lpbMemory, objectSrc.m_dwSize);
			else
				ZeroMemory(m_lpbMemory, objectSrc.m_dwSize);
			m_dwSize = objectSrc.m_dwSize;
		}
		else
			m_dwSize = 0;
	}
	else
	{
		m_lpbMemory = 0;
		m_dwSize = 0;
	}
	return *this;
}

/**
 * @brief Get a size of the block.
 * @return Size of the memory block.
 */
DWORD PMemoryBlock::GetObjectSize() const
{
	return m_dwSize;
}

/**
 * @brief Get a pointer to the memory in block.
 * @return Pointer to the memory block.
 */
LPBYTE PMemoryBlock::GetObjectMemory() const
{
	return m_lpbMemory;
}
