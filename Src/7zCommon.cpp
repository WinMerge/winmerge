/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////

/* 7zCommon.cpp: Implement 7z related classes and functions
 * Copyright (c) 2003 Jochen Tucht
 *
 * Remarks:	Different versions of 7-Zip are interfaced through specific
 *			versions of Merge7z (Merge7z310.dll, Merge7z311.dll, etc.)
 *			WinMerge can either use an installed copy of the 7-Zip software,
 *			or fallback to a local set of 7-Zip DLLs, which are to be included
 *			in the WinMerge binary distribution.
 *
 *			Fallback policies are as follows:
 *
 *			1. Detect 7-Zip version installed (XXX).
 *			2. If there is a Merge7zXXX.dll, be happy to use it
 *			3. Detect 7-Zip version from WinMerge distribution (YYY).
 *			4. If there is a Merge7zYYY.dll, be happy to use it.
 *			5. Sorry, no way.
 *
 *			If there is a registry variable *Settings\ForceLocal7z*
 *			of type DWORD and value 1, steps 1 and 2 will be skipped.
 *

Please mind 2. a) of the GNU General Public License, and log your changes below.

DATE:		BY:					DESCRIPTION:
==========	==================	================================================
2003/12/09	Jochen Tucht		Created
2003/12/16	Jochen Tucht		Properly generate .tar.gz and .tar.bz2
2003/12/16	Jochen Tucht		Obtain long path to temporary folder
2004/01/20	Jochen Tucht		Complain only once if Merge7z*.dll is missing
2004/01/25	Jochen Tucht		Fix bad default for OPENFILENAME::nFilterIndex
2004/03/15	Jochen Tucht		Fix Visual Studio 2003 build issue
2004/04/13	Jochen Tucht		Avoid StrNCat to get away with shlwapi 4.71
2004/08/25	Jochen Tucht		More explicit error message
2004/10/17	Jochen Tucht		Leave decision whether to recurse into folders
								to enumerator (Mask.Recurse)
2004/11/03	Jochen Tucht		FIX [1048997] as proposed by Kimmo 2004-11-02
2005/01/15	Jochen Tucht		Read 7-Zip version from 7zip_pad.xml
								Set Merge7z UI language if DllBuild_Merge7z >= 9
2005/01/22	Jochen Tucht		Better explain what's present/missing/outdated
2005/02/05	Jochen Tucht		Fall back to IDD_MERGE7ZMISMATCH template from
								.exe if .lang file isn't up to date.
*/

// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "Merge.h"		// DirDocFilter theApp
#include "resource.h"
#include "DirDoc.h"
#include "MainFrm.h"
#include "7zCommon.h"
#include "markdown.h"
#include "language.h"
#include <afxinet.h>
#include <shlwapi.h>
#include <paths.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 * @brief Exception class for more explicit error message.
 */
class C7ZipMismatchException : public CException
{
public:
	C7ZipMismatchException(DWORD dwVer7zInstalled, DWORD dwVer7zLocal, CException *pCause)
	{
		m_dwVer7zInstalled = dwVer7zInstalled;
		m_dwVer7zLocal = dwVer7zLocal;
		m_pCause = pCause;
	}
	~C7ZipMismatchException()
	{
		if (m_pCause)
			m_pCause->Delete();
	}
	virtual int ReportError(UINT nType = MB_OK, UINT nMessageID = 0);
protected:
	DWORD m_dwVer7zInstalled;
	DWORD m_dwVer7zLocal;
	CException *m_pCause;
	BOOL m_bShowAllways;
	static const DWORD m_dwVer7zRecommended;
	static const TCHAR m_strRegistryKey[];
	static BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
	static DWORD FormatVersion(LPTSTR, LPTSTR, DWORD);
};

/**
 * @brief Recommended version of 7-Zip.
 */
const DWORD C7ZipMismatchException::m_dwVer7zRecommended = DWORD MAKELONG(13,3);

/**
 * @brief Registry key for C7ZipMismatchException's ReportError() popup.
 */
const TCHAR C7ZipMismatchException::m_strRegistryKey[] = _T("7ZipMismatch");

