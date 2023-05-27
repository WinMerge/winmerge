/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  Merge.h
 *
 * @brief main header file for the MERGE application
 *
 */
#pragma once

#define WMU_CHILDFRAMEADDED						(WM_APP + 10)
#define WMU_CHILDFRAMEREMOVED					(WM_APP + 11)
#define WMU_CHILDFRAMEACTIVATE					(WM_APP + 12)
#define WMU_CHILDFRAMEACTIVATED					(WM_APP + 13)
constexpr UINT_PTR IDT_UPDATEMAINMENU = 1;

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include <memory>
#include <list>
#include "MergeCmdLineInfo.h"
#include "resource.h"       // main symbols

struct FileFilter;
class FileFilterHelper;
class CAssureScriptsForThread;
class CMainFrame;
class CLanguageSelect;
class MergeCmdLineInfo;
class ProjectFile;
class COptionsMgr;
class LineFiltersList;
class SubstitutionFiltersList;
class SyntaxColors;
class CCrystalTextMarkers;
class PackingInfo;
class MergeAppCOMClass;
namespace JumpList { struct Item; }

/////////////////////////////////////////////////////////////////////////////
// CMergeApp:
// See Merge.cpp for the implementation of this class
//

enum { IDLE_TIMER = 9754 };

/**
 * @brief WinMerge application class
 */
class CMergeApp : public CWinApp
{
public:
	bool m_bNeedIdleTimer;
	CMultiDocTemplate* m_pOpenTemplate;
	CMultiDocTemplate* m_pDiffTemplate;
	CMultiDocTemplate* m_pHexMergeTemplate;
	CMultiDocTemplate* m_pDirTemplate;
	std::unique_ptr<CLanguageSelect> m_pLangDlg;
	std::unique_ptr<SyntaxColors> m_pSyntaxColors; /**< Syntax color container */
	std::unique_ptr<CCrystalTextMarkers> m_pMarkers; /**< Marker container */
	String m_strSaveAsPath; /**< "3rd path" where output saved if given */
	bool m_bEscShutdown; /**< If commandline switch -e given ESC closes appliction */
	SyntaxColors * GetMainSyntaxColors() { return m_pSyntaxColors.get(); }
	CCrystalTextMarkers * GetMainMarkers() const { return m_pMarkers.get(); }
	MergeCmdLineInfo::ExitNoDiff m_bExitIfNoDiff; /**< Exit if files are identical? */
	std::unique_ptr<LineFiltersList> m_pLineFilters; /**< List of linefilters */
	std::unique_ptr<SubstitutionFiltersList> m_pSubstitutionFiltersList;

	WORD GetLangId() const;
	String GetLangName() const;
	void SetIndicators(CStatusBar &, const UINT *, int) const;
	void TranslateMenu(HMENU) const;
	void TranslateDialog(HWND) const;
	String LoadString(UINT) const;
	bool TranslateString(const std::string&, String&) const;
	bool TranslateString(const std::wstring&, String&) const;
	std::wstring LoadDialogCaption(const tchar_t*) const;

	CMergeApp();
	~CMergeApp();

public:
	void AddToRecentProjectsMRU(const tchar_t* sPathName);
	void SetNeedIdleTimer();
	void SetLastCompareResult(int nResult) { m_nLastCompareResult = nResult; }

	COptionsMgr * GetMergeOptionsMgr() { return static_cast<COptionsMgr *> (m_pOptions.get()); }
	FileFilterHelper* GetGlobalFileFilter();
	void ShowHelp(const tchar_t* helpLocation = nullptr);
	static void OpenFileToExternalEditor(const String& file, int nLineNumber = 1);
	static bool CreateBackup(bool bFolder, const String& pszPath);
	static int HandleReadonlySave(String& strSavePath, bool bMultiFile, bool &bApplyToAll);
	static String GetPackingErrorMessage(int pane, int paneCount, const String& path, const PackingInfo& plugin);
	static std::vector<JumpList::Item> CreateUserTasks(MergeCmdLineInfo::usertasksflags_t flags);
	bool GetMergingMode() const;
	void SetMergingMode(bool bMergingMode);
	static void SetupTempPath();
	bool IsReallyIdle() const;
	void RegisterIdleFunc(std::function<void()> func) { m_idleFuncs.push_back(func); };

