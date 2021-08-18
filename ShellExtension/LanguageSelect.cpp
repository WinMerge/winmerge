/**
 * @file  LanguageSelect.cpp
 *
 * @brief Implements the Language Selection dialog class (which contains the language data)
 */

#include "StdAfx.h"
#include "LanguageSelect.h"

// Escaped character constants in range 0x80-0xFF are interpreted in current codepage
// Using C locale gets us direct mapping to Unicode codepoints
#pragma setlocale("C")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static wchar_t *EatPrefix(wchar_t *text, const wchar_t *prefix);
static void unslash(std::wstring &s);
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
	std::wstring GetString(LCTYPE type) const;

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
		LANG_SINHALESE,		"SINHALESE\0"
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
	size_t upper = std::size(rg);
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
	if (_tfopen_s(&f, path, _T("r,ccs=utf-8")) == 0 && f)
	{
		wchar_t buf[1024 + 1];
		while (fgetws(buf, static_cast<int>(std::size(buf)) - 1, f) != nullptr)
		{
			int i = 0;
			wcscat_s(buf, L"1");
			swscanf_s(buf, L"msgid \" LANG_ENGLISH , SUBLANG_ENGLISH_US \" %d", &i);
			if (i)
			{
				if (fgetws(buf, static_cast<int>(std::size(buf)), f) != nullptr)
				{
					wchar_t *lang = wcsstr(buf, L"LANG_");
					wchar_t *sublang = wcsstr(buf, L"SUBLANG_");
					wchar_t *langNext = nullptr;
					wchar_t *sublangNext = nullptr;
					if (lang && sublang)
					{
						wcstok_s(lang, L",\" \t\r\n", &langNext);
						wcstok_s(sublang, L",\" \t\r\n", &sublangNext);
						lang += std::size("LANG");
						sublang += std::size("SUBLANG");
						if (0 != wcscmp(sublang, L"DEFAULT"))
						{
							sublang = EatPrefix(sublang, lang);
							if (sublang && *sublang)
								sublang = EatPrefix(sublang, L"_");
						}
						if (sublang)
						{
							USES_CONVERSION;
							id = LangId(W2A(lang), W2A(sublang));
						}
					}
				}
				break;
			}
		}
		fclose(f);
	}
}

std::wstring LangFileInfo::GetString(LCTYPE type) const
{
	std::wstring s;
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

CLanguageSelect::CLanguageSelect()
	: m_langId(0)
{
}

/**
 * @brief Remove prefix from the string.
 * @param [in] text String from which to jump over prefix.
 * @param [in] prefix Prefix string to jump over.
 * @return String without the prefix.
 * @note Function returns pointer to original string,
 *  it does not allocate a new string.
 */
static wchar_t *EatPrefix(wchar_t *text, const wchar_t *prefix)
{
	if (size_t len = wcslen(prefix))
		if (_memicmp(text, prefix, len * sizeof(wchar_t)) == 0)
			return text + len;
	return 0;
}

/**
 * @brief Convert C style \\nnn, \\r, \\n, \\t etc into their indicated characters.
 * @param [in,out] s String to convert.
 */
static void unslash(std::wstring &s)
{
	wchar_t *p = &*s.begin();
	wchar_t *q = p;
	wchar_t c = {};
	do
	{
		wchar_t *r = q + 1;
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
				*p = (wchar_t)wcstol(r, &q, 16);
				break;
			default:
				*p = (wchar_t)wcstol(r - 1, &q, 8);
				break;
			}
			if (q >= r)
				break;
			[[fallthrough]];
		default:
			*p = c;
			q = r;
		}
		++p;
	} while (c != '\0');
	s.resize(p - 1 - &*s.begin());
}

/**
 * @brief Load language.file
 * @param [in] wLangId 
 * @return `true` on success, `false` otherwise.
 */
