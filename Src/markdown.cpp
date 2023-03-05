/* markdown.cpp: Pull-parse XML sources
 * Copyright (c) 2005 Jochen Tucht
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * OS/Libs:	Win32/STL/shlwapi/iconv
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

Please mind 2. b) of the GNU LGPL terms, and log your changes below.

DATE:		BY:					DESCRIPTION:
==========	==================	================================================
2005-01-15	Jochen Tucht		Created
2005-02-26	Jochen Tucht		Load iconv.dll through DLLPSTUB
2005-03-20	Jochen Tucht		Add IgnoreCase option for ASCII-7 tag/attr names.
								Add HtmlUTags option to check for (potentially)
								unbalanced HTML tags. Html option is combination
								of the above. Using these options imposes
								performance penalty, so avoid it if you can.
								New flag CMarkdown::FileImage::Handle makes
								CMarkdown::FileImage::FileImage() accept a
								handle rather than a filename.
2005-06-22	Jochen Tucht		New method CMarkdown::_HSTR::Entities().
2005-07-29	Jochen Tucht		ByteOrder detection for 16/32 bit encodings
2005-09-09	Jochen Tucht		Patch by Takashi Sawanaka fixes crash due to
								reading beyond end of text with HtmlUTags option
2005-12-04	Jochen Tucht		Fix UTF-8 signature detection
								Strip bogus trailing slash in name of empty tag
2008-08-27	Jochen Neubeck		Replace MFC CMap by STL std::map
*/

#include "pch.h"
#include "markdown.h"
#include <cstring>
#include <cstdint>
#include <Poco/ByteOrder.h>
#include <Poco/NumberParser.h>
#include <Poco/SharedMemory.h>
#include "unicoder.h"
#include "TFile.h"

#ifndef MAKEWORD
#define MAKEWORD(a, b)      ((unsigned short)(((unsigned char)((unsigned)(a) & 0xff)) | ((unsigned short)((unsigned char)((unsigned)(b) & 0xff))) << 8))
#define MAKELONG(a, b)      ((unsigned)(((unsigned short)((unsigned)(a) & 0xffff)) | ((unsigned)((unsigned short)((unsigned)(b) & 0xffff))) << 16))
#define LOWORD(l)           ((unsigned short)((unsigned)(l) & 0xffff))
#define HIWORD(l)           ((unsigned short)((unsigned)(l) >> 16))
#define LOBYTE(w)           ((unsigned char)((unsigned)(w) & 0xff))
#define HIBYTE(w)           ((unsigned char)((unsigned)(w) >> 8))
#endif

using Poco::ByteOrder;
using Poco::NumberParser;
using Poco::SharedMemory;
using Poco::File;

void CMarkdown::Load(EntityMap &entityMap)
{
	entityMap["amp"] = "&";
	entityMap["quot"] = "\"";
	entityMap["apos"] = "'";
	entityMap["lt"] = "<";
	entityMap["gt"] = ">";
}

void CMarkdown::Load(EntityMap &entityMap, int dummy)
{
	while (Move("!ENTITY"))
	{
		std::string hstrValue;
		std::string hstrKey = GetAttribute(0, &hstrValue);
		if (!hstrKey.empty())
		{
			entityMap[hstrKey] = hstrValue;
		}
	}
}

std::string CMarkdown::Resolve(const EntityMap &map, const std::string& v)
{
	std::string ret(v);
	char *p, *q = &ret[0];
	while ((p = strchr(q, '&')) != nullptr && (q = strchr(p, ';')) != nullptr)
	{
		*q = '\0';
		char *key = p + 1;
		std::string value;
		if (*key == '#')
		{
			unsigned ordinal = '?';
			*key = '0';
			if (NumberParser::tryParseHex(key, ordinal))
				value.assign(1, static_cast<std::string::value_type>(ordinal));
			*key = '#';
		}
		else
		{
			EntityMap::const_iterator p1 = map.find(key);
			if (p1 != map.end())
				value = p1->second;
		}
		*q = ';';
		++q;
		size_t cchValue = value.length();
		if (cchValue != 0)
		{
			size_t i = p - &ret[0];
			size_t j = q - &ret[0];
			size_t cchKey = q - p;
			if (cchValue != cchKey)
			{
				size_t b = ret.length();
				size_t cbMove = (b - j) * sizeof(char);
				if (cchKey > cchValue)
				{
					size_t cchGrow = cchKey - cchValue;
					memmove(q - cchGrow, q, cbMove);
					ret.resize(b - cchGrow);
				}
				p = &ret[0] + i;
				q = &ret[0] + j;
				if (cchValue > cchKey)
				{
					size_t cchGrow = cchValue - cchKey;
					ret.resize(b + cchGrow);
					memmove(q + cchGrow, q, cbMove);
				}
			}
			memcpy(p, value.c_str(), cchValue * sizeof(char));
			q = p + cchValue;
		}
	}
	return ret;
}

