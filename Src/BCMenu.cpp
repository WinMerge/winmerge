//*************************************************************************
// BCMenu.cpp : implementation file
// Version : 3.0
// Date : January 2002
// Author : Brent Corkum
// Email :  corkum@rocscience.com
// Latest Version : http://www.rocscience.com/~corkum/BCMenu.html
// 
// Bug Fixes and portions of code supplied by:
//
// Ben Ashley,Girish Bharadwaj,Jean-Edouard Lachand-Robert,
// Robert Edward Caldecott,Kenny Goers,Leonardo Zide,
// Stefan Kuhr,Reiner Jung,Martin Vladic,Kim Yoo Chul,
// Oz Solomonovich,Tongzhe Cui,Stephane Clog,Warren Stevens,
// Damir Valiulin
//
// You are free to use/modify this code but leave this header intact.
// This class is public domain so you are free to use it any of
// your applications (Freeware,Shareware,Commercial). All I ask is
// that you let me know so that if you have a real winner I can
// brag to my buddies that some of my code is in your app. I also
// wouldn't mind if you sent me a copy of your application since I
// like to play with new stuff.
//*************************************************************************

#include "stdafx.h"        // Standard windows header file
#include "BCMenu.h"        // BCMenu class declaration
#include <afxpriv.h>       //SK: makes A2W and other spiffy AFX macros work

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GAP 1
#ifndef OBM_CHECK
#define OBM_CHECK 32760 // from winuser.h
#endif

#if _MFC_VER <0x400
#error This code does not work on Versions of MFC prior to 4.0
#endif

static CPINFO CPInfo;
// how the menu's are drawn in win9x/NT/2000
UINT BCMenu::original_drawmode=BCMENU_DRAWMODE_ORIGINAL;
BOOL BCMenu::xp_select_disabled=FALSE;
// how the menu's are drawn in winXP
UINT BCMenu::xp_drawmode=BCMENU_DRAWMODE_XP;
BOOL BCMenu::original_select_disabled=TRUE;

enum Win32Type{
	Win32s,
	Windoze95,
	WinNT3,
	WinNT4orHigher
};


Win32Type IsShellType()
{
	Win32Type  ShellType;
	DWORD winVer;
	OSVERSIONINFO *osvi;
	
	winVer=GetVersion();
	if(winVer<0x80000000){/*NT */
		ShellType=WinNT3;
		osvi= (OSVERSIONINFO *)malloc(sizeof(OSVERSIONINFO));
		if (osvi!=NULL){
			memset(osvi,0,sizeof(OSVERSIONINFO));
			osvi->dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
			GetVersionEx(osvi);
			if (osvi->dwMajorVersion>=4L)
				ShellType=WinNT4orHigher;//yup, it is NT 4 or higher!
			free(osvi);
		}
	}
	else if  (LOBYTE(LOWORD(winVer))<4)
		ShellType=Win32s;/*Win32s*/
	else
		ShellType=Windoze95;/*Windoze95*/
	return ShellType;
}

static Win32Type g_Shell=IsShellType();

void BCMenuData::SetAnsiString(LPCSTR szAnsiString)
{
	USES_CONVERSION;
	SetWideString(A2W(szAnsiString));  //SK:  see MFC Tech Note 059
}

CString BCMenuData::GetString(void)//returns the menu text in ANSI or UNICODE
//depending on the MFC-Version we are using
{
	CString strText;
	if (m_szMenuText)
    {
#ifdef UNICODE
		strText = m_szMenuText;
#else
		USES_CONVERSION;
		strText=W2A(m_szMenuText);     //SK:  see MFC Tech Note 059
#endif    
    }
	return strText;
}

CTypedPtrArray<CPtrArray, HMENU> BCMenu::m_AllSubMenus;  // Stores list of all sub-menus

/*
===============================================================================
BCMenu::BCMenu()
BCMenu::~BCMenu()
-----------------

Constructor and Destructor.

===============================================================================
*/

BCMenu::BCMenu()
{
	m_bDynIcons = FALSE;     // O.S. - no dynamic icons by default
	disable_old_style=FALSE;
	m_iconX = 16;            // Icon sizes default to 16 x 16
	m_iconY = 15;            // ...
	m_selectcheck = -1;
	m_unselectcheck = -1;
	checkmaps=NULL;
	checkmapsshare=FALSE;
	// set the color used for the transparent background in all bitmaps
	m_bitmapBackground=RGB(192,192,192); //gray
	m_bitmapBackgroundFlag=FALSE;
	GetCPInfo(CP_ACP,&CPInfo);
}


BCMenu::~BCMenu()
{
	DestroyMenu();
}

BOOL BCMenu::IsNewShell ()
{
	return (Windoze95==g_Shell || WinNT4orHigher==g_Shell);
}


BCMenuData::~BCMenuData()
{
	if(bitmap)
		delete(bitmap);
	
	delete[] m_szMenuText; //Need not check for NULL because ANSI X3J16 allows "delete NULL"
}


void BCMenuData::SetWideString(const wchar_t *szWideString)
{
	delete[] m_szMenuText;//Need not check for NULL because ANSI X3J16 allows "delete NULL"
	
	if (szWideString)
    {
		m_szMenuText = new wchar_t[sizeof(wchar_t)*(wcslen(szWideString)+1)];
		if (m_szMenuText)
			wcscpy(m_szMenuText,szWideString);
    }
	else
		m_szMenuText=NULL;//set to NULL so we need not bother about dangling non-NULL Ptrs
}

BOOL BCMenu::IsMenu(CMenu *submenu)
{
	int m;
	int numSubMenus = m_AllSubMenus.GetUpperBound();
	for(m=0;m<=numSubMenus;++m){
		if(submenu->m_hMenu==m_AllSubMenus[m])return(TRUE);
	}
	return(FALSE);
}

BOOL BCMenu::IsMenu(HMENU submenu)
{
	int m;
	int numSubMenus = m_AllSubMenus.GetUpperBound();
	for(m=0;m<=numSubMenus;++m){
		if(submenu==m_AllSubMenus[m])return(TRUE);
	}
	return(FALSE);
}

BOOL BCMenu::DestroyMenu()
{
	// Destroy Sub menus:
	int m,n;
	int numAllSubMenus = m_AllSubMenus.GetUpperBound();
	for(n = numAllSubMenus; n>= 0; n--){
		if(m_AllSubMenus[n]==this->m_hMenu)m_AllSubMenus.RemoveAt(n);
	}
	int numSubMenus = m_SubMenus.GetUpperBound();
	for(m = numSubMenus; m >= 0; m--){
		numAllSubMenus = m_AllSubMenus.GetUpperBound();
		for(n = numAllSubMenus; n>= 0; n--){
			if(m_AllSubMenus[n]==m_SubMenus[m])m_AllSubMenus.RemoveAt(n);
		}
		delete((BCMenu *)FromHandle(m_SubMenus[m]));
	}
	m_SubMenus.RemoveAll();
	// Destroy menu data
	int numItems = m_MenuList.GetUpperBound();
	for(m = 0; m <= numItems; m++)delete(m_MenuList[m]);
	m_MenuList.RemoveAll();
	if(checkmaps&&!checkmapsshare){
		delete checkmaps;
		checkmaps=NULL;
	}
	// Call base-class implementation last:
	return(CMenu::DestroyMenu());
};

int BCMenu::GetMenuDrawMode(void)
{
	COLORREF m_newclrBack=GetSysColor((IsNewShell())?COLOR_3DFACE:COLOR_MENU);
	COLORREF m_clrBack=GetSysColor(COLOR_MENU);
	BOOL XPflag = m_newclrBack==m_clrBack ? FALSE : TRUE;
	if(XPflag)return(xp_drawmode);
	return(original_drawmode);
}


/*
==========================================================================
void BCMenu::DrawItem(LPDRAWITEMSTRUCT)
---------------------------------------

  Called by the framework when a particular item needs to be drawn.  We
  overide this to draw the menu item in a custom-fashion, including icons
  and the 3D rectangle bar.
  ==========================================================================
*/

void BCMenu::DrawItem (LPDRAWITEMSTRUCT lpDIS)
{
	ASSERT(lpDIS != NULL);
	COLORREF m_newclrBack=GetSysColor((IsNewShell())?COLOR_3DFACE:COLOR_MENU);
	COLORREF m_clrBack=GetSysColor(COLOR_MENU);
	BOOL XPflag = m_newclrBack==m_clrBack ? FALSE : TRUE;
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	if(pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE)DrawItem_Win9xNT2000(lpDIS);
	else{
		if(XPflag){
			if(xp_drawmode==BCMENU_DRAWMODE_XP) DrawItem_WinXP(lpDIS);
			else DrawItem_Win9xNT2000(lpDIS);
		}
		else{
			if(original_drawmode==BCMENU_DRAWMODE_XP) DrawItem_WinXP(lpDIS);
			else DrawItem_Win9xNT2000(lpDIS);
		}	
	}
}

