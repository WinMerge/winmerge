/** 
 * @file  RegKey.h
 *
 * @brief Declaration of CRegKeyEx C++ wrapper class for reading Windows registry
 */
#pragma once

#include <Windows.h>
#include "UnicodeString.h"

/**
 * @brief Class for reading/writing registry.
 */
class CRegKeyEx
{

// Construction
public:
	CRegKeyEx();
	~CRegKeyEx();

// Operations
public:
	void Close();
	LONG Open(HKEY hKeyRoot, LPCTSTR pszPath);
	LONG OpenWithAccess(HKEY hKeyRoot, LPCTSTR pszPath, REGSAM regsam);
	LONG OpenNoCreateWithAccess(HKEY hKeyRoot, LPCTSTR pszPath, REGSAM regsam);
	bool QueryRegMachine(LPCTSTR key);
	bool QueryRegUser(LPCTSTR key);

	LONG WriteDword (LPCTSTR pszKey, DWORD dwVal);
	LONG WriteString (LPCTSTR pszKey, LPCTSTR pszVal);
	LONG WriteBool (LPCTSTR pszKey, bool bVal);
	LONG WriteFloat (LPCTSTR pszKey, float fVal);

	DWORD ReadDword (LPCTSTR pszKey, DWORD defval);
	float ReadFloat (LPCTSTR pszKey, float defval);
	bool ReadBool(LPCTSTR pszKey, bool defval);
	LONG ReadLong (LPCTSTR pszKey, LONG defval);
	UINT ReadUint (LPCTSTR pszKey, UINT defval);
	UINT ReadInt (LPCTSTR pszKey, int defval);
	short int ReadShort (LPCTSTR pszKey, short int defval);
	BYTE ReadByte (LPCTSTR pszKey, BYTE defval);
	String ReadString (LPCTSTR pszKey, LPCTSTR defval);
	void ReadChars (LPCTSTR pszKey, LPTSTR pData, DWORD dwLength, LPCTSTR defval);

protected:
	HKEY m_hKey; /**< Open key (HKLM, HKCU, etc). */
	String m_sPath; /**< Path to actual key to open. */
};
