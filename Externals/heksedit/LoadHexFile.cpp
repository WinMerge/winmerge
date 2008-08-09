#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "LoadHexFile.h"

int hexfile_stream::lheatwhite()
{
	int c;
	do c = lhgetc();
		while (isspace(c));
	lhungetc(c);
	return c;
}

bool load_hexfile_0::StreamIn(HexEditorWindow &hexwnd, hexfile_stream &hexin)
{
	load_hexfile_0 instance(hexwnd);
	if (!instance.StreamIn(hexin))
		return false;
	hexwnd.DataArray.Adopt(instance.m_pT, instance.m_nUpperBound, instance.m_nSize);
	instance.m_pT = 0;
	return true;
}

bool load_hexfile_0::StreamIn(hexfile_stream &hexin)
{
	int temp[4] = {0,0,0,0};
//Use only if U know c is a hex digit & not something else like' ', 'z' etc
#	define hex2nibble(c) (isdigit((c)) ? (c) - '0' : (c) - (islower((c)) ? 'a' : 'A') + 10)
	BYTE flnd = 0;//Start with the first nibble
	int ii = 0;
	int diio = 1;
	for (int i = 0 ; (temp[0] = hexin.lhgetc()) != EOF ; i++)
	{
		if (isxdigit(temp[0]))
		{
			if (!flnd)
			{
				if (!SetSize(ii + 1))
					return IDYES == MessageBox(hwnd, "Not enough memory to import data.\nCannot continue!\nDo you want to keep what has been found so far?", "Import Hexdump", MB_YESNO | MB_ICONERROR);
				ExpandToSize();
				m_pT[ii] = 0;
			}
			m_pT[ii] |= hex2nibble((BYTE)temp[0]) ;
			if (flnd)
				ii++;
			else
				m_pT[ii] <<= 4;
			flnd = !flnd;
		}
		else if (!isspace(temp[0]) && diio)
		{
			switch (MessageBox(hwnd, "Illegal character found.\nIgnore further illegal characters?", "Import Hexdump", MB_YESNOCANCEL | MB_ICONERROR))
			{
			case IDYES:
				diio = 0;
				break;
			case IDCANCEL:
				return false;
			}
		}
	}
	return true;
}