void BCMenu::DrawItem_Win9xNT2000 (LPDRAWITEMSTRUCT lpDIS)
{
	ASSERT(lpDIS != NULL);
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	CRect rect;
	UINT state = (((BCMenuData*)(lpDIS->itemData))->nFlags);
	CBrush m_brBackground;
	
	m_brBackground.CreateSolidBrush(GetSysColor((IsNewShell())?COLOR_3DFACE:COLOR_MENU));

	// remove the selected bit if it's grayed out
	if(lpDIS->itemState & ODS_GRAYED&&!original_select_disabled){
		if(lpDIS->itemState & ODS_SELECTED)lpDIS->itemState=lpDIS->itemState & ~ODS_SELECTED;
	}
	
	if(state & MF_SEPARATOR){
		rect.CopyRect(&lpDIS->rcItem);
		pDC->FillRect (rect,&m_brBackground);
		rect.top += (rect.Height()>>1);
		pDC->DrawEdge(&rect,EDGE_ETCHED,BF_TOP);
	}
	else{
		CRect rect2;
		BOOL standardflag=FALSE,selectedflag=FALSE,disableflag=FALSE;
		BOOL checkflag=FALSE;
		COLORREF crText = GetSysColor(COLOR_MENUTEXT);
		COLORREF m_clrBack=GetSysColor((IsNewShell())?COLOR_3DFACE:COLOR_MENU);
		CBrush m_brSelect;
		CPen m_penBack;
		int x0,y0,dy;
		int nIconNormal=-1,xoffset=-1;
		CImageList *bitmap=NULL;
		CFont m_fontMenu;
		LOGFONT m_lf;
		
		// set some colors and the font
		m_penBack.CreatePen (PS_SOLID,0,GetSysColor((IsNewShell())?COLOR_3DFACE:COLOR_MENU));
		m_brSelect.CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
		ZeroMemory ((PVOID) &m_lf,sizeof (LOGFONT));
		NONCLIENTMETRICS nm;
		nm.cbSize = sizeof (NONCLIENTMETRICS);
		VERIFY (SystemParametersInfo(SPI_GETNONCLIENTMETRICS,nm.cbSize,&nm,0)); 
		m_lf =  nm.lfMenuFont;
		m_fontMenu.CreateFontIndirect (&m_lf);
		
		// draw the colored rectangle portion
		
		rect.CopyRect(&lpDIS->rcItem);
		rect2=rect;
		
		// draw the up/down/focused/disabled state
		
		UINT state = lpDIS->itemState;
		CString strText;
		LOGFONT lf;
		lf = m_lf;
		
		CFont dispFont;
		CFont *pFont=NULL;
		
		if(lpDIS->itemData != NULL){
			nIconNormal = (((BCMenuData*)(lpDIS->itemData))->menuIconNormal);
			xoffset = (((BCMenuData*)(lpDIS->itemData))->xoffset);
			bitmap = (((BCMenuData*)(lpDIS->itemData))->bitmap);
			strText = ((BCMenuData*) (lpDIS->itemData))->GetString();
			
			if(state&ODS_CHECKED && nIconNormal<0){
				if(state&ODS_SELECTED && m_selectcheck>0)checkflag=TRUE;
				else if(m_unselectcheck>0) checkflag=TRUE;
			}
			else if(nIconNormal != -1){
				standardflag=TRUE;
				if(state&ODS_SELECTED && !(state&ODS_GRAYED))selectedflag=TRUE;
				else if(state&ODS_GRAYED) disableflag=TRUE;
			}
		}
		else{
			strText.Empty();
		}
		
		if(state&ODS_SELECTED){ // draw the down edges
			
			CPen *pOldPen = pDC->SelectObject (&m_penBack);
			
			// You need only Text highlight and thats what you get
			
			if(checkflag||standardflag||selectedflag||disableflag||state&ODS_CHECKED)
				rect2.SetRect(rect.left+m_iconX+4+GAP,rect.top,rect.right,rect.bottom);
			pDC->FillRect (rect2,&m_brSelect);
			
			pDC->SelectObject (pOldPen);
			if((HFONT)dispFont != NULL)dispFont.DeleteObject ();
			dispFont.CreateFontIndirect (&lf);
			crText = GetSysColor(COLOR_HIGHLIGHTTEXT);
		}
		else {
			CPen *pOldPen = pDC->SelectObject (&m_penBack);
			pDC->FillRect (rect,&m_brBackground);
			pDC->SelectObject (pOldPen);
			
			// draw the up edges
			
			pDC->Draw3dRect (rect,m_clrBack,m_clrBack);
			if ((HFONT)dispFont != NULL) dispFont.DeleteObject ();
			dispFont.CreateFontIndirect (&lf); //Normal
		}
		
		// draw the text if there is any
		//We have to paint the text only if the image is nonexistant
		
		dy = (rect.Height()-4-m_iconY)/2;
		dy = dy<0 ? 0 : dy;
		
		if(checkflag||standardflag||selectedflag||disableflag){
			rect2.SetRect(rect.left+1,rect.top+1+dy,rect.left+m_iconX+3,
				rect.top+m_iconY+3+dy);
			pDC->Draw3dRect (rect2,m_clrBack,m_clrBack);
			if(checkflag && checkmaps){
				pDC->FillRect (rect2,&m_brBackground);
				rect2.SetRect(rect.left,rect.top+dy,rect.left+m_iconX+4,
					rect.top+m_iconY+4+dy);
				
				pDC->Draw3dRect (rect2,m_clrBack,m_clrBack);
				CPoint ptImage(rect.left+2,rect.top+2+dy);
				
				if(state&ODS_SELECTED)checkmaps->Draw(pDC,1,ptImage,ILD_TRANSPARENT);
				else checkmaps->Draw(pDC,0,ptImage,ILD_TRANSPARENT);
			}
			else if(disableflag){
				if(!selectedflag){
					CBitmap bitmapstandard;
					GetBitmapFromImageList(pDC,bitmap,xoffset,bitmapstandard);
					rect2.SetRect(rect.left,rect.top+dy,rect.left+m_iconX+4,
                        rect.top+m_iconY+4+dy);
					pDC->Draw3dRect (rect2,m_clrBack,m_clrBack);
					if(disable_old_style)
						DitherBlt(lpDIS->hDC,rect.left+2,rect.top+2+dy,m_iconX,m_iconY,
						(HBITMAP)(bitmapstandard),0,0,m_clrBack);
					else
						DitherBlt2(pDC,rect.left+2,rect.top+2+dy,m_iconX,m_iconY,
						bitmapstandard,0,0,m_clrBack);
					bitmapstandard.DeleteObject();
				}
			}
			else if(selectedflag){
				pDC->FillRect (rect2,&m_brBackground);
				rect2.SetRect(rect.left,rect.top+dy,rect.left+m_iconX+4,
					rect.top+m_iconY+4+dy);
				if (IsNewShell()){
					if(state&ODS_CHECKED)
						pDC->Draw3dRect(rect2,GetSysColor(COLOR_3DSHADOW),
						GetSysColor(COLOR_3DHILIGHT));
					else
						pDC->Draw3dRect(rect2,GetSysColor(COLOR_3DHILIGHT),
						GetSysColor(COLOR_3DSHADOW));
				}
				CPoint ptImage(rect.left+2,rect.top+2+dy);
				if(bitmap)bitmap->Draw(pDC,xoffset,ptImage,ILD_TRANSPARENT);
			}
			else{
				if(state&ODS_CHECKED){
					CBrush brush;
					COLORREF col =GetSysColor(COLOR_3DLIGHT);
					brush.CreateSolidBrush(col);
					pDC->FillRect(rect2,&brush);
					brush.DeleteObject();
					rect2.SetRect(rect.left,rect.top+dy,rect.left+m_iconX+4,
                        rect.top+m_iconY+4+dy);
					if (IsNewShell())
						pDC->Draw3dRect(rect2,GetSysColor(COLOR_3DSHADOW),
						GetSysColor(COLOR_3DHILIGHT));
				}
				else{
					pDC->FillRect (rect2,&m_brBackground);
					rect2.SetRect(rect.left,rect.top+dy,rect.left+m_iconX+4,
                        rect.top+m_iconY+4+dy);
					pDC->Draw3dRect (rect2,m_clrBack,m_clrBack);
				}
				CPoint ptImage(rect.left+2,rect.top+2+dy);
				if(bitmap)bitmap->Draw(pDC,xoffset,ptImage,ILD_TRANSPARENT);
			}
		}
		if(nIconNormal<0 && state&ODS_CHECKED && !checkflag){
			rect2.SetRect(rect.left+1,rect.top+2+dy,rect.left+m_iconX+1,
				rect.top+m_iconY+2+dy);
			CMenuItemInfo info;
			info.fMask = MIIM_CHECKMARKS;
			::GetMenuItemInfo((HMENU)lpDIS->hwndItem,lpDIS->itemID,
				MF_BYCOMMAND, &info);
			if(state&ODS_CHECKED || info.hbmpUnchecked) {
				Draw3DCheckmark(pDC, rect2, state&ODS_SELECTED,
					state&ODS_CHECKED ? info.hbmpChecked :
				info.hbmpUnchecked);
			}
		}
		
		//This is needed always so that we can have the space for check marks
		
		x0=rect.left;y0=rect.top;
		rect.left = rect.left + m_iconX + 8 + GAP; 
		
		if(!strText.IsEmpty()){
			
			CRect rectt(rect.left,rect.top-1,rect.right,rect.bottom-1);
			
			//   Find tabs
			
			CString leftStr,rightStr;
			leftStr.Empty();rightStr.Empty();
			int tablocr=strText.ReverseFind(_T('\t'));
			if(tablocr!=-1){
				rightStr=strText.Mid(tablocr+1);
				leftStr=strText.Left(strText.Find(_T('\t')));
				rectt.right-=m_iconX;
			}
			else leftStr=strText;
			
			int iOldMode = pDC->GetBkMode();
			pDC->SetBkMode( TRANSPARENT);
			
			// Draw the text in the correct colour:
			
			UINT nFormat  = DT_LEFT|DT_SINGLELINE|DT_VCENTER;
			UINT nFormatr = DT_RIGHT|DT_SINGLELINE|DT_VCENTER;
			if(!(lpDIS->itemState & ODS_GRAYED)){
				pDC->SetTextColor(crText);
				pDC->DrawText (leftStr,rectt,nFormat);
				if(tablocr!=-1) pDC->DrawText (rightStr,rectt,nFormatr);
			}
			else{
				
				// Draw the disabled text
				if(!(state & ODS_SELECTED)){
					RECT offset = *rectt;
					offset.left+=1;
					offset.right+=1;
					offset.top+=1;
					offset.bottom+=1;
					pDC->SetTextColor(GetSysColor(COLOR_BTNHILIGHT));
					pDC->DrawText(leftStr,&offset, nFormat);
					if(tablocr!=-1) pDC->DrawText (rightStr,&offset,nFormatr);
					pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
					pDC->DrawText(leftStr,rectt, nFormat);
					if(tablocr!=-1) pDC->DrawText (rightStr,rectt,nFormatr);
				}
				else{
					// And the standard Grey text:
					pDC->SetTextColor(m_clrBack);
					pDC->DrawText(leftStr,rectt, nFormat);
					if(tablocr!=-1) pDC->DrawText (rightStr,rectt,nFormatr);
				}
			}
			pFont = pDC->SelectObject (&dispFont);
			pDC->SetBkMode( iOldMode );
			pDC->SelectObject (pFont); //set it to the old font
		}
		
		m_penBack.DeleteObject();
		m_fontMenu.DeleteObject();
		m_brSelect.DeleteObject();
		dispFont.DeleteObject ();
	}
	m_brBackground.DeleteObject();
}

COLORREF BCMenu::LightenColor(COLORREF col,double factor)
{
	if(factor>0.0&&factor<=1.0){
		BYTE red,green,blue,lightred,lightgreen,lightblue;
		red = GetRValue(col);
		green = GetGValue(col);
		blue = GetBValue(col);
		lightred = (BYTE)((factor*(255-red)) + red);
		lightgreen = (BYTE)((factor*(255-green)) + green);
		lightblue = (BYTE)((factor*(255-blue)) + blue);
		col = RGB(lightred,lightgreen,lightblue);
	}
	return(col);
}

