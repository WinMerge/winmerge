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
	HWindow *pwnd;
	load_hexfile_0(HexEditorWindow &hexwnd)
	: pwnd(hexwnd.pwnd)
	{
	}
	bool StreamIn(hexfile_stream &);
public:
	static bool StreamIn(HexEditorWindow &, hexfile_stream &);
};

class load_hexfile_1 : SimpleArray<unsigned char>
{
private:
	HWindow *pwnd;
	int iMinOffsetLen;
	int bAutoOffsetLen;
	int iBytesPerLine;
	int iAutomaticBPL;
	int iCharacterSet;
	int bPartialStats;
	INT64 iPartialOffset;
	load_hexfile_1(HexEditorWindow &hexwnd)
	: pwnd(hexwnd.pwnd)
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
