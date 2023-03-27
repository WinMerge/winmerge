/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
//
/////////////////////////////////////////////////////////////////////////////

/* 7zCommon.cpp: Implement 7z related classes and functions
 * Copyright (c) 2003 Jochen Tucht
 *
 * Remarks:	Different versions of 7-Zip are interfaced through specific
 *			versions of Merge7z (Merge7z311.dll, Merge7z312.dll, etc.)
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
 *			These rules can be customized by setting a registry variable
 *			*Merge7z/Enable* of type DWORD to one of the following values:
 *
 *			0 - Entirely disable 7-Zip integration.
 *			1 - Use installed 7-Zip if present. Otherwise, use local 7-Zip.
 *			2 - Always use local 7-Zip.
 *

Please mind 2. a) of the GNU General Public License, and log your changes below.

DATE:		BY:					DESCRIPTION:
==========	==================	================================================
2003-12-09	Jochen Tucht		Created
2003-12-16	Jochen Tucht		Properly generate .tar.gz and .tar.bz2
2003-12-16	Jochen Tucht		Obtain long path to temporary folder
2004-01-20	Jochen Tucht		Complain only once if Merge7z*.dll is missing
2004-01-25	Jochen Tucht		Fix bad default for OPENFILENAME::nFilterIndex
2004-03-15	Jochen Tucht		Fix Visual Studio 2003 build issue
2004-04-13	Jochen Tucht		Avoid StrNCat to get away with shlwapi 4.70
2004-08-25	Jochen Tucht		More explicit error message
2004-10-17	Jochen Tucht		Leave decision whether to recurse into folders
								to enumerator (Mask.Recurse)
2004-11-03	Jochen Tucht		FIX [1048997] as proposed by Kimmo 2004-11-02
2005-01-15	Jochen Tucht		Read 7-Zip version from 7zip_pad.xml
								Set Merge7z UI language if DllBuild_Merge7z >= 9
2005-01-22	Jochen Tucht		Better explain what's present/missing/outdated
2005-02-05	Jochen Tucht		Fall back to IDD_MERGE7ZMISMATCH template from
								.exe if .lang file isn't up to date.
2005-02-26	Jochen Tucht		Add download link to error message
2005-02-26	Jochen Tucht		Use WinAPI to obtain ISO language/region codes
2005-02-27	Jochen Tucht		FIX [1152375]
2005-04-24	Kimmo Varis			Don't use DiffContext exported from DirView
2005-06-08	Kimmo Varis			Use DIFFITEM, not reference to it (hopefully only
								temporarily, to sort out new directory compare)
2005-06-22	Jochen Tucht		Change recommended version of 7-Zip to 4.20
								Remove noise from Nagbox
2005-07-03	Jochen Tucht		DIFFITEM has changed due to RFE [ 1205516 ]
2005-07-04	Jochen Tucht		New global ArchiveGuessFormat() checks for
								formats to be handled by external command line
								tools. These take precedence over Merge7z
								internal handlers.
2005-07-05	Jochen Tucht		Move to Merge7z::Format::GetDefaultName() to
								build intermediate filenames for multi-step
								compression.
2005-07-15	Jochen Tucht		Remove external command line tool integration
								for now. Rethink about it after 2.4 branch.
2005-08-20	Jochen Tucht		Option to guess archive format by signature
								Map extensions through ExternalArchiveFormat.ini
2005-08-23	Jochen Tucht		Option to entirely disable 7-Zip integration
2007-01-04	Kimmo Varis			Convert using COptionsMgr for options.
2007-06-16	Jochen Neubeck		FIX [1723263] "Zip --> Both" operation...
2007-12-22	Jochen Neubeck		Fix Merge7z UI lang for new translation system
								Change recommended version of 7-Zip to 4.57
2010-05-16	Jochen Neubeck		Read 7-Zip version from 7z.dll (which has long
								ago replaced the various format and codec DLLs)
								Change recommended version of 7-Zip to 4.65
*/