/**
 * @brief Retrieve build number of given DLL.
 */
static DWORD NTAPI GetDllBuild(LPCTSTR cPath)
{
	HMODULE hModule = LoadLibrary(cPath);
	DLLVERSIONINFO dvi;
	dvi.cbSize = sizeof dvi;
	dvi.dwBuildNumber = ~0UL;
	if (hModule)
	{
		dvi.dwBuildNumber = 0UL;
		DLLGETVERSIONPROC DllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hModule, "DllGetVersion");
		if (DllGetVersion)
		{
			DllGetVersion(&dvi);
		}
		FreeLibrary(hModule);
	}
	return dvi.dwBuildNumber;
}

/**
 * @brief Format Merge7z version number and plugin name, and retrieve DllBuild.
 */
DWORD C7ZipMismatchException::FormatVersion(LPTSTR pcVersion, LPTSTR pcPluginName, DWORD dwVersion)
{
	*pcVersion = '\0';
	*pcPluginName = '\0';
	if (dwVersion)
	{
		wsprintf
		(
			pcVersion, _T("%u.%02u"),
			UINT HIWORD(dwVersion),
			UINT LOWORD(dwVersion)
		);
		wsprintf
		(
			pcPluginName,
			sizeof(TCHAR) == 1 ? _T("Merge7z%u%02u.dll") : _T("Merge7z%u%02uU.dll"),
			UINT HIWORD(dwVersion),
			UINT LOWORD(dwVersion)
		);
	}
	return GetDllBuild(pcPluginName);
}

/**
 * @brief Populate ListBox with names/revisions of DLLs matching given pattern.
 */
static void NTAPI DlgDirListDLLs(HWND hWnd, LPTSTR cPattern, int nIDListBox)
{
	HDC hDC = GetDC(hWnd);
	HFONT hFont = (HFONT)SendDlgItemMessage(hWnd, nIDListBox, WM_GETFONT, 0, 0);
	int cxView = (int)SendDlgItemMessage(hWnd, nIDListBox, LB_GETHORIZONTALEXTENT, 0, 0) - 8;
	HGDIOBJ hObject = SelectObject(hDC, hFont);
	WIN32_FIND_DATA ff;
	HANDLE h = FindFirstFile(cPattern, &ff);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			PathRemoveFileSpec(cPattern);
			PathAppend(cPattern, ff.cFileName);
			wsprintf(ff.cFileName, _T(" (dllbuild %04u)"), GetDllBuild(cPattern));
			lstrcat(cPattern, ff.cFileName);
			int cxText = (int)(WORD)GetTabbedTextExtent(hDC, cPattern, lstrlen(cPattern), 0, 0);
			if (cxView < cxText)
				cxView = cxText;
			SendDlgItemMessage(hWnd, nIDListBox, LB_ADDSTRING, 0, (LPARAM)cPattern);
		} while (FindNextFile(h, &ff));
		FindClose(h);
	}
	SelectObject(hDC, hObject);
	ReleaseDC(hWnd, hDC);
	SendDlgItemMessage(hWnd, nIDListBox, LB_SETHORIZONTALEXTENT, cxView + 8, 0);
}

/**
 * @brief DLGPROC for C7ZipMismatchException's ReportError() popup.
 */
