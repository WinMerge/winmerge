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
 * @file  Simparr_imp.h
 *
 * @brief Template class implementation file.
 *
 */
//-------------------------------------------------------------------
template<class T> SimpleArray<T>::SimpleArray()
{
	m_pT = NULL;
	m_nSize = 0;
	m_nUpperBound = ARR_EMPTY;
	m_nGrowBy = 1;
}

//-------------------------------------------------------------------
template<class T> SimpleArray<T>::SimpleArray(int nNewSize, int nGrowBy)
	: m_nGrowBy(nGrowBy)
	, m_nUpperBound(ARR_EMPTY),
	m_nSize(nNewSize)
{
	m_pT = new T[m_nSize];
}

//-------------------------------------------------------------------
template<class T> SimpleArray<T>::SimpleArray(const T* ptArray, int upbound, int size)
	: m_nGrowBy(1)
	, m_pT(ptArray)
	, m_nUpperBound(upbound)
	, m_nSize(size)
{
#ifdef _DEBUG
	if (m_pT == NULL || m_nUpperBound<0 || m_nSize<=0 || m_nUpperBound >= m_nSize)
	{
		m_nSize = 0;
		m_nUpperBound = ARR_EMPTY;
	}
#endif
}

//-------------------------------------------------------------------
template<class T> SimpleArray<T>::SimpleArray(const SimpleArray& spaArg)
	: m_nSize(spaArg.m_nSize)
	, m_nUpperBound(spaArg.m_nUpperBound)
	, m_nGrowBy(spaArg.m_nGrowBy)
{
	m_pT = new T[m_nSize];
	for (int k = 0; k <= m_nUpperBound; k++)
		m_pT[k] = spaArg.m_pT[k];
}

//-------------------------------------------------------------------
template<class T> SimpleArray<T>::~SimpleArray()
{
	if (m_pT != NULL)
		delete [] m_pT;
}

//-------------------------------------------------------------------
template<class T> bool SimpleArray<T>::InsertAtGrow(int nIndex, T argT, int nCount)
{
	if (nIndex<0 || nCount<1)
		return false;
	int i = 0;
	if (nIndex > m_nUpperBound)
	{
		for (i = 0; i < nCount; i++)
			SetAtGrow(nIndex + i, argT);
		return true;
	}
	else
	{
		if (m_nSize < m_nUpperBound + 1 + nCount)
		{
			if (!AddSpace(nCount))
				return false;
		}
		for(i = m_nUpperBound + nCount; i > nIndex; i--)
		{
			m_pT[i] = m_pT[i - nCount];
		}
		for (i = 0; i < nCount; i++)
			m_pT[nIndex + i] = argT;
		m_nUpperBound += nCount;
		return true;
	}
}

/**
 * @brief Insert bytes growing the array if needed.
 * @param [in] nIndex Index in array where to insert bytes.
 * @param [in] pT Pointer to inserted bytes.
 * @param [in] nSrcIndex Index from which inserted bytes in @p pT are read.
 * @param [in] nCount Count of bytes to insert.
 */