void BCMenu::DrawItem_WinXP (LPDRAWITEMSTRUCT lpDIS)
{
	ASSERT(lpDIS != NULL);
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
#ifdef BCMENU_USE_MEMDC
	BCMenuMemDC *pMemDC=NULL;
#endif
	CRect rect,rect2;
	UINT state = (((BCMenuData*)(lpDIS->itemData))->nFlags);
	COLORREF m_newclrBack=GetSysColor((IsNewShell())?COLOR_3DFACE:COLOR_MENU);
	COLORREF m_clrBack=GetSysColor(COLOR_MENU);
	BOOL XPflag = m_newclrBack==m_clrBack ? FALSE : TRUE;
	if(!XPflag){
		m_newclrBack=LightenColor(m_newclrBack,0.35);
		m_clrBack=RGB(255,255,255);
	}
	CBrush m_newbrBackground,m_brBackground;
	m_brBackground.CreateSolidBrush(m_clrBack);
	m_newbrBackground.CreateSolidBrush(m_newclrBack);
	
	// remove the selected bit if it's grayed out
	if(lpDIS->itemState & ODS_GRAYED&&!xp_select_disabled){
		if(lpDIS->itemState & ODS_SELECTED)lpDIS->itemState=lpDIS->itemState & ~ODS_SELECTED;
#ifdef BCMENU_USE_MEMDC
		pMemDC=new BCMenuMemDC(pDC,&lpDIS->rcItem);
		pDC = pMemDC;
#endif

	}
	
	if(state & MF_SEPARATOR){
		rect.CopyRect(&lpDIS->rcItem);
		pDC->FillRect (rect,&m_brBackground);
		rect2.SetRect(rect.left,rect.top,rect.left+m_iconX+4+GAP,rect.bottom);
		rect.top+=rect.Height()>>1;
		rect.left = rect2.right+4;
		pDC->DrawEdge(&rect,EDGE_ETCHED,BF_TOP);
		pDC->FillRect (rect2,&m_newbrBackground);
		pDC->Draw3dRect (rect2,m_newclrBack,m_newclrBack);
	}
	else{
		BOOL standardflag=FALSE,selectedflag=FALSE,disableflag=FALSE;
		BOOL checkflag=FALSE;
		COLORREF crText = GetSysColor(COLOR_MENUTEXT);
		COLORREF crSelect = GetSysColor(COLOR_HIGHLIGHT);
		COLORREF crSelectFill;
		if(!XPflag)crSelectFill=LightenColor(crSelect,0.85);
		else crSelectFill=LightenColor(crSelect,0.7);
		CBrush m_brSelect;
		CPen m_penBack;
		int x0,y0,dy;
		int nIconNormal=-1,xoffset=-1;
		CImageList *bitmap=NULL;
		CFont m_fontMenu;
		LOGFONT m_lf;
		
		// set some colors and the font
		m_penBack.CreatePen (PS_SOLID,0,m_clrBack);
		m_brSelect.CreateSolidBrush(crSelectFill);
		ZeroMemory ((PVOID) &m_lf,sizeof (LOGFONT));
		NONCLIENTMETRICS nm;
		nm.cbSize = sizeof (NONCLIENTMETRICS);
		VERIFY (SystemParametersInfo(SPI_GETNONCLIENTMETRICS,nm.cbSize,&nm,0)); 
		m_lf =  nm.lfMenuFont;
		m_fontMenu.CreateFontIndirect (&m_lf);
		
		// draw the colored rectangle portion
		
		rect.CopyRect(&lpDIS->rcItem);
		rect2=rect;
		
		// draw the up/down/focused/disabled state
		
		UINT state = lpDIS->itemState;
		CString strText;
		LOGFONT lf;
		lf = m_lf;
		
		CFont dispFont;
		CFont *pFont=NULL;
		
		if(lpDIS->itemData != NULL){
			nIconNormal = (((BCMenuData*)(lpDIS->itemData))->menuIconNormal);
			xoffset = (((BCMenuData*)(lpDIS->itemData))->xoffset);
			bitmap = (((BCMenuData*)(lpDIS->itemData))->bitmap);
			strText = ((BCMenuData*) (lpDIS->itemData))->GetString();
			
			if(state&ODS_CHECKED && nIconNormal<0){
				if(state&ODS_SELECTED && m_selectcheck>0)checkflag=TRUE;
				else if(m_unselectcheck>0) checkflag=TRUE;
			}
			else if(nIconNormal != -1){
				standardflag=TRUE;
				if(state&ODS_SELECTED && !(state&ODS_GRAYED))selectedflag=TRUE;
				else if(state&ODS_GRAYED) disableflag=TRUE;
			}
		}
		else{
			strText.Empty();
		}
		
		if(state&ODS_SELECTED){ // draw the down edges
			
			CPen *pOldPen = pDC->SelectObject (&m_penBack);
			
			pDC->FillRect (rect,&m_brSelect);
			pDC->Draw3dRect (rect,crSelect,crSelect);
			
			pDC->SelectObject (pOldPen);
			if((HFONT)dispFont != NULL)dispFont.DeleteObject ();
			dispFont.CreateFontIndirect (&lf);
		}
		else {
			rect2.SetRect(rect.left,rect.top,rect.left+m_iconX+4+GAP,rect.bottom);
			CPen *pOldPen = pDC->SelectObject (&m_penBack);
			pDC->FillRect (rect,&m_brBackground);
			pDC->FillRect (rect2,&m_newbrBackground);
			pDC->SelectObject (pOldPen);
			
			// draw the up edges
			
			pDC->Draw3dRect (rect,m_clrBack,m_clrBack);
			pDC->Draw3dRect (rect2,m_newclrBack,m_newclrBack);
			if ((HFONT)dispFont != NULL) dispFont.DeleteObject ();
			dispFont.CreateFontIndirect (&lf); //Normal
		}
		
		// draw the text if there is any
		//We have to paint the text only if the image is nonexistant
		
		dy = (rect.Height()-4-m_iconY)/2;
		dy = dy<0 ? 0 : dy;
		
		if(checkflag||standardflag||selectedflag||disableflag){
			rect2.SetRect(rect.left+1,rect.top+1+dy,rect.left+m_iconX+3,
				rect.top+m_iconY+3+dy);
			pDC->Draw3dRect (rect2,m_newclrBack,m_newclrBack);
			if(checkflag && checkmaps){
				pDC->FillRect (rect2,&m_newbrBackground);
				rect2.SetRect(rect.left,rect.top+dy,rect.left+m_iconX+4,
					rect.top+m_iconY+4+dy);
				
				pDC->Draw3dRect (rect2,m_newclrBack,m_newclrBack);
				CPoint ptImage(rect.left+3,rect.top+2+dy);
				
				if(state&ODS_SELECTED)checkmaps->Draw(pDC,1,ptImage,ILD_TRANSPARENT);
				else checkmaps->Draw(pDC,0,ptImage,ILD_TRANSPARENT);
			}
			else if(disableflag){
				if(!selectedflag){
					CBitmap bitmapstandard;
					GetBitmapFromImageList(pDC,bitmap,xoffset,bitmapstandard);
					rect2.SetRect(rect.left,rect.top+dy,rect.left+m_iconX+4,
                        rect.top+m_iconY+4+dy);
					pDC->Draw3dRect (rect2,m_newclrBack,m_newclrBack);
					COLORREF transparentcol=m_newclrBack;
					if(state&ODS_SELECTED)transparentcol=crSelectFill;
					if(disable_old_style)
						DitherBlt(lpDIS->hDC,rect.left+2,rect.top+2+dy,m_iconX,m_iconY,
						(HBITMAP)(bitmapstandard),0,0,transparentcol);
					else
						DitherBlt2(pDC,rect.left+2,rect.top+2+dy,m_iconX,m_iconY,
						bitmapstandard,0,0,transparentcol);
					if(state&ODS_SELECTED)pDC->Draw3dRect (rect,crSelect,crSelect);
					bitmapstandard.DeleteObject();
				}
			}
			else if(selectedflag){
				pDC->FillRect (rect2,&m_brSelect);
				if (IsNewShell() && state&ODS_CHECKED){
					rect2.SetRect(rect.left,rect.top,rect.left+m_iconX+GAP+3,rect.bottom);
					pDC->Draw3dRect(rect2,crSelect,crSelect);
				}
				CPoint ptImage(rect.left+3,rect.top+2+dy);
				if(bitmap)bitmap->Draw(pDC,xoffset,ptImage,ILD_TRANSPARENT);
			}
			else{
				if(state&ODS_CHECKED){
					if(!XPflag)rect2.SetRect(rect.left+1,rect.top,rect.left+m_iconX+GAP+3,rect.bottom);
					else rect2.SetRect(rect.left+1,rect.top,rect.left+m_iconX+GAP+3,rect.bottom-1);
					CBrush brushin;
					brushin.CreateSolidBrush(LightenColor(crSelect,0.85));
					pDC->FillRect(rect2,&brushin);
					brushin.DeleteObject();
					pDC->Draw3dRect(rect2,crSelect,crSelect);
				}
				else{
					pDC->FillRect (rect2,&m_newbrBackground);
					rect2.SetRect(rect.left,rect.top+dy,rect.left+m_iconX+4,
                        rect.top+m_iconY+4+dy);
					pDC->Draw3dRect (rect2,m_newclrBack,m_newclrBack);
				}
				CPoint ptImage(rect.left+3,rect.top+2+dy);
				if(bitmap)bitmap->Draw(pDC,xoffset,ptImage,ILD_TRANSPARENT);
			}
		}
		if(nIconNormal<0 && state&ODS_CHECKED && !checkflag){
			CMenuItemInfo info;
			info.fMask = MIIM_CHECKMARKS;
			::GetMenuItemInfo((HMENU)lpDIS->hwndItem,lpDIS->itemID,
				MF_BYCOMMAND, &info);
			if(state&ODS_CHECKED || info.hbmpUnchecked) {
				if(XPflag)rect2.SetRect(rect.left+2,rect.top+2+dy,rect.left+m_iconX+2,rect.top+m_iconY+3+dy);
				else rect2.SetRect(rect.left+2,rect.top+2+dy,rect.left+m_iconX+2,rect.top+m_iconY+2+dy);
				DrawXPCheckmark(pDC, rect2,state&ODS_CHECKED ? info.hbmpChecked :
				info.hbmpUnchecked,crSelect);
			}
		}
		
		//This is needed always so that we can have the space for check marks
		
		x0=rect.left;y0=rect.top;
		rect.left = rect.left + m_iconX + 8 + GAP; 
		
		if(!strText.IsEmpty()){
			
			CRect rectt(rect.left,rect.top-1,rect.right,rect.bottom-1);
			
			//   Find tabs
			
			CString leftStr,rightStr;
			leftStr.Empty();rightStr.Empty();
			int tablocr=strText.ReverseFind(_T('\t'));
			if(tablocr!=-1){
				rightStr=strText.Mid(tablocr+1);
				leftStr=strText.Left(strText.Find(_T('\t')));
				rectt.right-=m_iconX;
			}
			else leftStr=strText;
			
			int iOldMode = pDC->GetBkMode();
			pDC->SetBkMode( TRANSPARENT);
			
			// Draw the text in the correct colour:
			
			pFont = pDC->SelectObject (&dispFont);
			UINT nFormat  = DT_LEFT|DT_SINGLELINE|DT_VCENTER;
			UINT nFormatr = DT_RIGHT|DT_SINGLELINE|DT_VCENTER;
			if(!(lpDIS->itemState & ODS_GRAYED)){
				pDC->SetTextColor(crText);
				pDC->DrawText (leftStr,rectt,nFormat);
				if(tablocr!=-1) pDC->DrawText (rightStr,rectt,nFormatr);
			}
			else{
				RECT offset = *rectt;
				offset.left+=1;
				offset.right+=1;
				offset.top+=1;
				offset.bottom+=1;
				if(!XPflag){
					COLORREF graycol=GetSysColor(COLOR_GRAYTEXT);
					graycol = LightenColor(graycol,0.55);
					pDC->SetTextColor(graycol);
				}
				else pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
				pDC->DrawText(leftStr,rectt, nFormat);
				if(tablocr!=-1) pDC->DrawText (rightStr,rectt,nFormatr);
			}
			pDC->SetBkMode( iOldMode );
			pDC->SelectObject (pFont); //set it to the old font
		}
		
		m_penBack.DeleteObject();
		m_fontMenu.DeleteObject();
		m_brSelect.DeleteObject();
		dispFont.DeleteObject ();
	}
	m_brBackground.DeleteObject();
	m_newbrBackground.DeleteObject();
#ifdef BCMENU_USE_MEMDC
	if(pMemDC)delete pMemDC;
#endif
}

BOOL BCMenu::GetBitmapFromImageList(CDC* pDC,CImageList *imglist,int nIndex,CBitmap &bmp)
{
	HICON hIcon = imglist->ExtractIcon(nIndex);
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC,m_iconX,m_iconY);
	CBitmap* pOldBmp = dc.SelectObject(&bmp);
	CBrush brush ;
	brush.CreateSolidBrush(GetSysColor(COLOR_MENU));
	::DrawIconEx(
		dc.GetSafeHdc(),
		0,
		0,
		hIcon,
		m_iconX,
		m_iconY,
		0,
		(HBRUSH)brush,
		DI_NORMAL
		);
	dc.SelectObject( pOldBmp );
	dc.DeleteDC();
	// the icon is not longer needed
	DestroyIcon(hIcon);
	return(TRUE);
}

/*
==========================================================================
void BCMenu::MeasureItem(LPMEASUREITEMSTRUCT)
---------------------------------------------

  Called by the framework when it wants to know what the width and height
  of our item will be.  To accomplish this we provide the width of the
  icon plus the width of the menu text, and then the height of the icon.
  
	==========================================================================
*/

void BCMenu::MeasureItem( LPMEASUREITEMSTRUCT lpMIS )
{
	UINT state = (((BCMenuData*)(lpMIS->itemData))->nFlags);
	if(state & MF_SEPARATOR){
		lpMIS->itemWidth = 0;
		lpMIS->itemHeight = GetSystemMetrics(SM_CYMENU)>>1;
	}
	else{
		CFont m_fontMenu;
		LOGFONT m_lf;
		ZeroMemory ((PVOID) &m_lf,sizeof (LOGFONT));
		NONCLIENTMETRICS nm;
		nm.cbSize = sizeof (NONCLIENTMETRICS);
		VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
			nm.cbSize,&nm,0)); 
		m_lf =  nm.lfMenuFont;
		m_fontMenu.CreateFontIndirect (&m_lf);
		
		// Obtain the width of the text:
		CWnd *pWnd = AfxGetMainWnd();            // Get main window
		CDC *pDC = pWnd->GetDC();              // Get device context
		CFont* pFont=NULL;    // Select menu font in...
		
		if (IsNewShell())
			pFont = pDC->SelectObject (&m_fontMenu);// Select menu font in...
        
		//Get pointer to text SK
		const wchar_t *lpstrText = ((BCMenuData*)(lpMIS->itemData))->GetWideString();//SK: we use const to prevent misuse
		
        
		SIZE size;
		
		if (Win32s!=g_Shell)
			VERIFY(::GetTextExtentPoint32W(pDC->m_hDC,lpstrText,
			wcslen(lpstrText),&size)); //SK should also work on 95
#ifndef UNICODE //can't be UNICODE for Win32s
		else{//it's Win32suckx
			RECT rect;
			rect.left=rect.top=0;
			size.cy=DrawText(pDC->m_hDC,(LPCTSTR)lpstrText,
				wcslen(lpstrText),&rect,
				DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_CALCRECT);
			//+3 makes at least three pixels space to the menu border
			size.cx=rect.right-rect.left+3;
			size.cx += 3*(size.cx/wcslen(lpstrText));
		}
#endif    
		
		CSize t = CSize(size);
		if(IsNewShell())
			pDC->SelectObject (pFont);  // Select old font in
		AfxGetMainWnd()->ReleaseDC(pDC);  // Release the DC
		
		// Set width and height:
		
		lpMIS->itemWidth = m_iconX + t.cx + m_iconX + GAP;
		int temp = GetSystemMetrics(SM_CYMENU);
		lpMIS->itemHeight = temp>m_iconY+4 ? temp : m_iconY+4;
		m_fontMenu.DeleteObject();
	}
}

void BCMenu::SetIconSize (int width, int height)
{
	m_iconX = width;
	m_iconY = height;
}

BOOL BCMenu::AppendODMenuA(LPCSTR lpstrText,UINT nFlags,UINT nID,
                           int nIconNormal)
{
	USES_CONVERSION;
	return AppendODMenuW(A2W(lpstrText),nFlags,nID,nIconNormal);//SK: See MFC Tech Note 059
}


