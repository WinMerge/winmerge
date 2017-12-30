/**
 * @file  LanguageSelect.cpp
 *
 * @brief Implements the Language Selection dialog class (which contains the language data)
 */

#include "StdAfx.h"
#include "LanguageSelect.h"
#include "version.h"
#include "BCMenu.h"
#include "Environment.h"
#include "paths.h"
#include "unicoder.h"

// Escaped character constants in range 0x80-0xFF are interpreted in current codepage
// Using C locale gets us direct mapping to Unicode codepoints
#pragma setlocale("C")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** @brief Relative path to WinMerge executable for lang files. */
static const TCHAR szRelativePath[] = _T("Languages");

static char *EatPrefix(char *text, const char *prefix);
static void unslash(unsigned codepage, std::string &s);
static HANDLE NTAPI FindFile(HANDLE h, LPCTSTR path, WIN32_FIND_DATA *fd);

/**
 * @brief A class holding information about language file.
 */
class LangFileInfo
{
public:
	LANGID id; /**< Language ID. */

	static LANGID LangId(const char *lang, const char *sublang);
	
	/**
	 * A constructor taking a language id as parameter.
	 * @param [in] id Language ID to use.
	 */
	explicit LangFileInfo(LANGID id): id(id) { };
	
	explicit LangFileInfo(LPCTSTR path);
	String GetString(LCTYPE type) const;

private:
	struct rg
	{
		LANGID id;
		const char *lang;
	};
	static const struct rg rg[];
};

/**
 * @brief An array holding language IDs and names.
 */
