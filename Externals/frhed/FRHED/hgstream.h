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
 * @file  hgstream.h
 *
 * @brief Declaration of HGlobalStream class.
 *
 */
#ifndef _HG_STREAM_H_
#define _HG_STREAM_H_

/**
 * @brief a streaming class using global memory.
 * Returns a handle to a global memory pointer Caller is responsible for
 * passing the buck or GlobalFree'ing the return value
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
	void Realloc(SIZE_T len, void *src);
	void filter(const char *src);
	HGLOBAL Relinquish();
	void Reset();
	inline HGlobalStream& HGlobalStream::operator<<(HGlobalStream&(*_f)(HGlobalStream&))
	{
		(*_f)(*this);
		return *this;
	}
	//Manipulators
	inline friend HGlobalStream& hex(HGlobalStream& s)
	{
		s._hex = 1;
		return s;
	}
	inline friend HGlobalStream& nbsp(HGlobalStream& s)
	{
		s._nbsp = 1;
		return s;
	}
	inline friend HGlobalStream& escapefilter(HGlobalStream& s)
	{
		s._escfilt = 1;
		return s;
	}
	DWORD precision;
	SIZE_T m_dwLen;
	SIZE_T m_dwSize;
	DWORD m_dwBlockSize;
	HGLOBAL m_hGlobal;

private:
	char *Extend(SIZE_T len);
	unsigned _hex:1;
	unsigned _nbsp:1;
	unsigned _escfilt:1;
};

#endif // _HG_STREAM_H_
