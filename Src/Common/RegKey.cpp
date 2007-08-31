/** 
 * @file  RegKey.cpp
 *
 * @brief Implementation of CRegKeyEx C++ wrapper class for reading Windows registry
 */
// ID line follows -- this is updated by SVN
// $Id$

#include <assert.h>
#include "UnicodeString.h"
#include "RegKey.h"

/**
 * @brief Default constructor.
 */
CRegKeyEx::CRegKeyEx()
: m_hKey(NULL)
{
}

/**
 * @brief Default destructor.
 */
CRegKeyEx::~CRegKeyEx()
{
	Close();
}

/**
 * @brief Closes the key.
 */
void CRegKeyEx::Close()
{
	if (m_hKey) 
	{
		RegCloseKey(m_hKey);
		m_hKey = NULL;
	}
}

/**
 * @brief Opens or creates a key in given path.
 * @param [in] hKeyRoot Root key to open, HKLM, HKCU..
 * @param [in] pszPath Path to actual registry key to access.
 * @return ERROR_SUCCESS or error value.
 */
LONG CRegKeyEx::Open(HKEY hKeyRoot, LPCTSTR pszPath)
{
	return OpenWithAccess(hKeyRoot, pszPath, KEY_ALL_ACCESS);
}

/**
 * @brief Opens or creates a key in given path with access control.
 * @param [in] hKeyRoot Root key to open, HKLM, HKCU..
 * @param [in] pszPath Path to actual registry key to access.
 * @param [in] regsam Registry access parameter.
 * @return ERROR_SUCCESS or error value.
 */
LONG CRegKeyEx::OpenWithAccess(HKEY hKeyRoot, LPCTSTR pszPath, REGSAM regsam)
{
	DWORD dw;

	Close();
	m_sPath = pszPath;

	return RegCreateKeyEx(hKeyRoot, pszPath, 0L, NULL,
		REG_OPTION_NON_VOLATILE, regsam, NULL, 
		&m_hKey, &dw);
}

/**
 * @brief Opens key in given path.
 * @param [in] hKeyRoot Root key to open, HKLM, HKCU..
 * @param [in] pszPath Path to actual registry key to access.
 * @param [in] regsam Registry access parameter.
 * @return ERROR_SUCCESS or error value.
 */
LONG CRegKeyEx::OpenNoCreateWithAccess(HKEY hKeyRoot, LPCTSTR pszPath, REGSAM regsam)
{
	Close();

	m_sPath = pszPath;

	return RegOpenKeyEx (hKeyRoot, pszPath, 0L, regsam, &m_hKey);
}

/**
 * @brief Opens registry key from HKEY_LOCAL_MACHINE for reading.
 * @param [in] key Path to actual registry key to access.
 * @return true on success, false otherwise.
 */
bool CRegKeyEx::QueryRegMachine(LPCTSTR key)
{
	return OpenNoCreateWithAccess(HKEY_LOCAL_MACHINE, key, KEY_QUERY_VALUE) == ERROR_SUCCESS;
}

/**
 * @brief Opens registry key from HKEY_CURRENT_USER for reading.
 * @param [in] key Path to actual registry key to access.
 * @return true on success, false otherwise.
 */
bool CRegKeyEx::QueryRegUser(LPCTSTR key)
{
	return OpenNoCreateWithAccess(HKEY_CURRENT_USER, key, KEY_QUERY_VALUE) == ERROR_SUCCESS;
}

/**
 * @brief Write DWORD value to registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] dwVal Value to write.
 * @return ERROR_SUCCESS on success, or error value.
 */
LONG CRegKeyEx::WriteDword(LPCTSTR pszKey, DWORD dwVal)
{
	assert(m_hKey);
	assert(pszKey);
	return RegSetValueEx(m_hKey, pszKey, 0L, REG_DWORD,
		(const LPBYTE) &dwVal, sizeof(DWORD));
}

/**
 * @brief Write BOOL value to registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] bVal Value to write.
 * @return ERROR_SUCCESS on success, or error value.
 */
LONG CRegKeyEx::WriteBool(LPCTSTR pszKey, BOOL bVal)
{
	assert(m_hKey);
	assert(pszKey);
	DWORD dwVal = (DWORD)bVal; 
	return RegSetValueEx(m_hKey, pszKey, 0L, REG_DWORD,
		(const LPBYTE) &dwVal, sizeof(DWORD));
}

/**
 * @brief Write float value to registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] fVal Value to write.
 * @return ERROR_SUCCESS on success, or error value.
 */
LONG CRegKeyEx::WriteFloat(LPCTSTR pszKey, float fVal)
{
	assert(m_hKey);
	assert(pszKey);
	TCHAR pszData[100];
	_stprintf(pszData, _T("%f"), fVal);
	return RegSetValueEx(m_hKey, pszKey, 0L, REG_SZ,
		(const LPBYTE) pszData, (_tcslen(pszData)+ 1)*sizeof(TCHAR) );
}

/**
 * @brief Write string value to registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] pszData Value to write.
 * @return ERROR_SUCCESS on success, or error value.
 */
LONG CRegKeyEx::WriteString(LPCTSTR pszKey, LPCTSTR pszData)
{
	assert(m_hKey);
	assert(pszKey);
	assert(pszData);

	return RegSetValueEx(m_hKey, pszKey, 0L, REG_SZ,
		(const LPBYTE) pszData, (_tcslen(pszData)+ 1)*sizeof(TCHAR));
}