const struct LangFileInfo::rg LangFileInfo::rg[] =
{
	{
		LANG_AFRIKAANS,		"AFRIKAANS\0"
	},
	{
		LANG_ALBANIAN,		"ALBANIAN\0"
	},
	{
		LANG_ARABIC,		"ARABIC\0"						"SAUDI_ARABIA\0"
															"IRAQ\0"
															"EGYPT\0"
															"LIBYA\0"
															"ALGERIA\0"
															"MOROCCO\0"
															"TUNISIA\0"
															"OMAN\0"
															"YEMEN\0"
															"SYRIA\0"
															"JORDAN\0"
															"LEBANON\0"
															"KUWAIT\0"
															"UAE\0"
															"BAHRAIN\0"
															"QATAR\0"
	},
	{
		LANG_ARMENIAN,		"ARMENIAN\0"
	},
	{
		LANG_ASSAMESE,		"ASSAMESE\0"
	},
	{
		LANG_AZERI,			"AZERI\0"						"LATIN\0"
															"CYRILLIC\0"
	},
	{
		LANG_BASQUE,		"BASQUE\0"
	},
	{
		LANG_BELARUSIAN,	"BELARUSIAN\0"
	},
	{
		LANG_BENGALI,		"BENGALI\0"
	},
	{
		LANG_BULGARIAN,		"BULGARIAN\0"
	},
	{
		LANG_CATALAN,		"CATALAN\0"
	},
	{
		LANG_CHINESE,		"CHINESE\0"						"TRADITIONAL\0"
															"SIMPLIFIED\0"
															"HONGKONG\0"
															"SINGAPORE\0"
															"MACAU\0"
	},
	{
		LANG_CROATIAN,		"CROATIAN\0"
	},
	{
		LANG_CZECH,			"CZECH\0"
	},
	{
		LANG_DANISH,		"DANISH\0"
	},
	{
		LANG_DIVEHI,		"DIVEHI\0"
	},
	{
		MAKELANGID(LANG_DUTCH, SUBLANG_DUTCH),				"DUTCH\0"
															"BELGIAN\0"
	},
	{
		LANG_ENGLISH,		"ENGLISH\0"						"US\0"
															"UK\0"
															"AUS\0"
															"CAN\0"
															"NZ\0"
															"EIRE\0"
															"SOUTH_AFRICA\0"
															"JAMAICA\0"
															"CARIBBEAN\0"
															"BELIZE\0"
															"TRINIDAD\0"
															"ZIMBABWE\0"
															"PHILIPPINES\0"
	},
	{
		LANG_ESTONIAN,		"ESTONIAN\0"
	},
	{
		LANG_FAEROESE,		"FAEROESE\0"
	},
	{
		LANG_FARSI,			"FARSI\0"
	},
	{
		LANG_FINNISH,		"FINNISH\0"
	},
	{
		MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH),			"FRENCH\0"
															"BELGIAN\0"
															"CANADIAN\0"
															"SWISS\0"
															"LUXEMBOURG\0"
															"MONACO\0"
	},
	{
		LANG_GALICIAN,		"GALICIAN\0"
	},
	{
		LANG_GEORGIAN,		"GEORGIAN\0"
	},
	{
		MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN),			"GERMAN\0"
															"SWISS\0"
															"AUSTRIAN\0"
															"LUXEMBOURG\0"
															"LIECHTENSTEIN"
	},
	{
		LANG_GREEK,			"GREEK\0"
	},
	{
		LANG_GUJARATI,		"GUJARATI\0"
	},
	{
		LANG_HEBREW,		"HEBREW\0"
	},
	{
		LANG_HINDI,			"HINDI\0"
	},
	{
		LANG_HUNGARIAN,		"HUNGARIAN\0"
	},
	{
		LANG_ICELANDIC,		"ICELANDIC\0"
	},
	{
		LANG_INDONESIAN,	"INDONESIAN\0"
	},
	{
		MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN),			"ITALIAN\0"
															"SWISS\0"
	},
	{
		LANG_JAPANESE,		"JAPANESE\0"
	},
	{
		LANG_KANNADA,		"KANNADA\0"
	},
	{
		MAKELANGID(LANG_KASHMIRI, SUBLANG_DEFAULT),			"KASHMIRI\0"
															"SASIA\0"
	},
	{
		LANG_KAZAK,			"KAZAK\0"
	},
	{
		LANG_KONKANI,		"KONKANI\0"
	},
	{
		MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN),			"KOREAN\0"
	},
	{
		LANG_KYRGYZ,		"KYRGYZ\0"
	},
	{
		LANG_LATVIAN,		"LATVIAN\0"
	},
	{
		MAKELANGID(LANG_LITHUANIAN, SUBLANG_LITHUANIAN),	"LITHUANIAN\0"
	},
	{
		LANG_MACEDONIAN,	"MACEDONIAN\0"
	},
	{
		LANG_MALAY,			"MALAY\0"						"MALAYSIA\0"
															"BRUNEI_DARUSSALAM\0"
	},
	{
		LANG_MALAYALAM,		"MALAYALAM\0"
	},
	{
		LANG_MANIPURI,		"MANIPURI\0"
	},
	{
		LANG_MARATHI,		"MARATHI\0"
	},
	{
		LANG_MONGOLIAN,		"MONGOLIAN\0"
	},
	{
		MAKELANGID(LANG_NEPALI, SUBLANG_DEFAULT),			"NEPALI\0"
															"INDIA\0"
	},
	{
		LANG_NORWEGIAN,		"NORWEGIAN\0"					"BOKMAL\0"
															"NYNORSK\0"
	},
	{
		LANG_ORIYA,			"ORIYA\0"
	},
	{
		LANG_POLISH,		"POLISH\0"
	},
	{
		MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE),	"PORTUGUESE\0"
															"BRAZILIAN\0"
	},
	{
		LANG_PUNJABI,		"PUNJABI\0"
	},
	{
		LANG_ROMANIAN,		"ROMANIAN\0"
	},
	{
		LANG_RUSSIAN,		"RUSSIAN\0"
	},
	{
		LANG_SANSKRIT,		"SANSKRIT\0"
	},
	{
		MAKELANGID(LANG_SERBIAN, SUBLANG_DEFAULT),			"SERBIAN\0"
															"LATIN\0"
															"CYRILLIC\0"
	},
	{
		LANG_SINDHI,		"SINDHI\0"
	},
	{
		LANG_SLOVAK,		"SLOVAK\0"
	},
	{
		LANG_SLOVENIAN,		"SLOVENIAN\0"
	},
	{
		MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH),			"SPANISH\0"
															"MEXICAN\0"
															"MODERN\0"
															"GUATEMALA\0"
															"COSTA_RICA\0"
															"PANAMA\0"
															"DOMINICAN_REPUBLIC\0"
															"VENEZUELA\0"
															"COLOMBIA\0"
															"PERU\0"
															"ARGENTINA\0"
															"ECUADOR\0"
															"CHILE\0"
															"URUGUAY\0"
															"PARAGUAY\0"
															"BOLIVIA\0"
															"EL_SALVADOR\0"
															"HONDURAS\0"
															"NICARAGUA\0"
															"PUERTO_RICO\0"
	},
	{
		LANG_SWAHILI,		"SWAHILI\0"
	},
	{
		MAKELANGID(LANG_SWEDISH, SUBLANG_SWEDISH),			"SWEDISH\0"
															"FINLAND\0"
	},
	{
		LANG_SYRIAC,		"SYRIAC\0"
	},
	{
		LANG_TAMIL,			"TAMIL\0"
	},
	{
		LANG_TATAR,			"TATAR\0"
	},
	{
		LANG_TELUGU,		"TELUGU\0"
	},
	{
		LANG_THAI,			"THAI\0"
	},
	{
		LANG_TURKISH,		"TURKISH\0"
	},
	{
		LANG_UKRAINIAN,		"UKRAINIAN\0"
	},
	{
		LANG_URDU,			"URDU\0"						"PAKISTAN\0"
															"INDIA\0"
	},
	{
		LANG_UZBEK,			"UZBEK\0"						"LATIN\0"
															"CYRILLIC\0"
	},
	{
		LANG_VIETNAMESE,	"VIETNAMESE\0"
	},
};

