/**
 * @brief Implements the Language Selection dialog class (which contains the language data)
 */


#include "stdafx.h"
#include "resource.h"
#include "LanguageSelect.h"
#include "MainFrm.h"
#include <locale.h>

// Escaped character constants in range 0x80-0xFF are interpreted in current codepage
// Using C locale gets us direct mapping to Unicode codepoints
#pragma setlocale("C")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLanguageSelect dialog

const WORD wSourceLangId = MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US);

struct tLangDef
{
	UINT m_IdName; // id of name in current UI language
	LPCWSTR m_NativeName; // native language name
	LPCSTR m_AsciiName; // ASCII version of native name
	WORD  m_LangId; // Windows language identifier (LANGID)
	LPCTSTR pszLocale; // locale name to use
};

// Entry for languages for which we do not record a native name
// (either because we don't have one, or because the native name is the same as the ASCII
// name, which is in the string table resource -- eg, Italiano)
#define NONATIVE L""

// References:
// http://people.w3.org/rishida/names/languages.html
// http://www.vaelen.org/cgi-bin/vaelen/vaelen.cgi?topic=languagemenu-languagepacks

const tLangDef lang_map[] =
{
//	{IDS_AFRIKAANS, NONATIVE, MAKELANGID(LANG_AFRIKAANS, SUBLANG_DEFAULT), _T("")},
	{IDS_ALBANIAN, L"Shqip", "Shqip", MAKELANGID(LANG_ALBANIAN, SUBLANG_DEFAULT), _T("")},
//	{IDS_ARABIC_SAUDI, NONATIVE, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_SAUDI_ARABIA), _T("")},  
//	{IDS_ARABIC_IRAQ, NONATIVE, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_IRAQ), _T("")},  
	{IDS_ARABIC_EGYPT, L"\x0627\x0644\x0639\x0631\x0628\x064A\x0629", "Al Arabiya", MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_EGYPT), _T("")},  
//	{IDS_ARABIC_LIBYA, NONATIVE, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_LIBYA), _T("")},  
//	{IDS_ARABIC_ALGERIA, NONATIVE, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_ALGERIA), _T("")},  
//	{IDS_ARABIC_MOROCCO, NONATIVE, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_MOROCCO), _T("")},  
//	{IDS_ARABIC_TUNISIA, NONATIVE, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_TUNISIA), _T("")},  
//	{IDS_ARABIC_OMAN, NONATIVE, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_OMAN), _T("")},  
//	{IDS_ARABIC_YEMEN, NONATIVE, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_YEMEN), _T("")},  
//	{IDS_ARABIC_SYRIA, NONATIVE, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_SYRIA), _T("")},  
//	{IDS_ARABIC_JORDAN, NONATIVE, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_JORDAN), _T("")},  
//	{IDS_ARABIC_LEBANON, NONATIVE, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_LEBANON), _T("")},  
//	{IDS_ARABIC_KUWAIT, NONATIVE, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_KUWAIT), _T("")},  
//	{IDS_ARABIC_UAE, NONATIVE, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_UAE), _T("")},  
//	{IDS_ARABIC_BAHRAIN, NONATIVE, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_BAHRAIN), _T("")},  
//	{IDS_ARABIC_QATAR, NONATIVE, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_QATAR), _T("")},  
	{IDS_ARMENIAN, L"\x540\x561\x575\x565\x580\x567\x576", "Hayeren", MAKELANGID(LANG_ARMENIAN, SUBLANG_DEFAULT), _T("")},
//	{IDS_AZERI_LATIN, NONATIVE, MAKELANGID(LANG_AZERI, SUBLANG_AZERI_LATIN), _T("")},
//	{IDS_AZERI_CYRILLIC, NONATIVE, MAKELANGID(LANG_AZERI, SUBLANG_AZERI_CYRILLIC), _T("")},
	{IDS_BASQUE, L"Euskara", "Euskara", MAKELANGID(LANG_BASQUE, SUBLANG_DEFAULT), _T("")},
	{IDS_BELARUSIAN, L"\x0411\x0435\x043B\x0430\x0440\x0443\x0441\x043A\x0430\x044F", "Belaruski", MAKELANGID(LANG_BELARUSIAN, SUBLANG_DEFAULT), _T("")},
	{IDS_BULGARIAN, L"\x0411\x044A\x043B\x0433\x0430\x0440\x0441\x043A\x0438", "Bulgarian*", MAKELANGID(LANG_BULGARIAN, SUBLANG_DEFAULT), _T("")},
	{IDS_CATALAN, L"Catal\xE0", "Catala", MAKELANGID(LANG_CATALAN, SUBLANG_DEFAULT), _T("")},
	{IDS_CHINESE_TRADITIONAL, L"\x4E2d\x6587 (\x7E41\x9AD4)", "Zhongwen*", MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL), _T("cht")},
	{IDS_CHINESE_SIMPLIFIED, L"\x4E2D\x6587 (\x7C21\x55AE)", "Zhongwen*", MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), _T("chs")},