BOOL BCMenu::AppendODMenuW(wchar_t *lpstrText,UINT nFlags,UINT nID,
                           int nIconNormal)
{
	// Add the MF_OWNERDRAW flag if not specified:
	if(!nID){
		if(nFlags&MF_BYPOSITION)nFlags=MF_SEPARATOR|MF_OWNERDRAW|MF_BYPOSITION;
		else nFlags=MF_SEPARATOR|MF_OWNERDRAW;
	}
	else if(!(nFlags & MF_OWNERDRAW))nFlags |= MF_OWNERDRAW;
	
	if(nFlags & MF_POPUP){
		m_AllSubMenus.Add((HMENU)nID);
		m_SubMenus.Add((HMENU)nID);
	}
	
	BCMenuData *mdata = new BCMenuData;
	m_MenuList.Add(mdata);
	mdata->SetWideString(lpstrText);    //SK: modified for dynamic allocation
	
	mdata->menuIconNormal = nIconNormal;
	mdata->xoffset=-1;
	if(nIconNormal>=0){
		mdata->xoffset=0;
		LoadFromToolBar(nID,nIconNormal,mdata->xoffset);
		if(mdata->bitmap)mdata->bitmap->DeleteImageList();
		else mdata->bitmap=new(CImageList);
		mdata->bitmap->Create(m_iconX,m_iconY,ILC_COLORDDB|ILC_MASK,1,1);
		if(!AddBitmapToImageList(mdata->bitmap,nIconNormal)){
			mdata->bitmap->DeleteImageList();
			delete mdata->bitmap;
			mdata->bitmap=NULL;
			mdata->menuIconNormal = nIconNormal = -1;
			mdata->xoffset = -1;
		}
	}
	mdata->nFlags = nFlags;
	mdata->nID = nID;
	return(CMenu::AppendMenu(nFlags, nID, (LPCTSTR)mdata));
}

BOOL BCMenu::AppendODMenuA(LPCSTR lpstrText,UINT nFlags,UINT nID,
                           CImageList *il,int xoffset)
{
	USES_CONVERSION;
	return AppendODMenuW(A2W(lpstrText),nFlags,nID,il,xoffset);
}

BOOL BCMenu::AppendODMenuW(wchar_t *lpstrText,UINT nFlags,UINT nID,
                           CImageList *il,int xoffset)
{
	// Add the MF_OWNERDRAW flag if not specified:
	if(!nID){
		if(nFlags&MF_BYPOSITION)nFlags=MF_SEPARATOR|MF_OWNERDRAW|MF_BYPOSITION;
		else nFlags=MF_SEPARATOR|MF_OWNERDRAW;
	}
	else if(!(nFlags & MF_OWNERDRAW))nFlags |= MF_OWNERDRAW;
	
	if(nFlags & MF_POPUP){
		m_AllSubMenus.Add((HMENU)nID);
		m_SubMenus.Add((HMENU)nID);
	}
	
	BCMenuData *mdata = new BCMenuData;
	m_MenuList.Add(mdata);
	mdata->SetWideString(lpstrText);    //SK: modified for dynamic allocation
	
	if(il){
		mdata->menuIconNormal = 0;
		mdata->xoffset=0;
		if(mdata->bitmap)mdata->bitmap->DeleteImageList();
		else mdata->bitmap=new(CImageList);
		ImageListDuplicate(il,xoffset,mdata->bitmap);
	}
	else{
		mdata->menuIconNormal = -1;
		mdata->xoffset = -1;
	}
	mdata->nFlags = nFlags;
	mdata->nID = nID;
	return(CMenu::AppendMenu(nFlags, nID, (LPCTSTR)mdata));
}

BOOL BCMenu::InsertODMenuA(UINT nPosition,LPCSTR lpstrText,UINT nFlags,UINT nID,
                           int nIconNormal)
{
	USES_CONVERSION;
	return InsertODMenuW(nPosition,A2W(lpstrText),nFlags,nID,nIconNormal);
}


BOOL BCMenu::InsertODMenuW(UINT nPosition,wchar_t *lpstrText,UINT nFlags,UINT nID,
                           int nIconNormal)
{
	if(!(nFlags & MF_BYPOSITION)){
		int iPosition =0;
		BCMenu* pMenu = FindMenuOption(nPosition,iPosition);
		if(pMenu){
			return(pMenu->InsertODMenuW(iPosition,lpstrText,nFlags|MF_BYPOSITION,nID,nIconNormal));
		}
		else return(FALSE);
	}
	
	if(!nID)nFlags=MF_SEPARATOR|MF_OWNERDRAW|MF_BYPOSITION;
	else if(!(nFlags & MF_OWNERDRAW))nFlags |= MF_OWNERDRAW;
	
	if(nFlags & MF_POPUP){
		m_AllSubMenus.Add((HMENU)nID);
		m_SubMenus.InsertAt(nPosition,(HMENU)nID);
	}

	//Stephane Clog suggested adding this, believe it or not it's in the help 
	if(nPosition==(UINT)-1)nPosition=GetMenuItemCount();
	
	BCMenuData *mdata = new BCMenuData;
	m_MenuList.InsertAt(nPosition,mdata);
	mdata->SetWideString(lpstrText);    //SK: modified for dynamic allocation
	
	mdata->menuIconNormal = nIconNormal;
	mdata->xoffset=-1;
	if(nIconNormal>=0){
		mdata->xoffset=0;
		LoadFromToolBar(nID,nIconNormal,mdata->xoffset);
		if(mdata->bitmap)mdata->bitmap->DeleteImageList();
		else mdata->bitmap=new(CImageList);
		mdata->bitmap->Create(m_iconX,m_iconY,ILC_COLORDDB|ILC_MASK,1,1);
		if(!AddBitmapToImageList(mdata->bitmap,nIconNormal)){
			mdata->bitmap->DeleteImageList();
			delete mdata->bitmap;
			mdata->bitmap=NULL;
			mdata->menuIconNormal = nIconNormal = -1;
			mdata->xoffset = -1;
		}
	}
	mdata->nFlags = nFlags;
	mdata->nID = nID;
	return(CMenu::InsertMenu(nPosition,nFlags,nID,(LPCTSTR)mdata));
}

BOOL BCMenu::InsertODMenuA(UINT nPosition,LPCSTR lpstrText,UINT nFlags,UINT nID,
                           CImageList *il,int xoffset)
{
	USES_CONVERSION;
	return InsertODMenuW(nPosition,A2W(lpstrText),nFlags,nID,il,xoffset);
}

BOOL BCMenu::InsertODMenuW(UINT nPosition,wchar_t *lpstrText,UINT nFlags,UINT nID,
                           CImageList *il,int xoffset)
{
	if(!(nFlags & MF_BYPOSITION)){
		int iPosition =0;
		BCMenu* pMenu = FindMenuOption(nPosition,iPosition);
		if(pMenu){
			return(pMenu->InsertODMenuW(iPosition,lpstrText,nFlags|MF_BYPOSITION,nID,il,xoffset));
		}
		else return(FALSE);
	}
	
	if(!nID)nFlags=MF_SEPARATOR|MF_OWNERDRAW|MF_BYPOSITION;
	else if(!(nFlags & MF_OWNERDRAW))nFlags |= MF_OWNERDRAW;
	
	if(nFlags & MF_POPUP){
		m_AllSubMenus.Add((HMENU)nID);
		m_SubMenus.InsertAt(nPosition,(HMENU)nID);
	}
	
	//Stephane Clog suggested adding this, believe it or not it's in the help 
	if(nPosition==(UINT)-1)nPosition=GetMenuItemCount();
	
	BCMenuData *mdata = new BCMenuData;
	m_MenuList.InsertAt(nPosition,mdata);
	mdata->SetWideString(lpstrText);    //SK: modified for dynamic allocation
	
	if(il){
		mdata->menuIconNormal = 0;
		mdata->xoffset=0;
		if(mdata->bitmap)mdata->bitmap->DeleteImageList();
		else mdata->bitmap=new(CImageList);
		ImageListDuplicate(il,xoffset,mdata->bitmap);
	}
	else{
		mdata->menuIconNormal = -1;
		mdata->xoffset = -1;
	}
	mdata->nFlags = nFlags;
	mdata->nID = nID;
	return(CMenu::InsertMenu(nPosition,nFlags,nID,(LPCTSTR)mdata));
}

BOOL BCMenu::ModifyODMenuA(const char * lpstrText,UINT nID,int nIconNormal)
{
	USES_CONVERSION;
	return ModifyODMenuW(A2W(lpstrText),nID,nIconNormal);//SK: see MFC Tech Note 059
}

BOOL BCMenu::ModifyODMenuW(wchar_t *lpstrText,UINT nID,int nIconNormal)
{
	int nLoc;
	BCMenuData *mdata;
	CArray<BCMenu*,BCMenu*>bcsubs;
	CArray<int,int&>bclocs;
	
	// Find the old BCMenuData structure:
	BCMenu *psubmenu = FindMenuOption(nID,nLoc);
	do{
		if(psubmenu && nLoc>=0)mdata = psubmenu->m_MenuList[nLoc];
		else{
			// Create a new BCMenuData structure:
			mdata = new BCMenuData;
			m_MenuList.Add(mdata);
		}
		
		ASSERT(mdata);
		if(lpstrText)
			mdata->SetWideString(lpstrText);  //SK: modified for dynamic allocation
		mdata->menuIconNormal = nIconNormal;
		mdata->xoffset=-1;
		if(nIconNormal>=0){
			mdata->xoffset=0;
			LoadFromToolBar(nID,nIconNormal,mdata->xoffset);
			if(mdata->bitmap)mdata->bitmap->DeleteImageList();
			else mdata->bitmap=new CImageList;
			mdata->bitmap->Create(m_iconX,m_iconY,ILC_COLORDDB|ILC_MASK,1,1);
			if(!AddBitmapToImageList(mdata->bitmap,nIconNormal)){
				mdata->bitmap->DeleteImageList();
				delete mdata->bitmap;
				mdata->bitmap=NULL;
				mdata->menuIconNormal = nIconNormal = -1;
				mdata->xoffset = -1;
			}
		}
		mdata->nFlags &= ~(MF_BYPOSITION);
		mdata->nFlags |= MF_OWNERDRAW;
		mdata->nID = nID;
		bcsubs.Add(psubmenu);
		bclocs.Add(nLoc);
		if(psubmenu && nLoc>=0)psubmenu = FindAnotherMenuOption(nID,nLoc,bcsubs,bclocs);
		else psubmenu=NULL;
	}while(psubmenu);
	return (CMenu::ModifyMenu(nID,mdata->nFlags,nID,(LPCTSTR)mdata));
}

BOOL BCMenu::ModifyODMenuA(const char * lpstrText,UINT nID,CImageList *il,int xoffset)
{
	USES_CONVERSION;
	return ModifyODMenuW(A2W(lpstrText),nID,il,xoffset);
}

BOOL BCMenu::ModifyODMenuW(wchar_t *lpstrText,UINT nID,CImageList *il,int xoffset)
{
	int nLoc;
	BCMenuData *mdata;
	CArray<BCMenu*,BCMenu*>bcsubs;
	CArray<int,int&>bclocs;
	
	// Find the old BCMenuData structure:
	BCMenu *psubmenu = FindMenuOption(nID,nLoc);
	do{
		if(psubmenu && nLoc>=0)mdata = psubmenu->m_MenuList[nLoc];
		else{
			// Create a new BCMenuData structure:
			mdata = new BCMenuData;
			m_MenuList.Add(mdata);
		}
		
		ASSERT(mdata);
		if(lpstrText)
			mdata->SetWideString(lpstrText);  //SK: modified for dynamic allocation
		if(il){
			mdata->menuIconNormal = 0;
			mdata->xoffset=0;
			if(mdata->bitmap)mdata->bitmap->DeleteImageList();
			else mdata->bitmap=new(CImageList);
			ImageListDuplicate(il,xoffset,mdata->bitmap);
		}
		else{
			mdata->menuIconNormal = -1;
			mdata->xoffset = -1;
		}
		mdata->nFlags &= ~(MF_BYPOSITION);
		mdata->nFlags |= MF_OWNERDRAW;
		mdata->nID = nID;
		bcsubs.Add(psubmenu);
		bclocs.Add(nLoc);
		if(psubmenu && nLoc>=0)psubmenu = FindAnotherMenuOption(nID,nLoc,bcsubs,bclocs);
		else psubmenu=NULL;
	}while(psubmenu);
	return (CMenu::ModifyMenu(nID,mdata->nFlags,nID,(LPCTSTR)mdata));
}

BOOL BCMenu::ModifyODMenuA(const char * lpstrText,UINT nID,CBitmap *bmp)
{
	USES_CONVERSION;
	return ModifyODMenuW(A2W(lpstrText),nID,bmp);
}

BOOL BCMenu::ModifyODMenuW(wchar_t *lpstrText,UINT nID,CBitmap *bmp)
{
	if(bmp){
		CImageList temp;
		temp.Create(m_iconX,m_iconY,ILC_COLORDDB|ILC_MASK,1,1);
		if(m_bitmapBackgroundFlag)temp.Add(bmp,m_bitmapBackground);
		else temp.Add(bmp,GetSysColor(COLOR_3DFACE));
		return ModifyODMenuW(lpstrText,nID,&temp,0);
	}
	return ModifyODMenuW(lpstrText,nID,NULL,0);
}

// courtesy of Warren Stevens
BOOL BCMenu::ModifyODMenuA(const char * lpstrText,UINT nID,COLORREF fill,COLORREF border,int hatchstyle)
{
	USES_CONVERSION;
	return ModifyODMenuW(A2W(lpstrText),nID,fill,border,hatchstyle);
}

