// ComboBoxEx.cpp : implementation file
//

#include "stdafx.h"
#include "SuperComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEF_AUTOADD_STRING   _T(" <<new template>>")

//	To use this in an app, you'll need to :
//
//	1) Place a normal edit control on your dialog. 
//	2) Check the "Accept Files" property.
//
//	3) In your dialog class, declare a member variable of type CDropEdit
//	(be sure to #include "CDropEdit.h")
//		ex. CDropEdit m_dropEdit;
//
//	4) In your dialog's OnInitDialog, call
//		m_dropEdit.SubclassDlgItem(IDC_YOUR_EDIT_ID, this);
//
//	5) if you want the edit control to handle directories, call
//		m_dropEdit.SetUseDir(TRUE);
//
//	6) if you want the edit control to handle files, call
//		m_dropEdit.SetUseDir(FALSE);
//
//      7) In the dialog resource template, any groupboxes must be after any comboboxes which accept files
//
//	that's it!
//
//	This will behave exactly like a normal edit-control but with the 
//	ability to accept drag-n-dropped files (or directories).
//
//


/////////////////////////////////////////////////////////////////////////////
// CSuperComboBox

CSuperComboBox::CSuperComboBox(BOOL bAdd /*= TRUE*/, UINT idstrAddText /*= 0*/)
{
	m_bEditChanged=FALSE;
	m_bDoComplete = FALSE;
	m_bAutoComplete = FALSE;
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
	m_bMustUninitOLE = FALSE;
	_AFX_THREAD_STATE* pState = AfxGetThreadState();
	if (!pState->m_bNeedTerm)
	{
		SCODE sc = ::OleInitialize(NULL);
		if (FAILED(sc))
			AfxMessageBox(_T("OLE initialization failed. Make sure that the OLE libraries are the correct version"));
		else
			m_bMustUninitOLE = TRUE;
	}
}

CSuperComboBox::~CSuperComboBox()
{
	// Uninitialize OLE support
	if (m_bMustUninitOLE)
		::OleUninitialize();
}

BEGIN_MESSAGE_MAP(CSuperComboBox, CComboBox)
	//{{AFX_MSG_MAP(CSuperComboBox)
	ON_CONTROL_REFLECT_EX(CBN_EDITCHANGE, OnEditchange)
	ON_CONTROL_REFLECT_EX(CBN_SELCHANGE, OnSelchange)
	ON_WM_CREATE()
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSuperComboBox message handlers

void CSuperComboBox::LoadState(LPCTSTR szRegSubKey, UINT nMaxItems)
{
	CString s,s2;
	UINT cnt = AfxGetApp()->GetProfileInt(szRegSubKey, _T("Count"), 0);
	for (UINT i=0; i < cnt && i < nMaxItems; i++)
	{
		s2.Format(_T("Item_%d"), i);
		s = AfxGetApp()->GetProfileString(szRegSubKey, s2);
		if (i==0)
			SetWindowText(s);
		if (FindStringExact(-1, s) == -1
			&& !s.IsEmpty())
			AddString(s);
	}
}

void CSuperComboBox::SaveState(LPCTSTR szRegSubKey, UINT nMaxItems)
{
	CString strItem,s,s2;
	int i,idx,cnt = GetCount();

	GetWindowText(strItem);
	if (!strItem.IsEmpty())
	{
		AfxGetApp()->WriteProfileString(szRegSubKey, _T("Item_0"), strItem);
		idx=1;
	}
	else
		idx=0;
	for (i=0; i < cnt && idx < (int)nMaxItems; i++)
	{
		GetLBText(i, s);
		if (s != strItem
			&& !s.IsEmpty())
		{
			s2.Format(_T("Item_%d"), idx);
			AfxGetApp()->WriteProfileString(szRegSubKey, s2, s);
			idx++;
		}
	}
	AfxGetApp()->WriteProfileInt(szRegSubKey, _T("Count"), idx);
}


BOOL CSuperComboBox::OnEditchange() 
{
	m_bEditChanged=TRUE;
	
	int length = GetWindowTextLength();

	// bail if not auto completing or no text
	if (!m_bDoComplete
		|| length <= 0) 
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
        if (m_bAutoComplete
			&& pMsg->message == WM_KEYDOWN)
        {
                m_bDoComplete = TRUE;

                int nVirtKey = (int) pMsg->wParam;
                if (nVirtKey == VK_DELETE || nVirtKey == VK_BACK)
                        m_bDoComplete = FALSE;
        }

        return CComboBox::PreTranslateMessage(pMsg);
}

