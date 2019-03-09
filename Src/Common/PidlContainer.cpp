/////////////////////////////////////////////////////////////////////////////
//	  WinMerge:  an interactive diff/merge utility
//	  Copyright (C) 1997-2000  Thingamahoochie Software
//	  Author: Dean Grimm
//
//	  This program is free software; you can redistribute it and/or modify
//	  it under the terms of the GNU General Public License as published by
//	  the Free Software Foundation; either version 2 of the License, or
//	  (at your option) any later version.
//
//	  This program is distributed in the hope that it will be useful,
//	  but WITHOUT ANY WARRANTY; without even the implied warranty of
//	  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
//	  GNU General Public License for more details.
//
//	  You should have received a copy of the GNU General Public License
//	  along with this program; if not, write to the Free Software
//	  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
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
