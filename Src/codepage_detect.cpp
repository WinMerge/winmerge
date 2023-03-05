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
		if (strncasecmp(text, prefix, len) == 0)
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
		std::string charset(markdown.GetAttribute("charset"));
		if (charset.empty())
		{
			std::string http_equiv(markdown.GetAttribute("http-equiv"));
			if (!http_equiv.empty() && strcasecmp(http_equiv.c_str(), "content-type") == 0)
			{
				std::string content(markdown.GetAttribute("content"));
				if (!content.empty())
				{
					char *pchKey = &content[0];
					while (size_t cchKey = strcspn(pchKey += strspn(pchKey, "; \t\r\n"), ";="))
					{
						char *pchValue = pchKey + cchKey;
						size_t cchValue = strcspn(pchValue += strspn(pchValue, "= \t\r\n"), "; \t\r\n");
						if (cchKey >= 7 && strncasecmp(pchKey, "charset", 7) == 0 && (cchKey == 7 || strchr(" \t\r\n", pchKey[7])))
						{
							pchValue[cchValue] = '\0';
							charset = pchValue;
							break;
						}
						pchKey = pchValue + cchValue;
					}
				}
			}
		}
		if (!charset.empty())
		{
			// Is it an encoding name known to charsets module ?
			int encodingId = FindEncodingIdFromNameOrAlias(charset.c_str());
			if (encodingId)
				return GetEncodingCodePageFromId(encodingId);
			return defcodepage;
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

namespace codepage_detect
{
/**
 * @brief Try to deduce encoding for this file.
 * @param [in] ext File extension.
 * @param [in] src File contents (as a string).
 * @param [in] len Size of the file contents string.
 * @return Codepage number.
 */
FileTextEncoding Guess(const String& ext, const void * src, size_t len, int guessEncodingType)
{
	FileTextEncoding encoding;
	encoding.SetUnicoding(ucr::DetermineEncoding(reinterpret_cast<const unsigned char *>(src), len, &encoding.m_bom));
	if (encoding.m_unicoding != ucr::NONE)
		return encoding;
	unsigned cp = ucr::getDefaultCodepage();
	if (guessEncodingType != 0)
	{
		if (!ucr::CheckForInvalidUtf8(reinterpret_cast<const char*>(src), len))
			cp = ucr::CP_UTF_8;
		else if (guessEncodingType & 2)
		{
			IExconverter* pexconv = Exconverter::getInstance();
			if (pexconv != nullptr && src != nullptr)
			{
				int autodetectType = (unsigned)guessEncodingType >> 16;
				cp = pexconv->detectInputCodepage(autodetectType, cp, reinterpret_cast<const char *>(src), len);
			}
		}
		if (guessEncodingType & 1)
		{
			String lower_ext = strutils::makelower(ext);
			if (lower_ext == _T(".rc"))
			{
				cp = demoGuessEncoding_rc(reinterpret_cast<const char *>(src), len, cp);
			}
			else if (lower_ext == _T(".htm") || lower_ext == _T(".html"))
			{
				cp = demoGuessEncoding_html(reinterpret_cast<const char *>(src), len, cp);
			}
			else if (lower_ext == _T(".xml") || lower_ext == _T(".xsl"))
			{
				cp = demoGuessEncoding_xml(reinterpret_cast<const char *>(src), len, cp);
			}
		}
	}
	encoding.SetCodepage(cp);
	return encoding;
}

/**
 * @brief Try to deduce encoding for this file.
 * @param [in] filepath Full path to the file.
 * @param [in] bGuessEncoding Try to guess codepage (not just unicode encoding).
 * @return Structure getting the encoding info.
 */
FileTextEncoding Guess(const String& filepath, int guessEncodingType, ptrdiff_t mapmaxlen)
{
	CMarkdown::FileImage fi(filepath != _T("NUL") ? filepath.c_str() : nullptr, mapmaxlen);
	String ext = paths::FindExtension(filepath);
	return Guess(ext, fi.pImage, fi.cbImage, guessEncodingType);
}

}
