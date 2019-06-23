//*************************************************************************
// BCMenu.h : header file
// Version : 3.036
// Date : June 2005
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

#pragma once

#include <afxtempl.h>

// BCMenuData class. Fill this class structure to define a single menu item:
class BCMenuData
{
	wchar_t *m_szMenuText;
public:
	BCMenuData () {menuIconNormal=-1;xoffset=-1;bitmap=nullptr;pContext=nullptr;
	nFlags=0;nID=0;syncflag=0;m_szMenuText=nullptr;global_offset=-1;};
	void SetAnsiString(LPCSTR szAnsiString)
	{
		USES_CONVERSION;
		SetWideString(A2W(szAnsiString));  //SK:  see MFC Tech Note 059
	}
	void SetWideString(const wchar_t *szWideString);
	const wchar_t *GetWideString(void) {return m_szMenuText;};
	~BCMenuData ();
	CString GetString(void);//returns the menu text in ANSI or UNICODE
	INT_PTR xoffset,global_offset;
	int menuIconNormal;
	UINT nFlags,syncflag;
	UINT_PTR nID;
	CImageList *bitmap;
	void *pContext; // used to attach user data
};

struct CMenuItemInfo : public MENUITEMINFO 
{
	CMenuItemInfo() : MENUITEMINFO{ sizeof(MENUITEMINFO) }
	{
	}
};

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
#define SetImageForPopupFromToolbar SetImageForPopupFromToolbarA
#else
#define AppendMenu AppendMenuW
#define InsertMenu InsertMenuW
#define InsertODMenu InsertODMenuW
#define AppendODMenu AppendODMenuW
#define ModifyODMenu ModifyODMenuW
#define AppendODPopupMenu AppendODPopupMenuW
#define SetImageForPopupFromToolbar SetImageForPopupFromToolbarW
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
	bool LoadToolbar(UINT nToolBar);
	bool LoadToolbars(const UINT *arID,int n);
	bool LoadFromToolBar(UINT nID,UINT nToolBar,int& xoffset);
	bool AddBitmapToImageList(CImageList *list,UINT nResourceID,bool bDisabled=false);
	static HBITMAP LoadSysColorBitmap(int nResourceId);
	
	// functions for appending a menu option, use the AppendMenu call (see above define)
	bool AppendMenuA(UINT nFlags,UINT_PTR nIDNewItem=0,const char *lpszNewItem=nullptr,int nIconNormal=-1)
	{
		USES_CONVERSION;
		return AppendMenuW(nFlags, nIDNewItem, A2W(lpszNewItem), nIconNormal);
	}
	bool AppendMenuA(UINT nFlags,UINT_PTR nIDNewItem,const char *lpszNewItem,CImageList *il,int xoffset)
	{
		USES_CONVERSION;
		return AppendMenuW(nFlags,nIDNewItem,A2W(lpszNewItem),il,xoffset);
	}
	bool AppendMenuA(UINT nFlags,UINT_PTR nIDNewItem,const char *lpszNewItem,CBitmap *bmp)
	{
		USES_CONVERSION;
		return AppendMenuW(nFlags,nIDNewItem,A2W(lpszNewItem),bmp);
	}
	bool AppendMenuW(UINT nFlags,UINT_PTR nIDNewItem=0,const wchar_t *lpszNewItem=nullptr,int nIconNormal=-1);
	bool AppendMenuW(UINT nFlags,UINT_PTR nIDNewItem,const wchar_t *lpszNewItem,CImageList *il,int xoffset);
	bool AppendMenuW(UINT nFlags,UINT_PTR nIDNewItem,const wchar_t *lpszNewItem,CBitmap *bmp);
	bool AppendODMenuA(LPCSTR lpstrText,UINT nFlags,UINT_PTR nID,
							   int nIconNormal)
	{
		USES_CONVERSION;
		return AppendODMenuW(A2W(lpstrText),nFlags,nID,nIconNormal);//SK: See MFC Tech Note 059
	}
	bool AppendODMenuW(const wchar_t *lpstrText,UINT nFlags = MF_OWNERDRAW,UINT_PTR nID = 0,int nIconNormal = -1);  
	bool AppendODMenuA(LPCSTR lpstrText,UINT nFlags,UINT_PTR nID,
							   CImageList *il,int xoffset)
	{
		USES_CONVERSION;
		return AppendODMenuW(A2W(lpstrText),nFlags,nID,il,xoffset);
	}
	bool AppendODMenuW(const wchar_t *lpstrText,UINT nFlags,UINT_PTR nID,CImageList *il,int xoffset);
	bool AppendMenu (BCMenu* pMenuToAdd, bool add_separator = true, int num_items_to_remove_at_end = 0);
	
	// for appending a popup menu (see example application)
	BCMenu* AppendODPopupMenuA(LPCSTR lpstrText)
	{
		USES_CONVERSION;
		return AppendODPopupMenuW(A2W(lpstrText));
	}
	BCMenu* AppendODPopupMenuW(const wchar_t *lpstrText);

	// functions for inserting a menu option, use the InsertMenu call (see above define)
	bool InsertMenuA(UINT nPosition,UINT nFlags,UINT_PTR nIDNewItem,const char *lpszNewItem,CImageList *il,int xoffset)
	{
		USES_CONVERSION;
		return InsertMenuW(nPosition,nFlags,nIDNewItem,A2W(lpszNewItem),il,xoffset);
	}
	bool InsertMenuA(UINT nPosition,UINT nFlags,UINT_PTR nIDNewItem,const char *lpszNewItem,CBitmap *bmp)
	{
		USES_CONVERSION;
		return InsertMenuW(nPosition,nFlags,nIDNewItem,A2W(lpszNewItem),bmp);
	}
	bool InsertMenuA(UINT nPosition,UINT nFlags,UINT_PTR nIDNewItem,const char *lpszNewItem,int nIconNormal)
	{
		USES_CONVERSION;
		return InsertMenuW(nPosition,nFlags,nIDNewItem,A2W(lpszNewItem),nIconNormal);
	}
	bool InsertMenuW(UINT nPosition,UINT nFlags,UINT_PTR nIDNewItem=0,wchar_t *lpszNewItem=nullptr,int nIconNormal=-1);
	bool InsertMenuW(UINT nPosition,UINT nFlags,UINT_PTR nIDNewItem,wchar_t *lpszNewItem,CImageList *il,int xoffset);
	bool InsertMenuW(UINT nPosition,UINT nFlags,UINT_PTR nIDNewItem,wchar_t *lpszNewItem,CBitmap *bmp);
	bool InsertODMenuA(UINT nPosition,LPCSTR lpstrText,UINT nFlags,UINT_PTR nID,
							   int nIconNormal)
	{
		USES_CONVERSION;
		return InsertODMenuW(nPosition,A2W(lpstrText),nFlags,nID,nIconNormal);
	}
	bool InsertODMenuW(UINT nPosition,wchar_t *lpstrText,UINT nFlags = MF_OWNERDRAW,UINT_PTR nID = 0,int nIconNormal = -1);  
	bool InsertODMenuA(UINT nPosition,LPCSTR lpstrText,UINT nFlags,UINT_PTR nID,
							   CImageList *il,int xoffset)
	{
		USES_CONVERSION;
		return InsertODMenuW(nPosition,A2W(lpstrText),nFlags,nID,il,xoffset);
	}
	bool InsertODMenuW(UINT nPosition,wchar_t *lpstrText,UINT nFlags,UINT_PTR nID,CImageList *il,int xoffset);
	
	// functions for modifying a menu option, use the ModifyODMenu call (see above define)
	bool ModifyODMenuA(const char * lpstrText,UINT_PTR nID,int nIconNormal)
	{
		USES_CONVERSION;
		return ModifyODMenuW(A2W(lpstrText),nID,nIconNormal);//SK: see MFC Tech Note 059
	}
	bool ModifyODMenuA(const char * lpstrText,UINT_PTR nID,CImageList *il,int xoffset)
	{
		USES_CONVERSION;
		return ModifyODMenuW(A2W(lpstrText),nID,il,xoffset);
	}
	bool ModifyODMenuA(const char *lpstrText,UINT_PTR nID,CBitmap *bmp)
	{
		USES_CONVERSION;
		return ModifyODMenuW(A2W(lpstrText),nID,bmp);
	}
	bool ModifyODMenuA(const char *lpstrText,const char *OptionText,
							   int nIconNormal)
	{
		USES_CONVERSION;
		return ModifyODMenuW(A2W(lpstrText),A2W(OptionText),nIconNormal);//SK: see MFC  Tech Note 059
	}
	bool ModifyODMenuW(wchar_t *lpstrText,UINT_PTR nID=0,int nIconNormal=-1);
	bool ModifyODMenuW(wchar_t *lpstrText,UINT_PTR nID,CImageList *il,int xoffset);
	bool ModifyODMenuW(wchar_t *lpstrText,UINT_PTR nID,CBitmap *bmp);
	bool ModifyODMenuW(wchar_t *lpstrText,wchar_t *OptionText,int nIconNormal);

	bool SetImageForPopupFromToolbarA (const char *strPopUpText, UINT toolbarID, UINT command_id_to_extract_icon_from)
	{
		USES_CONVERSION;
		return SetImageForPopupFromToolbarW(A2W(strPopUpText),toolbarID,command_id_to_extract_icon_from);
	}
	bool SetImageForPopupFromToolbarW (wchar_t *strPopUpText, UINT toolbarID, UINT command_id_to_extract_icon_from);

	// for deleting and removing menu options
	bool	RemoveMenu(UINT uiId,UINT nFlags);
	bool	DeleteMenu(UINT uiId,UINT nFlags);
	// sPos means Seperator's position, since we have no way to find the seperator's position in the menu
	// we have to specify them when we call the RemoveMenu to make sure the unused seperators are removed;
	// sPos  = None no seperator removal;
	//       = Head  seperator in front of this menu item;
	//       = Tail  seperator right after this menu item;
	//       = Both  seperators at both ends;
	// remove the menu item based on their text, return -1 if not found, otherwise return the menu position;
	int RemoveMenu(char* pText, BC_Seperator sPos)
	{
		USES_CONVERSION;
		return RemoveMenu(A2W(pText), sPos);
	}
	int RemoveMenu(wchar_t* pText, BC_Seperator sPos=BCMENU_NONE);
	int DeleteMenu(char* pText, BC_Seperator sPos)
	{
		USES_CONVERSION;
		return DeleteMenu(A2W(pText), sPos);
	}
	int DeleteMenu(wchar_t* pText, BC_Seperator sPos=BCMENU_NONE);
	
	// Destoying
	virtual BOOL DestroyMenu();

	// function for retrieving and setting a menu options text (use this function
	// because it is ownerdrawn)
	bool GetMenuText(UINT id,CString &string,UINT nFlags = MF_BYPOSITION);
	bool SetMenuText(UINT id,CString string, UINT nFlags = MF_BYPOSITION);

	// Getting a submenu from it's name or position
	BCMenu* GetSubBCMenu(char* pText)
	{
		USES_CONVERSION;
		return GetSubBCMenu(A2W(pText));
	}
	BCMenu* GetSubBCMenu(wchar_t* lpszSubMenuName);
	CMenu* GetSubMenu (LPCTSTR lpszSubMenuName);
	CMenu* GetSubMenu (int nPos);
	int GetMenuPosition(char* pText)
	{
		USES_CONVERSION;
		return GetMenuPosition(A2W(pText));
	}
	int GetMenuPosition(wchar_t* pText);

	// Drawing: 
	virtual void DrawItem( LPDRAWITEMSTRUCT);  // Draw an item
	virtual void MeasureItem( LPMEASUREITEMSTRUCT );  // Measure an item

	// Static functions used for handling menu's in the mainframe
	static void UpdateMenu(CMenu *pmenu);
	static bool IsMenu(CMenu *submenu);
	static bool IsMenu(HMENU submenu);
	static LRESULT FindKeyboardShortcut(UINT nChar,UINT nFlags,CMenu *pMenu);

	// Customizing:
	// Set icon size
	static void SetIconSize (int, int); 
	// set the color in the bitmaps that is the background transparent color
	void SetBitmapBackground(COLORREF color);
	void UnSetBitmapBackground(void);
	// obsolete functions for setting how menu images are dithered for disabled menu options
	static inline COLORREF LightenColor(COLORREF col,double factor);