//	{IDS_CHINESE_HONGKONG, NONATIVE, MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_HONGKONG), _T("chinese_hkg")},
//	{IDS_CHINESE_SINGAPORE, NONATIVE, MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SINGAPORE), _T("chinese_sgp")},
//	{IDS_CHINESE_MACAU, NONATIVE, MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_MACAU), _T("")},
	{IDS_CROATIAN, L"Hrvatski", "Hrvatski", MAKELANGID(LANG_CROATIAN, SUBLANG_DEFAULT), _T("")},
	{IDS_CZECH, L"\x010C" L"esk\xFD", "Cesko", MAKELANGID(LANG_CZECH, SUBLANG_DEFAULT), _T("czech")},
	{IDS_DANISH, L"Dansk", "Dansk", MAKELANGID(LANG_DANISH, SUBLANG_DEFAULT), _T("danish")},
	{IDS_DUTCH, L"Nederlands*", "Nederlands*", MAKELANGID(LANG_DUTCH, SUBLANG_DUTCH), _T("dutch")},
//	{IDS_DUTCH_BELGIAN, NONATIVE, MAKELANGID(LANG_DUTCH, SUBLANG_DUTCH_BELGIAN), _T("")},
	{IDS_ENGLISH_US, L"English", "English", MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), _T("american")},
//	{IDS_ENGLISH_UK, NONATIVE, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK), _T("english-uk")},
//	{IDS_ENGLISH_AUS, NONATIVE, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_AUS), _T("australian")},  
//	{IDS_ENGLISH_CAN, NONATIVE, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_CAN), _T("canadian")},  
//	{IDS_ENGLISH_NZ, NONATIVE, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_NZ), _T("english-nz")},  
//	{IDS_ENGLISH_EIRE, NONATIVE, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_EIRE), _T("english_irl")},  
//	{IDS_ENGLISH_SOUTH_AFRICA, NONATIVE, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_SOUTH_AFRICA), _T("english")},  
//	{IDS_ENGLISH_JAMAICA, NONATIVE, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_JAMAICA), _T("english")},  
//	{IDS_ENGLISH_CARIBBEAN, NONATIVE, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_CARIBBEAN), _T("english")},  
//	{IDS_ENGLISH_BELIZE, NONATIVE, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_BELIZE), _T("english")},  
//	{IDS_ENGLISH_TRINIDAD, NONATIVE, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_TRINIDAD), _T("english")},  
//	{IDS_ENGLISH_ZIMBABWE, NONATIVE, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_ZIMBABWE), _T("english")},
//	{IDS_ENGLISH_PHILIPPINES, NONATIVE, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_PHILIPPINES), _T("english")},
	{IDS_ESTONIAN, L"Eesti", "Eesti", MAKELANGID(LANG_ESTONIAN, SUBLANG_DEFAULT), _T("")},
	{IDS_FAEROESE, L"F\xF8" L"royskt", "Føroyskt", MAKELANGID(LANG_FAEROESE, SUBLANG_DEFAULT), _T("")},
	{IDS_FARSI, L"\x0641\x0627\x0631\x0633", "Farsi", MAKELANGID(LANG_FARSI, SUBLANG_DEFAULT), _T("")},
	{IDS_FINNISH, L"Suomi", "Suomi", MAKELANGID(LANG_FINNISH, SUBLANG_DEFAULT), _T("fin")},
	{IDS_FRENCH, L"Fran\xE7" L"ais", "Francais", MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH), _T("fra")},  
