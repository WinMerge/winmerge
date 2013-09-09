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
 * @file  PMemoryBlock.h
 *
 * @brief Declaration of PMemoryBlock class.
 *
 */
#ifndef PMemoryBlock_h
#define PMemoryBlock_h

/**
 * @brief A class handling dynamically allocated memory block. The maximum
 * block size is 4 GB.
 */
class PMemoryBlock
{
public:
	PMemoryBlock();
	PMemoryBlock(DWORD dwSize, LPBYTE lpbSource = NULL, DWORD dwPadBytes = 0);
	PMemoryBlock(const PMemoryBlock& objectSrc);
	virtual ~PMemoryBlock();

	PMemoryBlock& operator=(const PMemoryBlock& objectSrc);
	virtual BOOL Create(DWORD dwSize, LPBYTE lpbSource = NULL,
			DWORD dwPadBytes = 0);
	virtual BOOL CreateAligned(DWORD dwUnalignedSize, DWORD dwAlignment,
			LPBYTE lpbSource = NULL, DWORD dwPadBytes = 0);
	virtual void Delete();
	DWORD GetObjectSize() const;
	LPBYTE GetObjectMemory() const;

protected:
	LPBYTE m_lpbMemory;
	DWORD m_dwSize;
};

#endif // PMemoryBlock_h