BOOL CALLBACK C7ZipMismatchException::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			if (GetDlgItem(hWnd, 9001) == NULL)
			{
				// Dialog template isn't up to date. Give it a second chance.
				EndDialog(hWnd, -1);
				return FALSE;
			}
			C7ZipMismatchException *pThis = (C7ZipMismatchException *)lParam;
			TCHAR cText[2600], cPresent[80], cMissing[80], cOutdated[80], cNone[80], cPlugin[80];
			if (pThis->m_pCause)
			{
				pThis->m_pCause->GetErrorMessage(cText, sizeof cText/sizeof*cText);
				SetDlgItemText(hWnd, 107, cText);
			}
			cPresent[GetDlgItemText(hWnd, 112, cPresent, 80)] = '\0';
			cMissing[GetDlgItemText(hWnd, 122, cMissing, 80)] = '\0';
			cOutdated[GetDlgItemText(hWnd, 132, cOutdated, 80)] = '\0';
			cNone[GetDlgItemText(hWnd, 120, cNone, 80)] = '\0';
			cPlugin[GetDlgItemText(hWnd, 102, cPlugin, 80)] = '\0';
			wsprintf(cText, cPlugin, DllBuild_Merge7z);
			SetDlgItemText(hWnd, 102, cText);
			SetDlgItemText
			(
				hWnd, 109,
				(
					pThis->m_dwVer7zRecommended == pThis->m_dwVer7zInstalled
				||	pThis->m_dwVer7zRecommended == pThis->m_dwVer7zLocal
				) ? cPresent : cMissing
			);
			DWORD dwDllBuild = FormatVersion(cText, cPlugin, pThis->m_dwVer7zRecommended);
			SetDlgItemText(hWnd, 110, *cText ? cText : cNone);
			SetDlgItemText(hWnd, 111, cPlugin);
			SetDlgItemText(hWnd, 112, *cPlugin == '\0' ? cPlugin : dwDllBuild == ~0 ? cMissing : dwDllBuild < DllBuild_Merge7z ? cOutdated : cPresent);
			dwDllBuild = FormatVersion(cText, cPlugin, pThis->m_dwVer7zInstalled);
			SetDlgItemText(hWnd, 120, *cText ? cText : cNone);
			SetDlgItemText(hWnd, 121, cPlugin);
			SetDlgItemText(hWnd, 122, *cPlugin == '\0' ? cPlugin : dwDllBuild == ~0 ? cMissing : dwDllBuild < DllBuild_Merge7z ? cOutdated : cPresent);
			dwDllBuild = FormatVersion(cText, cPlugin, pThis->m_dwVer7zLocal);
			SetDlgItemText(hWnd, 130, *cText ? cText : cNone);
			SetDlgItemText(hWnd, 131, cPlugin);
			SetDlgItemText(hWnd, 132, *cPlugin == '\0' ? cPlugin : dwDllBuild == ~0 ? cMissing : dwDllBuild < DllBuild_Merge7z ? cOutdated : cPresent);
			GetModuleFileName(0, cText, MAX_PATH);
			PathRemoveFileSpec(cText);
			PathAppend(cText, _T("Merge7z*.dll"));
			DlgDirListDLLs(hWnd, cText, 105);
			if (DWORD cchPath = GetEnvironmentVariable(_T("path"), 0, 0))
			{
				static const TCHAR cSep[] = _T(";");
				LPTSTR pchPath = new TCHAR[cchPath];
				GetEnvironmentVariable(_T("PATH"), pchPath, cchPath);
				LPTSTR pchItem = pchPath;
				while (int cchItem = StrCSpn(pchItem += StrSpn(pchItem, cSep), cSep))
				{
					if (cchItem < MAX_PATH)
					{
						CopyMemory(cText, pchItem, cchItem*sizeof*pchItem);
						cText[cchItem] = 0;
						PathAppend(cText, _T("Merge7z*.dll"));
						DlgDirListDLLs(hWnd, cText, 105);
					}
					pchItem += cchItem;
				}
				delete[] pchPath;
			}
			if (SendDlgItemMessage(hWnd, 105, LB_GETCOUNT, 0, 0) == 0)
			{
				SendDlgItemMessage(hWnd, 105, LB_ADDSTRING, 0, (LPARAM) cNone);
			}
			HICON hIcon = LoadIcon(0, IDI_EXCLAMATION);
			SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM) hIcon);
			SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM) hIcon);
			if (pThis->m_bShowAllways)
			{
				ShowWindow(GetDlgItem(hWnd, 106), SW_HIDE);
			}
		} return TRUE;
		case WM_COMMAND:
		{
			switch (wParam)
			{
				case IDOK:
				case IDCANCEL:
				{
					int nDontShowAgain = SendDlgItemMessage(hWnd, 106, BM_GETCHECK, 0, 0);
					EndDialog(hWnd, MAKEWORD(IDOK, nDontShowAgain));
				} break;
			}
		} return TRUE;
	}
	return FALSE;
}

