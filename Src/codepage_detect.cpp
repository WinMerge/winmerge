/** 
 * @file  codepage_detect.cpp
 *
 * @brief Deducing codepage from file contents, when we can
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "StdAfx.h"
#include "codepage_detect.h"
#include "UniFile.h"
#include "unicoder.h"
#include "codepage.h"
#include "charsets.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static bool GuessEncoding_html_from_unifile(UniFile * pufile, int * encoding, int * codepage);
static bool GuessEncoding_xml_from_unifile(UniFile * pufile, int * encoding, int * codepage);
static bool GuessEncoding_rc_from_unifile(UniFile * pufile, int * encoding, int * codepage);
static bool codepage_from_html_line(LPCSTR line, int * codepage);
static bool codepage_from_html_line(LPCWSTR line, int * codepage);
static bool codepage_from_xml_line(LPCSTR line, int * codepage);
static bool codepage_from_xml_line(LPCWSTR line, int * codepage);
static bool codepage_from_rc_line(LPCSTR line, int * codepage);
static bool codepage_from_rc_line(LPCWSTR line, int * codepage);
static bool isValidCodepage(int cp);
static bool encoding_from_attrib_value(LPCSTR valstart, int * pEncodingId);
static bool encoding_from_attrib_value(LPCWSTR valstart, int * pEncodingId);
static const char *stristr(const char * szStringToBeSearched, const char * szSubstringToSearchFor);
static const wchar_t *wcsistr(const wchar_t * szStringToBeSearched, const wchar_t * szSubstringToSearchFor);

/** 
 * @brief Default constructor setting color to black.
 */
/**
 * @brief Try to deduce encoding for this file
 */
void
GuessCodepageEncoding(const CString & filepath, int * unicoding, int * codepage,
                      BOOL bGuessEncoding)
{
	UniMemFile ufile;
	UniFile * pufile = &ufile;

	if (!pufile->OpenReadOnly(filepath))
	{
		*unicoding = ucr::NONE;
		*codepage = getDefaultCodepage();
		return;
	}
	bool hasbom = pufile->ReadBom();
	*unicoding = pufile->GetUnicoding();
	*codepage = pufile->GetCodepage();
	if (!hasbom && bGuessEncoding)
	{
		if (!filepath.Right(4).CompareNoCase(_T(".htm"))
			|| !filepath.Right(5).CompareNoCase(_T(".html")))
		{
			GuessEncoding_html_from_unifile(pufile, unicoding, codepage);
		}
		else if (!filepath.Right(4).CompareNoCase(_T(".xml"))
			|| !filepath.Right(5).CompareNoCase(_T(".xsl")))
		{
			GuessEncoding_xml_from_unifile(pufile, unicoding, codepage);
		}
		else if (!filepath.Right(3).CompareNoCase(_T(".rc")))
		{
			GuessEncoding_rc_from_unifile(pufile, unicoding, codepage);
		}
	}
	pufile->Close();
}

/*** Return true if text begins with prefix (case-insensitive), for char */
static bool
StartsWithInsensitive(LPCSTR text, LPCSTR prefix, int prefixlen)
{
	return 0 == strnicmp(text, prefix, prefixlen);
}
/*** Return true if text begins with prefix (case-insensitive), for wchar_t */
static bool
StartsWithInsensitive(LPCWSTR text, LPCWSTR prefix, int prefixlen)
{
	return 0 == wcsnicmp(text, prefix, prefixlen);
}

/**
 * @brief Parser for HTML files to find encoding information
 *
 * To be removed when plugin event added for this
 */
static bool
GuessEncoding_html_from_unifile(UniFile * pufile, int * encoding, int * codepage)
{
	CString line, eol;
	while (1)
	{
		if (pufile->GetLineNumber() > 30)
			break;
		if (!pufile->ReadString(line, eol))
			break;
		if (codepage_from_html_line(line, codepage))
			return true;

	}
	return false;
}

/**
 * @brief Parser for HTML files to find encoding information
 */
static bool demoGuessEncoding_html(const char **data, int count, int * cp)
{
	if (count > 30)
		count = 30;
	while (count--)
	{
		const char *line = *data++;
		if (codepage_from_html_line(line, cp))
			return true;
	}
	return false;
}


