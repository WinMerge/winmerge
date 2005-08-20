/** 
 * @file  codepage_detect.cpp
 *
 * @brief Deducing codepage from file contents, when we can
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "StdAfx.h"
#include <shlwapi.h>
#include "codepage_detect.h"
#include "unicoder.h"
#include "codepage.h"
#include "charsets.h"
#include "markdown.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 * @brief Is specified codepage number valid on this system?
 */
static unsigned ValidCodepage(unsigned cp)
{
	return cp && isCodepageSupported(cp) ? cp : 0;
}

/**
 * @brief Parser for HTML files to find encoding information
 */
static unsigned demoGuessEncoding_html(const char *src, size_t len)
{
	CMarkdown markdown(src, src + len, CMarkdown::Html);
	//As <html> and <head> are optional, there is nothing to pull...
	//markdown.Move("html").Pop().Move("head").Pop();
	while (markdown.Move("meta"))
	{
		CMarkdown::String http_equiv = markdown.GetAttribute("http-equiv");
		if (http_equiv.A && lstrcmpiA(http_equiv.A, "content-type") == 0)
		{
			CMarkdown::String content = markdown.GetAttribute("content");
			if (char *pchKey = content.A)
			{
				while (int cchKey = strcspn(pchKey += strspn(pchKey, "; \t\r\n"), ";="))
				{
					char *pchValue = pchKey + cchKey;
					int cchValue = strcspn(pchValue += strspn(pchValue, "= \t\r\n"), "; \t\r\n");
					if (cchKey >= 7 && memicmp(pchKey, "charset", 7) == 0 && (cchKey == 7 || strchr(" \t\r\n", pchKey[7])))
					{
						pchValue[cchValue] = '\0';
						// Is it an encoding name known to charsets module ?
						unsigned encodingId = GetEncodingIdFromName(pchValue);
						if (encodingId == 0)
						{
							if (unsigned codepage = atoi(pchValue))
							{
								encodingId = GetEncodingIdFromCodePage(codepage);
							}
						}
						if (encodingId)
						{
							return GetEncodingCodePageFromId(encodingId);
						}
						return 0;
					}
					pchKey = pchValue + cchValue;
				}
			}
		}
	}
	return 0;
}

/**
 * @brief Parser for XML files to find encoding information
 */
static unsigned demoGuessEncoding_xml(const char *src, size_t len)
{
	CMarkdown xml(src, src + len);
	if (xml.Move("?xml"))
	{
		CMarkdown::String encoding = xml.GetAttribute("encoding");
		if (encoding.A)
		{
			// Is it an encoding name known to charsets module ?
			unsigned encodingId = GetEncodingIdFromName(encoding.A);
			if (encodingId == 0)
			{
				if (unsigned codepage = atoi(encoding.A))
				{
					encodingId = GetEncodingIdFromCodePage(codepage);
				}
			}
			if (encodingId)
			{
				return GetEncodingCodePageFromId(encodingId);
			}
		}
	}
	return 0;
}

/**
 * @brief Parser for rc files to find encoding information
 */
static unsigned demoGuessEncoding_rc(const char *src, size_t len)
{
	unsigned cp = 0;
	const char *line = 0;
	do
	{
		while (len && (*src == '\r' || *src == '\n'))
		{
			++src;
			--len;
		}
		line = src;
		while (len && *src != '\r' && *src != '\n')
		{
			++src;
			--len;
		}
	} while (len && sscanf(line, "#pragma code_page(%d)", &cp) != 1);
	return ValidCodepage(cp);
}

/**
 * @brief Try to deduce encoding for this file
 */
unsigned GuessEncoding_from_bytes(LPCTSTR ext, const char *src, size_t len)
{
	if (len > 4096)
		len = 4096;
	unsigned cp = 0;
	if (lstrcmpi(ext, _T(".rc")) ==  0)
	{
		cp = demoGuessEncoding_rc(src, len);
	}
	else if (lstrcmpi(ext, _T(".htm")) == 0 || lstrcmpi(ext, _T(".html")) == 0)
	{
		cp = demoGuessEncoding_html(src, len);
	}
	else if (lstrcmpi(ext, _T(".xml")) == 0 || lstrcmpi(ext, _T(".xsl")) == 0)
	{
		cp = demoGuessEncoding_xml(src, len);
	}
	return cp;
}

/**
 * @brief Try to deduce encoding for this file
 */
bool GuessEncoding_from_bytes(LPCTSTR ext, const char **data, int count, int *codepage)
{
	if (data)
	{
		const char *src = data[0];
		size_t len = data[count] - src;
		if (unsigned cp = GuessEncoding_from_bytes(ext, src, len))
		{
			*codepage = cp;
			return true;
		}
	}
	return false;
}

/**
 * @brief Try to deduce encoding for this file
 */
void GuessCodepageEncoding(LPCTSTR filepath, int *unicoding, int *codepage, BOOL bGuessEncoding)
{
	CMarkdown::FileImage fi(filepath, 4096);
	*unicoding = ucr::NONE;
	switch (fi.nByteOrder)
	{
	case 8 + 2 + 0:
		*unicoding = ucr::UCS2LE;
		break;
	case 8 + 2 + 1:
		*unicoding = ucr::UCS2BE;
		break;
	case 8:
		*unicoding = ucr::UTF8;
		break;
	}
	if (fi.nByteOrder == 0 && bGuessEncoding)
	{
		LPCTSTR ext = PathFindExtension(filepath);
		if (unsigned cp = GuessEncoding_from_bytes(ext, (char *)fi.pImage, fi.cbImage))
		{
			*codepage = cp;
		}
	}
}
