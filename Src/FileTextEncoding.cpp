/**
 * @file  FileTextEncoding.cpp
 *
 * @brief Implementation of FileTextEncoding structure
 */
// ID line follows -- this is updated by SVN
// $Id: FileTextEncoding.cpp 5621 2008-07-15 19:44:58Z kimmov $

#include "stdafx.h"
#include "unicoder.h"
#include "FileTextEncoding.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

FileTextEncoding::FileTextEncoding()
{
	Clear();
}

/**
 * @brief Forget any encoding info we have
 */
void FileTextEncoding::Clear()
{
	m_codepage = -1;
	m_unicoding = ucr::NONE;
	m_bom = false;
	m_guessed = false;
}

/**
 * @brief Set codepage
 */
void FileTextEncoding::SetCodepage(int codepage)
{
	m_codepage = codepage;
	switch (codepage)
	{
	case CP_UTF8:
		m_unicoding = ucr::UTF8;
		break;
	case 1200:
		m_unicoding = ucr::UCS2LE;
		break;
	case 1201:
		m_unicoding = ucr::UCS2BE;
		break;
	}
}

void FileTextEncoding::SetUnicoding(ucr::UNICODESET unicoding)
{
	m_unicoding = unicoding;
	switch (unicoding)
	{
	case ucr::NONE:
		switch (m_codepage)
		{
		case CP_UTF8:
		case 1200:
		case 1201:
			m_codepage = CP_ACP; // not sure what to do here
			break;
		}
		break;
	case ucr::UTF8:
		m_codepage = CP_UTF8;
		break;
	case ucr::UCS2LE:
		m_codepage = 1200;
		break;
	case ucr::UCS2BE:
		m_codepage = 1201;
		break;
	}
}

/**
 * @brief Return string representation of encoding, eg "UCS-2LE", or "1252"
 * @todo This resource lookup should be done in GUI code?
 */
String FileTextEncoding::GetName() const
{
	if (m_unicoding == ucr::UTF8)
	{
		if (m_bom)
			return LoadResString(IDS_UNICODING_UTF8_BOM);
		else
			return LoadResString(IDS_UNICODING_UTF8);
	}

	if (m_unicoding == ucr::UCS2LE)
		return LoadResString(IDS_UNICODING_UCS2_LE);
	if (m_unicoding == ucr::UCS2BE)
		return LoadResString(IDS_UNICODING_UCS2_BE);

	String str;
	if (m_codepage > -1)
	{
		if (m_codepage == CP_UTF8)
		{
			// We detected codepage to be UTF-8, but unicoding was not set
			str = LoadResString(IDS_UNICODING_UTF8);
		}
		else
		{
			str.resize(32);
			LPTSTR s = &*str.begin(); //GetBuffer(32);
			int len = _sntprintf(s, 32, _T("%d"), m_codepage);
			str.resize(len);
		}
	}
	return str;
}

int FileTextEncoding::Collate(const FileTextEncoding & fte1, const FileTextEncoding & fte2)
{
	if (fte1.m_unicoding > fte2.m_unicoding)
		return 1;
	if (fte1.m_unicoding < fte2.m_unicoding)
		return 1;
	if (fte1.m_codepage > fte2.m_codepage)
		return 1;
	if (fte1.m_codepage < fte2.m_codepage)
		return 1;
	return 0;
}
