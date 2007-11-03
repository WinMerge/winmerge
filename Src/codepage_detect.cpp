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
#include "FileTextEncoding.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 * @brief Prefixes to handle when searching for codepage names
 * NB: prefixes ending in '-' must go first!
 */
static const char *f_wincp_prefixes[] =
{
	"WINDOWS-", "WINDOWS", "CP-", "CP", "MSDOS-", "MSDOS"
};

/**
 * @brief Eat prefix and return pointer to remaining text
 */
static const char *EatPrefix(const char *text, const char *prefix)
{
	if (int len = strlen(prefix))
		if (memicmp(text, prefix, len) == 0)
			return text + len;
	return 0;
}

/**
 * @brief Try to to match codepage name from codepages module, & watch for f_wincp_prefixes aliases
 */
static int
FindEncodingIdFromNameOrAlias(const char *encodingName)
{
	// Try name as given
	unsigned encodingId = GetEncodingIdFromName(encodingName);
	if (encodingId == 0)
	{
		// Handle purely numeric values (codepages)
		char *ahead = 0;
		unsigned codepage = strtol(encodingName, &ahead, 10);
		int i = 0;
		while (*ahead != '\0' && i < RTL_NUMBER_OF(f_wincp_prefixes))
		{
			if (const char *remainder = EatPrefix(encodingName, f_wincp_prefixes[i]))
			{
				codepage = strtol(remainder, &ahead, 10);
			}
			++i;
		}
		if (*ahead == '\0')
		{
			encodingId = GetEncodingIdFromCodePage(codepage);
		}
	}
	return encodingId;
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
						unsigned encodingId = FindEncodingIdFromNameOrAlias(pchValue);
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
			// Is it an encoding name we can find in charsets module ?
			unsigned encodingId = FindEncodingIdFromNameOrAlias(encoding.A);
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
 * @note sscanf() requires first argument to be zero-terminated so we must
 * copy lines to temporary buffer.
 */
static unsigned demoGuessEncoding_rc(const char *src, size_t len)
{
	unsigned cp = 0;
	char line[80];
	do
	{
		while (len && (*src == '\r' || *src == '\n'))
		{
			++src;
			--len;
		}
		const char *base = src;
		while (len && *src != '\r' && *src != '\n')
		{
			++src;
			--len;
		}
		lstrcpynA(line, base, sizeof line);
	} while (len && sscanf(line, "#pragma code_page(%d)", &cp) != 1);
	return cp;
}

/**
 * @brief Try to deduce encoding for this file
 */
static unsigned GuessEncoding_from_bytes(LPCTSTR ext, const char *src, size_t len)
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
bool GuessEncoding_from_bytes(LPCTSTR ext, const char **data, int count, FileTextEncoding * encoding)
{
	if (data)
	{
		const char *src = data[0];
		size_t len = data[count] - src;
		if (unsigned cp = GuessEncoding_from_bytes(ext, src, len))
		{
			encoding->Clear();
			encoding->SetCodepage(cp);
			return true;
		}
	}
	return false;
}

/**
 * @brief Try to deduce encoding for this file
 */
void GuessCodepageEncoding(LPCTSTR filepath, FileTextEncoding * encoding, BOOL bGuessEncoding)
{
	CMarkdown::FileImage fi(filepath, 4096);
	encoding->SetCodepage(getDefaultCodepage());
	encoding->m_bom = false;
	encoding->m_guessed = false;
	switch (fi.nByteOrder)
	{
	case 8 + 2 + 0:
		encoding->SetUnicoding(ucr::UCS2LE);
		encoding->m_bom = true;
		break;
	case 8 + 2 + 1:
		encoding->SetUnicoding(ucr::UCS2BE);
		encoding->m_bom = true;
		break;
	case 8 + 1:
		encoding->SetUnicoding(ucr::UTF8);
		encoding->m_bom = true;
		break;
	default:
		encoding->m_bom = false;
		break;

	}
	if (fi.nByteOrder == 1 && bGuessEncoding)
	{
		LPCTSTR ext = PathFindExtension(filepath);
		const char *src = (char *)fi.pImage;
		size_t len = fi.cbImage;
		if (unsigned cp = GuessEncoding_from_bytes(ext, src, len))
		{
			encoding->SetCodepage(cp);
			encoding->m_guessed = true;
		}
	}
}
