/** 
 * @file WinClasses.cpp
 *
 * @brief Implementation file for CWinClasses
 *
 * @note This code originates from AbstractSpoon / TodoList
 * (http://www.abstractspoon.com/) but is modified to use in
 * WinMerge.
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "WinClasses.h"
#include "wclassdefines.h"

#include <afxpriv.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// helper function for everyone to use
void TRACEWND(LPCTSTR szFunctionName, HWND hWnd)
{
#ifdef _DEBUG
	if (hWnd)
	{
		CString sText; 
		//		pWnd->GetWindowText(sText); 
		TRACE (_T("%s(%s, %s, id = %d)\n"), szFunctionName, CWinClasses::GetClass(hWnd), sText, GetDlgCtrlID(hWnd)); 
	}
#endif
}

#define ADDCONTROL(ctrlname) { CString sCtrl(ctrlname); sCtrl.MakeLower(); s_mapCtrlClasses[sCtrl] = NULL; }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapStringToPtr CWinClasses::s_mapCtrlClasses;

CString CWinClasses::GetClass(HWND hWnd)
{
	static TCHAR szWndClass[128] = _T("");
	
	if (hWnd)
	{
		::GetClassName(hWnd, szWndClass, 127);
		_tcslwr(szWndClass);

		return CString(szWndClass);
	}

	return _T("");
}

BOOL CWinClasses::IsClass(HWND hWnd, LPCTSTR szClass)
{
	if (hWnd)
	{
		TCHAR szWndClass[128] = _T("");

		::GetClassName(hWnd, szWndClass, 127);
		return IsClass(szClass, szWndClass);
	}

	return FALSE;
}

BOOL CWinClasses::IsClassEx(HWND hWnd, LPCTSTR szClass)
{
	if (hWnd)
	{
		CString sClass = GetClassEx(hWnd);

		if (IsClass(sClass, szClass))
			return TRUE;
		else
		{
			CWnd* pWnd = CWnd::FromHandlePermanent(hWnd);

			if (pWnd)
			{
				if (sClass == WC_MFCWND)
					return TRUE;

				else if (IsClass(sClass, WC_MFCFRAME) && pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
					return TRUE;

				else if (IsClass(sClass, WC_MFCVIEW) && pWnd->IsKindOf(RUNTIME_CLASS(CView)))
					return TRUE;

				else if (IsClass(sClass, WC_MFCMDIFRAME) && pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)))
					return TRUE;

				else if (IsClass(sClass, WC_MFCMDICHILD) && pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
					return TRUE;

				else if (IsClass(sClass, WC_MFCMINIDOCKFRAME) && pWnd->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)))
					return TRUE;

				else if (IsClass(sClass, WC_MFCMINIFRAME) && pWnd->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)))
					return TRUE;

				else if (IsClass(sClass, WC_MFCSPLITTER) && pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
					return TRUE;

				else if (IsClass(sClass, WC_MFCDIALOGBAR) && pWnd->IsKindOf(RUNTIME_CLASS(CDialogBar)))
					return TRUE;

				else if (IsClass(sClass, WC_CONTROLBAR) && pWnd->IsKindOf(RUNTIME_CLASS(CControlBar)))
					return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CWinClasses::IsClassEx(LPCTSTR szClass, LPCTSTR szWndClass)
{
	if (IsClass(szClass, szWndClass)) // string comparison
		return TRUE;
	
	else if (IsClass(szWndClass, WC_MFCFRAME)) // handle frame derivatives
	{
		if (IsClass(szClass, WC_MFCMDIFRAME) || IsClass(szClass, WC_MFCMDICHILD) ||
			IsClass(szClass, WC_MFCMINIDOCKFRAME) || IsClass(szClass, WC_MFCMINIFRAME))
			return TRUE;
	}

	return FALSE;
}

CString CWinClasses::GetClassEx(HWND hWnd)
{
	CString sClass = GetClass(hWnd);
	sClass.MakeLower();
	
	if (sClass.Find(_T("afx")) == 0) // its an mfc framework base or derived class
	{
		// can do the check if pWnd is permanent else mfc will not yet
		// have hooked up
		CWnd* pWnd = CWnd::FromHandlePermanent(hWnd);

		if (pWnd)
		{
			// must do the check in order of most derived class first
			if (pWnd->IsKindOf(RUNTIME_CLASS(CView)))
				return WC_MFCVIEW;
			
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)))
				return WC_MFCMDIFRAME;
			
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
				return WC_MFCMDICHILD;
			
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)))
				return WC_MFCMINIDOCKFRAME;
			
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)))
				return WC_MFCMINIFRAME;
			
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd))) // this is the catch all for frame wnds
				return WC_MFCFRAME;
			
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
				return WC_MFCSPLITTER;
			
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CDialogBar)))
				return WC_MFCDIALOGBAR;
			
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CControlBar)))
				return WC_CONTROLBAR;
			
			else 
				return WC_MFCWND; // catch all for all window classes
		}
	}
	
	return sClass;
}

BOOL CWinClasses::IsControlClass(HWND hWnd)
{
	return IsControlClass(GetClass(hWnd));
}

BOOL CWinClasses::IsControlClass(LPCTSTR szClass)
{
	// init
	if (!s_mapCtrlClasses.GetCount())
	{
		ADDCONTROL(WC_BUTTON);
		ADDCONTROL(WC_STATIC);
		ADDCONTROL(WC_EDIT);  
		ADDCONTROL(WC_COMBOBOX);
		ADDCONTROL(WC_COMBOLBOX);
		ADDCONTROL(WC_LISTBOX);  
		ADDCONTROL(WC_SCROLLBAR);
		ADDCONTROL(WC_TOOLBAR);  
		ADDCONTROL(WC_SPIN);     
		ADDCONTROL(WC_PROGRESS); 
		ADDCONTROL(WC_SLIDER);   
		ADDCONTROL(WC_HOTKEY);   
		ADDCONTROL(WC_SHELLDLLDEFVIEW);  
		ADDCONTROL(WC_STATUSBAR);        
		ADDCONTROL(WC_ANIMATE);          
		ADDCONTROL(WC_RICHEDIT);         
		ADDCONTROL(WC_RICHEDIT20);       
		ADDCONTROL(WC_DATETIMEPICK);     
		ADDCONTROL(WC_MONTHCAL);         
		ADDCONTROL(WC_REBAR);            
		ADDCONTROL(WC_TOOLTIPS); 
		ADDCONTROL(WC_THUMBNAILVIEW); 
		ADDCONTROL(WC_IE); 
		ADDCONTROL(WC_SHDOCOBJVW); 
		ADDCONTROL(WC_SHELLEMBEDDING); 
	}

	void* pTemp;
	CString sClass(szClass);
	sClass.MakeLower();

	return s_mapCtrlClasses.Lookup(sClass, pTemp);
}

BOOL CWinClasses::IsEditControl(HWND hWnd)
{
	CString sClass = GetClass(hWnd);

	return (IsClass(sClass, WC_EDIT) ||
			IsClass(sClass, WC_RICHEDIT) ||
			IsClass(sClass, WC_RICHEDIT20));
}

