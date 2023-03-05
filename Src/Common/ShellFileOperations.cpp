// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file  ShellFileOperations.cpp
 *
 * @brief Implementation of ShellFileOperations class.
 */

#include "pch.h"
#include "ShellFileOperations.h"
#include <windows.h>
#include <vector>
#include <shellAPI.h>
#pragma warning (push)			// prevent "warning C4091: 'typedef ': ignored on left of 'tagGPFIDL_FLAGS' when no variable is declared"
#pragma warning (disable:4091)	// VC bug when using XP enabled toolsets.
#include <shlobj.h>
#pragma warning (pop)
#include <shlobj.h>
#include <comip.h>
#include "UnicodeString.h"
#include "paths.h"
#include "TFile.h"

using std::vector;
typedef _com_ptr_t<_com_IIID<IFileOperation, &__uuidof(IFileOperation)>> IFileOperationPtr;
typedef _com_ptr_t<_com_IIID<IShellItem, &__uuidof(IShellItem)>> IShellItemPtr;

/**
 * @brief Constructor.
 */
ShellFileOperations::ShellFileOperations()
 : m_bOneToOneMapping(false)
 , m_function(0)
 , m_flags(0)
 , m_parentWindow(nullptr)
 , m_isCanceled(false)
{
}

/**
 * @brief Add source- and destination paths.
 * @param [in] source Source path.
 * @param [in] destination Destination path.
 */
void ShellFileOperations::AddSourceAndDestination(const String &source,
		const String &destination)
{
	m_sources.push_back(source);
	m_destinations.push_back(destination);
}

/**
 * @brief Add source path.
 * @param [in] source Source path.
 */
void ShellFileOperations::AddSource(const String &source)
{
	m_sources.push_back(source);
	m_bOneToOneMapping = false;
}

/**
 * @brief Add destination path.
 * @param [in] destination Destination path.
 */
void ShellFileOperations::SetDestination(const String &destination)
{
	m_destinations.push_back(destination);
	m_bOneToOneMapping = false;
}

/**
 * @brief Get a path list as C string with NULLs.
 * This function returns C-string with NULLs between paths and two NULLs
 * at the end of the paths. As the ShellFileOp() requires.
 * @param [in] source If true, return source paths, else return destination
 *   paths.
 * @param [out] string of the paths.
 */
vector<tchar_t> ShellFileOperations::GetPathList(bool source) const
{
	vector<tchar_t> paths;
	const size_t len = CountStringSize(source);
	paths.resize(len, 0);

	vector<String>::const_iterator iter;
	vector<String>::const_iterator end;
	if (source)
	{
		iter = m_sources.begin();
		end = m_sources.end();
	}
	else
	{
		iter = m_destinations.begin();
		end = m_destinations.end();
	}

	size_t ind = 0;
	while (iter != end)
	{
		const size_t slen = (*iter).length();
		memcpy(&paths[ind], (*iter).c_str(), slen * sizeof(tchar_t));
		ind += slen;
		ind++; // NUL between strings
		++iter;
	}
	return paths;
}

/**
 * @brief Calculate length of the C-string required for paths.
 * @param [in] source If true calculate source paths, else calculate
 *   destination paths.
 * @return Length of the string.
 */
size_t ShellFileOperations::CountStringSize(bool source) const
{
	vector<String>::const_iterator iter;
	vector<String>::const_iterator end;
	if (source)
	{
		iter = m_sources.begin();
		end = m_sources.end();
	}
	else
	{
		iter = m_destinations.begin();
		end = m_destinations.end();
	}

	size_t size = 0;
	while (iter != end)
	{
		size += (*iter).length() * sizeof(tchar_t);
		size += sizeof(tchar_t); // NUL between strings
		++iter;
	}
	size += 2; // Two zeros at end of the string
	return size;
}

/**
 * @brief Set the operation, flags and parent window.
 * @param [in] operation Operation to run (copy/move/delete/rename).
 * @param [in] flags Flags related to the operation.
 * @param [in] parentWindow Window getting notifications.
 */
void ShellFileOperations::SetOperation(UINT operation, FILEOP_FLAGS flags,
		HWND parentWindow /*= nullptr*/)
{
	m_function = operation;
	m_flags = flags;
	m_parentWindow = parentWindow;
}

/**
 * @brief Run the file operation(s).
 * This method runs the filesystem operation(s) added earlier.
 * @return true if succeeds and user did not cancel, false otherwise.
 */