/**
 * @brief Get a language ID for given language + sublanguage.
 * @param [in] lang Language name.
 * @param [in] sublang Sub language name.
 * @return Language ID.
 */
LANGID LangFileInfo::LangId(const char *lang, const char *sublang)
{
	// binary search the array for passed in lang
	size_t lower = 0;
	size_t upper = countof(rg);
	while (lower < upper)
	{
		size_t match = (upper + lower) >> 1;
		int cmp = strcmp(rg[match].lang, lang);
		if (cmp >= 0)
			upper = match;
		if (cmp <= 0)
			lower = match + 1;
	}
	if (lower <= upper)
		return 0;
	LANGID baseid = rg[upper].id;
	// figure out sublang
	if ((baseid & ~0x3ff) && *sublang == '\0')
		return baseid;
	LANGID id = PRIMARYLANGID(baseid);
	if (0 == strcmp(sublang, "DEFAULT"))
		return MAKELANGID(id, SUBLANG_DEFAULT);
	const char *sub = rg[upper].lang;
	do
	{
		do
		{
			id += MAKELANGID(0, 1);
		} while (id == baseid);
		sub += strlen(sub) + 1;
		if (0 == strcmp(sublang, sub))
			return id;
	} while (*sub);
	return 0;
}

/**
 * @brief A constructor taking a path to language file as parameter.
 * @param [in] path Full path to the language file.
 */
LangFileInfo::LangFileInfo(LPCTSTR path)
: id(0)
{
	FILE *f;
	if (_tfopen_s(&f, path, _T("r")) == 0)
	{
		char buf[1024 + 1];
		while (fgets(buf, sizeof buf - 1, f))
		{
			int i = 0;
			strcat_s(buf, "1");
			sscanf_s(buf, "msgid \" LANG_ENGLISH , SUBLANG_ENGLISH_US \" %d", &i);
			if (i)
			{
				if (fgets(buf, sizeof buf, f))
				{
					char *lang = strstr(buf, "LANG_");
					char *sublang = strstr(buf, "SUBLANG_");
					char *langNext = nullptr;
					char *sublangNext = nullptr;
					if (lang && sublang)
					{
						strtok_s(lang, ",\" \t\r\n", &langNext);
						strtok_s(sublang, ",\" \t\r\n", &sublangNext);
						lang += sizeof "LANG";
						sublang += sizeof "SUBLANG";
						if (0 != strcmp(sublang, "DEFAULT"))
						{
							sublang = EatPrefix(sublang, lang);
							if (sublang && *sublang)
								sublang = EatPrefix(sublang, "_");
						}
						if (sublang)
							id = LangId(lang, sublang);
					}
				}
				break;
			}
		}
		fclose(f);
	}
}