template<class T> bool SimpleArray<T>::InsertAtGrow(int nIndex, const T* pT,
		int nSrcIndex, int nCount)
{
	if (nIndex<0 || nCount<1)
		return false;
	int i = 0;

	if (nIndex > m_nUpperBound)
	{
		// If there is enough space, just insert the bytes.
		for (i = 0; i < nCount; i++)
		{
			if (!SetAtGrowRef(nIndex + i, pT[nSrcIndex + i]))
				return false;
		}
		return true;
	}
	else
	{
		// Not enough space, must expand the array first
		if (m_nSize < m_nUpperBound + 1 + nCount)
		{
			if (!AddSpace(nCount))
				return false;
		}

		// Copy bytes in table to make space for added bytes
		for (i = m_nUpperBound + nCount; i >= nIndex + nCount; i--)
		{
			m_pT[i] = m_pT[i - nCount];
		}

		// Copy bytes to insert
		for (i = 0; i < nCount; i++)
			m_pT[nIndex + i] = pT[nSrcIndex + i];

		m_nUpperBound += nCount;
		return true;
	}
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::InsertAtGrowRef(int nIndex, const T& argT, int nCount)
{
	if (nIndex<0 || nCount<1)
		return;
	int i = 0;
	if (nIndex > m_nUpperBound)
	{
		for (i = 0; i < nCount; i++)
			SetAtGrowRef(nIndex + i, argT);
		return;
	}
	else
	{
		if (m_nSize < m_nUpperBound + 1 + nCount)
			AddSpace(nCount);
		for (i = m_nUpperBound + nCount; i > nIndex; i--)
		{
			m_pT[i] = m_pT[i - nCount];
		}
		for (i = 0; i < nCount; i++)
			m_pT[nIndex + i] = argT;
		m_nUpperBound += nCount;
	}
}

//-------------------------------------------------------------------
template<class T> bool SimpleArray<T>::InsertAt(int nIndex, T argT, int nCount)
{
	// Valid index?
	if (nIndex < 0 || nIndex > m_nUpperBound)
		return false;

	int i;
	// Is there enough space after m_nUpperBound for inserting?
	if ((m_nSize - 1) - m_nUpperBound >= nCount)
	{
		// Yes, no need to allocate more memory.
		// Push up the elements at the current position.
		for (i = m_nUpperBound + nCount; i >= nIndex + nCount; i--)
			m_pT[i] = m_pT[i - nCount];
		// Copy in the new data.
		for (i = 0; i < nCount; i++)
			m_pT[nIndex + i] = argT;
		// Adjust m_nUpperBound to new size.
		m_nUpperBound += nCount;
	}
	else
	{
		// No, need to allocate more memory.
		for (i = m_nSize - 1; i >= nIndex + nCount; i--)
			m_pT[i] = m_pT[i - nCount];
		if (m_nSize - nIndex < nCount)
			nCount = m_nSize - nIndex;
		for (i = 0; i < nCount; i++)
			m_pT[nIndex + i] = argT;
		m_nUpperBound = m_nSize - 1;
	}
	return true;
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::InsertAtRef(int nIndex, const T& argT, int nCount)
{
	if (nIndex < 0 || nIndex > m_nUpperBound)
		return;
	int i = 0;
	if ((m_nSize - 1) - m_nUpperBound >= nCount)
	{
		for (i = m_nUpperBound + nCount; i >= nIndex + nCount; i--)
			m_pT[i] = m_pT[i - nCount];
		for (i = 0; i < nCount; i++)
			m_pT[nIndex + i] = argT;
		m_nUpperBound += nCount;
	}
	else
	{
		for (i = m_nSize - 1; i >= nIndex + nCount; i--)
			m_pT[i] = m_pT[i - nCount];
		if (m_nSize - nIndex < nCount)
			nCount = m_nSize - nIndex;
		for (i = 0; i < nCount; i++)
			m_pT[nIndex + i] = argT;
		m_nUpperBound = m_nSize - 1;
	}
}

//-------------------------------------------------------------------
template<class T> bool SimpleArray<T>::RemoveAt(int nIndex, int nCount)
{
	if (nIndex < 0 || nIndex > m_nUpperBound || nCount < 1)
		return false;
	if (nCount > m_nUpperBound - nIndex)
	{
		m_nUpperBound = nIndex - 1;
		return true;
	}
	for(int i = nIndex; i <= m_nUpperBound - nCount; i++)
	{
		m_pT[i] = m_pT[i + nCount];
	}
	m_nUpperBound -= nCount;
	return true;
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::SetAtGrow(int nIndex, T argT)
{
	if (nIndex < 0)
		return;
	if (nIndex > m_nSize - 1)
		AddSpace(nIndex - m_nSize + 1);
	m_pT[nIndex] = argT;
	if (nIndex > m_nUpperBound)
		m_nUpperBound = nIndex;
}

//-------------------------------------------------------------------
template<class T> bool SimpleArray<T>::SetAtGrowRef(int nIndex, const T& argT)
{
	if (nIndex < 0)
		return false;
	if (nIndex > m_nSize - 1)
	{
		if (!AddSpace(nIndex - m_nSize + 1))
			return false;
	}
	m_pT[nIndex] = argT;
	if (nIndex > m_nUpperBound)
		m_nUpperBound = nIndex;
	return true;
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::SetAt(int nIndex, T argT)
{
	if (nIndex >= 0 && nIndex < m_nSize)
	{
		m_pT[nIndex] = argT;
		if (nIndex > m_nUpperBound)
			m_nUpperBound = nIndex;
	}
	else
		return;
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::SetAtRef(int nIndex, const T& argT)
{
	if (nIndex >= 0 && nIndex < m_nSize)
	{
		m_pT[nIndex] = argT;
		if (nIndex > m_nUpperBound)
			m_nUpperBound = nIndex;
	}
	else
		return;
}

//-------------------------------------------------------------------
template<class T> T SimpleArray<T>::GetAt(int nIndex) const
{
	return m_pT[nIndex];
}

//-------------------------------------------------------------------
template<class T> bool SimpleArray<T>::AddSpace(int nExtend)
{
	int newsize = m_nSize + (((nExtend - 1) / m_nGrowBy) + 1) * m_nGrowBy;
	T* pT = new T[newsize];
	if (pT != NULL)
	{
		for (int i = 0; i < m_nSize; i++)
			pT[i] = m_pT[i];
		if (m_pT != NULL)
			delete [] m_pT;
		m_pT = pT;
		m_nSize = newsize;
		return true;
	}
	else
		return false;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::GetGrowBy() const
{
	return m_nGrowBy;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::GetSize() const
{
	return m_nSize;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::GetUpperBound() const
{
	return m_nUpperBound;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::GetLength() const
{
	return m_nUpperBound+1;
}

//-------------------------------------------------------------------
template<class T> bool SimpleArray<T>::SetSize(int nNewSize, int nGrowBy)
{
	if (nNewSize < 0)
		return false;
	if (nNewSize == m_nSize)
		return true;
	if (nNewSize == 0)
	{
		ClearAll();
		return true;
	}

	T* pT = new T[nNewSize];
	if (pT == NULL)
		return false;
	int i = 0;
	if (m_nUpperBound < nNewSize)
	{
		for (i = 0; i <= m_nUpperBound; i++)
			pT[i] = m_pT[i];
	}
	else
	{
		for (i = 0; i < nNewSize; i++)
			pT[i] = m_pT[i];
		m_nUpperBound = nNewSize - 1;
	}

	if (m_pT != NULL)
		delete [] m_pT;
	m_pT = pT;
	m_nSize = nNewSize;
	if (nGrowBy > 0)
		m_nGrowBy = nGrowBy;
	return true;
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::SetGrowBy(int nGrowBy)
{
	if (nGrowBy > 0)
		m_nGrowBy = nGrowBy;
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::ClearAll()
{
	if (m_pT != NULL)
		delete [] m_pT;
	m_pT = NULL;
	m_nSize = 0;
	m_nUpperBound = ARR_EMPTY;
}

//-------------------------------------------------------------------
template<class T> bool SimpleArray<T>::blContainsRef(const T& argT)
{
	for (int i = 0; i <= m_nUpperBound; i++)
	{
		if (argT == m_pT[i])
			return true;
	}
	return false;
}

//-------------------------------------------------------------------
template<class T> bool SimpleArray<T>::blContains(T argT)
{
	for (int i = 0; i <= m_nUpperBound; i++)
	{
		if (argT == m_pT[i])
			return true;
	}
	return false;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::nContainsAt(T argT)
{
	for (int i = 0; i <= m_nUpperBound; i++)
	{
		if (argT == m_pT[i])
			return i;
	}
	return ARR_EMPTY;
}

//-------------------------------------------------------------------
// Make a copy of the other SimpleArray.
template<class T> SimpleArray<T>& SimpleArray<T>::operator=(SimpleArray<T>& spa)
{
	// Can't assign to itself: "sa1 = sa1;" not allowed.
	if (&spa != this)
	{
		// If this array is not empty then delete it.
		ClearAll();
		// Allocate memory.
		int nSize = spa.m_nUpperBound + 1;
		m_pT = new T[nSize];
		// Copy the valid elements.
		if (m_pT != NULL)
		{
			// This array now is just large enough to contain the valid elements of spa.
			m_nUpperBound = spa.m_nUpperBound;
			m_nSize = nSize;
			// GrowBy rate is also copied.
			m_nGrowBy = spa.m_nGrowBy;
			int k;
			for (k = 0; k <= m_nUpperBound; k++)
				m_pT[k] = spa.m_pT[k];
		}
		// If no memory could be allocated, then this array remains empty.
	}
	return *this;
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::AppendRef(const T& argT)
{
	SetAt(m_nUpperBound + 1, argT);
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::Append(T argT)
{
	SetAtGrow(m_nUpperBound + 1, argT);
}

//-------------------------------------------------------------------
template<class T> T& SimpleArray<T>::GetRefAt(int nIndex) const
{
	return m_pT[nIndex];
}

//-------------------------------------------------------------------
template<class T> bool SimpleArray<T>::blCompare(SimpleArray<T>& spa) const
{
	if (m_nUpperBound != spa.GetUpperBound())
		return false;

	for (int k = 0; k <= m_nUpperBound; k++)
	{
		if (m_pT[k] != spa[k])
			return false;
	}
	return true;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::operator==(SimpleArray<T>& spa)
{
	return blCompare(spa);
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::operator!=(SimpleArray<T>& spa)
{
	return !blCompare(spa);
}

//-------------------------------------------------------------------
template<class T> bool SimpleArray<T>::blIsEmpty() const
{
	return (GetUpperBound() < 0) ? true : false;
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::Exchange(int nIndex1, int nIndex2)
{
	T temp(GetRefAt(nIndex2));
	GetRefAt(nIndex2) = GetRefAt(nIndex1);
	GetRefAt(nIndex1) = temp;
}

//-------------------------------------------------------------------
template<class T> bool SimpleArray<T>::Adopt(T* ptArray, int upbound, int size)
{
#ifdef _DEBUG
	if (ptArray == NULL || upbound<0 || size<=0 || upbound >= size)
		return false;
#endif
	if (m_pT != NULL)
		delete [] m_pT;
	m_pT = ptArray;
	m_nSize = size;
	m_nUpperBound = upbound;
	return true;
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::SetUpperBound(int upbnd)
{
	if (upbnd < m_nSize)
		m_nUpperBound = upbnd;
}

//-------------------------------------------------------------------
template<class T> bool SimpleArray<T>::AppendArray(T* pSrc, int srclen)
{
	if (srclen <= 0)
		return false;

	if (m_nUpperBound + 1 + srclen > m_nSize)
	{
		// Not enough space, so get some.
		if (!AddSpace(srclen))
			return false;
	}
	// Enough space to append without growing. Copy the data.
	for (int i = 0; i < srclen; i++ )
	{
		m_pT[m_nUpperBound + 1 + i] = pSrc[i];
	}
	m_nUpperBound += srclen;
	return true;
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::ExpandToSize()
{
	m_nUpperBound = m_nSize - 1;
}

//-------------------------------------------------------------------
template<class T> bool SimpleArray<T>::CopyFrom(int index, const T* pSrc, int srclen)
{
	if (m_nSize - index >= srclen)
	{
		// Enough space to copy into.
		for (int i = 0; i < srclen; i++)
			m_pT[index + i] = pSrc[i];
		if (index + srclen - 1 > m_nUpperBound)
			m_nUpperBound = index + srclen - 1;
		return true;
	}
	else
		return false;
}

//-------------------------------------------------------------------
template<class T> bool SimpleArray<T>::Replace(int ToReplaceIndex,
		int ToReplaceLength, const T* pReplaceWith, int ReplaceWithLength)
{
	if (m_pT != NULL && ToReplaceLength > 0)
	{
		// Number of elements from start to end of array large enough for request?
		if (m_nUpperBound - ToReplaceIndex + 1 >= ToReplaceLength)
		{
			if (ToReplaceLength < ReplaceWithLength)
			{
				int i;
				T dummy;

				// Next line might cause problems if used with non-pure-binary
				// objects.
				dummy = 0;

				InsertAtGrow(ToReplaceIndex, dummy, ReplaceWithLength - ToReplaceLength);
				for (i = 0; i < ReplaceWithLength; i++)
				{
					m_pT[ToReplaceIndex + i] = pReplaceWith[i];
				}
				return true;
			}
			else if (ToReplaceLength == ReplaceWithLength)
			{
				for (int i = 0; i < ReplaceWithLength; i++)
				{
					m_pT[ToReplaceIndex + i] = pReplaceWith[i];
				}
				return true;
			}
			else // if( ToReplaceLength > ReplaceWithLength )
			{
				for (int i = 0; i < ReplaceWithLength; i++ )
				{
					m_pT[ToReplaceIndex + i] = pReplaceWith[i];
				}

				RemoveAt(ToReplaceIndex + ReplaceWithLength, ToReplaceLength - ReplaceWithLength);
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}
