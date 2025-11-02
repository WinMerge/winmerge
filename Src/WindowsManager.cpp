/**
 * @brief Implementation of CWindowsManager class.
 */
#include "StdAfx.h"
#include "WindowsManager.h"
#include "WindowsManagerDialog.h"

void CWindowsManager::AddChildFrame(CMDIChildWnd* pChildWnd)
{
	for (int i = 0; i < m_arrChild.GetSize(); ++i)
	{
		if (pChildWnd == m_arrChild.GetAt(i))
			return;
	}
	m_arrChild.InsertAt(0, pChildWnd);
}

void CWindowsManager::RemoveChildFrame(CMDIChildWnd* pChildWnd)
{
	for (int i = 0; i < m_arrChild.GetSize(); ++i)
	{
		if (pChildWnd == m_arrChild.GetAt(i))
		{
			m_arrChild.RemoveAt(i);
			break;
		}
	}
}

void CWindowsManager::ActivateChildFrame(CMDIChildWnd* pChildWnd)
{
	for (int i = 0; i < m_arrChild.GetSize(); ++i)
	{
		if (pChildWnd == m_arrChild.GetAt(i))
		{
			CMDIChildWnd* pMDIChild = m_arrChild.GetAt(i);
			if (pMDIChild && pMDIChild->IsIconic())
				pMDIChild->ShowWindow(SW_RESTORE);
			((CMDIFrameWnd*)AfxGetMainWnd())->MDIActivate(pMDIChild);
			break;
		}
	}
}

void CWindowsManager::ChildFrameActivated(CMDIChildWnd* pChildWnd)
{
	for (int i = 0; i < m_arrChild.GetSize(); ++i)
	{
		if (pChildWnd == m_arrChild.GetAt(i))
		{
			m_arrChild.RemoveAt(i);
			m_arrChild.InsertAt(0, pChildWnd);
			break;
		}
	}
}

void CWindowsManager::ShowDialog(CMDIFrameWnd* pFrame)
{
	CWindowsManagerDialog* pDlg = new CWindowsManagerDialog(*this, pFrame);
	pDlg->Create(CWindowsManagerDialog::IDD, pFrame);
	pDlg->ShowWindow(SW_SHOW);
}
