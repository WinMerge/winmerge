////////////////////////////////////////////////////////////////////////////
//  File:       CCrystalEditView.inl
//  Version:    1.0.0.0
//  Created:    29-Dec-1998
//
//  Author:     Stcherbatchenko Andrei
//  E-mail:     windfall@gmx.de
//
//  Inline functions of Crystal Edit classes
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  19-Jul-99
//      Ferdinand Prantl:
//  +   FEATURE: see cpps ...
//
//  ... it's being edited very rapidly so sorry for non-commented
//        and maybe "ugly" code ...
////////////////////////////////////////////////////////////////////////////

#ifndef __CCrystalEditView_INL_INCLUDED
#define __CCrystalEditView_INL_INCLUDED

#include "CCrystalEditView.h"

#ifndef AFX_CCRYSTALTEXTBUFFER_H__AD7F2F49_6CB3_11D2_8C32_0080ADB86836__INCLUDED_
#include "ccrystaltextbuffer.h"
#endif

inline BOOL CCrystalEditView::
GetOverwriteMode ()
const
{
  return m_bOvrMode;
}

inline void CCrystalEditView::SetOverwriteMode (BOOL bOvrMode /*= TRUE*/ )
{
  m_bOvrMode = bOvrMode;
}

inline BOOL CCrystalEditView::
GetDisableBSAtSOL ()
const
{
  return m_bDisableBSAtSOL;
}

inline BOOL CCrystalEditView::GetAutoIndent ()
const
{
  return m_bAutoIndent;
}

inline void CCrystalEditView::SetAutoIndent (BOOL bAutoIndent)
{
  m_bAutoIndent = bAutoIndent;
}

inline BOOL CCrystalEditView::GetInsertTabs ()
const
{
  return m_pTextBuffer->GetInsertTabs();
}

inline void CCrystalEditView::SetInsertTabs (BOOL bInsertTabs)
{
  m_pTextBuffer->SetInsertTabs(bInsertTabs);
}

#endif
