/** 
 * @file  codepage_detect.cpp
 *
 * @brief Deducing codepage from file contents, when we can
 *
 */

#include "pch.h"
#include "codepage_detect.h"
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <memory>
#include "unicoder.h"
#include "ExConverter.h"
#include "charsets.h"
#include "FileTextEncoding.h"
#include "paths.h"
#include "markdown.h"

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
	size_t len = strlen(prefix);
	if (len)
		if (_strnicmp(text, prefix, len) == 0)
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
		char *ahead = nullptr;
		unsigned codepage = strtol(encodingName, &ahead, 10);
		int i = 0;
		while (*ahead != '\0' && i < sizeof(f_wincp_prefixes)/sizeof(f_wincp_prefixes[0]))
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
		std::string http_equiv(markdown.GetAttribute("http-equiv"));
		if (!http_equiv.empty() && _stricmp(http_equiv.c_str(), "content-type") == 0)
		{
			std::string content(markdown.GetAttribute("content"));
			if (!content.empty())
			{
				char *pchKey = &content[0];
				while (size_t cchKey = strcspn(pchKey += strspn(pchKey, "; \t\r\n"), ";="))
				{
					char *pchValue = pchKey + cchKey;
					size_t cchValue = strcspn(pchValue += strspn(pchValue, "= \t\r\n"), "; \t\r\n");
					if (cchKey >= 7 && _strnicmp(pchKey, "charset", 7) == 0 && (cchKey == 7 || strchr(" \t\r\n", pchKey[7])))
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
	const char *psrc = src;
	std::unique_ptr<char[]> buf;
	if (len >= 2 && (src[0] == 0 || src[1] == 0))
	{
		buf.reset(new char[len]);
		int i, j;
		for (i = 0, j = 0; i < (int)len; i++)
		{
			if (src[i])
				buf[j++] = src[i];
		}
		len = j;
		psrc = buf.get();
	}
	CMarkdown xml(psrc, psrc + len);
	if (xml.Move("?xml"))
	{
		std::string encoding(xml.GetAttribute("encoding"));
		if (!encoding.empty())
		{
			// Is it an encoding name we can find in charsets module ?
			unsigned encodingId = FindEncodingIdFromNameOrAlias(encoding.c_str());
			if (encodingId)
			{
				return GetEncodingCodePageFromId(encodingId);
			}
		}
	}
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
		size_t n = len < sizeof line - 1 ? len : sizeof line - 1;
		memcpy(line, base, n);
		line[n] = 0;
	} while (len && sscanf_s(line, "#pragma code_page(%5u)", &cp) != 1);
	return cp;
}

/**
 * @brief Try to deduce encoding for this file.
 * @param [in] ext File extension.
 * @param [in] src File contents (as a string).
 * @param [in] len Size of the file contents string.
 * @return Codepage number.
 */
static unsigned GuessEncoding_from_bytes(const String& ext, const char *src, size_t len, int guessEncodingType)
{
	unsigned cp = ucr::getDefaultCodepage();
	if (!ucr::CheckForInvalidUtf8(src, len))
		cp = ucr::CP_UTF_8;
	else if (guessEncodingType & 2)
	{
		IExconverter *pexconv = Exconverter::getInstance();
		if (pexconv != nullptr && src != nullptr)
		{
			int autodetectType = (unsigned)guessEncodingType >> 16;
			cp = pexconv->detectInputCodepage(autodetectType, cp, src, len);
		}
	}
	if (guessEncodingType & 1)
	{
		String lower_ext = strutils::makelower(ext);
		if (lower_ext == _T(".rc"))
		{
			cp = demoGuessEncoding_rc(src, len, cp);
		}
		else if (lower_ext == _T(".htm") || lower_ext == _T(".html"))
		{
			cp = demoGuessEncoding_html(src, len, cp);
		}
		else if (lower_ext == _T(".xml") || lower_ext == _T(".xsl"))
		{
			cp = demoGuessEncoding_xml(src, len, cp);
		}
	}
	return cp;
}

/**
 * @brief Try to deduce encoding for this file.
 * @param [in] filepath Full path to the file.
 * @param [in] bGuessEncoding Try to guess codepage (not just unicode encoding).
 * @return Structure getting the encoding info.
 */
FileTextEncoding GuessCodepageEncoding(const String& filepath, int guessEncodingType, ptrdiff_t mapmaxlen)
{
	FileTextEncoding encoding;
	CMarkdown::FileImage fi(filepath != _T("NUL") ? filepath.c_str() : nullptr, mapmaxlen);
	encoding.SetCodepage(ucr::getDefaultCodepage());
	encoding.m_bom = false;
	switch (fi.nByteOrder)
	{
	case 8 + 2 + 0:
		encoding.SetUnicoding(ucr::UCS2LE);
		encoding.SetCodepage(ucr::CP_UCS2LE);
		encoding.m_bom = true;
		break;
	case 8 + 2 + 1:
		encoding.SetUnicoding(ucr::UCS2BE);
		encoding.SetCodepage(ucr::CP_UCS2BE);
		encoding.m_bom = true;
		break;
	case 8 + 1:
		encoding.SetUnicoding(ucr::UTF8);
		encoding.SetCodepage(ucr::CP_UTF_8);
		encoding.m_bom = true;
		break;
	default:
		encoding.m_bom = false;
		break;
	}
	if (fi.nByteOrder < 4 && guessEncodingType != 0)
	{
		String ext = paths::FindExtension(filepath);
		const char *src = (char *)fi.pImage;
		size_t len = fi.cbImage;
		if (len == static_cast<size_t>(mapmaxlen))
		{
			for (size_t i = len; i--; )
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
			encoding.SetCodepage(cp);
		else
			encoding.SetCodepage(ucr::getDefaultCodepage());
	}
	return encoding;
}
