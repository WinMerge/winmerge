/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  FileActionScript.cpp
 *
 * @brief Implementation of FileActionScript and related classes
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "Merge.h"
#include "OptionsDef.h"
#include "MainFrm.h"
#include "FileActionScript.h"
#include "CShellFileOp.h"
#include "paths.h"

/**
 * @brief Standard constructor.
 */
FileActionScript::FileActionScript()
: m_bUseRecycleBin(TRUE)
, m_bHasCopyOperations(FALSE)
, m_bHasMoveOperations(FALSE)
, m_bHasDelOperations(FALSE)
, m_hParentWindow(NULL)
{
	m_pCopyOperations = new CShellFileOp();
	m_pMoveOperations = new CShellFileOp();
	m_pDelOperations = new CShellFileOp();
}

/**
 * @brief Standard destructor.
 */
FileActionScript::~FileActionScript()
{
	delete m_pCopyOperations;
	delete m_pMoveOperations;
	delete m_pDelOperations;
}

/**
 * @brief Set parent window used for showing MessageBoxes.
 * @param [in] hnd Handle to parent window.
 */
void FileActionScript::SetParentWindow(HWND hWnd)
{
	m_hParentWindow = hWnd;
}

/**
 * @brief Does user want to move deleted files to Recycle Bin?
 * @param [in] bUseRecycleBin If TRUE deleted files are moved to Recycle Bin.
 */
void FileActionScript::UseRecycleBin(BOOL bUseRecycleBin)
{
	m_bUseRecycleBin = bUseRecycleBin;
}

/**
 * @brief Return amount of actions (copy, move, etc)  in script.
 * @return Amount of actions.
 */
int FileActionScript::GetActionItemCount() const
{
	return m_actions.GetCount();
}

/**
 * @brief Checkout file from VSS before synching (copying) it.
 * @param [in] path Full path to a file.
 * @param [in,out] bApplyToAll Apply user selection to all (selected)files?
 * @return One of CreateScriptReturn values.
 */
int FileActionScript::VCSCheckOut(const CString &path, BOOL &bApplyToAll)
{
	CString strErr;
	int retVal = SCRIPT_SUCCESS;

	if (GetOptionsMgr()->GetInt(OPT_VCS_SYSTEM) == VCS_NONE)
		return retVal;

	// TODO: First param is not used!
	int nRetVal = GetMainFrame()->SyncFileToVCS(path, bApplyToAll, &strErr);
	if (nRetVal == -1)
	{
		retVal = SCRIPT_FAIL; // So we exit without file operations done
		AfxMessageBox(strErr, MB_OK | MB_ICONERROR);
	}
	else if (nRetVal == IDCANCEL)
	{
		retVal = SCRIPT_USERCANCEL; // User canceled, so we don't continue
	}
	else if (nRetVal == IDNO)
	{
		retVal = SCRIPT_USERSKIP;  // User wants to skip this item
	}

	return retVal;
}

/**
 * @brief Create CShellFileOp operation lists from our scripts.
 *
 * We use CShellFileOp internally to do actual file operations.
 * CShellFileOp can do only one type of operation (copy, move, delete)
 * with one instance at a time, so we use own instance for every
 * type of action.
 * @return One of CreateScriptReturn values.
 */
