//*************************************************************************
// BCMenu.h : header file
// Version : 3.033
// Date : April 2002
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

#ifndef BCMenu_H
#define BCMenu_H

#include <afxtempl.h>

// BCMenuData class. Fill this class structure to define a single menu item:
class BCMenuData
{
	wchar_t *m_szMenuText;
public:
	BCMenuData () {menuIconNormal=-1;xoffset=-1;bitmap=NULL;pContext=NULL;
	nFlags=0;nID=0;syncflag=0;m_szMenuText=NULL;global_offset=-1;};
	void SetAnsiString(LPCSTR szAnsiString);
	void SetWideString(const wchar_t *szWideString);
	const wchar_t *GetWideString(void) {return m_szMenuText;};
	~BCMenuData ();
	CString GetString(void);//returns the menu text in ANSI or UNICODE
	int xoffset,global_offset;
	int menuIconNormal;
	UINT nFlags,nID,syncflag;
	CImageList *bitmap;
	void *pContext; // used to attach user data
};

//struct CMenuItemInfo : public MENUITEMINFO {
struct CMenuItemInfo : public 
//MENUITEMINFO 
#ifndef UNICODE   //SK: this fixes warning C4097: typedef-name 'MENUITEMINFO' used as synonym for class-name 'tagMENUITEMINFOA'
tagMENUITEMINFOA
#else
tagMENUITEMINFOW
#endif
{
	CMenuItemInfo()
	{
		memset(this, 0, sizeof(MENUITEMINFO));
		cbSize = sizeof(MENUITEMINFO);
	}
};

// how the menu's are drawn, either original or XP style
typedef enum {BCMENU_DRAWMODE_ORIGINAL,BCMENU_DRAWMODE_XP} BC_MenuDrawMode;

// how seperators are handled when removing a menu (Tongzhe Cui)
typedef enum {BCMENU_NONE, BCMENU_HEAD, BCMENU_TAIL, BCMENU_BOTH} BC_Seperator;

// defines for unicode support
#ifndef UNICODE
#define AppendMenu AppendMenuA
#define InsertMenu InsertMenuA
#define InsertODMenu InsertODMenuA
#define AppendODMenu AppendODMenuA
#define AppendODPopupMenu AppendODPopupMenuA
#define ModifyODMenu ModifyODMenuA
#else
#define AppendMenu AppendMenuW
#define InsertMenu InsertMenuW
#define InsertODMenu InsertODMenuW
#define AppendODMenu AppendODMenuW
#define ModifyODMenu ModifyODMenuW
#define AppendODPopupMenu AppendODPopupMenuW
#endif


class BCMenu : public CMenu
{
	DECLARE_DYNAMIC( BCMenu )
public:
	BCMenu(); 
	virtual ~BCMenu();

	// Functions for loading and applying bitmaps to menus (see example application)
	virtual BOOL LoadMenu(LPCTSTR lpszResourceName);
	virtual BOOL LoadMenu(int nResource);
	BOOL LoadToolbar(UINT nToolBar);
	BOOL LoadToolbars(const UINT *arID,int n);
	void AddFromToolBar(CToolBar* pToolBar, int nResourceID);
	BOOL LoadFromToolBar(UINT nID,UINT nToolBar,int& xoffset);
	BOOL AddBitmapToImageList(CImageList *list,UINT nResourceID);
	static HBITMAP LoadSysColorBitmap(int nResourceId);
	void LoadCheckmarkBitmap(int unselect,int select); // custom check mark bitmaps
	
