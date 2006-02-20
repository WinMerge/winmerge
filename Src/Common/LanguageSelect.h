// LanguageSelect.h : header file
//
// Dialog to choose language of user interface

#if !defined(AFX_LANGUAGESELECT_H__4395A84F_E8DF_11D1_BBCB_00A024706EDC__INCLUDED_)
#define AFX_LANGUAGESELECT_H__4395A84F_E8DF_11D1_BBCB_00A024706EDC__INCLUDED_

#include "LogFile.h"

#ifndef CMoveConstraint_h
#include "CMoveConstraint.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CLanguageSelect dialog

#define LANGUAGE_SECTION  _T("Locale")
#define COUNTRY_ENTRY     _T("LanguageId")


class CLanguageSelect : public CDialog
{
// Construction
public:
	void SetModuleHandle(HMODULE hModule) { m_hModule = hModule; }
	CLanguageSelect(UINT idMainMenu, UINT idDocMenu, BOOL bReloadMenu =TRUE, BOOL bUpdateTitle =TRUE, CWnd* pParent = NULL);   // standard constructor
	UINT GetAvailLangCount();
	void GetAvailLangs( CWordArray& uiLanguageAry, CStringArray& DllFileNameAry );	   
	WORD GetLangId() { return m_wCurLanguage; };
	void InitializeLanguage();
	CString GetDllName (WORD LangId );
	void	UpdateDocTitle();
	void	ReloadMenu();
	CLogFile *m_pLog;
	void SetLogFile(CLogFile* pLog) { m_pLog = pLog; }
	

// Implementation data
private:
	prdlg::CMoveConstraint m_constraint; 
	CWordArray   m_wLangIds;
	BOOL m_bReloadMenu;
	BOOL m_bUpdateTitle;
	HMODULE m_hModule;
	UINT m_idMainMenu;
	UINT m_idDocMenu;
	HINSTANCE  m_hCurrentDll;
	WORD	   m_wCurLanguage;
	CStringArray m_DllFileNameAry;

// Implementation methods
private:
	int GetLanguageArrayIndex( WORD LangId );
	BOOL    LoadResourceDLL(LPCTSTR lpDllFileName = NULL);
	BOOL    SetLanguage(WORD LangId, bool override=false);
	BOOL    SetLanguageOverride(WORD LangId) { return SetLanguage(LangId, true); }
	void    GetDllsAt( LPCTSTR SearchPath, CStringArray& DllAry );
	BOOL    GetLanguage( const CString& DllName, WORD& uiLanguage ) ;
	UINT    GetDocResId();
	CString GetPath( LPCTSTR FileName);
	CString GetLanguagePath(LPCTSTR FileName);
	CString GetLanguageString( WORD LangId );
	CString GetNativeLanguageNameString( int idx );
	void LoadAndDisplayLanguages();

// Dialog Data
	//{{AFX_DATA(CLanguageSelect)
	enum { IDD = IDD_LANGUAGE_SELECT };
	CListBox	m_ctlLangList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLanguageSelect)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL


	// Generated message map functions
	//{{AFX_MSG(CLanguageSelect)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkLanguageList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};




//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LANGUAGESELECT_H__4395A84F_E8DF_11D1_BBCB_00A024706EDC__INCLUDED_)