BOOL BCMenu::ModifyODMenuW(wchar_t *lpstrText,UINT nID,COLORREF fill,COLORREF border,int hatchstyle)
{
	CWnd *pWnd = AfxGetMainWnd();            // Get main window
	CDC *pDC = pWnd->GetDC();              // Get device context
	CSize bitmap_size(m_iconX,m_iconY);
	CBitmap bmp;
	ColorBitmap(pDC,bmp,bitmap_size,fill,border,hatchstyle);	
	return ModifyODMenuW(lpstrText,nID,&bmp);
}


BOOL BCMenu::ModifyODMenuA(const char *lpstrText,const char *OptionText,
                           int nIconNormal)
{
	USES_CONVERSION;
	return ModifyODMenuW(A2W(lpstrText),A2W(OptionText),nIconNormal);//SK: see MFC  Tech Note 059
}

BOOL BCMenu::ModifyODMenuW(wchar_t *lpstrText,wchar_t *OptionText,
                           int nIconNormal)
{
	BCMenuData *mdata;
	
	// Find the old BCMenuData structure:
	CString junk=OptionText;
	mdata=FindMenuOption(OptionText);
	if(mdata){
		if(lpstrText)
			mdata->SetWideString(lpstrText);//SK: modified for dynamic allocation
		mdata->menuIconNormal = nIconNormal;
		mdata->xoffset=-1;
		if(nIconNormal>=0){
			mdata->xoffset=0;
			if(mdata->bitmap)mdata->bitmap->DeleteImageList();
			else mdata->bitmap=new(CImageList);
			mdata->bitmap->Create(m_iconX,m_iconY,ILC_COLORDDB|ILC_MASK,1,1);
			if(!AddBitmapToImageList(mdata->bitmap,nIconNormal)){
				mdata->bitmap->DeleteImageList();
				delete mdata->bitmap;
				mdata->bitmap=NULL;
				mdata->menuIconNormal = nIconNormal = -1;
				mdata->xoffset = -1;
			}
		}
		return(TRUE);
	}
	return(FALSE);
}

BCMenuData *BCMenu::NewODMenu(UINT pos,UINT nFlags,UINT nID,CString string)
{
	BCMenuData *mdata;
	
	mdata = new BCMenuData;
	mdata->menuIconNormal = -1;
	mdata->xoffset=-1;
#ifdef UNICODE
	mdata->SetWideString((LPCTSTR)string);//SK: modified for dynamic allocation
#else
	mdata->SetAnsiString(string);
#endif
	mdata->nFlags = nFlags;
	mdata->nID = nID;
	
//	if(nFlags & MF_POPUP)m_AllSubMenus.Add((HMENU)nID);
		
	if (nFlags&MF_OWNERDRAW){
		ASSERT(!(nFlags&MF_STRING));
		ModifyMenu(pos,nFlags,nID,(LPCTSTR)mdata);
	}
	else if (nFlags&MF_STRING){
		ASSERT(!(nFlags&MF_OWNERDRAW));
		ModifyMenu(pos,nFlags,nID,mdata->GetString());
	}
	else{
		ASSERT(nFlags&MF_SEPARATOR);
		ModifyMenu(pos,nFlags,nID);
	}
	
	return(mdata);
};

BOOL BCMenu::LoadToolbars(const UINT *arID,int n)
{
	ASSERT(arID);
	BOOL returnflag=TRUE;
	for(int i=0;i<n;++i){
		if(!LoadToolbar(arID[i]))returnflag=FALSE;
	}
	return(returnflag);
}

BOOL BCMenu::LoadToolbar(UINT nToolBar)
{
	UINT nID,nStyle;
	BOOL returnflag=FALSE;
	CToolBar bar;
	int xoffset=-1,xset;
	
	bar.Create(AfxGetMainWnd());
	if(bar.LoadToolBar(nToolBar)){
		CImageList imglist;
		imglist.Create(m_iconX,m_iconY,ILC_COLORDDB|ILC_MASK,1,1);
		if(AddBitmapToImageList(&imglist,nToolBar)){
			returnflag=TRUE;
			for(int i=0;i<bar.GetCount();++i){
				nID = bar.GetItemID(i); 
				if(nID && GetMenuState(nID, MF_BYCOMMAND)
					!=0xFFFFFFFF){
					xoffset=bar.CommandToIndex(nID);
					if(xoffset>=0){
						bar.GetButtonInfo(xoffset,nID,nStyle,xset);
						if(xset>0)xoffset=xset;
					}
					ModifyODMenu(NULL,nID,&imglist,xoffset);
				}
			}
		}
	}
	return(returnflag);
}

BOOL BCMenu::LoadFromToolBar(UINT nID,UINT nToolBar,int& xoffset)
{
	int xset,offset;
	UINT nStyle;
	BOOL returnflag=FALSE;
	CToolBar bar;
	
	bar.Create(AfxGetMainWnd());
	if(bar.LoadToolBar(nToolBar)){
		offset=bar.CommandToIndex(nID);
		if(offset>=0){
			bar.GetButtonInfo(offset,nID,nStyle,xset);
			if(xset>0)xoffset=xset;
			returnflag=TRUE;
		}
	}
	return(returnflag);
}

// O.S.
BCMenuData *BCMenu::FindMenuItem(UINT nID)
{
	BCMenuData *pData = NULL;
	int i;
	
	for(i = 0; i <= m_MenuList.GetUpperBound(); i++){
		if (m_MenuList[i]->nID == nID){
			pData = m_MenuList[i];
			break;
		}
	}
	if (!pData){
		int loc;
		BCMenu *pMenu = FindMenuOption(nID, loc);
		ASSERT(pMenu != this);
		if (loc >= 0){
			return pMenu->FindMenuItem(nID);
		}
	}
	return pData;
}


BCMenu *BCMenu::FindAnotherMenuOption(int nId,int& nLoc,CArray<BCMenu*,BCMenu*>&bcsubs,
									  CArray<int,int&>&bclocs)
{
	int i,numsubs,j;
	BCMenu *psubmenu,*pgoodmenu;
	BOOL foundflag;
	
	for(i=0;i<(int)(GetMenuItemCount());++i){
#ifdef _CPPRTTI 
		psubmenu=dynamic_cast<BCMenu *>(GetSubMenu(i));
#else
		psubmenu=(BCMenu *)GetSubMenu(i);
#endif
		if(psubmenu){
			pgoodmenu=psubmenu->FindAnotherMenuOption(nId,nLoc,bcsubs,bclocs);
			if(pgoodmenu)return(pgoodmenu);
		}
		else if(nId==(int)GetMenuItemID(i)){
			numsubs=bcsubs.GetSize();
			foundflag=TRUE;
			for(j=0;j<numsubs;++j){
				if(bcsubs[j]==this&&bclocs[j]==i){
					foundflag=FALSE;
					break;
				}
			}
			if(foundflag){
				nLoc=i;
				return(this);
			}
		}
	}
	nLoc = -1;
	return(NULL);
}

BCMenu *BCMenu::FindMenuOption(int nId,int& nLoc)
{
	int i;
	BCMenu *psubmenu,*pgoodmenu;
	
	for(i=0;i<(int)(GetMenuItemCount());++i){
#ifdef _CPPRTTI 
		psubmenu=dynamic_cast<BCMenu *>(GetSubMenu(i));
#else
		psubmenu=(BCMenu *)GetSubMenu(i);
#endif
		if(psubmenu){
			pgoodmenu=psubmenu->FindMenuOption(nId,nLoc);
			if(pgoodmenu)return(pgoodmenu);
		}
		else if(nId==(int)GetMenuItemID(i)){
			nLoc=i;
			return(this);
		}
	}
	nLoc = -1;
	return(NULL);
}

BCMenuData *BCMenu::FindMenuOption(wchar_t *lpstrText)
{
	int i,j;
	BCMenu *psubmenu;
	BCMenuData *pmenulist;
	
	for(i=0;i<(int)(GetMenuItemCount());++i){
#ifdef _CPPRTTI 
		psubmenu=dynamic_cast<BCMenu *>(GetSubMenu(i));
#else
		psubmenu=(BCMenu *)GetSubMenu(i);
#endif
		if(psubmenu){
			pmenulist=psubmenu->FindMenuOption(lpstrText);
			if(pmenulist)return(pmenulist);
		}
		else{
			const wchar_t *szWide;//SK: we use const to prevent misuse of this Ptr
			for(j=0;j<=m_MenuList.GetUpperBound();++j){     
				szWide = m_MenuList[j]->GetWideString ();
				if(szWide && !wcscmp(lpstrText,szWide))//SK: modified for dynamic allocation
					return(m_MenuList[j]);
			}
		}
	}
	return(NULL);
}


BOOL BCMenu::LoadMenu(int nResource)
{
	return(BCMenu::LoadMenu(MAKEINTRESOURCE(nResource)));
};

BOOL BCMenu::LoadMenu(LPCTSTR lpszResourceName)
{
	TRACE(_T(
		"IMPORTANT:Use BCMenu::DestroyMenu to destroy Loaded Menu's\n"));
	ASSERT_VALID(this);
	ASSERT(lpszResourceName != NULL);
	
	// Find the Menu Resource:
	HINSTANCE m_hInst = AfxFindResourceHandle(lpszResourceName,RT_MENU);
	HRSRC hRsrc = ::FindResource(m_hInst,lpszResourceName,RT_MENU);
	if(hRsrc == NULL)return FALSE;
	
	// Load the Menu Resource:
	
	HGLOBAL hGlobal = LoadResource(m_hInst, hRsrc);
	if(hGlobal == NULL)return FALSE;
	
	// Attempt to create us as a menu...
	
	if(!CMenu::CreateMenu())return FALSE;
	
	// Get Item template Header, and calculate offset of MENUITEMTEMPLATES
	
	MENUITEMTEMPLATEHEADER *pTpHdr=
		(MENUITEMTEMPLATEHEADER*)LockResource(hGlobal);
	BYTE* pTp=(BYTE*)pTpHdr + 
		(sizeof(MENUITEMTEMPLATEHEADER) + pTpHdr->offset);
	
	
	// Variables needed during processing of Menu Item Templates:
	
	int j=0;
	WORD    dwFlags = 0;              // Flags of the Menu Item
	WORD    dwID  = 0;              // ID of the Menu Item
	UINT    uFlags;                  // Actual Flags.
	wchar_t *szCaption=NULL;
	int      nLen   = 0;                // Length of caption
	CTypedPtrArray<CPtrArray, BCMenu*>  m_Stack;    // Popup menu stack
	CArray<BOOL,BOOL>  m_StackEnd;    // Popup menu stack
	m_Stack.Add(this);                  // Add it to this...
	m_StackEnd.Add(FALSE);
	
	do{
		// Obtain Flags and (if necessary), the ID...
		memcpy(&dwFlags, pTp, sizeof(WORD));pTp+=sizeof(WORD);// Obtain Flags
		if(!(dwFlags & MF_POPUP)){
			memcpy(&dwID, pTp, sizeof(WORD)); // Obtain ID
			pTp+=sizeof(WORD);
		}
		else dwID = 0;
		
		uFlags = (UINT)dwFlags; // Remove MF_END from the flags that will
		if(uFlags & MF_END) // be passed to the Append(OD)Menu functions.
			uFlags -= MF_END;
		
		// Obtain Caption (and length)
		
		nLen = 0;
		szCaption=new wchar_t[wcslen((wchar_t *)pTp)+1];
		wcscpy(szCaption,(wchar_t *)pTp);
		pTp=&pTp[(wcslen((wchar_t *)pTp)+1)*sizeof(wchar_t)];//modified SK
		
		// Handle popup menus first....
		
		//WideCharToMultiByte
		if(dwFlags & MF_POPUP){
			if(dwFlags & MF_END)m_StackEnd.SetAt(m_Stack.GetUpperBound(),TRUE);
			BCMenu* pSubMenu = new BCMenu;
			pSubMenu->m_unselectcheck=m_unselectcheck;
			pSubMenu->m_selectcheck=m_selectcheck;
			pSubMenu->checkmaps=checkmaps;
			pSubMenu->checkmapsshare=TRUE;
			pSubMenu->CreatePopupMenu();
			
			// Append it to the top of the stack:
			
			m_Stack[m_Stack.GetUpperBound()]->AppendODMenuW(szCaption,uFlags,
				(UINT)pSubMenu->m_hMenu, -1);
			m_Stack.Add(pSubMenu);
			m_StackEnd.Add(FALSE);
		}
		else {
			m_Stack[m_Stack.GetUpperBound()]->AppendODMenuW(szCaption, uFlags,
				dwID, -1);
			if(dwFlags & MF_END)m_StackEnd.SetAt(m_Stack.GetUpperBound(),TRUE);
			j = m_Stack.GetUpperBound();
			while(j>=0 && m_StackEnd.GetAt(j)){
				m_Stack[m_Stack.GetUpperBound()]->InsertSpaces();
				m_Stack.RemoveAt(j);
				m_StackEnd.RemoveAt(j);
				--j;
			}
		}
		
		delete[] szCaption;
	}while(m_Stack.GetUpperBound() != -1);
	
	for(int i=0;i<(int)GetMenuItemCount();++i){
		CString str=m_MenuList[i]->GetString();
		
		if(GetSubMenu(i)){
			m_MenuList[i]->nFlags=MF_POPUP|MF_BYPOSITION;
			ModifyMenu(i,MF_POPUP|MF_BYPOSITION,
				(UINT)GetSubMenu(i)->m_hMenu,str);
		}
		else{
			m_MenuList[i]->nFlags=MF_STRING|MF_BYPOSITION;
			ModifyMenu(i,MF_STRING|MF_BYPOSITION,m_MenuList[i]->nID,str);
		}
	}
	
	return(TRUE);
}