//	{IDS_FRENCH_BELGIAN, L"Fran\xE7" L"ais (Belgique)", MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_BELGIAN), _T("frb")},   
//	{IDS_FRENCH_CANADIAN, L"Fran\xE7" L"ais (Canada)", MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_CANADIAN), _T("frc")},   
//	{IDS_FRENCH_SWISS, L"Fran\xE7" L"ais (Suisse)", MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_SWISS), _T("frs")},   
//	{IDS_FRENCH_LUXEMBOURG, L"Fran\xE7" L"ais (Luxembourg)", MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_LUXEMBOURG), _T("french")},   
//	{IDS_FRENCH_MONACO, L"Fran\xE7" L"ais (Monaco)", MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_MONACO), _T("")},
	{IDS_GEORGIAN, L"\x10E5\x10D0\x10E0\x10D7\x10E3\x10DA\x10D8", "Kartuli", MAKELANGID(LANG_GEORGIAN, SUBLANG_DEFAULT), _T("")},
	{IDS_GERMAN, L"Deutsch", "Deutsch", MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), _T("deu")}, 
//	{IDS_GERMAN_SWISS, NONATIVE, MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_SWISS), _T("des")},  
//	{IDS_GERMAN_AUSTRIAN, NONATIVE, MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_AUSTRIAN), _T("dea")},  
//	{IDS_GERMAN_LUXEMBOURG, NONATIVE, MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_LUXEMBOURG), _T("deu")},  
//	{IDS_GERMAN_LIECHTENSTEIN, NONATIVE, MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_LIECHTENSTEIN), _T("deu")},  
	{IDS_GREEK, L"\x0395\x03BB\x03BB\x03B7\x03BD\x03B9\x03BA\x03AC", "Ellenika", MAKELANGID(LANG_GREEK, SUBLANG_DEFAULT), _T("greek")},
//	{IDS_HEBREW, L"\x05E2\x05D1\x05E8\x05D9\x05EA", MAKELANGID(LANG_HEBREW, SUBLANG_DEFAULT), _T("")},
//	{IDS_HINDI, NONATIVE, MAKELANGID(LANG_HINDI, SUBLANG_DEFAULT), _T("")},
//	{IDS_HUNGARIAN, NONATIVE, MAKELANGID(LANG_HUNGARIAN, SUBLANG_DEFAULT), _T("hun")},
//	{IDS_ICELANDIC, L"\xCDslenska", MAKELANGID(LANG_ICELANDIC, SUBLANG_DEFAULT), _T("isl")},
//	{IDS_INDONESIAN, NONATIVE, MAKELANGID(LANG_INDONESIAN, SUBLANG_DEFAULT), _T("")},
	{IDS_ITALIAN, L"Italiano", "Italiano", MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN), _T("ita")},
//	{IDS_ITALIAN_SWISS, NONATIVE, MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN_SWISS), _T("its")},
	// Kanji (ilbonhua in Hanja) (erbunhua in Mandarin)
	{IDS_JAPANESE, L"\x65E5\x672C\x8A9E", "Nihongo", MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT), _T("jpn")},
//	{IDS_KASHMIRI, NONATIVE, MAKELANGID(LANG_KASHMIRI, SUBLANG_KASHMIRI_INDIA), _T("")},
//	{IDS_KAZAK, L"\x049A\x0430\x0437\x0430\x049B", MAKELANGID(LANG_KAZAK, SUBLANG_DEFAULT), _T("")},
	// hangukhua in Hanja (should get this in Hangul ?) ? or "\xD55C\xAE00" ?
	// In Hangul, it is \xD55C\xaD6D\xC5B4 (HanGukO)
	{IDS_KOREAN, L"\x97D3\x56FD\x8A9E", "Hangul*", MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN), _T("kor")},
