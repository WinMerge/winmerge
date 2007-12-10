/**
 * @file  LanguageSelect.cpp
 *
 * @brief Implements the Language Selection dialog class (which contains the language data)
 */
// ID line follows -- this is updated by SVN
// $Id$


#include "stdafx.h"
#include "merge.h"
#include "version.h"
#include "resource.h"
#include "LanguageSelect.h"
#include "BCMenu.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "DirFrame.h"
#include "coretools.h"
#include <locale.h>
#include <sstream>

// Escaped character constants in range 0x80-0xFF are interpreted in current codepage
// Using C locale gets us direct mapping to Unicode codepoints
#pragma setlocale("C")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** @brief Relative path to WinMerge executable for lang files. */
static const TCHAR szRelativePath[] = _T("\\Languages\\");

static char *EatPrefix(char *text, const char *prefix);
static void unslash(unsigned codepage, std::string &s);
static HANDLE NTAPI FindFile(HANDLE h, LPCTSTR path, WIN32_FIND_DATA *fd);

class LangFileInfo
{
public:
	LANGID id;
	static LANGID LangId(const char *lang, const char *sublang);
	//static std::string FileName(LANGID id);
	LangFileInfo(LANGID id): id(id) { };
	LangFileInfo(LPCTSTR path);
	String GetString(LCTYPE type) const;
private:
	struct rg
	{
		LANGID id;
		const char *lang;
	};
	static const struct rg rg[];
};

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

/*
 * @brief Produce a canonical filename from given LANGID.
 * (not currently used)
std::string LangFileInfo::FileName(LANGID id)
{
	std::string filename;
	for (size_t i = 0 ; filename.empty() && i < countof(rg) ; ++i)
	{
		LANGID baseid = rg[i].id;
		if (PRIMARYLANGID(id) == PRIMARYLANGID(baseid))
		{
			const char *sub = rg[i].lang;
			filename = sub;
			_strlwr(&*filename.begin() + 1);
			if (id != baseid)
			{
				while ((id & ~0x3ff) && *(sub += strlen(sub) + 1))
				{
					do
					{
						id -= MAKELANGID(0, 1);
					} while ((id & ~0x3ff) && id == baseid);
				}
				if (*sub)
				{
					size_t i = filename.length();
					filename += sub;
					_strlwr(&*filename.begin() + i + 1);
				}
			}
		}
	}
	return filename;
}
*/

