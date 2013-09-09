/*
Frhed - Free hex editor
Copyright (C) 2000 Raihan Kibria

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

Last change: 2013-02-24 by Jochen Neubeck
*/
/** 
 * @file  Simparr.cpp
 *
 * @brief Implementation file for SimpleString class.
 *
 */
#include "precomp.h"
#include "Simparr.h"

/**
 * @brief Append string.
 * @param [in] ps String to append.
 * @return TRUE
 */
int SimpleString::AppendString(LPCSTR ps)
{
	if (m_nUpperBound == ARR_EMPTY)
		return SetToString(ps);
	else
		InsertAtGrow(m_nUpperBound, ps, 0, static_cast<int>(strlen(ps)));
	return TRUE;
}

/**
 * @brief Replace string content with given string.
 * @param [in] ps String to replace current content.
 * @return TRUE.
 */
int SimpleString::SetToString(LPCSTR ps)
{
	Clear();
	return AppendString(ps);
}

/**
 * @brief Replace string content with given string.
 * @param [in] ps String to replace current content.
 * @return TRUE.
 */
SimpleString& SimpleString::operator=(LPCSTR ps)
{
	SetToString(ps);
	return *this;
}

/**
 * @brief Replace string content with given string.
 * @param [in] str String to replace current content.
 * @return TRUE.
 */
SimpleString& SimpleString::operator=(const SimpleString &str)
{
	SetToString(&str[0]);
	return *this;
}

/**
 * @brief Append string to current string.
 * @param [in] ps String to append.
 * @return New string (given string appended).
 */
SimpleString& SimpleString::operator+=(LPCSTR ps)
{
	if (m_nUpperBound == ARR_EMPTY)
		SetToString(ps);
	else
		InsertAtGrow(m_nUpperBound, ps, 0, static_cast<int>(strlen(ps)));
	return *this;
}

/**
 * @brief Get length of the string.
 * @return String length.
 */
int SimpleString::StrLen() const
{
	return m_pT != NULL ? static_cast<int>(strlen(m_pT)) : 0;
}

/**
 * @brief Constructor.
 * Create a string containing only a zero-byte.
 */
SimpleString::SimpleString()
{
	m_nGrowBy = 64;
	Clear();
}

/**
 * @brief Constructor.
 * Create a SimpleString from a normal char array-string.
 * @param [in] ps String to use as initial value.
 */
SimpleString::SimpleString(LPCSTR ps)
{
	m_nGrowBy = 64;
	SetToString(ps);
}

/**
 * @brief Clear string contents.
 */
void SimpleString::Clear()
{
	ClearAll();
	Append('\0');
}

/**
 * @brief Append a string.
 * @param [in] str1 String to append.
 * @return String with given string appended.
 */
SimpleString SimpleString::operator+(const SimpleString& str1)
{
	SimpleString t1;
	t1.SetToString(m_pT);
	t1 += str1;
	return SimpleString(&t1[0]);
}

/**
 * @brief Check if string is empty.
 * @return true if the string is empty, false otherwise.
 */
bool SimpleString::IsEmpty() const
{
	return !StrLen();
}

/**
 * @brief Concat two strings.
 * @param [in] ps1 First string to concat.
 * @param [in] ps2 Second string to concat.
 * @return String with given strings combined.
 */
SimpleString operator+(const SimpleString &ps1, LPCSTR ps2)
{
	SimpleString s1;
	s1 += ps1;
	s1 += ps2;
	return SimpleString(s1);
}

/**
 * @brief Concat two strings.
 * @param [in] ps1 First string to concat.
 * @param [in] ps2 Second string to concat.
 * @return String with given strings combined.
 */
SimpleString operator+(LPCSTR ps1, const SimpleString &ps2)
{
	SimpleString s1;
	s1 += ps1;
	s1 += ps2;
	return SimpleString(s1);
}