//	{IDS_LATVIAN, L"Latvie\x0161u", MAKELANGID(LANG_LATVIAN, SUBLANG_DEFAULT), _T("")},
//	{IDS_LITHUANIAN, L"Lietuvi\x0161kai", MAKELANGID(LANG_LITHUANIAN, SUBLANG_DEFAULT), _T("")},
//	{IDS_MALAY_MALAYSIA, NONATIVE, MAKELANGID(LANG_MALAYALAM, SUBLANG_MALAY_MALAYSIA), _T("")},
//	{IDS_MALAY_BRUNEI_DARUSSALAM, NONATIVE, MAKELANGID(LANG_MALAYALAM, SUBLANG_MALAY_BRUNEI_DARUSSALAM), _T("")},
//	{IDS_MANIPURI, NONATIVE, MAKELANGID(LANG_MANIPURI, SUBLANG_DEFAULT), _T("")},
	{IDS_NORWEGIAN_BOKMAL, L"Norsk (Bokm\xE5l)", "Norsk (Bokmo)*", MAKELANGID(LANG_NORWEGIAN, SUBLANG_NORWEGIAN_BOKMAL), _T("nor")},
//	{IDS_NORWEGIAN_NYNORSK, NONATIVE, MAKELANGID(LANG_NORWEGIAN, SUBLANG_NORWEGIAN_NYNORSK), _T("non")},
	{IDS_POLISH, L"Polski", "Polski", MAKELANGID(LANG_POLISH, SUBLANG_DEFAULT), _T("plk")},
//	{IDS_PORTUGUESE, L"Portugu\xEAs", MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE), _T("ptg")},
	{IDS_PORTUGUESE_BRAZILIAN, L"Portugu\x00EAs brasileiro", "Portugues brasileiro*", MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN), _T("ptb")},
//	{IDS_ROMANIAN, L"Rom\xE2n\x0103", MAKELANGID(LANG_ROMANIAN, SUBLANG_DEFAULT), _T("")},
	{IDS_RUSSIAN, L"\x0440\x0443\0x0441\x0441\x043A\x0438\x0439", "Ruskiyi*", MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), _T("rus")},
//	{IDS_SANSKRIT, NONATIVE, MAKELANGID(LANG_SANSKRIT, SUBLANG_DEFAULT), _T("")},
	{IDS_SERBIAN_LATIN, L"Srpski", "Srpski", MAKELANGID(LANG_SERBIAN, SUBLANG_SERBIAN_LATIN), _T("")},
	{IDS_SERBIAN_CYRILLIC, L"\x0421\x0440\x043F\x0441\x043A\x0438", "srpski", MAKELANGID(LANG_SERBIAN, SUBLANG_SERBIAN_CYRILLIC), _T("")},
//	{IDS_SINDHI, NONATIVE, MAKELANGID(LANG_SINDHI, SUBLANG_DEFAULT), _T("")},
	{IDS_SLOVAK, L"Sloven\x010Dina", "Slovencina*", MAKELANGID(LANG_SLOVAK, SUBLANG_DEFAULT), _T("sky")},
	{IDS_SLOVENIAN, L"Sloven\x0161\x010Dina", "Slovenscina*", MAKELANGID(LANG_SLOVENIAN, SUBLANG_DEFAULT), _T("")},
//	{IDS_SPANISH, L"Espa\xF1ol", MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH), _T("esm")}, 
//	{IDS_SPANISH_MEXICAN, NONATIVE, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_MEXICAN), _T("esp")}, 
	{IDS_SPANISH_MODERN, L"Espa\xF1ol (moderno)", "Espanol (moderno)", MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_MODERN), _T("esn")}, 
