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
 * @file  EncoderLib.h
 *
 * @brief Definition of encode/decode plugin interface.
 *
 */
#ifndef _ENCODER_LIB_H_
#define _ENCODER_LIB_H_

struct s_MEMORY_ENCODING;

typedef void (WINAPI* LPFNEncodeMemoryFunction)( s_MEMORY_ENCODING* p );

/**
 * @brief Encoder description.
 */
typedef struct s_MEMORY_CODING_DESCRIPTION
{
	LPCSTR lpszDescription; /**< Name of the encoder. */
	LPFNEncodeMemoryFunction fpEncodeFunc; /**< Pointer to the encoder function. */
	LPCSTR lpvFuncParam;
} MEMORY_CODING_DESCRIPTION;

/**
 * @brief A struct defining encoded/decoded and operation.
 */
typedef struct s_MEMORY_ENCODING
{
	LPBYTE lpbMemory; /**< Pointer to data buffer. */
	DWORD dwSize; /**< Size of the data. */
	LPCTSTR lpszArguments; /**< Arguments for the function. */
	BOOL bEncode; /**< If TRUE encode, if false decode. */
	const MEMORY_CODING_DESCRIPTION *fpMcd; /**< Encoder description. */
} MEMORY_CODING;

EXTERN_C const MEMORY_CODING_DESCRIPTION *WINAPI GetMemoryCodings();
typedef const MEMORY_CODING_DESCRIPTION *(WINAPI *LPFNGetMemoryCodings)();

void WINAPI XorEncoder(MEMORY_CODING *);
void WINAPI Rot13Encoder(MEMORY_CODING *);

#endif // _ENCODER_LIB_H_
