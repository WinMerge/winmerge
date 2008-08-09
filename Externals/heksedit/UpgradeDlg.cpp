#include "precomp.h"
#include "resource.h"
#include <shlwapi.h>
#include "regtools.h"
#include "hexwnd.h"
#include "hexwdlg.h"

#define nibble2hex(c) ( (c) < 10 ? (c) + '0': (c) - 10 + 'a' )
#define NIBBLE2HEX(c) ( (c) < 10 ? (c) + '0': (c) - 10 + 'A' )

struct DispDataStruct
{
	int iTextColorValue,iBkColorValue,iSepColorValue,iSelTextColorValue,iSelBkColorValue,iBmkColor,//Color values
	iBytesPerLine,iOffsetLen,iFontSize,//signed integers
	iAutomaticBPL,bAutoOffsetLen,bOpenReadOnly,bMakeBackups,//Bool
	iWindowShowCmd,iCharacterSet;//Multiple different values
} DispData;

BOOL UpgradeDlg::OnInitDialog(HWND hw)
{
	int i;
	HKEY hk;
	//LONG res;
	char subkeynam[_MAX_PATH+1];
	LVITEM item;
	ZeroMemory(&item,sizeof(LVITEM));
	item.mask = LVIF_TEXT ;
	item.pszText = subkeynam;

	DWORD exstyle = LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP;
	HWND list = GetDlgItem(hw,IDC_VERS);
	ListView_DeleteColumn(list,0);
	ListView_DeleteAllItems(list);
	ListView_SetExtendedListViewStyle(list,exstyle);

	//Add a column
	LVCOLUMN col;
	ZeroMemory(&col,sizeof(col));
	col.mask = LVCF_TEXT|LVCF_WIDTH ;
	col.fmt = LVCFMT_LEFT;
	col.pszText = "HKCU\\Software\\frhed";
	col.cx = 165;
	ListView_InsertColumn(list,0,&col);

	//Fill the vers list with the various versions
	if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\frhed", 0, KEY_ALL_ACCESS, &hk) == 0)
	{
		i = 0;
		while (RegEnumKey(hk, i, subkeynam, _MAX_PATH+1) != ERROR_NO_MORE_ITEMS)
		{
			item.iItem = i++;
			ListView_InsertItem(list, &item);
		}
		RegCloseKey(hk);
	}
	list = GetDlgItem(hw,IDC_INSTS);
	ListView_DeleteColumn(list,0);
	ListView_DeleteAllItems(list);
	ListView_SetExtendedListViewStyle(list,exstyle);

	list = GetDlgItem(hw,IDC_INSTDATA);
	ListView_DeleteColumn(list,1);
	ListView_DeleteColumn(list,0);
	ListView_DeleteAllItems(list);
	ZeroMemory(&col,sizeof(col));
	col.mask = LVCF_TEXT|LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.cx = 105;
	col.pszText = "Option";
	ListView_InsertColumn(list,0,&col);
	ZeroMemory(&col,sizeof(col));
	col.mask = LVCF_TEXT|LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.cx = 105;
	col.pszText = "Value";
	ListView_InsertColumn(list,1,&col);

	list = GetDlgItem(hw,IDC_LINKS);
	ListView_DeleteColumn(list,0);
	ListView_DeleteAllItems(list);
	ZeroMemory(&col,sizeof(col));
	col.mask = LVCF_TEXT|LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.cx = 155;
	col.pszText = "Links";
	ListView_InsertColumn(list,0,&col);

	list = GetDlgItem(hw,IDC_MRU);
	ListView_DeleteColumn(list,0);
	ListView_DeleteAllItems(list);
	ZeroMemory(&col,sizeof(col));
	col.mask = LVCF_TEXT|LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.cx = 185;
	col.pszText = "MRU Files";
	ListView_InsertColumn(list,0,&col);
	HICON hIcon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1));
	SendDlgItemMessage(hw, IDC_DISPLAY, WM_SETICON, 1, (LPARAM)hIcon);
	return TRUE;
}

