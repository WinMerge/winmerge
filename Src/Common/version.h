/**
 *  @file version.h
 *
 *  @brief Declaration of CVersionInfo class
 */ 
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef VERSIONTOOLS_H
#define VERSIONTOOLS_H

#include <shlwapi.h>

/**
 * @brief Class providing access to version information of a file.
 * This class reads version information block from a file. Version information
 * consists of version numbers, copyright, descriptions etc. Since that is
 * many strings to read, there is constructor taking BOOL parameter and
 * only reading version numbers. That constructor is suggested to be used
 * if string information is not needed.
 */
class CVersionInfo
{
private:
	VS_FIXEDFILEINFO m_FixedFileInfo; /**< Fixed file information */
	DWORD m_dwVerInfoSize; /**< Size of version information block (0 if missing) */
	BYTE * m_pVffInfo; /**< Pointer to version information block */
	BOOL m_bVersionOnly; /**< Ask version numbers only */
	BOOL m_bDllVersion;
	WORD m_wLanguage; /**< Language-ID to use (if given) */

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

public:
	CVersionInfo(BOOL bVersionOnly);
	CVersionInfo(WORD wLanguage);
	CVersionInfo(LPCTSTR szFileToVersion,
				   BOOL bDllVersion);
	CVersionInfo(LPCTSTR szFileToVersion = NULL,
				   LPCTSTR szLanguage = NULL,
				   LPCTSTR szCodepage = NULL);
	CVersionInfo(HINSTANCE hModule);
	CString GetFileVersion() const;
	CString GetCompanyName() const;
	CString GetFileDescription() const;
	CString GetInternalName() const;
	CString GetLegalCopyright() const;
	CString GetOriginalFilename() const;
	CString GetProductVersion() const;
	CString GetComments() const;
	CString GetSpecialBuild() const;
	CString GetPrivateBuild() const;
	CString GetFixedProductVersion();
	CString GetFixedFileVersion();
	DLLVERSIONINFO m_dvi;

protected:
	void GetVersionInfo();
	void GetFixedVersionInfo();
	void QueryStrings();
	void QueryValue(LPCTSTR szId, CString& s);
	BOOL GetCodepageForLanguage(WORD wLanguage, WORD & wCodePage);
};


#endif

/*******************************************************************************
       END OF FILE
*******************************************************************************/