/**
 * @brief Tell user what went wrong and how she can help.
 */
int C7ZipMismatchException::ReportError(UINT nType, UINT nMessageID)
{
	int response = -1;
	m_bShowAllways = nMessageID;
	if (!m_bShowAllways)
	{
		response = theApp.GetProfileInt(REGISTRY_SECTION_MESSAGEBOX, m_strRegistryKey, -1);
	}
	if (response == -1)
	{
		HWND hwndOwner = CWnd::GetSafeOwner()->GetSafeHwnd();
		MessageBeep(nType & MB_ICONMASK);
		response = DialogBoxParam(AfxGetResourceHandle(), MAKEINTRESOURCE(IDD_MERGE7ZMISMATCH), hwndOwner, DlgProc, (LPARAM)this);
		if (response == -1)
		{
			response = DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_MERGE7ZMISMATCH), hwndOwner, DlgProc, (LPARAM)this);
			ASSERT(response != -1);
		}
		if (HIBYTE(response) == 1)
		{
			theApp.WriteProfileInt(REGISTRY_SECTION_MESSAGEBOX, m_strRegistryKey, response = int LOBYTE(response));
		}
	}
	return response;
}

/**
 * @brief Check whether archive support is available.
 */
int NTAPI HasZipSupport()
{
	static int HasZipSupport = -1;
	if (HasZipSupport == -1)
	{
		try
		{
			Merge7z.operator->();
			HasZipSupport = 1;
		}
		catch (CException *e)
		{
			e->Delete();
			HasZipSupport = 0;
		}
	}
	return HasZipSupport;
}

/**
 * @brief Tell user why archive support is not available.
 */
void NTAPI Recall7ZipMismatchError()
{
	try
	{
		Merge7z.operator->();
	}
	catch (CException *e)
	{
		e->ReportError(MB_ICONEXCLAMATION, TRUE);
		e->Delete();
	}
}

/**
 * @brief Recursively delete folder.
 */
void CTempPath::Clear()
{
	// SHFileOperation expects a ZZ terminated list of paths!
	LPTSTR pchPath = m_strPath.GetBufferSetLength(lstrlen(m_strPath) + 1);
	SHFILEOPSTRUCT fileop = {0, FO_DELETE, pchPath, 0, FOF_NOCONFIRMATION|FOF_SILENT|FOF_NOERRORUI, 0, 0, 0};
	SHFileOperation(&fileop);
}

/**
 * @brief Prepare an additional sibling folder.
 */
void CTempPath::MakeSibling(LPCTSTR pchExt)
{
	LPTSTR pchPath = m_strPath.GetBuffer(lstrlen(m_strPath) + lstrlen(pchExt) + 1);
	PathRenameExtension(pchPath, pchExt);
	m_strPath.ReleaseBuffer();
	Clear();
}

/**
 * @brief Prepare a temporary folder.
 */
CTempPath::CTempPath(LPVOID pOwner)
{
	m_strPath.Format
	(
		pOwner ? _T("%sWINMERGE.%08lX\\%08lX") : _T("%sWINMERGE.%08lX"),
		paths_GetTempPath(), GetCurrentProcessId(), pOwner
	);
	Clear();
}

/**
 * @brief Return installed or local version of 7-Zip.
 */
DWORD NTAPI VersionOf7z(BOOL bLocal)
{
	TCHAR path[MAX_PATH];
	if (bLocal)
	{
		GetModuleFileName(0, path, sizeof path/sizeof*path);
		PathRemoveFileSpec(path);
	}
	else
	{
		static const TCHAR szSubKey[] = _T("Software\\7-Zip");
		static const TCHAR szValue[] = _T("Path");
		DWORD type = 0;
		DWORD size = sizeof path;
		SHGetValue(HKEY_LOCAL_MACHINE, szSubKey, szValue, &type, path, &size);
	}
	PathAppend(path, _T("7zip_pad.xml"));
	CMarkdown::String version
	(
		CMarkdown::File(path)
		.Move("XML_DIZ_INFO").Pop()
		.Move("Program_Info").Pop()
		.Move("Program_Version").GetInnerText()
	);
	DWORD ver = (WORD)StrToIntA(version.A) << 16;
	if (LPSTR p = StrChrA(version.A, '.'))
	{
		ver |= (WORD)StrToIntA(p + 1);
	}
	return ver;
}

