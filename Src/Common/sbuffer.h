/** 
 * @file  sbuffer.h
 *
 * @brief Declaration file for sbuffer class
 *
 * @date  Created: 2003-10-09 (Perry Rapp)
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef sbuffer_h_included
#define sbuffer_h_included


/**
 * @brief Primitive string that grows with exponential allocation
 */
class sbuffer
{
public:
	sbuffer(UINT init)
		{
			m_size = init;
			m_buff = new TCHAR[m_size];
			m_len = 0;
		}
	~sbuffer()
		{
			delete m_buff;
			m_buff = 0;
		}
	void Append(LPCTSTR sz)
		{
			CString str = sz;
			Append(str);
		}
	void Append(const CString & s)
		{
			if (s.GetLength() + m_len > m_size-1)
			{
				do
				{
					m_size = m_size * 2;
				}
				while (s.GetLength() + m_len > m_size-1);
				TCHAR * newbuff = new TCHAR[m_size];
				_tcscpy(newbuff, m_buff);
				delete[] m_buff;
				m_buff = newbuff;
			}
			_tcscpy(&m_buff[m_len], s);
			m_len += s.GetLength();
		}
	void Clear() { m_buff[0] = 0; m_len = 0; }
	void Set(LPCTSTR sztext)
		{
			Clear();
			Append(sztext);
		}
	BOOL StartsWithInsensitive(const CString & prefix)
		{
			return (0 == _tcsnicmp(GetData(), prefix, prefix.GetLength()));
		}
	CString Left(int len)
		{
			CString s;
			LPTSTR sz = s.GetBuffer(len+1);
			_tcsncpy(sz, GetData(), len);
			sz[len] = 0;
			s.ReleaseBuffer();
			return s;
		}
	// get data length
	int GetLength() const { return m_len; }

	// TRUE if zero length
	BOOL IsEmpty() const { return m_len==0; }
	
	LPCTSTR GetData() const { return m_buff; }

// Implementation data
private:
	TCHAR * m_buff;
	UINT m_len;
	UINT m_size;
};

#endif // sbuffer_h_included