	virtual UINT GetProfileInt(const tchar_t* lpszSection, const tchar_t* lpszEntry, int nDefault) override;
	virtual BOOL WriteProfileInt(const tchar_t* lpszSection, const tchar_t* lpszEntry, int nValue) override;
	virtual CString GetProfileString(const tchar_t* lpszSection, const tchar_t* lpszEntry, const tchar_t* lpszDefault = NULL) override;
	virtual BOOL WriteProfileString(const tchar_t* lpszSection, const tchar_t* lpszEntry, const tchar_t* lpszValue) override;
	virtual HINSTANCE LoadAppLangResourceDLL() override { return nullptr; }; // Disable loading lang resource dll

// Implementation
protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int DoMessageBox(const tchar_t* lpszPrompt, UINT nType, UINT nIDPrompt);
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

	void InitializeFileFilters();
	bool ParseArgsAndDoOpen(MergeCmdLineInfo& cmdInfo, CMainFrame* pMainFrame);
	void UpdateDefaultCodepage(int cpDefaultMode, int cpCustomCodepage);
	void UpdateCodepageModule();
	void ApplyCommandLineConfigOptions(MergeCmdLineInfo & cmdInfo);
	bool ShowCompareAsMenu(MergeCmdLineInfo& cmdInfo);
	void ShowDialog(MergeCmdLineInfo::DialogType type);

	// End MergeArgs.cpp

	bool LoadProjectFile(const String& sProject, ProjectFile &project);
	bool SaveProjectFile(const String& sProject, const ProjectFile &project);
	bool LoadAndOpenProjectFile(const String& sFilepath, const String& sReportFile = _T(""));
	bool IsProjectFile(const String& filepath) const;

	//@{
	/**
	 * @name Active operations counter.
	 * These functions implement counter for active operations. We need to
	 * track active operations during whose user cannot exit the application.
	 * E.g. copying files in folder compare is such an operation.
	 */
	/**
	 * Increment the active operation counter.
	 */
	void AddOperation() { InterlockedIncrement(&m_nActiveOperations); }
	/**
	 * Decrement the active operation counter.
	 */
	void RemoveOperation()
	{
		ASSERT(m_nActiveOperations > 0);
		InterlockedDecrement( &m_nActiveOperations);
	}
	/**
	 * Get the active operations count.
	 */
	LONG GetActiveOperations() const { return m_nActiveOperations; }
	//@}

	//{{AFX_MSG(CMergeApp)
	afx_msg BOOL OnOpenRecentFile(UINT nID);
	afx_msg void OnAppAbout();
	afx_msg void OnHelp();
	afx_msg void OnMergingMode();
	afx_msg void OnUpdateMergingMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMergingStatus(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	std::unique_ptr<COptionsMgr> m_pOptions;
	std::unique_ptr<FileFilterHelper> m_pGlobalFileFilter;
	CAssureScriptsForThread * m_mainThreadScripts;
	int m_nLastCompareResult;
	bool m_bNonInteractive;
	LONG m_nActiveOperations; /**< Active operations count. */
	bool m_bMergingMode; /**< Merging or Edit mode */
	bool m_bEnableExitCode;
	CFont m_fontGUI;
	ATL::CImage m_imageForInitializingGdiplus;
	std::list<std::function<void()>> m_idleFuncs;
};

extern CMergeApp theApp;

/**
 * @brief Set flag so that application will broadcast notification at next
 * idle time (via WM_TIMER id=IDLE_TIMER)
 */
inline void CMergeApp::SetNeedIdleTimer()
{
	m_bNeedIdleTimer = true;
}