/**
 * @brief Callback to pass to EnumResourceLanguages.
 */
BOOL CALLBACK FindNextResLang(HMODULE hModule, LPCTSTR lpType, LPCTSTR lpName, WORD wLanguage, LONG lParam)
{
	LPWORD pwLanguage = (LPWORD)lParam;
	WORD wPrevious = *pwLanguage;
	if (wPrevious == 0 || wPrevious == wLanguage)
	{
		*pwLanguage ^= wLanguage;
		return wPrevious;
	}
	return TRUE;
}

/**
 * @brief Build 7z language file name from language id.
 */
CString NTAPI LangFile7z()
{
	CString langFile;
	if (HINSTANCE hinstLang = AfxGetResourceHandle())
	{
		WORD wLangID = 0;
		if (EnumResourceLanguages(hinstLang, RT_VERSION, MAKEINTRESOURCE(VS_VERSION_INFO), FindNextResLang, (LPARAM)&wLangID) == 0)
		{
			if (const char *lang = GetLanguageFromId(wLangID))
			{
				langFile = lang;
				langFile.Replace('_', '-');
			}
		}
	}
	return langFile;
}

/**
 * @brief Access dll functions through proxy.
 */
interface Merge7z *Merge7z::Proxy::operator->()
{
	if (const char *format = Merge7z[0])
	{
		char name[MAX_PATH];
		DWORD flags = ~0;
		CException *pCause = NULL;
		switch (theApp.GetProfileInt(_T("Settings"), _T("ForceLocal7z"), 0))
		{
		case 0:
			if (DWORD ver = VersionOf7z(FALSE))
			{
				flags = Initialize::Default;
				try
				{
					wsprintfA(name, format, UINT HIWORD(ver), UINT LOWORD(ver));
					Merge7z[0] = name;
					DllProxyHelper(Merge7z, DllBuild_Merge7z);
					break;
				}
				catch (CException *e)
				{
					Merge7z[0] = format;
					pCause = e;
				}
			}
		default:
			if (DWORD ver = VersionOf7z(TRUE))
			{
				flags = Initialize::Default | Initialize::Local7z;
				try
				{
					wsprintfA(name, format, UINT HIWORD(ver), UINT LOWORD(ver));
					Merge7z[0] = name;
					DllProxyHelper(Merge7z, DllBuild_Merge7z);
					break;
				}
				catch (CException *e)
				{
					Merge7z[0] = format;
					if (pCause) pCause->Delete();
					pCause = e;
				}
			}
			if (flags == ~0)
			{
				// 7-Zip not present: Fail silently.
				static CSilentException *pSilentException = NULL;
				if (pSilentException == NULL)
				{
					// Leave an intentional memory leak so the leak dump will
					// reveal where the error occured.
					pSilentException = new CSilentException;
				}
				throw pSilentException;
			}
			throw new C7ZipMismatchException
			(
				VersionOf7z(FALSE),
				VersionOf7z(TRUE),
				pCause
			);
		}
		((interface Merge7z *)Merge7z[1])->Initialize(flags);
#if DllBuild_Merge7z >= 9
		((interface Merge7z *)Merge7z[1])->LoadLang(LangFile7z());
#endif
	}
	return ((interface Merge7z *)Merge7z[1]);
}

/**
 * @brief Proxy for Merge7z
 */
Merge7z::Proxy Merge7z =
{
	"Merge7z%u%02u"
#ifdef _UNICODE
	"U"
#endif
	,"Merge7z"
};

/**
 * @brief Tell Merge7z we are going to enumerate just 1 item.
 */
UINT CompressSingleFile::Open()
{
	return 1;
}

/**
 * @brief Pass information about the item to Merge7z.
 */
Merge7z::Envelope *CompressSingleFile::Enum(Item &item)
{
	item.Mask.Item = item.Mask.Name|item.Mask.FullPath|item.Mask.Recurse;
	item.Name = Name;
	item.FullPath = FullPath;
	return 0;
}

