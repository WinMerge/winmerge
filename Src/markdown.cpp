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
2005/03/20	Jochen Tucht		Add IgnoreCase option for ASCII-7 tag/attr names.
								Add HtmlUTags option to check for (potentially)
								unbalanced HTML tags. Html option is combination
								of the above. Using these options imposes
								performance penalty, so avoid it if you can.
								New flag CMarkdown::FileImage::Handle makes
								CMarkdown::FileImage::FileImage() accept a
								handle rather than a filename.
2005/06/22	Jochen Tucht		New method CMarkdown::_HSTR::Entities().
2005/07/29	Jochen Tucht		ByteOrder detection for 16/32 bit encodings
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

size_t CMarkdown::Converter::Convert(const char *S, size_t s, char *D, size_t d) const
{
	// reset iconv internal state and tell if converter is valid
	if (iconv(0, 0, 0, 0) != -1)
	{
		if (D == NULL)
		{
			while (s)
			{
				char buffer[100];
				char *C = buffer;
				size_t c = sizeof buffer;
				if (iconv(&S, &s, &C, &c) == -1 && c == sizeof buffer)
				{
					// some error other than 'outbuf exhausted': stop here
					break;
				}
				d += sizeof buffer - c;
			}
		}
		else
		{
			iconv(&S, &s, &D, &d); // convert entire string
		}
	}
	else
	{
		d = 0;
	}
	return d;
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
		int a = WideCharToMultiByte(codepage, 0, W, w, 0, 0, 0, 0);
		H = (HSTR)SysAllocStringByteLen(0, a);
		WideCharToMultiByte(codepage, 0, W, w, H->A, a, 0, 0);
		SysFreeString(B);
	}
	return H;
}

