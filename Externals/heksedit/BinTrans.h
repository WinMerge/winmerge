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
 * @file  BinTrans.h
 *
 * @brief Text to binary converter class declaration.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: BinTrans.h 102 2008-11-05 22:16:31Z kimmov $

#ifndef BinTrans_h
#define BinTrans_h

#include "Simparr.h"

//-------------------------------------------------------------------
class Text2BinTranslator : public SimpleString
{
public:
	int bCompareBin( Text2BinTranslator& tr2, int charmode, int binmode );
	Text2BinTranslator( char* ps );
	static int iIsBytecode( char* src, int len );
	static int iBytes2BytecodeDestLen( char* src, int srclen );
	static int iLengthOfTransToBin( char* src, int srclen );
	static int iCreateBcTranslation( char* dest, char* src, int srclen, int charmode, int binmode );
	static int iTranslateOneBytecode( char* dest, char* src, int srclen, int binmode );
	static int iFindBytePos( char* src, char c );
	static int iTranslateBytesToBC (char* pd, unsigned char* src, int srclen);
private:
	int GetTrans2Bin( SimpleArray<char>& sa, int charmode, int binmode );
};

#endif // BinTrans_h
