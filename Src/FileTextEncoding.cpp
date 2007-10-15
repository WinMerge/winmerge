/**
 * @file  FileTextEncoding.cpp
 *
 * @brief Implementation of FileTextEncoding structure
 */
// ID line follows -- this is updated by SVN
// $Id$

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
	m_unicoding = 0; // assume unicoding==0 means none
	m_bom = false;
	m_guessed = false;
}

/**
 * @brief Set codepage
 */
void FileTextEncoding::SetCodepage(int codepage)
{
	m_codepage = codepage;
	m_unicoding = ucr::NONE;
	if (codepage == CP_UTF8)
		m_unicoding = ucr::UTF8;
}

void FileTextEncoding::SetUnicoding(int unicoding)
{
	if (unicoding == ucr::NONE)
		m_codepage = CP_ACP; // not sure what to do here
	m_unicoding = unicoding;
	if (m_unicoding == ucr::UTF8)
		m_codepage = CP_UTF8;
}

/**
 * @brief Return string representation of encoding, eg "UCS-2LE", or "1252"
 */
String FileTextEncoding::GetName() const
{
	if (m_unicoding == ucr::UCS2LE)
		return _T("UCS-2LE");
	if (m_unicoding == ucr::UCS2BE)
		return _T("UCS-2BE");
	if (m_unicoding == ucr::UTF8)
		return _T("UTF-8");

	String str;
	if (m_codepage > -1)
	{
		str.resize(32);
		LPTSTR s = &*str.begin(); //GetBuffer(32);
		int len = _sntprintf(s, 32, _T("%d"), m_codepage);
		str.resize(len);
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
