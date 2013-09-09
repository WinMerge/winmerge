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
 * @file  BinTrans.cpp
 *
 * @brief Text to binary converter class implementation.
 *
 */
#include "precomp.h"
#include "Simparr.h"
#include "hexwnd.h"
#include "BinTrans.h"

//-------------------------------------------------------------------
// Create translation of bytecode-string.
// Return: Length of resulting string.
// ppd = pointer to pointer to result, must be delete[]-ed later.
// If the input string was empty, no translated array is created and zero is returned.
int create_bc_translation(BYTE **ppd, const char* src, int srclen, int charset, int binarymode)
{
	int destlen = Text2BinTranslator::iLengthOfTransToBin(src, srclen);
	if (destlen > 0)
	{
		*ppd = new BYTE[destlen];
		Text2BinTranslator::iCreateBcTranslation(*ppd, src, srclen,
				charset, binarymode);
		return destlen;
	}
	else
	{
		// Empty input string => don't allocate anything and return 0.
		*ppd = NULL;
		return 0;
	}
}

/**
 * @brief Find character from string.
 * @param [in] src String from which the character is searched.
 * @param [in] c Character to search.
 * @return Position of the found char in the string, or length
 *  of the string if character not found.
 */
int Text2BinTranslator::iFindBytePos(const char* src, char c)
{
	const char *ptr = strchr(src, c);
	return static_cast<int>(ptr != NULL ? ptr - src : strlen(src));
}

/**
 * @brief Create translation of bytecode-string.
 * @param [in] sa Array where the string is added.
 * @param [in] charmode ANSI/OEM character set.
 * @param [in] binmode BIG/LITTLE endian.
 * @return TRUE if translation succeeded, FALSE otherwise.
 */
int Text2BinTranslator::GetTrans2Bin(SimpleArray<BYTE>& sa, int charmode, int binmode)
{
	sa.ClearAll();

	int destlen = iLengthOfTransToBin(m_pT, m_nUpperBound);
	if (destlen > 0)
	{
		sa.SetSize(destlen);
		sa.ExpandToSize();
		iCreateBcTranslation(sa, m_pT, m_nUpperBound, charmode, binmode);
		return TRUE;
	}
	else
	{
		// Empty input string => don't allocate anything and return 0.
		return FALSE;
	}

	return TRUE;
}

/**
 * @brief  Calculate length of bytecode string for char array. 
 * @param [in] src Char array to calculate.
 * @param [in] srclen How many chars to calculate.
 * @return Length of bytecode-string including zero-byte.
 */
int Text2BinTranslator::iBytes2BytecodeDestLen(const BYTE *src, int srclen)
{
	int destlen = 1;
	for (int i = 0; i < srclen; i++)
	{
		if (src[i] == '<')
			destlen += 2; // Escapecode needed.
		else if (src[i] == '\\')
			destlen += 2; // Escapecode needed.
		else if (src[i] >= 32 && src[i] < 127)
			destlen++; // Normal char.
		else if (src[i] == 10 || src[i] == 13)
			destlen++; // LF/CR.
		else
			destlen += 7; // Escapecode needed: <bh:xx>
	}
	return destlen;
}

/**
 * Is the string a bytecode?
 * @param [in] src String to check.
 * @param [in] len Lengt of the string.
 * @return 0 if no  bytecode, lenght of the bytecode (1/2/4/8) else.
 */
