/**
 *  @file version.h
 *
 *  @brief Declaration of CVersionInfo class
 */ 
#pragma once

#include <shlwapi.h>
#include <memory>
#include "UnicodeString.h"

/**
 * @brief Class providing access to version information of a file.
 * This class reads version information block from a file. Version information
 * consists of version numbers, copyright, descriptions etc. Since that is
 * many strings to read, there is constructor taking `bool` parameter and
 * only reading version numbers. That constructor is suggested to be used
 * if string information is not needed.
 */
class CVersionInfo
{
private:
	VS_FIXEDFILEINFO m_FixedFileInfo; /**< Fixed file information */
	std::unique_ptr<BYTE[]> m_pVffInfo; /**< Pointer to version information block */
	bool m_bVersionOnly; /**< Ask version numbers only */
	bool m_bDllVersion; /**< Dll file version is being queried */
	WORD m_wLanguage; /**< Language-ID to use (if given) */
	bool m_bVersionFound; /**< Was version info found from file? */

	String m_strFileName;
	String m_strLanguage;
	String m_strCodepage;
	String m_strCompanyName;
	String m_strFileDescription;
	String m_strFileVersion;
	String m_strInternalName; 
	String m_strLegalCopyright; 
	String m_strOriginalFilename; 
	String m_strProductName; 
	String m_strProductVersion; 
	String m_strComments;
	String m_strSpecialBuild;
	String m_strPrivateBuild;

public:
	explicit CVersionInfo(bool bVersionOnly);
	explicit CVersionInfo(WORD wLanguage);
	CVersionInfo(LPCTSTR szFileToVersion,
				   bool bDllVersion);
	CVersionInfo(LPCTSTR szFileToVersion = nullptr,
				   LPCTSTR szLanguage = nullptr,
				   LPCTSTR szCodepage = nullptr);
	explicit CVersionInfo(HINSTANCE hModule);
	String GetFileVersion() const;
	String GetCompanyName() const;
	String GetFileDescription() const;
	String GetInternalName() const;
	String GetFullFileName() const;
	String GetLegalCopyright() const;
	String GetOriginalFilename() const;
	String GetProductVersion() const;
	String GetComments() const;
	String GetSpecialBuild() const;
	String GetPrivateBuild() const;
	String GetFixedProductVersion();
	String GetFixedFileVersion();
	DLLVERSIONINFO m_dvi;
	bool GetFixedFileVersion(unsigned& versionMS, unsigned& versionLS);

protected:
	void GetVersionInfo();
	void GetFixedVersionInfo();
	void QueryStrings();
	void QueryValue(LPCTSTR szId, String& s);
	bool GetCodepageForLanguage(WORD wLanguage, WORD & wCodePage);
};