String LangFileInfo::GetString(LCTYPE type) const
{
	String s;
	if (int cch = GetLocaleInfo(id, type, 0, 0))
	{
		s.resize(cch - 1);
		GetLocaleInfo(id, type, &*s.begin(), cch);
	}
	return s;
}

static HANDLE NTAPI FindFile(HANDLE h, LPCTSTR path, WIN32_FIND_DATA *fd)
{
	if (h == INVALID_HANDLE_VALUE)
	{
		h = FindFirstFile(path, fd);
	}
	else if (fd->dwFileAttributes == INVALID_FILE_ATTRIBUTES || !FindNextFile(h, fd))
	{
		FindClose(h);
		h = INVALID_HANDLE_VALUE;
	}
	return h;
}

/////////////////////////////////////////////////////////////////////////////
// CLanguageSelect dialog

/** @brief Default English language. */
const WORD wSourceLangId = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);

CLanguageSelect::CLanguageSelect()
: m_hCurrentDll(0)
, m_wCurLanguage(wSourceLangId)
{
	SetThreadLocale(MAKELCID(m_wCurLanguage, SORT_DEFAULT));
}

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
	if (size_t len = strlen(prefix))
		if (_memicmp(text, prefix, len) == 0)
			return text + len;
	return 0;
}

/**
 * @brief Convert C style \\nnn, \\r, \\n, \\t etc into their indicated characters.
 * @param [in] codepage Codepage to use in conversion.
 * @param [in,out] s String to convert.
 */
static void unslash(unsigned codepage, std::string &s)
{
	char *p = &*s.begin();
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
	s.resize(p - 1 - &*s.begin());
}

/**
 * @brief Load language.file
 * @param [in] wLangId 
 * @return TRUE on success, FALSE otherwise.
 */
