/* markdown.cpp: Pull-parse XML sources
 * Copyright (c) 2005 Jochen Tucht
 *
 * License:	This program is free software; you can redistribute it and/or modify
 *			it under the terms of the GNU General Public License as published by
 *			the Free Software Foundation; either version 2 of the License, or
 *			(at your option) any later version.
 *
 *			This program is distributed in the hope that it will be useful,
 *			but WITHOUT ANY WARRANTY; without even the implied warranty of
 *			MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *			GNU General Public License for more details.
 *
 *			You should have received a copy of the GNU General Public License
 *			along with this program; if not, write to the Free Software
 *			Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * OS/Libs:	Win32/MFC/shlwapi/iconv
 *			iconv.dll is loaded on demand, and is not required as long as
 *			program doesn't call iconv based methods.
 *
 * Remarks:	Pull-parsing is a very simple way to parse XML. It does not require
 *			callback functions, and it does not build object trees in memory. It
 *			just travels through plain source.
 *
 *			This library reads source text from memory. It can safely operate
 *			on memory mapped files, as it does not require text to be zero-
 *			terminated. It will also read most of the usual meta stuff (<? ?>,
 *			<!-- -->, <![ []]>, and DTD tags), but applying meta information is
 *			left to the caller. Thus, the library does not exactly implement an
 *			XML parser. It just helps reading XML.
 *			
 *			This library is not modeled after an existing pull parsing API,
 *			so don't expect to find the same methods you've seen elsewhere.
 *			In particular, this library does not follow XmlPull's event model,
 *			but attempts to be somewhat closer to a tree-based API.
 *			For simplicity, this library does not perform any validation, nor
 *			provide error handling other than returning empty text in case it
 *			fails to retrieve something.
 *
 *			The name of the core class, CMarkdown, actually was going to be
 *			CMarkup when I came across another XML tool with same name on
 *			CodeProject. Like TinyXml and XMLite, and unlike CMarkdown, CMarkup
 *			follows DOM-like approach, suffering from considerable memory
 *			footprint. Anyway, class name CMarkdown somewhat reflects the nature
 *			of pull-parsing, pulling down the leaves of an XML tree so programs
 *			can reach them from a flat loop, rather than climb up the tree and
 *			push the leaves to some callback function, or preprocess the entire
 *			tree in some way before allowing programs to retrieve content.
 *
 *			Recommended reading:
 *
 *			www.sosnoski.com/articles/parsing1.html (SAX2 Basics)
 *			www.sosnoski.com/articles/parsing2.html (SAX vs Pull)
 *			www.sosnoski.com/articles/parsing3.html (Performance)
 *			www.xml.com/pub/a/2002/08/14/xmlpull.html (XMLPULL API)
 *			www.xml.com/pub/a/2002/09/25/xmlpull.html (response to above)
 *			www.stylusstudio.com/xmldev/200205/post61120.html (discussion)
 *
 *			There are lots of related articles on the web, though.

Please mind 2. a) of the GNU General Public License, and log your changes below.

DATE:		BY:					DESCRIPTION:
==========	==================	================================================
2005/01/15	Jochen Tucht		Created
2005/02/26	Jochen Tucht		Load iconv.dll through DLLPSTUB
*/

#include "stdafx.h"
#include <afxinet.h>
#include <shlwapi.h> // StrToIntEx
#include "markdown.h"

ICONV::Proxy ICONV =
{
	{ 0, 0, 0 },
	"ICONV.DLL",
	"libiconv_open",
	"libiconv",
	"libiconv_close",
	"libiconvctl",
	"libiconvlist",
	"_libiconv_version",
	(HMODULE)0
};

CMarkdown::Converter::Converter(const char *tocode, const char *fromcode)
{
	handle = INVALID_HANDLE_VALUE;
	if (tocode && fromcode) // && strcmp(tocode, fromcode))
	{
		handle = ICONV->iconv_open(tocode, fromcode);
	}
}

CMarkdown::Converter::~Converter()
{
	if (handle != INVALID_HANDLE_VALUE)
	{
		ICONV->iconv_close(handle);
	}
}

size_t CMarkdown::Converter::iconv(const char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft) const
{
	return handle != INVALID_HANDLE_VALUE ? ICONV->iconv(handle, inbuf, inbytesleft, outbuf, outbytesleft) : -1;
}