	// functions for appending a menu option, use the AppendMenu call (see above define)
	BOOL AppendMenuA(UINT nFlags,UINT nIDNewItem=0,const char *lpszNewItem=NULL,int nIconNormal=-1);
	BOOL AppendMenuA(UINT nFlags,UINT nIDNewItem,const char *lpszNewItem,CImageList *il,int xoffset);
	BOOL AppendMenuA(UINT nFlags,UINT nIDNewItem,const char *lpszNewItem,CBitmap *bmp);
	BOOL AppendMenuW(UINT nFlags,UINT nIDNewItem=0,wchar_t *lpszNewItem=NULL,int nIconNormal=-1);
	BOOL AppendMenuW(UINT nFlags,UINT nIDNewItem,wchar_t *lpszNewItem,CImageList *il,int xoffset);
	BOOL AppendMenuW(UINT nFlags,UINT nIDNewItem,wchar_t *lpszNewItem,CBitmap *bmp);
	BOOL AppendODMenuA(LPCSTR lpstrText,UINT nFlags = MF_OWNERDRAW,UINT nID = 0,int nIconNormal = -1);  
	BOOL AppendODMenuW(wchar_t *lpstrText,UINT nFlags = MF_OWNERDRAW,UINT nID = 0,int nIconNormal = -1);  
	BOOL AppendODMenuA(LPCSTR lpstrText,UINT nFlags,UINT nID,CImageList *il,int xoffset);
	BOOL AppendODMenuW(wchar_t *lpstrText,UINT nFlags,UINT nID,CImageList *il,int xoffset);
	
	// for appending a popup menu (see example application)
	BCMenu* AppendODPopupMenuA(LPCSTR lpstrText);
	BCMenu* AppendODPopupMenuW(wchar_t *lpstrText);

	// functions for inserting a menu option, use the InsertMenu call (see above define)
	BOOL InsertMenuA(UINT nPosition,UINT nFlags,UINT nIDNewItem=0,const char *lpszNewItem=NULL,int nIconNormal=-1);
	BOOL InsertMenuA(UINT nPosition,UINT nFlags,UINT nIDNewItem,const char *lpszNewItem,CImageList *il,int xoffset);
	BOOL InsertMenuA(UINT nPosition,UINT nFlags,UINT nIDNewItem,const char *lpszNewItem,CBitmap *bmp);
	BOOL InsertMenuW(UINT nPosition,UINT nFlags,UINT nIDNewItem=0,wchar_t *lpszNewItem=NULL,int nIconNormal=-1);
	BOOL InsertMenuW(UINT nPosition,UINT nFlags,UINT nIDNewItem,wchar_t *lpszNewItem,CImageList *il,int xoffset);
	BOOL InsertMenuW(UINT nPosition,UINT nFlags,UINT nIDNewItem,wchar_t *lpszNewItem,CBitmap *bmp);
	BOOL InsertODMenuA(UINT nPosition,LPCSTR lpstrText,UINT nFlags = MF_OWNERDRAW,UINT nID = 0,int nIconNormal = -1); 
	BOOL InsertODMenuW(UINT nPosition,wchar_t *lpstrText,UINT nFlags = MF_OWNERDRAW,UINT nID = 0,int nIconNormal = -1);  
	BOOL InsertODMenuA(UINT nPosition,LPCSTR lpstrText,UINT nFlags,UINT nID,CImageList *il,int xoffset);
	BOOL InsertODMenuW(UINT nPosition,wchar_t *lpstrText,UINT nFlags,UINT nID,CImageList *il,int xoffset);
	