std::string CMarkdown::Entities(const std::string& v)
{
	std::string ret(v);
	char *p, *q = &ret[0];
	while (*(p = q))
	{
		char *value = nullptr;
		switch (*p)
		{
		case '&': value = "&amp;"; break;
		case '"': value = "&quot;"; break;
		case '\'': value = "&apos;"; break;
		case '<' : value = "&lt;"; break;
		case '>' : value = "&gt;"; break;
		}
		++q;
		if (value != nullptr)
		{
			size_t cchValue = strlen(value);
			if (cchValue > 1)
			{
				ptrdiff_t i = p - &ret[0];
				ptrdiff_t j = q - &ret[0];
				size_t b = ret.length();
				ret.resize(b + cchValue - 1);
				p = &ret[0] + i;
				q = &ret[0] + j;
				memmove(q + cchValue - 1, q, (b - j) * sizeof(char));
			}
			memcpy(p, value, cchValue * sizeof(char));
			q = p + cchValue;
		}
	}
	return ret;
}

//This is a hopefully complete list of the 36 (?) (potentially) unbalanced HTML
//tags. It is based on tags.c from Tidy library,
//"http://cvs.sourceforge.net/viewcvs.py/*checkout*/tidy/tidy/src/tags.c?rev=1.55".
//It should include all tags from tag_defs[] array which are flagged either
//CM_EMPTY (no closing tag) or CM_OPT (optional closing tag).

static const char htmlUTags[] = 
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
first(nullptr), lower(nullptr), upper(upper), ahead(ahead),
memcmp(flags & IgnoreCase ? ::_memicmp : ::memcmp),
utags(flags & HtmlUTags ? htmlUTags : nullptr)
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

size_t CMarkdown::FindTag(const char *tags, const char *markup) const
{
	while (ptrdiff_t len = strlen(tags))
	{
		unsigned char c;
		if
		(
			(ahead - markup) > len
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
		if (utags != nullptr && upper < ahead && *upper == '<')
		{
			size_t utlen = FindTag(utags, upper + 2);
			if (utlen != 0)
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
		size_t length = q - p;
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

std::string CMarkdown::GetTagName() const
{
	const char *p = first;
	const char *q = first;
	if (q < ahead && (p = ++q) < ahead)
	{
		if (*q == '!' && (*++q == '-' || *q == '['))
		{
			++q;
		}
		else
		{
			unsigned char c;
			while (q < ahead && !isspace(c = *q) && c != '[' && c != '>' && c != '"' && c != '\'' && c != '=' && c != '/')
			{
				++q;
			}
		}
	}
	return std::string(p, q - p);
}

std::string CMarkdown::GetTagText() const
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
	return std::string(p, q - p);
}

std::string CMarkdown::GetInnerText()
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
	return std::string(p, q - p);
}

std::string CMarkdown::GetOuterText()
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
	return std::string(lower, q - first);
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
			[[fallthrough]];
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

std::string CMarkdown::GetAttribute(const char *key, std::string *pv)
{
	const char *name = 0;
	size_t cname = 0;
	const char *value = 0;
	size_t cvalue = 0;
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
			if (key == nullptr)
			{
				lower = p;
				*pv = std::string(value, cvalue);
				return std::string(name, cname);
			}
			if (memcmp(name, key, cname) == 0 && key[cname] == '\0')
			{
				return std::string(value, cvalue);
			}
			name = value = 0;
		}
	} while (token.upper != token.lower);
	if (key == nullptr)
	{
		lower = p;
		return "";
	}
	return pv ? *pv : "";
}

