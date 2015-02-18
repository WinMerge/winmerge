/**
 * @file  FileTextEncoding.cpp
 *
 * @brief Implementation of FileTextEncoding structure
 */

#include "FileTextEncoding.h"
#include "unicoder.h"
#include "codepage.h"

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
	case CP_UCS2LE:
		m_unicoding = ucr::UCS2LE;
		break;
	case CP_UCS2BE:
		m_unicoding = ucr::UCS2BE;
		break;
	default:
		m_unicoding = ucr::NONE;
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
		case CP_UCS2LE:
		case CP_UCS2BE:
			m_codepage = 0; // not sure what to do here
			break;
		}
		break;
	case ucr::UTF8:
		m_codepage = CP_UTF8;
		break;
	case ucr::UCS2LE:
		m_codepage = CP_UCS2LE;
		break;
	case ucr::UCS2BE:
		m_codepage = CP_UCS2BE;
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
			return _T("UTF-8 (B)");
		else
			return _T("UTF-8");
	}

	if (m_unicoding == ucr::UCS2LE)
		return _T("UCS-2 LE");
	if (m_unicoding == ucr::UCS2BE)
		return _T("UCS-2 BE");

	if (m_codepage <= 0)
		return _T("");

	if (m_codepage == CP_UTF8)
	{
		// We detected codepage to be UTF-8, but unicoding was not set
		return _T("UTF-8");
	}

	return string_format(_T("%d"), m_codepage);
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
