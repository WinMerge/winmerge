/* textfile.cpp  
   Implementation file for CTextFile class
*/       
#include "stdafx.h"
#include "textfile.h" 
#include "ctype.h"
#include "coretools.h"

 
CTextFile::CTextFile()
	: CFile()
{
 	Reset();
}
                         
                         
int CTextFile::ReadLine(LPTSTR buf, int max)
{             
	TCHAR *p, *end, *prev=NULL;   
	p=buf;
	end=buf+max-1; 
	*end = _T('\0');
	while (p < end)
	{              
		if (m_pBuf >= m_endBuf)
		{ 
			if ((m_cntBuf = Read( m_buf, sizeof m_buf)) <= 0)
				break;  
			
			m_endBuf = m_buf + m_cntBuf;
			m_pBuf = m_buf;	
		}     
		        
		*p = *m_pBuf;
		if (*p == _T('\n'))
		{
			m_pBuf = _tcsinc(m_pBuf); 
			p = _tcsinc(p); 
			break;
		}
		else if (prev && *prev == _T('\r'))
		{
			*p = _T('\n');
			p = _tcsinc(p); 
			break;
		}
		m_pBuf = _tcsinc(m_pBuf); 
		prev=p;
		p = _tcsinc(p); 
	}	
	buf[p-buf]=_T('\0');
	return (p - buf);
}      
                         
                         
int CTextFile::ReadToken(LPTSTR buf, int max)
{             
	TCHAR *p, *end;   
	BOOL init=TRUE;
	p=buf;
	end=buf+max-1; 
	*end = _T('\0');    

	while (p < end)
	{              
		if (m_pBuf >= m_endBuf)
		{ 
			if ((m_cntBuf = Read( m_buf, sizeof m_buf)) <= 0)
				break;  
			
			m_endBuf = m_buf + m_cntBuf;
			m_pBuf = m_buf;	
		}   
		
		if (init)
		{     
			if (!_istgraph(*m_pBuf))
			{
				m_pBuf = _tcsinc(m_pBuf);
				continue;
			} 
			else
				init=FALSE;
		}      
		else if (!_istgraph(*m_pBuf))
		{
			*p=_T('\0');
			break;
		}
		*p = *m_pBuf;
		m_pBuf = _tcsinc(m_pBuf);
		p = _tcsinc(p);
	}	
	return (p - buf);
}                 
                         
                         
int CTextFile::ReadNextHTMLTag(LPTSTR buf, int max)
{             
	TCHAR *p, *end;   
	BOOL init=TRUE;
	p=buf;
	end=buf+max-1; 
	*end = _T('\0');    

	while (p < end)
	{              
		if (m_pBuf >= m_endBuf)
		{ 
			if ((m_cntBuf = Read( m_buf, sizeof m_buf)) <= 0)
				break;  
			
			m_endBuf = m_buf + m_cntBuf;
			m_pBuf = m_buf;	
		}   
		
		if (init)
		{     
			if (*m_pBuf != _T('<'))
			{
				m_pBuf = _tcsinc(m_pBuf);
				continue;
			} 
			else
				init=FALSE;
		}      
		else if (*m_pBuf == _T('>'))
		{          
			*p = *m_pBuf;
			m_pBuf = _tcsinc(m_pBuf);
			p = _tcsinc(p);
			*p=_T('\0');
			break;
		}
		*p = *m_pBuf;
		m_pBuf = _tcsinc(m_pBuf);
		p = _tcsinc(p);
	}	
	return (p - buf);
}                        
                   
                         
                         
