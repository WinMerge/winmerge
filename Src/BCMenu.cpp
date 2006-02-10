//*************************************************************************
// BCMenu.cpp : implementation file
// Version : 3.034
// Date : May 2002
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
// Damir Valiulin,David Kinder,Marc Loiry
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

#define BCMENU_GAP 1
#ifndef OBM_CHECK
#define OBM_CHECK 32760 // from winuser.h
#endif

#if _MFC_VER <0x400
#error This code does not work on Versions of MFC prior to 4.0
#endif

static CPINFO CPInfo;
// how the menu's are drawn in win9x/NT/2000
UINT BCMenu::original_drawmode=BCMENU_DRAWMODE_ORIGINAL;
BOOL BCMenu::original_select_disabled=TRUE;
// how the menu's are drawn in winXP
UINT BCMenu::xp_drawmode=BCMENU_DRAWMODE_XP;
BOOL BCMenu::xp_select_disabled=FALSE;

// Set to FALSE since TRUE value causes WinMerge to start slowly in WinXP
// Ref: BUG item #1052762 WinMerge VERY slow to startup
// https://sourceforge.net/tracker/index.php?func=detail&aid=1052762&group_id=13216&atid=113216
BOOL BCMenu::xp_draw_3D_bitmaps=FALSE;
BOOL BCMenu::hicolor_bitmaps=FALSE;
// Variable to set how accelerators are justified. The default mode (TRUE) right
// justifies them to the right of the longes string in the menu. FALSE
// just right justifies them.
BOOL BCMenu::xp_space_accelerators=TRUE;
BOOL BCMenu::original_space_accelerators=TRUE;

CImageList BCMenu::m_AllImages;
CArray<int,int&> BCMenu::m_AllImagesID;
int BCMenu::m_iconX = 16;
int BCMenu::m_iconY = 15;

enum Win32Type{
	Win32s,
	WinNT3,
	Win95,
	Win98,
	WinME,
	WinNT4,
	Win2000,
	WinXP
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
			if(osvi->dwMajorVersion==4L)ShellType=WinNT4;
			else if(osvi->dwMajorVersion==5L&&osvi->dwMinorVersion==0L)ShellType=Win2000;
			else if(osvi->dwMajorVersion==5L&&osvi->dwMinorVersion==1L)ShellType=WinXP;
			free(osvi);
		}
	}
	else if  (LOBYTE(LOWORD(winVer))<4)
		ShellType=Win32s;
	else{
		ShellType=Win95;
		osvi= (OSVERSIONINFO *)malloc(sizeof(OSVERSIONINFO));
		if (osvi!=NULL){
			memset(osvi,0,sizeof(OSVERSIONINFO));
			osvi->dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
			GetVersionEx(osvi);
			if(osvi->dwMajorVersion==4L&&osvi->dwMinorVersion==10L)ShellType=Win98;
			else if(osvi->dwMajorVersion==4L&&osvi->dwMinorVersion==90L)ShellType=WinME;
			free(osvi);
		}
	}
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

IMPLEMENT_DYNAMIC( BCMenu, CMenu )

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
	m_selectcheck = -1;
	m_unselectcheck = -1;
	checkmaps=NULL;
	checkmapsshare=FALSE;
	// set the color used for the transparent background in all bitmaps
	m_bitmapBackground=RGB(192,192,192); //gray
	m_bitmapBackgroundFlag=FALSE;
	GetCPInfo(CP_ACP,&CPInfo);
	m_loadmenu=FALSE;
}


BCMenu::~BCMenu()
{
	DestroyMenu();
}

BOOL BCMenu::IsNewShell ()
{
	return (g_Shell>=Win95);
}

BOOL BCMenu::IsWinXPLuna()
{
	if(g_Shell==WinXP){
		if(IsWindowsClassicTheme())return(FALSE);
		else return(TRUE);
	}
	return(FALSE);
}

BOOL BCMenu::IsLunaMenuStyle()
{
	if(IsWinXPLuna()){
		if(xp_drawmode==BCMENU_DRAWMODE_XP)return(TRUE);
	}
	else{
		if(original_drawmode==BCMENU_DRAWMODE_XP)return(TRUE);
	}
	return(FALSE);
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
		CMenu *ptr=FromHandle(m_SubMenus[m]);
		if(ptr){
			BOOL flag=ptr->IsKindOf(RUNTIME_CLASS( BCMenu ));
			if(flag)delete((BCMenu *)ptr);
		}
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
	if(IsWinXPLuna())return(xp_drawmode);
	return(original_drawmode);
}

