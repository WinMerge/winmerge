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
 * @file  BinTrans.h
 *
 * @brief Text to binary converter class declaration.
 *
 */
#ifndef BinTrans_h
#define BinTrans_h

#include "Simparr.h"

int create_bc_translation(BYTE** ppd, const char* src, int srclen, int charset, int binarymode);

/**
 * @brief A class translating between text export and binary data.
 */
class Text2BinTranslator : public SimpleString
{
public:
	int bCompareBin(Text2BinTranslator& tr2, int charmode, int binmode);
	Text2BinTranslator(const char* ps);
	static int iIsBytecode(const char* src, int len);
	static int iBytes2BytecodeDestLen(const BYTE* src, int srclen);
	static int iLengthOfTransToBin(const char* src, int srclen);
	static int iCreateBcTranslation(BYTE* dest, const char* src, int srclen, int charmode, int binmode);
	static int iTranslateOneBytecode(BYTE* dest, const char* src, int srclen, int binmode);
	static int iFindBytePos(const char* src, char c);
	static int iTranslateBytesToBC(char* pd, const BYTE* src, int srclen);

private:
	int GetTrans2Bin(SimpleArray<BYTE>& sa, int charmode, int binmode);
};

#endif // BinTrans_h
