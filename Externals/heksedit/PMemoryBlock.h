#ifndef PMemoryBlock_h
#define PMemoryBlock_h

class PMemoryBlock
{
public:
	PMemoryBlock();
	PMemoryBlock( DWORD dwSize, LPBYTE lpbSource = NULL, DWORD dwPadBytes = 0 );
	PMemoryBlock( const PMemoryBlock& objectSrc );
	PMemoryBlock& operator=( const PMemoryBlock& objectSrc );
	virtual BOOL Create( DWORD dwSize, LPBYTE lpbSource = NULL, DWORD dwPadBytes = 0 );
	virtual BOOL CreateAligned( DWORD dwUnalignedSize, DWORD dwAlignment, LPBYTE lpbSource = NULL, DWORD dwPadBytes = 0 );
	virtual void Delete();
	virtual ~PMemoryBlock();
	DWORD GetObjectSize();
	LPBYTE GetObjectMemory();

protected:
	LPBYTE m_lpbMemory;
	DWORD m_dwSize;
};

#endif // PMemoryBlock_h
