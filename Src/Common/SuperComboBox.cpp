// CSuperComboBox.cpp : implementation file
//

#include "StdAfx.h"
#include "SuperComboBox.h"
#include <vector>
#include "DropHandler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DEF_AUTOADD_STRING   _T(" <<new template>>")


/////////////////////////////////////////////////////////////////////////////
// CSuperComboBox

HIMAGELIST CSuperComboBox::m_himlSystem = NULL;

CSuperComboBox::CSuperComboBox(bool bAdd /*= true*/, UINT idstrAddText /*= 0*/)
	: m_pDropHandler(NULL)
{
	m_bEditChanged = false;
	m_bDoComplete = false;
	m_bAutoComplete = false;
	m_bHasImageList = false;
	m_bRecognizedMyself = false;
	m_bComboBoxEx = false;

	m_strCurSel = _T("");
	if (bAdd)
	{
		if (idstrAddText > 0)
			VERIFY(m_strAutoAdd.LoadString(idstrAddText));
		else
			m_strAutoAdd = DEF_AUTOADD_STRING;
	}
	else
		m_strAutoAdd = _T("");

	// Initialize OLE libraries if not yet initialized
	m_bMustUninitOLE = false;
	_AFX_THREAD_STATE* pState = AfxGetThreadState();
	if (!pState->m_bNeedTerm)
	{
		SCODE sc = ::OleInitialize(NULL);
		if (FAILED(sc))
			AfxMessageBox(_T("OLE initialization failed. Make sure that the OLE libraries are the correct version"));
		else
			m_bMustUninitOLE = true;
	}
}

CSuperComboBox::~CSuperComboBox()
{
	// Uninitialize OLE support
	if (m_bMustUninitOLE)
		::OleUninitialize();
}

