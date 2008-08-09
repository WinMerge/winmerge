/*
Returns a handle to a global memory pointer
Caller is responsible for passing the buck
or GlobalFree'ing the return value
#include <strstrea.h>
#include <iomanip.h>
*/
class HGlobalStream
{
public:

	HGlobalStream(DWORD blcksz = 1024); //This block size was picked at random
	~HGlobalStream();
	HGlobalStream& operator << (const char *);
	HGlobalStream& operator << (BYTE);
	HGlobalStream& operator << (DWORD);
	HGlobalStream& operator << (int);
	void Realloc(DWORD len, void* src);
	void filter( const char* src );
	HGLOBAL Relinquish();
	void Reset();
	inline HGlobalStream& HGlobalStream::operator<<(HGlobalStream&(*_f)(HGlobalStream&))
	{
		(*_f)(*this);
		return *this;
	}
	//Manipulators
	inline friend HGlobalStream& hex( HGlobalStream& s )
	{
		s._hex = 1;
		return s;
	}
	inline friend HGlobalStream& nbsp( HGlobalStream& s )
	{
		s._nbsp = 1;
		return s;
	}
	inline friend HGlobalStream& escapefilter( HGlobalStream& s )
	{
		s._escfilt = 1;
		return s;
	}
	DWORD precision;
	DWORD m_dwLen;
	DWORD m_dwSize;
	DWORD m_dwBlockSize;
	HGLOBAL m_hGlobal;
private:
	char *Extend(DWORD len);
	unsigned _hex:1;
	unsigned _nbsp:1;
	unsigned _escfilt:1;
};
