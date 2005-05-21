// WinMergeScript.cpp : Implementation of CWinMergeScript
#include "stdafx.h"
#include "unicheck.h"

/**
 * @brief Check buffer for Unicode byte order mark, and fill in uinfo as appropriate
 */
bool CheckForBom(char * buffer, int len, unicodingInfo * uinfo)
{
	// Need to use unsigned char arithmetic to avoid sign extension
	unsigned char * buffptr = (unsigned char *)buffer;

	memset(uinfo, 0, sizeof(*uinfo));
	if (len>=4)
	{
		if (buffptr[0] == 0xFF && buffptr[1] == 0xFE && buffptr[2] == 0 && buffptr[3] == 0)
		{
			uinfo->type = unicodingInfo::UNICODE_UTF32LE;
			uinfo->bom_width = 4;
			uinfo->char_width = 4;
			uinfo->low_byte = 0;
			return true;
		}
		if (buffptr[0] == 0 && buffptr[1] == 0 && buffptr[2] == 0xFE && buffptr[3] == 0xFF)
		{
			uinfo->type = unicodingInfo::UNICODE_UTF32BE;
			uinfo->bom_width = 4;
			uinfo->char_width = 4;
			uinfo->low_byte = 3;
			return true;
		}
	}
	if (len>=3)
	{
		if (buffptr[0] == 0xEF && buffptr[1] == 0xBB && buffptr[2] == 0xBF)
		{
			uinfo->type = unicodingInfo::UNICODE_UTF8;
			uinfo->bom_width = 3;
			uinfo->char_width = 1; // check every byte for 0
			uinfo->low_byte = 0; // irrelevant if char_width is 1
			return true;
		}
	}
	if (len>=2)
	{
		if (buffptr[0] == 0xFF && buffptr[1] == 0xFE)
		{
			uinfo->type = unicodingInfo::UNICODE_UCS2LE;
			uinfo->bom_width = 2;
			uinfo->char_width = 2;
			uinfo->low_byte = 0;
			return true;
		}
		if (buffptr[0] == 0xFE && buffptr[1] == 0xFF)
		{
			uinfo->type = unicodingInfo::UNICODE_UCS2BE;
			uinfo->bom_width = 2;
			uinfo->char_width = 2;
			uinfo->low_byte = 1;
			return true;
		}
	}
	uinfo->type = unicodingInfo::UNICODE_NONE;
	uinfo->bom_width = 0;
	uinfo->char_width = 1; // check every byte for 0
	uinfo->low_byte = 0; // irrelevant if char_width is 1
	return false;
}