int CTextFile::ReadToNextHTMLTag(LPTSTR buf, int max)
{             
	TCHAR *p, *end; 
	p=buf;
	end=buf+max-1; 
	*end = _T('\0');    

	while (p < end)
	{              
		if (m_pBuf >= m_endBuf)
		{ 
			if ((m_cntBuf = Read( m_buf, sizeof m_buf)) <= 0)
				break;  
			
			m_endBuf = m_buf + m_cntBuf;
			m_pBuf = m_buf;	
		}   
		
		if (*m_pBuf == _T('<')) // || *m_pBuf == '\r' || *m_pBuf == '\n')
		{
			*p = _T('\0');
			break;
		}    
		*p = *m_pBuf;
		m_pBuf = _tcsinc(m_pBuf);
		p = _tcsinc(p);
	}	
	return (p - buf);
}                        

         
CString CTextFile::FindString(LPCTSTR szStringToFind, UINT nFrom /*= CFile::begin*/, DWORD dwOffset /*= 0*/)
{
	if (dwOffset == 0 && nFrom == CFile::end)
		SeekToEnd();
	else
		Seek(dwOffset, nFrom);

	while (ReadLine(m_bufParse, (sizeof m_bufParse)-1))
	{
		m_bufParse[(sizeof m_bufParse)-1] = NULL;
		if (_tcsstr(m_bufParse, szStringToFind))
		{
			return CString(m_bufParse);
		}
	}
	return CString(_T(""));
}                       

         
CString CTextFile::FindDSCCommentString(LPCTSTR szStringToFind, LPCTSTR pszEndOn /*=_T("%%EndComments")*/, UINT nFrom /*= CFile::begin*/, DWORD dwOffset /*= 0*/)
{
	if (dwOffset == 0 && nFrom == CFile::end)
		SeekToEnd();
	else
		Seek(dwOffset, nFrom);

	while (ReadLine(m_bufParse, (sizeof m_bufParse)-1))
	{
		m_bufParse[(sizeof m_bufParse)-1] = NULL;
		if (_tcsstr(m_bufParse, szStringToFind))
		{
			return CString(m_bufParse);
		}
		else if (nFrom == CFile::begin
			&& _tcsstr(m_bufParse, pszEndOn))
		{
			break;
		}
	}
	return CString(_T(""));
}

DWORD CTextFile::GetPosition() const
{
	DWORD pos = CFile::GetPosition();
	pos -= tcssubptr(m_pBuf, m_endBuf);
	return pos;
}

BOOL CTextFile::ParseXMLValue(LPTSTR pszResult, DWORD dwMax, LPCTSTR pszEndTag)
{
	CString s;
	BOOL bResult = ParseXMLValue((CString&)s, pszEndTag);
	_tcsncpy(pszResult, s, dwMax);
	if (dwMax > 0)
		pszResult[dwMax-1] = _T('\0');
	return bResult;
}


BOOL CTextFile::LocateXMLRootObject(LPCTSTR pszRoot)
{
	if (!IsXMLDocument())
		return FALSE;
	
	// we have an XML doc, now let's look for the root object
	BOOL bFound=FALSE;
	while (ReadNextHTMLTag(m_bufParse, sizeof m_bufParse))
	{
		if (!_tcscmp(m_bufParse, pszRoot))
		{
			bFound=TRUE;
			break;
		}
	}
	
	return bFound;
}

BOOL CTextFile::IsXMLDocument(LPCTSTR /*pszVersion  = _T("1.0")*/)
{
	SeekToBegin();

	BOOL bFound=FALSE;
	while (ReadNextHTMLTag(m_bufParse, sizeof m_bufParse))
	{
		if (!_tcsnicmp(m_bufParse, _T("<?xml"), 5))
		{
			bFound=TRUE;
			break;
		}
	}
	
	return bFound;
}

void CTextFile::Reset()
{
	m_pBuf = m_endBuf = m_buf;
 	m_cntBuf = 0;
}

LONG CTextFile::Seek( LONG lOff, UINT nFrom )
{
	if (nFrom == CFile::current)
	{
		lOff += GetPosition();
		nFrom = CFile::begin;
	}
	Reset();
	return CFile::Seek(lOff, nFrom);
}

void CTextFile::SeekToBegin( )
{
	Reset();
	CFile::SeekToBegin();
}

void CTextFile::SeekToEnd( )
{
	Reset();
	CFile::SeekToEnd();
}