bool load_hexfile_1::StreamIn(hexfile_stream &hexin)
{
	int temp[4] = {0,0,0,0};
	unsigned char c[4] = {0,0,0,0};
	int i, ii = 0, ls, bpl, fo = 0, fol;
	int flnd = 1, dim = 1, diio = 1;
	bAutoOffsetLen = 1;
	do
	{
		//get the offset
		if (diio)
			ls = hexin.lhtell();
		int ol = 0;
		for (;;)
		{
			temp[0] = hexin.lhgetc();
			if (temp[0] == EOF)
				goto UnexpectedEndOfData;
			c[0] = (BYTE)temp[0];
			if (isspace(c[0]))
				break;
			if (!isxdigit(c[0]) && diio)
			{
				switch (MessageBox(hwnd, "Illegal character in offset.\nIgnore further invalid offsets?","Import Hexdump",MB_YESNOCANCEL | MB_ICONERROR))
				{
				case IDYES:
					diio = 0;
					break;
				case IDCANCEL:
					return FALSE;//bad file
				}
			}
			++ol;
		}

		if (flnd)
			iMinOffsetLen = fol = ol;
		else if (fol != ol)
			bAutoOffsetLen = 0;

		i = hexin.lhtell();

		if (diio)
		{
			int tmp = 0;
			hexin.lhseek(ls);
			hexin.scanf("%x", &tmp);
			if (flnd && tmp)
			{
				char msg[150];
				sprintf(msg,
					"The first offset found was 0x%x, which is greater than zero.\n"
					"Do you want to insert %d null bytes at the start of the data?", tmp, tmp);
				if (IDYES == MessageBox(hwnd, msg, "Import Hexdump", MB_YESNO | MB_ICONWARNING))
				{
					ii = tmp;
					if (!SetSize(ii))
						goto OutOfMemory;
					ExpandToSize();
					memset(m_pT, 0, ii);
				}
				else
				{
					fo = tmp;
					bPartialStats = TRUE;
					iPartialOffset = tmp;
				}
			}
			else if (ii + fo != tmp)
			{
				switch (MessageBox(hwnd, "Invalid offset found.\nIgnore further invalid offsets?","Import Hexdump",MB_YESNOCANCEL | MB_ICONWARNING))
				{
				case IDYES:
					diio = 0;
					break;
				case IDNO:
					return FALSE;//bad file
				}
			}
		}

		hexin.lhseek(i);

		if (hexin.lheatwhite() == EOF)
			goto UnexpectedEndOfData;

		ls = ii;//remember the start of the line in the DataArray

		//get data bytes
		for (bpl = 0 ;; bpl++)
		{
			//get the three chars
			for(i = 0 ; i < 3 ; i++)
			{
				temp[i] = hexin.lhgetc();
				if (temp[i] == EOF)
					goto UnexpectedEndOfData;
				c[i] = (BYTE)temp[i];
			}
			if (!(isxdigit(c[0]) && isxdigit(c[1]) && isspace(c[2])))
				goto IllegalCharacter;
			//yes we are fine
			//store the value no matter what
			if (!SetSize(ii + 1))
				goto OutOfMemory;
			ExpandToSize();
			//do this so that we don't overwrite memory outside the DataArray
			// - because sscanf requires an int for storage
			int tmp = 0;
			sscanf((char*)c, "%x", &tmp);//save it to tmp
			m_pT[ii] = (BYTE)tmp;
			ii++;//next byte

			for (i = 0 ; i < 3 ; i++)
			{
				temp[i] = hexin.lhgetc();
				if (temp[i] == EOF)
					goto UnexpectedEndOfData;
				c[i] = (BYTE)temp[i];
			}
			hexin.lhungetc(c[2]);
			hexin.lhungetc(c[1]);
			hexin.lhungetc(c[0]);
			if (c[0] == ' ' || c[0] =='_')
			{
				if (c[1] == c[0] && c[2] == ' ')
				{
					//get those back
					for (i = 0 ; i < 3 ; i++)
						if (hexin.lhgetc() == EOF)
							goto UnexpectedEndOfData;
					bpl++;
					for (;;bpl++)
					{
						for (i = 0 ; i < 3 ; i++)
						{
							temp[i] = hexin.lhgetc();
							if (temp[i] == EOF)
								return TRUE;//Assume the file is good
							c[i] = (BYTE)temp[i];
						}
						if (c[0] == '\r' && c[1] == '\n')
						{//We have missed the chars because of all the spaces
							hexin.lhungetc(c[2]);
							hexin.lhungetc(c[1]);
							hexin.lhungetc(c[0]);
							goto NextLine;
						}
						if (c[0] == ' ' && c[1] != ' ')
						{//We have found the start of the chars
							hexin.lhungetc(c[2]);
							hexin.lhungetc(c[1]);
							hexin.lhungetc(c[0]);
							break;
						}
					}
				}
				else if (c[0]=='_')
					goto IllegalCharacter;

				if (flnd)
				{
					iBytesPerLine = bpl + 1;
					iAutomaticBPL = 0;
				}
				break;

			}
			else if (!isxdigit(c[0]))
			{
				goto IllegalCharacter;
			}
		}//got the data bytes

		//weak point - assumes iCharSpace is 1
		//trash the extra space
		if (hexin.lhgetc() == EOF)
			goto UnexpectedEndOfData;
		
		//Verify that the data read by the above loop is correct and equal to that read by this loop
		for ( ; ls < ii ; ls++)
		{
			temp[0] = hexin.lhgetc();
			if (temp[0] == EOF)
				goto UnexpectedEndOfData;
			c[0] = (BYTE)temp[0];
			BYTE ct = m_pT[ls];
			//Get translated character - '.' for non-printables c[0] else

			c[1] = ct >= 32 && ct <= 126 || ct >= 160 && ct <= 255 || ct >= 145 && ct <= 146 ? ct : '.';
			//check if the data is the same
			if (!ct || c[1] == ct)
			{//0 (both OEM & ANSI translate) or one of those ranges where neither cset translates the character
				if (c[0] != c[1])
				{
BadData:
					if (dim)
					{
						switch (MessageBox(hwnd, "Character data does not agree with hex data.\nIgnore further mismatched data?\nNB: Hex data will be used when ignoring.", "Import Hexdump", MB_YESNOCANCEL | MB_ICONWARNING))
						{
						case IDYES:
							dim = 0;
							break;
						case IDCANCEL:
							return FALSE;//bad file
						}
					}
				}
			}
			else
			{
				if (c[0] == ct)
				{
					iCharacterSet = OEM_FIXED_FONT;
				}
				else
				{
					if (c[0] != c[1])
						goto BadData;
					iCharacterSet = ANSI_FIXED_FONT;
				}
			}
		}//get rest of line
NextLine:
		flnd = 0;
	} while (hexin.lheatwhite() != EOF);
	//parsing loop
	return TRUE;

IllegalCharacter:
	//someone has been buggering with the file & the syntax is screwed up
	//the next digit is not hex ' ' or '_'
	return IDYES == MessageBox(hwnd, "Illegal character in hex data.\nCannot continue!\nDo you want to keep what has been found so far?", "Import Hexdump", MB_YESNO | MB_ICONERROR);//bad file
UnexpectedEndOfData:
	return IDYES == MessageBox(hwnd, "Unexpected end of data found\nCannot continue!\nDo you want to keep what has been found so far?", "Import Hexdump", MB_YESNO | MB_ICONERROR);
OutOfMemory:
	return IDYES == MessageBox(hwnd, "Not enough memory to import data.\nCannot continue!\nDo you want to keep what has been found so far?", "Import Hexdump", MB_YESNO | MB_ICONERROR);
}

bool load_hexfile_1::StreamIn(HexEditorWindow &hexwnd, hexfile_stream &hexin)
{
	load_hexfile_1 instance(hexwnd);
	if (!instance.StreamIn(hexin))
		return false;
	if (IDYES == MessageBox(hexwnd.hwnd,
		"Would you like display settings found in the data to replace current ones?",
		"Import Hexdump", MB_YESNO))
	{
		hexwnd.iMinOffsetLen = instance.iMinOffsetLen;
		hexwnd.bAutoOffsetLen = instance.bAutoOffsetLen;
		hexwnd.iBytesPerLine = instance.iBytesPerLine;
		hexwnd.iAutomaticBPL = instance.iAutomaticBPL;
		hexwnd.iCharacterSet = instance.iCharacterSet;
		hexwnd.bPartialStats = instance.bPartialStats;
		hexwnd.iPartialOffset = instance.iPartialOffset;
	}
	hexwnd.DataArray.Adopt(instance.m_pT, instance.m_nUpperBound, instance.m_nSize);
	instance.m_pT = 0;
	return true;
}
