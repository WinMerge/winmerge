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
	LONG Open(HKEY hKeyRoot, const tchar_t* pszPath);
	LONG OpenWithAccess(HKEY hKeyRoot, const tchar_t* pszPath, REGSAM regsam);
	LONG OpenNoCreateWithAccess(HKEY hKeyRoot, const tchar_t* pszPath, REGSAM regsam);
	bool QueryRegMachine(const tchar_t* key);
	bool QueryRegUser(const tchar_t* key);

	LONG WriteDword (const tchar_t* pszKey, DWORD dwVal);
	LONG WriteString (const tchar_t* pszKey, const tchar_t* pszVal);
	LONG WriteBool (const tchar_t* pszKey, bool bVal);
	LONG WriteFloat (const tchar_t* pszKey, float fVal);

	DWORD ReadDword (const tchar_t* pszKey, DWORD defval);
	float ReadFloat (const tchar_t* pszKey, float defval);
	bool ReadBool(const tchar_t* pszKey, bool defval);
	LONG ReadLong (const tchar_t* pszKey, LONG defval);
	UINT ReadUint (const tchar_t* pszKey, UINT defval);
	UINT ReadInt (const tchar_t* pszKey, int defval);
	short int ReadShort (const tchar_t* pszKey, short int defval);
	BYTE ReadByte (const tchar_t* pszKey, BYTE defval);
	String ReadString (const tchar_t* pszKey, const tchar_t* defval);
	void ReadChars (const tchar_t* pszKey, tchar_t* pData, DWORD dwLength, const tchar_t* defval);

protected:
	HKEY m_hKey; /**< Open key (HKLM, HKCU, etc). */
	String m_sPath; /**< Path to actual key to open. */
};
