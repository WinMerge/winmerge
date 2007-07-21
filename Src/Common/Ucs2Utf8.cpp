/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//    
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  Ucs2Utf8.cpp
 *
 * @brief UCS-2 <--> UTF-8 conversion routines implementation.
 */

#include "stdafx.h"
#include "unicoder.h"
#include "Ucs2Utf8.h"

/**
 * @brief Copy UCS-2LE string to UTF-8 string
 *
 * @param nUcs is the size in wide characters of the source string
 * @param nUtf is the size in bytes of the resulting string
 *
 * @return if nUtf = 0, return the size required for the translation buffer
 */
UINT TransformUcs2ToUtf8(LPCWSTR psUcs, UINT nUcs, LPSTR pcsUtf, UINT nUtf)
{
	if (nUtf == 0)
		// just tell required length
		return ucr::Utf8len_of_string(psUcs, nUcs);

	// the buffer is allocated, output in it directly
	unsigned char * pc = (unsigned char *) pcsUtf;
	int nremains = nUtf;

	// quick way 
	UINT i=0;
	for (i = 0 ; i < nUcs && nremains > 10; ++i)
		nremains -= ucr::to_utf8_advance(psUcs[i], pc);

	// be careful for the end of the buffer, risk of overflow because
	// of the variable length of the UTF-8 character
	unsigned char smallTempBuffer[20];
	int nremainsend = nremains;
	unsigned char * pcTemp = (unsigned char *) smallTempBuffer;
	for ( ; i < nUcs && nremainsend > 0; ++i)
		nremainsend -= ucr::to_utf8_advance(psUcs[i], pcTemp);

	int ncomplement = min(nremains, pcTemp-smallTempBuffer);
	CopyMemory(pc, smallTempBuffer, ncomplement);
	nremains -= ncomplement;

	// return number of written bytes
	return (nUtf - nremains);
}

/**
 * @brief Copy UTF-8 string to UCS-2LE string
 *
 * @return if nUcs = 0, return the size required for the translation buffer
 */
UINT TransformUtf8ToUcs2(LPCSTR pcsUtf, UINT nUtf, LPWSTR psUcs, UINT nUcs)
{
	if (nUcs == 0)
		// just tell required length
		return ucr::stringlen_of_utf8(pcsUtf, nUtf);

	// the buffer is allocated, output in it directly
	unsigned char * pUtf = (unsigned char * ) pcsUtf;
	LPWSTR pwc = psUcs;
	int nremains = nUcs;

	for (UINT i = 0 ; i < nUtf && nremains > 0; )
	{
		*pwc++ = ucr::GetUtf8Char(pUtf+i);
		nremains --;
		int chlen = ucr::Utf8len_fromLeadByte(pUtf[i]);
		if (chlen < 1) chlen = 1;
		i += chlen;
	}

	// return number of written wchars
	return (nUtf - nremains);
}

/**
 * @brief Convert string to UTF-8.
 * This function converts the given string to UTF-8, and returns pointer
 * to converted string. Given string must be deallocated by calling
 * UCS2UTF8_Dealloc() after use.
 * @param [in] strOrigin String to convert.
 * @return Pointer to UTF-8 string.
 */
char * UCS2UTF8_ConvertToUtf8(LPCTSTR strOrigin)
{
#ifdef UNICODE
	// Get the size of UTF-8 string
	int str_len = TransformUcs2ToUtf8(strOrigin, _tcslen(strOrigin), NULL, 0);
	++str_len; // Space for zero at end

	char * str_utf = (char *) malloc(str_len);
	ZeroMemory(str_utf, str_len);
	str_len = TransformUcs2ToUtf8(strOrigin, _tcslen(strOrigin), str_utf, str_len);
#else
	char *str_utf = strdup(strOrigin);
#endif
	
	return str_utf;
}

/**
 * @brief Free string allocated by UCS2UTF8_ConvertToUtf8().
 * @param [in] Utf8Str String to deallocate.
 */
void UCS2UTF8_Dealloc(char * Utf8Str)
{
	free(Utf8Str);
}