BOOL UpgradeDlg::OnCommand(HWND hw, WPARAM w, LPARAM l)
{
	HWND insts = GetDlgItem(hw,IDC_INSTS);
	char keynam[_MAX_PATH+1]="Software\\frhed\\";
	LVCOLUMN col;
	switch (w)
	{
	case IDCANCEL:
		EndDialog(hw, 0);
		return TRUE;
	case IDC_COPY:
		{
			HKEY lk;
			HWND vers = GetDlgItem(hw,IDC_VERS);
			int i = ListView_GetNextItem(vers, (UINT)-1, LVNI_SELECTED);
			ListView_GetItemText(vers,i,0,&keynam[15],_MAX_PATH+1-15);
			if(!strcmp(&keynam[15],"v"CURRENT_VERSION"."SUB_RELEASE_NO)){
				//If that key was this version don't copy
				MessageBox(hw,
					"You can't copy the registry data of the selected version\n"
					"to the current one because it is the current one!", "Copy",MB_OK);
				return 0;
			}

			//Open the reg key to load from
			if(ERROR_SUCCESS==RegOpenKey(HKEY_CURRENT_USER,keynam,&lk)){
				char cver[_MAX_PATH+1]="Software\\frhed\\v"CURRENT_VERSION"."SUB_RELEASE_NO"\\";
				int i,numi=ListView_GetItemCount(insts),len,lenc=strlen(cver);
				strcat(keynam,"\\");
				for(i=0;i<numi;i++){
					if(ListView_GetCheckState(insts,i)){
						len = strlen(keynam);
						ListView_GetItemText(insts,i,0,&keynam[len],_MAX_PATH+1-len);//get the inst
						strcpy(&cver[lenc],&keynam[len]);
						RegCopyValues(HKEY_CURRENT_USER,keynam,HKEY_CURRENT_USER,cver);//copy the key
						keynam[len]=cver[lenc]=0;
					}//if cur inst checked
				}//loop insts
				RegCloseKey(lk);
				SendMessage(hw,WM_INITDIALOG,0,0);//Readd all the instances
			}
			else MessageBox(hw,"Could not open the selected version key","Copy",MB_OK);
		}
		return 0;
	case IDC_READ:
		{
			//Get the instance
			int i = ListView_GetNextItem(insts, (UINT)-1, LVNI_SELECTED);
			char text[_MAX_PATH+1];
			ListView_GetItemText(insts,i,0,text,_MAX_PATH+1);
			//Get the version
			ZeroMemory(&col,sizeof(col));
			col.mask = LVCF_TEXT;
			col.pszText = keynam;
			col.cchTextMax = _MAX_PATH+1;
			ListView_GetColumn(insts,0,&col);
			//Save the current instance
			int tmp = iInstCount;
			//Set the instance to read from
			iInstCount = atoi(text);
			//Read the data
			read_ini_data(keynam);
			//Reset the instance
			iInstCount = tmp;
			resize_window();
		}
		break;
	case IDC_DELETE:
		{
			HWND vers = GetDlgItem(hw,IDC_VERS);
			int v,i,numv=ListView_GetItemCount(vers),numi=ListView_GetItemCount(insts),len;
			for(v=0;v<numv;v++){
				if(ListView_GetCheckState(vers,v)){
					for(i=0;i<numi;i++){
						if(ListView_GetCheckState(insts,i)){
							ListView_GetItemText(vers,v,0,&keynam[15],_MAX_PATH+1-15);//get the ver
							strcat(keynam,"\\");
							len = strlen(keynam);
							ListView_GetItemText(insts,i,0,&keynam[len],_MAX_PATH+1-len);//get the inst
							RegDeleteKey(HKEY_CURRENT_USER,keynam);//delete the key
							keynam[len-1]=0;//cut off the "\\<inst>"
							SHDeleteEmptyKey(HKEY_CURRENT_USER,keynam);//Delete an empty key
							if (strcmp(&keynam[15],"v"CURRENT_VERSION"."SUB_RELEASE_NO) == 0)
								bSaveIni = 0;//If that key was this version don't save
						}//if cur inst checked
					}//loop insts
				}//if cur ver checked
			}//loop vers
			SendMessage(hw,WM_INITDIALOG,0,0);//Readd all the instances
		}
		break;
	}//switch ctrl id
	return TRUE;
}