BOOL CLanguageSelect::LoadLanguageFile(LANGID wLangId, BOOL bShowError)
{
	String strPath = GetFileName(wLangId);
	if (strPath.empty())
		return FALSE;

	m_hCurrentDll = LoadLibrary(_T("MergeLang.dll"));
	// There is no point in translating error messages about inoperational
	// translation system, so go without string resources here.
	if (m_hCurrentDll == 0)
	{
		if (bShowError)
			AfxMessageBox(_T("Failed to load MergeLang.dll"), MB_ICONSTOP);
		return FALSE;
	}
	CVersionInfo viInstance(AfxGetInstanceHandle());
	unsigned instanceVerMS = 0;
	unsigned instanceVerLS = 0;
	viInstance.GetFixedFileVersion(instanceVerMS, instanceVerLS);
	CVersionInfo viResource(m_hCurrentDll);
	unsigned resourceVerMS = 0;
	unsigned resourceVerLS = 0;
	viResource.GetFixedFileVersion(resourceVerMS, resourceVerLS);
	if (instanceVerMS != resourceVerMS || instanceVerLS != resourceVerLS)
	{
		FreeLibrary(m_hCurrentDll);
		m_hCurrentDll = 0;
		if (bShowError)
			AfxMessageBox(_T("MergeLang.dll version mismatch"), MB_ICONSTOP);
		return FALSE;
	}
	HRSRC mergepot = FindResource(m_hCurrentDll, _T("MERGEPOT"), RT_RCDATA);
	if (mergepot == 0)
	{
		if (bShowError)
			AfxMessageBox(_T("MergeLang.dll is invalid"), MB_ICONSTOP);
		return FALSE;
	}
	size_t size = SizeofResource(m_hCurrentDll, mergepot);
	const char *data = (const char *)LoadResource(m_hCurrentDll, mergepot);
	char buf[1024];
	std::string *ps = 0;
	std::string msgid;
	unsigned uid = 0;
	bool found_uid = false;
	int unresolved = 0;
	int mismatched = 0;
	while (const char *eol = (const char *)memchr(data, '\n', size))
	{
		size_t len = eol - data;
		if (len >= sizeof buf)
		{
			ASSERT(FALSE);
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
				uid = strtoul(q + 1, &q, 16);
				found_uid = true;
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
			if (std::string::size_type n = q - p)
			{
				ps->append(p + 1, n - 1);
			}
			else
			{
				ps = 0;
				// avoid dereference of empty vector or last vector
				if (found_uid)
				{
					unslash(0, msgid);
					m_map_msgid_to_uid.insert(std::make_pair(msgid, uid));
					m_map_uid_to_msgid.insert(std::make_pair(uid, msgid));
				}
				found_uid = false;
				msgid.erase();
			}
		}
	}
	FILE *f;
	if (_tfopen_s(&f, strPath.c_str(), _T("r")) != 0)
	{
		FreeLibrary(m_hCurrentDll);
		m_hCurrentDll = 0;
		if (bShowError)
		{
			String str = _T("Failed to load ") + strPath;
			AfxMessageBox(str.c_str(), MB_ICONSTOP);
		}
		return FALSE;
	}
	ps = 0;
	msgid.erase();
	found_uid = false;
	std::string format;
	std::string msgstr;
	std::string directive;
	while (fgets(buf, sizeof buf, f))
	{
		if (char *p0 = EatPrefix(buf, "#:"))
		{
			if (char *q = strchr(p0, ':'))
			{
				uid = strtoul(q + 1, &q, 16);
				found_uid = true;
				--unresolved;
			}
		}
		else if (char *p1 = EatPrefix(buf, "#,"))
		{
			format = p1;
			format.erase(0, format.find_first_not_of(" \t\r\n"));
			format.erase(format.find_last_not_of(" \t\r\n") + 1);
		}
		else if (char *p2 = EatPrefix(buf, "#."))
		{
			directive = p2;
			directive.erase(0, directive.find_first_not_of(" \t\r\n"));
			directive.erase(directive.find_last_not_of(" \t\r\n") + 1);
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
			if (std::string::size_type n = q - p)
			{
				ps->append(p + 1, n - 1);
			}
			else
			{
				ps = 0;
				if (!msgid.empty())
					unslash(0, msgid);
				if (msgstr.empty())
					msgstr = msgid;
				unslash(m_codepage, msgstr);
				// avoid dereference of empty vector or last vector
				if (found_uid)
				{
					if (m_map_uid_to_msgid.find(uid) != m_map_uid_to_msgid.end() && m_map_uid_to_msgid.at(uid) == msgid)
						m_map_uid_to_msgid[uid] = msgstr;
					else
						++mismatched;
				}
				found_uid = false;
				if (directive == "Codepage")
				{
					m_codepage = strtol(msgstr.c_str(), &p, 10);
					directive.erase();
				}
				msgid.erase();
				msgstr.erase();
			}
		}
	}
	fclose(f);
	if (unresolved != 0 || mismatched != 0)
	{
		FreeLibrary(m_hCurrentDll);
		m_hCurrentDll = 0;
		m_map_uid_to_msgid.clear();
		m_map_msgid_to_uid.clear();
		m_codepage = 0;
		if (bShowError)
		{
			String str = _T("Unresolved or mismatched references detected when ")
				_T("attempting to read translations from\n") + strPath;
			AfxMessageBox(str.c_str(), MB_ICONSTOP);
		}
		return FALSE;
	}
	return TRUE;
}

/**
 * @brief Set UI language.
 * @param [in] wLangId 
 * @return TRUE on success, FALSE otherwise.
 */
BOOL CLanguageSelect::SetLanguage(LANGID wLangId, BOOL bShowError)
{
	if (wLangId == 0)
		return FALSE;
	if (m_wCurLanguage == wLangId)
		return TRUE;
	// reset the resource handle
	AfxSetResourceHandle(AfxGetInstanceHandle());
	// free the existing DLL
	if (m_hCurrentDll)
	{
		FreeLibrary(m_hCurrentDll);
		m_hCurrentDll = NULL;
	}
	m_map_uid_to_msgid.clear();
	m_map_msgid_to_uid.clear();
	m_codepage = 0;
	if (wLangId != wSourceLangId)
	{
		if (LoadLanguageFile(wLangId, bShowError))
			AfxSetResourceHandle(m_hCurrentDll);
		else
			wLangId = wSourceLangId;
	}
	m_wCurLanguage = wLangId;
	SetThreadLocale(MAKELCID(m_wCurLanguage, SORT_DEFAULT));
	return TRUE;
}