template<> UINT AFXAPI HashKey(BSTR B)
{
	return MAKELONG(B[0], lstrlenW(B));
}

void CMarkdown::EntityMap::Load()
{
	SetAt(SysAllocString(L"amp"), SysAllocString(L"&"));
	SetAt(SysAllocString(L"quot"), SysAllocString(L"\""));
	SetAt(SysAllocString(L"apos"), SysAllocString(L"'"));
	SetAt(SysAllocString(L"lt"), SysAllocString(L"<"));
	SetAt(SysAllocString(L"gt"), SysAllocString(L">"));
}

void CMarkdown::EntityMap::Load(CMarkdown &markup, const CMarkdown::Converter &converter)
{
	while (markup.Move("!ENTITY"))
	{
		HSTR hstrValue = 0, hstrKey = markup.GetAttribute(0, &hstrValue);
		if (hstrKey)
		{
			SetAt(hstrKey->Convert(converter)->B, hstrValue->Convert(converter)->B);
		}
	}
}

CMarkdown::HSTR CMarkdown::_HSTR::Unicode(UINT codepage)
{
	HSTR H = this;
	if (codepage != 1200) // 1200 means 'no conversion'
	{
		int a = SysStringByteLen(B);
		int w = MultiByteToWideChar(codepage, 0, A, a, 0, 0);
		H = (HSTR)SysAllocStringLen(0, w);
		MultiByteToWideChar(codepage, 0, A, a, H->W, w);
		SysFreeString(B);
	}
	return H;
}

CMarkdown::HSTR CMarkdown::_HSTR::Octets(UINT codepage)
{
	HSTR H = this;
	if (codepage != 1200) // 1200 means 'no conversion'
	{
		int w = SysStringLen(B);
		int a = WideCharToMultiByte (codepage, 0, W, w, 0, 0, 0, 0);
		H = (HSTR)SysAllocStringByteLen(0, a);
		WideCharToMultiByte(codepage, 0, W, w, H->A, a, 0, 0);
		SysFreeString(B);
	}
	return H;
}

CMarkdown::HSTR CMarkdown::_HSTR::Convert(const CMarkdown::Converter &converter)
{
	HSTR H = this;
	// reset iconv internal state and tell if converter is valid
	if (converter.iconv(0, 0, 0, 0) != -1)
	{
		const char *R = A;
		size_t r = SysStringByteLen(B);
		const char *S = R;
		size_t s = r;
		size_t d = 0;
		while (r)
		{
			char buffer[100];
			char *C = buffer;
			size_t c = sizeof buffer;
			if (converter.iconv(&R, &r, &C, &c) == -1 && c == sizeof buffer)
			{
				// some error other than 'outbuf exhausted': stop here
				break;
			}
			d += sizeof buffer - c;
		}
		H = (HSTR)SysAllocStringByteLen(0, d);
		char *D = H->A;
		// nothing should go wrong here as outbuf has now accurate size
		converter.iconv(0, 0, 0, 0); // reset iconv internal state
		converter.iconv(&S, &s, &D, &d); // convert entire string
		SysFreeString(B);
	}
	return H;
}

CMarkdown::HSTR CMarkdown::_HSTR::Resolve(const CMarkdown::EntityMap &map)
{
	HSTR H = this;
	BSTR p, q = H->B;
	while ((p = StrChrW(q, '&')) != NULL && (q = StrChrW(p, ';')) != NULL)
	{
		*q = '\0';
		OLECHAR *key = p + 1;
		BOOL found = FALSE;
		EntityString value;
		int cchValue = 1;
		OLECHAR chValue[2];
		if (*key == '#')
		{
			int ordinal = '?';
			*key = '0';
			found = StrToIntExW(key, STIF_SUPPORT_HEX, &ordinal);
			*key = '#';
			chValue[0] = (OLECHAR)ordinal;
			chValue[1] = '\0';
			value.B = chValue;
		}
		else
		{
			found = map.Lookup(key, value);
			cchValue = SysStringLen(value.B);
		}
		*q = ';';
		++q;
		if (found)
		{
			int i = p - H->B;
			int j = q - H->B;
			int cchKey = q - p;
			if (int cchGrow = cchValue - cchKey)
			{
				BSTR B = H->B;
				int b = SysStringLen(B);
				size_t cbMove = (b - j) * sizeof(OLECHAR);
				if (cchGrow < 0)
				{
					memmove(q + cchGrow, q, cbMove);
				}
				if (!SysReAllocStringLen(&B, B, b + cchGrow))
				{
					continue;
				}
				H = (HSTR)B;
				p = H->B + i;
				q = H->B + j;
				if (cchGrow > 0)
				{
					memmove(q + cchGrow, q, cbMove);
				}
			}
			memcpy(p, value.B, cchValue * sizeof(OLECHAR));
			q = p + cchValue;
		}
	}
	return H;
}

