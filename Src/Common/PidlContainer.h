/////////////////////////////////////////////////////////////////////////////
//	  WinMerge:  an interactive diff/merge utility
//	  Copyright (C) 1997  Dean P. Grimm
//	  SPDX-License-Identifier: GPL-2.0-or-later
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
