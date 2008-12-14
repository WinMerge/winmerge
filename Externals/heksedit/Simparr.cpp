/** 
 * @file  Simparr.cpp
 *
 * @brief Implementation file for SimpleString class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: Simparr.cpp 104 2008-11-05 22:43:37Z kimmov $

#include "precomp.h"
#include "Simparr.h"

//-------------------------------------------------------------------
int SimpleString::AppendString(const char* ps)
{
	if( m_nUpperBound == -1 )
		return SetToString( ps );
	else
	{
		InsertAtGrow( m_nUpperBound, ps, 0, strlen( ps ) );
	}
	return TRUE;
}

//-------------------------------------------------------------------
int SimpleString::SetToString(const char* ps)
{
	Clear();
	return AppendString( ps );
}

//-------------------------------------------------------------------
SimpleString& SimpleString::operator=(const char* ps)
{
	SetToString( ps );
	return *this;
}

//-------------------------------------------------------------------
SimpleString& SimpleString::operator=(const SimpleString &str)
{
	SetToString( &str[0] );
	return *this;
}

//-------------------------------------------------------------------
SimpleString& SimpleString::operator+=(const char* ps)
{
	if( m_nUpperBound == -1 )
		SetToString( ps );
	else
	{
		InsertAtGrow( m_nUpperBound, ps, 0, strlen( ps ) );
	}
	return *this;
}

//-------------------------------------------------------------------
int SimpleString::StrLen() const
{
	if( m_pT != NULL )
		return strlen( m_pT );
	else
		return 0;
}

//-------------------------------------------------------------------
SimpleString::SimpleString()
{
	// Create a string containing only a zero-byte.
	m_nGrowBy = 64;
	Clear();
}

//-------------------------------------------------------------------
SimpleString::SimpleString( const char* ps )
{
	// Create a SimpleString from a normal char array-string.
	m_nGrowBy = 64;
	Clear();
	SetToString( ps );
}

//-------------------------------------------------------------------
void SimpleString::Clear()
{
	ClearAll();
	Append( '\0' );
}

//-------------------------------------------------------------------
SimpleString SimpleString::operator+( const SimpleString& str1 )
{
	SimpleString t1;
	t1.SetToString( m_pT );
	t1 += str1;
	return SimpleString( &t1[0] );
}

//-------------------------------------------------------------------
int SimpleString::IsEmpty() const
{
	return !StrLen();
}

//-------------------------------------------------------------------
SimpleString operator+( const SimpleString &ps1, const char* ps2 )
{
	SimpleString s1;
	s1 += ps1;
	s1 += ps2;
	return SimpleString(s1);
}

//-------------------------------------------------------------------
SimpleString operator+( const char* ps1, const SimpleString &ps2 )
{
	SimpleString s1;
	s1 += ps1;
	s1 += ps2;
	return SimpleString(s1);
}

//-------------------------------------------------------------------
