/*/VersionData.h
This code has been released into the public domain and may be used
for any purposes whatsoever without acknowledgment.
*/

class CVersionData
{
private:
	// Disallow construction
	CVersionData();
public:
	WORD   wLength;
	WORD   wValueLength;
	WORD   wType;
	WCHAR  szKey[1];
	const WCHAR *Data() const
	{
		if (this)
		{
			const WCHAR *p = szKey + lstrlenW(szKey) + 1;
			return (const WCHAR *)((UINT_PTR) p + 3U & ~3U);
		}
		return 0;
	}
	const CVersionData *First() const
	{
		if (this)
		{
			return (const CVersionData *)((UINT_PTR)Data() + wValueLength + 3U & ~3U);
		}
		return 0;
	}
	const CVersionData *Next() const
	{
		if (this)
		{
			return (const CVersionData *)((UINT_PTR)this + wLength + 3U & ~3U);
		}
		return 0;
	}
	const CVersionData *Find(LPCWSTR szKey) const
	{
		const CVersionData *p = First();
		while (p < Next())
		{
			if (szKey == 0 || StrCmpW(szKey, p->szKey) == 0)
				return p;
			p = p->Next();
		}
		return 0;
	}
	static const CVersionData *Load(HMODULE hModule = 0, LPCTSTR lpszRes = MAKEINTRESOURCE(VS_VERSION_INFO))
	{
		HRSRC hRes = FindResource(hModule, lpszRes, RT_VERSION);
		return (const CVersionData *)LoadResource(hModule, hRes);
	}
};