void BCMenu::InsertSpaces(void)
{
	int i,j,numitems,maxlength;
	CString string,newstring;
	CSize t;
	CFont m_fontMenu;
	LOGFONT m_lf;
	
	ZeroMemory ((PVOID) &m_lf,sizeof (LOGFONT));
	NONCLIENTMETRICS nm;
	nm.cbSize = sizeof (NONCLIENTMETRICS);
	VERIFY (SystemParametersInfo (SPI_GETNONCLIENTMETRICS,nm.cbSize,&nm,0)); 
	m_lf =  nm.lfMenuFont;
	m_fontMenu.CreateFontIndirect (&m_lf);
	
	CWnd *pWnd = AfxGetMainWnd();  
	CDC *pDC = pWnd->GetDC();
	CFont* pFont = pDC->SelectObject (&m_fontMenu);
	
	numitems=GetMenuItemCount();
	maxlength = -1;
	for(i=0;i<numitems;++i){
		string=m_MenuList[i]->GetString();
		j=string.Find((char)9);
		newstring.Empty();
		if(j!=-1)newstring=string.Left(j);
		else newstring=string;
		newstring+=_T(" ");//SK: modified for Unicode correctness. 
		LPCTSTR lpstrText = (LPCTSTR)newstring;
		t=pDC->GetTextExtent(lpstrText,_tcslen(lpstrText));
		if(t.cx>maxlength)maxlength = t.cx;
	}
	for(i=0;i<numitems;++i){
		string=m_MenuList[i]->GetString();
		j=string.Find((char)9);
		if(j!=-1){
			newstring.Empty();
			newstring=string.Left(j);
			LPCTSTR lpstrText = (LPCTSTR)(newstring);
			t=pDC->GetTextExtent(lpstrText,_tcslen(lpstrText));
			while(t.cx<maxlength){
				newstring+=_T(' ');//SK: modified for Unicode correctness
				LPCTSTR lpstrText = (LPCTSTR)(newstring);
				t=pDC->GetTextExtent(lpstrText,_tcslen(lpstrText));
			}
			newstring+=string.Mid(j);
#ifdef UNICODE      
			m_MenuList[i]->SetWideString(newstring);//SK: modified for dynamic allocation
#else
			m_MenuList[i]->SetAnsiString(newstring);
#endif
		}
	}
	pDC->SelectObject (pFont);              // Select old font in
	AfxGetMainWnd()->ReleaseDC(pDC);       // Release the DC
	m_fontMenu.DeleteObject();
}

void BCMenu::LoadCheckmarkBitmap(int unselect, int select)
{
	if(unselect>0 && select>0){
		m_selectcheck=select;
		m_unselectcheck=unselect;
		if(checkmaps)checkmaps->DeleteImageList();
		else checkmaps=new(CImageList);
		checkmaps->Create(m_iconX,m_iconY,ILC_MASK,2,1);
		BOOL flag1=AddBitmapToImageList(checkmaps,unselect);
		BOOL flag2=AddBitmapToImageList(checkmaps,select);
		if(!flag1||!flag2){
			checkmaps->DeleteImageList();
			delete checkmaps;
			checkmaps=NULL;
		}
	}
}

//--------------------------------------------------------------------------
//[18.06.99 rj]
BOOL BCMenu::GetMenuText(UINT id, CString& string, UINT nFlags/*= MF_BYPOSITION*/)
{
	BOOL returnflag=FALSE;
	
	if(MF_BYPOSITION&nFlags){
		UINT numMenuItems = m_MenuList.GetUpperBound();
		if(id<=numMenuItems){
			string=m_MenuList[id]->GetString();
			returnflag=TRUE;
		}
	}
	else{
		int uiLoc;
		BCMenu* pMenu = FindMenuOption(id,uiLoc);
		if(NULL!=pMenu) returnflag = pMenu->GetMenuText(uiLoc,string);
	}
	return(returnflag);
}


void BCMenu::DrawRadioDot(CDC *pDC,int x,int y,COLORREF color)
{
	CRect rcDot(x,y,x+6,y+6);
	CBrush brush;
	CPen pen;
	brush.CreateSolidBrush(color);
	pen.CreatePen(PS_SOLID,0,color);
	CBrush *pOldBrush=pDC->SelectObject(&brush);
	CPen *pOldPen=pDC->SelectObject(&pen);
	pDC->Ellipse(&rcDot);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
	brush.DeleteObject();
}

void BCMenu::DrawCheckMark(CDC* pDC,int x,int y,COLORREF color)
{
	CPen m_penBack;
	m_penBack.CreatePen (PS_SOLID,0,color);
	CPen *pOldPen = pDC->SelectObject (&m_penBack);
	pDC->MoveTo(x,y+2);
	pDC->LineTo(x,y+5);
	
	pDC->MoveTo(x+1,y+3);
	pDC->LineTo(x+1,y+6);
	
	pDC->MoveTo(x+2,y+4);
	pDC->LineTo(x+2,y+7);
	
	pDC->MoveTo(x+3,y+3);
	pDC->LineTo(x+3,y+6);
	
	pDC->MoveTo(x+4,y+2);
	pDC->LineTo(x+4,y+5);
	
	pDC->MoveTo(x+5,y+1);
	pDC->LineTo(x+5,y+4);
	
	pDC->MoveTo(x+6,y);
	pDC->LineTo(x+6,y+3);
	
	pDC->SelectObject (pOldPen);
	m_penBack.DeleteObject();
}

BCMenuData *BCMenu::FindMenuList(UINT nID)
{
	for(int i=0;i<=m_MenuList.GetUpperBound();++i){
		if(m_MenuList[i]->nID==nID && !m_MenuList[i]->syncflag){
			m_MenuList[i]->syncflag=1;
			return(m_MenuList[i]);
		}
	}
	return(NULL);
}

void BCMenu::InitializeMenuList(int value)
{
	for(int i=0;i<=m_MenuList.GetUpperBound();++i)
		m_MenuList[i]->syncflag=value;
}

void BCMenu::DeleteMenuList(void)
{
	for(int i=0;i<=m_MenuList.GetUpperBound();++i){
		if(!m_MenuList[i]->syncflag){
			delete m_MenuList[i];
		}
	}
}

void BCMenu::SynchronizeMenu(void)
{
	CTypedPtrArray<CPtrArray, BCMenuData*> temp;
	BCMenuData *mdata;
	CString string;
	UINT submenu,nID=0,state,j;
	
	InitializeMenuList(0);
	for(j=0;j<GetMenuItemCount();++j){
		mdata=NULL;
		state=GetMenuState(j,MF_BYPOSITION);
		if(state&MF_POPUP){
			submenu=(UINT)GetSubMenu(j)->m_hMenu;
			mdata=FindMenuList(submenu);
			GetMenuString(j,string,MF_BYPOSITION);
			if(!mdata)mdata=NewODMenu(j,
				(state&0xFF)|MF_BYPOSITION|MF_POPUP|MF_OWNERDRAW,submenu,string);
			else if(string.GetLength()>0)
#ifdef UNICODE
				mdata->SetWideString(string);  //SK: modified for dynamic allocation
#else
			mdata->SetAnsiString(string);
#endif
		}
		else if(state&MF_SEPARATOR){
			mdata=FindMenuList(0);
			if(!mdata)mdata=NewODMenu(j,
				state|MF_BYPOSITION|MF_SEPARATOR|MF_OWNERDRAW,0,_T(""));//SK: modified for Unicode correctness
			else ModifyMenu(j,mdata->nFlags,nID,(LPCTSTR)mdata);
		}
		else{
			nID=GetMenuItemID(j);
			mdata=FindMenuList(nID);
			GetMenuString(j,string,MF_BYPOSITION);
			if(!mdata)mdata=NewODMenu(j,state|MF_BYPOSITION|MF_OWNERDRAW,
				nID,string);
			else{
				mdata->nFlags=state|MF_BYPOSITION|MF_OWNERDRAW;
				if(string.GetLength()>0)
#ifdef UNICODE
					mdata->SetWideString(string);//SK: modified for dynamic allocation
#else
				mdata->SetAnsiString(string);
#endif
				
				ModifyMenu(j,mdata->nFlags,nID,(LPCTSTR)mdata);
			}
		}
		if(mdata)temp.Add(mdata);
	}
	DeleteMenuList();
	m_MenuList.RemoveAll();
	m_MenuList.Append(temp);
	temp.RemoveAll(); 
}

void BCMenu::UpdateMenu(CMenu *pmenu)
{
#ifdef _CPPRTTI 
	BCMenu *psubmenu = dynamic_cast<BCMenu *>(pmenu);
#else
	BCMenu *psubmenu = (BCMenu *)pmenu;
#endif
	if(psubmenu)psubmenu->SynchronizeMenu();
}

LRESULT BCMenu::FindKeyboardShortcut(UINT nChar, UINT nFlags,
                                     CMenu *pMenu)
{
#ifdef _CPPRTTI 
	BCMenu *pBCMenu = dynamic_cast<BCMenu *>(pMenu);
#else
	BCMenu *pBCMenu = (BCMenu *)pMenu;
#endif
	if(pBCMenu && nFlags&MF_POPUP){
		CString key(_T('&'),2);//SK: modified for Unicode correctness
		key.SetAt(1,(TCHAR)nChar);
		key.MakeLower();
		CString menutext;
		int menusize = (int)pBCMenu->GetMenuItemCount();
		if(menusize!=(pBCMenu->m_MenuList.GetUpperBound()+1))
			pBCMenu->SynchronizeMenu();
		for(int i=0;i<menusize;++i){
			if(pBCMenu->GetMenuText(i,menutext)){
				menutext.MakeLower();
				if(menutext.Find(key)>=0)return(MAKELRESULT(i,2));
			}
		}
	}
	return(0);
}

void BCMenu::DitherBlt (HDC hdcDest, int nXDest, int nYDest, int nWidth, 
                        int nHeight, HBITMAP hbm, int nXSrc, int nYSrc,
						COLORREF bgcolor)
{
	ASSERT(hdcDest && hbm);
	ASSERT(nWidth > 0 && nHeight > 0);
	
	// Create a generic DC for all BitBlts
	HDC hDC = CreateCompatibleDC(hdcDest);
	ASSERT(hDC);
	
	if (hDC)
	{
		// Create a DC for the monochrome DIB section
		HDC bwDC = CreateCompatibleDC(hDC);
		ASSERT(bwDC);
		
		if (bwDC)
		{
			// Create the monochrome DIB section with a black and white palette
			struct {
				BITMAPINFOHEADER bmiHeader; 
				RGBQUAD      bmiColors[2]; 
			} RGBBWBITMAPINFO = {
				
				{    // a BITMAPINFOHEADER
					sizeof(BITMAPINFOHEADER),  // biSize 
						nWidth,         // biWidth; 
						nHeight,        // biHeight; 
						1,            // biPlanes; 
						1,            // biBitCount 
						BI_RGB,         // biCompression; 
						0,            // biSizeImage; 
						0,            // biXPelsPerMeter; 
						0,            // biYPelsPerMeter; 
						0,            // biClrUsed; 
						0            // biClrImportant; 
				},    
				{
					{ 0x00, 0x00, 0x00, 0x00 }, { 0xFF, 0xFF, 0xFF, 0x00 }
					} 
			};
			VOID *pbitsBW;
			HBITMAP hbmBW = CreateDIBSection(bwDC,
				(LPBITMAPINFO)&RGBBWBITMAPINFO, DIB_RGB_COLORS, &pbitsBW, NULL, 0);
			ASSERT(hbmBW);
			
			if (hbmBW)
			{
				// Attach the monochrome DIB section and the bitmap to the DCs
				HBITMAP olddib = (HBITMAP)SelectObject(bwDC, hbmBW);
				HBITMAP hdcolddib = (HBITMAP)SelectObject(hDC, hbm);
				
				// BitBlt the bitmap into the monochrome DIB section
				BitBlt(bwDC, 0, 0, nWidth, nHeight, hDC, nXSrc, nYSrc, SRCCOPY);
				
				// Paint the destination rectangle in gray
				FillRect(hdcDest, CRect(nXDest, nYDest, nXDest + nWidth, nYDest +
					nHeight), GetSysColorBrush(bgcolor));
				//SK: looks better on the old shell
				// BitBlt the black bits in the monochrome bitmap into COLOR_3DHILIGHT
				// bits in the destination DC
				// The magic ROP comes from the Charles Petzold's book
				HBRUSH hb = CreateSolidBrush(GetSysColor(COLOR_3DHILIGHT));
				HBRUSH oldBrush = (HBRUSH)SelectObject(hdcDest, hb);
				BitBlt(hdcDest,nXDest+1,nYDest+1,nWidth,nHeight,bwDC,0,0,0xB8074A);
				
				// BitBlt the black bits in the monochrome bitmap into COLOR_3DSHADOW
				// bits in the destination DC
				hb = CreateSolidBrush(GetSysColor(COLOR_3DSHADOW));
				DeleteObject(SelectObject(hdcDest, hb));
				BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight,bwDC,0,0,0xB8074A);
				DeleteObject(SelectObject(hdcDest, oldBrush));
				VERIFY(DeleteObject(SelectObject(bwDC, olddib)));
				SelectObject(hDC, hdcolddib);
			}
			
			VERIFY(DeleteDC(bwDC));
		}
		
		VERIFY(DeleteDC(hDC));
	}
}

