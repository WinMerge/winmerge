/* textfile.h
*/  
#ifndef TEXTFILE_H
#define TEXTFILE_H

#define TEXTFILE_BUFSIZE	1024

#define XML_HEADER	_T("<?xml version=\"1.0\" ?>") 
#define XML_DOCTYPE( name ) 	_T("<!DOCTYPE ") _T(#name) _T(" SYSTEM \"")  _T(#name) _T(".dtd\">\r\n")

#define XML_fmt( name, type )  _T("<") _T(#name) _T(">%") _T(#type)  _T("</") _T(#name)  _T(">\r\n")
#define XML_FMT1( name, type )  _T("\t") XML_fmt(name, type)
#define XML_FMT2( name, type )  _T("\t\t") XML_fmt(name, type)
#define XML_FMT3( name, type )  _T("\t\t\t") XML_fmt(name, type)
#define XML_FMT4( name, type )  _T("\t\t\t\t") XML_fmt(name, type)
#define XML_FMT5( name, type )  _T("\t\t\t\t\t") XML_fmt(name, type)
#define XML_FMT6( name, type )  _T("\t\t\t\t\t\t") XML_fmt(name, type)
#define XML_FMT7( name, type )  _T("\t\t\t\t\t\t\t") XML_fmt(name, type)
#define XML_FMT8( name, type )  _T("\t\t\t\t\t\t\t\t") XML_fmt(name, type)

#define XML_BEGIN0(name) _T("<") _T(#name) _T(">\r\n")
#define XML_BEGIN1(name) _T("\t<") _T(#name) _T(">\r\n")
#define XML_BEGIN2(name) _T("\t\t<") _T(#name) _T(">\r\n")
#define XML_BEGIN3(name) _T("\t\t\t<") _T(#name) _T(">\r\n")
#define XML_BEGIN4(name) _T("\t\t\t\t<") _T(#name) _T(">\r\n")
#define XML_BEGIN5(name) _T("\t\t\t\t\t<") _T(#name) _T(">\r\n")
#define XML_BEGIN6(name) _T("\t\t\t\t\t\t<") _T(#name) _T(">\r\n")
#define XML_BEGIN7(name) _T("\t\t\t\t\t\t\t<") _T(#name) _T(">\r\n")
#define XML_BEGIN8(name) _T("\t\t\t\t\t\t\t\t<") _T(#name) _T(">\r\n")
#define XML_END0(name) _T("</") _T(#name) _T(">\r\n")
#define XML_END1(name) _T("\t</") _T(#name) _T(">\r\n")
#define XML_END2(name) _T("\t\t</") _T(#name) _T(">\r\n")
#define XML_END3(name) _T("\t\t\t</") _T(#name) _T(">\r\n")
#define XML_END4(name) _T("\t\t\t\t</") _T(#name) _T(">\r\n")
#define XML_END5(name) _T("\t\t\t\t\t</") _T(#name) _T(">\r\n")
#define XML_END6(name) _T("\t\t\t\t\t\t</") _T(#name) _T(">\r\n")
#define XML_END7(name) _T("\t\t\t\t\t\t\t</") _T(#name) _T(">\r\n")
#define XML_END8(name) _T("\t\t\t\t\t\t\t\t</") _T(#name) _T(">\r\n")

#define XML_BOOL1(bVal, name) ((bVal)? (_T("\t<") _T(#name) _T("/>\r\n")):_T(""))
#define XML_BOOL2(bVal, name) ((bVal)? (_T("\t\t<") _T(#name) _T("/>\r\n")):_T(""))
#define XML_BOOL3(bVal, name) ((bVal)? (_T("\t\t\t<") _T(#name) _T("/>\r\n")):_T(""))
#define XML_BOOL4(bVal, name) ((bVal)? (_T("\t\t\t\t<") _T(#name) _T("/>\r\n")):_T(""))
#define XML_BOOL5(bVal, name) ((bVal)? (_T("\t\t\t\t\t<") _T(#name) _T("/>\r\n")):_T(""))
#define XML_BOOL6(bVal, name) ((bVal)? (_T("\t\t\t\t\t\t<") _T(#name) _T("/>\r\n")):_T(""))
#define XML_BOOL7(bVal, name) ((bVal)? (_T("\t\t\t\t\t\t\t<") _T(#name) _T("/>\r\n")):_T(""))
#define XML_BOOL8(bVal, name) ((bVal)? (_T("\t\t\t\t\t\t\t\t<") _T(#name) _T("/>\r\n")):_T(""))

