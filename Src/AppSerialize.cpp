/** 
 * @file  AppSerialize.cpp
 *
 * @brief Code to serialize settings to/from registry.
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "AppSerialize.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 * @brief Constructor.
 *
 * @param [in] direction Are we reading or writing data.
 * @param [in] section Section (key) name in registry, under application.
 */
AppSerialize::AppSerialize(Direction direction, LPCTSTR section)
: m_direction(direction)
, m_section(section)
{
}

/**
 * @brief Read or write integer value, in current section.
 * @param [in] name Name of value.
 * @param [in,out] value Value to read/write.
 * @param [in] defval Default value.
 */
void AppSerialize::SerializeInt(LPCTSTR name, int & value, int defval)
{
	SerializeInt(m_section, name, value, defval);
}

/**
 * @brief Write or read one integer value, in specified section.
 * @param [in] section Section for value.
 * @param [in] name Name of value.
 * @param [in,out] value Value to read/write.
 * @param [in] defval Default value.
 */
void AppSerialize::SerializeInt(LPCTSTR section, LPCTSTR name, int & value,
	int defval)
{
	if (m_direction == Save)
		AfxGetApp()->WriteProfileInt(section, name, value);
	else
		value = AfxGetApp()->GetProfileInt(section, name, 0);
}

/**
 * @brief Write or read one integer value, in current section.
 * @param [in] name Name of value.
 * @param [in,out] value Value to read/write.
 * @param [in] defval Default value.
 */
void AppSerialize::SerializeLong(LPCTSTR name, LONG & value, LONG defval)
{
	SerializeLong(m_section, name, value, defval);
}

/**
 * @brief Write or read one integer value, in specified section.
 * @param [in] section Section for value.
 * @param [in] name Name of value.
 * @param [in,out] value Value to read/write.
 * @param [in] defval Default value.
 */
void AppSerialize::SerializeLong(LPCTSTR section, LPCTSTR name, LONG & value,
	LONG defval)
{
	if (m_direction == Save)
		AfxGetApp()->WriteProfileInt(section, name, value);
	else
		value = AfxGetApp()->GetProfileInt(section, name, 0);
}

/**
 * @brief Write or read one integer value, in current section.
 * @param [in] name Name of value.
 * @param [in,out] value Value to read/write.
 * @param [in] defval Default value.
 */
void AppSerialize::SerializeByte(LPCTSTR name, BYTE & value, BYTE defval)
{
	SerializeByte(m_section, name, value, defval);
}

/**
 * Write or read one integer value, in specified section.
 * @param [in] section Section for value.
 * @param [in] name Name of value.
 * @param [in,out] value Value to read/write.
 * @param [in] defval Default value.
 */
void AppSerialize::SerializeByte(LPCTSTR section, LPCTSTR name, BYTE & value,
	BYTE defval)
{
	if (m_direction == Save)
		AfxGetApp()->WriteProfileInt(section, name, value);
	else
		value = AfxGetApp()->GetProfileInt(section, name, 0);
}

/**
 * @brief Write or read one string value, in current section.
 * @param [in] name Name of value.
 * @param [in,out] value String to read/write.
 * @param [in] valsize Length of the string.
 * @param [in] defval Default value for string.
 */
void AppSerialize::SerializeTcharArray(LPCTSTR name, LPTSTR value, int valsize,
	LPCTSTR defval)
{
	SerializeTcharArray(m_section, name, value, valsize, defval);
}

/**
 * @brief Write or read one string value, in specified section.
 * @param [in] section Section for value.
 * @param [in] name Name of value.
 * @param [in,out] value String to read/write.
 * @param [in] valsize Length of the string.
 * @param [in] defval Default value for string.
 */
void AppSerialize::SerializeTcharArray(LPCTSTR section, LPCTSTR name,
	LPTSTR value, int valsize, LPCTSTR defval)
{
	if (m_direction == Save)
	{
		AfxGetApp()->WriteProfileString(section, name, value);
	}
	else
	{
		CString sval = AfxGetApp()->GetProfileString(section, name, defval);
		if (sval.GetLength() < valsize)
			_tcscpy(value, sval);
		else
			_tcscpy(value, defval);
	}
}

/**
 * @brief Write or read one set of font setting, in current section.
 * @param [in] name Name of the value.
 * @param [in,out] value Font setting to read/write.
 */
void AppSerialize::SerializeFont(LPCTSTR name, LOGFONT & value)
{
	SerializeFont(m_section, name, value);
}


/**
 * @brief Write or read one set of font setting, in specified section.
 * @param [in] section Section for value.
 * @param [in] name Name of the value.
 * @param [in,out] value Font setting to read/write.
 */
void AppSerialize::SerializeFont(LPCTSTR section, LPCTSTR name,
	LOGFONT & value)
{
	CString sName = name;
	if (!sName.IsEmpty()) sName += _T(".");

	SerializeLong(section, sName + _T("Height"), value.lfHeight);
	SerializeLong(section, sName + _T("Width"), value.lfWidth);
	SerializeLong(section, sName + _T("Escapement"), value.lfEscapement);
	SerializeLong(section, sName + _T("Orientation"), value.lfOrientation);
	SerializeLong(section, sName + _T("Weight"), value.lfWeight);
	SerializeByte(section, sName + _T("Italic"), value.lfItalic);
	SerializeByte(section, sName + _T("Underline"), value.lfUnderline);
	SerializeByte(section, sName + _T("StrikeOut"), value.lfStrikeOut);
	SerializeByte(section, sName + _T("CharSet"), value.lfCharSet);
	SerializeByte(section, sName + _T("OutPrecision"), value.lfOutPrecision);
	SerializeByte(section, sName + _T("ClipPrecision"), value.lfClipPrecision);
	SerializeByte(section, sName + _T("Quality"), value.lfQuality);
	SerializeByte(section, sName + _T("PitchAndFamily"), value.lfPitchAndFamily);
	SerializeTcharArray(section, sName + _T("FaceName"), value.lfFaceName,
		sizeof(value.lfFaceName)/sizeof(value.lfFaceName[0]));
}
