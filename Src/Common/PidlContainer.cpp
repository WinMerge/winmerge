/////////////////////////////////////////////////////////////////////////////
//	  WinMerge:  an interactive diff/merge utility
//	  Copyright (C) 1997-2000  Thingamahoochie Software
//	  Author: Dean Grimm
//	  SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  PidlContainer.cpp
 *
 * @brief Main implementation file for CPidlContainer
 */

#include "pch.h"
#include "PidlContainer.h"
#pragma warning (push)			// prevent "warning C4091: 'typedef ': ignored on left of 'tagGPFIDL_FLAGS' when no variable is declared"
#pragma warning (disable:4091)	// VC bug when using XP enabled toolsets.
#include <shlobj.h>
#pragma warning (pop)
#include <comdef.h>

CPidlContainer::~CPidlContainer()
{
	IMallocPtr m_pMalloc;
	if (FAILED(SHGetMalloc(&m_pMalloc)))
	{
		// can't do anything but return
		return;
	}
	// free memory allocated for pidls
	for (PidlContainer::iterator iter = m_container.begin(); iter != m_container.end(); ++iter)
	{
		m_pMalloc->Free(*iter);
	}
}

LPCITEMIDLIST* CPidlContainer::GetList() const
{
	if (m_container.empty())
		return nullptr;
	return const_cast<LPCITEMIDLIST*>(&m_container[0]);
}
