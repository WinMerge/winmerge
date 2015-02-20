/////////////////////////////////////////////////////////////////////////////
//	  WinMerge:  an interactive diff/merge utility
//	  Copyright (C) 1997  Dean P. Grimm
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
 *	@file PidlContainer.h
 *
 *	@brief Declaration of class CPidlContainer
 */ 
#pragma once

#include <ShTypes.h>
#include <vector>

/**
 * @brief Container for LPITEMIDLIST items
 *
 * When showing shell context menu we need to collect pidls for items.
 * This class simplifies this task.
 * Items are stored in vector. Memory allocated for pidls are freed in destructor.
 * You can add items and then query container size and pointer to 
 * LPCITEMIDLIST needed by IContextMenu::GetUIObjectOf
 */
class CPidlContainer
{
public:
	/**
	 * @brief Destructor
	 *
	 * Frees memory allocated for pidls using IMalloc interface
	 */
	~CPidlContainer();

	/**
	 * @brief Adds item to container
	 *
	 * @param[in]	pidl	item to add
	 */
	void Add(LPITEMIDLIST pidl) { m_container.push_back(pidl); } 

	/**
	 * @brief Returns pointer to array of LPCITEMIDLIST items
	 *
	 * @return Pointer to array of LPCITEMIDLIST items.
	 *		   May be invalidated by Add()
	 */
	LPCITEMIDLIST* GetList() const;

	/**
	 * @brief Returns number of items in container
	 *
	 * @return Number of items in container
	 */
	size_t Size() const { return m_container.size(); }

private:
	typedef std::vector<LPITEMIDLIST> PidlContainer;
	PidlContainer m_container; /**< stores items */
};