//	{IDS_SPANISH_GUATEMALA, NONATIVE, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_GUATEMALA), _T("esp")}, 
//	{IDS_SPANISH_COSTA_RICA, NONATIVE, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_COSTA_RICA), _T("esp")}, 
//	{IDS_SPANISH_PANAMA, NONATIVE, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_PANAMA), _T("esp")}, 
//	{IDS_SPANISH_DOMINICAN, NONATIVE, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_DOMINICAN_REPUBLIC), _T("esp")}, 
//	{IDS_SPANISH_VENEZUELA, NONATIVE, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_VENEZUELA), _T("esp")}, 
//	{IDS_SPANISH_COLOMBIA, NONATIVE, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_COLOMBIA), _T("esp")}, 
//	{IDS_SPANISH_PERU, NONATIVE, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_PERU), _T("esp")}, 
//	{IDS_SPANISH_ARGENTINA, NONATIVE, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_ARGENTINA), _T("esp")}, 
//	{IDS_SPANISH_ECUADOR, NONATIVE, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_ECUADOR), _T("esp")}, 
//	{IDS_SPANISH_CHILE, NONATIVE, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_CHILE), _T("esp")}, 
//	{IDS_SPANISH_URUGUAY, NONATIVE, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_URUGUAY), _T("esp")}, 
//	{IDS_SPANISH_PARAGUAY, NONATIVE, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_PARAGUAY), _T("esp")}, 
//	{IDS_SPANISH_BOLIVIA, NONATIVE, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_BOLIVIA), _T("esp")}, 
//	{IDS_SPANISH_EL_SALVADOR, NONATIVE, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_EL_SALVADOR), _T("esp")}, 
//	{IDS_SPANISH_HONDURAS, NONATIVE, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_HONDURAS), _T("esp")}, 
//	{IDS_SPANISH_NICARAGUA, NONATIVE, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_NICARAGUA), _T("esp")}, 
//	{IDS_SPANISH_PUERTO_RICO, NONATIVE, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_PUERTO_RICO), _T("esp")}, 
//	{IDS_SWAHILI, NONATIVE, MAKELANGID(LANG_SWAHILI, SUBLANG_DEFAULT), _T("")},
	{IDS_SWEDISH, L"Svenska*", "Svenska*", MAKELANGID(LANG_SWEDISH, SUBLANG_SWEDISH), _T("sve")},
//	{IDS_SWEDISH_FINLAND, NONATIVE, MAKELANGID(LANG_SWEDISH, SUBLANG_SWEDISH_FINLAND), _T("sve")},
//	{IDS_TAMIL, NONATIVE, MAKELANGID(LANG_TAMIL, SUBLANG_DEFAULT), _T("")},
//	{IDS_TATAR, NONATIVE, MAKELANGID(LANG_TATAR, SUBLANG_DEFAULT), _T("")},
//	{IDS_THAI, L"\x0E20\x0E32\x0E29\x0E32\x0E44\x0E17\x0E22", MAKELANGID(LANG_THAI, SUBLANG_DEFAULT), _T("")},
	{IDS_TURKISH, L"T\x03CBrk\xE7" L"e", "Turkce", MAKELANGID(LANG_TURKISH, SUBLANG_DEFAULT), _T("trk")},
//	{IDS_UKRANIAN, L"\x0423\x043A\x0440\x0430\x0457\x043D\x0441\x044C\x043A\x0430", MAKELANGID(LANG_UKRAINIAN, SUBLANG_DEFAULT), _T("")},
	{IDS_URDU_PAKISTAN, L"\x0627\x0631\x062F\x0648", "Urdu (Pakistan)", MAKELANGID(LANG_URDU, SUBLANG_URDU_PAKISTAN), _T("")},
	{IDS_URDU_INDIA, L"\x0627\x0631\x062F\x0648", "Urdu (India)", MAKELANGID(LANG_URDU, SUBLANG_URDU_INDIA), _T("")},
//	{IDS_UZBEK_LATIN, NONATIVE, MAKELANGID(LANG_UZBEK, SUBLANG_UZBEK_LATIN), _T("")},
//	{IDS_UZBEK_CYRILLIC, L"\x040E\x0437\x0431\x0435\x043A", MAKELANGID(LANG_UZBEK, SUBLANG_UZBEK_CYRILLIC), _T("")},
//	{IDS_VIETNAMESE, L"Ti\xEA\x0301ng Vi\xEA\x0323t", MAKELANGID(LANG_VIETNAMESE, SUBLANG_DEFAULT), _T("")},
	{0, L"0", "0", 0, NULL},
};





CLanguageSelect::CLanguageSelect(UINT idMainMenu, UINT idDocMenu, BOOL bReloadMenu /*=TRUE*/, BOOL bUpdateTitle /*=TRUE*/, CWnd* pParent /*=NULL*/)
: CDialog(CLanguageSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLanguageSelect)
	m_strLanguage = _T("");
	//}}AFX_DATA_INIT
	m_wCurLanguage = wSourceLangId;
	m_idMainMenu = idMainMenu;
	m_idDocMenu = idDocMenu;
	m_hModule = NULL;
	m_bReloadMenu = bReloadMenu;
	m_bUpdateTitle = bUpdateTitle;
}


void CLanguageSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLanguageSelect)
	DDX_Control(pDX, IDC_LANGUAGE_LIST, m_ctlLangList);
	DDX_LBString(pDX, IDC_LANGUAGE_LIST, m_strLanguage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLanguageSelect, CDialog)