BEGIN_MESSAGE_MAP(CSuperComboBox, CComboBoxEx)
	//{{AFX_MSG_MAP(CSuperComboBox)
	ON_CONTROL_REFLECT_EX(CBN_EDITCHANGE, OnEditchange)
	ON_CONTROL_REFLECT_EX(CBN_SELCHANGE, OnSelchange)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(CBEN_GETDISPINFO, OnGetDispInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSuperComboBox message handlers

void CSuperComboBox::PreSubclassWindow()
{
	CComboBoxEx::PreSubclassWindow();
	m_pDropHandler = new DropHandler(std::bind(&CSuperComboBox::OnDropFiles, this, std::placeholders::_1));
	RegisterDragDrop(m_hWnd, m_pDropHandler);
	
	TCHAR szClassName[256];
	GetClassName(m_hWnd, szClassName, sizeof(szClassName)/sizeof(szClassName[0]));
	if (lstrcmpi(_T("ComboBoxEx32"), szClassName) == 0)
		m_bComboBoxEx = true;

	m_bRecognizedMyself = true;
}

/**
 * @brief Returns whether the window associated with this object is ComboBoxEx.
 */
bool CSuperComboBox::IsComboBoxEx() const
{
	ASSERT(m_bRecognizedMyself);
	return m_bComboBoxEx;

}

/**
 * @brief Adds a string to the list box of a combo box
 * @param lpszItem Pointer to the null-terminated string that is to be added. 
 */
int CSuperComboBox::AddString(LPCTSTR lpszItem)
{
	return InsertString(GetCount(), lpszItem);
}

/**
 * @brief Inserts a string into the list box of a combo box.
 * @param nIndex The zero-based index to the position in the list box that receives the string.
 * @param lpszItem Pointer to the null-terminated string that is to be added. 
 */
int CSuperComboBox::InsertString(int nIndex, LPCTSTR lpszItem)
{
	if (IsComboBoxEx())
	{
		COMBOBOXEXITEM cbitem = {0};
		cbitem.mask = CBEIF_TEXT |
			(m_bHasImageList ? CBEIF_IMAGE|CBEIF_SELECTEDIMAGE : 0);
		cbitem.pszText = (LPTSTR)lpszItem;
		cbitem.cchTextMax = (int)_tcslen(lpszItem);
		cbitem.iItem = nIndex;
		cbitem.iImage = I_IMAGECALLBACK;
		cbitem.iSelectedImage = I_IMAGECALLBACK;
		return CComboBoxEx::InsertItem(&cbitem);
	}
	else
	{
		return CComboBox::InsertString(nIndex, lpszItem);
	}
}

/**
 * @brief Gets the system image list and attaches the image list to a combo box control.
 */
bool CSuperComboBox::AttachSystemImageList()
{
	ASSERT(IsComboBoxEx());
	if (!m_himlSystem)
	{
		SHFILEINFO sfi = {0};
		m_himlSystem = (HIMAGELIST)SHGetFileInfo(_T(""), 0, 
			&sfi, sizeof(sfi), SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
		if (!m_himlSystem)
			return false;
	}
	SetImageList(CImageList::FromHandle(m_himlSystem));
	m_bHasImageList = true;
	return true;
}

void CSuperComboBox::LoadState(LPCTSTR szRegSubKey, bool bCanBeEmpty /*=false */, int nMaxItems /*=20 */)
{
	CString s,s2;
	bool bIsEmpty = false;
	if( bCanBeEmpty )
		bIsEmpty = (AfxGetApp()->GetProfileInt(szRegSubKey, _T("Empty"), FALSE) == TRUE);
	int cnt = AfxGetApp()->GetProfileInt(szRegSubKey, _T("Count"), 0);
	for (int i=0; i < cnt && i < nMaxItems; i++)
	{
		s2.Format(_T("Item_%u"), i);
		s = AfxGetApp()->GetProfileString(szRegSubKey, s2);
		if (FindStringExact(-1, s) == CB_ERR && !s.IsEmpty())
			AddString(s);
	}
	if (cnt > 0)
		if( bIsEmpty )
		{
			SetCurSel(-1);
		}
		else
		{
			SetCurSel(0);
		}
}

void CSuperComboBox::GetLBText(int nIndex, CString &rString) const
{
	ASSERT(::IsWindow(m_hWnd));
	CComboBoxEx::GetLBText(nIndex, rString.GetBufferSetLength(GetLBTextLen(nIndex)));
	rString.ReleaseBuffer();
}

int CSuperComboBox::GetLBTextLen(int nIndex) const
{
	return CComboBoxEx::GetLBTextLen(nIndex);
}

/** 
 * @brief Saves strings in combobox.
 * This function saves strings in combobox, in editbox and in dropdown.
 * Whitespace characters are stripped from begin and end of the strings
 * before saving. Empty strings are not saved. So strings which have only
 * whitespace characters aren't save either.
 * @param [in] szRegSubKey Registry subkey where to save strings.
 * @param [in] nMaxItems Max number of strings to save.
 */
void CSuperComboBox::SaveState(LPCTSTR szRegSubKey, bool bCanBeEmpty /*=false */, int nMaxItems /*=20 */)
{
	CString strItem,s,s2;
	int idx = 0;

	if (IsComboBoxEx())
		GetEditCtrl()->GetWindowText(strItem);
	else
		GetWindowText(strItem);
	if (!strItem.IsEmpty())
	{
		AfxGetApp()->WriteProfileString(szRegSubKey, _T("Item_0"), strItem);
		idx=1;
	}
	int cnt = GetCount();
	for (int i=idx; i < cnt && i < nMaxItems; i++)
	{
		GetLBText(i, s);
		s.TrimLeft();
		s.TrimRight();
		if (s != strItem && !s.IsEmpty())
		{
			s2.Format(_T("Item_%d"), idx);
			AfxGetApp()->WriteProfileString(szRegSubKey, s2, s);
			idx++;
		}
	}
	AfxGetApp()->WriteProfileInt(szRegSubKey, _T("Count"), idx);
	
	if (bCanBeEmpty)
		AfxGetApp()->WriteProfileInt(_T("Files\\Option"), _T("Empty"), strItem.IsEmpty());
}


BOOL CSuperComboBox::OnEditchange() 
{
	m_bEditChanged = true;

	// bail if not auto completing 
	if (!m_bDoComplete) 
		return FALSE;
	
	int length = GetWindowTextLength();

	// bail if no text
	if (length <= 0) 
		return FALSE;
	
	// Get the text in the edit box
	CString s;
	GetWindowText(s);
	
	// get the current selection
	DWORD sel = GetEditSel();
	WORD start=LOWORD(sel), end=HIWORD(sel);
	
	// look for the string that is prefixed by the typed text
	int idx = FindString(-1, s);
	if (idx != CB_ERR)
	{
		// set the new string
		CString strNew;
		GetLBText(idx, strNew);
		SetWindowText(strNew);         
		
		// get the caret back in the right spot
		if (sel != CB_ERR)
			SetEditSel(start, end);  
	}
	
	// select the text after our typing
	if (sel == CB_ERR
		|| end >= length)
		SetEditSel(length, -1);
	// restore the selection
	else
		SetEditSel(start, end);

	return FALSE;
}

BOOL CSuperComboBox::OnSelchange() 
{
	m_bEditChanged=FALSE;

	CString strCurSel;
	GetWindowText(strCurSel);
	if (m_strAutoAdd.IsEmpty()
		|| strCurSel != m_strAutoAdd)
		m_strCurSel = strCurSel;
	
	if (!m_strAutoAdd.IsEmpty())
	{
		int sel = GetCurSel();
		if (sel != CB_ERR)
		{
			CString s;
			GetLBText(sel, s);
			if (s == m_strAutoAdd)
			{
				if (OnAddTemplate())
				{
				}
				else if (!m_strCurSel.IsEmpty()
					&& m_strCurSel != m_strAutoAdd)
				{
					if (SelectString(0, m_strCurSel) != CB_ERR)
						return TRUE;
				}
				else
				{
					SetCurSel(1);
				}
			}
		}
	}
	return FALSE;
}

BOOL CSuperComboBox::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
		int nVirtKey = (int) pMsg->wParam;
		// If Shift+Del pressed when dropdown is open, delete selected item
		// from dropdown list
		if (GetAsyncKeyState(VK_SHIFT))
		{
			if (GetDroppedState() && nVirtKey == VK_DELETE)
			{
				int cursel = GetCurSel();
				if (cursel != CB_ERR)
					DeleteString(cursel);
				return FALSE; // No need to further handle this message
			}
		}
		if (m_bAutoComplete)
		{
			m_bDoComplete = true;

			if (nVirtKey == VK_DELETE || nVirtKey == VK_BACK)
					m_bDoComplete = FALSE;
		}
    }

    return CComboBoxEx::PreTranslateMessage(pMsg);
}