public:
	// Miscellaneous Protected Member functions
protected:
	BCMenuData *FindMenuItem(UINT_PTR nID);
	BCMenu *FindMenuOption(int nId,UINT& nLoc);
	BCMenu *FindAnotherMenuOption(int nId,UINT& nLoc,CArray<BCMenu*,BCMenu*>&bcsubs,
								  CArray<UINT,UINT&>&bclocs);
	BCMenuData *FindMenuOption(wchar_t *lpstrText);
	void DrawCheckMark(CDC* pDC,int x,int y,COLORREF color,bool narrowflag=false);
	void DrawRadioDot(CDC *pDC,int x,int y,COLORREF color);
	BCMenuData *NewODMenu(UINT pos,UINT nFlags,UINT_PTR nID,CString string);
	void SynchronizeMenu(void);
	void InitializeMenuList(int value);
	void DeleteMenuList(void);
	BCMenuData *FindMenuList(UINT_PTR nID);
	void DrawItem_Win9xNT2000 (LPDRAWITEMSTRUCT lpDIS);
	bool Draw3DCheckmark(CDC *dc, const CRect& rc,bool bSelected,HBITMAP hbmCheck);
	void DrawItem_Theme (LPDRAWITEMSTRUCT lpDIS);
	void DitherBlt2(CDC *drawdc, int nXDest, int nYDest, int nWidth, 
		int nHeight, CBitmap &bmp, int nXSrc, int nYSrc,COLORREF bgcolor);
	void DitherBlt3(CDC *drawdc, int nXDest, int nYDest, int nWidth, 
		int nHeight, CBitmap &bmp,COLORREF bgcolor);
	bool GetBitmapFromImageList(CDC* pDC,CImageList *imglist,int nIndex,CBitmap &bmp);
	bool ImageListDuplicate(CImageList *il,int xoffset,CImageList *newlist);
	static WORD NumBitmapColors(LPBITMAPINFOHEADER lpBitmap);
	void RemoveTopLevelOwnerDraw(void);
	int GetMenuStart(void);
	void GetTransparentBitmap(CBitmap &bmp);
	void GetDisabledBitmap(CBitmap &bmp,COLORREF background=0);
	INT_PTR AddToGlobalImageList(CImageList *il,int xoffset,int nID);
	INT_PTR AddToGlobalImageList(int nIconNormal,int nID);
	int GlobalImageListOffset(int nID);
	
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
	bool m_bitmapBackgroundFlag;
	static bool hicolor_bitmaps;
	bool m_loadmenu;
	static MARGINS m_marginCheck;
	static MARGINS m_marginSeparator;
	static SIZE m_sizeCheck;
	static SIZE m_sizeSeparator;
	static int m_textBorder;
	static int m_checkBgWidth;
	static int m_gutterWidth;
	static int m_arrowWidth;
	static HTHEME m_hTheme;
}; 

