/**
 *  @file VersionInfo.h
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
	CVersionInfo(const tchar_t* szFileToVersion,
				   bool bDllVersion);
	CVersionInfo(const tchar_t* szFileToVersion = nullptr,
				   const tchar_t* szLanguage = nullptr,
				   const tchar_t* szCodepage = nullptr);
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
	void QueryValue(const tchar_t* szId, String& s);
	bool GetCodepageForLanguage(WORD wLanguage, WORD & wCodePage);
};

/** 
 * @brief Return file version string.
 * @return File version as string.
 */
inline String CVersionInfo::GetFileVersion() const
{
	return m_strFileVersion;
}

/** 
 * @brief Return private build value.
 * @return Private build number as string.
 */
inline String CVersionInfo::GetPrivateBuild() const
{
	return m_strPrivateBuild;
}

/** 
 * @brief Return special build value.
 * @return Special build number as string.
 */
inline String CVersionInfo::GetSpecialBuild() const
{
	return m_strSpecialBuild;
}

/** 
 * @brief Return company name.
 * @return Company name.
 */
inline String CVersionInfo::GetCompanyName() const
{
	return m_strCompanyName;
}

/** 
 * @brief Return file description string.
 * @return File description string.
 */
inline String CVersionInfo::GetFileDescription() const
{
	return m_strFileDescription;
}

/** 
 * @brief Return internal name.
 * @return Internal name.
 */
inline String CVersionInfo::GetInternalName() const
{
	return m_strInternalName;
}

/** 
 * @brief Return full file name.
 * @return full file name.
 */
inline String CVersionInfo::GetFullFileName() const
{
	return m_strFileName;
}

/** 
 * @brief Return copyright info.
 * @return Copyright info.
 */
inline String CVersionInfo::GetLegalCopyright() const
{
	return m_strLegalCopyright;
}

/** 
 * @brief Return original filename.
 * @return Original filename.
 */
inline String CVersionInfo::GetOriginalFilename() const
{
	return m_strOriginalFilename;
}

/** 
 * @brief Return product's version number.
 * @return Product's version number as string.
 */
inline String CVersionInfo::GetProductVersion() const
{
	return m_strProductVersion;
}

/** 
 * @brief Return comment string.
 * @return Comment string.
 */
inline String CVersionInfo::GetComments() const
{
	return m_strComments;
}