/**
 * @brief Get a language file for the specified language ID.
 * This function gets a language file name for the given language ID. Language
 * files are currently named as [languagename].po.
 * @param [in] wLangId Language ID.
 * @return Language filename, or empty string if no file for language found.
 */
String CLanguageSelect::GetFileName(LANGID wLangId) const
{
	String filename;
	String path = paths::ConcatPath(env::GetProgPath(), szRelativePath);
	String pattern = paths::ConcatPath(path, _T("*.po"));
	WIN32_FIND_DATA ff;
	HANDLE h = INVALID_HANDLE_VALUE;
	while ((h = FindFile(h, pattern.c_str(), &ff)) != INVALID_HANDLE_VALUE)
	{
		filename = paths::ConcatPath(path, ff.cFileName);
		LangFileInfo lfi(filename.c_str());
		if (lfi.id == wLangId)
			ff.dwFileAttributes = INVALID_FILE_ATTRIBUTES; // terminate loop
		else
			filename.erase();
	}
	return filename;
}

/////////////////////////////////////////////////////////////////////////////
// CLanguageSelect commands

bool CLanguageSelect::TranslateString(unsigned uid, std::string &s) const
{
	if (m_map_uid_to_msgid.find(uid) != m_map_uid_to_msgid.end())
	{
		s = m_map_uid_to_msgid.at(uid);
		unsigned codepage = GetACP();
		if (m_codepage != codepage)
		{
			// Attempt to convert to UI codepage
			if (size_t len = s.length())
			{
				std::wstring ws;
				ws.resize(len);
				len = MultiByteToWideChar(m_codepage, 0, s.c_str(), -1, &*ws.begin(), static_cast<int>(len) + 1);
				if (len)
				{
					ws.resize(len - 1);
					len = WideCharToMultiByte(codepage, 0, ws.c_str(), -1, 0, 0, 0, 0);
					if (len)
					{
						s.resize(len - 1);
						WideCharToMultiByte(codepage, 0, ws.c_str(), -1, &*s.begin(), static_cast<int>(len), 0, 0);
					}
				}
			}
		}
		return true;
	}
	return false;
}

bool CLanguageSelect::TranslateString(unsigned uid, std::wstring &ws) const
{
	if (m_map_uid_to_msgid.find(uid) != m_map_uid_to_msgid.end())
	{
		if (size_t len = m_map_uid_to_msgid.at(uid).length())
		{
			ws.resize(len);
			const char *msgstr = m_map_uid_to_msgid.at(uid).c_str();
			len = MultiByteToWideChar(m_codepage, 0, msgstr, -1, &*ws.begin(), static_cast<int>(len) + 1);
			ws.resize(len - 1);
			return true;
		}
	}
	return false;
}

bool CLanguageSelect::TranslateString(const std::string& str, String &translated_str) const
{
	EngMsgIDToUIDMap::const_iterator it = m_map_msgid_to_uid.find(str);
	if (it != m_map_msgid_to_uid.end())
	{
		return TranslateString(it->second, translated_str);
	}
	translated_str = ucr::toTString(str);
	return false;
}

void CLanguageSelect::SetIndicators(CStatusBar &sb, const UINT *rgid, int n) const
{
	HGDIOBJ hf = (HGDIOBJ)sb.SendMessage(WM_GETFONT);
	CClientDC dc(0);
	if (hf)
		hf = dc.SelectObject(hf);
	if (n)
		sb.SetIndicators(0, n);
	else
		n = sb.m_nCount;
	int cx = ::GetSystemMetrics(SM_CXSCREEN) / 4;	// default to 1/4 the screen width
	UINT style = SBPS_STRETCH | SBPS_NOBORDERS;		// first pane is stretchy
	for (int i = 0 ; i < n ; ++i)
	{
		UINT id = rgid ? rgid[i] : sb.GetItemID(i);
		if (id >= ID_INDICATOR_EXT)
		{
			String text = LoadString(id);
			int cx1 = dc.GetTextExtent(text.c_str(), static_cast<int>(text.length())).cx;
			sb.SetPaneInfo(i, id, style | SBPS_DISABLED, cx1);
			sb.SetPaneText(i, text.c_str(), FALSE);
		}
		else if (rgid)
		{
			sb.SetPaneInfo(i, 0, style, cx);
		}
		style = 0;
	}
	if (hf)
		hf = dc.SelectObject(hf);
	// Send WM_SIZE to get pane rectangles right
	RECT rect;
	sb.GetClientRect(&rect);
	sb.SendMessage(WM_SIZE, 0, MAKELPARAM(rect.right, rect.bottom));
}

