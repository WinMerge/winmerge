
#include "stdafx.h"	
#include "regkey.h"
#include "assert.h"

CRegKeyEx::CRegKeyEx()
{
	m_hKey = NULL;
}

CRegKeyEx::~CRegKeyEx()
{
	Close();
}

void CRegKeyEx::Close()
{
	if (m_hKey) 
	{
		RegCloseKey (m_hKey);
		m_hKey = NULL;
	}
}

LONG CRegKeyEx::Open(HKEY hKeyRoot, LPCTSTR pszPath)
{
	DWORD dw;
	_tcscpy(m_sPath,pszPath);

	return RegCreateKeyEx (hKeyRoot, pszPath, 0L, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, 
		&m_hKey, &dw);
}


LONG CRegKeyEx::WriteDword (LPCTSTR pszKey, DWORD dwVal)
{
	assert(m_hKey);
	assert(pszKey);
	return RegSetValueEx (m_hKey, pszKey, 0L, REG_DWORD,
		(const LPBYTE) &dwVal, sizeof(DWORD));
}

LONG CRegKeyEx::WriteBool (LPCTSTR pszKey, BOOL bVal)
{
	assert(m_hKey);
	assert(pszKey);
	DWORD dwVal = (DWORD)bVal; 
	return RegSetValueEx (m_hKey, pszKey, 0L, REG_DWORD,
		(const LPBYTE) &dwVal, sizeof(DWORD));
}

LONG CRegKeyEx::WriteFloat (LPCTSTR pszKey, float fVal)
{
	assert(m_hKey);
	assert(pszKey);
	TCHAR pszData[100];
	_stprintf(pszData,_T("%f"),fVal);
	return RegSetValueEx (m_hKey, pszKey, 0L, REG_SZ,
		(const LPBYTE) pszData, (_tcslen(pszData)+ 1)*sizeof(TCHAR) );
}

LONG CRegKeyEx::WriteString (LPCTSTR pszKey, LPCTSTR pszData)
{
	assert(m_hKey);
	assert(pszKey);
	assert(pszData);
//	assert(AfxIsValidAddress(pszData, strlen(pszData), FALSE));

	return RegSetValueEx (m_hKey, pszKey, 0L, REG_SZ,
		(const LPBYTE) pszData, (_tcslen(pszData)+ 1)*sizeof(TCHAR));
}


DWORD CRegKeyEx::ReadDword (LPCTSTR pszKey, DWORD defval)
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

LONG CRegKeyEx::ReadLong (LPCTSTR pszKey, LONG defval)
{
	return (UINT)ReadDword(pszKey, (DWORD)defval);
}

UINT CRegKeyEx::ReadUint (LPCTSTR pszKey, UINT defval)
{
	return (UINT)ReadDword(pszKey, (DWORD)defval);
}

UINT CRegKeyEx::ReadInt (LPCTSTR pszKey, int defval)
{
	return (int)ReadDword(pszKey, (DWORD)defval);
}

short int CRegKeyEx::ReadShort (LPCTSTR pszKey, short int defval)
{
	return (short int)ReadDword(pszKey, (DWORD)defval);
}

BYTE CRegKeyEx::ReadByte (LPCTSTR pszKey, BYTE defval)
{
	return (BYTE)ReadDword(pszKey, (DWORD)defval);
}

float CRegKeyEx::ReadFloat (LPCTSTR pszKey, float defval)
{
	assert(m_hKey);
	assert(pszKey);

	DWORD dwType;
	DWORD dwSize = 100;
	TCHAR  string[100];

	LONG lReturn = RegQueryValueEx (m_hKey, (LPTSTR) pszKey, NULL,
		&dwType, (LPBYTE) string, &dwSize);

	if (lReturn == ERROR_SUCCESS)
		return (float)_tcstod(string, NULL);
	else
		return defval;
}

BOOL CRegKeyEx::ReadBool (LPCTSTR pszKey, BOOL defval)
{
	assert(m_hKey);
	assert(pszKey);

	DWORD dwType;
	DWORD dwSize = sizeof (DWORD);
	DWORD dwDest;

	LONG lRet = RegQueryValueEx (m_hKey, (LPTSTR) pszKey, NULL, 
		&dwType, (LPBYTE) &dwDest, &dwSize);

	if (lRet == ERROR_SUCCESS)
		return (dwDest!=(DWORD)0);
	else
		return defval;
}

CString CRegKeyEx::ReadString (LPCTSTR pszKey, LPCTSTR defval)
{
	assert(m_hKey);
	assert(pszKey);

	DWORD dwType;
	DWORD dwSize = 400;
	TCHAR  string[400];

	LONG lReturn = RegQueryValueEx (m_hKey, (LPTSTR) pszKey, NULL,
		&dwType, (LPBYTE) string, &dwSize);

	if (lReturn == ERROR_SUCCESS)
		return CString(string);
	else
		return CString(defval);
}

void CRegKeyEx::ReadChars (LPCTSTR pszKey, LPTSTR pData, DWORD dwLen, LPCTSTR defval)
{
	assert(m_hKey);
	assert(pszKey);

	DWORD dwType, len=dwLen;

	LONG ret = RegQueryValueEx (m_hKey, (LPTSTR) pszKey, NULL,
		&dwType, (LPBYTE)pData, &len);
	if (ret != ERROR_SUCCESS)
	{
		_tcsncpy(pData, defval, min(_tcslen(defval), dwLen));
		pData[min(_tcslen(defval), dwLen-1)] = _T('\0');
	}
}



