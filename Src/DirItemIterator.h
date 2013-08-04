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
#include "IListCtrl.h"

struct DIFFITEM;

class SelectedDirItemIterator : public std::iterator<std::forward_iterator_tag, std::pair<int, DIFFITEM *> >
{
public:
	SelectedDirItemIterator(IListCtrl *pList) : m_pList(pList)
	{
		m_sel = m_pList->GetNextSelectedItem(-1);
	}

	SelectedDirItemIterator() : m_sel(-1), m_pList(NULL)
	{
	}

	~SelectedDirItemIterator() {}

	SelectedDirItemIterator& operator=(const SelectedDirItemIterator& it)
	{
		m_sel = it.m_sel;
		m_pList = it.m_pList;
	}

	SelectedDirItemIterator& operator++()
	{
		m_sel = m_pList->GetNextSelectedItem(m_sel);
		return *this;
	}

	std::pair<int, DIFFITEM *> operator*()
	{
		return std::make_pair(m_sel, reinterpret_cast<DIFFITEM *>(m_pList->GetItemData(m_sel)));
	}

	bool operator==(const SelectedDirItemIterator& it) const
	{
		return m_sel == it.m_sel;
	}

	bool operator!=(const SelectedDirItemIterator& it) const
	{
		return m_sel != it.m_sel;
	}

	int m_sel;

private:
	IListCtrl *m_pList;
};