/**
 * @brief Deduce codepage from this line of text from an HTML file, if we can
 *
 * char version
 *
 * @todo It is unfortunate to have both a char and a wchar_t version of this.
 */
static bool
codepage_from_html_line(LPCSTR line, int * codepage)
{
	/** @todo This is not a very complete matching algorithm */
	static LPCSTR metapref = "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=";
	static int metalen = strlen(metapref);

	LPCSTR cpcandidate = stristr(line, metapref);
	if (!cpcandidate)
		return false;
	cpcandidate += metalen;

	int encodingId = 0;
	if (encoding_from_attrib_value(cpcandidate, &encodingId))
	{
		*codepage = GetEncodingCodePageFromId(encodingId);
		if (*codepage)
			return true;
	}

	return false;
}

/** 
 * @brief Deduce codepage from this line of text from an HTML file, if we can
 *
 * wchar_t version
 */
static bool
codepage_from_html_line(LPCWSTR line, int * codepage)
{
	/** @todo This is not a very complete matching algorithm */
	static LPCWSTR metapref = L"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=";
	static int metalen = wcslen(metapref);

	LPCWSTR cpcandidate = wcsistr(line, metapref);
	if (!cpcandidate)
		return false;
	cpcandidate += metalen;

	int encodingId = 0;
	if (encoding_from_attrib_value(cpcandidate, &encodingId))
	{
		*codepage = GetEncodingCodePageFromId(encodingId);
		if (*codepage)
			return true;
	}

	return false;
}

/**
 * @brief Parser for XML files to find encoding information
 */
static bool
GuessEncoding_xml_from_unifile(UniFile * pufile, int * encoding, int * codepage)
{
	CString line, eol;
	while (1)
	{
		if (pufile->GetLineNumber() > 30)
			break;
		if (!pufile->ReadString(line, eol))
			break;
		if (codepage_from_xml_line(line, codepage))
			return true;

	}
	return false;
}

/**
 * @brief Parser for HTML files to find encoding information
 *
 * To be removed when plugin event added for this
 */
static bool demoGuessEncoding_xml(const char **data, int count, int * cp)
{
	if (count > 30)
		count = 30;
	while (count--)
	{
		const char *line = *data++;
		if (codepage_from_xml_line(line, cp))
			return true;
	}
	return false;
}

/** 
 * @brief Deduce codepage from this line of text from an XML file, if we can
 *
 * char version
 *
 * @todo It is unfortunate to have both a char and a wchar_t version of this.
 */
static bool
codepage_from_xml_line(LPCSTR line, int * codepage)
{
	/** @todo This is not a very complete matching algorithm */
	static LPCSTR metapref = "<?xml version=\"1.0\" encoding=";
	static int metalen = strlen(metapref);

	LPCSTR cpcandidate = stristr(line, metapref);
	if (!cpcandidate)
		return false;
	cpcandidate += metalen;

	int encodingId = 0;
	if (encoding_from_attrib_value(cpcandidate, &encodingId))
	{
		*codepage = GetEncodingCodePageFromId(encodingId);
		if (*codepage)
			return true;
	}

	return false;
}

/** 
 * @brief Deduce codepage from this line of text from an XML file, if we can
 *
 * wchar_t version
 */
static bool
codepage_from_xml_line(LPCWSTR line, int * codepage)
{
	/** @todo This is not a very complete matching algorithm */
	static LPCWSTR metapref = L"<?xml version=\"1.0\" encoding=";
	static int metalen = wcslen(metapref);

	LPCWSTR cpcandidate = wcsistr(line, metapref);
	if (!cpcandidate)
		return false;
	cpcandidate += metalen;
	
	int encodingId = 0;
	if (encoding_from_attrib_value(cpcandidate, &encodingId))
	{
		*codepage = GetEncodingCodePageFromId(encodingId);
		if (*codepage)
			return true;
	}

	return false;
}

/**
 * @brief Parser for rc files to find encoding information
 */
static bool
GuessEncoding_rc_from_unifile(UniFile * pufile, int * encoding, int * codepage)
{
	CString line, eol;
	while (1)
	{
		if (pufile->GetLineNumber() > 30)
			break;
		if (!pufile->ReadString(line, eol))
			break;
		if (codepage_from_rc_line(line, codepage))
			return true;
	}
	return false;
}


