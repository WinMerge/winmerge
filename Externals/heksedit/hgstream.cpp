#include "precomp.h"
#include "hgstream.h"

HGlobalStream::HGlobalStream(DWORD blcksz)
{
	_hex = _nbsp = _escfilt = 0;
	precision = 0;
	m_hGlobal = NULL;
	m_dwLen = m_dwSize = 0;
	m_dwBlockSize = blcksz;
}

HGlobalStream::~HGlobalStream()
{
	if (m_hGlobal) GlobalFree( m_hGlobal );
}

HGlobalStream& HGlobalStream::operator << (const char *pszSource)
{
	if( !_nbsp && !_escfilt )
		Realloc( strlen(pszSource), (void*)pszSource );
	else
		filter( pszSource );
	_hex = _nbsp = _escfilt = 0;
	return *this;
}

HGlobalStream& HGlobalStream::operator << (BYTE c)
{
	if (_hex)
	{
		BYTE nib[2] = { c >> 4 & 0xf, c & 0xf };
		nib[0] += nib[0] >= 0xa ? 'a' - 0xa : '0';
		nib[1] += nib[1] >= 0xa ? 'a' - 0xa : '0';
		Realloc(sizeof nib, nib);
	}
	else
	{
		BYTE e[2] = { '\\', c };
		DWORD size = 1;
		switch (c)
		{
		case ' ': 
			if (_nbsp)
			{
				e[1] = '~';
				size = 2;
			}
			break;
		case '\\': case '{': case '}':
			if (_escfilt)
			{
				size = 2;
			}
			break;
		}
		Realloc(size, e + sizeof e - size);
	}
	_hex = _nbsp = _escfilt = 0;
	return *this;
}

HGlobalStream& HGlobalStream::operator << (DWORD i)
{
	//Maximum size of an integer in hex is 8
	//Maximum size for an unsigned int is the length of 4294967295 (10)
	//+1 for the \0
#if( UINT_MAX>0xffffffff )
#error The buffer below needs increasing
#endif
	char integer[11];
	if (precision)
		precision = sprintf(integer, _hex ? "%*.*x" : "%*.*u", precision, precision, i);
	else
		precision = sprintf(integer, _hex? "%x" : "%u", i);
	Realloc(precision, integer);
	_hex = _nbsp = _escfilt = 0;
	precision = 0;
	return *this;
}

HGlobalStream& HGlobalStream::operator << (int i)
{
	//Maximum size of an integer in hex is 8
	//Maximum size for an int is the length of -2147483647 (11)
	//+1 for the \0
#if( UINT_MAX>0xffffffff )
#error The buffer below needs increasing
#endif
	char integer[12];
	Realloc(sprintf(integer, _hex ? "%x" : "%d", i), integer);
	_hex = _nbsp = _escfilt = 0;
	return *this;
}

char *HGlobalStream::Extend(DWORD len)
{
	char *pTemp = 0;
	DWORD newlen = m_dwLen + len;
	DWORD newsize = (newlen / m_dwBlockSize + 1) * m_dwBlockSize;
	HGLOBAL hgTemp = m_hGlobal;
	if (newsize > m_dwSize)
	{
		if (hgTemp)
			hgTemp = GlobalReAlloc(hgTemp, newsize, GHND|GMEM_DDESHARE);
		else
			hgTemp = GlobalAlloc(GHND|GMEM_DDESHARE, newsize);
	}
	if (hgTemp)
	{
		pTemp = (char *)GlobalLock(hgTemp) + m_dwLen;
		m_dwLen = newlen;
		m_hGlobal = hgTemp;
		m_dwSize = newsize;
	}
	return pTemp;
}

void HGlobalStream::Realloc(DWORD len, void *src)
{
	if (char *pTemp = Extend(len))
	{
		if (src)
			memcpy(pTemp, src, len);
		GlobalUnlock(m_hGlobal);
	}
}

void HGlobalStream::filter(const char *src)
{
	if (src == 0)
		return;
	DWORD i = 0;
	DWORD len = 0;
	//Find out the length
	if( _nbsp && _escfilt ){
		for(; src[i]!='\0'; i++){
			switch(src[i]){ case '\\': case '{': case '}': case ' ': len++; break; }
			len++;
		}
	} else if( _escfilt ){
		for(; src[i]!='\0'; i++){
			switch(src[i]){ case '\\': case '{': case '}': len++; break; }
			len++;
		}
	} else if( _nbsp ){
		for(; src[i]!='\0'; i++){
			switch(src[i]){ case ' ': len++; break; }
			len++;
		}
	} else return;

	if (char *pTemp = Extend(len))
	{
		DWORD ii = i = 0;
		//Filter the data
		if( _nbsp && _escfilt ){
			char c;
			for(; src[i]!='\0'; i++){
				switch(src[i]){
					case '\\': case '{': case '}': case ' ':
					pTemp[ii++] = '\\';
				}
				c = src[i];
				if( src[i] == ' ') c = '~';
				pTemp[ii++] = c;
			}
		} else if( _escfilt ){
			for(; src[i]!='\0'; i++){
				switch(src[i]){
					case '\\': case '{': case '}':
					pTemp[ii++] = '\\';
				}
				pTemp[ii++] = src[i];
			}
		} else if( _nbsp ){
			for(; src[i]!='\0'; i++){
				if( src[i] == ' '){
					pTemp[ii++] = '\\';
					pTemp[ii++] = '~';
				} else pTemp[ii++] = src[i];
			}
		}
		GlobalUnlock( m_hGlobal );
	}
	_hex = _nbsp = _escfilt = 0;
}

HGLOBAL HGlobalStream::Relinquish()
{
	HGLOBAL ret = m_hGlobal;
	m_hGlobal = NULL;
	m_dwLen = m_dwSize = 0;
	return ret;
}

void HGlobalStream::Reset()
{
	if (HGLOBAL hGlobal = Relinquish())
		GlobalFree(hGlobal);
}