CMarkdown::HSTR CMarkdown::_HSTR::Trim(const OLECHAR *pszTrimChars)
{
	HSTR H = this;
	BSTR B = H->B;
	if (StrTrimW(B, pszTrimChars) && SysReAllocStringLen(&B, B, lstrlenW(B))) // 1200 means 'no conversion'
	{
		H = (HSTR)B;
	}
	return H;
}

CMarkdown::CMarkdown(const char *upper, const char *ahead):
first(0), lower(0), upper(upper), ahead(ahead)
{
	if (CMarkdown::ahead > CMarkdown::upper)
	{
		--CMarkdown::ahead;
	}
}

CMarkdown::operator bool()
{
	return upper < ahead &&
	(
		MAKEWORD(upper[0], upper[1]) != MAKEWORD('<', '/')
	&&	MAKEWORD(upper[0], upper[1]) != MAKEWORD(']', '>')
	);
}

void CMarkdown::Scan()
{
	if (first == upper && *this)
	{
		int depth = 0;
		do
		{
			switch (*upper++)
			{
			case '/':
				if (upper[-2] == '<')
					depth -= 2;
				break;
			case '?':
				if (upper[-2] == '<')
				{
					do
					{
					} while (upper <= ahead && (*upper++ != '>' || upper[-2] != '?'));
					--depth;
				}
				break;
			case '!':
				if (upper[-2] == '<' && upper <= ahead)
				{
					if (*upper == '-')
					{
						do
						{
						} while (upper <= ahead && (*upper++ != '>' || upper[-2] != '-' || upper[-3] != '-'));
						--depth;
					}
					else if (*upper == '[')
					{
						do
						{
						} while (upper <= ahead && (*upper++ != '>' || upper[-2] != ']' || upper[-3] != ']'));
						--depth;
					}
					else
					{
						int quoting = 0;
						do
						{
							switch (*upper)
							{
							case '"':
								if (!(quoting & 1))
									quoting ^= 2;
								break;
							case '\'': 
								if (!(quoting & 2))
									quoting ^= 1;
								break;
							case '<':
							case '[':
								if (!quoting)
									++depth;
								break;
							case ']':
							case '>':
								if (!quoting)
									--depth;
								break;
							}
						} while (++upper <= ahead && depth);
					}
				}
				break;
			case '>':
				if (upper[-2] == '/')
					--depth;
				break;
			case '<':
				++depth;
				break;
			}
		} while (upper <= ahead && depth);
	}
}

CMarkdown &CMarkdown::Move()
{
	Scan();
	while (*this && *upper != '<')
	{
		++upper;
	}
	first = lower = upper;
	return *this;
}

CMarkdown &CMarkdown::Move(const char *name)
{
	while (Move())
	{
		const char *q = lower;
		const char *p = q + 1;
		unsigned char c;
		do
		{
			++q;
		} while (q <= ahead && !isspace(c = *q) && c != '[' && c != '>' && c != '"' && c != '\'' && c != '=');
		int length = q - p;
		if (memcmp(p, name, length) == 0 && name[length] == '\0')
		{
			break;
		}
	}
	return *this;
}