//{{AFX_MSG_MAP(CLanguageSelect)
	ON_LBN_DBLCLK(IDC_LANGUAGE_LIST, OnDblclkLanguageList)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLanguageSelect message handlers
BOOL  CLanguageSelect::SetLanguage(WORD wLangId)
{ 
	BOOL result = FALSE;
	
	// use local resources
	if ((PRIMARYLANGID(wLangId)== LANG_ENGLISH)
		&& (SUBLANGID(wLangId) == SUBLANG_ENGLISH_US))
	{  
		LoadResourceDLL();
		result = TRUE;
	}
	// use resources from DLL
	else
	{  
		if ( m_wCurLanguage != wLangId)
		{
			CString strPath = GetDllName(wLangId);
			
			if ( !strPath.IsEmpty()
				&& LoadResourceDLL(strPath) )
			{
				result = TRUE;
			}
		}
	}
	
	
	if (result)
	{
		m_wCurLanguage = wLangId;
		AfxGetApp()->WriteProfileInt( LANGUAGE_SECTION, COUNTRY_ENTRY, (INT) wLangId );
		SetThreadLocale(MAKELCID(m_wCurLanguage, SORT_DEFAULT));

		int idx = GetLanguageArrayIndex(m_wCurLanguage);
		if (idx != -1
			&& *lang_map[idx].pszLocale != _T('\0'))
		{
			_tsetlocale(LC_ALL, lang_map[idx].pszLocale);
		}
	}
	
	return result;
}


BOOL CLanguageSelect::LoadResourceDLL(LPCTSTR szDllFileName /*=NULL*/) 
{
	// reset the resource handle to point to the current file
	AfxSetResourceHandle(AfxGetInstanceHandle( ));
	
	// free the existing DLL
	if ( m_hCurrentDll != NULL )
	{
		FreeLibrary(m_hCurrentDll);
		m_hCurrentDll = NULL;
	}
	
	// bail if using local resources
	if (szDllFileName == NULL
		|| *szDllFileName == _T('\0'))
		return FALSE;
	
	// load the DLL
	if (m_pLog != NULL)
		m_pLog->Write(_T("Loading resource DLL: %s"), szDllFileName);
	if ((m_hCurrentDll = LoadLibrary(szDllFileName)) != NULL)
	{
		AfxSetResourceHandle(m_hCurrentDll);
		return TRUE;
	}
	
	return FALSE;
}


CString CLanguageSelect::GetDllName( WORD wLangId ) 
{
	TCHAR fullpath[MAX_PATH+1];
	
	if ( GetModuleFileName(m_hModule, fullpath, _MAX_PATH ))
	{
		CStringArray dlls;
		WORD wDllLang;
		
		CString strPath = GetPath(fullpath);
		GetDllsAt(strPath, dlls);
		
		for (int i = 0; i < dlls.GetSize(); i++)
		{
			if (GetLanguage( dlls[i], wDllLang))
			{
				if (wLangId == wDllLang)
					return dlls[i];
			}
		}
	}
	
	return CString(_T(""));
}


