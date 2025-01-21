// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file  FileActionScript.cpp
 *
 * @brief Implementation of FileActionScript and related classes
 */

#include "stdafx.h"
#include "FileActionScript.h"
#include <vector>
#include "UnicodeString.h"
#include "Merge.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "ShellFileOperations.h"
#include "paths.h"

using std::vector;

/**
 * @brief Standard constructor.
 */
FileActionScript::FileActionScript()
: m_bUseRecycleBin(true)
, m_bHasCopyOperations(false)
, m_bHasMoveOperations(false)
, m_bHasRenameOperations(false)
, m_bHasDelOperations(false)
, m_hParentWindow(nullptr)
, m_pCopyOperations(new ShellFileOperations())
, m_pMoveOperations(new ShellFileOperations())
, m_pRenameOperations(new ShellFileOperations())
, m_pDelOperations(new ShellFileOperations())
, m_bCanceled(false)
{
}

/**
 * @brief Standard destructor.
 */
FileActionScript::~FileActionScript() = default;

/**
 * @brief Remove last action item from the list.
 * @return Item removed from the list.
 */
FileActionItem FileActionScript::RemoveTailActionItem()
{
	FileActionItem item = m_actions.back();
	m_actions.pop_back();
	return item;
}

/**
 * @brief Removes duplicate entries from a vector of FileActionItem objects.
 */
void FileActionScript::RemoveDuplicates()
{
	auto compare = [](const FileActionItem& a, const FileActionItem& b) {
		return std::tie(a.src, a.dest, a.atype) < std::tie(b.src, b.dest, b.atype);
		};
	
	auto equal = [](const FileActionItem& a, const FileActionItem& b) {
		return std::tie(a.src, a.dest, a.atype) == std::tie(b.src, b.dest, b.atype);
		};
	
	std::sort(m_actions.begin(), m_actions.end(), compare);
	
	m_actions.erase(std::unique(m_actions.begin(), m_actions.end(), equal), m_actions.end());
}

/**
 * @brief Create ShellFileOperations operation lists from our scripts.
 *
 * We use ShellFileOperations internally to do actual file operations.
 * ShellFileOperations can do only one type of operation (copy, move, delete)
 * with one instance at a time, so we use own instance for every
 * type of action.
 * @return One of CreateScriptReturn values.
 */
