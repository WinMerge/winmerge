#ifndef gtools_h
#define gtools_h

#define THIS_BASED_ADDRESS(__MEMBERVARIABLE__) \
	(LPBYTE)(((LPBYTE)(&(__MEMBERVARIABLE__)))-((LPBYTE)this))

#define ENUMERATE(l,c,o) for(c* o=(c*)((PList*)(l))->m_pHead;o;o=(c*)((PNode*)o)->m_pNext)
#define ENUMERATE_BACKWARDS(l,c,o) for(c* o=(c*)((PList*)(l))->m_pTail;o;o=(c*)((PNode*)o)->m_pPrevious )
#define ENUMERATE_POINTER(l,c,o) for(o=(c*)((PList*)(l))->m_pHead;o;o=(c*)((PNode*)o)->m_pNext)
#define ENUMERATE_POINTER_BACKWARDS(l,c,o) for(o=(c*)((PList*)(l))->m_pTail;o;o=(c*)((PNode*)o)->m_pPrevious)

class PNode
{
public:
	PNode(): m_pNext(0), m_pPrevious(0) { }

	virtual ~PNode() { }

	PNode* m_pNext;
	PNode* m_pPrevious;
};

typedef unsigned long ULONG;

class PList : public PNode
{
public:
	PList();
	virtual ~PList();
	virtual bool DeleteContents();
	virtual bool RemoveContents();
	PNode* Find( long lZeroBasedIndex );
	bool Contains(PNode* pNode);
	bool AddHead(PNode* pNode);
	bool AddTail(PNode* pNode);
	void Remove(PNode* pNode);
	void Delete(PNode* pNode);
	bool InsertBefore(PNode *pInsert,PNode* pBeforeThis);
	bool InsertAfter(PNode* pInsert, PNode* pAfterThis);
	bool Swap(PNode* pNodeA,PNode* pNodeB);
	bool Move(PNode* pNodeA,PNode* pNodeB);
	bool IsEmpty();
	bool Merge( PList& objectSource );

	PNode* m_pHead;
	PNode* m_pTail;
	long m_lCount;
};

class PString : public PNode
{
public:
	PString();
	PString( const char* szFormat );
	PString( int, const char* szFormat, ... );
	PString( const PString& objectSrc );
	virtual ~PString();
	void sprintf( const char* szFormat, ... );
	void vsprintf( const char* szFormat, va_list args );
	PString& operator=( const char* objectSrc );
	PString& operator=( PString& objectSrc );

	void Append( LPCSTR pString, DWORD dwSize );

	inline operator char*()
	{
		return m_lpszData;
	}

	/*inline operator const char*() const
	{
		return m_lpszData;
	}*/

	inline int GetLength()
	{
		return m_iStringLength;
	}

protected:
	void DeleteStringData();
	void CopyStringData(const char* lpszFrom);

	int m_iStringLength;
	char* m_lpszData;
	char m_szFixedBuffer[256];
};

#define IsEmptyString(x) (!(x)||!*(x))
PString GetLastErrorString();


void TRACE(const char* pszFormat,...);


#endif // gtools_h