void CLanguageSelect::GetDllsAt(LPCTSTR szSearchPath, CStringArray& dlls )
{
	WIN32_FIND_DATA ffi;
	CString strFileSpec;
	
	strFileSpec.Format(_T("%s*.lang"), szSearchPath);
	HANDLE hff = FindFirstFile(strFileSpec, &ffi);
	
	if (  hff != INVALID_HANDLE_VALUE )
	{
		do
		{
			if (!(ffi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{

				strFileSpec.Format(_T("%s%s"), szSearchPath, ffi.cFileName);
				if (m_pLog != NULL)
					m_pLog->Write(_T("Found LANG file: %s"), strFileSpec);
				dlls.Add(strFileSpec);  
			}
		}
		while (FindNextFile(hff, &ffi));
		FindClose(hff);
	}
}

static CWordArray foundLangs;

BOOL CALLBACK EnumResLangProc(HANDLE /*hModule*/,	// module handle
							  LPCTSTR /*lpszType*/,  // pointer to resource type
							  LPCTSTR /*lpszName*/,  // pointer to resource name
							  WORD wIDLanguage,  // resource language identifier
							  LPARAM /*lParam*/)		// application-defined parameter)
{
	
	foundLangs.Add(wIDLanguage);
	return TRUE;
}


BOOL CLanguageSelect::GetLanguage( const CString& DllName, WORD& uiLanguage )
{
	DWORD   dwVerInfoSize;		// Size of version information block
	DWORD   dwVerHnd=0;			// An 'ignored' parameter, always '0'
	CString s(DllName);
	LPTSTR pszFilename = s.GetBuffer(MAX_PATH);
	BOOL bResult = FALSE;
	LPTSTR   m_lpstrVffInfo;	
	m_lpstrVffInfo = NULL;

	uiLanguage = wSourceLangId;
	dwVerInfoSize = GetFileVersionInfoSize(pszFilename, &dwVerHnd);
	if (dwVerInfoSize) {
		HANDLE  hMem;
		hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		m_lpstrVffInfo  = (LPTSTR)GlobalLock(hMem);
		if (GetFileVersionInfo(pszFilename, dwVerHnd, dwVerInfoSize, m_lpstrVffInfo))
		{
			LPWORD langInfo;
			DWORD langLen;
			if (VerQueryValue((LPVOID)m_lpstrVffInfo,
				_T("\\VarFileInfo\\Translation"),
				(LPVOID *)&langInfo, (UINT *)&langLen))
			{
				uiLanguage = langInfo[0];
				bResult = TRUE;
			}
		}
		GlobalUnlock(hMem);
		GlobalFree(hMem);
	}
	return bResult;
}

typedef long GetDllLangProc();
/*BOOL CLanguageSelect::GetLanguage( const CString& DllName, WORD& uiLanguage ) 
{
	BOOL bRes = FALSE;
	HINSTANCE hInst = LoadLibrary(DllName);
	
	if ( hInst )
	{										 
		foundLangs.SetSize(0,1);
		if (EnumResourceLanguages(hInst,			 // resource-module handle
			RT_DIALOG,			  // pointer to resource type
			MAKEINTRESOURCE(30000),			  // pointer to resource name
			(ENUMRESLANGPROC)EnumResLangProc,  // pointer to callback function
			0L))				  // application-defined parameter
		{
			if (m_pLog != NULL)
				m_pLog->Write(_T("%d languages found in file %s"), foundLangs.GetSize(), DllName);
			if (foundLangs.GetSize()>0)
			{
				if (m_pLog != NULL)
				{
					for (int i=0; i < foundLangs.GetSize(); i++)
						m_pLog->Write(_T("Found language: %s"), GetLanguageString(foundLangs.GetAt(i)));
				}
				uiLanguage = foundLangs.GetAt(0);
			bRes = TRUE;
		}
		}
		else
			uiLanguage = 0;
		
		FreeLibrary(hInst);
	}
	
	return bRes;
}*/

CString CLanguageSelect::GetPath( LPCTSTR FileName)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_PATH];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	
	_tsplitpath( FileName, drive, dir, fname, ext );
	CString Path = drive;
	Path += dir;

	if (Path.Right(1) != _T('\\')
		&& Path.Right(1) != _T('/'))
		Path += _T('\\');
	
	return Path;
}

