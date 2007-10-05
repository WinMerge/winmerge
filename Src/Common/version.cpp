/**
 *  @file version.cpp
 *
 *  @brief Implementation of CVersionInfo class
 */ 
// ID line follows -- this is updated by SVN
// $Id$


#include "stdafx.h"
#include "version.h"

/** 
 * @brief Structure used to store language and codepage.
 */
struct LANGUAGEANDCODEPAGE
{
	WORD wLanguage;
	WORD wCodePage;
};

/** 
 * @brief Constructor for asking only numeric version info.
 * This constructor creates instance that only reads version numbers,
 * not several strings there are. This saves some time.
 * @param [in] bVersionOnly If TRUE only version numbers are read.
 */
CVersionInfo::CVersionInfo(BOOL bVersionOnly)
: m_wLanguage(0)
, m_bVersionOnly(bVersionOnly)
, m_bDllVersion(FALSE)
{
	GetVersionInfo();
}

/** 
 * @brief Constructor for asking version infor when language is known.
 * This constructor creates instance which gets version-info matching
 * given language-ID. There can be several different language-codepage
 * combinations, but we use first version info block that matches the
 * given language. So even if codepage isn't correct, we get correct
 * version information.
 * @param [in] wLanguage Language-ID for which the version info is wanted.
 */
CVersionInfo::CVersionInfo(WORD wLanguage)
: m_wLanguage(wLanguage)
, m_bVersionOnly(FALSE)
, m_bDllVersion(FALSE)
{
	GetVersionInfo();
}

/** 
 * @brief Constructor.
 * @param [in] szFileToVersion Filename to read version from.
 * @param [in] bDllVersion If TRUE queries DLL version.
 */
CVersionInfo::CVersionInfo(LPCTSTR szFileToVersion, 
						   BOOL bDllVersion)
: m_strFileName(szFileToVersion)
, m_wLanguage(0)
, m_bVersionOnly(FALSE)
, m_bDllVersion(bDllVersion)
{
	GetVersionInfo();
}

/** 
 * @brief Constructor.
 * @param [in] szFileToVersion Filename to read version from.
 * @param [in] szLanguage Language for version.
 * @param [in] szCodePage Codepage for version.
 */
CVersionInfo::CVersionInfo(LPCTSTR szFileToVersion /* = NULL*/, 
						   LPCTSTR szLanguage /* = NULL*/,
						   LPCTSTR szCodepage /* = NULL*/)
: m_strFileName(szFileToVersion)
, m_strLanguage(szLanguage)
, m_strCodepage(szCodepage)
, m_wLanguage(0)
, m_bVersionOnly(FALSE)
, m_bDllVersion(FALSE)
{
	GetVersionInfo();
}

/** 
 * @brief Constructor for asking version number from known module.
 * @param [in] hModule Handle to module for version info.
 */
CVersionInfo::CVersionInfo(HINSTANCE hModule)
: m_wLanguage(0)
, m_bVersionOnly(FALSE)
, m_bDllVersion(FALSE)
{
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(hModule, szFileName, MAX_PATH);
	m_strFileName = szFileName;
	GetVersionInfo();
}

/** 
 * @brief Return file version string.
 * @return File version as string.
 */
CString CVersionInfo::GetFileVersion() const
{
	return m_strFileVersion;
}

/** 
 * @brief Return private build number.
 * @return Private build number as string.
 */
CString CVersionInfo::GetPrivateBuild() const
{
	return m_strPrivateBuild;
}

/** 
 * @brief Return special build number.
 * @return Special build number as string.
 */
CString CVersionInfo::GetSpecialBuild() const
{
	return m_strSpecialBuild;
}

/** 
 * @brief Return company name.
 * @return Company name.
 */
CString CVersionInfo::GetCompanyName() const
{
	return m_strCompanyName;
}

/** 
 * @brief Return file description string.
 * @return File description string.
 */
CString CVersionInfo::GetFileDescription() const
{
	return m_strFileDescription;
}

/** 
 * @brief Return internal name.
 * @return Internal name.
 */
CString CVersionInfo::GetInternalName() const
{
	return m_strInternalName;
}

/** 
 * @brief Return copyright info.
 * @return Copyright info.
 */
CString CVersionInfo::GetLegalCopyright() const
{
	return m_strLegalCopyright;
}

/** 
 * @brief Return original filename.
 * @return Original filename.
 */
CString CVersionInfo::GetOriginalFilename() const
{
	return m_strOriginalFilename;
}