int CMarkdown::FileImage::GuessByteOrder(unsigned dwBOM)
{
	int nByteOrder = 0;
	if (dwBOM)
	{
		unsigned short wBOM = LOWORD(dwBOM);
		unsigned short wBOMhigh = HIWORD(dwBOM);
		nByteOrder = 2;
		if (wBOM == 0 || wBOMhigh == 0)
		{
			wBOM |= wBOMhigh;
			nByteOrder = 4;
		}
		if (wBOM == 0xFEFF || wBOM == 0xFFFE)
		{
			nByteOrder += 8 + static_cast<int>((char *)memchr(&dwBOM, 0xFF, 4) - (char *)&dwBOM);
		}
		else if (LOBYTE(wBOM) == 0 || HIBYTE(wBOM) == 0)
		{
			unsigned char cBOM = LOBYTE(wBOM) | HIBYTE(wBOM);
			nByteOrder += static_cast<int>((char *)memchr(&dwBOM, cBOM, 4) - (char *)&dwBOM);
		}
		else if ((dwBOM & 0xFFFFFF) == 0xBFBBEF)
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

CMarkdown::FileImage::FileImage(const tchar_t *path, size_t trunc, unsigned flags)
: pImage(nullptr), cbImage(0), nByteOrder(0), m_pSharedMemory(nullptr), pCopy(nullptr)
{
	if (flags & Mapping)
	{
		pImage = (void *)(path);
		cbImage = trunc;
	}
	else if (path != nullptr)
	{
		try
		{
			TFile file(path);
			m_pSharedMemory = new SharedMemory(file, SharedMemory::AM_READ);
			pImage = m_pSharedMemory->begin();
			cbImage = m_pSharedMemory->end() - static_cast<char *>(pImage);
		}
		catch (...)
		{
		}
	}
	if (pImage == nullptr)
	{
		cbImage = 0;
	}
	else if (cbImage >= 4 && (flags & Octets & (nByteOrder = GuessByteOrder(*(unsigned *)pImage))))
	{
		switch (nByteOrder)
		{
		case 2 + 1:
		case 2 + 1 + 8:
			// big endian: swab first
			cbImage &= ~1UL;
			pCopy = new(std::nothrow) unsigned char[cbImage];
			if (pCopy != nullptr)
			{
				for (size_t i = 0; i < cbImage / 2; ++i)
					*((uint16_t *)pCopy + i) = Poco::ByteOrder::flipBytes(*((uint16_t *)pImage + i));
			}

			delete m_pSharedMemory;
			m_pSharedMemory = nullptr;
			pImage = pCopy;
			if (pImage != nullptr)
			{
				[[fallthrough]];
			case 2 + 0:
			case 2 + 0 + 8:
				// little endian
				size_t cchImage = cbImage / 2;
				uint16_t *pchImage = (uint16_t *)pImage;
				if (nByteOrder & 8)
				{
					++pchImage;
					--cchImage;
				}
				cbImage = ucr::Utf8len_of_string(pchImage, cchImage);
				pCopy = new(std::nothrow) unsigned char[cbImage];
				if (pCopy != nullptr)
				{
					uint16_t *pu16;
					unsigned char *pu8;
					for (pu16 = (uint16_t *)pchImage, pu8 = (unsigned char *)pCopy; pu16 < pchImage + cchImage; ++pu16)
						pu8 += ucr::Ucs4_to_Utf8(*pu16, pu8);
				}
				delete m_pSharedMemory;
				m_pSharedMemory = nullptr;
				pImage = pCopy;
			}
			break;
		case 4 + 1:
		case 4 + 1 + 8:
		case 4 + 2:
		case 4 + 2 + 8:
			// odd word endianness: swab first
			cbImage &= ~3UL;
			pCopy = new(std::nothrow) unsigned char[cbImage];
			if (pCopy != nullptr)
			{
				for (size_t i = 0; i < cbImage / 2; ++i)
					*((uint16_t *)pCopy + i) = Poco::ByteOrder::flipBytes(*((uint16_t *)pImage + i));
			}
			delete m_pSharedMemory;
			m_pSharedMemory = nullptr;
			pImage = pCopy;
			if (pImage != nullptr)
			{
				[[fallthrough]];
			case 4 + 0:
			case 4 + 0 + 8:
			case 4 + 3:
			case 4 + 3 + 8:
				size_t cchImage = cbImage;
				char *pchImage = (char *)pImage;
				if (nByteOrder & 8)
				{
					pchImage += 4;
					cchImage -= 4;
				}
				unsigned uch;
				cbImage = 0;
				for (size_t i = 0; i < cchImage; i += 4)
				{
					memcpy(&uch, pchImage + i, 4);
					if (nByteOrder & 2)
						uch = ByteOrder::fromBigEndian(uch);
					else
						uch = ByteOrder::fromLittleEndian(uch);
					cbImage += ucr::Utf8len_fromCodepoint(uch);
				}
				void *pCopy2 = new(std::nothrow) unsigned char[cbImage];
				if (pCopy2 != nullptr)
				{
					cbImage = 0;
					for (size_t i = 0; i < cchImage; i += 4)
					{
						memcpy(&uch, pchImage + i, 4);
						if (nByteOrder & 2)
							uch = ByteOrder::fromBigEndian(uch);
						else
							uch = ByteOrder::fromLittleEndian(uch);
						cbImage += ucr::Ucs4_to_Utf8(uch, (unsigned char *)pCopy2 + cbImage);
					}
				}
				delete m_pSharedMemory;
				m_pSharedMemory = nullptr;
				pImage = pCopy2;
				delete [] pCopy;
				pCopy = pCopy2;
			}
			break;
		}
	}
}

CMarkdown::FileImage::~FileImage()
{
	delete m_pSharedMemory;
	delete [] pCopy;
}