	// functions for modifying a menu option, use the ModifyODMenu call (see above define)
	BOOL ModifyODMenuA(const char *lpstrText,UINT nID=0,int nIconNormal=-1);
	BOOL ModifyODMenuA(const char *lpstrText,UINT nID,CImageList *il,int xoffset);
	BOOL ModifyODMenuA(const char *lpstrText,UINT nID,CBitmap *bmp);
	BOOL ModifyODMenuA(const char *lpstrText,const char *OptionText,int nIconNormal);
	BOOL ModifyODMenuW(wchar_t *lpstrText,UINT nID=0,int nIconNormal=-1);
	BOOL ModifyODMenuW(wchar_t *lpstrText,UINT nID,CImageList *il,int xoffset);
	BOOL ModifyODMenuW(wchar_t *lpstrText,UINT nID,CBitmap *bmp);
	BOOL ModifyODMenuW(wchar_t *lpstrText,wchar_t *OptionText,int nIconNormal);
	// use this method for adding a solid/hatched colored square beside a menu option
	// courtesy of Warren Stevens
	BOOL ModifyODMenuA(const char *lpstrText,UINT nID,COLORREF fill,COLORREF border,int hatchstyle=-1,CSize *pSize=NULL);
	BOOL ModifyODMenuW(wchar_t *lpstrText,UINT nID,COLORREF fill,COLORREF border,int hatchstyle=-1,CSize *pSize=NULL);
	
	// for deleting and removing menu options
	BOOL	RemoveMenu(UINT uiId,UINT nFlags);
	BOOL	DeleteMenu(UINT uiId,UINT nFlags);
	// sPos means Seperator's position, since we have no way to find the seperator's position in the menu
	// we have to specify them when we call the RemoveMenu to make sure the unused seperators are removed;
	// sPos  = None no seperator removal;
	//       = Head  seperator in front of this menu item;
	//       = Tail  seperator right after this menu item;
	//       = Both  seperators at both ends;
	// remove the menu item based on their text, return -1 if not found, otherwise return the menu position;
	int RemoveMenu(char* pText, BC_Seperator sPos=BCMENU_NONE);
	int RemoveMenu(wchar_t* pText, BC_Seperator sPos=BCMENU_NONE);
	int DeleteMenu(char* pText, BC_Seperator sPos=BCMENU_NONE);
	int DeleteMenu(wchar_t* pText, BC_Seperator sPos=BCMENU_NONE);
	
	// Destoying
	virtual BOOL DestroyMenu();

	// function for retrieving and setting a menu options text (use this function
	// because it is ownerdrawn)
	BOOL GetMenuText(UINT id,CString &string,UINT nFlags = MF_BYPOSITION);
	BOOL SetMenuText(UINT id,CString string, UINT nFlags = MF_BYPOSITION);

	// Getting a submenu from it's name or position
	BCMenu* GetSubBCMenu(char* lpszSubMenuName);
	BCMenu* GetSubBCMenu(wchar_t* lpszSubMenuName);
	CMenu* GetSubMenu (LPCTSTR lpszSubMenuName);
	CMenu* GetSubMenu (int nPos);
	int GetMenuPosition(char* pText);
	int GetMenuPosition(wchar_t* pText);

	// Drawing: 
	virtual void DrawItem( LPDRAWITEMSTRUCT);  // Draw an item
	virtual void MeasureItem( LPMEASUREITEMSTRUCT );  // Measure an item

	// Static functions used for handling menu's in the mainframe
	static void UpdateMenu(CMenu *pmenu);
	static BOOL IsMenu(CMenu *submenu);
	static BOOL IsMenu(HMENU submenu);
	static LRESULT FindKeyboardShortcut(UINT nChar,UINT nFlags,CMenu *pMenu);

	// Function to set how menu is drawn, either original or XP style
	static void SetMenuDrawMode(UINT mode){
		BCMenu::original_drawmode=mode;
		BCMenu::xp_drawmode=mode;
	};
	// Function to set how disabled items are drawn (mode=FALSE means they are not drawn selected)
	static void SetSelectDisableMode(BOOL mode){
		BCMenu::original_select_disabled=mode;
		BCMenu::xp_select_disabled=mode;
	};

	static int BCMenu::GetMenuDrawMode(void);
	static BOOL BCMenu::GetSelectDisableMode(void);

	// how the bitmaps are drawn in XP Luna mode
	static void SetXPBitmap3D(BOOL val){
		BCMenu::xp_draw_3D_bitmaps=val;
	};
	static BOOL GetXPBitmap3D(void){return BCMenu::xp_draw_3D_bitmaps;}

