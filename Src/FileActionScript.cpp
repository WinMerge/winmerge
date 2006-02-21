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
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "OptionsDef.h"
#include "MainFrm.h"
#include "FileActionScript.h"
#include "CShellFileOp.h"

FileActionScript::FileActionScript()
: m_bUseRecycleBin(TRUE)
{
	m_bHasOperations[0] = FALSE;
	m_bHasOperations[1] = FALSE;
	m_bHasOperations[2] = FALSE;
	m_pOperations[0] = new CShellFileOp();
	m_pOperations[1] = new CShellFileOp();
	m_pOperations[2] = new CShellFileOp();
}

FileActionScript::~FileActionScript()
{
	delete m_pOperations[0];
	delete m_pOperations[1];
	delete m_pOperations[2];
}

/**
 * @brief Set parent window used for showing MessageBoxes.
 */
void FileActionScript::SetParentWindow(CWnd * pWnd)
{
	m_pParentWindow = pWnd;
}

/**
 * @brief Does user want to move deleted files to Recycle Bin?
 */
void FileActionScript::UseRecycleBin(BOOL bUseRecycleBin)
{
	m_bUseRecycleBin = bUseRecycleBin;
}

/**
 * @brief Return amount of actions (copy, move, etc)  in script.
 */
int FileActionScript::GetCount() const
{
	return actions.GetCount();
}

/**
 * @brief Checkout file from VSS before synching (copying) it.
 */
int FileActionScript::VCSCheckOut(const CString &path, BOOL &bApplyToAll)
{
	CMainFrame *pMf = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	CString strErr;
	int retVal = SCRIPT_SUCCESS;

	if (GetOptionsMgr()->GetInt(OPT_VCS_SYSTEM) == VCS_NONE)
		return retVal;

	// TODO: First param is not used!
	int nRetVal = GetMainFrame()->SyncFileToVCS(_T(""), path, bApplyToAll, &strErr);
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
 */
int FileActionScript::CreateOperationsScripts()
{
	UINT operation = 0;
	UINT operFlags = 0;
	BOOL bApplyToAll = FALSE;
	BOOL bContinue = TRUE;

	// Copy operations first
	operation = FO_COPY;
	operFlags |= FOF_NOCONFIRMMKDIR | FOF_MULTIDESTFILES | FOF_NOCONFIRMATION;
	if (m_bUseRecycleBin)
		operFlags |= FOF_ALLOWUNDO;

	POSITION pos = actions.GetHeadPosition();
	while (pos != NULL && bContinue == TRUE)
	{
		// Handle VCS checkout first
		BOOL bSkip = FALSE;
		const FileActionItem act = actions.GetNext(pos);
		if (act.atype == FileAction::ACT_COPY && !act.dirflag)
		{
			int retVal = VCSCheckOut(act.src, bApplyToAll);
			if (retVal == SCRIPT_USERCANCEL)
				bContinue = FALSE;
			else if (retVal == SCRIPT_USERSKIP)
				bSkip = TRUE;
			else if (retVal == SCRIPT_FAIL)
				bContinue = FALSE;
		}

		if (act.atype == FileAction::ACT_COPY &&
			bSkip == FALSE && bContinue == TRUE)
		{
			m_pOperations[0]->AddSourceFile(act.src);
			m_pOperations[0]->AddDestFile(act.dest);
			if (m_bHasOperations[0] == FALSE)
				m_bHasOperations[0] = TRUE;
		}
	}
	if (bContinue == FALSE)
	{
		m_bHasOperations[0] = FALSE;
		m_pOperations[0]->Reset();
		return SCRIPT_USERCANCEL;
	}
	
	if (m_bHasOperations[0] == TRUE)
		m_pOperations[0]->SetOperationFlags(operation, m_pParentWindow, operFlags);

	// Move operations next
	operation = FO_MOVE;
	operFlags = 0;
	if (m_bUseRecycleBin)
		operFlags |= FOF_ALLOWUNDO;

	pos = actions.GetHeadPosition();
	while (pos != NULL)
	{
		const FileActionItem act = actions.GetNext(pos);
		if (act.atype == FileAction::ACT_MOVE)
		{
			m_pOperations[1]->AddSourceFile(act.src);
			m_pOperations[1]->AddDestFile(act.dest);
			if (m_bHasOperations[1] == FALSE)
				m_bHasOperations[1] = TRUE;
		}
	}
	if (m_bHasOperations[1] == TRUE)
		m_pOperations[1]->SetOperationFlags(operation, m_pParentWindow, operFlags);

	// Delete operations last
	operation = FO_DELETE;
	operFlags = 0;
	if (m_bUseRecycleBin)
		operFlags |= FOF_ALLOWUNDO;

	pos = actions.GetHeadPosition();
	while (pos != NULL)
	{
		const FileActionItem act = actions.GetNext(pos);
		if (act.atype == FileAction::ACT_DEL)
		{
			m_pOperations[2]->AddSourceFile(act.src);
			if (!act.dest.IsEmpty())
				m_pOperations[2]->AddSourceFile(act.dest);
			if (m_bHasOperations[2] == FALSE)
				m_bHasOperations[2] = TRUE;
		}
	}
	if (m_bHasOperations[2] == TRUE)
		m_pOperations[2]->SetOperationFlags(operation, m_pParentWindow, operFlags);
	return SCRIPT_SUCCESS;
}

/**
 * @brief Execute fileoperations.
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
		if (m_bHasOperations[0] == TRUE)
			bFileOpSucceed = m_pOperations[0]->Go(&bOpStarted,
					&apiRetVal, &bUserCancelled);

		if (m_bHasOperations[1] == TRUE)
		{
			if (bFileOpSucceed && !bUserCancelled)
				bFileOpSucceed = m_pOperations[1]->Go(&bOpStarted, &apiRetVal, &bUserCancelled);
			else
				bRetVal = FALSE;
		}

		if (m_bHasOperations[2] == TRUE)
		{
			if (bFileOpSucceed && !bUserCancelled)
				bFileOpSucceed = m_pOperations[2]->Go(&bOpStarted, &apiRetVal, &bUserCancelled);
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