BOOL UpgradeDlg::OnNotify(HWND hw, WPARAM w, LPARAM l)
{
	NMHDR *nmh = (NMHDR *)l;
	if (nmh->code == LVN_ITEMCHANGED)
	{
		NMLISTVIEW *nml = (NMLISTVIEW*)l;
		if (nml->uChanged == LVIF_STATE &&
			(nml->uNewState & LVIS_FOCUSED) > (nml->uOldState & LVIS_FOCUSED))
		{
			char text[_MAX_PATH+1];
			switch (nmh->idFrom)
			{
			case IDC_VERS:
				ListView_GetItemText(nmh->hwndFrom, nml->iItem, 0, text, _MAX_PATH + 1);
				ChangeSelVer(hw, text);
				break;
			case IDC_INSTS:
				ListView_GetItemText(nmh->hwndFrom, nml->iItem, 0, text, _MAX_PATH + 1);
				ChangeSelInst(hw, text);
				break;
			}
		}
	}
	return TRUE;
}

BOOL UpgradeDlg::OnDrawitem(HWND, WPARAM, LPARAM l)
{
	DRAWITEMSTRUCT *pdis = (DRAWITEMSTRUCT *)l;
	HWND hw = pdis->hwndItem;
	HDC dc = pdis->hDC;
	RECT rt;
	GetClientRect(hw,&rt);
	FillRect(dc, &rt, GetSysColorBrush(COLOR_BTNFACE));
	if (GetWindowTextLength(hw))
	{
		rt.bottom = rt.top + 18;
		DrawCaption(hw, dc, &rt, DC_ACTIVE|DC_ICON|DC_TEXT);
		rt.left = 150; rt.right = rt.left + 16; rt.bottom--; rt.top++;
		UINT type;
		switch (DispData.iWindowShowCmd)
		{
		case SW_HIDE:
		case SW_MINIMIZE:
		case SW_SHOWMINIMIZED:
		case SW_SHOWMINNOACTIVE:
			type = DFCS_CAPTIONMIN;
			break;
		case SW_RESTORE:
		case SW_SHOWNORMAL:
		case SW_SHOW:
		case SW_SHOWNA:
		case SW_SHOWNOACTIVATE:
			type = DFCS_CAPTIONRESTORE;
			break;
		case SW_SHOWMAXIMIZED:
			type = DFCS_CAPTIONMAX;
			break;
		}
		DrawFrameControl(dc, &rt, DFC_CAPTION, type);

	//-------------Draw the status bar-----------------------------------------
		GetClientRect(hw,&rt);
		rt.top = rt.bottom-18;
		DrawEdge (dc, &rt, BDR_SUNKENOUTER, BF_RECT);
		HFONT fon = (HFONT) SendMessage(GetParent(hw),WM_GETFONT,0,0);
		HFONT ofon = (HFONT) SelectObject(dc,fon);
		char statusbuf[]="ANSI / READ";int i=0,len=11;
		if(DispData.iCharacterSet!=ANSI_FIXED_FONT){
			statusbuf[1]='O';
			statusbuf[2]='E';
			statusbuf[3]='M';
			i++;len--;
		}
		if(!DispData.bOpenReadOnly){
			statusbuf[7]=0;
			strcat(statusbuf,"OVR");
			len--;
		}
		SIZE s;
		GetTextExtentPoint32(dc,&statusbuf[i],len,&s);
		int mode = SetBkMode(dc,TRANSPARENT);
		UINT align = SetTextAlign(dc,TA_CENTER);
		TextOut(dc,(rt.left+rt.right)/2,(rt.top+rt.bottom-s.cy)/2,&statusbuf[i],len);
		SetTextAlign(dc,align);
		SetBkMode(dc,mode);
		SelectObject(dc,ofon);

	//-------------Draw the border---------------------------------------------
		GetClientRect(hw,&rt);
		rt.top+=19;
		rt.bottom-=20;
		DrawEdge (dc, &rt, EDGE_SUNKEN, BF_RECT);

	//-------------Draw hex contents-------------------------------------------
		//Print 1 row unselected, 1 row selected, 2 bookmarks & separators
		rt.left+=2;rt.top+=2;
		rt.right-=2;rt.bottom-=2;
		//Create the font & stick in the DC
		int nHeight = -MulDiv(DispData.iFontSize, GetDeviceCaps(dc, LOGPIXELSY), 72);
		int cset = ( DispData.iCharacterSet==ANSI_FIXED_FONT ? ANSI_CHARSET : OEM_CHARSET);
		fon = CreateFont (nHeight,0,0,0,0,0,0,0,cset,OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH | FF_DONTCARE,0);
		ofon = (HFONT) SelectObject (dc, fon);

		//Set the text & back colours for non-selected
		SetBkColor(dc,DispData.iBkColorValue);
		SetTextColor(dc,DispData.iTextColorValue);

		//Create the text
		int p;
		TEXTMETRIC tm;
		GetTextMetrics (dc, &tm);
		int tmp = DispData.iBytesPerLine;
		if (DispData.iAutomaticBPL)
		{
			//Get the number of chars that will fit into the box
			tmp = len = (rt.right - rt.left) / tm.tmAveCharWidth;
			len++;//'\0'
			//Get the number of chars to print
			tmp -= DispData.iOffsetLen + 3;//3 spaces
			tmp /= 4;//"ff " & "ÿ" = 4 chars so numchars = leftovers / 4
		}
		//length of last offset
		//Offset len, 3 spaces & '\0', numchars * 4 (2 hex 1 space & character) - \0 not printed
		int mol = 1;
		while (tmp & ~0 << 4 * mol)
			++mol;
		if (mol < DispData.iOffsetLen)
			mol = DispData.iOffsetLen;
		len = mol + 4 + 4 * tmp;
		char* linebuf = new char[len];
		if (!linebuf)
			return 0;
		//Offset & 2 spaces
		int tol = DispData.bAutoOffsetLen ? mol : DispData.iOffsetLen;
		sprintf(linebuf, "%*.*x", tol, tol, 0);
		p = strlen(linebuf);
		mol += 2;
		memset(linebuf + p, ' ', mol - p);
		linebuf[mol]=0;
		p=strlen(linebuf);
		//numchars
		for (i = 0 ; i < tmp ; i++)
		{
			int ii = (i >> 4) & 0x0f;
			linebuf[p++] = (char)nibble2hex(ii);
			ii = i & 0x0f;
			linebuf[p++] = (char)nibble2hex(ii);
			linebuf[p++] = ' ';
		}
		linebuf[p++] = ' ';
		for (i = 0 ; i < tmp ; i++)
		{
			linebuf[p++] =
			(
				DispData.iCharacterSet == OEM_FIXED_FONT && i != 0 ||
				(i >= 32 && i <= 126 || i >= 160 && i <= 255 || i >= 145 && i <= 146)
			) ? (char)i : '.';
		}
		linebuf[p] = 0;

		SetBkMode(dc,OPAQUE);

		//Draw the non-selected text
		ExtTextOut(dc,rt.left,rt.top,ETO_CLIPPED|ETO_OPAQUE,&rt, linebuf, p,NULL);

		rt.top-=nHeight;

		//Set the text & back colours for selected text
		SetBkColor(dc,DispData.iSelBkColorValue);
		SetTextColor(dc,DispData.iSelTextColorValue);

		//Create the text
		sprintf(linebuf, "%*.*x", tol, tol, tmp);
		p = strlen(linebuf);
		memset(linebuf+p,' ',mol-p);
		linebuf[mol]=0;
		mol -= 2;
		p=strlen(linebuf);
		//numchars
		for (i = 0 ; i < tmp - 1 ; i++)
		{
			int ii = ((tmp+i)>>4)&0x0f;
			linebuf[p++] = (char)nibble2hex(ii);
			ii = (tmp+i)&0x0f;
			linebuf[p++] = (char)nibble2hex(ii);
			linebuf[p++] = ' ';
		}
		linebuf[p++] = '_';
		linebuf[p++] = '_';
		linebuf[p++] = ' ';
		linebuf[p++] = ' ';
		for (i = 0 ; i < tmp - 1 ; i++)
		{
			int ii = tmp + i;
			linebuf[p++] =
			(
				DispData.iCharacterSet == OEM_FIXED_FONT && ii != 0 ||
				(ii >= 32 && ii <= 126 || ii >= 160 && ii <= 255 || ii >= 145 && ii <= 146)
			) ? (char)ii : '.';
		}
		linebuf[p++] = ' ';
		linebuf[p] = 0;

		//Draw the selected text
		ExtTextOut(dc,rt.left,rt.top,ETO_CLIPPED,&rt, linebuf, p,NULL);
		delete[]linebuf;

		//Kill the font
		SelectObject (dc, ofon);
		DeleteObject(fon);

		rt.top+=nHeight;

		//Create the separator pen
		HPEN sp = CreatePen (PS_SOLID, 1, DispData.iSepColorValue);
		HPEN op = (HPEN) SelectObject (dc, sp);

		//Draw the separators
		int m;
		for (i = 0; i < (tmp / 4) + 1; i++)
		{
			m = (mol + 2) * tm.tmAveCharWidth - tm.tmAveCharWidth / 2 + 3 * tm.tmAveCharWidth * 4 * i;
			MoveToEx (dc, m, rt.top, NULL);
			LineTo (dc, m, rt.top-nHeight*2);
		}
		// Separator for chars.
		m = tm.tmAveCharWidth * (mol + 3 + tmp * 3) - 2;
		MoveToEx(dc, m, rt.top, NULL);
		LineTo(dc, m, rt.top - nHeight * 2);
		// Second separator.
		MoveToEx(dc, m + 2, rt.top, NULL);
		LineTo(dc, m + 2, rt.top - nHeight * 2);

		//Kill the separator pen
		SelectObject(dc, op);
		DeleteObject(sp);

		rt.bottom=rt.top-nHeight;

		//Create a brush for bookmarks
		HBRUSH bb = CreateSolidBrush(DispData.iBmkColor);
		// Mark hex.
		rt.left += tm.tmAveCharWidth * (mol + 2);
		rt.right = rt.left + 2 * tm.tmAveCharWidth + 1;
		FrameRect(dc, &rt, bb);
		// Mark char.
		rt.left += tm.tmAveCharWidth*(tmp*3+1);
		rt.right = rt.left + tm.tmAveCharWidth + 1;
		FrameRect(dc, &rt, bb);

		//Kill the brush
		DeleteObject(bb);
	}
	return TRUE;
}

