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
 * @file  Simparr.h
 *
 * @brief Declaration file for SimpleArray and SimpleString classes.
 *
 */
#ifndef simplearr_h
#define simplearr_h

#define ARR_EMPTY -1

/**
 * A simple array class template.
 * This template offers basic array class methods.
 * @note This class is limited in 32-bit space for item count.
 */
template<class T> class SimpleArray
{
public:
	SimpleArray();
	SimpleArray(const T* ptArray, int upbound, int size);
	SimpleArray(int nNewSize, int nGrowBy = 1);
	SimpleArray(const SimpleArray& spaArg);
	virtual ~SimpleArray();

	operator T*()
	{
		return m_pT;
	}
	operator const T*() const
	{
		return m_pT;
	}

	bool InsertAt(int nIndex, T argT, int nCount = 1);
	void InsertAtRef(int nIndex, const T& argT, int nCount = 1);
	bool InsertAtGrow(int nIndex, T argT, int nCount = 1);
	bool InsertAtGrow (int nIndex, const T* pT, int nSrcIndex, int nCount);
	void InsertAtGrowRef(int nIndex, const T& argT, int nCount = 1);
	bool RemoveAt(int nIndex, int nCount = 1);
	void SetAtGrow(int nIndex, T argT);
	bool SetAtGrowRef(int nIndex, const T& argT);
	void SetAt(int nIndex, T argT);
	void SetAtRef(int nIndex, const T& argT);
	T GetAt(int nIndex) const;
	T& GetRefAt(int nIndex) const;
	int GetSize() const;
	int GetUpperBound() const;
	int GetLength() const;
	int GetGrowBy() const;
	bool SetSize(int nNewSize, int nGrowBy = 0);
	void SetGrowBy(int nGrowBy);
	T& operator[](int nIndex) {return m_pT[nIndex];}
	SimpleArray<T>& operator=(SimpleArray<T>& spa);
	void ClearAll();
	bool blContainsRef(const T& argT);
	bool blContains(T argT);
	int nContainsAt(T argT);
	bool blIsEmpty() const;
	void AppendRef(const T& argT);
	void Append(T argT);
	void Exchange(int nIndex1, int nIndex2);
	bool blCompare(SimpleArray<T>& spa) const;
	int operator==(SimpleArray<T>& spa);
	int operator!=(SimpleArray<T>& spa);
	bool Adopt(T* ptArray, int upbound, int size);
	void SetUpperBound(int upbnd);
	bool AppendArray( T* pSrc, int srclen );
	void ExpandToSize();
	bool CopyFrom(int index, const T* pSrc, int srclen);
	bool Replace(int ToReplaceIndex, int ToReplaceLength, const T* pReplaceWith, int ReplaceWithLength);

protected:
	bool AddSpace(int nExtend);
	T* m_pT;
	int m_nSize;
	int m_nUpperBound;
	int m_nGrowBy;
};

// The template implementation methods must be included to the header file.
// Otherwise there will be link errors.
#include "Simparr_imp.h"

/**
 * @brief A string class.
 */
class SimpleString : public SimpleArray<CHAR>
{
public:
	SimpleString operator+(const SimpleString& str1);
	SimpleString();
	SimpleString(LPCSTR ps);

	bool IsEmpty() const;
	int StrLen() const;
	int AppendString(LPCSTR ps);
	int SetToString(LPCSTR ps);
	void Clear();

	SimpleString& operator=(LPCSTR ps);
	SimpleString& operator=(const SimpleString &str);
	SimpleString& operator+=(LPCSTR ps);
};

SimpleString operator+(const SimpleString &ps1, LPCSTR ps2);
SimpleString operator+(LPCSTR ps1, const SimpleString &ps2);

#endif // simplearr_h