#include "stdafx.h"
#include "7zCommon.h"
#include <afxinet.h>
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "DirView.h"
#include "DirDoc.h"
#include "DirActions.h"
//#include "ExternalArchiveFormat.h"
#include "VersionInfo.h"
#include "paths.h"
#include "Environment.h"
#include "Merge7zFormatRegister.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @brief Proxy for Merge7z
 */
static __declspec(thread) Merge7z::Proxy m_Merge7z =
{
	{ 0, 0, DllBuild_Merge7z, },
	"Merge7z\\Merge7z.dll",
	"Merge7z",
	nullptr
};

std::vector<Merge7z::Format *(*)(const String& path)> Merge7zFormatRegister::optionalFormats;

/**
 * @brief assign BSTR to String, and return BSTR for optional SysFreeString()
 */
inline BSTR Assign(CString &dst, BSTR src)
{
	dst = src;
	return src;
}

bool IsArchiveFile(const String& pszFile)
{
	try {
		Merge7z::Format *piHandler = ArchiveGuessFormat(pszFile);
		if (piHandler != nullptr)
			return true;
		else
			return false;
	}
	catch (CException *e)
	{
		e->Delete();
		return false;
	}
}

/**
 * @brief Wrap Merge7z::GuessFormat() to allow for some customizing:
 * - Check if 7-Zip integration is enabled.
 * - Check for filename extension mappings.
 */