BOOL BCMenu::GetSelectDisableMode(void)
{
	if(IsLunaMenuStyle())return(xp_select_disabled);
	return(original_select_disabled);
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
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	if(pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE)DrawItem_Win9xNT2000(lpDIS);
	else{
		if(IsWinXPLuna()){
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
	COLORREF m_clrBack;

	if(IsWinXPLuna())m_clrBack=GetSysColor(COLOR_3DFACE);
	else m_clrBack=GetSysColor(COLOR_MENU);
	
	m_brBackground.CreateSolidBrush(m_clrBack);

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
		CBrush m_brSelect;
		CPen m_penBack;
		int x0,y0,dy;
		int nIconNormal=-1,xoffset=-1,global_offset=-1;
		CImageList *bitmap=NULL;
		
		// set some colors
		m_penBack.CreatePen (PS_SOLID,0,m_clrBack);
		m_brSelect.CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
		
		// draw the colored rectangle portion
		
		rect.CopyRect(&lpDIS->rcItem);
		rect2=rect;
		
		// draw the up/down/focused/disabled state
		
		UINT state = lpDIS->itemState;
		CString strText;
		
		if(lpDIS->itemData != NULL){
			nIconNormal = (((BCMenuData*)(lpDIS->itemData))->menuIconNormal);
			xoffset = (((BCMenuData*)(lpDIS->itemData))->xoffset);
			global_offset = (((BCMenuData*)(lpDIS->itemData))->global_offset);
			bitmap = (((BCMenuData*)(lpDIS->itemData))->bitmap);
			strText = ((BCMenuData*) (lpDIS->itemData))->GetString();

			if(nIconNormal<0&&global_offset>=0){
				xoffset=global_offset;
				nIconNormal=0;
				bitmap = &m_AllImages;
			}
			
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
				rect2.SetRect(rect.left+m_iconX+4+BCMENU_GAP,rect.top,rect.right,rect.bottom);
			pDC->FillRect (rect2,&m_brSelect);
			
			pDC->SelectObject (pOldPen);
			crText = GetSysColor(COLOR_HIGHLIGHTTEXT);
		}
		else {
			CPen *pOldPen = pDC->SelectObject (&m_penBack);
			pDC->FillRect (rect,&m_brBackground);
			pDC->SelectObject (pOldPen);
			
			// draw the up edges	
			pDC->Draw3dRect (rect,m_clrBack,m_clrBack);
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
					else{
						if(hicolor_bitmaps)
							DitherBlt3(pDC,rect.left+2,rect.top+2+dy,m_iconX,m_iconY,
							bitmapstandard,m_clrBack);
						else
							DitherBlt2(pDC,rect.left+2,rect.top+2+dy,m_iconX,m_iconY,
							bitmapstandard,0,0,m_clrBack);
					}
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
					COLORREF col = m_clrBack;
					col = LightenColor(col,0.6);
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
		rect.left = rect.left + m_iconX + 8 + BCMENU_GAP; 
		
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
			pDC->SetBkMode( iOldMode );
		}
		
		m_penBack.DeleteObject();
		m_brSelect.DeleteObject();
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

COLORREF BCMenu::DarkenColor(COLORREF col,double factor)
{
	if(factor>0.0&&factor<=1.0){
		BYTE red,green,blue,lightred,lightgreen,lightblue;
		red = GetRValue(col);
		green = GetGValue(col);
		blue = GetBValue(col);
		lightred = (BYTE)(red-(factor*red));
		lightgreen = (BYTE)(green-(factor*green));
		lightblue = (BYTE)(blue-(factor*blue));
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
	COLORREF m_newclrBack=GetSysColor(COLOR_3DFACE);
	COLORREF m_clrBack=GetSysColor(COLOR_WINDOW);
	m_clrBack=DarkenColor(m_clrBack,0.02);
	CFont m_fontMenu,*pFont=NULL;
	LOGFONT m_lf;
	if(!IsWinXPLuna())m_newclrBack=LightenColor(m_newclrBack,0.25);
	CBrush m_newbrBackground,m_brBackground;
	m_brBackground.CreateSolidBrush(m_clrBack);
	m_newbrBackground.CreateSolidBrush(m_newclrBack);
	int BCMENU_PAD=4;
	if(xp_draw_3D_bitmaps)BCMENU_PAD=7;
	int barwidth=m_iconX+BCMENU_PAD;
	
	// remove the selected bit if it's grayed out
	if(lpDIS->itemState & ODS_GRAYED&&!xp_select_disabled){
		if(lpDIS->itemState & ODS_SELECTED)lpDIS->itemState=lpDIS->itemState & ~ODS_SELECTED;
#ifdef BCMENU_USE_MEMDC
		pMemDC=new BCMenuMemDC(pDC,&lpDIS->rcItem);
		pDC = pMemDC;
		ZeroMemory ((PVOID) &m_lf,sizeof (LOGFONT));
		NONCLIENTMETRICS nm;
		nm.cbSize = sizeof (NONCLIENTMETRICS);
		VERIFY (SystemParametersInfo(SPI_GETNONCLIENTMETRICS,nm.cbSize,&nm,0)); 
		m_lf =  nm.lfMenuFont;
		m_fontMenu.CreateFontIndirect (&m_lf);
		pFont = pDC->SelectObject (&m_fontMenu);
#endif

	}
	
	if(state & MF_SEPARATOR){
		rect.CopyRect(&lpDIS->rcItem);
		pDC->FillRect (rect,&m_brBackground);
		rect2.SetRect(rect.left,rect.top,rect.left+barwidth,rect.bottom);
		rect.top+=rect.Height()>>1;
		rect.left = rect2.right+BCMENU_PAD;
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
		if(!IsWinXPLuna())crSelectFill=LightenColor(crSelect,0.85);
		else crSelectFill=LightenColor(crSelect,0.7);
		CBrush m_brSelect;
		CPen m_penBack;
		int x0,y0,dx,dy;
		int nIconNormal=-1,xoffset=-1,global_offset=-1;
		int faded_offset=1,shadow_offset=2,disabled_offset=3;
		CImageList *bitmap=NULL;
		BOOL CanDraw3D=FALSE;
		
		// set some colors
		m_penBack.CreatePen (PS_SOLID,0,m_clrBack);
		m_brSelect.CreateSolidBrush(crSelectFill);
		
		// draw the colored rectangle portion
		
		rect.CopyRect(&lpDIS->rcItem);
		rect2=rect;
		
		// draw the up/down/focused/disabled state
		
		UINT state = lpDIS->itemState;
		CString strText;
		
		if(lpDIS->itemData != NULL){
			nIconNormal = (((BCMenuData*)(lpDIS->itemData))->menuIconNormal);
			xoffset = (((BCMenuData*)(lpDIS->itemData))->xoffset);
			bitmap = (((BCMenuData*)(lpDIS->itemData))->bitmap);
			strText = ((BCMenuData*) (lpDIS->itemData))->GetString();
			global_offset = (((BCMenuData*)(lpDIS->itemData))->global_offset);

			if(xoffset==0&&xp_draw_3D_bitmaps&&bitmap&&bitmap->GetImageCount()>2)CanDraw3D=TRUE;

			if(nIconNormal<0&&xoffset<0&&global_offset>=0){
				xoffset=global_offset;
				nIconNormal=0;
				bitmap = &m_AllImages;
				if(xp_draw_3D_bitmaps&&CanDraw3DImageList(global_offset)){
					CanDraw3D=TRUE;
					faded_offset=global_offset+1;
					shadow_offset=global_offset+2;
					disabled_offset=global_offset+3;
				}
			}

			
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
		}
		else {
			rect2.SetRect(rect.left,rect.top,rect.left+barwidth,rect.bottom);
			CPen *pOldPen = pDC->SelectObject (&m_penBack);
			pDC->FillRect (rect,&m_brBackground);
			pDC->FillRect (rect2,&m_newbrBackground);
			pDC->SelectObject (pOldPen);
			
			// draw the up edges
			
			pDC->Draw3dRect (rect,m_clrBack,m_clrBack);
			pDC->Draw3dRect (rect2,m_newclrBack,m_newclrBack);
		}
		
		// draw the text if there is any
		//We have to paint the text only if the image is nonexistant
		
		dy = (int)(0.5+(rect.Height()-m_iconY)/2.0);
		dy = dy<0 ? 0 : dy;
		dx = (int)(0.5+(barwidth-m_iconX)/2.0);
		dx = dx<0 ? 0 : dx;
		rect2.SetRect(rect.left+1,rect.top+1,rect.left+barwidth-2,rect.bottom-1);
		
		if(checkflag||standardflag||selectedflag||disableflag){
			if(checkflag && checkmaps){
				pDC->FillRect (rect2,&m_newbrBackground);
				CPoint ptImage(rect.left+dx,rect.top+dy);		
				if(state&ODS_SELECTED)checkmaps->Draw(pDC,1,ptImage,ILD_TRANSPARENT);
				else checkmaps->Draw(pDC,0,ptImage,ILD_TRANSPARENT);
			}
			else if(disableflag){
				if(!selectedflag){
					if(CanDraw3D){
						CPoint ptImage(rect.left+dx,rect.top+dy);
						bitmap->Draw(pDC,disabled_offset,ptImage,ILD_TRANSPARENT);
					}
					else{
						CBitmap bitmapstandard;
						GetBitmapFromImageList(pDC,bitmap,xoffset,bitmapstandard);
						COLORREF transparentcol=m_newclrBack;
						if(state&ODS_SELECTED)transparentcol=crSelectFill;
						if(disable_old_style)
							DitherBlt(lpDIS->hDC,rect.left+dx,rect.top+dy,m_iconX,m_iconY,
							(HBITMAP)(bitmapstandard),0,0,transparentcol);
						else
							DitherBlt2(pDC,rect.left+dx,rect.top+dy,m_iconX,m_iconY,
							bitmapstandard,0,0,transparentcol);
						if(state&ODS_SELECTED)pDC->Draw3dRect (rect,crSelect,crSelect);
						bitmapstandard.DeleteObject();
					}
				}
			}
			else if(selectedflag){
				CPoint ptImage(rect.left+dx,rect.top+dy);
				if(state&ODS_CHECKED){
					CBrush brushin;
					brushin.CreateSolidBrush(LightenColor(crSelect,0.55));
					pDC->FillRect(rect2,&brushin);
					brushin.DeleteObject();
					pDC->Draw3dRect(rect2,crSelect,crSelect);
					ptImage.x-=1;ptImage.y-=1;
				}
				else pDC->FillRect (rect2,&m_brSelect);
				if(bitmap){
					if(CanDraw3D&&!(state&ODS_CHECKED)){
						CPoint ptImage1(ptImage.x+1,ptImage.y+1);
						CPoint ptImage2(ptImage.x-1,ptImage.y-1);
						bitmap->Draw(pDC,shadow_offset,ptImage1,ILD_TRANSPARENT);
						bitmap->Draw(pDC,xoffset,ptImage2,ILD_TRANSPARENT);
					}
					else bitmap->Draw(pDC,xoffset,ptImage,ILD_TRANSPARENT);
				}
			}
			else{
				if(state&ODS_CHECKED){
					CBrush brushin;
					brushin.CreateSolidBrush(LightenColor(crSelect,0.85));
					pDC->FillRect(rect2,&brushin);
					brushin.DeleteObject();
					pDC->Draw3dRect(rect2,crSelect,crSelect);
					CPoint ptImage(rect.left+dx-1,rect.top+dy-1);
					if(bitmap)bitmap->Draw(pDC,xoffset,ptImage,ILD_TRANSPARENT);
				}
				else{
					pDC->FillRect (rect2,&m_newbrBackground);
					pDC->Draw3dRect (rect2,m_newclrBack,m_newclrBack);
					CPoint ptImage(rect.left+dx,rect.top+dy);
					if(bitmap){
						if(CanDraw3D)
							bitmap->Draw(pDC,faded_offset,ptImage,ILD_TRANSPARENT);
						else
							bitmap->Draw(pDC,xoffset,ptImage,ILD_TRANSPARENT);
					}
				}
			}
		}
		if(nIconNormal<0 && state&ODS_CHECKED && !checkflag){
			CMenuItemInfo info;
			info.fMask = MIIM_CHECKMARKS;
			::GetMenuItemInfo((HMENU)lpDIS->hwndItem,lpDIS->itemID,
				MF_BYCOMMAND, &info);
			if(state&ODS_CHECKED || info.hbmpUnchecked) {
				DrawXPCheckmark(pDC, rect2,state&ODS_CHECKED ? info.hbmpChecked :
				info.hbmpUnchecked,crSelect,state&ODS_SELECTED);
			}
		}
		
		//This is needed always so that we can have the space for check marks
		
		x0=rect.left;y0=rect.top;
		rect.left = rect.left + barwidth + 8; 
		
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
				RECT offset = *rectt;
				offset.left+=1;
				offset.right+=1;
				offset.top+=1;
				offset.bottom+=1;
				if(!IsWinXPLuna()){
					COLORREF graycol=GetSysColor(COLOR_GRAYTEXT);
					if(!(state&ODS_SELECTED))graycol = LightenColor(graycol,0.4);
					pDC->SetTextColor(graycol);
				}
				else pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
				pDC->DrawText(leftStr,rectt, nFormat);
				if(tablocr!=-1) pDC->DrawText (rightStr,rectt,nFormatr);
			}
			pDC->SetBkMode( iOldMode );
		}
		
		m_penBack.DeleteObject();
		m_brSelect.DeleteObject();
	}
	m_brBackground.DeleteObject();
	m_newbrBackground.DeleteObject();
#ifdef BCMENU_USE_MEMDC
	if(pFont)pDC->SelectObject (pFont); //set it to the old font
	m_fontMenu.DeleteObject();
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
	COLORREF m_newclrBack;
	m_newclrBack=GetSysColor(COLOR_3DFACE);
	brush.CreateSolidBrush(m_newclrBack);
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
	::DestroyIcon(hIcon);
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
	int BCMENU_PAD=4;
	if(IsLunaMenuStyle()&&xp_draw_3D_bitmaps)BCMENU_PAD=7;
	if(state & MF_SEPARATOR){
		lpMIS->itemWidth = 0;
		int temp = GetSystemMetrics(SM_CYMENU)>>1;
		if(IsLunaMenuStyle())
			lpMIS->itemHeight = 3;
		else
			lpMIS->itemHeight = temp>(m_iconY+BCMENU_PAD)/2 ? temp : (m_iconY+BCMENU_PAD)/2;
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
		if (pWnd == NULL) pWnd = CWnd::GetDesktopWindow();
		CDC *pDC = pWnd->GetDC();              // Get device context
		CFont* pFont=NULL;    // Select menu font in...
		
		if (IsNewShell())
			pFont = pDC->SelectObject (&m_fontMenu);// Select menu font in...
        
		//Get pointer to text SK
		const wchar_t *lpstrText = ((BCMenuData*)(lpMIS->itemData))->GetWideString();//SK: we use const to prevent misuse
		    
		SIZE size;
		size.cx=size.cy=0;
		
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
		pWnd->ReleaseDC(pDC);  // Release the DC
		
		// Set width and height:
		
		if(IsLunaMenuStyle())lpMIS->itemWidth = m_iconX+BCMENU_PAD+8+t.cx;
		else lpMIS->itemWidth = m_iconX + t.cx + m_iconX + BCMENU_GAP;
		int temp = GetSystemMetrics(SM_CYMENU);
		lpMIS->itemHeight = temp>m_iconY+BCMENU_PAD ? temp : m_iconY+BCMENU_PAD;
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
	
	mdata->menuIconNormal = -1;
	mdata->xoffset = -1;
	
	if(nIconNormal>=0){
		CImageList bitmap;
		int xoffset=0;
		LoadFromToolBar(nID,nIconNormal,xoffset);
		if(mdata->bitmap){
			mdata->bitmap->DeleteImageList();
			mdata->bitmap=NULL;
		}
		bitmap.Create(m_iconX,m_iconY,ILC_COLORDDB|ILC_MASK,1,1);
		if(AddBitmapToImageList(&bitmap,nIconNormal)){
			mdata->global_offset = AddToGlobalImageList(&bitmap,xoffset,nID);
		}
	}
	else mdata->global_offset = GlobalImageListOffset(nID);

	mdata->nFlags = nFlags;
	mdata->nID = nID;
	BOOL returnflag=CMenu::AppendMenu(nFlags, nID, (LPCTSTR)mdata);
	if(m_loadmenu)RemoveTopLevelOwnerDraw();
	return(returnflag);
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

	int menustart=0;

	if(nFlags & MF_POPUP){
		if(m_loadmenu){
			menustart=GetMenuStart();
			if(nPosition<(UINT)menustart)menustart=0;
		}
		m_AllSubMenus.Add((HMENU)nID);
		m_SubMenus.Add((HMENU)nID);
	}

	//Stephane Clog suggested adding this, believe it or not it's in the help 
	if(nPosition==(UINT)-1)nPosition=GetMenuItemCount();
	
	BCMenuData *mdata = new BCMenuData;
	m_MenuList.InsertAt(nPosition-menustart,mdata);
	mdata->SetWideString(lpstrText);    //SK: modified for dynamic allocation
	
	mdata->menuIconNormal = nIconNormal;
	mdata->xoffset=-1;
	if(nIconNormal>=0){
		CImageList bitmap;
		int xoffset=0;
		LoadFromToolBar(nID,nIconNormal,xoffset);
		if(mdata->bitmap){
			mdata->bitmap->DeleteImageList();
			mdata->bitmap=NULL;
		}
		bitmap.Create(m_iconX,m_iconY,ILC_COLORDDB|ILC_MASK,1,1);
		if(AddBitmapToImageList(&bitmap,nIconNormal)){
			mdata->global_offset = AddToGlobalImageList(&bitmap,xoffset,nID);
		}
	}
	else mdata->global_offset = GlobalImageListOffset(nID);
	mdata->nFlags = nFlags;
	mdata->nID = nID;
	BOOL returnflag=CMenu::InsertMenu(nPosition,nFlags,nID,(LPCTSTR)mdata);
	if(m_loadmenu)RemoveTopLevelOwnerDraw();
	return(returnflag);
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
		m_SubMenus.Add((HMENU)nID);
	}
	
	//Stephane Clog suggested adding this, believe it or not it's in the help 
	if(nPosition==(UINT)-1)nPosition=GetMenuItemCount();
	
	BCMenuData *mdata = new BCMenuData;
	m_MenuList.InsertAt(nPosition,mdata);
	mdata->SetWideString(lpstrText);    //SK: modified for dynamic allocation
	
	mdata->menuIconNormal = -1;
	mdata->xoffset = -1;

	if(il){
		if(mdata->bitmap){
			mdata->bitmap->DeleteImageList();
			mdata->bitmap=NULL;
		}
		mdata->global_offset = AddToGlobalImageList(il,xoffset,nID);
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
		mdata->menuIconNormal = -1;
		mdata->xoffset = -1;
		if(nIconNormal>=0){
			CImageList bitmap;
			int xoffset=0;
			LoadFromToolBar(nID,nIconNormal,xoffset);
			if(mdata->bitmap){
				mdata->bitmap->DeleteImageList();
				mdata->bitmap=NULL;
			}
			bitmap.Create(m_iconX,m_iconY,ILC_COLORDDB|ILC_MASK,1,1);
			if(AddBitmapToImageList(&bitmap,nIconNormal)){
				mdata->global_offset = AddToGlobalImageList(&bitmap,xoffset,nID);
			}
		}
		else mdata->global_offset = GlobalImageListOffset(nID);
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
		mdata->menuIconNormal = -1;
		mdata->xoffset = -1;
		if(il){
			if(mdata->bitmap){
				mdata->bitmap->DeleteImageList();
				mdata->bitmap=NULL;
			}
			mdata->global_offset = AddToGlobalImageList(il,xoffset,nID);
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
BOOL BCMenu::ModifyODMenuA(const char * lpstrText,UINT nID,COLORREF fill,COLORREF border,int hatchstyle,CSize *pSize)
{
	USES_CONVERSION;
	return ModifyODMenuW(A2W(lpstrText),nID,fill,border,hatchstyle,pSize);
}

BOOL BCMenu::ModifyODMenuW(wchar_t *lpstrText,UINT nID,COLORREF fill,COLORREF border,int hatchstyle,CSize *pSize)
{
	CWnd *pWnd = AfxGetMainWnd();            // Get main window
	CDC *pDC = pWnd->GetDC();              // Get device context
	SIZE sz;
	if(!pSize){
		sz.cx = m_iconX;
		sz.cy = m_iconY;
	}
	else{
		sz.cx = pSize->cx;
		sz.cy = pSize->cy;
	}
	CSize bitmap_size(sz);
	CSize icon_size(m_iconX,m_iconY);
	CBitmap bmp;
	ColorBitmap(pDC,bmp,bitmap_size,icon_size,fill,border,hatchstyle);		
	pWnd->ReleaseDC(pDC);
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
	
	CWnd* pWnd = AfxGetMainWnd();
	if (pWnd == NULL)pWnd = CWnd::GetDesktopWindow();
	bar.Create(pWnd);
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
	
	CWnd* pWnd = AfxGetMainWnd();
	if (pWnd == NULL)pWnd = CWnd::GetDesktopWindow();
	bar.Create(pWnd);
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
	ASSERT_VALID(this);
	ASSERT(lpszResourceName != NULL);
	
	// Find the Menu Resource:
	HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName,RT_MENU);
	HRSRC hRsrc = ::FindResource(hInst,lpszResourceName,RT_MENU);
	if (hRsrc == NULL){
		hInst = NULL;
		hRsrc = ::FindResource(hInst,lpszResourceName,RT_MENU);
	}
	if(hRsrc == NULL)return FALSE;
	
	// Load the Menu Resource:
	
	HGLOBAL hGlobal = LoadResource(hInst, hRsrc);
	if(hGlobal == NULL)return FALSE;

	// first destroy the menu if we're trying to loadmenu again
	DestroyMenu();

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

	m_loadmenu=TRUE;
	
	return(TRUE);
}

int BCMenu::GetMenuStart(void)
{
	if(!m_loadmenu)return(0);

	CString name,str;
	int menuloc=-1,listloc=-1,menustart=0,i=0,j=0;
	int nummenulist=m_MenuList.GetSize();
	int nummenu=(int)GetMenuItemCount();

	while(i<nummenu&&menuloc==-1){
		GetMenuString (i, name, MF_BYPOSITION);
		if(name.GetLength()>0){
			for(j=0;j<nummenulist;++j){
				str=m_MenuList[j]->GetString();
				if(name==str){
					menuloc=i;
					listloc=j;
					break;
				}
			}
		}
		++i;
	}
	if(menuloc>=0&&listloc>=0&&menuloc>=listloc)menustart=menuloc-listloc;
	return(menustart);
}

void BCMenu::RemoveTopLevelOwnerDraw(void)
{
	CString str;
	int i=0,j=0;
	int nummenulist=m_MenuList.GetSize(),menustart;

	menustart=GetMenuStart();
	for(i=menustart,j=0;i<(int)GetMenuItemCount();++i,++j){
		if(j<nummenulist){
			str=m_MenuList[j]->GetString();
			if(GetSubMenu(i)){
				m_MenuList[j]->nFlags=MF_POPUP|MF_BYPOSITION;
				ModifyMenu(i,MF_POPUP|MF_BYPOSITION,
					(UINT)GetSubMenu(i)->m_hMenu,str);
			}
		}
	}

}

void BCMenu::InsertSpaces(void)
{
	if(IsLunaMenuStyle())
		if(!xp_space_accelerators)return;
	else
		if(!original_space_accelerators)return;
	
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
	if (pWnd == NULL)pWnd = CWnd::GetDesktopWindow();
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
	pWnd->ReleaseDC(pDC);       // Release the DC
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

void BCMenu::DrawCheckMark(CDC* pDC,int x,int y,COLORREF color,BOOL narrowflag)
{
	int dp=0;
	CPen m_penBack;
	m_penBack.CreatePen (PS_SOLID,0,color);
	CPen *pOldPen = pDC->SelectObject (&m_penBack);
	if(narrowflag)dp=1;

	pDC->MoveTo(x,y+2);
	pDC->LineTo(x,y+5-dp);
	
	pDC->MoveTo(x+1,y+3);
	pDC->LineTo(x+1,y+6-dp);
	
	pDC->MoveTo(x+2,y+4);
	pDC->LineTo(x+2,y+7-dp);
	
	pDC->MoveTo(x+3,y+3);
	pDC->LineTo(x+3,y+6-dp);
	
	pDC->MoveTo(x+4,y+2);
	pDC->LineTo(x+4,y+5-dp);
	
	pDC->MoveTo(x+5,y+1);
	pDC->LineTo(x+5,y+4-dp);
	
	pDC->MoveTo(x+6,y);
	pDC->LineTo(x+6,y+3-dp);
	
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

void BCMenu::GetFadedBitmap(CBitmap &bmp)
{
	CDC ddc;
	COLORREF bgcol,col;
	BITMAP BitMap;

	bmp.GetBitmap(&BitMap);
	ddc.CreateCompatibleDC(NULL);
	CBitmap * pddcOldBmp = ddc.SelectObject(&bmp);

	// use this to get the background color, takes into account color shifting
	CDC ddc2;
	CBrush brush;
	CBitmap bmp2;
	ddc2.CreateCompatibleDC(NULL);
	bmp2.CreateCompatibleBitmap(&ddc,BitMap.bmWidth,BitMap.bmHeight);
	col=GetSysColor(COLOR_3DFACE);
	brush.CreateSolidBrush(col);
	CBitmap * pddcOldBmp2 = ddc2.SelectObject(&bmp2);
	CRect rect(0,0,BitMap.bmWidth,BitMap.bmHeight);
	ddc2.FillRect(rect,&brush);
	bgcol=ddc2.GetPixel(1,1);
	brush.DeleteObject();
	ddc2.SelectObject(pddcOldBmp2);

	for(int i=0;i<BitMap.bmWidth;++i){
		for(int j=0;j<BitMap.bmHeight;++j){
			col=ddc.GetPixel(i,j);
			if(col!=bgcol)ddc.SetPixel(i,j,LightenColor(col,0.3));
		}
	}
	ddc.SelectObject(pddcOldBmp);
}

void BCMenu::GetTransparentBitmap(CBitmap &bmp)
{
	CDC ddc;
	COLORREF bgcol,col,newcol;
	BITMAP BitMap;

	bmp.GetBitmap(&BitMap);
	ddc.CreateCompatibleDC(NULL);
	CBitmap * pddcOldBmp = ddc.SelectObject(&bmp);

	// use this to get the background color, takes into account color shifting
	CDC ddc2;
	CBrush brush;
	CBitmap bmp2;
	ddc2.CreateCompatibleDC(NULL);
	bmp2.CreateCompatibleBitmap(&ddc,BitMap.bmWidth,BitMap.bmHeight);
	col=RGB(192,192,192);
	brush.CreateSolidBrush(col);
	CBitmap * pddcOldBmp2 = ddc2.SelectObject(&bmp2);
	CRect rect(0,0,BitMap.bmWidth,BitMap.bmHeight);
	ddc2.FillRect(rect,&brush);
	bgcol=ddc2.GetPixel(1,1);
	brush.DeleteObject();
	ddc2.SelectObject(pddcOldBmp2);
	newcol=GetSysColor(COLOR_3DFACE);

	for(int i=0;i<BitMap.bmWidth;++i){
		for(int j=0;j<BitMap.bmHeight;++j){
			col=ddc.GetPixel(i,j);
			if(col==bgcol)ddc.SetPixel(i,j,newcol);
		}
	}
	ddc.SelectObject(pddcOldBmp);
}

void BCMenu::GetDisabledBitmap(CBitmap &bmp,COLORREF background)
{
	CDC ddc;
	COLORREF bgcol,col,discol;
	BITMAP BitMap;

	bmp.GetBitmap(&BitMap);
	ddc.CreateCompatibleDC(NULL);
	CBitmap * pddcOldBmp = ddc.SelectObject(&bmp);

	// use this to get the background color, takes into account color shifting
	CDC ddc2;
	CBrush brush;
	CBitmap bmp2;
	ddc2.CreateCompatibleDC(NULL);
	bmp2.CreateCompatibleBitmap(&ddc,BitMap.bmWidth,BitMap.bmHeight);
	col=GetSysColor(COLOR_3DFACE);
	brush.CreateSolidBrush(col);
	CBitmap * pddcOldBmp2 = ddc2.SelectObject(&bmp2);
	CRect rect(0,0,BitMap.bmWidth,BitMap.bmHeight);
	ddc2.FillRect(rect,&brush);
	bgcol=ddc2.GetPixel(1,1);
	brush.DeleteObject();
	ddc2.SelectObject(pddcOldBmp2);
	discol=GetSysColor(COLOR_BTNSHADOW);

	for(int i=0;i<BitMap.bmWidth;++i){
		for(int j=0;j<BitMap.bmHeight;++j){
			col=ddc.GetPixel(i,j);
			if(col!=bgcol){
				BYTE r = GetRValue(col);
				BYTE g = GetGValue(col);
				BYTE b = GetBValue(col);
				int avgcol = (r+g+b)/3;
				double factor = avgcol/255.0;
				ddc.SetPixel(i,j,LightenColor(discol,factor));
			}
			else{
				if(background)ddc.SetPixel(i,j,background);
			}
		}
	}
	ddc.SelectObject(pddcOldBmp);
}

void BCMenu::GetShadowBitmap(CBitmap &bmp)
{
	CDC ddc;
	COLORREF bgcol,col,shadowcol=GetSysColor(COLOR_BTNSHADOW);
	BITMAP BitMap;

	if(!IsWinXPLuna())shadowcol=LightenColor(shadowcol,0.49);
	bmp.GetBitmap(&BitMap);
	ddc.CreateCompatibleDC(NULL);
	CBitmap * pddcOldBmp = ddc.SelectObject(&bmp);

	// use this to get the background color, takes into account color shifting
	CDC ddc2;
	CBrush brush;
	CBitmap bmp2;
	ddc2.CreateCompatibleDC(NULL);
	bmp2.CreateCompatibleBitmap(&ddc,BitMap.bmWidth,BitMap.bmHeight);
	col=GetSysColor(COLOR_3DFACE);
	brush.CreateSolidBrush(col);
	CBitmap * pddcOldBmp2 = ddc2.SelectObject(&bmp2);
	CRect rect(0,0,BitMap.bmWidth,BitMap.bmHeight);
	ddc2.FillRect(rect,&brush);
	bgcol=ddc2.GetPixel(1,1);
	brush.DeleteObject();
	ddc2.SelectObject(pddcOldBmp2);

	for(int i=0;i<BitMap.bmWidth;++i){
		for(int j=0;j<BitMap.bmHeight;++j){
			col=ddc.GetPixel(i,j);
			if(col!=bgcol)ddc.SetPixel(i,j,shadowcol);
		}
	}
	ddc.SelectObject(pddcOldBmp);
}


BOOL BCMenu::AddBitmapToImageList(CImageList *bmplist,UINT nResourceID)
{
	BOOL bReturn=FALSE;

	// O.S.
	if (m_bDynIcons){
		bmplist->Add((HICON)nResourceID);
		bReturn=TRUE;
	}
	else{
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
		else{ // a hicolor bitmap
			CBitmap mybmp;
			if(mybmp.LoadBitmap(nResourceID)){
				hicolor_bitmaps=TRUE;
				GetTransparentBitmap(mybmp);
				if(m_bitmapBackgroundFlag){
					if(bmplist->Add(&mybmp,m_bitmapBackground)>=0)bReturn=TRUE;
				}
				else{
					if(bmplist->Add(&mybmp,GetSysColor(COLOR_3DFACE))>=0)bReturn=TRUE;
				}
			}
		}
	}
	if(bReturn&&IsLunaMenuStyle()&&xp_draw_3D_bitmaps){
		CWnd *pWnd = AfxGetMainWnd();            // Get main window
		if (pWnd == NULL) pWnd = CWnd::GetDesktopWindow();
		CDC *pDC = pWnd->GetDC();              // Get device context
		CBitmap bmp,bmp2,bmp3;
		GetBitmapFromImageList(pDC,bmplist,0,bmp);
		GetFadedBitmap(bmp);
		bmplist->Add(&bmp,GetSysColor(COLOR_3DFACE));
		GetBitmapFromImageList(pDC,bmplist,0,bmp2);
		GetShadowBitmap(bmp2);
		bmplist->Add(&bmp2,GetSysColor(COLOR_3DFACE));
		GetBitmapFromImageList(pDC,bmplist,0,bmp3);
		GetDisabledBitmap(bmp3);
		bmplist->Add(&bmp3,GetSysColor(COLOR_3DFACE));
		pWnd->ReleaseDC(pDC);  // Release the DC
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
	COLORREF col;
	if(IsWinXPLuna())col=GetSysColor(COLOR_3DFACE);
	else col=GetSysColor(COLOR_MENU);
	if(!bSelected)col = LightenColor(col,0.6);
	brush.CreateSolidBrush(col);
	dc->FillRect(rcDest,&brush);
	brush.DeleteObject();
	if (IsNewShell()) //SK: looks better on the old shell
		dc->DrawEdge(&rcDest, BDR_SUNKENOUTER, BF_RECT);
	if (!hbmCheck)DrawCheckMark(dc,rc.left+4,rc.top+4,GetSysColor(COLOR_MENUTEXT));
	else DrawRadioDot(dc,rc.left+5,rc.top+4,GetSysColor(COLOR_MENUTEXT));
	return TRUE;
}

BOOL BCMenu::DrawXPCheckmark(CDC *dc, const CRect& rc, HBITMAP hbmCheck,COLORREF &colorout,BOOL selected)
{
	CBrush brushin;
	COLORREF col;
	int dx,dy;
	dy = (rc.Height()>>1)-3;
	dy = dy<0 ? 0 : dy;
	dx = (rc.Width()>>1)-3;
	dx = dx<0 ? 0 : dx;

	if(selected) col = LightenColor(colorout,0.55);
	else col = LightenColor(colorout,0.85);
	brushin.CreateSolidBrush(col);
	dc->FillRect(rc,&brushin);
	brushin.DeleteObject();
	dc->Draw3dRect (rc,colorout,colorout);

	if (!hbmCheck)DrawCheckMark(dc,rc.left+dx,rc.top+dy,GetSysColor(COLOR_MENUTEXT),TRUE);
	else DrawRadioDot(dc,rc.left+dx,rc.top+dy,GetSysColor(COLOR_MENUTEXT));
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

void BCMenu::DitherBlt3(CDC *drawdc, int nXDest, int nYDest, int nWidth, 
                        int nHeight, CBitmap &bmp,COLORREF bgcolor)
{
	GetDisabledBitmap(bmp,bgcolor);
	CDC dc;
	dc.CreateCompatibleDC(NULL);
	CBitmap * pdcOldBmp = dc.SelectObject(&bmp);
	drawdc->BitBlt(nXDest,nYDest,nWidth, nHeight, &dc,0,0,SRCCOPY);
	// reset DCs
	dc.SelectObject(pdcOldBmp);
	dc.DeleteDC();
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
	if (hRsrc == NULL){
		hInst = NULL;
		hRsrc = ::FindResource(hInst,MAKEINTRESOURCE(nResourceId),RT_BITMAP);
	}
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
				int i=0;
				for(i=num-1;i>=0;--i)pSubMenu->RemoveMenu(i,MF_BYPOSITION);
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
			// make sure it's a separator
			int menulistsize=m_MenuList.GetSize();	
			if(uiId<(UINT)menulistsize){
				CString str=m_MenuList[uiId]->GetString();
				if(str==""){
					delete m_MenuList.GetAt(uiId);
					m_MenuList.RemoveAt(uiId);
				}
			}
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
				int i=0;
				for(i=num-1;i>=0;--i)pSubMenu->DeleteMenu(i,MF_BYPOSITION);
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
		if(pMenu)return pMenu->DeleteMenu(iPosition,MF_BYPOSITION);
	}

	return(CMenu::DeleteMenu(uiId,nFlags));
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
	if(IsLunaMenuStyle()&&xp_draw_3D_bitmaps){
		CWnd *pWnd = AfxGetMainWnd();            // Get main window
		if (pWnd == NULL) pWnd = CWnd::GetDesktopWindow();
		CDC *pDC = pWnd->GetDC();              // Get device context
		CBitmap bmp,bmp2,bmp3;
		GetBitmapFromImageList(pDC,newlist,0,bmp);
		GetFadedBitmap(bmp);
		newlist->Add(&bmp,GetSysColor(COLOR_3DFACE));
		GetBitmapFromImageList(pDC,newlist,0,bmp2);
		GetShadowBitmap(bmp2);
		newlist->Add(&bmp2,GetSysColor(COLOR_3DFACE));
		GetBitmapFromImageList(pDC,newlist,0,bmp3);
		GetDisabledBitmap(bmp3);
		newlist->Add(&bmp3,GetSysColor(COLOR_3DFACE));
		pWnd->ReleaseDC(pDC);  // Release the DC
	}
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

BCMenu* BCMenu::GetSubBCMenu(char* pText)
{
	USES_CONVERSION;
	return GetSubBCMenu(A2W(pText));
}

BCMenu* BCMenu::GetSubBCMenu(wchar_t* lpszSubMenuName)
{
	BCMenuData *mdata;
	mdata=FindMenuOption(lpszSubMenuName);
	if(mdata){
		HMENU bchmenu=(HMENU)mdata->nID;
		CMenu *ptr=FromHandle(bchmenu);
		if(ptr){
			BOOL flag=ptr->IsKindOf(RUNTIME_CLASS( BCMenu ));
			if(flag)return((BCMenu *)ptr);
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

int BCMenu::DeleteMenu(char* pText, BC_Seperator sPos)
{
	USES_CONVERSION;
	return DeleteMenu(A2W(pText), sPos);
}

int BCMenu::DeleteMenu(wchar_t* pText, BC_Seperator sPos)
{
	int nPos = GetMenuPosition(pText);
	if(nPos != -1)
	{
		DeleteMenu(nPos, MF_BYPOSITION);
	}
	if(sPos == BCMENU_HEAD)
	{
		ASSERT(nPos - 1 >= 0);
		DeleteMenu(nPos-1, MF_BYPOSITION);
	}
	else if(sPos == BCMENU_TAIL)
	{
		DeleteMenu(nPos-1, MF_BYPOSITION);
	}
	else if(sPos == BCMENU_BOTH)
	{
		// remove the end first;
		DeleteMenu(nPos-1, MF_BYPOSITION);
		// remove the head;
		ASSERT(nPos - 1 >= 0);
		DeleteMenu(nPos-1, MF_BYPOSITION);
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
void BCMenu::ColorBitmap(CDC* pDC,CBitmap& bmp,CSize bitmap_size,CSize icon_size,COLORREF fill,COLORREF border,int hatchstyle)
{
	CDC bmpdc;
	COLORREF m_newclrBack;
	int x1=0,y1=0,x2=bitmap_size.cx,y2=bitmap_size.cy;

	if(IsWinXPLuna())m_newclrBack=GetSysColor(COLOR_3DFACE);
	else m_newclrBack=GetSysColor(COLOR_MENU);

	bmpdc.CreateCompatibleDC(pDC);

	bmp.CreateCompatibleBitmap(pDC, icon_size.cx, icon_size.cy);	
	CBitmap* pOldBitmap = bmpdc.SelectObject(&bmp);

	if(bitmap_size!=icon_size){
		CBrush background_brush;
		background_brush.CreateSolidBrush(m_newclrBack);
		CRect rect(0,0, icon_size.cx, icon_size.cy);
		bmpdc.FillRect(rect,&background_brush);
		x1 = (icon_size.cx-bitmap_size.cx)/2;
		y1 = (icon_size.cy-bitmap_size.cy)/2;
		x2 = x1+bitmap_size.cx;
		y2 = y1+bitmap_size.cy;
		background_brush.DeleteObject();
	}
	
	CPen border_pen(PS_SOLID, 1, border);
	CBrush fill_brush;
	if(hatchstyle!=-1) { fill_brush.CreateHatchBrush(hatchstyle, fill); }
	else      { fill_brush.CreateSolidBrush(fill);             }
	
	CPen*    pOldPen    = bmpdc.SelectObject(&border_pen);
	CBrush*  pOldBrush  = bmpdc.SelectObject(&fill_brush);

	bmpdc.Rectangle(x1,y1,x2,y2);

	if(NULL!=pOldBrush)  { bmpdc.SelectObject(pOldBrush);  }
	if(NULL!=pOldPen)    { bmpdc.SelectObject(pOldPen);    }
	if(NULL!=pOldBitmap) { bmpdc.SelectObject(pOldBitmap); }
}

BOOL BCMenu::IsWindowsClassicTheme(void)
{
	TCHAR Buf[_MAX_PATH+10];
	HKEY hKey;
	DWORD size,type; 
	long lRetCode; 
	static BOOL XPTheme_returnflag=FALSE;
	static BOOL XPTheme_checkflag=FALSE;
	
	if(XPTheme_checkflag)return(XPTheme_returnflag);

	XPTheme_checkflag=TRUE;
	lRetCode = RegOpenKeyEx ( HKEY_CURRENT_USER, 
		_T("Software\\Microsoft\\Plus!\\Themes\\Current"), 
		0,KEY_READ,&hKey);  
	if (lRetCode == ERROR_SUCCESS){ 
		size = _MAX_PATH;type=REG_SZ;
		lRetCode=::RegQueryValueEx(hKey,NULL,NULL,&type,
			(unsigned char *)Buf,&size);
		if(lRetCode == ERROR_SUCCESS){
			TCHAR szClassicTheme[]=_T("Windows Classic.theme");
			int len=lstrlen(Buf);
			if(len>=lstrlen(szClassicTheme)){
				if(!lstrcmpi(&Buf[len-lstrlen(szClassicTheme)],szClassicTheme)){
					XPTheme_returnflag=TRUE;
				}
			}
		}
		RegCloseKey(hKey);  
	}
	return(XPTheme_returnflag);
}

int BCMenu::GlobalImageListOffset(int nID)
{
	int numcurrent=m_AllImagesID.GetSize();
	int existsloc = -1;
	for(int i=0;i<numcurrent;++i){
		if(m_AllImagesID[i]==nID){
			existsloc=i;
			break;
		}
	}
	return existsloc;
}

BOOL BCMenu::CanDraw3DImageList(int offset)
{
	BOOL retflag=FALSE;
	int numcurrent=m_AllImagesID.GetSize();
	if(offset+1<numcurrent&&offset+2<numcurrent){
		int nID=m_AllImagesID[offset];
		if(m_AllImagesID[offset+1]==nID&&m_AllImagesID[offset+2]==nID)retflag=TRUE;
	}
	return(retflag);
}

int BCMenu::AddToGlobalImageList(CImageList *il,int xoffset,int nID)
{
	int loc = -1;
	HIMAGELIST hImageList = m_AllImages.m_hImageList;
	if(!hImageList){
		m_AllImages.Create(m_iconX,m_iconY,ILC_COLORDDB|ILC_MASK,1,1);
	}
	HICON hIcon = il->ExtractIcon(xoffset);
	if(hIcon){
		CBitmap bmp,bmp2,bmp3;
		if(IsLunaMenuStyle()&&xp_draw_3D_bitmaps){
			CWnd *pWnd = AfxGetMainWnd();            // Get main window
			if (pWnd == NULL) pWnd = CWnd::GetDesktopWindow();
			CDC *pDC = pWnd->GetDC();              // Get device context
			GetBitmapFromImageList(pDC,il,xoffset,bmp);
			GetFadedBitmap(bmp);
			GetBitmapFromImageList(pDC,il,xoffset,bmp2);
			GetShadowBitmap(bmp2);
			GetBitmapFromImageList(pDC,il,xoffset,bmp3);
			GetDisabledBitmap(bmp3);
			pWnd->ReleaseDC(pDC);  // Release the DC
		}
		int numcurrent=m_AllImagesID.GetSize();
		int existsloc = -1;
		for(int i=0;i<numcurrent;++i){
			if(m_AllImagesID[i]==nID){
				existsloc=i;
				break;
			}
		}
		if(existsloc>=0){
			m_AllImages.Replace(existsloc,hIcon);
			loc = existsloc;
			if(IsLunaMenuStyle()&&xp_draw_3D_bitmaps){
				if(existsloc+1<numcurrent&&m_AllImagesID[existsloc+1]==nID){
					if(existsloc+2<numcurrent&&m_AllImagesID[existsloc+2]==nID){
						CImageList il2;
						il2.Create(m_iconX,m_iconY,ILC_COLORDDB|ILC_MASK,1,1);
						il2.Add(&bmp,GetSysColor(COLOR_3DFACE));
						HICON hIcon2 = il2.ExtractIcon(0);
						m_AllImages.Replace(existsloc+1,hIcon2);
						il2.Add(&bmp2,GetSysColor(COLOR_3DFACE));
						HICON hIcon3 = il2.ExtractIcon(1);
						m_AllImages.Replace(existsloc+2,hIcon3);
						il2.Add(&bmp3,GetSysColor(COLOR_3DFACE));
						HICON hIcon4 = il2.ExtractIcon(2);
						m_AllImages.Replace(existsloc+3,hIcon4);
						::DestroyIcon(hIcon2);
						::DestroyIcon(hIcon3);
						::DestroyIcon(hIcon4);
					}
				}
			}
		}
		else{
			m_AllImages.Add(hIcon);
			m_AllImagesID.Add(nID);
			loc=numcurrent;
			if(IsLunaMenuStyle()&&xp_draw_3D_bitmaps){
				m_AllImages.Add(&bmp,GetSysColor(COLOR_3DFACE));
				m_AllImages.Add(&bmp2,GetSysColor(COLOR_3DFACE));
				m_AllImages.Add(&bmp3,GetSysColor(COLOR_3DFACE));
				m_AllImagesID.Add(nID);
				m_AllImagesID.Add(nID);
				m_AllImagesID.Add(nID);
			}
		}
		::DestroyIcon(hIcon);
	}
	return(loc);
}
