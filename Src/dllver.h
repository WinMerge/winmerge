#ifndef dllver_h_included
#define dllver_h_included

#ifndef PACKVERSION
#define PACKVERSION(major,minor) MAKELONG(minor,major)
#endif

// resource pattern for APIs LoadLibrary, FreeLibrary
struct Library
{
	Library() : m_hinst(0) { }
	~Library() { Unload(); }
	bool Load(LPCTSTR sz)
	{
		Unload();
		m_hinst = LoadLibrary(sz);
		return m_hinst != NULL;
	}
	void Unload()
	{
		if (m_hinst)
		{
			FreeLibrary(m_hinst);
			m_hinst = NULL;
		}
	}
	HINSTANCE Inst() const { return m_hinst; }
protected:
	HINSTANCE m_hinst;
};


DWORD getFixedModuleVersion(HMODULE hmod);
DWORD GetDllVersion_Raw(LPCTSTR lpszDllName); // checks very time
DWORD GetDllVersion(LPCTSTR lpszDllName); // checks once & caches

#endif // dllver_h_included