/*******************************************************************************
FILE: version.h

*******************************************************************************/
#ifndef VERSIONTOOLS_H
#define VERSIONTOOLS_H

class CVersionInfo
{
	LPTSTR   m_lpstrVffInfo;
	BOOL m_bQueryDone;
	VS_FIXEDFILEINFO m_FixedFileInfo;

	CString m_strFileName;
	CString m_strLanguage;
	CString m_strCodepage;
	CString m_strCompanyName;
	CString m_strFileDescription;
	CString m_strFileVersion; 
	CString m_strInternalName; 
	CString m_strLegalCopyright; 
	CString m_strOriginalFilename; 
	CString m_strProductName; 
	CString m_strProductVersion; 
	CString m_strComments;
	CString m_strSpecialBuild;
	CString m_strPrivateBuild;
	void Init();

public:
	CVersionInfo(LPCTSTR szFileToVersion = NULL, 
				   LPCTSTR szLanguage = NULL,
				   LPCTSTR szCodepage = NULL);
	CVersionInfo(HINSTANCE hModule);
	CString GetFileVersion();
	CString GetCompanyName();
	CString GetFileDescription();
	CString GetInternalName();
	CString GetLegalCopyright();
	CString GetOriginalFilename();
	CString GetProductVersion();
	CString GetComments();
	CString GetSpecialBuild();
	CString GetPrivateBuild();
	CString GetFixedProductVersion();
protected:
	void GetVersionInfo();
	void GetFixedVersionInfo();
	void QueryValue(LPCTSTR szId, CString& s);
};


#endif

/*******************************************************************************
       END OF FILE
*******************************************************************************/