void CLanguageSelect::TranslateMenu(HMENU h) const
{
	int i = ::GetMenuItemCount(h);
	while (i > 0)
	{
		--i;
		MENUITEMINFO mii = {0};
#if(WINVER >= 0x0500)
		mii.cbSize = sizeof mii - sizeof HBITMAP;
#else
		mii.cbSize = sizeof mii;
#endif
		mii.fMask = MIIM_STATE|MIIM_ID|MIIM_SUBMENU|MIIM_DATA;
		::GetMenuItemInfo(h, i, TRUE, &mii);
		if (mii.hSubMenu)
		{
			TranslateMenu(mii.hSubMenu);
			mii.wID = static_cast<UINT>(reinterpret_cast<uintptr_t>(mii.hSubMenu));
		}
		if (BCMenuData *pItemData = reinterpret_cast<BCMenuData *>(mii.dwItemData))
		{
			if (LPCWSTR text = pItemData->GetWideString())
			{
				unsigned uid = 0;
				swscanf_s(text, L"Merge.rc:%x", &uid);
				std::wstring s;
				if (TranslateString(uid, s))
					pItemData->SetWideString(s.c_str());
			}
		}
		TCHAR text[80];
		if (::GetMenuString(h, i, text, countof(text), MF_BYPOSITION))
		{
			unsigned uid = 0;
			_stscanf_s(text, _T("Merge.rc:%x"), &uid);
			String s;
			if (TranslateString(uid, s))
				::ModifyMenu(h, i, mii.fState | MF_BYPOSITION, mii.wID, s.c_str());
		}
	}
}

void CLanguageSelect::TranslateDialog(HWND h) const
{
	UINT gw = GW_CHILD;
	do
	{
		TCHAR text[80];
		::GetWindowText(h, text, countof(text));
		unsigned uid = 0;
		_stscanf_s(text, _T("Merge.rc:%x"), &uid);
		String s;
		if (TranslateString(uid, s))
			::SetWindowText(h, s.c_str());
		h = ::GetWindow(h, gw);
		gw = GW_HWNDNEXT;
	} while (h);
}

String CLanguageSelect::LoadString(UINT id) const
{
	String s;
	if (id)
	{
		TCHAR text[1024];
		AfxLoadString(id, text, countof(text));
		unsigned uid = 0;
		_stscanf_s(text, _T("Merge.rc:%x"), &uid);
		if (!TranslateString(uid, s))
			s = text;
	}
	return s;
}

std::wstring CLanguageSelect::LoadDialogCaption(LPCTSTR lpDialogTemplateID) const
{
	std::wstring s;
	if (HINSTANCE hInst = AfxFindResourceHandle(lpDialogTemplateID, RT_DIALOG))
	{
		if (HRSRC hRsrc = FindResource(hInst, lpDialogTemplateID, RT_DIALOG))
		{
			if (LPCWSTR text = (LPCWSTR)LoadResource(hInst, hRsrc))
			{
				// Skip DLGTEMPLATE or DLGTEMPLATEEX
				text += text[1] == 0xFFFF ? 13 : 9;
				// Skip menu name string or ordinal
				if (*text == (const WCHAR)-1)
					text += 2; // WCHARs
				else
					while (*text++);
				// Skip class name string or ordinal
				if (*text == (const WCHAR)-1)
					text += 2; // WCHARs
				else
					while (*text++);
				// Caption string is ahead
				unsigned uid = 0;
				swscanf_s(text, L"Merge.rc:%x", &uid);
				if (!TranslateString(uid, s))
					s = text;
			}
		}
	}
	return s;
}