BOOL BCMenu::AddBitmapToImageList(CImageList *bmplist,UINT nResourceID)
{
	// O.S.
	if (m_bDynIcons){
		bmplist->Add((HICON)nResourceID);
		return TRUE;
	}
	
	BOOL bReturn=FALSE;
	
	HBITMAP hbmp=LoadSysColorBitmap(nResourceID);
	if(hbmp){
		CBitmap bmp;
		bmp.Attach(hbmp);
		if(m_bitmapBackgroundFlag){
			if(bmplist->Add(&bmp,m_bitmapBackground)>=0)bReturn=TRUE;
		}
		else{
			if(bmplist->Add(&bmp,GetSysColor(COLOR_3DFACE))>=0)bReturn=TRUE;
		}
		bmp.Detach();
		DeleteObject(hbmp);
	}
	else{
		CBitmap mybmp;
		if(mybmp.LoadBitmap(nResourceID)){
			if(m_bitmapBackgroundFlag){
				if(bmplist->Add(&mybmp,m_bitmapBackground)>=0)bReturn=TRUE;
			}
			else{
				if(bmplist->Add(&mybmp,GetSysColor(COLOR_3DFACE))>=0)bReturn=TRUE;
			}
		}
	}
	return(bReturn);
}

void BCMenu::SetBitmapBackground(COLORREF color)
{
	m_bitmapBackground=color;
	m_bitmapBackgroundFlag=TRUE;
}

void BCMenu::UnSetBitmapBackground(void)
{
	m_bitmapBackgroundFlag=FALSE;
}

// Given a toolbar, append all the options from it to this menu
// Passed a ptr to the toolbar object and the toolbar ID
// Author : Robert Edward Caldecott
void BCMenu::AddFromToolBar(CToolBar* pToolBar, int nResourceID)
{
	for (int i = 0; i < pToolBar->GetCount(); i++) {
		UINT nID = pToolBar->GetItemID(i);
		// See if this toolbar option
		// appears as a command on this
		// menu or is a separator
		if (nID == 0 || GetMenuState(nID, MF_BYCOMMAND) == 0xFFFFFFFF)
			continue; // Item doesn't exist
		UINT nStyle;
		int nImage;
		// Get the toolbar button info
		pToolBar->GetButtonInfo(i, nID, nStyle, nImage);
		// OK, we have the command ID of the toolbar
		// option, and the tollbar bitmap offset
		int nLoc;
		BCMenuData* pData;
		BCMenu *pSubMenu = FindMenuOption(nID, nLoc);
		if (pSubMenu && nLoc >= 0)pData = pSubMenu->m_MenuList[nLoc];
		else {
			// Create a new BCMenuData structure
			pData = new BCMenuData;
			m_MenuList.Add(pData);
		}
		// Set some default structure members
		pData->menuIconNormal = nResourceID;
		pData->nID = nID;
		pData->nFlags =  MF_BYCOMMAND | MF_OWNERDRAW;
		pData->xoffset = nImage;
		if (pData->bitmap)pData->bitmap->DeleteImageList();
		else pData->bitmap = new CImageList;
		pData->bitmap->Create(m_iconX, m_iconY,ILC_COLORDDB|ILC_MASK, 1, 1);
		
		if(!AddBitmapToImageList(pData->bitmap, nResourceID)){
			pData->bitmap->DeleteImageList();
			delete pData->bitmap;
			pData->bitmap=NULL;
			pData->menuIconNormal = -1;
			pData->xoffset = -1;
		}
		
		// Modify our menu
		ModifyMenu(nID,pData->nFlags,nID,(LPCTSTR)pData);
	}
}

BOOL BCMenu::Draw3DCheckmark(CDC *dc, const CRect& rc,
                             BOOL bSelected, HBITMAP hbmCheck)
{
	CRect rcDest = rc;
	CBrush brush;
	COLORREF col=GetSysColor((bSelected||!IsNewShell())?COLOR_MENU:COLOR_3DLIGHT);//SK: Looks better on the old shell
	brush.CreateSolidBrush(col);
	dc->FillRect(rcDest,&brush);
	brush.DeleteObject();
	if (IsNewShell()) //SK: looks better on the old shell
		dc->DrawEdge(&rcDest, BDR_SUNKENOUTER, BF_RECT);
	if (!hbmCheck)DrawCheckMark(dc,rc.left+4,rc.top+4,GetSysColor(COLOR_MENUTEXT));
	else DrawRadioDot(dc,rc.left+5,rc.top+4,GetSysColor(COLOR_MENUTEXT));
	return TRUE;
}

BOOL BCMenu::DrawXPCheckmark(CDC *dc, const CRect& rc, HBITMAP hbmCheck,COLORREF &colorout)
{
	CBrush brushin;
	brushin.CreateSolidBrush(LightenColor(colorout,0.85));
	dc->FillRect(rc,&brushin);
	brushin.DeleteObject();
	dc->Draw3dRect (rc,colorout,colorout);
	if (!hbmCheck)DrawCheckMark(dc,rc.left+5,rc.top+4,GetSysColor(COLOR_MENUTEXT));
	else DrawRadioDot(dc,rc.left+5,rc.top+4,GetSysColor(COLOR_MENUTEXT));
	return TRUE;
}

void BCMenu::DitherBlt2(CDC *drawdc, int nXDest, int nYDest, int nWidth, 
                        int nHeight, CBitmap &bmp, int nXSrc, int nYSrc,
						COLORREF bgcolor)
{
	// create a monochrome memory DC
	CDC ddc;
	ddc.CreateCompatibleDC(0);
	CBitmap bwbmp;
	bwbmp.CreateCompatibleBitmap(&ddc, nWidth, nHeight);
	CBitmap * pddcOldBmp = ddc.SelectObject(&bwbmp);
	
	CDC dc;
	dc.CreateCompatibleDC(0);
	CBitmap * pdcOldBmp = dc.SelectObject(&bmp);
	
	// build a mask
	ddc.PatBlt(0, 0, nWidth, nHeight, WHITENESS);
	dc.SetBkColor(GetSysColor(COLOR_BTNFACE));
	ddc.BitBlt(0, 0, nWidth, nHeight, &dc, nXSrc,nYSrc, SRCCOPY);
	dc.SetBkColor(GetSysColor(COLOR_BTNHILIGHT));
	ddc.BitBlt(0, 0, nWidth, nHeight, &dc, nXSrc,nYSrc, SRCPAINT);
	
	// Copy the image from the toolbar into the memory DC
	// and draw it (grayed) back into the toolbar.
	dc.FillSolidRect(0,0, nWidth, nHeight, bgcolor);
	//SK: Looks better on the old shell
	dc.SetBkColor(RGB(0, 0, 0));
	dc.SetTextColor(RGB(255, 255, 255));
	CBrush brShadow, brHilight;
	brHilight.CreateSolidBrush(GetSysColor(COLOR_BTNHILIGHT));
	brShadow.CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
	CBrush * pOldBrush = dc.SelectObject(&brHilight);
	dc.BitBlt(0,0, nWidth, nHeight, &ddc, 0, 0, 0x00E20746L);
	drawdc->BitBlt(nXDest+1,nYDest+1,nWidth, nHeight, &dc,0,0,SRCCOPY);
	dc.BitBlt(1,1, nWidth, nHeight, &ddc, 0, 0, 0x00E20746L);
	dc.SelectObject(&brShadow);
	dc.BitBlt(0,0, nWidth, nHeight, &ddc, 0, 0, 0x00E20746L);
	drawdc->BitBlt(nXDest,nYDest,nWidth, nHeight, &dc,0,0,SRCCOPY);
	// reset DCs
	ddc.SelectObject(pddcOldBmp);
	ddc.DeleteDC();
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pdcOldBmp);
	dc.DeleteDC();
	
	brShadow.DeleteObject();
	brHilight.DeleteObject();
	bwbmp.DeleteObject();
}

void BCMenu::SetDisableOldStyle(void)
{
	disable_old_style=TRUE;
}

void BCMenu::UnSetDisableOldStyle(void)
{
	disable_old_style=FALSE;
}

BOOL BCMenu::GetDisableOldStyle(void)
{
	return(disable_old_style);
}


WORD BCMenu::NumBitmapColors(LPBITMAPINFOHEADER lpBitmap)
{
	if ( lpBitmap->biClrUsed != 0)
		return (WORD)lpBitmap->biClrUsed;
	
	switch (lpBitmap->biBitCount){
		case 1:
			return 2;
		case 4:
			return 16;
		case 8:
			return 256;
		default:
			return 0;
	}
	return 0;
}

HBITMAP BCMenu::LoadSysColorBitmap(int nResourceId)
{
	HINSTANCE hInst = 
		AfxFindResourceHandle(MAKEINTRESOURCE(nResourceId),RT_BITMAP);
	HRSRC hRsrc = 
		::FindResource(hInst,MAKEINTRESOURCE(nResourceId),RT_BITMAP);
	if (hRsrc == NULL)return NULL;

	// determine how many colors in the bitmap
	HGLOBAL hglb;
	if ((hglb = LoadResource(hInst, hRsrc)) == NULL)
		return NULL;
	LPBITMAPINFOHEADER lpBitmap = (LPBITMAPINFOHEADER)LockResource(hglb);
	if (lpBitmap == NULL)return NULL;
	WORD numcol = NumBitmapColors(lpBitmap);
	::FreeResource(hglb);

	if(numcol!=16)return(NULL);

	return AfxLoadSysColorBitmap(hInst, hRsrc, FALSE);
}

BOOL BCMenu::RemoveMenu(UINT uiId,UINT nFlags)
{
	if(MF_BYPOSITION&nFlags){
		UINT uint = GetMenuState(uiId,MF_BYPOSITION);
		if(uint&MF_SEPARATOR && !(uint&MF_POPUP)){
			delete m_MenuList.GetAt(uiId);
			m_MenuList.RemoveAt(uiId);
		}
		else{
			BCMenu* pSubMenu = (BCMenu*) GetSubMenu(uiId);
			if(NULL==pSubMenu){
				UINT uiCommandId = GetMenuItemID(uiId);
				for(int i=0;i<m_MenuList.GetSize(); i++){
					if(m_MenuList[i]->nID==uiCommandId){
						delete m_MenuList.GetAt(i);
						m_MenuList.RemoveAt(i);
						break;
					}
				}
			}
			else{
				int numSubMenus = m_SubMenus.GetUpperBound();
				for(int m = numSubMenus; m >= 0; m--){
					if(m_SubMenus[m]==pSubMenu->m_hMenu){
						int numAllSubMenus = m_AllSubMenus.GetUpperBound();
						for(int n = numAllSubMenus; n>= 0; n--){
							if(m_AllSubMenus[n]==m_SubMenus[m])m_AllSubMenus.RemoveAt(n);
						}
						m_SubMenus.RemoveAt(m);
					}
				}
				int num = pSubMenu->GetMenuItemCount();
				for(int i=num-1;i>=0;--i)pSubMenu->RemoveMenu(i,MF_BYPOSITION);
				for(i=m_MenuList.GetUpperBound();i>=0;i--){
					if(m_MenuList[i]->nID==(UINT)pSubMenu->m_hMenu){
						delete m_MenuList.GetAt(i);
						m_MenuList.RemoveAt(i);
						break;
					}
				}
				delete pSubMenu;
			}
		}
	}
	else{
		int iPosition =0;
		BCMenu* pMenu = FindMenuOption(uiId,iPosition);
		// bug fix RIA 14th September 2000 
		// failed to return correct value on call to remove menu as the item was 
		// removed twice. The second time its not found 
		// so a value of 0 was being returned 
		if(pMenu) return pMenu->RemoveMenu(iPosition,MF_BYPOSITION); // added return 
	}
	return CMenu::RemoveMenu(uiId,nFlags);
}

