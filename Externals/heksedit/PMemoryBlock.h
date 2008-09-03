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
 * @file  PMemoryBlock.h
 *
 * @brief Declaration of PMemoryBlock class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: PMemoryBlock.h 27 2008-08-21 19:52:33Z kimmov $

#ifndef PMemoryBlock_h
#define PMemoryBlock_h

class PMemoryBlock
{
public:
	PMemoryBlock();
	PMemoryBlock( DWORD dwSize, LPBYTE lpbSource = NULL, DWORD dwPadBytes = 0 );
	PMemoryBlock( const PMemoryBlock& objectSrc );
	PMemoryBlock& operator=( const PMemoryBlock& objectSrc );
	virtual BOOL Create( DWORD dwSize, LPBYTE lpbSource = NULL, DWORD dwPadBytes = 0 );
	virtual BOOL CreateAligned( DWORD dwUnalignedSize, DWORD dwAlignment, LPBYTE lpbSource = NULL, DWORD dwPadBytes = 0 );
	virtual void Delete();
	virtual ~PMemoryBlock();
	DWORD GetObjectSize();
	LPBYTE GetObjectMemory();

protected:
	LPBYTE m_lpbMemory;
	DWORD m_dwSize;
};

#endif // PMemoryBlock_h
