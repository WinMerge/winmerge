// LanguageSelect.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "LanguageSelect.h"
#include <locale.h>


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
	UINT  m_Ids;
	WORD  m_LangId;
	LPCTSTR pszLocale;
};

const tLangDef lang_map[] =
{
	{IDS_AFRIKAANS, MAKELANGID(LANG_AFRIKAANS, SUBLANG_DEFAULT), _T("")},
	{IDS_ALBANIAN, MAKELANGID(LANG_ALBANIAN, SUBLANG_DEFAULT), _T("")},
	{IDS_ARABIC_SAUDI, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_SAUDI_ARABIA), _T("")},  
	{IDS_ARABIC_IRAQ, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_IRAQ), _T("")},  
	{IDS_ARABIC_EGYPT, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_EGYPT), _T("")},  
	{IDS_ARABIC_LIBYA, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_LIBYA), _T("")},  
	{IDS_ARABIC_ALGERIA, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_ALGERIA), _T("")},  
	{IDS_ARABIC_MOROCCO, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_MOROCCO), _T("")},  
	{IDS_ARABIC_TUNISIA, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_TUNISIA), _T("")},  
	{IDS_ARABIC_OMAN, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_OMAN), _T("")},  
	{IDS_ARABIC_YEMEN, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_YEMEN), _T("")},  
	{IDS_ARABIC_SYRIA, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_SYRIA), _T("")},  
	{IDS_ARABIC_JORDAN, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_JORDAN), _T("")},  
	{IDS_ARABIC_LEBANON, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_LEBANON), _T("")},  
	{IDS_ARABIC_KUWAIT, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_KUWAIT), _T("")},  
	{IDS_ARABIC_UAE, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_UAE), _T("")},  
	{IDS_ARABIC_BAHRAIN, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_BAHRAIN), _T("")},  
	{IDS_ARABIC_QATAR, MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_QATAR), _T("")},  
	{IDS_BASQUE, MAKELANGID(LANG_BASQUE, SUBLANG_DEFAULT), _T("")},
	{IDS_BELARUSIAN, MAKELANGID(LANG_BELARUSIAN, SUBLANG_DEFAULT), _T("")},
	{IDS_BULGARIAN, MAKELANGID(LANG_BULGARIAN, SUBLANG_DEFAULT), _T("")},
	{IDS_CATALAN, MAKELANGID(LANG_CATALAN, SUBLANG_DEFAULT), _T("")},
	{IDS_CHINESE_TRADITIONAL, MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL), _T("cht")},
	{IDS_CHINESE_SIMPLIFIED, MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), _T("chs")},
	{IDS_CHINESE_HONGKONG, MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_HONGKONG), _T("chinese_hkg")},
	{IDS_CHINESE_SINGAPORE, MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SINGAPORE), _T("chinese_sgp")},
	{IDS_CROATIAN, MAKELANGID(LANG_CROATIAN, SUBLANG_DEFAULT), _T("")},
	{IDS_CZECH, MAKELANGID(LANG_CZECH, SUBLANG_DEFAULT), _T("czech")},
	{IDS_DANISH, MAKELANGID(LANG_DANISH, SUBLANG_DEFAULT), _T("danish")},
	{IDS_DUTCH, MAKELANGID(LANG_DUTCH, SUBLANG_DEFAULT), _T("dutch")},
	{IDS_ENGLISH_US, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), _T("american")},
	{IDS_ENGLISH_UK, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK), _T("english-uk")},
	{IDS_ENGLISH_AUS, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_AUS), _T("australian")},  
	{IDS_ENGLISH_CAN, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_CAN), _T("canadian")},  
	{IDS_ENGLISH_NZ, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_NZ), _T("english-nz")},  
	{IDS_ENGLISH_EIRE, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_EIRE), _T("english_irl")},  
	{IDS_ENGLISH_SOUTH_AFRICA, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_SOUTH_AFRICA), _T("english")},  
	{IDS_ENGLISH_JAMAICA, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_JAMAICA), _T("english")},  
	{IDS_ENGLISH_CARIBBEAN, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_CARIBBEAN), _T("english")},  
	{IDS_ENGLISH_BELIZE, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_BELIZE), _T("english")},  
	{IDS_ENGLISH_TRINIDAD, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_TRINIDAD), _T("english")},  
	{IDS_ESTONIAN, MAKELANGID(LANG_ESTONIAN, SUBLANG_DEFAULT), _T("")},
	{IDS_FAEROESE, MAKELANGID(LANG_FAEROESE, SUBLANG_DEFAULT), _T("")},
	{IDS_FARSI, MAKELANGID(LANG_FARSI, SUBLANG_DEFAULT), _T("")},
	{IDS_FINNISH, MAKELANGID(LANG_FINNISH, SUBLANG_DEFAULT), _T("fin")},
	{IDS_FRENCH, MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH), _T("fra")},  
	{IDS_FRENCH_BELGIAN, MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_BELGIAN), _T("frb")},   
	{IDS_FRENCH_CANADIAN, MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_CANADIAN), _T("frc")},   
	{IDS_FRENCH_SWISS, MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_SWISS), _T("frs")},   
	{IDS_FRENCH_LUXEMBOURG, MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_LUXEMBOURG), _T("french")},   
	{IDS_GERMAN, MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), _T("deu")}, 
	{IDS_GERMAN_SWISS, MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_SWISS), _T("des")},  
	{IDS_GERMAN_AUSTRIAN, MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_AUSTRIAN), _T("dea")},  
	{IDS_GERMAN_LUXEMBOURG, MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_LUXEMBOURG), _T("deu")},  
	{IDS_GERMAN_LIECHTENSTEIN, MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN_LIECHTENSTEIN), _T("deu")},  
	{IDS_GREEK, MAKELANGID(LANG_GREEK, SUBLANG_DEFAULT), _T("greek")},
	{IDS_HEBREW, MAKELANGID(LANG_HEBREW, SUBLANG_DEFAULT), _T("")},
	{IDS_HUNGARIAN, MAKELANGID(LANG_HUNGARIAN, SUBLANG_DEFAULT), _T("hun")},
	{IDS_ICELANDIC, MAKELANGID(LANG_ICELANDIC, SUBLANG_DEFAULT), _T("isl")},
	{IDS_INDONESIAN, MAKELANGID(LANG_INDONESIAN, SUBLANG_DEFAULT), _T("")},
	{IDS_ITALIAN, MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN), _T("ita")},
	{IDS_ITALIAN_SWISS, MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN_SWISS), _T("its")},
	{IDS_JAPANESE, MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT), _T("jpn")},
	{IDS_KOREAN, MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN), _T("kor")},
	{IDS_LATVIAN, MAKELANGID(LANG_LATVIAN, SUBLANG_DEFAULT), _T("")},
	{IDS_LITHUANIAN, MAKELANGID(LANG_LITHUANIAN, SUBLANG_DEFAULT), _T("")},
	{IDS_NORWEGIAN_BOKMAL, MAKELANGID(LANG_NORWEGIAN, SUBLANG_NORWEGIAN_BOKMAL), _T("nor")},
	{IDS_NORWEGIAN_NYNORSK, MAKELANGID(LANG_NORWEGIAN, SUBLANG_NORWEGIAN_NYNORSK), _T("non")},
	{IDS_POLISH, MAKELANGID(LANG_POLISH, SUBLANG_DEFAULT), _T("plk")},
	{IDS_PORTUGUESE, MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE), _T("ptg")},
	{IDS_PORTUGUESE_BRAZILIAN, MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN), _T("ptb")},
	{IDS_ROMANIAN, MAKELANGID(LANG_ROMANIAN, SUBLANG_DEFAULT), _T("")},
	{IDS_RUSSIAN, MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), _T("rus")},
	{IDS_SERBIAN_LATIN, MAKELANGID(LANG_SERBIAN, SUBLANG_SERBIAN_LATIN), _T("")},
	{IDS_SERBIAN_CYRILLIC, MAKELANGID(LANG_SERBIAN, SUBLANG_SERBIAN_CYRILLIC), _T("")},
	{IDS_SLOVAK, MAKELANGID(LANG_SLOVAK, SUBLANG_DEFAULT), _T("sky")},
	{IDS_SLOVENIAN, MAKELANGID(LANG_SLOVENIAN, SUBLANG_DEFAULT), _T("")},
	{IDS_SPANISH, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH), _T("esm")}, 
	{IDS_SPANISH_MEXICAN, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_MEXICAN), _T("esp")}, 
	{IDS_SPANISH_MODERN, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_MODERN), _T("esn")}, 
	{IDS_SPANISH_GUATEMALA, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_GUATEMALA), _T("esp")}, 
	{IDS_SPANISH_COSTA_RICA, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_COSTA_RICA), _T("esp")}, 
	{IDS_SPANISH_PANAMA, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_PANAMA), _T("esp")}, 
	{IDS_SPANISH_DOMINICAN, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_DOMINICAN_REPUBLIC), _T("esp")}, 
	{IDS_SPANISH_VENEZUELA, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_VENEZUELA), _T("esp")}, 
	{IDS_SPANISH_COLOMBIA, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_COLOMBIA), _T("esp")}, 
	{IDS_SPANISH_PERU, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_PERU), _T("esp")}, 
	{IDS_SPANISH_ARGENTINA, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_ARGENTINA), _T("esp")}, 
	{IDS_SPANISH_ECUADOR, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_ECUADOR), _T("esp")}, 
	{IDS_SPANISH_CHILE, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_CHILE), _T("esp")}, 
	{IDS_SPANISH_URUGUAY, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_URUGUAY), _T("esp")}, 
	{IDS_SPANISH_PARAGUAY, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_PARAGUAY), _T("esp")}, 
	{IDS_SPANISH_BOLIVIA, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_BOLIVIA), _T("esp")}, 
	{IDS_SPANISH_EL_SALVADOR, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_EL_SALVADOR), _T("esp")}, 
	{IDS_SPANISH_HONDURAS, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_HONDURAS), _T("esp")}, 
	{IDS_SPANISH_NICARAGUA, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_NICARAGUA), _T("esp")}, 
	{IDS_SPANISH_PUERTO_RICO, MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_PUERTO_RICO), _T("esp")}, 
	{IDS_SWEDISH, MAKELANGID(LANG_SWEDISH, SUBLANG_SWEDISH), _T("sve")},
	{IDS_SWEDISH_FINLAND, MAKELANGID(LANG_SWEDISH, SUBLANG_SWEDISH_FINLAND), _T("sve")},
	{IDS_THAI, MAKELANGID(LANG_THAI, SUBLANG_DEFAULT), _T("")},
	{IDS_TURKISH, MAKELANGID(LANG_TURKISH, SUBLANG_DEFAULT), _T("trk")},
	{IDS_UKRANIAN, MAKELANGID(LANG_UKRAINIAN, SUBLANG_DEFAULT), _T("")},
	{IDS_VIETNAMESE, MAKELANGID(LANG_VIETNAMESE, SUBLANG_DEFAULT), _T("")},
	{0, 0, NULL},
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
	DDX_Control(pDX, 30001, m_ctlLangList);
	DDX_LBString(pDX, 30001, m_strLanguage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLanguageSelect, CDialog)
