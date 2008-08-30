#include "precomp.h"
#include "simparr.h"
#include "LangArray.h"
#include "VersionData.h"

/**
 * @brief Remove prefix from the string.
 * @param [in] text String from which to jump over prefix.
 * @param [in] prefix Prefix string to jump over.
 * @return String without the prefix.
 * @note Function returns pointer to original string,
 *  it does not allocate a new string.
 */
static char *EatPrefix(char *text, const char *prefix)
{
	if (int len = strlen(prefix))
		if (_memicmp(text, prefix, len) == 0)
			return text + len;
	return 0;
}

LangArray::StringData *LangArray::StringData::Create(const char *ps, size_t length)
{
	size_t cb = FIELD_OFFSET(StringData, data) + length + 1;
	StringData *psd = reinterpret_cast<StringData *>(new char[cb]);
	psd->refcount = 0;
	memcpy(psd->data, ps, length);
	psd->data[length] = '\0';
	return psd;
}

char *LangArray::StringData::Share()
{
	++refcount;
	return data;
}

void LangArray::StringData::Unshare(char *data)
{
	data -= FIELD_OFFSET(StringData, data);
	if (--reinterpret_cast<StringData *>(data)->refcount == 0)
		delete [] data;
}

LangArray::LangArray()
: m_hLangDll(0)
, m_codepage(0)
, m_langid(0)
{
}

LangArray::~LangArray()
{
	ClearAll();
}

void LangArray::ExpandToSize()
{
	while (m_nUpperBound < m_nSize - 1)
		m_pT[++m_nUpperBound] = 0;
}

void LangArray::ClearAll()
{
	while (m_nSize)
		if (char *data = m_pT[--m_nSize])
			StringData::Unshare(data);
	SimpleArray<char *>::ClearAll();
}

/**
 * @brief Convert C style \\nnn, \\r, \\n, \\t etc into their indicated characters.
 * @param [in] codepage Codepage to use in conversion.
 * @param [in,out] s String to convert.
 */
static void unslash(unsigned codepage, char *p)
{
	char *q = p;
	char c;
	do
	{
		char *r = q + 1;
		switch (c = *q)
		{
		case '\\':
			switch (c = *r++)
			{
			case 'a':
				c = '\a';
				break;
			case 'b':
				c = '\b';
				break;
			case 'f':
				c = '\f';
				break;
			case 'n':
				c = '\n';
				break;
			case 'r':
				c = '\r';
				break;
			case 't':
				c = '\t';
				break;
			case 'v':
				c = '\v';
				break;
			case 'x':
				*p = (char)strtol(r, &q, 16);
				break;
			default:
				*p = (char)strtol(r - 1, &q, 8);
				break;
			}
			if (q >= r)
				break;
			// fall through
		default:
			*p = c;
			if ((*p & 0x80) && IsDBCSLeadByteEx(codepage, *p))
				*++p = *r++;
			q = r;
		}
		++p;
	} while (c != '\0');
}

