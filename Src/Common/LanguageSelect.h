#if !defined(AFX_LANGUAGESELECT_H__4395A84F_E8DF_11D1_BBCB_00A024706EDC__INCLUDED_)
#define AFX_LANGUAGESELECT_H__4395A84F_E8DF_11D1_BBCB_00A024706EDC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LanguageSelect.h : header file
//
#include "logfile.h"

/////////////////////////////////////////////////////////////////////////////
// CLanguageSelect dialog
#define LANGUAGE_SECTION  "Locale"
#define COUNTRY_ENTRY	 "LanguageId"


class CLanguageSelect : public CDialog
{
// Construction
public:
	void SetModuleHandle(HMODULE hModule) { m_hModule = hModule; }
	CLanguageSelect(UINT idMainMenu, UINT idDocMenu, BOOL bReloadMenu =TRUE, BOOL bUpdateTitle =TRUE, CWnd* pParent = NULL);   // standard constructor
	void GetAvailLangs( CWordArray& uiLanguageAry, CStringArray& DllFileNameAry );	   
	WORD GetLangId() { return m_wCurLanguage; };
	void InitializeLanguage();
	CString GetDllName (WORD LangId );
	void	UpdateDocTitle();
	void	ReloadMenu();
	CLogFile *m_pLog;
	void SetLogFile(CLogFile* pLog) { m_pLog = pLog; }
	

private:
	BOOL m_bReloadMenu;
	BOOL m_bUpdateTitle;
	HMODULE m_hModule;
	UINT m_idMainMenu;
	UINT m_idDocMenu;
	HINSTANCE  m_hCurrentDll;
	WORD	   m_wCurLanguage;
	int GetLanguageArrayIndex( WORD LangId );
	CStringArray m_DllFileNameAry;
	BOOL	LoadResourceDLL(LPCTSTR lpDllFileName = NULL);
	BOOL	SetLanguage(WORD LangId);
	void	GetDllsAt( LPCTSTR SearchPath, CStringArray& DllAry );
	BOOL	GetLanguage( const CString& DllName, WORD& uiLanguage ) ;
	
	UINT	GetDocResId();
	CString GetPath( LPCTSTR FileName);
	
	CString GetLanguageString( WORD LangId );
	WORD  GetLanguageId(const CString& Language );

// Dialog Data
	//{{AFX_DATA(CLanguageSelect)
	enum { IDD = 30000 };
	CListBox	m_ctlLangList;
	CString	m_strLanguage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLanguageSelect)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   CWordArray   m_wLangIds;

	// Generated message map functions
	//{{AFX_MSG(CLanguageSelect)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};




//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LANGUAGESELECT_H__4395A84F_E8DF_11D1_BBCB_00A024706EDC__INCLUDED_)