void CSuperComboBox::SetAutoAdd(BOOL bAdd, UINT idstrAddText)
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

void CSuperComboBox::ResetContent()
{
	CComboBox::ResetContent();
	if (!m_strAutoAdd.IsEmpty())
	{
		InsertString(0, m_strAutoAdd);
	}
}

int CSuperComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetAutoAdd(!m_strAutoAdd.IsEmpty());
	DragAcceptFiles(TRUE);
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//	OnDropFiles code from CDropEdit
//	Copyright 1997 Chris Losinger
//
//	shortcut expansion code modified from :
//	CShortcut, 1996 Rob Warner
//
void CSuperComboBox::OnDropFiles(HDROP dropInfo)
{
	// Get the number of pathnames that have been dropped
	UINT wNumFilesDropped = DragQueryFile(dropInfo, 0xFFFFFFFF, NULL, 0);

	CString firstFile;

	// get all file names. but we'll only need the first one.
	for (WORD x = 0 ; x < wNumFilesDropped; x++) {

		// Get the number of characters required by the file's full pathname
		UINT wPathnameSize = DragQueryFile(dropInfo, x, NULL, 0);

		// Allocate memory to contain full pathname & zero byte
		wPathnameSize += 1;
		LPTSTR npszFile = (TCHAR *) new TCHAR[wPathnameSize];

		// If not enough memory, skip this one
		if (npszFile == NULL)
			continue;

		// Copy the pathname into the buffer
		DragQueryFile(dropInfo, x, npszFile, wPathnameSize);

		// we only care about the first
		if (firstFile==_T(""))
			firstFile=npszFile;

		// clean up
		delete[] npszFile;
	}

	// Free the memory block containing the dropped-file information
	DragFinish(dropInfo);

	// if this was a shortcut, we need to expand it to the target path
	CString expandedFile = ExpandShortcut(firstFile);

	// if that worked, we should have a real file name
	if (expandedFile!=_T("")) 
		firstFile=expandedFile;

	GetParent()->SendMessage(WM_COMMAND, GetDlgCtrlID() +
		(CBN_EDITUPDATE << 16), (LPARAM)m_hWnd);
	SetWindowText(firstFile);
	GetParent()->SendMessage(WM_COMMAND, GetDlgCtrlID() +
		(CBN_EDITCHANGE << 16), (LPARAM)m_hWnd);
}

//////////////////////////////////////////////////////////////////
//	use IShellLink to expand the shortcut
//	returns the expanded file, or "" on error
//
//	original code was part of CShortcut 
//	1996 by Rob Warner
//	rhwarner@southeast.net
//	http://users.southeast.net/~rhwarner

CString CSuperComboBox::ExpandShortcut(CString &inFile)
{
	CString outFile = "";

    // Make sure we have a path
    ASSERT(inFile != _T(""));

    IShellLink* psl;
    HRESULT hres;
    LPTSTR lpsz = inFile.GetBuffer(MAX_PATH);

    // Create instance for shell link
    hres = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
        IID_IShellLink, (LPVOID*) &psl);
    if (SUCCEEDED(hres))
    {
        // Get a pointer to the persist file interface
        IPersistFile* ppf;
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*) &ppf);
        if (SUCCEEDED(hres))
        {
            // Make sure it's ANSI
            WORD wsz[MAX_PATH];
#ifdef _UNICODE
	     wcsncpy(wsz, lpsz, sizeof(wsz)/sizeof(WCHAR));
#else
            ::MultiByteToWideChar(CP_ACP, 0, lpsz, -1, wsz, MAX_PATH);
#endif

            // Load shortcut
            hres = ppf->Load(wsz, STGM_READ);
            if (SUCCEEDED(hres)) {
				WIN32_FIND_DATA wfd;
				// find the path from that
				HRESULT hres = psl->GetPath(outFile.GetBuffer(MAX_PATH), 
								MAX_PATH,
								&wfd, 
								SLGP_UNCPRIORITY);

				outFile.ReleaseBuffer();
            }
            ppf->Release();
        }
        psl->Release();
    }

	inFile.ReleaseBuffer();

	// if this fails, outFile == ""
    return outFile;
}