Merge7z::Format *ArchiveGuessFormat(const String& path)
{
	if (!GetOptionsMgr()->GetBool(OPT_ARCHIVE_ENABLE))
		return nullptr;
	if (paths::IsDirectory(path))
		return nullptr;
	String path2(path);
	// Map extensions through ExternalArchiveFormat.ini
	static tchar_t null[] = _T("");
	static const tchar_t section[] = _T("extensions");
	String entry = paths::FindExtension(path);
	tchar_t value[20];
	static const tchar_t* filename = nullptr;
	if (filename == nullptr)
	{
		tchar_t cPath[INTERNET_MAX_PATH_LENGTH];
		DWORD cchPath = SearchPath(nullptr, _T("ExternalArchiveFormat.ini"), nullptr,
			INTERNET_MAX_PATH_LENGTH, cPath, nullptr);
		filename = cchPath && cchPath < INTERNET_MAX_PATH_LENGTH ? tc::tcsdup(cPath) : null;
	}
	if (*filename &&
		GetPrivateProfileString(section, entry.c_str(), null, value, 20, filename) &&
		*value == '.')
	{
		// Remove end-of-line comments (in string returned from GetPrivateProfileString)
		// that is, remove semicolon & whatever follows it
		if (tchar_t* p = StrChr(value, ';'))
		{
			*p = '\0';
			StrTrim(value, _T(" \t"));
		}
		path2 = value;
	}

	// PATCH [ 1229867 ] RFE [ 1205516 ], RFE [ 887948 ], and other issues
	// command line integration portion is not yet applied
	// so following code not yet valid, so temporarily commented out
	// Look for command line tool first
	/*Merge7z::Format *pFormat;
	if (CExternalArchiveFormat::GuessFormat(path, pFormat))
	{
		return pFormat;
	}*/
	// Default to Merge7z*.dll

	try
	{
		Merge7z::Format* pFormat = nullptr;
		if (!paths::IsURL(path2))
			pFormat = m_Merge7z->GuessFormat(path2.c_str());
		if (pFormat == nullptr)
			pFormat = Merge7zFormatRegister::GuessFormat(path2);
		return pFormat;
	}
	catch (...)
	{
		Merge7z::Format *pFormat = Merge7zFormatRegister::GuessFormat(path2);
		if (pFormat != nullptr)
			return pFormat;
		throw;
	}
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
			m_Merge7z.operator->();
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
 * @brief Delete head of temp path context list, and return its parent context.
 */
CTempPathContext *CTempPathContext::DeleteHead()
{
	CTempPathContext *pParent = m_pParent;
	delete this;
	return pParent;
}

void CTempPathContext::Swap(int idx1, int idx2)
{
	std::swap(m_strDisplayRoot[idx1], m_strDisplayRoot[idx2]);
	std::swap(m_strRoot[idx1], m_strRoot[idx2]);
	if (m_pParent != nullptr)
		m_pParent->Swap(idx1, idx2);
}

/**
 * @brief Return installed or local version of 7-Zip.
 */
DWORD NTAPI VersionOf7z()
{
	String path = paths::ConcatPath(env::GetProgPath(), _T("Merge7z\\7z.dll"));
	unsigned versionMS = 0;
	unsigned versionLS = 0;
	CVersionInfo(path.c_str()).GetFixedFileVersion(versionMS, versionLS);
	return versionMS;
}

/**
 * @brief Access dll functions through proxy.
 */
interface Merge7z *Merge7z::Proxy::operator->()
{
	// As long as the Merge7z*.DLL has not yet been loaded, Merge7z
	// [0] points to the name of the DLL (with placeholders for 7-
	// Zip major and minor version numbers). Once the DLL has been
	// loaded successfully, Merge7z[0] is set to nullptr, causing the
	// if to fail on subsequent calls.

	if (const char *format = Merge7z[0])
	{
		// Merge7z has not yet been loaded

		if (!GetOptionsMgr()->GetBool(OPT_ARCHIVE_ENABLE))
			throw new CResourceException();
		if (DWORD ver = VersionOf7z())
		{
			Merge7z[0] = format;
			stub.Load();
		}
		else
		{
			throw new CResourceException();
		}
		LANGID wLangID = (LANGID)GetThreadLocale();
		DWORD flags = Initialize::Default | Initialize::Local7z | (wLangID << 16);
		if (GetOptionsMgr()->GetBool(OPT_ARCHIVE_PROBETYPE))
		{
			flags |= Initialize::GuessFormatBySignature | Initialize::GuessFormatByExtension;
		}
		if (Merge7z[1])
			((interface Merge7z *)Merge7z[1])->Initialize(flags);
	}
	return ((interface Merge7z *)Merge7z[1]);
}

/**
 * @brief Tell Merge7z we are going to enumerate just 1 item.
 */
UINT SingleItemEnumerator::Open()
{
	return 1;
}

/**
 * @brief Pass information about the item to Merge7z.
 */
Merge7z::Envelope *SingleItemEnumerator::Enum(Item &item)
{
	item.Mask.Item = item.Mask.Name|item.Mask.FullPath|item.Mask.Recurse;
	item.Name = Name;
	item.FullPath = FullPath;
	return 0;
}

/**
 * @brief SingleFileEnumerator constructor.
 */
SingleItemEnumerator::SingleItemEnumerator(const tchar_t* path, const tchar_t* FullPath, const tchar_t* Name)
: FullPath(FullPath)
, Name(Name)
{
}

/**
 * @brief Construct a DirItemEnumerator.
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
DirItemEnumerator::DirItemEnumerator(CDirView *pView, int nFlags)
: m_pView(pView)
, m_nFlags(nFlags)
{
	if (m_nFlags & Original)
	{
		m_rgFolderPrefix.push_back(_T("original"));
	}
	if (m_nFlags & Altered)
	{
		m_rgFolderPrefix.push_back(_T("altered"));
	}
	if (m_nFlags & BalanceFolders)
	{
		const CDiffContext& ctxt = pView->GetDiffContext();
		// Collect implied folders
		for (UINT i = Open() ; i-- ; )
		{
			const DIFFITEM &di = Next();
			if ((m_nFlags & DiffsOnly) && !IsItemNavigableDiff(ctxt, di))
			{
				continue;
			}
			// Enumerating items
			if (di.diffcode.exists(m_index))
			{
				// Item is present on right side, i.e. folder is implied
				m_rgImpliedFolders[m_index][di.diffFileInfo[m_index].path.get()] = PVOID(1);
			}
		}
	}
}

/**
 * @brief Initialize enumerator, return number of items to be enumerated.
 */
UINT DirItemEnumerator::Open()
{
	m_nIndex = -1;
	m_curFolderPrefix = m_rgFolderPrefix.begin();
	if (m_pView->GetDocument()->m_nDirs < 3)
		m_index = (m_nFlags & Right) != 0 ? 1 : 0;
	else
		m_index = ((m_nFlags & Right) != 0) ? 2 : ((m_nFlags & Middle) != 0 ? 1 : 0);
	size_t nrgFolderPrefix = m_rgFolderPrefix.size();
	if (nrgFolderPrefix)
	{
		m_strFolderPrefix = *m_curFolderPrefix++;
	}
	else
	{
		nrgFolderPrefix = 1;
	}
	return
	static_cast<UINT>((
		(m_nFlags & LVNI_SELECTED)
	?	pView(m_pView)->GetSelectedCount()
	:	pView(m_pView)->GetItemCount()
	) * nrgFolderPrefix);
}

/**
 * @brief Return next item.
 */
const DIFFITEM &DirItemEnumerator::Next()
{
	enum {nMask = LVNI_FOCUSED|LVNI_SELECTED|LVNI_CUT|LVNI_DROPHILITED};
	while ((m_nIndex = pView(m_pView)->GetNextItem(m_nIndex, m_nFlags & nMask)) == -1)
	{
		m_strFolderPrefix = *m_curFolderPrefix++;
		m_index++;
	}
	const auto& di = m_pView->GetDiffItem(m_nIndex);
	// If the current item is a folder, ignore the current item if the next selected item is a child element of that folder.
	if (m_index > (((di.diffcode.diffcode & DIFFCODE::THREEWAY) == 0) ? 1 : 2) || !di.diffcode.isDirectory())
		return di;
	const int nextIndex = pView(m_pView)->GetNextItem(m_nIndex, m_nFlags & nMask);
	if (nextIndex == -1)
		return di;
	const auto& diNext = m_pView->GetDiffItem(nextIndex);
	const String curRelPath = strutils::makelower(di.diffFileInfo[m_index].GetFile());
	if (strutils::makelower(diNext.diffFileInfo[m_index].GetFile()).find(curRelPath) != 0)
		return di;
	return *DIFFITEM::GetEmptyItem();
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
 * to return `nullptr`.
 */
Merge7z::Envelope *DirItemEnumerator::Enum(Item &item)
{
	const CDiffContext& ctxt = m_pView->GetDiffContext();
	const DIFFITEM &di = Next();

	if (di.isEmpty() || ((m_nFlags & DiffsOnly) && !IsItemNavigableDiff(ctxt, di)))
	{
		return 0;
	}

	bool isSideOnly = !di.diffcode.exists(m_index);

	Envelope *envelope = new Envelope;

	const String &sFilename = di.diffFileInfo[m_index].filename;
	const String &sSubdir = di.diffFileInfo[m_index].path;
	if (sSubdir.length())
		envelope->Name = paths::ConcatPath(sSubdir, sFilename);
	else
		envelope->Name = sFilename;
	envelope->FullPath = paths::ConcatPath(
			di.getFilepath(m_index, ctxt.GetNormalizedPath(m_index)),
			sFilename);

	UINT32 Recurse = item.Mask.Recurse;

	if (m_nFlags & BalanceFolders)
	{
		// Enumerating items on right side
		if (isSideOnly)
		{
			// Item is missing on right side
			PVOID &implied = m_rgImpliedFolders[m_index][di.diffFileInfo[1-m_index].path.get()];
			if (implied == nullptr)
			{
				// Folder is not implied by some other file, and has
				// not been enumerated so far, so enumerate it now!
				envelope->Name = di.diffFileInfo[1-m_index].path;
				envelope->FullPath = di.getFilepath(1-m_index, ctxt.GetNormalizedPath(1-m_index));
				implied = PVOID(2); // Don't enumerate same folder twice!
				isSideOnly = false;
				Recurse = 0;
			}
		}
	}

	if (isSideOnly)
	{
		return envelope;
	}

	if (m_strFolderPrefix.length())
	{
		if (envelope->Name.length())
			envelope->Name.insert(0, _T("\\"));
		envelope->Name.insert(0, m_strFolderPrefix);
	}

	item.Mask.Item = item.Mask.Name|item.Mask.FullPath|item.Mask.CheckIfPresent|Recurse;
	item.Name = envelope->Name.c_str();
	item.FullPath = envelope->FullPath.c_str();
	return envelope;
}

/**
 * @brief Apply appropriate handlers from left to right.
 */
bool DirItemEnumerator::MultiStepCompressArchive(const tchar_t* path)
{
	DeleteFile(path);
	Merge7z::Format *piHandler = ArchiveGuessFormat(path);
	if (piHandler != nullptr)
	{
		HWND hwndOwner = CWnd::GetSafeOwner()->GetSafeHwnd();
		CString pathIntermediate;
		SysFreeString(Assign(pathIntermediate, piHandler->GetDefaultName(hwndOwner, path)));
		String pathPrepend = path;
		pathPrepend.resize(pathPrepend.rfind('\\') + 1);
		pathIntermediate.Insert(0, pathPrepend.c_str());
		bool bDone = MultiStepCompressArchive(pathIntermediate);
		if (bDone)
		{
			SingleItemEnumerator tmpEnumerator(path, pathIntermediate);
			piHandler->CompressArchive(hwndOwner, path, &tmpEnumerator);
			DeleteFile(pathIntermediate);
		}
		else
		{
			piHandler->CompressArchive(hwndOwner, path, this);
		}
		return true;
	}
	return false;
}

/**
 * @brief Generate archive from DirView items.
 */
void DirItemEnumerator::CompressArchive(const tchar_t* path)
{
	String strPath;
	if (path == nullptr)
	{
		// 7z311 can only write 7z, zip, and tar(.gz|.bz2) archives, so don't
		// offer other formats here!
		static const tchar_t _T_Filter[]
		(
			_T("7z|*.7z|")
			//_T("z|*.z|")
			_T("zip|*.zip|")
			_T("jar (zip)|*.jar|")
			_T("ear (zip)|*.ear|")
			_T("war (zip)|*.war|")
			_T("xpi (zip)|*.xpi|")
			//_T("rar|*.rar|")
			_T("tar|*.tar|")
			_T("tar.z|*.tar.z|")
			_T("tar.gz|*.tar.gz|")
			_T("tar.bz2|*.tar.bz2|")
			//_T("tz|*.tz|")
			_T("tgz|*.tgz|")
			_T("tbz2|*.tbz2|")
			//_T("lzh|*.lzh|")
			//_T("cab|*.cab|")
			//_T("arj|*.arj|")
			//_T("deb|*.deb|")
			//_T("rpm|*.rpm|")
			//_T("cpio|*.cpio|")
			//_T("|")
		);
		String strFilter; // = CExternalArchiveFormat::GetOpenFileFilterString();
		strFilter.insert(0, _T_Filter);
		strFilter += _T("|");
 		CFileDialog dlg
		(
			FALSE,
			0,
			0,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN,
			strFilter.c_str()
		);
		dlg.m_ofn.nFilterIndex = GetOptionsMgr()->GetInt(OPT_ARCHIVE_FILTER_INDEX);
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
			strPath = dlg.GetPathName();
			path = strPath.c_str();
			GetOptionsMgr()->SaveOption(OPT_ARCHIVE_FILTER_INDEX, static_cast<int>(dlg.m_ofn.nFilterIndex));
		}
	}
	if (path && !MultiStepCompressArchive(path))
	{
		LangMessageBox(IDS_UNKNOWN_ARCHIVE_FORMAT, MB_ICONEXCLAMATION);
	}
}


DecompressResult DecompressArchive(HWND hWnd, const PathContext& files)
{
	DecompressResult res(files, nullptr, paths::IS_EXISTING_DIR);
	try
	{
		HRESULT hr;
		String path;
		// Handle archives using 7-zip
		Merge7z::Format *piHandler;
		piHandler = ArchiveGuessFormat(res.files[0]);
		if (piHandler  != nullptr)
		{
			res.pTempPathContext = new CTempPathContext;
			path = env::GetTempChildPath();
			for (int index = 0; index < res.files.GetSize(); index++)
				res.pTempPathContext->m_strDisplayRoot[index] = res.files[index];
			if (res.files.GetSize() == 2 && res.files[0] == res.files[1])
				res.files[1].erase();
			do
			{
				hr = piHandler->DeCompressArchive(hWnd, res.files[0].c_str(), path.c_str());
				if (FAILED(hr))
				{
					res.hr = hr;
					break;
				}
				if (res.files[0].find(path) == 0)
				{
					VERIFY(::DeleteFile(res.files[0].c_str()) || (LogErrorString(strutils::format(_T("DeleteFile(%s) failed"), res.files[0])), false));
				}
				BSTR pTmp = piHandler->GetDefaultName(hWnd, res.files[0].c_str());
				res.files[0] = ucr::toTString(pTmp);
				SysFreeString(pTmp);
				res.files[0].insert(0, _T("\\"));
				res.files[0].insert(0, path);
				piHandler = ArchiveGuessFormat(res.files[0]);
			} while (piHandler != nullptr);
			res.files[0] = path;
		}
		piHandler = res.files[1].empty() ? nullptr
										 : ArchiveGuessFormat(res.files[1]);
		if (piHandler != nullptr)
		{
			if (res.pTempPathContext == nullptr)
			{
				res.pTempPathContext = new CTempPathContext;
				for (int index = 0; index < res.files.GetSize(); index++)
					res.pTempPathContext->m_strDisplayRoot[index] = res.files[index];
			}
			path = env::GetTempChildPath();
			do
			{
				hr = piHandler->DeCompressArchive(hWnd, res.files[1].c_str(), path.c_str());
				if (FAILED(hr))
				{
					res.hr = hr;
					break;
				}
				if (res.files[1].find(path) == 0)
				{
					VERIFY(::DeleteFile(res.files[1].c_str()) || (LogErrorString(strutils::format(_T("DeleteFile(%s) failed"), res.files[1])), false));
				}
				BSTR pTmp = piHandler->GetDefaultName(hWnd, res.files[1].c_str());
				res.files[1] = OLE2T(pTmp);
				SysFreeString(pTmp);
				res.files[1].insert(0, _T("\\"));
				res.files[1].insert(0, path);
				piHandler = ArchiveGuessFormat(res.files[1]);
			} while (piHandler != nullptr);
			res.files[1] = path;
		}
		piHandler = (res.files.GetSize() <= 2) ? nullptr : ArchiveGuessFormat(res.files[2]);
		if (piHandler != nullptr)
		{
			if (res.pTempPathContext == nullptr)
			{
				res.pTempPathContext = new CTempPathContext;
				for (int index = 0; index < res.files.GetSize(); index++)
					res.pTempPathContext->m_strDisplayRoot[index] = res.files[index];
			}
			path = env::GetTempChildPath();
			do
			{
				hr = piHandler->DeCompressArchive(hWnd, res.files[2].c_str(), path.c_str());
				if (FAILED(hr))
				{
					res.hr = hr;
					break;
				}
				if (res.files[2].find(path) == 0)
				{
					VERIFY(::DeleteFile(res.files[2].c_str()) || (LogErrorString(strutils::format(_T("DeleteFile(%s) failed"), res.files[2])), false));
				}
				BSTR pTmp = piHandler->GetDefaultName(hWnd, res.files[2].c_str());
				res.files[2] = OLE2T(pTmp);
				SysFreeString(pTmp);
				res.files[2].insert(0, _T("\\"));
				res.files[2].insert(0, path);
				piHandler = ArchiveGuessFormat(res.files[2]);
			} while (piHandler != nullptr);
			res.files[2] = path;
		}
		if (res.files[1].empty())
		{
			// assume Perry style patch
			res.files[1] = path;
			res.files[0] += _T("\\ORIGINAL");
			res.files[1] += _T("\\ALTERED");
			if (!PathFileExists(res.files[0].c_str()) || !PathFileExists(res.files[1].c_str()))
			{
				// not a Perry style patch: diff with itself...
				res.files[0] = path;
				res.files[1] = std::move(path);
			}
			else
			{
				res.pTempPathContext->m_strDisplayRoot[0] += _T("\\ORIGINAL");
				res.pTempPathContext->m_strDisplayRoot[1] += _T("\\ALTERED");
			}
		}
	}
	catch (CException *e)
	{
		res.hr = E_FAIL;
		e->Delete();
	}
	return res;
}