void CSuperComboBox::SetAutoAdd(bool bAdd /*= true*/, UINT idstrAddText /*= 0*/)
{
	if (bAdd)
	{
		if (idstrAddText > 0)
			VERIFY(m_strAutoAdd.LoadString(idstrAddText));
		else if (m_strAutoAdd.IsEmpty())
			m_strAutoAdd = DEF_AUTOADD_STRING;

		InsertString(0, m_strAutoAdd);
	}
	else
		m_strAutoAdd = _T("");
}

BOOL CSuperComboBox::OnAddTemplate()
{
	// do nothing, this should get overridden
	// return TRUE if template is added
	return FALSE;
}

void CSuperComboBox::SetAutoComplete(INT nSource)
{
	switch (nSource)
	{
		case AUTO_COMPLETE_DISABLED:
			m_bAutoComplete = false;
			break;

		case AUTO_COMPLETE_FILE_SYSTEM:
		{
			// Disable the build-in auto-completion and use the Windows
			// shell functionality.
			m_bAutoComplete = false;

			// ComboBox's edit control is alway 1001.
			CWnd *pWnd = IsComboBoxEx() ? this->GetEditCtrl() : GetDlgItem(1001);
			ASSERT(NULL != pWnd);
			SHAutoComplete(pWnd->m_hWnd, SHACF_FILESYSTEM);
			break;
		}

		case AUTO_COMPLETE_RECENTLY_USED:
			m_bAutoComplete = true;
			break;

		default:
			ASSERT(!"Unknown AutoComplete source.");
			m_bAutoComplete = false;
	}
}