void CLanguageSelect::GetAvailLangs( CWordArray& wLanguageAry,
									CStringArray& DllFileNames ) 
{
	CString strPath;
	TCHAR filespec[MAX_PATH+1];
	WORD wLanguage;
	
	if ( GetModuleFileName(m_hModule, filespec, _MAX_PATH ))
	{
		strPath = GetPath(filespec);
		CStringArray dlls;
		
		GetDllsAt(strPath, dlls );
		
		for ( int i = 0; i < dlls.GetSize(); i++ )
		{
			if ( GetLanguage( dlls[i], wLanguage ) )
			{
				wLanguageAry.Add(wLanguage);
				DllFileNames.Add(dlls[i]);
			}
			else if (m_pLog != NULL)
				m_pLog->Write(_T("No languages found in file %s"), dlls[i]);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CLanguageSelect commands


void CLanguageSelect::ReloadMenu() 
{
	if (m_idDocMenu)
	{
		// set the menu of the main frame window
		UINT idMenu = GetDocResId();
		CMainFrame * pMainFrame = dynamic_cast<CMainFrame *> ((CFrameWnd*)AfxGetApp()->m_pMainWnd);
		HMENU hNewMenu = pMainFrame->NewDefaultMenu(idMenu);
		if (hNewMenu)
		{
			CMenu* pOldMenu = pMainFrame->GetMenu();

			// Note : for Windows98 compatibility, use FromHandle and not Attach/Detach
			CMenu * pNewMenu = CMenu::FromHandle(hNewMenu);
			if (pMainFrame->MDISetMenu(pNewMenu, NULL))			 
			{
				// Don't delete the old menu
				// There is a bug in BCMenu or in Windows98 : the new menu does not
				// appear correctly if we destroy the old one
//			if (pOldMenu)
//				pOldMenu->DestroyMenu();

				// m_hMenuDefault is used to redraw the main menu when we close a child frame
				// if this child frame had a different menu
				pMainFrame->m_hMenuDefault = hNewMenu;

				// force redrawing the menu bar
				pMainFrame->DrawMenuBar();  
			}
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
	int i = m_ctlLangList.GetItemData(index);
	int lang = m_wLangIds[i];
	if ( lang != m_wCurLanguage )
	{
		SetLanguage(lang);
		
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
	CDialog::OnInitDialog();
	
	if (m_wLangIds.GetSize()<=0)
	{
		// get all available resource only Dlls
		//
		GetAvailLangs( m_wLangIds, m_DllFileNameAry );
		////
		
		// Add the language of this exe file to list at the
		// language select dialog
		//
		m_wLangIds.Add(wSourceLangId);  // Language Id of this english (US) application
		m_DllFileNameAry.Add("");	   // Dll Name - none
	}
		
// Fill the ComboBox
	CString Language;
	for ( int i = 0; i < m_wLangIds.GetSize(); i++)
	{
		CString Language = GetLanguageString(m_wLangIds[i]);
		if ( !Language.IsEmpty() )
		{
			int idx = m_ctlLangList.AddString(Language);
			m_ctlLangList.SetItemData(idx, i);
		}
	}
// Select the current language (if found)
	for (i=0; i<m_ctlLangList.GetCount(); ++i)
	{
		if (m_wCurLanguage == m_wLangIds[m_ctlLangList.GetItemData(i)])
		{
			m_ctlLangList.SetCurSel(i);
			break;
		}
	}
	
	return TRUE;  
}


int CLanguageSelect::GetLanguageArrayIndex( WORD LangId )
{
	for ( int i = 0; lang_map[i].m_LangId != 0; i++)
		if ( lang_map[i].m_LangId == LangId)
			return i;
	
	return -1;
}
	
CString CLanguageSelect::GetLanguageString( WORD LangId )
	{
	int idx = GetLanguageArrayIndex(LangId);
	if (idx == -1) return _T("");

	// Localized name
	CString Language = LoadResString(lang_map[idx].m_IdName);
	// Append native name
	Language += _T(" - ") + GetNativeLanguageNameString(idx);
	return Language;
	}

CString CLanguageSelect::GetNativeLanguageNameString( int idx )
	{
	CString Language(_T(""));
	// Display the native name (from the array in this file) if it fits into current codepage
	// Otherwise, take the name from the RC file (which will be the name from the English RC
	// file, as none of the other RC files have language name entries, and the names in the
	// English RC file are all ASCII, so they fit into any codepage)
	LPCWSTR name = lang_map[idx].m_NativeName;
	if (name[0])
	{
		int codepage = GetACP();
		int flags = 0;
		char cbuffer[256];

		BOOL defaulted = FALSE;
		int nbytes = WideCharToMultiByte(codepage, flags, name, wcslen(name), cbuffer, sizeof(cbuffer), 0, &defaulted);
		if (nbytes && !defaulted)
		{
			cbuffer[nbytes] = 0;
#ifdef _UNICODE
			Language = name;
#else
			Language = cbuffer;
#endif
		}
	}

	if (Language.IsEmpty())
		Language = lang_map[idx].m_AsciiName;
	
	return Language;
}

void
CLanguageSelect::InitializeLanguage()
{
	int iLangId = AfxGetApp()->GetProfileInt( LANGUAGE_SECTION, COUNTRY_ENTRY, (INT) -1 );
	if ( iLangId != -1 )
		SetLanguage((WORD)iLangId);
}

