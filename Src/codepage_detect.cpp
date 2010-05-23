/** 
 * @file  codepage_detect.cpp
 *
 * @brief Deducing codepage from file contents, when we can
 *
 */
// ID line follows -- this is updated by SVN
// $Id: codepage_detect.cpp 7172 2010-05-19 12:57:18Z jtuc $

#include "StdAfx.h"
#include <shlwapi.h>
#include "codepage_detect.h"
#include "unicoder.h"
#include "codepage.h"
#include "charsets.h"
#include "markdown.h"
#include "FileTextEncoding.h"
#include "Utf8FileDetect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** @brief Buffer size used in this file. */
static const int BufSize = 65536;

/**
 * @brief Prefixes to handle when searching for codepage names
 * NB: prefixes ending in '-' must go first!
 */
static const char *f_wincp_prefixes[] =
{
	"WINDOWS-", "WINDOWS", "CP-", "CP", "MSDOS-", "MSDOS"
};

/**
 * @brief Remove prefix from the text.
 * @param [in] text Text to process.
 * @param [in] prefix Prefix to remove.
 * @return Text without the prefix.
 */
static const char *EatPrefix(const char *text, const char *prefix)
{
	int len = strlen(prefix);
	if (len)
		if (_memicmp(text, prefix, len) == 0)
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
		while (*ahead != '\0' && i < countof(f_wincp_prefixes))
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
static unsigned demoGuessEncoding_html(const char *src, size_t len, int defcodepage)
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
					if (cchKey >= 7 && _memicmp(pchKey, "charset", 7) == 0 && (cchKey == 7 || strchr(" \t\r\n", pchKey[7])))
					{
						pchValue[cchValue] = '\0';
						// Is it an encoding name known to charsets module ?
						unsigned encodingId = FindEncodingIdFromNameOrAlias(pchValue);
						if (encodingId)
						{
							return GetEncodingCodePageFromId(encodingId);
						}
						return defcodepage;
					}
					pchKey = pchValue + cchValue;
				}
			}
		}
	}
	return defcodepage;
}

/**
 * @brief Parser for XML files to find encoding information
 */
static unsigned demoGuessEncoding_xml(const char *src, size_t len, int defcodepage)
{
	char *psrc;
	if (len >= 2 && (src[0] == 0 || src[1] == 0))
	{
		psrc = new char[len];
		int i, j;
		for (i = 0, j = 0; i < (int)len; i++)
		{
			if (src[i])
				psrc[j++] = src[i];
		}
		len = j;
	}
	else
	{
		psrc = (char *)src;
	}
	CMarkdown xml(psrc, psrc + len);
	if (xml.Move("?xml"))
	{
		CMarkdown::String encoding = xml.GetAttribute("encoding");
		if (encoding.A)
		{
			// Is it an encoding name we can find in charsets module ?
			unsigned encodingId = FindEncodingIdFromNameOrAlias(encoding.A);
			if (encodingId)
			{
				if (psrc != src) delete psrc;
				return GetEncodingCodePageFromId(encodingId);
			}
		}
	}
	if (psrc != src) delete psrc;
	return defcodepage;
}

/**
 * @brief Parser for rc files to find encoding information
 * @note sscanf() requires first argument to be zero-terminated so we must
 * copy lines to temporary buffer.
 */
static unsigned demoGuessEncoding_rc(const char *src, size_t len, int defcodepage)
{
	// NB: Diffutils may replace line endings by '\0'
	unsigned cp = defcodepage;
	char line[80];
	do
	{
		while (len && (*src == '\r' || *src == '\n' || *src == '\0'))
		{
			++src;
			--len;
		}
		const char *base = src;
		while (len && *src != '\r' && *src != '\n' && *src != '\0')
		{
			++src;
			--len;
		}
		lstrcpynA(line, base, len < sizeof line ? len + 1 : sizeof line);
	} while (len && sscanf(line, "#pragma code_page(%d)", &cp) != 1);
	return cp;
}

/**
 * @brief Try to deduce encoding for this file.
 * @param [in] ext File extension.
 * @param [in] src File contents (as a string).
 * @param [in] len Size of the file contents string.
 * @return Codepage number.
 */
static unsigned GuessEncoding_from_bytes(LPCTSTR ext, const char *src, size_t len, int guessEncodingType)
{
	unsigned cp = getDefaultCodepage();
	if (guessEncodingType & 2)
	{
		ucr::IExconverterPtr pexconv(ucr::createConverterMLang());
		if (pexconv && src != NULL)
		{
			int autodetectType = (unsigned int)guessEncodingType >> 16;
			cp = pexconv->detectInputCodepage(autodetectType, cp, src, len);
		}
	}
	else
	{
		if (!CheckForInvalidUtf8((LPBYTE)src, len))
			cp = CP_UTF8;
	}
	if (guessEncodingType & 1)
	{
		if (len > BufSize)
			len = BufSize;
		if (lstrcmpi(ext, _T(".rc")) ==  0)
		{
			cp = demoGuessEncoding_rc(src, len, cp);
		}
		else if (lstrcmpi(ext, _T(".htm")) == 0 || lstrcmpi(ext, _T(".html")) == 0)
		{
			cp = demoGuessEncoding_html(src, len, cp);
		}
		else if (lstrcmpi(ext, _T(".xml")) == 0 || lstrcmpi(ext, _T(".xsl")) == 0)
		{
			cp = demoGuessEncoding_xml(src, len, cp);
		}
	}
	return cp;
}

/**
 * @brief Try to deduce encoding for this file.
 * @param [in] filepath Full path to the file.
 * @param [in,out] encoding Structure getting the encoding info.
 * @param [in] bGuessEncoding Try to guess codepage (not just unicode encoding).
 */
void GuessCodepageEncoding(LPCTSTR filepath, FileTextEncoding * encoding, int guessEncodingType)
{
	const int mapmaxlen = BufSize;
	CMarkdown::FileImage fi(filepath, mapmaxlen);
	encoding->SetCodepage(getDefaultCodepage());
	encoding->m_bom = false;
	encoding->m_guessed = false;
	encoding->m_binary = false;
	switch (fi.nByteOrder)
	{
	case 8 + 2 + 0:
		encoding->SetUnicoding(ucr::UCS2LE);
		encoding->SetCodepage(1200);
		encoding->m_bom = true;
		break;
	case 8 + 2 + 1:
		encoding->SetUnicoding(ucr::UCS2BE);
		encoding->SetCodepage(1201);
		encoding->m_bom = true;
		break;
	case 8 + 1:
		encoding->SetUnicoding(ucr::UTF8);
		encoding->SetCodepage(CP_UTF8);
		encoding->m_bom = true;
		break;
	default:
		encoding->m_bom = false;
		if (memchr(fi.pImage, 0, fi.cbImage))
			encoding->m_binary = true;
		break;
	}
	if (fi.nByteOrder < 4 && guessEncodingType != 0)
	{
		LPCTSTR ext = PathFindExtension(filepath);
		const char *src = (char *)fi.pImage;
		size_t len = fi.cbImage;
		if (len == mapmaxlen)
		{
			int i;
			for (i = len - 1; i >= 0; i--)
			{
				if (isspace((unsigned char)src[i]))
				{
					// make len an even number for ucs-2 detection
					if ((i % 2) == 0)
						len = i;
					else
						len = i + 1;
					break;
				}
			}
		}
		if (unsigned cp = GuessEncoding_from_bytes(ext, src, len, guessEncodingType))
		{
			encoding->SetCodepage(cp);
			encoding->m_guessed = true;
		}
		else
			encoding->SetCodepage(getDefaultCodepage());
	}
}