/**
 * @brief CompressSingleFile constructor. Called like a function.
 */
CompressSingleFile::CompressSingleFile(LPCTSTR path, LPCTSTR FullPath, LPCTSTR Name)
: FullPath(FullPath)
, Name(Name)
{
	if (Merge7z::Format *piHandler = Merge7z->GuessFormat(path))
	{
		HWND hwndOwner = CWnd::GetSafeOwner()->GetSafeHwnd();
		piHandler->CompressArchive(hwndOwner, path, this);
	}
	else
	{
		AfxMessageBox(IDS_UNKNOWN_ARCHIVE_FORMAT, MB_ICONEXCLAMATION);
	}
}

/**
 * @brief Construct a CDirView::DirItemEnumerator.
 *
 * Argument *nFlags* controls operation as follows:
 * LVNI_ALL:		Enumerate all items.
 * LVNI_SELECTED:	Enumerate selected items only.
 * Original:		Set folder prefix for first iteration to "original"
 * Altered:			Set folder prefix for second iteration to "altered"
 * BalanceFolders:	Ensure that all nonempty folders on either side have a
 *					corresponding folder on the other side, even if it is
 *					empty (DirScan doesn't recurse into folders which
 *					appear only on one side).
 * DiffsOnly:		Enumerate diffs only.
 */
CDirView::DirItemEnumerator::DirItemEnumerator(CDirView *pView, int nFlags)
: m_pView(pView)
, m_nFlags(nFlags)
{
	if (m_nFlags & Original)
	{
		m_rgFolderPrefix.AddTail(_T("original"));
	}
	if (m_nFlags & Altered)
	{
		m_rgFolderPrefix.AddTail(_T("altered"));
	}
	if (m_nFlags & BalanceFolders)
	{
		// Collect implied folders
		for (UINT i = Open() ; i-- ; )
		{
			const DIFFITEM &di = Next();
			if ((m_nFlags & DiffsOnly) && !m_pView->IsItemNavigableDiff(di))
			{
				continue;
			}
			if (m_bRight) 
			{
				// Enumerating items on right side
				if (!di.isSideLeft())
				{
					// Item is present on right side, i.e. folder is implied
					m_rgImpliedFoldersRight[di.sSubdir] = PVOID(1);
				}
			}
			else
			{
				// Enumerating items on left side
				if (!di.isSideRight())
				{
					// Item is present on left side, i.e. folder is implied
					m_rgImpliedFoldersLeft[di.sSubdir] = PVOID(1);
				}
			}
		}
	}
}

/**
 * @brief Initialize enumerator, return number of items to be enumerated.
 */
UINT CDirView::DirItemEnumerator::Open()
{
	m_nIndex = -1;
	m_curFolderPrefix = m_rgFolderPrefix.GetHeadPosition();
	m_bRight = (m_nFlags & Right) != 0;
	int nrgFolderPrefix = m_rgFolderPrefix.GetCount();
	if (nrgFolderPrefix)
	{
		m_strFolderPrefix = m_rgFolderPrefix.GetNext(m_curFolderPrefix);
	}
	else
	{
		nrgFolderPrefix = 1;
	}
	return
	(
		m_nFlags & LVNI_SELECTED
	?	pView(m_pView)->GetSelectedCount()
	:	pView(m_pView)->GetItemCount()
	) * nrgFolderPrefix;
}

/**
 * @brief Return next item.
 */
const DIFFITEM &CDirView::DirItemEnumerator::Next()
{
	enum {nMask = LVNI_FOCUSED|LVNI_SELECTED|LVNI_CUT|LVNI_DROPHILITED};
	while ((m_nIndex = pView(m_pView)->GetNextItem(m_nIndex, m_nFlags & nMask)) == -1)
	{
		m_strFolderPrefix = m_rgFolderPrefix.GetNext(m_curFolderPrefix);
		m_bRight = TRUE;
	}
	return m_pView->GetItemAt(m_nIndex);
}