/** 
 * @brief Return product's version number.
 * @return Product's version number as string.
 */
CString CVersionInfo::GetProductVersion() const
{
	return m_strProductVersion;
}

/** 
 * @brief Format version string from numbers.
 * Version number consists of four WORD (16-bit) numbers. This function
 * formats those numbers to string, where numbers are separated by
 * dots.
 * @param [in] First two (most significant) numbers for version number.
 * @param [in] Last two numbers for version number.
 * @return Formatted version string.
 */
static CString MakeVersionString(DWORD hi, DWORD lo)
{
	CString sver;
	sver.Format(_T("%d.%d.%d.%d"), HIWORD(hi), LOWORD(hi), HIWORD(lo), LOWORD(lo));
	return sver;
}

/** 
 * @brief Return numeric product's version number.
 * This function returns version number given as a number in version info.
 * @return Product's version number as string.
 */
CString CVersionInfo::GetFixedProductVersion()
{
	if (!m_bVersionFound)
		return _T("");
	return MakeVersionString(m_FixedFileInfo.dwProductVersionMS
		, m_FixedFileInfo.dwProductVersionLS);
}

/** 
 * @brief Return numeric file's version number.
 * This function returns version number given as a number in version info.
 * @return File's version number as string.
 */
CString CVersionInfo::GetFixedFileVersion()
{
	if (!m_bVersionFound)
		return _T("");
	return MakeVersionString(m_FixedFileInfo.dwFileVersionMS
		, m_FixedFileInfo.dwFileVersionLS);
}

/** 
 * @brief Return numeric file's version number.
 * This function returns version number given as two DWORDs.
 * @param [out] versionMS High DWORD for version number.
 * @param [out] versionLS Low DWORD for version number.
 * @return TRUE if version info was found, FALSE otherwise.
 */
BOOL CVersionInfo::GetFixedFileVersion(DWORD &versionMS, DWORD &versionLS)
{
	if (m_bVersionFound)
	{
		versionMS = m_FixedFileInfo.dwFileVersionMS;
		versionLS = m_FixedFileInfo.dwFileVersionLS;
		return TRUE;
	}
	return FALSE;
}

/** 
 * @brief Return comment string.
 * @return Comment string.
 */
CString CVersionInfo::GetComments() const
{
	return m_strComments;
}

/** 
 * @brief Read version info from file.
 * This function reads version information from file's version resource
 * to member variables.
 */
void CVersionInfo::GetVersionInfo()
{
	m_pVffInfo = NULL;
	ZeroMemory(&m_FixedFileInfo, sizeof(m_FixedFileInfo));
	ZeroMemory(&m_dvi, sizeof(m_dvi));

	DWORD dwVerHnd = 0;			// An 'ignored' parameter, always '0'
	TCHAR szFileName[MAX_PATH];

	if (m_strFileName.IsEmpty())
		GetModuleFileName(NULL, szFileName, MAX_PATH);
	else
		_tcsncpy(szFileName, m_strFileName, MAX_PATH - 1);
	
	DWORD dwVerInfoSize = GetFileVersionInfoSize(szFileName, &dwVerHnd);
	if (dwVerInfoSize)
	{
		m_bVersionFound = TRUE;
		m_pVffInfo = new BYTE[dwVerInfoSize];
		if (GetFileVersionInfo(szFileName, dwVerHnd, dwVerInfoSize, m_pVffInfo))
		{
			GetFixedVersionInfo();
			if (m_bVersionOnly == FALSE)
				QueryStrings();
		}
		delete [] m_pVffInfo;
		m_pVffInfo = NULL;
	}

	if (m_bDllVersion)
	{
		if (HINSTANCE hinstDll = LoadLibrary(szFileName))
		{
			DLLGETVERSIONPROC DllGetVersion = (DLLGETVERSIONPROC) 
					GetProcAddress(hinstDll, "DllGetVersion");
			if (DllGetVersion)
			{
				m_dvi.cbSize = sizeof(m_dvi);
				if FAILED(DllGetVersion(&m_dvi))
				{
					m_dvi.cbSize = 0;
				}
			}
			FreeLibrary(hinstDll);
		}
	}
}

/** 
 * @brief Read strings from version info data.
 */