	// Customizing:
	// Set icon size
	static void SetIconSize (int, int); 
	// set the color in the bitmaps that is the background transparent color
	void SetBitmapBackground(COLORREF color);
	void UnSetBitmapBackground(void);
	// obsolete functions for setting how menu images are dithered for disabled menu options
	BOOL GetDisableOldStyle(void);
	void SetDisableOldStyle(void);
	void UnSetDisableOldStyle(void);
	static COLORREF LightenColor(COLORREF col,double factor);
	static COLORREF DarkenColor(COLORREF col,double factor);

// Miscellaneous Protected Member functions
protected:
	static BOOL IsNewShell(void);
	static BOOL IsWinXPLuna(void);
	static BOOL IsLunaMenuStyle(void);
	static BOOL IsWindowsClassicTheme(void);
	BCMenuData *BCMenu::FindMenuItem(UINT nID);
	BCMenu *FindMenuOption(int nId,int& nLoc);
	BCMenu *FindAnotherMenuOption(int nId,int& nLoc,CArray<BCMenu*,BCMenu*>&bcsubs,
								  CArray<int,int&>&bclocs);
	BCMenuData *FindMenuOption(wchar_t *lpstrText);
	void InsertSpaces(void);
	void DrawCheckMark(CDC* pDC,int x,int y,COLORREF color,BOOL narrowflag=FALSE);
	void DrawRadioDot(CDC *pDC,int x,int y,COLORREF color);
	BCMenuData *NewODMenu(UINT pos,UINT nFlags,UINT nID,CString string);
	void SynchronizeMenu(void);
	void BCMenu::InitializeMenuList(int value);
	void BCMenu::DeleteMenuList(void);
	BCMenuData *BCMenu::FindMenuList(UINT nID);
	void DrawItem_Win9xNT2000 (LPDRAWITEMSTRUCT lpDIS);
	void DrawItem_WinXP (LPDRAWITEMSTRUCT lpDIS);
	BOOL Draw3DCheckmark(CDC *dc, const CRect& rc,BOOL bSelected,HBITMAP hbmCheck);
	BOOL DrawXPCheckmark(CDC *dc, const CRect& rc, HBITMAP hbmCheck,COLORREF &colorout,BOOL selected);
	void DitherBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, 
		int nHeight, HBITMAP hbm, int nXSrc, int nYSrc,COLORREF bgcolor);
	void DitherBlt2(CDC *drawdc, int nXDest, int nYDest, int nWidth, 
		int nHeight, CBitmap &bmp, int nXSrc, int nYSrc,COLORREF bgcolor);
	void DitherBlt3(CDC *drawdc, int nXDest, int nYDest, int nWidth, 
		int nHeight, CBitmap &bmp,COLORREF bgcolor);
	BOOL GetBitmapFromImageList(CDC* pDC,CImageList *imglist,int nIndex,CBitmap &bmp);
	BOOL ImageListDuplicate(CImageList *il,int xoffset,CImageList *newlist);
	static WORD NumBitmapColors(LPBITMAPINFOHEADER lpBitmap);
	void ColorBitmap(CDC* pDC, CBitmap& bmp,CSize bitmap_size,CSize icon_size,COLORREF fill,COLORREF border,int hatchstyle=-1);
	void RemoveTopLevelOwnerDraw(void);
	int GetMenuStart(void);
	void GetFadedBitmap(CBitmap &bmp);
	void GetTransparentBitmap(CBitmap &bmp);
	void GetDisabledBitmap(CBitmap &bmp,COLORREF background=0);
	void GetShadowBitmap(CBitmap &bmp);
	int AddToGlobalImageList(CImageList *il,int xoffset,int nID);
	int GlobalImageListOffset(int nID);
	BOOL CanDraw3DImageList(int offset);
	