/**
 * @brief Parser for rc files to find encoding information
 */
static bool demoGuessEncoding_rc(const char **data, int count, int * cp)
{
	if (count > 30)
		count = 30;
	while (count--)
	{
		const char *line = *data++;
		if (codepage_from_rc_line(line, cp))
			return true;
	}
	return false;
}

/**
 * @brief Deduce codepage from this line of text from an HTML file, if we can
 *
 * char version
 *
 * @todo It is unfortunate to have both a char and a wchar_t version of this.
 */
static bool
codepage_from_rc_line(LPCSTR line, int * codepage)
{
	int cp=0;
	if (1 == sscanf(line, "#pragma code_page(%d)", &cp)
		&& isValidCodepage(cp))
	{
		*codepage = cp;
		return true;
	}
	return false;

}

/**
 * @brief Deduce codepage from this line of text from an HTML file, if we can
 *
 * wchar_t version
 *
 * @todo It is unfortunate to have both a char and a wchar_t version of this.
 */
static bool
codepage_from_rc_line(LPCWSTR line, int * codepage)
{
	int cp=0;
	if (1 == swscanf(line, L"#pragma code_page(%d)", &cp)
		&& isValidCodepage(cp))
	{
		*codepage = cp;
		return true;
	}
	return false;

}

/**
 * @brief Is specified codepage number valid on this system?
 */
static bool isValidCodepage(int cp)
{
	return isCodepageSupported(cp);
}

/**
 * @brief Try to deduce encoding for this file
 */
bool
GuessEncoding_from_bytes(const CString & sExt, const char **data, int count, int *codepage)
{
	if (lstrcmpi(sExt, _T(".rc")) ==  0)
	{
		int cp=0;
		if (demoGuessEncoding_rc(data, count, &cp))
		{
			*codepage = cp;
			return true;
		}
	}
	else if (lstrcmpi(sExt, _T(".htm")) == 0 || lstrcmpi(sExt, _T(".html")) == 0)
	{
		int cp=0;
		if (demoGuessEncoding_html(data, count, &cp))
		{
			*codepage = cp;
			return true;
		}
	}
	else if (lstrcmpi(sExt, _T(".xml")) == 0 || lstrcmpi(sExt, _T(".xsl")) == 0)
	{
		int cp=0;
		if (demoGuessEncoding_xml(data, count, &cp))
		{
			*codepage = cp;
			return true;
		}
	}
	return false;
}

/**
 * @brief Parse an xml or html attribute into an encoding id from charset.h
 */
static bool
encoding_from_attrib_value(LPCSTR valstart, int * pEncodingId)
{
	static char buffer[128];
	LPCSTR end = valstart;
	int offset = 0;

	// copy candidate value into buffer (using appropriate delimiter)
	if (*valstart == '\'')
	{
		++end;
		// single quoted attribute
		while (*end && (end - valstart < sizeof(buffer)-1)
			&& *end != '\'')
		{
			buffer[offset] = *end;
			++end;
			++offset;
		}
	}
	else if (*valstart == '"')
	{
		++end;
		// double quoted attribute
		while (*end && (end - valstart < sizeof(buffer)-1)
			&& *end != '"')
		{
			buffer[offset] = *end;
			++end;
			++offset;
		}
	}
	else
	{
		// unquoted attibute, so watch for space or end tag
		while (*end && (end - valstart < sizeof(buffer)-1)
			&& *end != ' ' && *end != '>')
		{
			buffer[offset] = *end;
			++end;
			++offset;
		}
	}
	// must zero-terminate buffer
	buffer[offset] = 0;

	// Is it an encoding name known to charsets module ?
	*pEncodingId = GetEncodingIdFromName(buffer);
	// GetEncodingIdFromName returns non-zero if valid
	if (*pEncodingId != 0)
		return true;

	// Is it a codepage known to charsets module ?
	int cpnum = 0;
	if (1 == sscanf(buffer, "%d", &cpnum))
	{
		*pEncodingId = GetEncodingIdFromCodePage(cpnum);
		// GetEncodingIdFromName returns non-zero if valid
		if (*pEncodingId != 0)
			return true;
	}

	return false;
}