/**
 * @brief Pass information about an item to Merge7z.
 *
 * Information is passed through struct Merge7z::DirItemEnumerator::Item.
 * The *mask* member denotes which of the other members contain valid data.
 * If *mask* is zero upon return, which will be the case if Enum() decides to
 * leave the struct untouched, Merge7z will ignore the item.
 * If Enum() allocates temporary storage for string members, it must also
 * allocate an Envelope, providing a Free() method to free the temporary
 * storage, along with the Envelope itself. The Envelope pointer is passed to
 * Merge7z as the return value of the function. It is not meant to be a success
 * indicator, so if no temporary storage is required, it is perfectly alright
 * to return NULL.
 */
Merge7z::Envelope *CDirView::DirItemEnumerator::Enum(Item &item)
{
	CDiffContext *pCtxt = m_pView->GetDiffContext();
	const DIFFITEM &di = Next();

	if ((m_nFlags & DiffsOnly) && !m_pView->IsItemNavigableDiff(di))
	{
		return 0;
	}

	Envelope *envelope = new Envelope;

	envelope->Name = di.sfilename;
	if (di.sSubdir.GetLength())
	{
		envelope->Name.Insert(0, '\\');
		envelope->Name.Insert(0, di.sSubdir);
	}
	envelope->FullPath = di.sfilename;
	envelope->FullPath.Insert(0, '\\');
	envelope->FullPath.Insert(0, m_bRight ? di.getRightFilepath(pCtxt) : di.getLeftFilepath(pCtxt));

	if (m_nFlags & BalanceFolders)
	{
		if (m_bRight)
		{
			// Enumerating items on right side
			if (di.isSideLeft())
			{
				// Item is missing on right side
				PVOID &implied = m_rgImpliedFoldersRight[di.sSubdir];
				if (!implied)
				{
					// Folder is not implied by some other file, and has
					// not been enumerated so far, so enumerate it now!
					envelope->Name = di.sSubdir;
					envelope->FullPath = di.getLeftFilepath(pCtxt);
					implied = PVOID(2); // Don't enumerate same folder twice!
				}
			}
		}
		else
		{
			// Enumerating items on left side
			if (di.isSideRight())
			{
				// Item is missing on left side
				PVOID &implied = m_rgImpliedFoldersLeft[di.sSubdir];
				if (!implied)
				{
					// Folder is not implied by some other file, and has
					// not been enumerated so far, so enumerate it now!
					envelope->Name = di.sSubdir;
					envelope->FullPath = di.getRightFilepath(pCtxt);
					implied = PVOID(2); // Don't enumerate same folder twice!
				}
			}
		}
	}

	if (m_strFolderPrefix.GetLength())
	{
		envelope->Name.Insert(0, '\\');
		envelope->Name.Insert(0, m_strFolderPrefix);
	}

	item.Mask.Item = item.Mask.Name|item.Mask.FullPath|item.Mask.CheckIfPresent|item.Mask.Recurse;
	item.Name = envelope->Name;
	item.FullPath = envelope->FullPath;
	return envelope;
}
 //<winerror.h>
/**
 * @brief Generate archive from DirView items.
 */
