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
 * @file  LoadHexFile.h
 *
 * @brief Hex file loader class declaration.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: LoadHexFile.h 105 2008-11-05 22:57:48Z kimmov $

#ifndef _LOADHEXFILE_H_
#define _LOADHEXFILE_H_

#include "Simparr.h"

class hexfile_stream
{
public:
	// Methods
	virtual int lhgetc() = 0;
	virtual int lhungetc(int) = 0;
	virtual long lhtell() = 0;
	virtual int lhseek(long) = 0;
	virtual int scanf(const char *, int *) = 0;
	int lheatwhite();
};

class fhexfile_stream : public hexfile_stream
{
public:
	// Data
	FILE *i;
	// Constructor
	fhexfile_stream(FILE *i): i(i) { }
	// Methods
	virtual int lhgetc()
	{
		return fgetc(i);
	}
	virtual int lhungetc(int c)
	{
		return ungetc(c, i);
	}
	virtual long lhtell()
	{
		return ftell(i);
	}
	virtual int lhseek(long p)
	{
		return fseek(i, p, SEEK_SET);
	}
	virtual int scanf(const char *fmt, int *p)
	{
		return fscanf(i, fmt, p);
	}
};

class chexfile_stream : public hexfile_stream
{
public:
	// Data
	char *i;
	long lhpos;
	// Constructor
	chexfile_stream(char *i): i(i), lhpos(0) { }
	// Methods
	virtual int lhgetc()
	{
		int c = (int)(BYTE)i[lhpos];
		if (c)
			++lhpos;
		else
			c = EOF;
		return c;
	}
	virtual int lhungetc(int c)
	{
		return BYTE(i[--lhpos] = (BYTE)c);
	}
	virtual long lhtell()
	{
		return lhpos;
	}
	virtual int lhseek(long p)
	{
		return lhpos = p;
	}
	virtual int scanf(const char *fmt, int *p)
	{
		return sscanf(i + lhpos, fmt, p);
	}
};

class load_hexfile_0 : SimpleArray<unsigned char>
{
private:
	HWND hwnd;
	load_hexfile_0(HexEditorWindow &hexwnd)
	: hwnd(hexwnd.hwnd)
	{
	}
	bool StreamIn(hexfile_stream &);
public:
	static bool StreamIn(HexEditorWindow &, hexfile_stream &);
};

class load_hexfile_1 : SimpleArray<unsigned char>
{
private:
	HWND hwnd;
	int iMinOffsetLen;
	int bAutoOffsetLen;
	int iBytesPerLine;
	int iAutomaticBPL;
	int iCharacterSet;
	int bPartialStats;
	int iPartialOffset;
	load_hexfile_1(HexEditorWindow &hexwnd)
	: hwnd(hexwnd.hwnd)
	, iMinOffsetLen(hexwnd.iMinOffsetLen)
	, bAutoOffsetLen(hexwnd.bAutoOffsetLen)
	, iBytesPerLine(hexwnd.iBytesPerLine)
	, iAutomaticBPL(hexwnd.iAutomaticBPL)
	, iCharacterSet(hexwnd.iCharacterSet)
	, bPartialStats(hexwnd.bPartialStats)
	, iPartialOffset(hexwnd.iPartialOffset)
	{
	}
	bool StreamIn(hexfile_stream &);
public:
	static bool StreamIn(HexEditorWindow &, hexfile_stream &);
};

#endif // _LOADHEXFILE_H_