int FileActionScript::CreateOperationsScripts()
{
	UINT operation = 0;
	FILEOP_FLAGS operFlags = 0;
	BOOL bApplyToAll = FALSE;
	BOOL bContinue = TRUE;

	// Copy operations first
	operation = FO_COPY;
	operFlags |= FOF_NOCONFIRMMKDIR | FOF_MULTIDESTFILES | FOF_NOCONFIRMATION;
	if (m_bUseRecycleBin)
		operFlags |= FOF_ALLOWUNDO;

	POSITION pos = m_actions.GetHeadPosition();
	while (pos != NULL && bContinue == TRUE)
	{
		BOOL bSkip = FALSE;
		const FileActionItem act = m_actions.GetNext(pos);
		if (act.atype == FileAction::ACT_COPY && !act.dirflag)
		{
			// Handle VCS checkout
			// Before we can write over destination file, we must unlock
			// (checkout) it. This also notifies VCS system that the file
			// has been modified.
			if (GetOptionsMgr()->GetInt(OPT_VCS_SYSTEM) != VCS_NONE)
			{
				int retVal = VCSCheckOut(act.dest.c_str(), bApplyToAll);
				if (retVal == SCRIPT_USERCANCEL)
					bContinue = FALSE;
				else if (retVal == SCRIPT_USERSKIP)
					bSkip = TRUE;
				else if (retVal == SCRIPT_FAIL)
					bContinue = FALSE;
			}

			if (bContinue)
			{
				if (!GetMainFrame()->CreateBackup(TRUE, act.dest.c_str()))
				{
					String strErr = theApp.LoadString(IDS_ERROR_BACKUP);
					AfxMessageBox(strErr.c_str(), MB_OK | MB_ICONERROR);
					bContinue = FALSE;
				}
			}
		}

		if (act.atype == FileAction::ACT_COPY &&
			bSkip == FALSE && bContinue == TRUE)
		{
			m_pCopyOperations->AddSourceFile(act.src.c_str());
			m_pCopyOperations->AddDestFile(act.dest.c_str());
			m_bHasCopyOperations = TRUE;
		}
	}
	if (bContinue == FALSE)
	{
		m_bHasCopyOperations = FALSE;
		m_pCopyOperations->Reset();
		return SCRIPT_USERCANCEL;
	}
	
	if (m_bHasCopyOperations)
		m_pCopyOperations->SetOperationFlags(operation, m_hParentWindow, operFlags);

	// Move operations next
	operation = FO_MOVE;
	operFlags = FOF_MULTIDESTFILES;
	if (m_bUseRecycleBin)
		operFlags |= FOF_ALLOWUNDO;

	pos = m_actions.GetHeadPosition();
	while (pos != NULL)
	{
		const FileActionItem act = m_actions.GetNext(pos);
		if (act.atype == FileAction::ACT_MOVE)
		{
			m_pMoveOperations->AddSourceFile(act.src.c_str());
			m_pMoveOperations->AddDestFile(act.dest.c_str());
			m_bHasMoveOperations = TRUE;
		}
	}
	if (m_bHasMoveOperations)
		m_pMoveOperations->SetOperationFlags(operation, m_hParentWindow, operFlags);

	// Delete operations last
	operation = FO_DELETE;
	operFlags = 0;
	if (m_bUseRecycleBin)
		operFlags |= FOF_ALLOWUNDO;

	pos = m_actions.GetHeadPosition();
	while (pos != NULL)
	{
		const FileActionItem act = m_actions.GetNext(pos);
		if (act.atype == FileAction::ACT_DEL)
		{
			m_pDelOperations->AddSourceFile(act.src.c_str());
			if (!act.dest.empty())
				m_pDelOperations->AddSourceFile(act.dest.c_str());
			m_bHasDelOperations = TRUE;
		}
	}
	if (m_bHasDelOperations)
		m_pDelOperations->SetOperationFlags(operation, m_hParentWindow, operFlags);
	return SCRIPT_SUCCESS;
}

/**
 * @brief Execute fileoperations.
 * @return TRUE if all actions were done successfully, FALSE otherwise.
 */
BOOL FileActionScript::Run()
{
	// Now process files/directories that got added to list
	BOOL bOpStarted = FALSE;
	BOOL bFileOpSucceed = TRUE;
	BOOL bUserCancelled = FALSE;
	BOOL bRetVal = TRUE;
	int apiRetVal = 0;

	CreateOperationsScripts();

	__try
	{
		if (m_bHasCopyOperations)
		{
			POSITION pos = m_actions.GetHeadPosition();
			while (pos != NULL)
			{
				const FileActionItem &act = m_actions.GetNext(pos);
				if (act.dirflag)
					paths_CreateIfNeeded(act.dest.c_str());
			}

			bFileOpSucceed = m_pCopyOperations->Go(&bOpStarted,
					&apiRetVal, &bUserCancelled);
		}

		if (m_bHasMoveOperations)
		{
			if (bFileOpSucceed && !bUserCancelled)
				bFileOpSucceed = m_pMoveOperations->Go(&bOpStarted, &apiRetVal,
						 &bUserCancelled);
			else
				bRetVal = FALSE;
		}

		if (m_bHasDelOperations)
		{
			if (bFileOpSucceed && !bUserCancelled)
				bFileOpSucceed = m_pDelOperations->Go(&bOpStarted, &apiRetVal, &bUserCancelled);
			else
				bRetVal = FALSE;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		bFileOpSucceed = FALSE;
		bRetVal = FALSE;
	}

	if (!bFileOpSucceed || bUserCancelled)
		bRetVal = FALSE;

	return bRetVal;
}