int FileActionScript::CreateOperationsScripts()
{
	UINT operation = 0;
	FILEOP_FLAGS operFlags = 0;
	bool bContinue = true;

	// Copy operations first
	operation = FO_COPY;
	operFlags |= FOF_NOCONFIRMMKDIR | FOF_MULTIDESTFILES | FOF_NOCONFIRMATION;
	if (m_bUseRecycleBin)
		operFlags |= FOF_ALLOWUNDO;

	vector<FileActionItem>::const_iterator iter = m_actions.begin();
	while (iter != m_actions.end() && bContinue)
	{
		bool bSkip = false;
		if ((*iter).atype == FileAction::ACT_COPY && !(*iter).dirflag)
		{
			if (!CMergeApp::CreateBackup(true, (*iter).dest))
			{
				String strErr = _("Error backing up file");
				AfxMessageBox(strErr.c_str(), MB_OK | MB_ICONERROR);
				bContinue = false;
			}
		}

		if ((*iter).atype == FileAction::ACT_COPY &&
			!bSkip && bContinue)
		{
			m_pCopyOperations->AddSourceAndDestination((*iter).src, (*iter).dest);
			m_bHasCopyOperations = true;
		}
		++iter;
	}
	if (!bContinue)
	{
		m_bHasCopyOperations = false;
		m_pCopyOperations->Reset();
		return SCRIPT_USERCANCEL;
	}
	
	if (m_bHasCopyOperations)
		m_pCopyOperations->SetOperation(operation, operFlags, m_hParentWindow);

	// Move operations next
	operation = FO_MOVE;
	operFlags = FOF_MULTIDESTFILES;
	if (m_bUseRecycleBin)
		operFlags |= FOF_ALLOWUNDO;

	iter = m_actions.begin();
	while (iter != m_actions.end())
	{
		if ((*iter).atype == FileAction::ACT_MOVE)
		{
			m_pMoveOperations->AddSourceAndDestination((*iter).src, (*iter).dest);
			m_bHasMoveOperations = true;
		}
		++iter;
	}
	if (m_bHasMoveOperations)
		m_pMoveOperations->SetOperation(operation, operFlags,  m_hParentWindow);

	// Rename operations nextbbbb
	operation = FO_RENAME;
	operFlags = FOF_MULTIDESTFILES;
	if (m_bUseRecycleBin)
		operFlags |= FOF_ALLOWUNDO;

	iter = m_actions.begin();
	while (iter != m_actions.end())
	{
		if ((*iter).atype == FileAction::ACT_RENAME)
		{
			m_pRenameOperations->AddSourceAndDestination((*iter).src, (*iter).dest);
			m_bHasRenameOperations = true;
		}
		++iter;
	}
	if (m_bHasRenameOperations)
		m_pRenameOperations->SetOperation(operation, operFlags, m_hParentWindow);

	// Delete operations last
	operation = FO_DELETE;
	operFlags = 0;
	if (m_bUseRecycleBin)
		operFlags |= FOF_ALLOWUNDO;

	iter = m_actions.begin();
	while (iter != m_actions.end())
	{
		if ((*iter).atype == FileAction::ACT_DEL)
		{
			m_pDelOperations->AddSource((*iter).src);
			if (!(*iter).dest.empty())
				m_pDelOperations->AddSource((*iter).dest);
			m_bHasDelOperations = true;
		}
		++iter;
	}
	if (m_bHasDelOperations)
		m_pDelOperations->SetOperation(operation, operFlags, m_hParentWindow);
	return SCRIPT_SUCCESS;
}

/**
 * @brief Run one operation set.
 * @param [in] oplist List of operations to run.
 * @param [out] userCancelled Did user cancel the operation?
 * @return true if the operation succeeded and finished.
 */
bool FileActionScript::RunOp(ShellFileOperations *oplist, bool & userCancelled)
{
	bool fileOpSucceed = false;
	__try
	{
		fileOpSucceed = oplist->Run();
		userCancelled = oplist->IsCanceled();
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		fileOpSucceed = false;
	}
	return fileOpSucceed;
}

/**
 * @brief Execute fileoperations.
 * @return `true` if all actions were done successfully, `false` otherwise.
 */
bool FileActionScript::Run()
{
	// Now process files/directories that got added to list
	bool bFileOpSucceed = true;
	bool bUserCancelled = false;
	bool bRetVal = true;

	CreateOperationsScripts();

	if (m_bHasCopyOperations)
	{
		vector<FileActionItem>::const_iterator iter = m_actions.begin();
		while (iter != m_actions.end())
		{
			if ((*iter).dirflag)
				paths::CreateIfNeeded((*iter).dest);
			++iter;
		}
		bFileOpSucceed = RunOp(m_pCopyOperations.get(), bUserCancelled);
	}

	if (m_bHasMoveOperations)
	{
		if (bFileOpSucceed && !bUserCancelled)
		{
			bFileOpSucceed = RunOp(m_pMoveOperations.get(), bUserCancelled);
		}
		else
			bRetVal = false;
	}

	if (m_bHasRenameOperations)
	{
		if (bFileOpSucceed && !bUserCancelled)
		{
			bFileOpSucceed = RunOp(m_pRenameOperations.get(), bUserCancelled);
		}
		else
			bRetVal = false;
	}

	if (m_bHasDelOperations)
	{
		if (bFileOpSucceed && !bUserCancelled)
		{
			bFileOpSucceed = RunOp(m_pDelOperations.get(), bUserCancelled);
		}
		else
			bRetVal = false;
	}

	if (!bFileOpSucceed || bUserCancelled)
		bRetVal = false;

	m_bCanceled = bUserCancelled;

	return bRetVal;
}
