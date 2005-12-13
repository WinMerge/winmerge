#include "stdafx.h"
#include "Setting.h"


// Wrapper class to close a registry key handle when it goes out of scope
class OpenRegKey
{
public:
	OpenRegKey(HKEY hkey) : m_hkey(hkey) { }
	~OpenRegKey() { CloseKey(); }
	void CloseKey() { if (m_hkey) { RegCloseKey(m_hkey); m_hkey = 0; } }
private:
	HKEY m_hkey;
};

// Read a setting value (try HKCU then HKLM)
bool ReadSetting(LPCTSTR setting, LPCTSTR name, LPTSTR buffer, int maxlen)
{
	if (ReadSetting(HKEY_CURRENT_USER, setting, name, buffer, maxlen))
		return true;
	return ReadSetting(HKEY_LOCAL_MACHINE, setting, name, buffer, maxlen);
}

// Return error string for specified error, if available (else 0)
static LPCTSTR GetSysErrString(DWORD errnum)
{
	LPVOID lpMsgBuf=0;
	if (FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		errnum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL))
	{
		return (LPCTSTR)lpMsgBuf;
	}
	else
	{
		return 0;
	}
}

// Read a setting value from specified registry location
bool ReadSetting(HKEY hKeyRoot, LPCTSTR setting, LPCTSTR name, LPTSTR buffer, int maxlen)
{
	bool success=false;
	DWORD ulOptions = 0;
	HKEY hKey = 0;
	LONG rtn = RegOpenKeyEx(hKeyRoot, setting, ulOptions, KEY_READ, &hKey);
	if (rtn != ERROR_SUCCESS)
	{
		LPCTSTR errstr = GetSysErrString(rtn);
		if (errstr)
		{
			// not reporting it anywhere
			LocalFree((LPVOID)errstr);
		}
		return false;
	}
	OpenRegKey regKeyWrapper(hKey); // ensure key is closed

	DWORD dwType=0;
	DWORD dwSize = maxlen/sizeof(TCHAR);
	rtn = RegQueryValueEx(hKey, name, NULL, &dwType, (unsigned char *)buffer, &dwSize);
	if (rtn != ERROR_SUCCESS)
	{
		LPCTSTR errstr = GetSysErrString(rtn);
		if (errstr)
		{
			// not reporting it anywhere
			LocalFree((LPVOID)errstr);
		}
		return false;
	}
	if (dwType != REG_SZ || dwSize > (unsigned int)maxlen)
		return false;
	return true;
}