BOOL BCMenu::DeleteMenu(UINT uiId,UINT nFlags)
{
	if(MF_BYPOSITION&nFlags){
		UINT uint = GetMenuState(uiId,MF_BYPOSITION);
		if(uint&MF_SEPARATOR && !(uint&MF_POPUP)){
			delete m_MenuList.GetAt(uiId);
			m_MenuList.RemoveAt(uiId);
		}
		else{
			BCMenu* pSubMenu = (BCMenu*) GetSubMenu(uiId);
			if(NULL==pSubMenu){
				UINT uiCommandId = GetMenuItemID(uiId);
				for(int i=0;i<m_MenuList.GetSize(); i++){
					if(m_MenuList[i]->nID==uiCommandId){
						delete m_MenuList.GetAt(i);
						m_MenuList.RemoveAt(i);
						break;
					}
				}
			}
			else{
				int numSubMenus = m_SubMenus.GetUpperBound();
				for(int m = numSubMenus; m >= 0; m--){
					if(m_SubMenus[m]==pSubMenu->m_hMenu){
						int numAllSubMenus = m_AllSubMenus.GetUpperBound();
						for(int n = numAllSubMenus; n>= 0; n--){
							if(m_AllSubMenus[n]==m_SubMenus[m])m_AllSubMenus.RemoveAt(n);
						}
						m_SubMenus.RemoveAt(m);
					}
				}
				int num = pSubMenu->GetMenuItemCount();
				for(int i=num-1;i>=0;--i)pSubMenu->DeleteMenu(i,MF_BYPOSITION);
				for(i=m_MenuList.GetUpperBound();i>=0;i--){
					if(m_MenuList[i]->nID==(UINT)pSubMenu->m_hMenu){
						delete m_MenuList.GetAt(i);
						m_MenuList.RemoveAt(i);
						break;
					}
				}
				delete pSubMenu;
			}
		}
	}
	else{
		int iPosition =0;
		BCMenu* pMenu = FindMenuOption(uiId,iPosition);
		if(pMenu)pMenu->DeleteMenu(iPosition,MF_BYPOSITION);
	}
	return CMenu::DeleteMenu(uiId,nFlags);
}


BOOL BCMenu::AppendMenuA(UINT nFlags,UINT nIDNewItem,const char *lpszNewItem,int nIconNormal)
{
	USES_CONVERSION;
	return AppendMenuW(nFlags,nIDNewItem,A2W(lpszNewItem),nIconNormal);
}

BOOL BCMenu::AppendMenuW(UINT nFlags,UINT nIDNewItem,wchar_t *lpszNewItem,int nIconNormal)
{
	return AppendODMenuW(lpszNewItem,nFlags,nIDNewItem,nIconNormal);
}

BOOL BCMenu::AppendMenuA(UINT nFlags,UINT nIDNewItem,const char *lpszNewItem,CImageList *il,int xoffset)
{
	USES_CONVERSION;
	return AppendMenuW(nFlags,nIDNewItem,A2W(lpszNewItem),il,xoffset);
}

BOOL BCMenu::AppendMenuW(UINT nFlags,UINT nIDNewItem,wchar_t *lpszNewItem,CImageList *il,int xoffset)
{
	return AppendODMenuW(lpszNewItem,nFlags,nIDNewItem,il,xoffset);
}

BOOL BCMenu::AppendMenuA(UINT nFlags,UINT nIDNewItem,const char *lpszNewItem,CBitmap *bmp)
{
	USES_CONVERSION;
	return AppendMenuW(nFlags,nIDNewItem,A2W(lpszNewItem),bmp);
}

BOOL BCMenu::AppendMenuW(UINT nFlags,UINT nIDNewItem,wchar_t *lpszNewItem,CBitmap *bmp)
{
	if(bmp){
		CImageList temp;
		temp.Create(m_iconX,m_iconY,ILC_COLORDDB|ILC_MASK,1,1);
		if(m_bitmapBackgroundFlag)temp.Add(bmp,m_bitmapBackground);
		else temp.Add(bmp,GetSysColor(COLOR_3DFACE));
		return AppendODMenuW(lpszNewItem,nFlags,nIDNewItem,&temp,0);
	}
	return AppendODMenuW(lpszNewItem,nFlags,nIDNewItem,NULL,0);
}

BOOL BCMenu::InsertMenuA(UINT nPosition,UINT nFlags,UINT nIDNewItem,const char *lpszNewItem,int nIconNormal)
{
	USES_CONVERSION;
	return InsertMenuW(nPosition,nFlags,nIDNewItem,A2W(lpszNewItem),nIconNormal);
}

BOOL BCMenu::InsertMenuW(UINT nPosition,UINT nFlags,UINT nIDNewItem,wchar_t *lpszNewItem,int nIconNormal)
{
	return InsertODMenuW(nPosition,lpszNewItem,nFlags,nIDNewItem,nIconNormal);
}

BOOL BCMenu::InsertMenuA(UINT nPosition,UINT nFlags,UINT nIDNewItem,const char *lpszNewItem,CImageList *il,int xoffset)
{
	USES_CONVERSION;
	return InsertMenuW(nPosition,nFlags,nIDNewItem,A2W(lpszNewItem),il,xoffset);
}

BOOL BCMenu::InsertMenuW(UINT nPosition,UINT nFlags,UINT nIDNewItem,wchar_t *lpszNewItem,CImageList *il,int xoffset)
{
	return InsertODMenuW(nPosition,lpszNewItem,nFlags,nIDNewItem,il,xoffset);
}

BOOL BCMenu::InsertMenuA(UINT nPosition,UINT nFlags,UINT nIDNewItem,const char *lpszNewItem,CBitmap *bmp)
{
	USES_CONVERSION;
	return InsertMenuW(nPosition,nFlags,nIDNewItem,A2W(lpszNewItem),bmp);
}

BOOL BCMenu::InsertMenuW(UINT nPosition,UINT nFlags,UINT nIDNewItem,wchar_t *lpszNewItem,CBitmap *bmp)
{
	if(bmp){
		CImageList temp;
		temp.Create(m_iconX,m_iconY,ILC_COLORDDB|ILC_MASK,1,1);
		if(m_bitmapBackgroundFlag)temp.Add(bmp,m_bitmapBackground);
		else temp.Add(bmp,GetSysColor(COLOR_3DFACE));
		return InsertODMenuW(nPosition,lpszNewItem,nFlags,nIDNewItem,&temp,0);
	}
	return InsertODMenuW(nPosition,lpszNewItem,nFlags,nIDNewItem,NULL,0);
}

//--------------------------------------------------------------------------
//[21.06.99 rj]
BCMenu* BCMenu::AppendODPopupMenuW(wchar_t *lpstrText)
{
	BCMenu* pSubMenu = new BCMenu;
	pSubMenu->m_unselectcheck=m_unselectcheck;
	pSubMenu->m_selectcheck=m_selectcheck;
	pSubMenu->checkmaps=checkmaps;
	pSubMenu->checkmapsshare=TRUE;
	pSubMenu->CreatePopupMenu();
	AppendODMenuW(lpstrText,MF_POPUP,(UINT)pSubMenu->m_hMenu, -1);
	return pSubMenu;
}

//--------------------------------------------------------------------------
//[21.06.99 rj]
BCMenu* BCMenu::AppendODPopupMenuA(LPCSTR lpstrText)
{
	USES_CONVERSION;
	return AppendODPopupMenuW(A2W(lpstrText));
}

BOOL BCMenu::ImageListDuplicate(CImageList *il,int xoffset,CImageList *newlist)
{
	if (il == NULL||newlist==NULL||xoffset<0) return FALSE;
	HICON hIcon = il->ExtractIcon(xoffset);
	int cx, cy;
	ImageList_GetIconSize(il->m_hImageList, &cx, &cy);
	newlist->Create(cx,cy,ILC_COLORDDB|ILC_MASK,1,1);
	newlist->Add(hIcon);
	::DestroyIcon(hIcon);
	return TRUE;
}

// 2001-07-12, Damir Valiulin:
//          Added GetSubMenu (LPCTSTR lpszSubMenuName) function
//

CMenu* BCMenu::GetSubMenu(int nPos)
{
	return CMenu::GetSubMenu (nPos);
}

CMenu* BCMenu::GetSubMenu(LPCTSTR lpszSubMenuName)
{
	int num = GetMenuItemCount ();
	CString name;
	
	for (int i=0; i<num; i++)
	{
		GetMenuString (i, name, MF_BYPOSITION);
		if (name.Compare (lpszSubMenuName) == 0)
		{
			return CMenu::GetSubMenu (i);
		}
	}
	
	return NULL;
}

// Tongzhe Cui, Functions to remove a popup menu based on its name. Seperators
// before and after the popup menu can also be removed if they exist.
int BCMenu::GetMenuPosition(char* pText)
{
	USES_CONVERSION;
	return GetMenuPosition(A2W(pText));
}

int BCMenu::GetMenuPosition(wchar_t* pText)
{
	int i,j;
	BCMenu* psubmenu;
	for(i=0;i<(int)(GetMenuItemCount());++i)
	{
		psubmenu=(BCMenu *)GetSubMenu(i);
		if(!psubmenu)
		{
			const wchar_t *szWide;//SK: we use const to prevent misuse of this Ptr
			for(j=0;j<=m_MenuList.GetUpperBound();++j)
			{     
				szWide = m_MenuList[j]->GetWideString ();
				if(szWide && !wcscmp(pText,szWide))//SK: modified for dynamic allocation
					return j;
			}
		}
	}
	// means no found;
	return -1;
}

int BCMenu::RemoveMenu(char* pText, BC_Seperator sPos)
{
	USES_CONVERSION;
	return RemoveMenu(A2W(pText), sPos);
}

int BCMenu::RemoveMenu(wchar_t* pText, BC_Seperator sPos)
{
	int nPos = GetMenuPosition(pText);
	if(nPos != -1)
	{
		RemoveMenu(nPos, MF_BYPOSITION);
	}
	if(sPos == BCMENU_HEAD)
	{
		ASSERT(nPos - 1 >= 0);
		RemoveMenu(nPos-1, MF_BYPOSITION);
	}
	else if(sPos == BCMENU_TAIL)
	{
		RemoveMenu(nPos-1, MF_BYPOSITION);
	}
	else if(sPos == BCMENU_BOTH)
	{
		// remove the end first;
		RemoveMenu(nPos-1, MF_BYPOSITION);
		// remove the head;
		ASSERT(nPos - 1 >= 0);
		RemoveMenu(nPos-1, MF_BYPOSITION);
	}
	return nPos;
}

// Larry Antram
BOOL BCMenu::SetMenuText(UINT id, CString string, UINT nFlags/*= MF_BYPOSITION*/ )
{
	BOOL returnflag=FALSE;
	
	if(MF_BYPOSITION&nFlags)
	{
		UINT numMenuItems = m_MenuList.GetUpperBound();
		if(id<=numMenuItems){
#ifdef UNICODE
			m_MenuList[id]->SetWideString((LPCTSTR)string);
#else
			m_MenuList[id]->SetAnsiString(string);
#endif
			returnflag=TRUE;
		}
	}
	else{
		int uiLoc;
		BCMenu* pMenu = FindMenuOption(id,uiLoc);
		if(NULL!=pMenu) returnflag = pMenu->SetMenuText(uiLoc,string);
	}
	return(returnflag);
}

// courtesy of Warren Stevens
void BCMenu::ColorBitmap(CDC* pDC,CBitmap& bmp,CSize size,COLORREF fill,COLORREF border,int hatchstyle)
{
	CDC bmpdc;
	bmpdc.CreateCompatibleDC(pDC);

	bmp.CreateCompatibleBitmap(pDC, size.cx, size.cy);	
	CPen border_pen(PS_SOLID, 1, border);
	
	CBrush fill_brush;
	if(hatchstyle!=-1) { fill_brush.CreateHatchBrush(hatchstyle, fill); }
	else      { fill_brush.CreateSolidBrush(fill);             }
	
	CBitmap* pOldBitmap = bmpdc.SelectObject(&bmp);
	CPen*    pOldPen    = bmpdc.SelectObject(&border_pen);
	CBrush*  pOldBrush  = bmpdc.SelectObject(&fill_brush);

	bmpdc.Rectangle(0,0, size.cx, size.cy);

	if(NULL!=pOldBrush)  { bmpdc.SelectObject(pOldBrush);  }
	if(NULL!=pOldPen)    { bmpdc.SelectObject(pOldPen);    }
	if(NULL!=pOldBitmap) { bmpdc.SelectObject(pOldBitmap); }
}

//*************************************************************************