void CVersionInfo::QueryStrings()
{
	if (m_wLanguage != 0)
	{
		WORD codepage;
		GetCodepageForLanguage(m_wLanguage, codepage);
		m_strLanguage.Format(_T("%04x"), m_wLanguage);
		m_strCodepage.Format(_T("%04x"), codepage);
	}
	else if (m_strLanguage.IsEmpty()
		|| m_strCodepage.IsEmpty())
	{
		LANGUAGEANDCODEPAGE *lpTranslate;
		DWORD langLen;
		if (VerQueryValue((LPVOID)m_pVffInfo,
			_T("\\VarFileInfo\\Translation"),
			(LPVOID *)&lpTranslate, (UINT *)&langLen))
		{
			m_strLanguage.Format(_T("%4.4X"), lpTranslate[0].wLanguage);
			m_strCodepage.Format(_T("%4.4X"), lpTranslate[0].wCodePage);
		}
	}
	QueryValue(_T("CompanyName"), m_strCompanyName);
	QueryValue(_T("FileDescription"), m_strFileDescription);
	QueryValue(_T("FileVersion"), m_strFileVersion); 
	QueryValue(_T("InternalName"), m_strInternalName); 
	QueryValue(_T("LegalCopyright"), m_strLegalCopyright); 
	QueryValue(_T("OriginalFilename"), m_strOriginalFilename); 
	QueryValue(_T("ProductName"), m_strProductName); 
	QueryValue(_T("ProductVersion"), m_strProductVersion); 
	QueryValue(_T("Comments"), m_strComments);
	QueryValue(_T("SpecialBuild"), m_strSpecialBuild);
	QueryValue(_T("PrivateBuild"), m_strPrivateBuild);
}

/** 
 * @brief Read value from version info data.
 * @param [in] szId Name of value/string to read.
 * @param [out] Value read.
 */
void CVersionInfo::QueryValue(LPCTSTR szId, CString& s)
{
	ASSERT(m_pVffInfo != NULL);
	LPTSTR   lpVersion;			// String pointer to 'version' text
	UINT    uVersionLen;
	BOOL    bRetCode;

	TCHAR szSelector[256];
	_stprintf(szSelector,_T("\\StringFileInfo\\%s%s\\%s"),
			m_strLanguage, m_strCodepage, szId);
	bRetCode = VerQueryValue((LPVOID)m_pVffInfo,
		szSelector,
		(LPVOID *)&lpVersion,
		&uVersionLen);
	
	if (bRetCode)
	{
		s = lpVersion;
		s.TrimLeft();
		s.TrimRight();
	}
	else
		s.Empty();
}

/** 
 * @brief Read numeric version information.
 */
void CVersionInfo::GetFixedVersionInfo()
{
	VS_FIXEDFILEINFO * pffi;
	UINT len = sizeof(*pffi);
	BOOL bRetCode = VerQueryValue(
		(LPVOID)m_pVffInfo, _T("\\"), (LPVOID *)&pffi, &len);
	memcpy(&m_FixedFileInfo, pffi, sizeof(m_FixedFileInfo));
	m_dvi.dwMajorVersion = HIWORD(m_FixedFileInfo.dwFileVersionMS);
	m_dvi.dwMinorVersion = LOWORD(m_FixedFileInfo.dwFileVersionMS);
	m_dvi.dwBuildNumber = HIWORD(m_FixedFileInfo.dwFileVersionLS);
}

/** 
 * @brief Get codepage for given language.
 * This function finds codepage value for given language from version info.
 * That is, we have certain combinations of language-codepage in version info.
 * This function tells which codepage exists with given language, so we can
 * find existing version info data.
 * @param [in] wLanguage Language ID for which we need matching codepage.
 * @param [out] wCodePage Found codepage.
 * @return TRUE if language was found from version info block.
 */
BOOL CVersionInfo::GetCodepageForLanguage(WORD wLanguage, WORD & wCodePage)
{
	LANGUAGEANDCODEPAGE *lpTranslate;
	UINT cbTranslate;
	// Read the list of languages and code pages.

	VerQueryValue((LPVOID)m_pVffInfo, 
				_T("\\VarFileInfo\\Translation"),
				(LPVOID*)&lpTranslate,
				&cbTranslate);

	// Read the file description for each language and code page.

	const int nLangCount = cbTranslate / sizeof(LANGUAGEANDCODEPAGE);
	int i = 0;
	BOOL bFound = FALSE;
	while (bFound == FALSE && i < nLangCount)
	{
		if (lpTranslate[i].wLanguage == wLanguage)
		{
			wCodePage = lpTranslate[i].wCodePage;
			bFound = TRUE;
		}
		else
			++i;
	}
	return bFound;
}
