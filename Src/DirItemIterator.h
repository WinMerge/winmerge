/**
 *  @file DirItemIterator.h
 *
 *  @brief Declaration DirItemIterator classes.
 */ 
// ID line follows -- this is updated by SVN
// $Id$

#pragma once

#include <iterator>
#include <utility>
#include <Poco/Types.h>
#include "IListCtrl.h"

struct DIFFITEM;

class DirItemWithIndexIterator : public std::iterator<std::forward_iterator_tag, std::pair<int, DIFFITEM *> >
{
public:
	DirItemWithIndexIterator(IListCtrl *pList, int sel = -1, bool selected = false, bool reverse = false) :
	  m_pList(pList), m_sel(sel), m_selected(selected), m_reverse(reverse)
	{
		if (m_sel == -1)
		{
			if (m_reverse)
			{
				int last = m_pList->GetRowCount() - 1;
				if (!m_selected || m_pList->IsSelectedItem(last))
					m_sel = last;
				else
					m_sel = m_pList->GetNextItem(last, m_selected, m_reverse);
			}
			else
				m_sel = m_pList->GetNextItem(-1, m_selected, m_reverse);
		}
		if (m_sel != -1)
		{
			if (m_pList->GetItemData(m_sel) == reinterpret_cast<void *>((Poco::UIntPtr)-1L))
				m_sel = m_pList->GetNextItem(m_sel, m_selected, m_reverse);
		}
	}

	DirItemWithIndexIterator() : m_pList(NULL), m_sel(-1)
	{
	}

	~DirItemWithIndexIterator() {}

	DirItemWithIndexIterator& operator=(const DirItemWithIndexIterator& it)
	{
		m_sel = it.m_sel;
		m_pList = it.m_pList;
		return *this;
	}

	DirItemWithIndexIterator& operator++()
	{
		m_sel = m_pList->GetNextItem(m_sel, m_selected, m_reverse);
		return *this;
	}

	std::pair<int, DIFFITEM *> operator*()
	{
		return std::make_pair(m_sel, reinterpret_cast<DIFFITEM *>(m_pList->GetItemData(m_sel)));
	}

	bool operator==(const DirItemWithIndexIterator& it) const
	{
		return m_sel == it.m_sel;
	}

	bool operator!=(const DirItemWithIndexIterator& it) const
	{
		return m_sel != it.m_sel;
	}

	bool m_selected;
	bool m_reverse;
	int m_sel;

private:
	IListCtrl *m_pList;
};

class DirItemIterator : public std::iterator<std::forward_iterator_tag, DIFFITEM*>
{
public:
	DirItemIterator(IListCtrl *pList, int sel = -1, bool selected = false, bool reverse = false) : 
	  m_pList(pList), m_sel(sel), m_selected(selected), m_reverse(reverse), m_pdi(NULL)
	{
		if (m_sel == -1)
		{
			if (m_reverse)
			{
				int last = m_pList->GetRowCount() - 1;
				if (!m_selected || m_pList->IsSelectedItem(last))
					m_sel = last;
				else
					m_sel = m_pList->GetNextItem(last, m_selected, m_reverse);
			}
			else
				m_sel = m_pList->GetNextItem(-1, m_selected, m_reverse);
		}
		if (m_sel != -1)
		{
			m_pdi = reinterpret_cast<const DIFFITEM *>(m_pList->GetItemData(m_sel));
			if (m_pdi == reinterpret_cast<const DIFFITEM *>(-1L))
			{
				m_sel = m_pList->GetNextItem(m_sel, m_selected, m_reverse);
				m_pdi = reinterpret_cast<const DIFFITEM *>(m_pList->GetItemData(m_sel));
			}
		}
	}

	DirItemIterator() : m_pList(NULL), m_sel(-1), m_reverse(false)
	{
	}

	~DirItemIterator() {}

	DirItemIterator& operator=(const DirItemIterator& it)
	{
		m_sel = it.m_sel;
		m_pList = it.m_pList;
		return *this;
	}

	DirItemIterator& operator++()
	{
		m_sel = m_pList->GetNextItem(m_sel, m_selected, m_reverse);
		m_pdi = reinterpret_cast<const DIFFITEM *>(m_pList->GetItemData(m_sel));
		if (m_pdi == reinterpret_cast<const DIFFITEM *>(-1L))
			m_sel = -1;
		return *this;
	}

	DIFFITEM& operator*()
	{
		return *const_cast<DIFFITEM *>(m_pdi);
	}

	const DIFFITEM& operator*() const
	{
		return *m_pdi;
	}

	bool operator==(const DirItemIterator& it) const
	{
		return m_sel == it.m_sel;
	}

	bool operator!=(const DirItemIterator& it) const
	{
		return m_sel != it.m_sel;
	}

	bool m_selected;
	bool m_reverse;
	int m_sel;
	const DIFFITEM *m_pdi;

private:
	IListCtrl *m_pList;
};