int Text2BinTranslator::iIsBytecode(const char* src, int len)
{
	if (len < 5)
		return 0; // Too short to be a bytecode

	if (src[0] != '<')
		return 0;

	if (src[1] != 'b' && src[1] != 'w' && src[1] != 'l' && src[1] != 'f' &&
			src[1] != 'd')
		return 0; // Wrong first option.

	if (src[2] != 'd' && src[2] != 'h' && src[2] != 'l' && src[2] != 'o')
		return 0; // Wrong second option.

	if (src[3] != ':')
		return 0;

	int j;
	for (j = 4; j < len; j++)
	{
		if (src[j] == '>')
			break;
	}

	if (j == 4 || j == len)
		// No concluding ">" found.
		return 0;

	for (int k = 4; k < j; k++)
	{
		switch (src[2])
		{
		case 'd':
			if ((src[k] >= '0' && src[k] <= '9') || src[k] == '-')
				continue;
			else
				return 0; // Non-digit found.
			break;

		case 'h':
			if ((src[k] >= '0' && src[k] <= '9') ||
					(src[k] >= 'a' && src[k] <= 'f'))
				continue;
			else
				return 0; // Non-hex-digit.
			break;

		case 'o': case 'l': // float or double.
			if ((src[k] >= '0' && src[k] <= '9') || src[k] == '-' ||
					src[k] == '.' || src[k] == 'e' || src[k] == 'E')
				continue;
			else
				return 0;
			break;
		}
	}
	
	// Return length by type
	switch (src[1])
	{
	default:
	case 'b': return 1;
	case 'w': return 2;
	case 'l': return 4;
	case 'f': return 4;
	case 'd': return 8;
	}
}

//-------------------------------------------------------------------
// Get value of *one* bytecode token.
// Return: value of code.
// bytecode must be checked before!!
int Text2BinTranslator::iTranslateOneBytecode(BYTE* dest, const char* src, int srclen, int binmode )
{
	int i, k=0;
	char buf[50];
	for (i=4; i<srclen; i++)
	{
		if (src[i]=='>')
			break;
		else
		{
			buf[k++] = src[i];
		}
	}
	buf[k] = 0;
	int value;
	float fvalue;
	double dvalue;
	switch (src[2]) // Get value from text.
	{
	case 'd':
		sscanf(buf, "%d", &value);
		break;

	case 'h':
		sscanf(buf, "%x", &value);
		break;

	case 'l':
		sscanf(buf, "%f", &fvalue);
		break;

	case 'o':
		sscanf(buf, "%lf", &dvalue);
		break;
	}

	if (binmode == HexEditorWindow::ENDIAN_LITTLE)
	{
		switch (src[1])
		{
		case 'b':
			dest[0] = (char) value;
			break;

		case 'w':
			dest[0] = (char)(value & 0xff);
			dest[1] = (char)((value & 0xff00)>>8);
			break;

		case 'l':
			dest[0] = (char)(value & 0xff);
			dest[1] = (char)((value & 0xff00)>>8);
			dest[2] = (char)((value & 0xff0000)>>16);
			dest[3] = (char)((value & 0xff000000)>>24);
			break;

		case 'f':
			*((float*)dest) = fvalue;
			break;

		case 'd':
			*((double*)dest) = dvalue;
			break;
		}
	}
	else // BIGENDIAN_MODE
	{
		switch (src[1])
		{
		case 'b':
			dest[0] = (char) value;
			break;

		case 'w':
			dest[0] = HIBYTE(LOWORD(value));
			dest[1] = LOBYTE(LOWORD(value));
			break;

		case 'l':
			dest[0] = HIBYTE(HIWORD(value));
			dest[1] = LOBYTE(HIWORD(value));
			dest[2] = HIBYTE(LOWORD(value));
			dest[3] = LOBYTE(LOWORD(value));
			break;

		case 'f':
			{
				char* p = (char*) &fvalue;
				int i;
				for (i=0; i<4; i++)
				{
					dest[i] = p[3-i];
				}
			}
			break;

		case 'd':
			{
				char* p = (char*) &dvalue;
				int i;
				for (i = 0; i < 8; i++)
				{
					dest[i] = p[7 - i];
				}
			}
			break;
		}
	}
	return value;
}