//{{AFX_MSG_MAP(CLanguageSelect)
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
			setlocale(LC_ALL, lang_map[idx].pszLocale);
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
	
	_splitpath( FileName, drive, dir, fname, ext );
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
	char filespec[MAX_PATH+1];
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
		CMenu menu;
		UINT idMenu = GetDocResId();
		if ( (idMenu > 0) && menu.LoadMenu( idMenu ) )
		{
			CMenu* pOldMenu = AfxGetApp()->m_pMainWnd->GetMenu();
			
			if ( AfxGetApp()->m_pMainWnd->SetMenu( &menu ) )
			{
				pOldMenu->DestroyMenu();
				AfxGetApp()->m_pMainWnd->DrawMenuBar();  
				((CFrameWnd*)AfxGetApp()->m_pMainWnd)->m_hMenuDefault = menu.m_hMenu;
				
			}
			menu.Detach( );
		}
		else
		{
			ASSERT(FALSE);
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
	WORD lang = GetLanguageId(m_strLanguage);
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
		
	int iSel = -1;
	// Fill the ComboBox
	//
	CString Language;
	for ( int i = 0; i < m_wLangIds.GetSize(); i++)
	{
		CString Language = GetLanguageString(m_wLangIds[i]);
		if ( !Language.IsEmpty() )
		{
			m_ctlLangList.AddString(Language);
			
			if ( m_wCurLanguage == m_wLangIds[i])
				iSel = i;
		}
	}
	
	ASSERT(iSel != -1);
	m_ctlLangList.SetCurSel(iSel);
	
	
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
	CString Language(_T(""));
	int idx = GetLanguageArrayIndex(LangId);
	if (idx != -1)
		{
		Language.LoadString(lang_map[idx].m_Ids);
		}
	
	return Language;
}


WORD  CLanguageSelect::GetLanguageId(const CString& Language )
{
	CString LangStr;
	for ( int i = 0; lang_map[i].m_LangId != 0; i++)
	{
		LangStr.LoadString(lang_map[i].m_Ids);
		
		if (LangStr == Language ) 
			return lang_map[i].m_LangId;
	}
	
	return 0;
}


void
CLanguageSelect::InitializeLanguage()
{
	int iLangId = AfxGetApp()->GetProfileInt( LANGUAGE_SECTION, COUNTRY_ENTRY, (INT) -1 );
	if ( iLangId != -1 )
		SetLanguage((WORD)iLangId);
}