bool CMarkdown::Pull()
{
	if (lower < ahead && (*lower != '<' || ++lower < ahead))
	{
		if (first[1] == '!')
		{
			if (first[2] != '[' && first[2] != '-')
			{
				// neither CDATA nor comment: assume DTD tag
				unsigned quoting = 0;
				while (lower < ahead && (quoting || *lower != '[' && *lower != '>'))
				{
					switch (*lower)
					{
					case '"':
						if (!(quoting & 1))
							quoting ^= 2;
						break;
					case '\'': 
						if (!(quoting & 2))
							quoting ^= 1;
						break;
					}
					++lower;
				}
				if (*lower == '[')
				{
					upper = lower;
					return true;
				}
			}
			return false;
		}
		while (lower < ahead && *lower != '>')
		{
			++lower;
		}
		if (lower[-1] != '/' && lower[-1] != '?')
		{
			upper = lower;
			return true;
		}
	}
	return false;
}

CMarkdown &CMarkdown::Pop()
{
	if (!Pull())
	{
		upper = ahead;
	}
	return *this;
}

bool CMarkdown::Push()
{
	if (upper < ahead)
	{
		switch MAKEWORD(upper[0], upper[1])
		{
		case MAKEWORD('<', '/'):
		case MAKEWORD(']', '>'):
			upper += 2;
			return true;
		}
	}
	return false;
}

CMarkdown::HSTR CMarkdown::GetTagName()
{
	const char *p = first;
	const char *q = first;
	unsigned char c;
	if (q < ahead && (p = ++q) < ahead)
	{
		if (*q == '!' && (*++q == '-' || *q == '['))
		{
			++q;
		}
		else
		{
			while (q < ahead && !isspace(c = *q) && c != '[' && c != '>' && c != '"' && c != '\'' && c != '=' )
			{
				++q;
			}
		}
	}
	return (HSTR)SysAllocStringByteLen(p, q - p);
}

CMarkdown::HSTR CMarkdown::GetTagText()
{
	const char *p = first, *q = first;
	if (q < ahead && (p = ++q) < ahead && (*q != '!' || ++q < ahead))
	{
		if (*q == '-' || *q == '[')
		{
			++q;
		}
		else
		{
			unsigned quoting = 0;
			while (q < ahead && (quoting || (*q != '[' && *q != '<' && *q != '>' && *q != '/')))
			{
				switch (*q)
				{
				case '"':
					if (!(quoting & 1))
						quoting ^= 2;
					break;
				case '\'': 
					if (!(quoting & 2))
						quoting ^= 1;
					break;
				}
				++q;
			}
		}
	}
	return (HSTR)SysAllocStringByteLen(p, q - p);
}

CMarkdown::HSTR CMarkdown::GetInnerText()
{
	Scan();
	const char *p = first;
	const char *q = upper;
	char bracket = '>';
	if (p < upper && ++p < upper && *p == '!' && ++p < upper)
	{
		bracket = *p;
		if (bracket != '-')
		{
			bracket = '[';
		}
	}
	p = lower;
	unsigned quoting = 0;
	while (p < upper && (quoting || *p != bracket))
	{
		switch (*p)
		{
		case '"':
			if (!(quoting & 1))
				quoting ^= 2;
			break;
		case '\'': 
			if (!(quoting & 2))
				quoting ^= 1;
			break;
		}
		++p;
	}
	if (p < q && p < --q && p < --q)
	{
		++p;
	}
	return (HSTR)SysAllocStringByteLen(p, q - p);
}

CMarkdown::HSTR CMarkdown::GetOuterText()
{
	Scan();
	const char *q = upper;
	if (q > first)
	{
		while (q[-1] != '>' && q <= ahead)
		{
			++q;
		}
	}
	return (HSTR)SysAllocStringByteLen(lower, q - first);
}

class CMarkdown::Token
{
public:
	const char *lower;
	const char *upper;
	int IsSpecial(const char *, const char *);
};

int CMarkdown::Token::IsSpecial(const char *p, const char *ahead)
{
	while (p <= ahead && isspace((unsigned char)*p))
	{
		++p;
	}
	lower = p;
	int special = 1;
	while (p <= ahead && !isspace((unsigned char)*p))
	{
		switch (char c = *p)
		{
		case '"':
		case '\'':
			if (special && p < ahead)
			{
				do
				{
					++p;
				} while (p < ahead && *p != c);
			}
			// fall through
		case '/':
		case '=':
		case '<':
		case '>':
		case '[':
		case ']':
			upper = p + special;
			return special;
		}
		++p;
		special = 0;
	}
	upper = p;
	return special;
}