BOOL LangArray::Load(HINSTANCE hMainInstance, LANGID langid)
{
	if (m_langid == langid)
		return TRUE;
	m_langid = langid;
	ClearAll();
	char buf[1024];
	SimpleString *ps = 0;
	SimpleString msgid;
	SimpleArray<int> lines;
	int unresolved = 0;
	int mismatched = 0;
	FILE *f = 0;
	if (langid)
	{
		TCHAR path[MAX_PATH];
		GetModuleFileName(hMainInstance, path, MAX_PATH);
		LPTSTR name = PathFindFileName(path); 
		LPTSTR ext = PathFindExtension(name);
		*ext = _T('\0');
		wsprintf(ext + 1, _T("lng\\%s.lng"), name);
		*ext = _T('.');
		if (m_hLangDll == 0)
		{
			m_hLangDll = LoadLibrary(path);
			if (m_hLangDll == 0)
				return FALSE;
		}
		const CVersionData *pvdMain = CVersionData::Load(hMainInstance);
		if (pvdMain == 0)
			return FALSE;
		const CVersionData *pvdLang = CVersionData::Load(m_hLangDll);
		if (pvdLang == 0)
			return FALSE;
		if (memcmp(pvdMain->Data(), pvdLang->Data(), FIELD_OFFSET(VS_FIXEDFILEINFO, dwProductVersionMS)))
			return FALSE;
		HRSRC potfile = FindResource(m_hLangDll, _T("en-US.pot"), RT_RCDATA);
		if (potfile == 0)
			return FALSE;
		size_t size = SizeofResource(m_hLangDll, potfile);
		const char *data = (const char *)LoadResource(m_hLangDll, potfile);
		while (const char *eol = (const char *)memchr(data, '\n', size))
		{
			size_t len = eol - data;
			if (len >= sizeof buf)
			{
				assert(FALSE);
				break;
			}
			memcpy(buf, data, len);
			buf[len++] = '\0';
			data += len;
			size -= len;
			if (char *p = EatPrefix(buf, "#:"))
			{
				if (char *q = strchr(p, ':'))
				{
					int line = strtol(q + 1, &q, 10);
					lines.Append(line);
					++unresolved;
				}
			}
			else if (EatPrefix(buf, "msgid "))
			{
				ps = &msgid;
			}
			if (ps)
			{
				char *p = strchr(buf, '"');
				char *q = strrchr(buf, '"');
				if (q > p)
				{
					*q = '\0';
					ps->AppendString(p + 1);
				}
				else
				{
					ps = 0;
					if (int i = lines.GetLength())
					{
						StringData *psd = StringData::Create(msgid, msgid.StrLen());
						do
						{
							int line = lines[--i];
							if (GetLength() <= line)
							{
								SetSize(line + 1);
								ExpandToSize();
							}
							SetAt(line, psd->Share());
						} while (i);
					}
					lines.ClearAll();
					msgid.Clear();
				}
			}
		}
		PathRemoveFileSpec(path);
		name = PathAddBackslash(path);
		// Look for a .po file that matches the given langid.
		// Possible cases in order of precedence:
		// (1) Country specific translation for given country, e.g. de-CH.po
		// (2) Country specific translation to serve as default, e.g. de-DE.po
		// (3) Country inspecific translation, e.g. de.po
		do
		{
			if (int i = GetLocaleInfo(langid, LOCALE_SISO639LANGNAME, name, 4))
			{
				int j = GetLocaleInfo(langid, LOCALE_SISO3166CTRYNAME, name + i, 4);
				name[--i] = '-';
				do
				{
					strcpy(name + i + j, ".po");
					f = fopen(path, "r");
					if (j == 0 || SUBLANGID(langid) != SUBLANG_DEFAULT)
						break;
					j = 0;
				} while (f == 0);
			}
			if (SUBLANGID(langid) == SUBLANG_DEFAULT)
				break;
			langid = MAKELANGID(PRIMARYLANGID(langid), SUBLANG_DEFAULT);
		} while (f == 0);
	}
	if (f == 0)
	{
		if (m_hLangDll)
			FreeLibrary(m_hLangDll);
		m_hLangDll = 0;
		return FALSE;
	}
	ps = 0;
	msgid.ClearAll();
	lines.ClearAll();
	SimpleString msgstr;
	SimpleString format;
	SimpleString directive;
	while (fgets(buf, sizeof buf, f))
	{
		if (char *p = EatPrefix(buf, "#:"))
		{
			if (char *q = strchr(p, ':'))
			{
				int line = strtol(q + 1, &q, 10);
				if (line == 367)
					line = line;
				lines.Append(line);
				--unresolved;
			}
		}
		else if (char *p = EatPrefix(buf, "#,"))
		{
			StrTrim(p, " \t\r\n");
			format = p;
		}
		else if (char *p = EatPrefix(buf, "#."))
		{
			StrTrim(p, " \t\r\n");
			directive = p;
		}
		else if (EatPrefix(buf, "msgid "))
		{
			ps = &msgid;
		}
		else if (EatPrefix(buf, "msgstr "))
		{
			ps = &msgstr;
		}
		if (ps)
		{
			char *p = strchr(buf, '"');
			char *q = strrchr(buf, '"');
			if (q > p)
			{
				*q = '\0';
				ps->AppendString(p + 1);
			}
			else
			{
				ps = 0;
				if (msgstr.IsEmpty())
					msgstr = msgid;
				unslash(m_codepage, msgstr);
				if (int i = lines.GetLength())
				{
					StringData *psd = StringData::Create(msgstr, msgstr.StrLen());
					do
					{
						int line = lines[--i];
						if (GetLength() <= line)
						{
							SetSize(line + 1);
							ExpandToSize();
						}
						char *data = GetAt(line);
						if (data && strcmp(data, msgid) == 0)
						{
							StringData::Unshare(data);
							SetAt(line, psd->Share());
						}
						else
						{
							++mismatched;
						}
					} while (i);
				}
				lines.ClearAll();
				if (strcmp(directive, "Codepage") == 0)
				{
					m_codepage = strtol(msgstr, &p, 10);
					directive.Clear();
				}
				msgid.Clear();
				msgstr.Clear();
			}
		}
	}
	fclose(f);
	if (unresolved || mismatched)
	{
		ClearAll();
		if (m_hLangDll)
			FreeLibrary(m_hLangDll);
		m_hLangDll = 0;
		return FALSE;
	}
	return TRUE;
}

