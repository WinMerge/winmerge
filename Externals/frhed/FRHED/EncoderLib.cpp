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
 * @file EncoderLib.cpp
 *
 * @brief Build-in encoder functions implementations.
 *
 */
#include "precomp.h"
#include "EncoderLib.h"

/**
 * @brief Build-in XOR -1 encoder.
 * @param [in, out] p Data (and parameters) to encode.
 */
void WINAPI XorEncoder(MEMORY_CODING *p)
{
	LPBYTE q = p->lpbMemory;
	LPBYTE qMax = q + p->dwSize;
	while (q < qMax)
		*(q++) ^= -1;
}

/**
 * @brief Build-in ROT-13 encoder.
 * @param [in, out] p Data (and parameters) to encode.
 */
void WINAPI Rot13Encoder(MEMORY_CODING *p)
{
	LPBYTE q = p->lpbMemory;
	LPBYTE qMax = q + p->dwSize;
	while (q < qMax)
		*(q++) = isalpha(*q) ? (BYTE)(tolower(*q) < 'n' ? *q + 13 : *q - 13) : *q;
}