CMarkdown::HSTR CMarkdown::GetAttribute(const char *key, const void *pv)
{
	typedef HSTR *strName;
	const char *name = 0;
	int cname = 0;
	const char *value = 0;
	int cvalue = 0;
	bool equals = false;
	const char *p = lower;
	Token token;
	do
	{
		if (token.IsSpecial(p, ahead))
		{
			switch (*token.lower)
			{
			case '=':
				equals = true;
				break;
			case '"':
			case '\'':
				equals = false;
				cvalue = token.upper - (value = token.lower); 
				if (cvalue >= 2)
				{
					++value;
					cvalue -= 2;
				}
				break;
			case '[':
			case '>':
				token.upper = token.lower;
				break;
			}
		}
		else if (token.upper != token.lower)
		{
			if (equals)
			{
				equals = false;
				cvalue = token.upper - (value = token.lower);
			}
			else
			{
				cname = token.upper - (name = token.lower);
			}
		}
		p = token.upper;
		if (name && value)
		{
			if (key == 0)
			{
				lower = p;
				*strName(pv) = (HSTR)SysAllocStringByteLen(value, cvalue);
				return (HSTR)SysAllocStringByteLen(name, cname);
			}
			if (memcmp(name, key, cname) == 0 && key[cname] == '\0')
			{
				return (HSTR)SysAllocStringByteLen(value, cvalue);
			}
			name = value = 0;
		}
	} while (token.upper != token.lower);
	if (key == 0)
	{
		lower = p;
		return 0;
	}
	return (HSTR)SysAllocStringByteLen((const char *)pv, lstrlenA((const char *)pv));
}

LPVOID NTAPI CMarkdown::FileImage::MapFile(HANDLE hFile, DWORD dwSize)
{
	DWORD flProtect = PAGE_READONLY;
	DWORD dwDesiredAccess = FILE_MAP_READ;
	if (hFile == INVALID_HANDLE_VALUE)
	{
		flProtect = PAGE_READWRITE;
		dwDesiredAccess = FILE_MAP_READ | FILE_MAP_WRITE;
	}
	LPVOID pMapping = 0;
	HANDLE hMapping = CreateFileMapping(hFile, NULL, flProtect, 0, dwSize, NULL);
	if (hMapping)
	{
		pMapping = MapViewOfFile(hMapping, dwDesiredAccess, 0, 0, dwSize);
		CloseHandle(hMapping);
	}
	return pMapping;
}

CMarkdown::FileImage::FileImage(LPCTSTR path, DWORD trunc, int flags):
pImage(NULL)
{
	HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		cbImage = GetFileSize(hFile, 0);
		if (cbImage != INVALID_FILE_SIZE)
		{
			if (trunc && cbImage > trunc)
			{
				cbImage = trunc;
			}
			pImage = MapFile(hFile, cbImage);
			if (pImage)
			{
				if (flags & Octets && cbImage >= 2)
				{
					LPVOID pCopy;
					switch (*(LPWCH)pImage) case 0xFFFE:
					{
						// big endian: swab first
						cbImage &= ~1UL;
						pCopy = MapFile(INVALID_HANDLE_VALUE, cbImage);
						if (pCopy)
						{
							_swab((char *)pImage, (char *)pCopy, cbImage);
						}
						UnmapViewOfFile(pImage);
						pImage = pCopy;
						if (pImage) case 0xFEFF:
						{
							// little endian
							int cchImage = cbImage / 2 - 1;
							cbImage = WideCharToMultiByte(CP_UTF8, 0, (LPWCH)pImage + 1, cchImage, 0, 0, 0, 0);
							pCopy = MapFile(INVALID_HANDLE_VALUE, cbImage);
							if (pCopy)
							{
								WideCharToMultiByte(CP_UTF8, 0, (LPWCH)pImage + 1, cchImage, (LPCH)pCopy, cbImage, 0, 0);
							}
							UnmapViewOfFile(pImage);
							pImage = pCopy;
						}
					}
				}
			}
		}
		CloseHandle(hFile);
	}
	if (pImage == NULL)
	{
		cbImage = 0;
	}
}

CMarkdown::FileImage::~FileImage()
{
	if (pImage)
	{
		UnmapViewOfFile(pImage);
	}
}
