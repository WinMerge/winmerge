/** 
 * @file  RegKey.cpp
 *
 * @brief Implementation of CRegKeyEx C++ wrapper class for reading Windows registry
 */

#include "pch.h"
#define NOMINMAX
#include "RegKey.h"
#include <windows.h>
#include <cassert>
#include <strsafe.h>
#include "UnicodeString.h"

/**
 * @brief Default constructor.
 */
CRegKeyEx::CRegKeyEx()
: m_hKey(nullptr)
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
	if (m_hKey != nullptr) 
	{
		RegCloseKey(m_hKey);
		m_hKey = nullptr;
	}
}

/**
 * @brief Opens or creates a key in given path.
 * @param [in] hKeyRoot Root key to open, HKLM, HKCU..
 * @param [in] pszPath Path to actual registry key to access.
 * @return ERROR_SUCCESS or error value.
 */
LONG CRegKeyEx::Open(HKEY hKeyRoot, const tchar_t* pszPath)
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
LONG CRegKeyEx::OpenWithAccess(HKEY hKeyRoot, const tchar_t* pszPath, REGSAM regsam)
{
	DWORD dw;

	Close();
	m_sPath = pszPath;

	return RegCreateKeyEx(hKeyRoot, pszPath, 0L, nullptr,
		REG_OPTION_NON_VOLATILE, regsam, nullptr, 
		&m_hKey, &dw);
}

/**
 * @brief Opens key in given path.
 * @param [in] hKeyRoot Root key to open, HKLM, HKCU..
 * @param [in] pszPath Path to actual registry key to access.
 * @param [in] regsam Registry access parameter.
 * @return ERROR_SUCCESS or error value.
 */
LONG CRegKeyEx::OpenNoCreateWithAccess(HKEY hKeyRoot, const tchar_t* pszPath, REGSAM regsam)
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
bool CRegKeyEx::QueryRegMachine(const tchar_t* key)
{
	return OpenNoCreateWithAccess(HKEY_LOCAL_MACHINE, key, KEY_QUERY_VALUE) == ERROR_SUCCESS;
}

/**
 * @brief Opens registry key from HKEY_CURRENT_USER for reading.
 * @param [in] key Path to actual registry key to access.
 * @return true on success, false otherwise.
 */
bool CRegKeyEx::QueryRegUser(const tchar_t* key)
{
	return OpenNoCreateWithAccess(HKEY_CURRENT_USER, key, KEY_QUERY_VALUE) == ERROR_SUCCESS;
}

/**
 * @brief Write DWORD value to registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] dwVal Value to write.
 * @return ERROR_SUCCESS on success, or error value.
 */
LONG CRegKeyEx::WriteDword(const tchar_t* pszKey, DWORD dwVal)
{
	assert(m_hKey != nullptr);
	assert(pszKey != nullptr);
	return RegSetValueEx(m_hKey, pszKey, 0L, REG_DWORD,
		(const LPBYTE) &dwVal, sizeof(DWORD));
}

/**
 * @brief Write `bool` value to registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] bVal Value to write.
 * @return ERROR_SUCCESS on success, or error value.
 */