BSTR LangArray::TranslateStringA(int line)
{
	BSTR t = 0;
	if (line > 0 && line < GetLength())
	{
		if (char *s = GetAt(line))
		{
			if (int len = strlen(s))
			{
				unsigned codepage = GetACP();
				if (m_codepage != codepage)
				{
					// Attempt to convert to UI codepage
					BSTR ws = SysAllocStringLen(0, len);
					len = MultiByteToWideChar(m_codepage, 0, s, -1, ws, len + 1);
					if (len)
					{
						SysReAllocStringLen(&ws, ws, len - 1);
						len = WideCharToMultiByte(codepage, 0, ws, -1, 0, 0, 0, 0);
						if (len)
						{
							t = SysAllocStringByteLen(0, len - 1);
							WideCharToMultiByte(codepage, 0, ws, -1, (PSTR)t, len, 0, 0);
						}
					}
					SysFreeString(ws);
				}
				else
				{
					t = SysAllocStringByteLen(s, len);
				}
			}
		}
	}
	return t;
}

BSTR LangArray::TranslateStringW(int line)
{
	BSTR ws = 0;
	if (line > 0 && line < GetLength())
	{
		if (char *s = GetAt(line))
		{
			if (int len = strlen(s))
			{
				ws = SysAllocStringLen(0, len);
				len = MultiByteToWideChar(m_codepage, 0, s, -1, ws, len + 1);
				SysReAllocStringLen(&ws, ws, len - 1);
			}
		}
	}
	return ws;
}

void LangArray::TranslateDialogA(HWND h)
{
	UINT gw = GW_CHILD;
	do
	{
		TCHAR text[80];
		::GetWindowTextA(h, text, RTL_NUMBER_OF(text));
		int line = 0;
		if (LPTSTR p = _tcschr(text, _T(':')))
			line = _ttoi(p + 1);
		if (BSTR t = TranslateStringA(line))
		{
			::SetWindowTextA(h, (PSTR)t);
			::SysFreeString(t);
		}
		h = ::GetWindow(h, gw);
		gw = GW_HWNDNEXT;
	} while (h);
}

void LangArray::TranslateDialogW(HWND h)
{
	UINT gw = GW_CHILD;
	do
	{
		WCHAR text[80];
		::GetWindowTextW(h, text, RTL_NUMBER_OF(text));
		int line = 0;
		if (LPWSTR p = wcschr(text, L':'))
			line = _wtoi(p + 1);
		if (BSTR t = TranslateStringW(line))
		{
			::SetWindowTextW(h, t);
			::SysFreeString(t);
		}
		h = ::GetWindow(h, gw);
		gw = GW_HWNDNEXT;
	} while (h);
}