void CSuperComboBox::ResetContent()
{
	CComboBoxEx::ResetContent();
	if (!m_strAutoAdd.IsEmpty())
	{
		InsertString(0, m_strAutoAdd);
	}
}

int CSuperComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBoxEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetAutoAdd(!m_strAutoAdd.IsEmpty());
	m_pDropHandler = new DropHandler(std::bind(&CSuperComboBox::OnDropFiles, this, std::placeholders::_1));
	RegisterDragDrop(m_hWnd, m_pDropHandler);
	return 0;
}

void CSuperComboBox::OnDestroy(void)
{
	if (m_pDropHandler)
		RevokeDragDrop(m_hWnd);
}

/////////////////////////////////////////////////////////////////////////////
//
//	OnDropFiles code from CDropEdit
//	Copyright 1997 Chris Losinger
//
//	shortcut expansion code modified from :
//	CShortcut, 1996 Rob Warner
//
void CSuperComboBox::OnDropFiles(const std::vector<String>& files)
{
	GetParent()->SendMessage(WM_COMMAND, GetDlgCtrlID() +
		(CBN_EDITUPDATE << 16), (LPARAM)m_hWnd);
	SetWindowText(files[0].c_str());
	GetParent()->SendMessage(WM_COMMAND, GetDlgCtrlID() +
		(CBN_EDITCHANGE << 16), (LPARAM)m_hWnd);
}

static DWORD WINAPI SHGetFileInfoThread(LPVOID pParam)
{
	TCHAR szPath[MAX_PATH_FULL];
	lstrcpy(szPath, (LPCTSTR)pParam);

	SHFILEINFO sfi = {0};
	sfi.iIcon = -1;
	SHGetFileInfo(szPath, 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX);
	return sfi.iIcon;
}

/**
 * @brief A message handler for CBEN_GETDISPINFO message
 */
void CSuperComboBox::OnGetDispInfo(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	NMCOMBOBOXEX *pDispInfo = (NMCOMBOBOXEX *)pNotifyStruct;
	if (pDispInfo && pDispInfo->ceItem.pszText && m_bHasImageList)
	{
		pDispInfo->ceItem.mask |= CBEIF_DI_SETITEM;
		SHFILEINFO sfi = {0};
		CString sText;
		GetLBText(static_cast<int>(pDispInfo->ceItem.iItem), sText);
		bool isNetworkDrive = false;
		if (sText.GetLength() >= 2 && (sText[1] == L'\\'))
		{
			if (sText.GetLength() > 4 && sText.Left(4) == L"\\\\?\\")
				if (sText.GetLength() > 8 && sText.Left(8) == L"\\\\?\\UNC\\")
					isNetworkDrive = true;
				else
					isNetworkDrive = false;	// Just a Long File Name indicator
			else
				isNetworkDrive = true;
		}
		else
		if (sText.GetLength() >= 3 && GetDriveType(sText.Left(3)) == DRIVE_REMOTE)
			isNetworkDrive = true;	// Drive letter, but mapped to Remote UNC device.

		if (!isNetworkDrive)
		{
			// The path is not a network path.
			if (SHGetFileInfo(sText, 0, &sfi, sizeof(sfi), 
								SHGFI_SYSICONINDEX) != NULL)
			{
				pDispInfo->ceItem.iImage = sfi.iIcon;
				pDispInfo->ceItem.iSelectedImage = sfi.iIcon;
			}
		}
		else
		{
			// The path is a network path. 
			// Try to get the index of a system image list icon, with 1-sec timeout.
			HANDLE hThread = CreateThread(NULL, 0, SHGetFileInfoThread, 
											(VOID *)(LPCTSTR)sText, 0, NULL);
			if (hThread != NULL)
			{
				DWORD dwResult = WaitForSingleObject(hThread, 1000);
				if (dwResult == WAIT_OBJECT_0)
				{
					GetExitCodeThread(hThread, (DWORD*)&sfi.iIcon);
					pDispInfo->ceItem.iImage = sfi.iIcon;
					pDispInfo->ceItem.iSelectedImage = sfi.iIcon;
				}
				CloseHandle(hThread);
			}
		}
	}
	*pResult = 0;
}
