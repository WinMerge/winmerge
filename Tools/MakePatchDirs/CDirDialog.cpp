/*!
  \file    CDirDialog.cpp
  \author  Perry Rapp, Smartronix, Creator, 1999-2001
  \date    Created: 1999
  \date    Edited:  2001/12/12 PR

  \brief   Implementation of CDirDialog
*/
/* The MIT License
Copyright (c) 2001 Perry Rapp
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#include "stdafx.h"
#include "CDirDialog.h"
#include "shlobj.h"

#ifndef __ATLCONV_H__
// for T2OLE
#include <atlconv.h>
#endif

namespace prdlg {

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Callback function called by SHBrowseForFolder's browse control
// after initialization and when selection changes
int __stdcall CDirDialog::BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	CDirDialog* pDirDialogObj = (CDirDialog*)lpData;
	if (uMsg == BFFM_INITIALIZED )
	{
		if( ! pDirDialogObj->m_strSelDir.IsEmpty() )
			::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCTSTR)(pDirDialogObj->m_strSelDir));
		if( ! pDirDialogObj->m_strWindowTitle.IsEmpty() )
			::SetWindowText(hwnd, (LPCTSTR) pDirDialogObj->m_strWindowTitle);
	}
	else if( uMsg == BFFM_SELCHANGED )
	{
		LPITEMIDLIST pidl = (LPITEMIDLIST) lParam;
		TCHAR selection[MAX_PATH];
		if( ! ::SHGetPathFromIDList(pidl, selection) )
			selection[0] = '\0';

		CString csStatusText;
		BOOL bOk = pDirDialogObj->SelChanged(selection, csStatusText);

		if( pDirDialogObj->m_ulFlags & BIF_STATUSTEXT)
			::SendMessage(hwnd, BFFM_SETSTATUSTEXT , 0, (LPARAM)(LPCTSTR)csStatusText);

		::SendMessage(hwnd, BFFM_ENABLEOK, 0, bOk);
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirDialog::CDirDialog()
{
	m_ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
}

CDirDialog::~CDirDialog()
{
}

BOOL CDirDialog::DoBrowse(CWnd *pwndParent)
{
	HWND hwndParent = pwndParent == NULL ? NULL : pwndParent->GetSafeHwnd();
	return DoBrowse(hwndParent);
}

BOOL CDirDialog::DoBrowse(HWND hwndParent)
{

	if( ! m_strSelDir.IsEmpty() )
	{
		m_strSelDir.TrimRight();
		if( m_strSelDir.Right(1) == "\\" || m_strSelDir.Right(1) == "//" )
			m_strSelDir = m_strSelDir.Left(m_strSelDir.GetLength() - 1);
	}

	LPMALLOC pMalloc;
	if (SHGetMalloc (&pMalloc)!= NOERROR)
		return FALSE;

	BROWSEINFO bInfo;
	LPITEMIDLIST pidl;
	ZeroMemory ( (PVOID) &bInfo,sizeof (BROWSEINFO));

	if (!m_strInitDir.IsEmpty ())
	{
		OLECHAR       olePath[MAX_PATH];
		ULONG         chEaten;
		ULONG         dwAttributes;
		HRESULT       hr;
		LPSHELLFOLDER pDesktopFolder;
		//
		// Get a pointer to the Desktop's IShellFolder interface.
		//
		if (SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
		{
			USES_CONVERSION;

			LPCWSTR wpath = T2COLE(m_strInitDir);
			wcsncpy(olePath, wpath, sizeof(olePath)/sizeof(olePath[0]));
			olePath[sizeof(olePath)/sizeof(olePath[0])-1]=0;
			//
			// IShellFolder::ParseDisplayName requires the file name be in Unicode.
			//
//			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_strInitDir.GetBuffer(MAX_PATH), -1,
//			                    olePath, MAX_PATH);

			m_strInitDir.ReleaseBuffer (-1);
			//
			// Convert the path to an ITEMIDLIST.
			//
			hr = pDesktopFolder->ParseDisplayName(
				NULL
				,NULL
				,olePath
				,&chEaten
				,&pidl
				,&dwAttributes
			);
			if (FAILED(hr))
			{
				pMalloc ->Free (pidl);
				pMalloc ->Release ();
				return FALSE;
			}
			bInfo.pidlRoot = pidl;

		}
	}
	bInfo.hwndOwner = hwndParent;
	bInfo.pszDisplayName = m_strPath.GetBuffer (MAX_PATH);
	if (m_strTitle.IsEmpty())
		bInfo.lpszTitle = _T("Open");
	else
		bInfo.lpszTitle = m_strTitle;
	bInfo.ulFlags = m_ulFlags;


	bInfo.lpfn = BrowseCtrlCallback;  // address of callback function
	bInfo.lParam = (LPARAM)this;      // pass address of object to callback function

	if ((pidl = ::SHBrowseForFolder(&bInfo)) == NULL)
	{
		return FALSE;
	}
	m_strPath.ReleaseBuffer();
	m_iImageIndex = bInfo.iImage;

	if (::SHGetPathFromIDList(pidl, m_strPath.GetBuffer(MAX_PATH)) == FALSE)
	{
		pMalloc ->Free(pidl);
		pMalloc ->Release();
		return FALSE;
	}

	m_strPath.ReleaseBuffer();

	pMalloc ->Free(pidl);
	pMalloc ->Release();

	return TRUE;
}

} // namespace