/**
 * @brief Load languages available on disk, and display in list, and select current
 */
std::vector<std::pair<LANGID, String> > CLanguageSelect::GetAvailableLanguages() const
{
	std::vector<std::pair<LANGID, String> > list;
	String path = paths::ConcatPath(env::GetProgPath(), szRelativePath);
	String pattern = paths::ConcatPath(path, _T("*.po"));
	WIN32_FIND_DATA ff;
	HANDLE h = INVALID_HANDLE_VALUE;
	do
	{
		LangFileInfo lfi(wSourceLangId);
		if (h != INVALID_HANDLE_VALUE)
			lfi = LangFileInfo(paths::ConcatPath(path, ff.cFileName).c_str());
		String str;
		str += lfi.GetString(LOCALE_SLANGUAGE);
		str += _T(" - ");
		str += lfi.GetString(LOCALE_SNATIVELANGNAME | LOCALE_USE_CP_ACP);
		str += _T(" (");
		str += lfi.GetString(LOCALE_SNATIVECTRYNAME | LOCALE_USE_CP_ACP);
		str += _T(")");
		str += _T(" - ");
		str += lfi.GetString(LOCALE_SENGLANGUAGE);
		str += _T(" (");
		str += lfi.GetString(LOCALE_SENGCOUNTRY);
		str += _T(")");
		list.emplace_back(lfi.id, str);
	} while ((h = FindFile(h, pattern.c_str(), &ff)) != INVALID_HANDLE_VALUE);
	return list;
}

/**
 * @brief Find DLL entry in lang_map for language for specified locale
 */
static WORD GetLangFromLocale(LCID lcid)
{
	TCHAR buff[8] = {0};
	WORD langID = 0;
	if (GetLocaleInfo(lcid, LOCALE_IDEFAULTLANGUAGE, buff, countof(buff)))
		_stscanf_s(buff, _T("%4hx"), &langID);
	return langID;
}

void CLanguageSelect::InitializeLanguage(WORD langID)
{
	ASSERT(LangFileInfo::LangId("GERMAN", "") == MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN));
	ASSERT(LangFileInfo::LangId("GERMAN", "DEFAULT") == MAKELANGID(LANG_GERMAN, SUBLANG_DEFAULT));
	ASSERT(LangFileInfo::LangId("GERMAN", "SWISS") == MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_SWISS));
	ASSERT(LangFileInfo::LangId("PORTUGUESE", "") == MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE));
	ASSERT(LangFileInfo::LangId("NORWEGIAN", "BOKMAL") == MAKELANGID(LANG_NORWEGIAN, SUBLANG_NORWEGIAN_BOKMAL));
	ASSERT(LangFileInfo::LangId("NORWEGIAN", "NYNORSK") == MAKELANGID(LANG_NORWEGIAN, SUBLANG_NORWEGIAN_NYNORSK));

	//TRACE(_T("%hs\n"), LangFileInfo::FileName(MAKELANGID(LANG_NORWEGIAN, SUBLANG_NORWEGIAN_BOKMAL)).c_str());
	//TRACE(_T("%hs\n"), LangFileInfo::FileName(MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE)).c_str());
	//TRACE(_T("%hs\n"), LangFileInfo::FileName(MAKELANGID(LANG_GERMAN, SUBLANG_DEFAULT)).c_str());

	if (langID)
	{
		// User has set a language override
		SetLanguage(langID);
		return;
	}
	// User has not specified a language
	// so look in thread locale, user locale, and then system locale for
	// a language that WinMerge supports
	WORD Lang1 = GetLangFromLocale(GetThreadLocale());
	if (SetLanguage(Lang1))
		return;
	WORD Lang2 = GetLangFromLocale(LOCALE_USER_DEFAULT);
	if (Lang2 != Lang1 && SetLanguage(Lang2))
		return;
	WORD Lang3 = GetLangFromLocale(LOCALE_SYSTEM_DEFAULT);
	if (Lang3 != Lang2 && Lang3 != Lang1 && SetLanguage(Lang3))
		return;
}