class CTextFile : public CFile
{
  TCHAR *m_pBuf, *m_endBuf, m_buf[TEXTFILE_BUFSIZE], m_bufParse[TEXTFILE_BUFSIZE];
  int m_cntBuf;
  
  void Reset();
  
public:  
	BOOL IsXMLDocument(LPCTSTR pszVersion = _T("1.0"));
	BOOL LocateXMLRootObject(LPCTSTR pszRoot);
	CTextFile();
	int ReadLine(LPTSTR buf, int max); 
	int ReadToken(LPTSTR buf, int max);
	int ReadNextHTMLTag(LPTSTR buf, int max); 
	int ReadToNextHTMLTag(LPTSTR buf, int max);
	CString FindString(LPCTSTR szStringToFind, UINT nFrom = CFile::begin, DWORD dwOffset = 0);
	CString FindDSCCommentString(LPCTSTR szStringToFind, LPCTSTR pszEndOn =_T("%%EndComments"), UINT nFrom = CFile::begin, DWORD dwOffset = 0);
	BOOL IsOpen() const { return m_hFile != CFile::hFileNull; }
	virtual LONG Seek( LONG lOff, UINT nFrom );
	void SeekToBegin( );
	void SeekToEnd( );
	virtual DWORD GetPosition( ) const;
	
	virtual BOOL ParseXMLValue(LPTSTR pszResult, DWORD dwMax, LPCTSTR pszEndTag);

	template <class T> BOOL ParseXMLValue(T& value, LPCTSTR pszEndTag)
	{
		LONG val;
		BOOL bResult = ParseXMLValue(val, pszEndTag);
		value=(T)val;
		return bResult;
	}
	template <> BOOL ParseXMLValue(CString& strResult, LPCTSTR pszEndTag)
	{
		ReadToNextHTMLTag(m_bufParse, TEXTFILE_BUFSIZE);
		CString s = m_bufParse;
		if (ReadNextHTMLTag(m_bufParse, TEXTFILE_BUFSIZE)
			&& !_tcscmp(m_bufParse, pszEndTag))
		{
			strResult = s;
			return TRUE;
		}
		strResult = _T("");
		return FALSE;
	}
	
	template <> BOOL ParseXMLValue(float& fResult, LPCTSTR pszEndTag)
	{
		if (ReadToNextHTMLTag(m_bufParse, TEXTFILE_BUFSIZE))
		{
			float f = (float)_tcstod(m_bufParse, NULL);
			if (ReadNextHTMLTag(m_bufParse, TEXTFILE_BUFSIZE)
				&& !_tcscmp(m_bufParse, pszEndTag))
			{
				fResult = f;
				return TRUE;
			}
		}
		fResult=0.f;
		return FALSE;
	}
	
	template <> BOOL ParseXMLValue(LONG& lResult, LPCTSTR pszEndTag)
	{
		if (ReadToNextHTMLTag(m_bufParse, TEXTFILE_BUFSIZE))
		{
			LONG i = _ttol(m_bufParse);
			if (ReadNextHTMLTag(m_bufParse, TEXTFILE_BUFSIZE)
				&& !_tcscmp(m_bufParse, pszEndTag))
			{
				lResult=i;
				return TRUE;
			}
		}
		lResult=0L;
		return FALSE;
	}



};


#endif