// Member Variables
protected:
	CTypedPtrArray<CPtrArray, BCMenuData*> m_MenuList;  // Stores list of menu items 
	// When loading an owner-drawn menu using a Resource, BCMenu must keep track of
	// the popup menu's that it creates. Warning, this list *MUST* be destroyed
	// last item first :)
	CTypedPtrArray<CPtrArray, HMENU>  m_SubMenus;  // Stores list of sub-menus 
	// Stores a list of all BCMenu's ever created 
	static CTypedPtrArray<CPtrArray, HMENU>  m_AllSubMenus;
	// Global ImageList
	static CImageList m_AllImages;
	static CArray<int,int&> m_AllImagesID;
	// icon size
	static int m_iconX;
	static int m_iconY;
	COLORREF m_bitmapBackground;
	BOOL m_bitmapBackgroundFlag;
	BOOL disable_old_style;
	static UINT original_drawmode;
	static BOOL original_select_disabled;
	static UINT xp_drawmode;
	static BOOL xp_select_disabled;
	static BOOL xp_draw_3D_bitmaps;
	static BOOL hicolor_bitmaps;
	static BOOL xp_space_accelerators;
	static BOOL original_space_accelerators;
	CImageList *checkmaps;
	BOOL checkmapsshare;
	int m_selectcheck;
	int m_unselectcheck;
	BOOL m_bDynIcons;
	BOOL m_loadmenu;
}; 

#define BCMENU_USE_MEMDC

#ifdef BCMENU_USE_MEMDC
//////////////////////////////////////////////////
// BCMenuMemDC - memory DC
//
// Author: Keith Rule
// Email:  keithr@europa.com
// Copyright 1996-1997, Keith Rule
//
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// History - 10/3/97 Fixed scrolling bug.
//                   Added print support.
//           25 feb 98 - fixed minor assertion bug
//
// This class implements a memory Device Context

class BCMenuMemDC : public CDC
{
public:

    // constructor sets up the memory DC
    BCMenuMemDC(CDC* pDC,LPCRECT lpSrcRect) : CDC()
    {
        ASSERT(pDC != NULL);

		m_rect.CopyRect(lpSrcRect);
        m_pDC = pDC;
        m_pOldBitmap = NULL;
        m_bMemDC = !pDC->IsPrinting();
              
        if (m_bMemDC)    // Create a Memory DC
        {
            CreateCompatibleDC(pDC);
            m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
            m_pOldBitmap = SelectObject(&m_bitmap);
            SetWindowOrg(m_rect.left, m_rect.top);
        }
        else        // Make a copy of the relevent parts of the current DC for printing
        {
            m_bPrinting = pDC->m_bPrinting;
            m_hDC       = pDC->m_hDC;
            m_hAttribDC = pDC->m_hAttribDC;
        }
    }
    
    // Destructor copies the contents of the mem DC to the original DC
    ~BCMenuMemDC()
    {
        if (m_bMemDC) 
        {    
            // Copy the offscreen bitmap onto the screen.
            m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
                          this, m_rect.left, m_rect.top, SRCCOPY);

            //Swap back the original bitmap.
            SelectObject(m_pOldBitmap);
        } else {
            // All we need to do is replace the DC with an illegal value,
            // this keeps us from accidently deleting the handles associated with
            // the CDC that was passed to the constructor.
            m_hDC = m_hAttribDC = NULL;
        }
    }

    // Allow usage as a pointer
    BCMenuMemDC* operator->() {return this;}
        
    // Allow usage as a pointer
    operator BCMenuMemDC*() {return this;}

private:
    CBitmap  m_bitmap;      // Offscreen bitmap
    CBitmap* m_pOldBitmap;  // bitmap originally found in BCMenuMemDC
    CDC*     m_pDC;         // Saves CDC passed in constructor
    CRect    m_rect;        // Rectangle of drawing area.
    BOOL     m_bMemDC;      // TRUE if CDC really is a Memory DC.
};

#endif

#endif

//*************************************************************************