LangFileInfo::LangFileInfo(LPCTSTR path)
: id(0)
{
	if (FILE *f = _tfopen(path, _T("r")))
	{
		char buf[1024 + 1];
		while (fgets(buf, sizeof buf - 1, f))
		{
			int i = 0;
			strcat(buf, "1");
			sscanf(buf, "msgid \" LANG_ENGLISH , SUBLANG_ENGLISH_US \" %d", &i);
			if (i)
			{
				if (fgets(buf, sizeof buf, f))
				{
					char *lang = strstr(buf, "LANG_");
					char *sublang = strstr(buf, "SUBLANG_");
					if (lang && sublang)
					{
						strtok(lang, ",\" \t\r\n");
						strtok(sublang, ",\" \t\r\n");
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

CLanguageSelect::CLanguageSelect(UINT idMainMenu, UINT idDocMenu, BOOL bReloadMenu /*=TRUE*/, BOOL bUpdateTitle /*=TRUE*/, CWnd* pParent /*=NULL*/)
: CDialog(CLanguageSelect::IDD, pParent)
, m_hCurrentDll(0)
, m_wCurLanguage(wSourceLangId)
, m_idMainMenu(idMainMenu)
, m_idDocMenu(idDocMenu)
, m_hModule(NULL)
, m_bReloadMenu(bReloadMenu)
, m_bUpdateTitle(bUpdateTitle)
{
}

void CLanguageSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLanguageSelect)
	DDX_Control(pDX, IDC_LANGUAGE_LIST, m_ctlLangList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLanguageSelect, CDialog)
//{{AFX_MSG_MAP(CLanguageSelect)
	ON_LBN_DBLCLK(IDC_LANGUAGE_LIST, OnDblclkLanguageList)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

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

/**
 * @brief Convert C style \nnn, \r, \n, \t etc into their indicated characters.
 * @param [in, out] s String to convert.
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
BOOL CLanguageSelect::LoadLanguageFile(LANGID wLangId)
{
	String strPath = GetFileName(wLangId);
	if (strPath.empty())
		return FALSE;

	m_hCurrentDll = LoadLibrary(_T("MergeLang.dll"));
	// There is no point in translating error messages about inoperational
	// translation system, so go without string resources here.
	if (m_hCurrentDll == 0)
	{
		if (m_hWnd)
			AfxMessageBox(_T("Failed to load MergeLang.dll"), MB_ICONSTOP);
		return FALSE;
	}
	CVersionInfo viInstance = AfxGetInstanceHandle();
	DWORD instanceVerMS = 0;
	DWORD instanceVerLS = 0;
	viInstance.GetFixedFileVersion(instanceVerMS, instanceVerLS);
	CVersionInfo viResource = m_hCurrentDll;
	DWORD resourceVerMS = 0;
	DWORD resourceVerLS = 0;
	viResource.GetFixedFileVersion(resourceVerMS, resourceVerLS);
	if (instanceVerMS != resourceVerMS || instanceVerLS != resourceVerLS)
	{
		FreeLibrary(m_hCurrentDll);
		m_hCurrentDll = 0;
		if (m_hWnd)
			AfxMessageBox(_T("MergeLang.dll version mismatch"), MB_ICONSTOP);
		return FALSE;
	}
	HRSRC mergepot = FindResource(m_hCurrentDll, _T("MERGEPOT"), RT_RCDATA);
	if (mergepot == 0)
	{
		if (m_hWnd)
			AfxMessageBox(_T("MergeLang.dll is invalid"), MB_ICONSTOP);
		return FALSE;
	}
	size_t size = SizeofResource(m_hCurrentDll, mergepot);
	const char *data = (const char *)LoadResource(m_hCurrentDll, mergepot);
	char buf[1024];
	std::string *ps = 0;
	std::string msgid;
	std::vector<unsigned> lines;
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
				int line = strtol(q + 1, &q, 10);
				lines.push_back(line);
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
				if (lines.size() > 0)
				{
					for (unsigned *pline = &*lines.begin() ; pline <= &*(lines.end() - 1) ; ++pline)
					{
						unsigned line = *pline;
						if (m_strarray.size() <= line)
							m_strarray.resize(line + 1);
						m_strarray[line] = msgid;
					}
				}
				lines.clear();
				msgid.erase();
			}
		}
	}
	FILE *f = _tfopen(strPath.c_str(), _T("r"));
	if (f == 0)
	{
		FreeLibrary(m_hCurrentDll);
		m_hCurrentDll = 0;
		if (m_hWnd)
		{
			std_tchar(ostringstream) stm;
			stm << _T("Failed to load ") << strPath.c_str();
			AfxMessageBox(stm.str().c_str(), MB_ICONSTOP);
		}
		return FALSE;
	}
	ps = 0;
	msgid.erase();
	lines.clear();
	std::string format;
	std::string msgstr;
	std::string directive;
	while (fgets(buf, sizeof buf, f))
	{
		if (char *p = EatPrefix(buf, "#:"))
		{
			if (char *q = strchr(p, ':'))
			{
				int line = strtol(q + 1, &q, 10);
				lines.push_back(line);
				--unresolved;
			}
		}
		else if (char *p = EatPrefix(buf, "#,"))
		{
			format = p;
			format.erase(0, format.find_first_not_of(" \t\r\n"));
			format.erase(format.find_last_not_of(" \t\r\n") + 1);
		}
		else if (char *p = EatPrefix(buf, "#."))
		{
			directive = p;
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
				if (msgstr.empty())
					msgstr = msgid;
				unslash(m_codepage, msgstr);
				// avoid dereference of empty vector or last vector
				if (lines.size()>0)
				{
					for (unsigned *pline = &*lines.begin() ; pline <= &*(lines.end() - 1) ; ++pline)
					{
						unsigned line = *pline;
						if (m_strarray.size() <= line)
							m_strarray.resize(line + 1);
						if (m_strarray[line] == msgid)
							m_strarray[line] = msgstr;
						else
							++mismatched;
					}
				}
				lines.clear();
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
		m_strarray.clear();
		m_codepage = 0;
		if (m_hWnd)
		{
			std_tchar(ostringstream) stm;
			stm << _T("Unresolved or mismatched references detected when ")
				_T("attempting to read translations from\n") << strPath.c_str();
			AfxMessageBox(stm.str().c_str(), MB_ICONSTOP);
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
BOOL CLanguageSelect::SetLanguage(LANGID wLangId)
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
	m_strarray.clear();
	m_codepage = 0;
	if (wLangId != wSourceLangId)
	{
		if (LoadLanguageFile(wLangId))
			AfxSetResourceHandle(m_hCurrentDll);
		else
			wLangId = wSourceLangId;
	}
	m_wCurLanguage = wLangId;
	SetThreadLocale(MAKELCID(m_wCurLanguage, SORT_DEFAULT));
	return TRUE;
}

/**
 * @brief Convert specified Language ID into resource filename, if we have one for it
 */
String CLanguageSelect::GetFileName(LANGID wLangId)
{
	String filename;
	String path = GetModulePath().append(szRelativePath);
	String pattern = path + _T("*.po");
	WIN32_FIND_DATA ff;
	HANDLE h = INVALID_HANDLE_VALUE;
	while ((h = FindFile(h, pattern.c_str(), &ff)) != INVALID_HANDLE_VALUE)
	{
		filename = path + ff.cFileName;
		LangFileInfo lfi = filename.c_str();
		if (lfi.id == wLangId)
			ff.dwFileAttributes = INVALID_FILE_ATTRIBUTES; // terminate loop
		else
			filename.erase();
	}
	return filename;
}

/**
 * @brief Check if there are language files installed.
 *
 * This function does as fast as possible check for installed language
 * files. It needs to be fast since it is used in enabling/disabling
 * GUI item(s). So the simple check we do is just find one .lang file.
 * If there is a .lang file we assume we have at least one language
 * installed.
 * @return TRUE if at least one lang file is found. FALSE if no lang
 * files are found.
 */
BOOL CLanguageSelect::AreLangsInstalled() const
{
	BOOL bFound = FALSE;
	String path = GetModulePath().append(szRelativePath);
	String pattern = path + _T("*.po");
	WIN32_FIND_DATA ff;
	HANDLE h = INVALID_HANDLE_VALUE;
	while ((h = FindFile(h, pattern.c_str(), &ff)) != INVALID_HANDLE_VALUE)
	{
		ff.dwFileAttributes = INVALID_FILE_ATTRIBUTES;
		bFound = TRUE;
	}
	return bFound;
}

/////////////////////////////////////////////////////////////////////////////
// CLanguageSelect commands

bool CLanguageSelect::TranslateString(size_t line, std::string &s) const
{
	if (line > 0 && line < m_strarray.size())
	{
		s = m_strarray[line];
		unsigned codepage = GetACP();
		if (m_codepage != codepage)
		{
			// Attempt to convert to UI codepage
			if (int len = s.length())
			{
				std::wstring ws;
				ws.resize(len);
				len = MultiByteToWideChar(m_codepage, 0, s.c_str(), -1, &*ws.begin(), len + 1);
				if (len)
				{
					ws.resize(len - 1);
					len = WideCharToMultiByte(codepage, 0, ws.c_str(), -1, 0, 0, 0, 0);
					if (len)
					{
						s.resize(len - 1);
						WideCharToMultiByte(codepage, 0, ws.c_str(), -1, &*s.begin(), len, 0, 0);
					}
				}
			}
		}
		return true;
	}
	return false;
}

bool CLanguageSelect::TranslateString(size_t line, std::wstring &ws) const
{
	if (line > 0 && line < m_strarray.size())
	{
		if (int len = m_strarray[line].length())
		{
			ws.resize(len);
			const char *msgstr = m_strarray[line].c_str();
			len = MultiByteToWideChar(m_codepage, 0, msgstr, -1, &*ws.begin(), len + 1);
			ws.resize(len - 1);
			return true;
		}
	}
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
			int cx = dc.GetTextExtent(text.c_str(), text.length()).cx;
			sb.SetPaneInfo(i, id, style | SBPS_DISABLED, cx);
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
		UINT id = 0;
		MENUITEMINFO mii;
		mii.cbSize = sizeof mii;
		mii.fMask = MIIM_STATE|MIIM_ID|MIIM_SUBMENU|MIIM_DATA;
		::GetMenuItemInfo(h, i, TRUE, &mii);
		if (mii.hSubMenu)
		{
			TranslateMenu(mii.hSubMenu);
			mii.wID = reinterpret_cast<UINT>(mii.hSubMenu);
		}
		if (BCMenuData *pItemData = reinterpret_cast<BCMenuData *>(mii.dwItemData))
		{
			if (LPCWSTR text = pItemData->GetWideString())
			{
				unsigned line = 0;
				swscanf(text, L"Merge.rc:%u", &line);
				std::wstring s;
				if (TranslateString(line, s))
					pItemData->SetWideString(s.c_str());
			}
		}
		TCHAR text[80];
		if (::GetMenuString(h, i, text, countof(text), MF_BYPOSITION))
		{
			unsigned line = 0;
			_stscanf(text, _T("Merge.rc:%u"), &line);
			String s;
			if (TranslateString(line, s))
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
		unsigned line = 0;
		_stscanf(text, _T("Merge.rc:%u"), &line);
		String s;
		if (TranslateString(line, s))
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
		unsigned line = 0;
		_stscanf(text, _T("Merge.rc:%u"), &line);
		if (!TranslateString(line, s))
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
				unsigned line = 0;
				swscanf(text, L"Merge.rc:%u", &line);
				if (!TranslateString(line, s))
					s = text;
			}
		}
	}
	return s;
}

void CLanguageSelect::ReloadMenu() 
{
	if (m_idDocMenu)
	{
		// set the menu of the main frame window
		UINT idMenu = GetDocResId();
		CMergeApp *pApp = dynamic_cast<CMergeApp *> (AfxGetApp());
		CMainFrame * pMainFrame = dynamic_cast<CMainFrame *> ((CFrameWnd*)pApp->m_pMainWnd);
		HMENU hNewDefaultMenu = pMainFrame->NewDefaultMenu(idMenu);
		HMENU hNewMergeMenu = pMainFrame->NewMergeViewMenu();
		HMENU hNewDirMenu = pMainFrame->NewDirViewMenu();
		if (hNewDefaultMenu && hNewMergeMenu && hNewDirMenu)
		{
			CMenu* pOldDefaultMenu = CMenu::FromHandle(pMainFrame->m_hMenuDefault);
			CMenu* hOldMergeMenu = CMenu::FromHandle(pApp->m_pDiffTemplate->m_hMenuShared);
			CMenu* hOldDirMenu = CMenu::FromHandle(pApp->m_pDirTemplate->m_hMenuShared);

			// Note : for Windows98 compatibility, use FromHandle and not Attach/Detach
			CMenu * pNewDefaultMenu = CMenu::FromHandle(hNewDefaultMenu);
			CMenu * pNewMergeMenu = CMenu::FromHandle(hNewMergeMenu);
			CMenu * pNewDirMenu = CMenu::FromHandle(hNewDirMenu);
			
			CWnd *pFrame = CWnd::FromHandle(::GetWindow(pMainFrame->m_hWndMDIClient, GW_CHILD));
			while (pFrame)
			{
				if (pFrame->IsKindOf(RUNTIME_CLASS(CChildFrame)))
					((CChildFrame *)pFrame)->SetSharedMenu(hNewMergeMenu);
				else if (pFrame->IsKindOf(RUNTIME_CLASS(CDirFrame)))
					((CDirFrame *)pFrame)->SetSharedMenu(hNewDirMenu);
				pFrame = pFrame->GetNextWindow();
			}

			CFrameWnd *pActiveFrame = pMainFrame->GetActiveFrame();
			if (pActiveFrame)
			{
				if (pActiveFrame->IsKindOf(RUNTIME_CLASS(CChildFrame)))
					pMainFrame->MDISetMenu(pNewMergeMenu, NULL);
				else if (pActiveFrame->IsKindOf(RUNTIME_CLASS(CDirFrame)))
					pMainFrame->MDISetMenu(pNewDirMenu, NULL);
				else
					pMainFrame->MDISetMenu(pNewDefaultMenu, NULL);
			}
			else
				pMainFrame->MDISetMenu(pNewDefaultMenu, NULL);

			// Don't delete the old menu
			// There is a bug in BCMenu or in Windows98 : the new menu does not
			// appear correctly if we destroy the old one
//			if (pOldDefaultMenu)
//				pOldDefaultMenu->DestroyMenu();
//			if (pOldMergeMenu)
//				pOldMergeMenu->DestroyMenu();
//			if (pOldDirMenu)
//				pOldDirMenu->DestroyMenu();

			// m_hMenuDefault is used to redraw the main menu when we close a child frame
			// if this child frame had a different menu
			pMainFrame->m_hMenuDefault = hNewDefaultMenu;
			pApp->m_pDiffTemplate->m_hMenuShared = hNewMergeMenu;
			pApp->m_pDirTemplate->m_hMenuShared = hNewDirMenu;

			// force redrawing the menu bar
			pMainFrame->DrawMenuBar();  

		}
	}
}


UINT CLanguageSelect::GetDocResId()
{
	if (((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->MDIGetActive())
		return m_idDocMenu;
	
	return m_idMainMenu;
}


void CLanguageSelect::UpdateDocTitle()
{
	CDocManager* pDocManager = AfxGetApp()->m_pDocManager;
	POSITION posTemplate = pDocManager->GetFirstDocTemplatePosition();
	ASSERT(posTemplate != NULL);

	while (posTemplate != NULL)
	{
		CDocTemplate* pTemplate = pDocManager->GetNextDocTemplate(posTemplate);
		
		ASSERT(pTemplate != NULL);
		
		POSITION pos = pTemplate->GetFirstDocPosition();
		CDocument* pDoc;
		
		while ( pos != NULL  )
		{
			pDoc = pTemplate->GetNextDoc(pos);
			pDoc->SetTitle(NULL);
			((CFrameWnd*)AfxGetApp()->m_pMainWnd)->OnUpdateFrameTitle(TRUE);
		}
	}
} 



void CLanguageSelect::OnOK() 
{
	UpdateData();
	int index = m_ctlLangList.GetCurSel();
	if (index<0) return;
	//int i = m_ctlLangList.GetItemData(index);
	WORD lang = (WORD)m_ctlLangList.GetItemData(index); //m_wLangIds[i];
	if (lang != m_wCurLanguage)
	{
		if (SetLanguage(lang))
    		AfxGetApp()->WriteProfileInt(LANGUAGE_SECTION, COUNTRY_ENTRY, (int)lang);

		CMainFrame *pMainFrame = static_cast<CMainFrame *>(AfxGetApp()->m_pMainWnd);
		pMainFrame->UpdateCodepageModule();

		// Update status bar inicator texts
		SetIndicators(pMainFrame->m_wndStatusBar, 0, 0);

		// Update the current menu
		if (m_bReloadMenu)
			ReloadMenu();
		
		// update the title text of the document
		if (m_bUpdateTitle)
			UpdateDocTitle();
	}
	
	EndDialog(IDOK);
}

void CLanguageSelect::OnDblclkLanguageList()
{
	OnOK();
}

BOOL CLanguageSelect::OnInitDialog()
{
	TranslateDialog(m_hWnd);
	CDialog::OnInitDialog();
	
	CMainFrame::SetMainIcon(this);

	// setup handler for resizing this dialog	
	m_constraint.InitializeCurrentSize(this);
	// configure how individual controls adjust when dialog resizes
	m_constraint.ConstrainItem(IDC_LANGUAGE_LIST, 0, 1, 0, 1); // grows right & down
	m_constraint.ConstrainItem(IDCANCEL, .6, 0, 1, 0); // slides down, floats right
	m_constraint.ConstrainItem(IDOK, .3, 0, 1, 0); // slides down, floats right
	m_constraint.SubclassWnd(); // install subclassing
	m_constraint.LoadPosition(_T("ResizeableDialogs"), _T("LanguageSelectDlg"), false); // persist size via registry

	GetMainFrame()->CenterToMainFrame(this);

	LoadAndDisplayLanguages();

	return TRUE;
}

/**
 * @brief Load languages available on disk, and display in list, and select current
 */
void CLanguageSelect::LoadAndDisplayLanguages()
{
	String path = GetModulePath().append(szRelativePath);
	String pattern = path + _T("*.po");
	WIN32_FIND_DATA ff;
	HANDLE h = INVALID_HANDLE_VALUE;
	do
	{
		LangFileInfo &lfi =
			h == INVALID_HANDLE_VALUE
		?	LangFileInfo(wSourceLangId)
		:	LangFileInfo((path + ff.cFileName).c_str());
		std_tchar(ostringstream) stm;
		stm << lfi.GetString(LOCALE_SLANGUAGE).c_str();
		stm << _T(" - ");
		stm << lfi.GetString(LOCALE_SNATIVELANGNAME|LOCALE_USE_CP_ACP).c_str();
		stm << _T(" (");
		stm << lfi.GetString(LOCALE_SNATIVECTRYNAME|LOCALE_USE_CP_ACP).c_str();
		stm << _T(")");
		/*stm << _T(" - ");
		stm << lfi.GetString(LOCALE_SABBREVLANGNAME|LOCALE_USE_CP_ACP).c_str();
		stm << _T(" (");
		stm << lfi.GetString(LOCALE_SABBREVCTRYNAME|LOCALE_USE_CP_ACP).c_str();
		stm << _T(") ");*/
		stm << _T(" - ");
		stm << lfi.GetString(LOCALE_SENGLANGUAGE).c_str();
		stm << _T(" (");
		stm << lfi.GetString(LOCALE_SENGCOUNTRY).c_str();
		stm << _T(")");
		int i = m_ctlLangList.AddString(stm.str().c_str());
		m_ctlLangList.SetItemData(i, lfi.id);
		if (lfi.id == m_wCurLanguage)
			m_ctlLangList.SetCurSel(i);
	} while ((h = FindFile(h, pattern.c_str(), &ff)) != INVALID_HANDLE_VALUE);
}

/**
 * @brief Find DLL entry in lang_map for language for specified locale
 */
static WORD GetLangFromLocale(LCID lcid)
{
	TCHAR buff[8] = {0};
	WORD langID = 0;
	if (GetLocaleInfo(lcid, LOCALE_IDEFAULTLANGUAGE, buff, countof(buff)))
		_stscanf(buff, _T("%4hx"), &langID);
	return langID;
}

void CLanguageSelect::InitializeLanguage()
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

	WORD langID = (WORD)AfxGetApp()->GetProfileInt(LANGUAGE_SECTION, COUNTRY_ENTRY, 0);
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