INT_PTR UpgradeDlg::DlgProc(HWND hw, UINT m, WPARAM w, LPARAM l)
{
	switch(m)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hw);
	case WM_COMMAND:
		return OnCommand(hw, w, l);
	case WM_NOTIFY:
		return OnNotify(hw, w, l);
	case WM_DRAWITEM:
		return OnDrawitem(hw, w, l);
	}
	return FALSE;
}

//Delete all items from all lists
//init the insts list
void UpgradeDlg::ChangeSelVer(HWND hw, char* text)
{
	HWND insts = GetDlgItem(hw,IDC_INSTS);
	HWND instdata = GetDlgItem(hw,IDC_INSTDATA);
	HWND links = GetDlgItem(hw,IDC_LINKS);
	HWND mru = GetDlgItem(hw,IDC_MRU);

	ListView_DeleteAllItems(insts);
	ListView_DeleteColumn(insts,0);
	ListView_DeleteAllItems(instdata);
	ListView_DeleteAllItems(links);
	ListView_DeleteAllItems(mru);

	//Init the version number on the insts list header
	LVCOLUMN col;
	ZeroMemory(&col,sizeof(col));
	col.mask = LVCF_TEXT|LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.pszText = text;
	col.cx = 120;
	ListView_InsertColumn(insts,0,&col);

	char keyname[100];
	char subkeynam[_MAX_PATH+1];
	strcpy(keyname,"Software\\frhed\\");
	strcat(keyname,text);

	LVITEM item;
	ZeroMemory(&item,sizeof(LVITEM));
	item.mask = LVIF_TEXT ;
	item.pszText = subkeynam;
	HKEY hk;
	LONG res;

	//Fill the instance list with the various instances of the current selected version
	if(0==RegOpenKeyEx(HKEY_CURRENT_USER,keyname,0,KEY_ALL_ACCESS,&hk)){
		for(int i=0;;i++){
			res = RegEnumKey(hk,i,subkeynam,_MAX_PATH+1);
			if(res==ERROR_NO_MORE_ITEMS)break;
			else{
				int instno=0;
				if(StrToIntEx(subkeynam,STIF_DEFAULT,&instno)){
					item.iItem = i;
					ListView_InsertItem(insts, &item);
				}
			}
		}
		RegCloseKey(hk);
	}

	//Add all the links
	strcat(keyname,"\\links");
	char* valnam = subkeynam;
	char valbuf[_MAX_PATH+1];
	DWORD valnamsize,valbufsize,typ;
	item.pszText = valbuf;
	if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER, keyname,0,KEY_ALL_ACCESS,&hk)){
		//Load all the string values
		for(DWORD i = 0;;i++){
			typ=0;
			valnamsize = valbufsize = _MAX_PATH+1;
			valbuf[0]=valnam[0]=0;
			res = RegEnumValue(hk,i,valnam,&valnamsize,0,&typ,(BYTE*) valbuf,&valbufsize);
			if(typ==REG_SZ && valbuf[0]!=0 ){
				//Add the string
				item.iItem = i;
				ListView_InsertItem(links, &item);
			}
			if(ERROR_NO_MORE_ITEMS==res)break;
		}
		RegCloseKey(hk);
	}
	SetDlgItemText(hw, IDC_DISPLAY, "");
}

