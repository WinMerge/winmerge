/*!
  \file    CDirDialog.h
  \author  Perry Rapp, Smartronix, Creator, 1999-2001
  \date    Created: 1999
  \date    Edited:  2001/12/12 PR

  \brief   Declaration of CDirDialog
*/
/* The MIT License
Copyright (c) 2001 Perry Rapp
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#if !defined(AFX_DIRDIALOG_H__62FFAC92_1DEE_11D1_B87A_0060979CDF6D__INCLUDED_)
#define AFX_DIRDIALOG_H__62FFAC92_1DEE_11D1_B87A_0060979CDF6D__INCLUDED_
#pragma once

namespace prdlg {

//! directory browse dialog
class CDirDialog
{
public:

	CDirDialog();
	virtual ~CDirDialog();

	BOOL DoBrowse(HWND hwndParent = NULL);
	BOOL DoBrowse(CWnd *pwndParent);

	CString m_strWindowTitle;
	CString m_strPath;
	CString m_strInitDir;
	CString m_strSelDir;
	CString m_strTitle;
	int  m_iImageIndex;
	UINT m_ulFlags;

private:

	virtual BOOL SelChanged(LPCTSTR /*lpcsSelection*/, CString& /*csStatusText*/) { return TRUE; };
	static int __stdcall CDirDialog::BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
};

// not in VC6 supplied headers, but in Aug2001 platform sdk headers
#ifndef BIF_USENEWUI
#define BIF_USENEWUI           (BIF_NEWDIALOGSTYLE | BIF_EDITBOX)
#endif

} // namespace

#endif // !defined(AFX_DIRDIALOG_H__62FFAC92_1DEE_11D1_B87A_0060979CDF6D__INCLUDED_)