bool ShellFileOperations::Run()
{
	if (m_function == 0)
		return false; // Operation not set!

	HRESULT hr;
	IFileOperationPtr pFileOperation;
	if (FAILED(hr = pFileOperation.CreateInstance(CLSID_FileOperation, nullptr, CLSCTX_ALL)))
	{
		vector<tchar_t> destStr;
		vector<tchar_t> sourceStr = GetPathList(true);
		if (m_function != FO_DELETE)
			destStr = GetPathList(false);

		SHFILEOPSTRUCT fileop = {m_parentWindow, m_function, &sourceStr[0],
			m_function != FO_DELETE ? &destStr[0] : nullptr, m_flags, FALSE, 0, 0};
		int ret = SHFileOperation(&fileop);

		if (ret == 0x75 || fileop.fAnyOperationsAborted) // DE_OPCANCELLED
			m_isCanceled = true;

		bool anyAborted = !!fileop.fAnyOperationsAborted;

		// SHFileOperation returns 0 when succeeds
		if (ret == 0 && !anyAborted)
			return true;
		return false;
	}

	auto CreateShellItemParseDisplayName = [](const String& path, IShellItem **psi)
	{
		HRESULT hr;
		PIDLIST_ABSOLUTE pidl;
		if (FAILED(hr = SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, nullptr)))
		{
			tchar_t szShortPath[32768] = {};
			if (GetShortPathName(TFile(path).wpath().c_str(), szShortPath, sizeof(szShortPath) / sizeof(szShortPath[0])) == 0)
			{
				hr = E_FAIL;
			}
			else
			{
				String shortPath = szShortPath;
				strutils::replace(shortPath, _T("\\\\?\\UNC\\"), _T("\\\\"));
				strutils::replace(shortPath, _T("\\\\?\\"), _T(""));
				hr = SHParseDisplayName(shortPath.c_str(), nullptr, &pidl, 0, nullptr);
			}
		}
		if (SUCCEEDED(hr))
		{
			hr = SHCreateShellItem(nullptr, nullptr, pidl, psi);
			ILFree(pidl);
		}
		return hr;
	};

	pFileOperation->SetOperationFlags(m_flags & ~FOF_MULTIDESTFILES);

	auto itsrc = m_sources.begin();
	auto itdst = m_destinations.begin();

	while (itsrc != m_sources.end() || itdst != m_destinations.end())
	{
		IShellItemPtr pShellItemSrc;
		IShellItemPtr pShellItemDst;
		String dstFileName;
		if (itsrc != m_sources.end())
		{
			if (FAILED(CreateShellItemParseDisplayName(*itsrc, &pShellItemSrc)))
				return false;
			++itsrc;
		}
		if (itdst != m_destinations.end())
		{
			String parent = paths::GetParentPath(*itdst);
			if (FAILED(CreateShellItemParseDisplayName(parent, &pShellItemDst)))
			{
				TFile(parent).createDirectories();
				if (FAILED(CreateShellItemParseDisplayName(parent, &pShellItemDst)))
					return false;
			}
			dstFileName = paths::FindFileName(*itdst);
			++itdst;
		}
		switch (m_function)
		{
		case FO_COPY:
			hr = pFileOperation->CopyItem(pShellItemSrc, pShellItemDst, dstFileName.c_str(), nullptr);
			break;
		case FO_MOVE:
			hr = pFileOperation->MoveItem(pShellItemSrc, pShellItemDst, dstFileName.c_str(), nullptr);
			break;
		case FO_DELETE:
			hr = pFileOperation->DeleteItem(pShellItemSrc, nullptr);
			break;
		case FO_RENAME:
			hr = pFileOperation->RenameItem(pShellItemSrc, dstFileName.c_str(), nullptr);
			break;
		}
		if (FAILED(hr))
			return false;
	}
	hr = pFileOperation->PerformOperations();
	BOOL fAnyOperationsAborted = FALSE;
	pFileOperation->GetAnyOperationsAborted(&fAnyOperationsAborted);
	m_isCanceled = fAnyOperationsAborted;
	return SUCCEEDED(hr) && !fAnyOperationsAborted;
}

/**
 * @brief Reset the class.
 */
void ShellFileOperations::Reset()
{
	m_bOneToOneMapping = false;
	m_function = 0;
	m_flags = 0;
	m_parentWindow = nullptr;
	m_isCanceled = false;

	m_sources.clear();
	m_destinations.clear();
}
