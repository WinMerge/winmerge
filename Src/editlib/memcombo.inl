///////////////////////////////////////////////////////////////////////////
//  File:    memcombo.inl
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl
//  E-mail:     prantl@ff.cuni.cz
//
//  Combo-box saving last typed expressions
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#ifndef __MEMCOMBO_INL__INCLUDED__
#define __MEMCOMBO_INL__INCLUDED__

#ifdef INLINE
#undef INLINE
#endif

#ifndef _DEBUG
#define INLINE inline
#else
#define INLINE
#endif // _DEBUG

////////////////////////////////////////////////////////////////////////////////
// Methods

INLINE
CMemComboBox::CMemComboBox () : m_bFirstFocus (TRUE)
{
}

INLINE
CMemComboBox::~CMemComboBox ()
{
}

////////////////////////////////////////////////////////////////////////////////

#endif // __MEMCOMBO_INL__INCLUDED__