//-------------------------------------------------------------------
// Get length of translated array of bytes from text.
int Text2BinTranslator::iLengthOfTransToBin(const char* src, int srclen)
{
	int i, destlen = 0, l, k;
	for (i = 0; i < srclen; i++)
	{
		if ((l = iIsBytecode (&(src[i]), srclen - i)) == 0)
		{
			if (src[i] == '\\')
			{
				if (i + 1 < srclen)
				{
					if (src[i + 1] == '<')
					{
						// Code for "<" alone without decoding.
						destlen++;
						i++;
					}
					else if (src[i + 1] == '\\')
					{
						// Code for "\\".
						destlen++;
						i++;
					}
					else
					{
						destlen++;
					}
				}
				else
				{
					destlen++;
				}
			}
			else
			{
				destlen++;
			}
		}
		else
		{
			destlen += l;
			for (k = i; i < srclen; k++)
			{
				if (src[k] == '>')
					break;
			}
			i = k;
		}
	}
	return destlen;
}

//-------------------------------------------------------------------
// dest must be set to right length before calling.
int Text2BinTranslator::iCreateBcTranslation(BYTE* dest, const char* src, int srclen, int charmode, int binmode )
{
	int i, di = 0, bclen;
	for (i = 0; i < srclen; i++)
	{
		if ((bclen = iIsBytecode(&(src[i]), srclen - i)) > 0) // Get length of byte-code.
		{
			// Bytecode found.
			iTranslateOneBytecode(&(dest[di]), &(src[i]), srclen - i, binmode);
			di += bclen;
			i += iFindBytePos(&(src[i]), '>');
		}
		else // Normal character.
		{
			if (src[i] == '\\') // Special char "\<" or "\\"?
			{
				if (i + 1 < srclen)
				{
					if (src[i + 1] == '<')
					{
						dest[di++] = '<'; // Special char recognized.
						i++;
					}
					else if (src[i + 1] == '\\')
					{
						dest[di++] = '\\'; // Special char recognized.
						i++;
					}
					else
						dest[di++] = src[i]; // Unknown special char.
				}
				else
					dest[di++] = src[i]; // Not enough space for special char.
			}
			else
			{
				// No special char.
				switch (charmode)
				{
				case ANSI_SET:
					dest[di++] = src[i];
					break;
				case OEM_SET:
					CharToOemBuffA(&src[i], (LPSTR)&dest[di++], 1);
					break;
				}
			}
		}
	}
	return di;
}

//-------------------------------------------------------------------
Text2BinTranslator::Text2BinTranslator(const char* ps)
{
	// Create a Text2BinTranslator from a normal char array-string.
	m_nGrowBy = 64;
	Clear();
	SetToString(ps);
}

//-------------------------------------------------------------------
int Text2BinTranslator::bCompareBin( Text2BinTranslator& tr2, int charmode, int binmode )
{
	SimpleArray<BYTE> sa1, sa2;
	GetTrans2Bin(sa1, charmode, binmode);
	tr2.GetTrans2Bin(sa2, charmode, binmode);
	return (sa1 == sa2);
}

/**
 * @brief Translate an array of bytes to a text string using special syntax.
 * @param [out] pd Buffer where string is written to.
 * @param [in] src Source byte array.
 * @param [in] srclen Length of the source byte array.
 * @return Length of string including zero-byte.
 */
int Text2BinTranslator::iTranslateBytesToBC(char* pd, const BYTE* src, int srclen)
{
	int i, k = 0;
	char buf[16];
	for (i = 0; i < srclen; i++)
	{
		if (src[i] == '<')
		{
			pd[k++] = '\\';
			pd[k++] = '<';
		}
		else if (src[i] == '\\')
		{
			pd[k++] = '\\';
			pd[k++] = '\\';
		}
		else if (src[i] >= 32 && src[i] < 127)
		{
			pd[k++] = src[i];
		}
		else if (src[i] == 10 || src[i] == 13)
		{
			pd[k++] = src[i];
		}
		else
		{
			pd[k++] = '<';
			pd[k++] = 'b';
			pd[k++] = 'h';
			pd[k++] = ':';
			sprintf(buf, "%2.2x", src[i]);
			pd[k++] = buf[0];
			pd[k++] = buf[1];
			pd[k++] = '>';
		}
	}
	pd[k] = '\0';
	return k + 1;
}
