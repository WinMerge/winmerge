#include <winreg.h>

class CRegKeyEx
{

// Construction
public:
	CRegKeyEx();
	~CRegKeyEx();

// Operations
public:
	HKEY GetKey()  { return m_hKey; }
	void Close();
	LONG Open(HKEY hKeyRoot, LPCTSTR pszPath);

	LONG WriteDword (LPCTSTR pszKey, DWORD dwVal);
	LONG WriteString (LPCTSTR pszKey, LPCTSTR pszVal);
	LONG WriteBool (LPCTSTR pszKey, BOOL bVal);
	LONG WriteFloat (LPCTSTR pszKey, float fVal);

	DWORD ReadDword (LPCTSTR pszKey, DWORD defval);
	float ReadFloat (LPCTSTR pszKey, float defval);
	BOOL ReadBool(LPCTSTR pszKey, BOOL defval);
	LONG ReadLong (LPCTSTR pszKey, LONG defval);
	UINT ReadUint (LPCTSTR pszKey, UINT defval);
	UINT ReadInt (LPCTSTR pszKey, int defval);
	short int ReadShort (LPCTSTR pszKey, short int defval);
	BYTE ReadByte (LPCTSTR pszKey, BYTE defval);
	CString ReadString (LPCTSTR pszKey, LPCTSTR defval);
	void ReadChars (LPCTSTR pszKey, LPTSTR pData, DWORD dwLength, LPCTSTR defval);

protected:
	HKEY 	m_hKey;
	TCHAR m_sPath[_MAX_PATH];

};