void UpgradeDlg::ChangeSelInst(HWND hw, char* text)
{
	HWND insts = GetDlgItem(hw,IDC_INSTS);
	HWND instdata = GetDlgItem(hw,IDC_INSTDATA);
	HWND mru = GetDlgItem(hw,IDC_MRU);

	ListView_DeleteAllItems(instdata);
	ListView_DeleteAllItems(mru);

	//Assemble the keyname
	char keynam[_MAX_PATH+1]="Software\\frhed\\";
	LVCOLUMN col;
	ZeroMemory(&col,sizeof(col));
	col.mask = LVCF_TEXT;
	col.pszText = &keynam[15];
	col.cchTextMax = _MAX_PATH+1-15;
	ListView_GetColumn(insts,0,&col);
	strcat(keynam,"\\");
	strcat(keynam,text);

	HKEY hk;
	if (0 == RegOpenKeyEx(HKEY_CURRENT_USER,keynam,0,KEY_ALL_ACCESS,&hk))
	{
		//Add all the data
		BYTE databuf[_MAX_PATH + 1];
		char szText[_MAX_PATH + 1];

		int mrucount = 0;

		LVITEM item;
		item.mask = LVIF_TEXT;
		item.iSubItem = 0;

		static struct
		{
			const char *name;	// registry value names
			const char *text;	// names to go in the list box
			int offset;
			} dict[] = {
			// Color values
			{ "iTextColorValue",		"Text Color",			FIELD_OFFSET(DispDataStruct, iTextColorValue)		},
			{ "iBkColorValue",			"Back Color",			FIELD_OFFSET(DispDataStruct, iBkColorValue)			},
			{ "iSepColorValue",			"Separator Color",		FIELD_OFFSET(DispDataStruct, iSepColorValue)		},
			{ "iSelTextColorValue",		"Selected Text Color",	FIELD_OFFSET(DispDataStruct, iSelTextColorValue)	},
			{ "iSelBkColorValue",		"Selected Back Color",	FIELD_OFFSET(DispDataStruct, iSelBkColorValue)		},
			{ "iBmkColor",				"Bookmark Color",		FIELD_OFFSET(DispDataStruct, iBmkColor)				},
			// signed integers
			{ "iBytesPerLine",			"Bytes Per Line",		FIELD_OFFSET(DispDataStruct, iBytesPerLine)			},
			{ "iOffsetLen",				"Offset Len",			FIELD_OFFSET(DispDataStruct, iOffsetLen)			},
			{ "iFontSize",				"Font Size",			FIELD_OFFSET(DispDataStruct, iFontSize)				},
			{ "iWindowX",				"Window XPos",			-1													},
			{ "iWindowY",				"Window YPos",			-1													},
			{ "iWindowWidth",			"Window Width",			-1													},
			{ "iWindowHeight",			"Window Height",		-1													},
			{ "iMRU_count",				"# MRU items",			-1													},
			// Bool
			{ "iAutomaticBPL",			"Automatic BPL",		FIELD_OFFSET(DispDataStruct, iAutomaticBPL)			},
			{ "bAutoOffsetLen",			"Auto Offset Len",		FIELD_OFFSET(DispDataStruct, bAutoOffsetLen)		},
			{ "bOpenReadOnly",			"Open Read Only",		FIELD_OFFSET(DispDataStruct, bOpenReadOnly)			},
			{ "bMakeBackups",			"Make backups",			FIELD_OFFSET(DispDataStruct, bMakeBackups)			},
			// Multiple different values
			{ "iWindowShowCmd",			"Window Show Cmd",		FIELD_OFFSET(DispDataStruct, iWindowShowCmd)		},
			{ "iCharacterSet",			"Character Set",		FIELD_OFFSET(DispDataStruct, iCharacterSet)			},
			// Strings
			{ "TexteditorName",			"Text Editor Name",		-1													},
		};
		int i = 0;
		for ( ; i < RTL_NUMBER_OF(dict) ; i++)
		{
			DWORD typ;
			DWORD datasize = sizeof databuf;
			item.iItem = i;
			item.pszText = (char *)dict[i].text;
			ListView_InsertItem(instdata, &item);
			item.pszText = (char *)dict[i].name;
			ZeroMemory(databuf, sizeof databuf);
			RegQueryValueEx( hk, item.pszText, NULL,&typ, databuf, &datasize );
			int offset = dict[item.iItem].offset;
			if (offset >= 0)
				*(int *)((char *)&DispData + offset) = *(int*)databuf;
			if (i == 13)
				mrucount = *(int*)databuf;
			if(i<6)
			{
				sprintf(szText, "RGB - %u,%u,%u",
					(unsigned)databuf[0], (unsigned)databuf[1], (unsigned)databuf[2]);
			}
			else if(i<6+8)
			{
				sprintf(szText,"%u", *(int*)databuf);
			}
			else if(i<6+8+4)
			{
				strcpy(szText, databuf[0] ? "True" : "False");
			}
			else if(i<6+8+4+1)
			{
				switch (*(int*)databuf)
				{
					case SW_HIDE: strcpy(szText,"Hide");break;
					case SW_MINIMIZE: strcpy(szText,"Minimize");break;
					case SW_RESTORE: strcpy(szText,"Restore");break;
					case SW_SHOW: strcpy(szText,"Show");break;
					case SW_SHOWMAXIMIZED: strcpy(szText,"Show Maximized");break;
					case SW_SHOWMINIMIZED: strcpy(szText,"Show Minimized");break;
					case SW_SHOWMINNOACTIVE: strcpy(szText,"Show MinNoactive");break;
					case SW_SHOWNA: strcpy(szText,"Show NA");break;
					case SW_SHOWNOACTIVATE: strcpy(szText,"Show Noactivate");break;
					case SW_SHOWNORMAL: strcpy(szText,"Show Normal");break;
				}
			}
			else if(i<6+8+4+1+1)
			{
				switch (*(int*)databuf)
				{
					case ANSI_FIXED_FONT:strcpy (szText, "ANSI");break;
					case OEM_FIXED_FONT:strcpy (szText, "OEM");break;
				}
			}
			else if(i<6+8+4+1+1+2)
				strcpy(szText, (char *)databuf);
			else
				strcpy(szText, "?");
			ListView_SetItemText(instdata, item.iItem, 1, szText);
			item.iItem++;
		}
		item.pszText = (char *)databuf;
		//Add all the MRUs
		for (i = 0 ; i < mrucount ; i++ )
		{
			sprintf(szText, "MRU_File%d", i + 1);
			DWORD datasize = sizeof databuf;
			ZeroMemory(databuf, sizeof databuf);
			RegQueryValueEx(hk, szText, NULL, NULL, databuf, &datasize);
			item.iItem = i;
			ListView_InsertItem(mru, &item);
		}
		RegCloseKey(hk);
		//Paint the display box
		SetDlgItemText(hw, IDC_DISPLAY, "frhed Display");
	}
}
