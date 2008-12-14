/** 
 * @file  Simparr.h
 *
 * @brief Declaration file for SimpleArray and SimpleString classes.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: Simparr.h 104 2008-11-05 22:43:37Z kimmov $

#ifndef simplearr_h
#define simplearr_h

#define ARR_EMPTY -1

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

	int InsertAt(int nIndex, T argT, int nCount = 1);
	void InsertAtRef(int nIndex, const T& argT, int nCount = 1);
	int InsertAtGrow(int nIndex, T argT, int nCount = 1);
	void InsertAtGrowRef(int nIndex, const T& argT, int nCount = 1);
	int InsertAtGrow (int nIndex, const T* pT, int nSrcIndex, int nCount);
	int RemoveAt(int nIndex, int nCount = 1);
	void SetAtGrow(int nIndex, T argT);
	int SetAtGrowRef(int nIndex, const T& argT);
	void SetAt(int nIndex, T argT);
	void SetAtRef(int nIndex, const T& argT);
	T GetAt(int nIndex) const;
	T& GetRefAt(int nIndex) const;
	int GetSize() const;
	int GetUpperBound() const;
	int GetLength() const;
	int GetGrowBy() const;
	int SetSize(int nNewSize, int nGrowBy = 0);
	void SetGrowBy(int nGrowBy);
	//T& operator[](int nIndex) {return m_pT[nIndex];}
	SimpleArray<T>& operator=(SimpleArray<T>& spa);
	void ClearAll();
	int blContainsRef(const T& argT);
	int blContains(T argT);
	int nContainsAt(T argT);
	int blIsEmpty() const;
	void AppendRef(const T& argT);
	void Append(T argT);
	void Exchange(int nIndex1, int nIndex2);
	int blCompare(SimpleArray<T>& spa) const;
	int operator==(SimpleArray<T>& spa);
	int operator!=(SimpleArray<T>& spa);
	int Adopt(T* ptArray, int upbound, int size);
	void SetUpperBound(int upbnd);
	int AppendArray(const T* pSrc, int srclen);
	int ExpandToSize();
	int CopyFrom(int index, const T* pSrc, int srclen);
	int Replace(int ToReplaceIndex, int ToReplaceLength, const T* pReplaceWith, int ReplaceWithLength);

protected:
	int AddSpace(int nExtend);
	T* m_pT;
	int m_nSize;
	int m_nUpperBound;
	int m_nGrowBy;
};

// The template implementation methods must be included to the header file.
// Otherwise there will be link errors.
#include "Simparr_imp.h"

// A string class.
class SimpleString : public SimpleArray<char>
{
public:
	SimpleString operator+( const SimpleString& str1 );
	SimpleString();
	SimpleString( const char* ps );

	int IsEmpty() const;
	int StrLen() const;
	int AppendString( const char* ps );
	int SetToString( const char* ps );
	void Clear();

	SimpleString& operator=( const char* ps );
	SimpleString& operator=( const SimpleString &str );
	SimpleString& operator+=( const char* ps );
};

SimpleString operator+( const SimpleString &ps1, const char* ps2 );
SimpleString operator+( const char* ps1, const SimpleString &ps2 );

#endif // simplearr_h
