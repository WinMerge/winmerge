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