CMarkdown::HSTR CMarkdown::_HSTR::Convert(const CMarkdown::Converter &converter)
{
	HSTR H = this;
	size_t s = SysStringByteLen(B);
	if (size_t d = converter.Convert(A, s, 0, 0))
	{
		H = (HSTR)SysAllocStringByteLen(0, d);
		converter.Convert(A, s, H->A, d);
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

CMarkdown::HSTR CMarkdown::_HSTR::Entities()
{
	HSTR H = this;
	BSTR p, q = H->B;
	while (*(p = q))
	{
		OLECHAR *value = 0;
		switch (*p)
		{
		case '&': value = L"&amp;"; break;
		case '"': value = L"&quot;"; break;
		case '\'': value = L"&apos;"; break;
		case '<' : value = L"&lt;"; break;
		case '>' : value = L"&gt;"; break;
		}
		++q;
		if (value)
		{
			int i = p - H->B;
			int j = q - H->B;
			int cchValue = lstrlenW(value);
			if (int cchGrow = cchValue - 1)
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
			memcpy(p, value, cchValue * sizeof(OLECHAR));
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

//This is a hopefully complete list of the 36 (?) (potentially) unbalanced HTML
//tags. It is based on tags.c from Tidy library,
//"http://cvs.sourceforge.net/viewcvs.py/*checkout*/tidy/tidy/src/tags.c?rev=1.55".
//It should include all tags from tag_defs[] array which are flagged either
//CM_EMPTY (no closing tag) or CM_OPT (optional closing tag).

static const char htmlUTags[]
(
	"area\0"
	"base\0"
	"basefont\0"
	"body\0"
	"br\0"
	"col\0"
	"colgroup\0"
	"dd\0"
	"dt\0"
	"frame\0"
	"head\0"
	"hr\0"
	"html\0"
	"img\0"
	"input\0"
	"isindex\0"
	"li\0"
	"link\0"
	"meta\0"
	"optgroup\0"
	"option\0"
	"p\0"
	"param\0"
	"tbody\0"
	"td\0"
	"tfoot\0"
	"th\0"
	"thead\0"
	"tr\0"
	"nextid\0"
	/* proprietary elements */
	"bgsound\0"	//MICROSOFT
	"embed\0"	//NETSCAPE
	"keygen\0"	//NETSCAPE
	"marquee\0"	//MICROSOFT
	"spacer\0"	//NETSCAPE
	"wbr\0"		//PROPRIETARY
);

CMarkdown::CMarkdown(const char *upper, const char *ahead, unsigned flags):
first(0), lower(0), upper(upper), ahead(ahead),
memcmp(flags & IgnoreCase ? ::memicmp : ::memcmp),
utags(flags & HtmlUTags ? htmlUTags : NULL)
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

int CMarkdown::FindTag(const char *tags, const char *markup)
{
	while (int len = lstrlenA(tags))
	{
		unsigned char c;
		if
		(
			ahead - markup > len
		&&	memcmp(markup, tags, len) == 0
		&&	(isspace(c = markup[len]) || c == '[' || c == '>' || c == '"' || c == '\'' || c == '=')
		)
		{
			return len;
		}
		tags += len + 1;
	}
	return 0;
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
				if (upper[-2] == '/' || utags && FindTag(utags, first + 1))
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
	for (;;)
	{
		while (*this && *upper != '<')
		{
			++upper;
		}
		if (utags && MAKEWORD(upper[0], upper[1]) == MAKEWORD('<', '/'))
		{
			if (int utlen = FindTag(utags, upper + 2))
			{
				upper += 2 + utlen;
				continue;
			}
		}
		break;
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
		if (lower[-1] != '/' && lower[-1] != '?' && !(utags && FindTag(utags, first + 1)))
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
			while (q < ahead && !isspace(c = *q) && c != '[' && c != '>' && c != '"' && c != '\'' && c != '=')
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

int CMarkdown::FileImage::GuessByteOrder(DWORD dwBOM)
{
	int nByteOrder = 0;
	if (dwBOM)
	{
		WORD wBOM = LOWORD(dwBOM);
		WORD wBOMhigh = HIWORD(dwBOM);
		nByteOrder = 2;
		if (wBOM == 0 || wBOMhigh == 0)
		{
			wBOM |= wBOMhigh;
			nByteOrder = 4;
		}
		if (wBOM == 0xFEFF || wBOM == 0xFFFE)
		{
			nByteOrder += 8 + ((char *)memchr(&dwBOM, 0xFF, 4) - (char *)&dwBOM);
		}
		else if (LOBYTE(wBOM) == 0 || HIBYTE(wBOM) == 0)
		{
			BYTE cBOM = LOBYTE(wBOM) | HIBYTE(wBOM);
			nByteOrder += ((char *)memchr(&dwBOM, cBOM, 4) - (char *)&dwBOM);
		}
		else if (dwBOM & 0xFFFFFF == 0xBFBBEF)
		{
			nByteOrder = 8 + 1;
		}
		else
		{
			nByteOrder = 1;
		}
	}
	return nByteOrder;
}

CMarkdown::FileImage::FileImage(LPCTSTR path, DWORD trunc, int flags)
: pImage(NULL), nByteOrder(0)
{
	HANDLE hFile
	(
		flags & Handle
	?	HANDLE(path)
	:	CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0)
	);
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
			if (pImage && cbImage >= 4 && (flags & Octets & (nByteOrder = GuessByteOrder(*(LPDWORD)pImage))))
			{
				LPVOID pCopy;
				switch (nByteOrder)
				{
				case 2 + 1:
				case 2 + 1 + 8:
					// big endian: swab first
					cbImage &= ~1UL;
					pCopy = MapFile(INVALID_HANDLE_VALUE, cbImage);
					if (pCopy)
					{
						_swab((char *)pImage, (char *)pCopy, cbImage);
					}
					UnmapViewOfFile(pImage);
					pImage = pCopy;
					if (pImage)
					{
					case 2 + 0:
					case 2 + 0 + 8:
						// little endian
						int cchImage = cbImage / 2;
						LPWCH pchImage = (LPWCH)pImage;
						if (nByteOrder & 8)
						{
							++pchImage;
							--cchImage;
						}
						cbImage = WideCharToMultiByte(CP_UTF8, 0, pchImage, cchImage, 0, 0, 0, 0);
						pCopy = MapFile(INVALID_HANDLE_VALUE, cbImage);
						if (pCopy)
						{
							WideCharToMultiByte(CP_UTF8, 0, pchImage, cchImage, (LPCH)pCopy, cbImage, 0, 0);
						}
						UnmapViewOfFile(pImage);
						pImage = pCopy;
					}
					break;
				case 4 + 1:
				case 4 + 1 + 8:
				case 4 + 2:
				case 4 + 2 + 8:
					// odd word endianness: swab first
					cbImage &= ~3UL;
					pCopy = MapFile(INVALID_HANDLE_VALUE, cbImage);
					if (pCopy)
					{
						_swab((char *)pImage, (char *)pCopy, cbImage);
					}
					UnmapViewOfFile(pImage);
					pImage = pCopy;
					if (pImage)
					{
					case 4 + 0:
					case 4 + 0 + 8:
					case 4 + 3:
					case 4 + 3 + 8:
						int cchImage = cbImage;
						LPCH pchImage = (LPCH)pImage;
						if (nByteOrder & 8)
						{
							pchImage += 4;
							cchImage -= 4;
						}
						CMarkdown::Converter converter("utf-8", nByteOrder & 2 ? "ucs-4be" : "ucs-4le");
						cbImage = converter.Convert(pchImage, cchImage, 0, 0);
						pCopy = MapFile(INVALID_HANDLE_VALUE, cbImage);
						if (pCopy)
						{
							converter.Convert(pchImage, cchImage, (LPCH)pCopy, cbImage);
						}
						UnmapViewOfFile(pImage);
						pImage = pCopy;
					}
					break;
				}
			}
		}
		if (!(flags & Handle))
		{
			CloseHandle(hFile);
		}
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
