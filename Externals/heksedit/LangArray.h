class LangArray : public SimpleArray<char *>
{
public:
	static const LANGID DefLangId;
	struct StringData
	{
		int refcount;
		char data[1];
		static StringData *Create(const char *, size_t);
		char *Share();
		static void Unshare(char *);
	};
	HMODULE m_hLangDll;
	unsigned m_codepage;
	LANGID m_langid;
	LangArray();
	~LangArray();
	void ExpandToSize();
	void ClearAll();
	BOOL Load(HINSTANCE, LANGID);
	BSTR TranslateStringA(int line);
	BSTR TranslateStringW(int line);
	void TranslateDialogA(HWND);
	void TranslateDialogW(HWND);
	static int LangCodeMajor(LANGID, LPTSTR);
	static int LangCodeMinor(LANGID, LPTSTR);
};