bool CLanguageSelect::LoadLanguageFile(LANGID wLangId, const std::wstring& sLanguagesFolder)
{
	std::wstring strPath = GetFileName(wLangId, sLanguagesFolder);
	if (strPath.empty())
		return false;

	wchar_t buf[1024];
	std::wstring *ps = nullptr;
	std::wstring msgctxt;
	std::wstring msgid;
	FILE *f;
	if (_tfopen_s(&f, strPath.c_str(), _T("r,ccs=UTF-8")) != 0)
		return false;
	ps = nullptr;
	std::wstring format;
	std::wstring msgstr;
	std::wstring directive;
	auto addToMap = [&]() {
		ps = nullptr;
		if (!msgctxt.empty())
			unslash(msgctxt);
		if (!msgid.empty())
			unslash(msgid);
		if (msgstr.empty())
			msgstr = msgid;
		unslash(msgstr);
		if (!msgid.empty())
		{
			if (msgctxt.empty())
				m_map_msgid_to_msgstr.insert_or_assign(msgid, msgstr);
			else
				m_map_msgid_to_msgstr.insert_or_assign(L"\x01\"" + msgctxt + L"\"" + msgid, msgstr);
		}
		msgctxt.erase();
		msgid.erase();
		msgstr.erase();
	};
	while (fgetws(buf, static_cast<int>(std::size(buf)), f) != nullptr)
	{
		if (wchar_t *p1 = EatPrefix(buf, L"#,"))
		{
			format = p1;
			format.erase(0, format.find_first_not_of(L" \t\r\n"));
			format.erase(format.find_last_not_of(L" \t\r\n") + 1);
		}
		else if (wchar_t *p2 = EatPrefix(buf, L"#."))
		{
			directive = p2;
			directive.erase(0, directive.find_first_not_of(L" \t\r\n"));
			directive.erase(directive.find_last_not_of(L" \t\r\n") + 1);
		}
		else if (EatPrefix(buf, L"msgctxt "))
		{
			ps = &msgctxt;
		}
		else if (EatPrefix(buf, L"msgid "))
		{
			ps = &msgid;
		}
		else if (EatPrefix(buf, L"msgstr "))
		{
			ps = &msgstr;
		}
		if (ps != nullptr)
		{
			wchar_t *p = wcschr(buf, '"');
			wchar_t *q = wcsrchr(buf, '"');
			if (std::wstring::size_type n = q - p)
			{
				ps->append(p + 1, n - 1);
			}
			else
			{
				addToMap();
			}
		}
	}
	if (ps != nullptr)
		addToMap();
	fclose(f);

	m_langId = wLangId;

	return true;
}

/**
 * @brief Get a language file for the specified language ID.
 * This function gets a language file name for the given language ID. Language
 * files are currently named as [languagename].po.
 * @param [in] wLangId Language ID.
 * @return Language filename, or empty string if no file for language found.
 */
std::wstring CLanguageSelect::GetFileName(LANGID wLangId, const std::wstring& sLanguagesFolder)
{
	std::wstring filename;
	std::wstring path = sLanguagesFolder;
	if (!path.empty() && path.back() != '\\')
		path += '\\';
	std::wstring pattern = path + L"*.po";
	WIN32_FIND_DATA ff;
	HANDLE h = INVALID_HANDLE_VALUE;
	while ((h = FindFile(h, pattern.c_str(), &ff)) != INVALID_HANDLE_VALUE)
	{
		filename = path + ff.cFileName;
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

bool CLanguageSelect::TranslateString(const std::wstring& msgid, std::wstring &s) const
{
	if (m_map_msgid_to_msgstr.find(msgid) != m_map_msgid_to_msgstr.end())
	{
		s = m_map_msgid_to_msgstr.at(msgid);
		return true;
	}
	if (msgid.length() > 2 && msgid[0] == '\x01' && msgid[1] == '"')
	{
		size_t pos = msgid.find('"', 2);
		if (pos != std::wstring::npos)
		{
			s = msgid.substr(pos + 1);
			return true;
		}
	}
	s = msgid;
	return false;
}