void CDirView::DirItemEnumerator::CompressArchive(LPCTSTR path)
{
	CString strPath;
	if (path == 0)
	{
		// No path given, so prompt for path!
		static const TCHAR _T_Merge7z[] = _T("Merge7z");
		static const TCHAR _T_FilterIndex[] = _T("FilterIndex");
		// 7z311 can only write 7z, zip, and tar(.gz|.bz2) archives, so don't
		// offer other formats here!
		static const TCHAR _T_Filter[]
		(
			_T("7z|*.7z|")
			//_T("z|*.z|")
			_T("zip|*.zip|")
			_T("jar (zip)|*.jar|")
			_T("xpi (zip)|*.xpi|")
			//_T("rar|*.rar|")
			_T("tar|*.tar|")
			_T("tar.gz|*.tar.gz|")
			_T("tar.bz2|*.tar.bz2|")
			//_T("cab|*.cab|")
			//_T("arj|*.arj|")
			//_T("deb|*.deb|")
			//_T("rpm|*.rpm|")
			//_T("cpio|*.cpio|")
			_T("|")
		);
		// Make CFileDialog static to preserve settings across invocations:
 		static CFileDialog dlg
		(
			FALSE,
			0,
			0,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN,
			_T_Filter
		);
		dlg.m_ofn.nFilterIndex = AfxGetApp()->GetProfileInt(_T_Merge7z, _T_FilterIndex, 1);
		// Use extension from current filter as default extension:
		if (int i = dlg.m_ofn.nFilterIndex)
		{
			dlg.m_ofn.lpstrDefExt = dlg.m_ofn.lpstrFilter;
			while (*dlg.m_ofn.lpstrDefExt && --i)
			{
				dlg.m_ofn.lpstrDefExt += lstrlen(dlg.m_ofn.lpstrDefExt) + 1;
				dlg.m_ofn.lpstrDefExt += lstrlen(dlg.m_ofn.lpstrDefExt) + 1;
			}
			if (*dlg.m_ofn.lpstrDefExt)
			{
				dlg.m_ofn.lpstrDefExt += lstrlen(dlg.m_ofn.lpstrDefExt) + 3;
			}
		}
		if (dlg.DoModal() == IDOK)
		{
			path = strPath = dlg.GetPathName();
			AfxGetApp()->WriteProfileInt(_T_Merge7z, _T_FilterIndex, dlg.m_ofn.nFilterIndex);
		}
	}
	if (path)
	{
		LPCTSTR pathFinal = 0;
		CString strPathIntermediate;
		if (StrRStrI(path, 0, _T(".tar.")) == PathFindExtension(path) - 4)
		{
			// Extension is preceeded by .tar:
			// - Keep full path in pathFinal for actual compression.
			// - Cut off extension to produce an intermediate tar first.
			strPathIntermediate = pathFinal = path;
			path = strPathIntermediate.GetBufferSetLength(strPathIntermediate.ReverseFind('.'));
		}
		if (Merge7z::Format *piHandler = Merge7z->GuessFormat(path))
		{
			HWND hwndOwner = CWnd::GetSafeOwner()->GetSafeHwnd();
			piHandler->CompressArchive(hwndOwner, path, this);
		}
		else
		{
			AfxMessageBox(IDS_UNKNOWN_ARCHIVE_FORMAT, MB_ICONEXCLAMATION);
		}
		if (pathFinal)
		{
			// Compress the intermediate tar to pathFinal, then delete it.
			CompressSingleFile(pathFinal, path);
			DeleteFile(path);
		}
	}
#ifdef _DEBUG
	afxDump << m_rgImpliedFoldersLeft;
	afxDump << m_rgImpliedFoldersRight;
#endif
}

/**
 * @brief Collect files for SHFileOperation
 */
void CDirView::DirItemEnumerator::CollectFiles(CString &strBuffer)
{
	CDiffContext *pCtxt = m_pView->GetDiffContext();
	UINT i;
	int cchBuffer = 0;
	for (i = Open() ; i-- ; )
	{
		const DIFFITEM &di = Next();
		if (m_bRight ? m_pView->IsItemOpenableOnRightWith(di) : m_pView->IsItemOpenableOnLeftWith(di))
		{
			cchBuffer +=
			(
				m_bRight ? di.getRightFilepath(pCtxt) : di.getLeftFilepath(pCtxt)
			).GetLength() + di.sfilename.GetLength() + 2;
		}
	}
	LPTSTR pchBuffer = strBuffer.GetBufferSetLength(cchBuffer);
	for (i = Open() ; i-- ; )
	{
		const DIFFITEM &di = Next();
		if (m_bRight ? m_pView->IsItemOpenableOnRightWith(di) : m_pView->IsItemOpenableOnLeftWith(di))
		{
			pchBuffer += wsprintf
			(
				pchBuffer,
				_T("%s\\%s"),
				m_bRight ? di.getRightFilepath(pCtxt) : di.getLeftFilepath(pCtxt),
				di.sfilename
			) + 1;
		}
	}
	ASSERT(pchBuffer - strBuffer == cchBuffer);
}