LONG CRegKeyEx::WriteBool(const tchar_t* pszKey, bool bVal)
{
	assert(m_hKey != nullptr);
	assert(pszKey != nullptr);
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
LONG CRegKeyEx::WriteFloat(const tchar_t* pszKey, float fVal)
{
	assert(m_hKey != nullptr);
	assert(pszKey != nullptr);
	String s = strutils::to_str(fVal);
	return RegSetValueEx(m_hKey, pszKey, 0L, REG_SZ,
		(const LPBYTE) s.c_str(), static_cast<DWORD>((s.length() + 1))*sizeof(tchar_t) );
}

/**
 * @brief Write string value to registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] pszData Value to write.
 * @return ERROR_SUCCESS on success, or error value.
 */
LONG CRegKeyEx::WriteString(const tchar_t* pszKey, const tchar_t* pszData)
{
	assert(m_hKey != nullptr);
	assert(pszKey != nullptr);
	assert(pszData != nullptr);

	return RegSetValueEx(m_hKey, pszKey, 0L, REG_SZ,
		(const LPBYTE) pszData, static_cast<DWORD>(tc::tcslen(pszData)+ 1)*sizeof(tchar_t));
}

/**
 * @brief Read DWORD value from registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] defval Default value to return if reading fails.
 * @return Read DWORD value.
 */
DWORD CRegKeyEx::ReadDword(const tchar_t* pszKey, DWORD defval)
{
	assert(m_hKey != nullptr);
	assert(pszKey != nullptr);

	DWORD dwType;
	DWORD dwSize = sizeof (DWORD);
	DWORD dwDest;

	LONG lRet = RegQueryValueEx (m_hKey, (tchar_t*) pszKey, nullptr, 
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
LONG CRegKeyEx::ReadLong(const tchar_t* pszKey, LONG defval)
{
	return (LONG)ReadDword(pszKey, (DWORD)defval);
}

/**
 * @brief Read UINT value from registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] defval Default value to return if reading fails.
 * @return Read UINT value.
 */
UINT CRegKeyEx::ReadUint(const tchar_t* pszKey, UINT defval)
{
	return (UINT)ReadDword(pszKey, (DWORD)defval);
}

/**
 * @brief Read UINT value from registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] defval Default value to return if reading fails.
 * @return Read UINT value.
 */
UINT CRegKeyEx::ReadInt(const tchar_t* pszKey, int defval)
{
	return (int)ReadDword(pszKey, (DWORD)defval);
}

/**
 * @brief Read short int value from registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] defval Default value to return if reading fails.
 * @return Read short int value.
 */
short int CRegKeyEx::ReadShort(const tchar_t* pszKey, short int defval)
{
	return (short int)ReadDword(pszKey, (DWORD)defval);
}

/**
 * @brief Read BYTE value from registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] defval Default value to return if reading fails.
 * @return Read BYTE value.
 */
BYTE CRegKeyEx::ReadByte(const tchar_t* pszKey, BYTE defval)
{
	return (BYTE)ReadDword(pszKey, (DWORD)defval);
}

/**
 * @brief Read float value from registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] defval Default value to return if reading fails.
 * @return Read float value.
 */
float CRegKeyEx::ReadFloat(const tchar_t* pszKey, float defval)
{
	assert(m_hKey != nullptr);
	assert(pszKey != nullptr);

	DWORD dwType;
	DWORD dwSize = 100;
	tchar_t  string[100];

	LONG lReturn = RegQueryValueEx(m_hKey, (tchar_t*) pszKey, nullptr,
		&dwType, (LPBYTE) string, &dwSize);

	if (lReturn == ERROR_SUCCESS)
		return (float)tc::tcstod(string, nullptr);
	else
		return defval;
}

/**
 * @brief Read `bool` value from registry.
 * @param [in] pszKey Path to actual registry key to access.
 * @param [in] defval Default value to return if reading fails.
 * @return Read `bool` value.
 */
bool CRegKeyEx::ReadBool(const tchar_t* pszKey, bool defval)
{
	assert(m_hKey != nullptr);
	assert(pszKey != nullptr);

	DWORD dwType;
	DWORD dwSize = sizeof(DWORD);
	DWORD dwDest;

	LONG lRet = RegQueryValueEx(m_hKey, (tchar_t*) pszKey, nullptr, 
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
String CRegKeyEx::ReadString (const tchar_t* pszKey, const tchar_t* defval)
{
	assert(m_hKey != nullptr);
	assert(pszKey != nullptr);

	DWORD dwType;
	DWORD dwSize = 0;
	String retString;

	// Get size of the string
	LONG lReturn = RegQueryValueEx(m_hKey, (tchar_t*) pszKey, nullptr,
		&dwType, nullptr, &dwSize);

	if (lReturn == ERROR_SUCCESS)
	{
		retString.resize(dwSize/sizeof(tchar_t));
		lReturn = RegQueryValueEx(m_hKey, (tchar_t*) pszKey, nullptr,
			&dwType, (LPBYTE) retString.data(), &dwSize);
		retString.resize(dwSize/sizeof(tchar_t)-1);
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
void CRegKeyEx::ReadChars (const tchar_t* pszKey, tchar_t* pData, DWORD dwLen, const tchar_t* defval)
{
	assert(m_hKey != nullptr);
	assert(pszKey != nullptr);

	DWORD dwType;
	DWORD len = dwLen;

	LONG ret = RegQueryValueEx (m_hKey, (tchar_t*) pszKey, nullptr,
		&dwType, (LPBYTE)pData, &len);
	if (ret != ERROR_SUCCESS)
		StringCchCopy(pData, dwLen, defval);
}