/**
 * @brief Parse an xml or html attribute into an encoding id from charset.h
 */
static bool
encoding_from_attrib_value(LPCWSTR valstart, int * pEncodingId)
{
	static wchar_t buffer[128];
	LPCWSTR end = valstart;
	int offset = 0;

	// copy candidate value into buffer (using appropriate delimiter)
	if (*valstart == '\'')
	{
		++end;
		// single quoted attribute
		while (*end && (end - valstart < sizeof(buffer)-1)
			&& *end != '\'')
		{
			buffer[offset] = *end;
			++end;
			++offset;
		}
	}
	else if (*valstart == '"')
	{
		++end;
		// double quoted attribute
		while (*end && (end - valstart < sizeof(buffer)-1)
			&& *end != '"')
		{
			buffer[offset] = *end;
			++end;
			++offset;
		}
	}
	else
	{
		// unquoted attibute, so watch for space or end tag
		while (*end && (end - valstart < sizeof(buffer)-1)
			&& *end != ' ' && *end != '>')
		{
			buffer[offset] = *end;
			++end;
			++offset;
		}
	}
	// must zero-terminate buffer
	buffer[offset] = 0;

	// Is it an encoding name known to charsets module ?
	USES_CONVERSION;
	*pEncodingId = GetEncodingIdFromName(W2A(buffer));
	// GetEncodingIdFromName returns non-zero if valid
	if (*pEncodingId != 0)
		return true;

	// Is it a codepage known to charsets module ?
	int cpnum = 0;
	if (1 == swscanf(buffer, L"%d", &cpnum))
	{
		*pEncodingId = GetEncodingIdFromCodePage(cpnum);
		// GetEncodingIdFromName returns non-zero if valid
		if (*pEncodingId != 0)
			return true;
	}

	return false;
}


static const char *
stristr(const char * szStringToBeSearched, const char * szSubstringToSearchFor)
{
	const char * pPos = NULL;
	char * szCopy1 = NULL;
	char * szCopy2 = NULL;

	// verify parameters
	if (szStringToBeSearched == NULL || szSubstringToSearchFor == NULL)
	{
		return szStringToBeSearched;
	}

	// empty substring - return input (consistent with strstr)
	if (strlen(szSubstringToSearchFor) == 0)
		return szStringToBeSearched;

	szCopy1 = strlwr(strdup(szStringToBeSearched));
	szCopy2 = strlwr(strdup(szSubstringToSearchFor));

	if ( szCopy1 == NULL || szCopy2 == NULL  ) {
		// another option is to raise an exception here
		free((void*)szCopy1);
		free((void*)szCopy2);
		return NULL;
	}

	pPos = strstr(szCopy1, szCopy2);

	if ( pPos != NULL ) {
		// map to the original string
		pPos = szStringToBeSearched + (pPos - szCopy1);
	}

	free((void*)szCopy1);
	free((void*)szCopy2);

	return pPos;
} // stristr(...)

static const wchar_t *
wcsistr(const wchar_t * szStringToBeSearched, const wchar_t * szSubstringToSearchFor)
{
	const wchar_t * pPos = NULL;
	wchar_t * szCopy1 = NULL;
	wchar_t * szCopy2 = NULL;

	// verify parameters
	if (szStringToBeSearched == NULL || szSubstringToSearchFor == NULL)
	{
		return szStringToBeSearched;
	}

	// empty substring - return input (consistent with strstr)
	if (wcslen(szSubstringToSearchFor) == 0)
		return szStringToBeSearched;

	szCopy1 = wcslwr(wcsdup(szStringToBeSearched));
	szCopy2 = wcslwr(wcsdup(szSubstringToSearchFor));

	if ( szCopy1 == NULL || szCopy2 == NULL  ) {
		// another option is to raise an exception here
		free((void*)szCopy1);
		free((void*)szCopy2);
		return NULL;
	}

	pPos = wcsstr(szCopy1, szCopy2);

	if ( pPos != NULL ) {
		// map to the original string
		pPos = szStringToBeSearched + (pPos - szCopy1);
	}

	free((void*)szCopy1);
	free((void*)szCopy2);

	return pPos;
} // wcsistr(...)