/**
 * @brief Read DWORD value from registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] defval Default value to return if reading fails.
 * @return Read DWORD value.
 */
DWORD CRegKeyEx::ReadDword(LPCTSTR pszKey, DWORD defval)
{
	assert(m_hKey);
	assert(pszKey);

	DWORD dwType;
	DWORD dwSize = sizeof (DWORD);
	DWORD dwDest;

	LONG lRet = RegQueryValueEx (m_hKey, (LPTSTR) pszKey, NULL, 
		&dwType, (LPBYTE) &dwDest, &dwSize);

	if (lRet == ERROR_SUCCESS)
		return dwDest;
	else
		return defval;
}

/**
 * @brief Read LONG value from registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] defval Default value to return if reading fails.
 * @return Read LONG value.
 */
LONG CRegKeyEx::ReadLong(LPCTSTR pszKey, LONG defval)
{
	return (LONG)ReadDword(pszKey, (DWORD)defval);
}

/**
 * @brief Read UINT value from registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] defval Default value to return if reading fails.
 * @return Read UINT value.
 */
UINT CRegKeyEx::ReadUint(LPCTSTR pszKey, UINT defval)
{
	return (UINT)ReadDword(pszKey, (DWORD)defval);
}

/**
 * @brief Read UINT value from registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] defval Default value to return if reading fails.
 * @return Read UINT value.
 */
UINT CRegKeyEx::ReadInt(LPCTSTR pszKey, int defval)
{
	return (int)ReadDword(pszKey, (DWORD)defval);
}

/**
 * @brief Read short int value from registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] defval Default value to return if reading fails.
 * @return Read short int value.
 */
short int CRegKeyEx::ReadShort(LPCTSTR pszKey, short int defval)
{
	return (short int)ReadDword(pszKey, (DWORD)defval);
}

/**
 * @brief Read BYTE value from registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] defval Default value to return if reading fails.
 * @return Read BYTE value.
 */
BYTE CRegKeyEx::ReadByte(LPCTSTR pszKey, BYTE defval)
{
	return (BYTE)ReadDword(pszKey, (DWORD)defval);
}

/**
 * @brief Read float value from registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] defval Default value to return if reading fails.
 * @return Read float value.
 */
float CRegKeyEx::ReadFloat(LPCTSTR pszKey, float defval)
{
	assert(m_hKey);
	assert(pszKey);

	DWORD dwType;
	DWORD dwSize = 100;
	TCHAR  string[100];

	LONG lReturn = RegQueryValueEx(m_hKey, (LPTSTR) pszKey, NULL,
		&dwType, (LPBYTE) string, &dwSize);

	if (lReturn == ERROR_SUCCESS)
		return (float)_tcstod(string, NULL);
	else
		return defval;
}

/**
 * @brief Read BOOL value from registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] defval Default value to return if reading fails.
 * @return Read BOOL value.
 */
BOOL CRegKeyEx::ReadBool(LPCTSTR pszKey, BOOL defval)
{
	assert(m_hKey);
	assert(pszKey);

	DWORD dwType;
	DWORD dwSize = sizeof(DWORD);
	DWORD dwDest;

	LONG lRet = RegQueryValueEx(m_hKey, (LPTSTR) pszKey, NULL, 
		&dwType, (LPBYTE) &dwDest, &dwSize);

	if (lRet == ERROR_SUCCESS)
		return (dwDest!=(DWORD)0);
	else
		return defval;
}

/**
 * @brief Read String value from registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] defval Default value to return if reading fails.
 * @return Read String value.
 */
String CRegKeyEx::ReadString (LPCTSTR pszKey, LPCTSTR defval)
{
	assert(m_hKey);
	assert(pszKey);

	DWORD dwType;
	DWORD dwSize = 0;
	String retString;

	// Get size of the string
	LONG lReturn = RegQueryValueEx(m_hKey, (LPTSTR) pszKey, NULL,
		&dwType, NULL, &dwSize);

	if (lReturn == ERROR_SUCCESS)
	{
		TCHAR *stringVal = new TCHAR[dwSize];

		lReturn = RegQueryValueEx(m_hKey, (LPTSTR) pszKey, NULL,
			&dwType, (LPBYTE) stringVal, &dwSize);
		retString = stringVal;
		delete stringVal;
	}
	if (lReturn == ERROR_SUCCESS)
		return retString;
	else
		return defval;
}

/**
 * @brief Read char table from registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] pData Pointer to char table where value is written to.
 * @param [in] dwLen Size of pData table in bytes.
 * @param [in] defval Default value to return if reading fails.
 */
void CRegKeyEx::ReadChars (LPCTSTR pszKey, LPTSTR pData, DWORD dwLen, LPCTSTR defval)
{
	assert(m_hKey);
	assert(pszKey);

	DWORD dwType;
	DWORD len = dwLen;

	LONG ret = RegQueryValueEx (m_hKey, (LPTSTR) pszKey, NULL,
		&dwType, (LPBYTE)pData, &len);
	if (ret != ERROR_SUCCESS)
	{
		_tcsncpy(pData, defval, min(_tcslen(defval), dwLen));
		pData[min(_tcslen(defval), dwLen-1)] = _T('\0');
	}
}
