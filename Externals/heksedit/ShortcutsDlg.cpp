#include "precomp.h"
#include "shtools.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

class TraverseFolders
{
public:
	HWND hwlist;
	char rn[MAX_PATH];
	char vn[50];
	LVITEM li;
	LVFINDINFO fi;
	BOOL cr;
	TraverseFolders(HWND);
	void Recurse();
};

TraverseFolders::TraverseFolders(HWND h)
{
	ZeroMemory(this, sizeof *this);
	hwlist = h;
	li.mask = LVIF_TEXT;//Using the text only
	li.iItem = ListView_GetItemCount(hwlist);//Initial insert pos
	fi.flags = LVFI_STRING;//will need to check for duplicates
	fi.psz = li.pszText = rn;//Positions don't change beween files (Absolute path is entered into rn)
}

//Thanks to Raihan for the code this was based on - see his web page
void TraverseFolders::Recurse()
{
	_finddata_t F;
	int S;
	//First find all the links
	if ((S = _findfirst ("*.lnk", &F)) != -1)
	{
		do if ( !( F.attrib & _A_SUBDIR ) && !ResolveIt(NULL,F.name,rn) )
		{
			int si;
			if (cr)
			{//findnfix
				PathStripPath(rn);//strip to file name
				si = _stricmp(rn, "heksedit.exe");
			}
			else
			{
				si = PathsEqual(_pgmptr, rn);//update
			}
			if (si == 0)
			{
				_fullpath( rn, F.name, MAX_PATH);
				remove(rn);//get rid of the file if we are fixing (in case of 2 links to frhed in same dir & links with the wrong name)
				char *fnam = PathFindFileName(rn);
				fnam[-1] = 0; //strip the file name (& '\\') off
				if (-1 == ListView_FindItem(hwlist, -1, &fi)) //Not present yet
				{
					//Insert the item
					ListView_InsertItem(hwlist,&li);
					//Add to the Registry
					sprintf(vn,"%d",li.iItem);
					SHSetValue(HKEY_CURRENT_USER,
						"Software\\frhed\\v"CURRENT_VERSION"."SUB_RELEASE_NO"\\links",
						vn, REG_SZ, rn, fnam - rn);
					li.iItem++;
				}
				strcat(rn,"\\frhed.lnk");//put the name backon
				CreateLink(_pgmptr,rn);//create the new link
			}
		} while (_findnext(S, &F) == 0);
		_findclose(S);
	}
	//Then find all the subdirs
	if ((S = _findfirst ("*", &F)) != -1)
	{
		//except "." && ".."
		do if (F.attrib & _A_SUBDIR && strstr("..", F.name) == 0)
		{
			_chdir(F.name);
			Recurse();
			_chdir("..");
		} while (_findnext(S, &F) == 0);
		_findclose(S);
	}
}

BOOL ShortcutsDlg::OnInitDialog(HWND hw)
{
	//Add a column
	LVCOLUMN col;
	ZeroMemory(&col, sizeof col);
	col.mask = LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.pszText = "link names are \"frhed.lnk\"";
	col.cx = 153;
	ListView_InsertColumn(GetDlgItem(hw, IDC_LIST), 0, &col);
	//Load links from the registry
	//Tricky-tricky
	SendMessage(hw, WM_COMMAND, MAKEWPARAM(IDC_RELOAD, BN_CLICKED), (LPARAM)GetDlgItem(hw,IDC_RELOAD));
	return TRUE;
}

int CALLBACK ShortcutsDlg::BrowseCallbackProc(HWND hw, UINT m, LPARAM l, LPARAM)
{
	//If the folder exists & doesn't contain a link then enabled
	// Set the status window to the currently selected path.
	if (m == BFFM_SELCHANGED)
	{
		char szDir[MAX_PATH];
		if (SHGetPathFromIDList((LPITEMIDLIST) l ,szDir) && PathIsDirectory(szDir))
		{
			PathAddBackslash(szDir);
			strcat(szDir, "frhed.lnk");
			if (PathFileExists(szDir))
			{
				SendMessage(hw,BFFM_ENABLEOK,0,0);//Disable
				SendMessage(hw,BFFM_SETSTATUSTEXT,0,(LPARAM)"This folder already contains a file called \"frhed.lnk\"");
			}
			else
			{
				//If there is any other (faster/easier) way to test whether the file-system is writeable or not Please let me know - Pabs
				int fh = _creat(szDir,_S_IWRITE);
				if (fh != -1)
				{
					_close(fh);
					remove(szDir);
					SendMessage(hw,BFFM_ENABLEOK,0,1);//Enable
					SendMessage(hw,BFFM_SETSTATUSTEXT,0,(LPARAM)"\"frhed.lnk\" can be added to this folder");
				}
				else
				{
					SendMessage(hw,BFFM_ENABLEOK,0,0);//Disable
					SendMessage(hw,BFFM_SETSTATUSTEXT,0,(LPARAM)"\"frhed.lnk\" cannot be added to this folder");
				}
			}
		}
		else
		{
			SendMessage(hw,BFFM_ENABLEOK,0,0);//Disable
			SendMessage(hw,BFFM_SETSTATUSTEXT,0,(LPARAM)"\"frhed.lnk\" cannot be added to this folder");
		}
	}
	return 0;
}

int CALLBACK ShortcutsDlg::SearchCallbackProc(HWND hw, UINT m, LPARAM l, LPARAM)
{
	if (m == BFFM_SELCHANGED)
	{
		char szDir[MAX_PATH];
		BOOL ret = SHGetPathFromIDList((LPITEMIDLIST) l ,szDir) && PathIsDirectory(szDir);
		SendMessage(hw, BFFM_ENABLEOK, 0, ret);//Enable/Disable
		SendMessage(hw, BFFM_SETSTATUSTEXT, 0,
			(LPARAM)(ret ? "frhed can start searching here" : "frhed cannot start the search from here"));
	}
	return 0;
}

BOOL ShortcutsDlg::OnCommand(HWND hw, WPARAM w, LPARAM l)
{
	switch (LOWORD(w))
	{
	case IDCANCEL:
	case IDOK:
		{
			//Delete all values
			RegDeleteKey(HKEY_CURRENT_USER, "Software\\frhed\\v"CURRENT_VERSION"." SUB_RELEASE_NO "\\links");
			//Save links (from the list box) to the registry & file system
			HKEY hk;
			if(ERROR_SUCCESS==RegCreateKey(HKEY_CURRENT_USER, "Software\\frhed\\v"CURRENT_VERSION"." SUB_RELEASE_NO "\\links",&hk)){
				char valnam[50] = "";//value name
				char buf[_MAX_PATH+1]="";//location of the link (all links named frhed.lnk)
				HWND list = GetDlgItem (hw, IDC_LIST);//get the list
				int num = ListView_GetItemCount(list);//get the # items in the list
				int len;
				for(int i=0;i<num;i++){//loop num times
					sprintf(valnam,"%d",i);//write the valname
					ListView_GetItemText(list, i, 0, buf, _MAX_PATH+1);//get the string
					len = strlen(buf)+1;//string len +1
					RegSetValueEx(hk,valnam,0,REG_SZ,(BYTE*)buf,len);
					//Just in case
					PathAddBackslash(buf);
					strcat(buf,"frhed.lnk");
					CreateLink(_pgmptr,buf);
				}//end of the loop
				RegCloseKey(hk);
			}
			else MessageBox(hw,"Could not Save shortcut entries", "Shortcuts",MB_OK);

			//If the key is empty after this kill it (to prevent re-enabling of "Remove frhed")
			SHDeleteEmptyKey(HKEY_CURRENT_USER, "Software\\frhed\\v"CURRENT_VERSION"." SUB_RELEASE_NO "\\links");
			SHDeleteEmptyKey(HKEY_CURRENT_USER, "Software\\frhed\\v"CURRENT_VERSION"." SUB_RELEASE_NO );

			EndDialog (hw, 0);
			return TRUE;
		}

	case IDC_MOVE:
	case IDC_ADD:
		{
			//Call the filesystem dialog box to browse to a folder
			//& add it

			BROWSEINFO bi;
			CHAR szDir[MAX_PATH];
			LPITEMIDLIST pidl;
			LPMALLOC pMalloc;
			HWND list = GetDlgItem(hw,IDC_LIST);
			int di = -1;
			HKEY hk;

			if(LOWORD(w)==IDC_MOVE){
				di = ListView_GetSelectedCount(list);
				if(di>1){
					MessageBox(hw,"Can't move more than 1 link at a time","Move link",MB_OK);
					return TRUE;
				}
				else if(di!=1){
					MessageBox(hw,"No link selected to move","Move link",MB_OK);
					return TRUE;
				}
				di = ListView_GetNextItem(list, (UINT)-1, LVNI_SELECTED);
				if(di==-1){
					MessageBox(hw,"Couldn't find the selected item","Move link",MB_OK);
					return TRUE;
				}
			}
			if (SUCCEEDED(SHGetMalloc(&pMalloc)))
			{
				ZeroMemory(&bi,sizeof(bi));
				bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT ;
				bi.hwndOwner = hw;
				bi.lpfn = BrowseCallbackProc;
				if(LOWORD(w)==IDC_ADD)
					bi.lpszTitle = "Place a link to frhed in...";
				else if(LOWORD(w)==IDC_MOVE)
					bi.lpszTitle = "Move the link to frhed to...";

				pidl = SHBrowseForFolder(&bi);
				if (pidl)
				{
					if (SHGetPathFromIDList(pidl,szDir))
					{
						//Check if the item is already in the list
						int num = ListView_GetItemCount(list);//get the # items in the list
						int done = 0;
						char path[MAX_PATH]="";
						char buf[_MAX_PATH+1]="";

						strcpy(path,szDir);
						_strupr(path);
						for(int i=0;i<num;i++){//loop num times
							ListView_GetItemText(list, i, 0, buf,_MAX_PATH+1);//get the string
							_strupr(buf);
							if(!strcmp(buf,path)){
								done = 1;
								break;
							}
						}//end of the loop
						char valnam[_MAX_PATH+1];
						if(done){
							MessageBox(hw,"There is already a link in that folder","Add/Move",MB_OK);
							//Just in case
							PathAddBackslash(szDir);
							strcat(szDir,"frhed.lnk");
							CreateLink(_pgmptr,szDir);
						}
						else{
							if(LOWORD(w)==IDC_ADD){
								//Add to the list
								LVITEM item;
								ZeroMemory(&item,sizeof(item));
								item.mask=LVIF_TEXT;
								item.pszText = szDir;
								item.iItem = num;
								ListView_InsertItem(list, &item);
								//Add to the registry (find a string name that doesn't exist first)
								if(ERROR_SUCCESS==RegCreateKey(HKEY_CURRENT_USER, "Software\\frhed\\v"CURRENT_VERSION"." SUB_RELEASE_NO "\\links",&hk)){
									for(DWORD i = 0;;i++){
										sprintf(valnam,"%d",i);
										if(ERROR_FILE_NOT_FOUND==RegQueryValueEx(hk,valnam,0,NULL,NULL,NULL)){
											RegSetValueEx(hk,valnam,0,REG_SZ,(BYTE*)szDir,strlen(szDir)+1);
											break;
										}
									}
									RegCloseKey(hk);
									//Add to the filesystem
									PathAddBackslash(szDir);
									strcat(szDir,"frhed.lnk");
									CreateLink(_pgmptr,szDir);
								}
							}
							else if(LOWORD(w)==IDC_MOVE){
								//Move the old one to the new loc
								DWORD valnamsize,typ;
								char valbuf[_MAX_PATH+1];
								DWORD valbufsize,ret;
								char cursel[_MAX_PATH+1]="";
								//Get the old path
								ListView_GetItemText(list,di,0,cursel,_MAX_PATH+1);
								_strupr(cursel);
								//Set the new path in the registry
								if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\frhed\\v"CURRENT_VERSION"." SUB_RELEASE_NO "\\links",0,KEY_ALL_ACCESS,&hk)){
									for(DWORD i = 0;;i++){
										typ=0;
										valnamsize = valbufsize = _MAX_PATH+1;
										valbuf[0]=valnam[0]=0;
										ret = RegEnumValue(hk,i,valnam,&valnamsize,0,&typ,(BYTE*) valbuf,&valbufsize);
										_strupr(valbuf);
										if(typ==REG_SZ && valbuf[0]!=0 && !strcmp(valbuf,cursel)){
											RegSetValueEx(hk,valnam,0,REG_SZ,(BYTE*)szDir,strlen(szDir)+1);
											break;
										}
										if(ERROR_NO_MORE_ITEMS==ret)break;
									}
									RegCloseKey(hk);
									//Set the new path
									ListView_SetItemText(list,di,0,szDir);
									//Move the actual file
									PathAddBackslash(szDir);
									strcat(szDir,"frhed.lnk");
									PathAddBackslash(cursel);
									strcat(cursel,"frhed.lnk");
									CreateLink(_pgmptr,cursel);//Just in case
									rename(cursel,szDir);
								}
							}
						}
					}
					pMalloc->Free(pidl);
				}
				pMalloc->Release();
			}
			return TRUE;
		}
	case IDC_FIND_AND_FIX:
		//Go through the file system searching for links to frhed.exe and fix them so they point to this exe
	case IDC_UPDATE:
		{
			//Go through the file system searching for links to this exe
			//-Thanks to Raihan for the traversing code this was based on.
			if(LOWORD (w) == IDC_FIND_AND_FIX && IDNO == MessageBox(hw,"Existing links to old versions of frhed will be updated to this version\nAre you sure you want to continue","Find & fix",MB_YESNO))return TRUE;
			//Find a spot to start from
			LPMALLOC pMalloc;

			if (SUCCEEDED(SHGetMalloc(&pMalloc)))
			{
				BROWSEINFO bi;
				LPITEMIDLIST pidl;
				ZeroMemory(&bi,sizeof(bi));
				bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT ;
				bi.hwndOwner = hw;
				bi.lpfn = SearchCallbackProc;
				bi.lpszTitle = "Pick a folder to start searching in.";

				pidl = SHBrowseForFolder(&bi);
				if (pidl)
				{
					CHAR szDir[MAX_PATH];
					if (SHGetPathFromIDList(pidl,szDir))
					{
						WaitCursor wc;//Wait until finished
						_chdir(szDir);//Set the dir to start searching in
						TraverseFolders tf = GetDlgItem(hw,IDC_LIST);//Set the list hwnd;
						tf.cr = LOWORD(w) == IDC_FIND_AND_FIX;//any frhed.exe if 1 else _pgmptr
						tf.Recurse();//Search
					}
					pMalloc->Free(pidl);
				}
				pMalloc->Release();
			}
			return TRUE;
		}
	case IDC_DELETE:
		{
			HWND list = GetDlgItem (hw, IDC_LIST);//get the list
			//Delete the selected links from the registry & the filesystem
			int di = ListView_GetSelectedCount(list);
			if(di==0){
				MessageBox(hw,"No links selected to delete","Delete links",MB_OK);
				return TRUE;
			}
			for(;;){
				di = ListView_GetNextItem(list, (UINT)-1, LVNI_SELECTED);
				if(di==-1)break;
				char valnam[_MAX_PATH+1];
				DWORD valnamsize,typ;
				char valbuf[_MAX_PATH+1];
				DWORD valbufsize,ret;
				char delbuf[_MAX_PATH+1] = "";
				ListView_GetItemText(list,di,0,delbuf,_MAX_PATH+1);
				_strupr(delbuf);
				ListView_DeleteItem(list,di);
				HKEY hk;
				if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\frhed\\v"CURRENT_VERSION"." SUB_RELEASE_NO "\\links",0,KEY_ALL_ACCESS,&hk)){
					for(DWORD i = 0;;i++){
						typ=0;
						valnamsize = valbufsize = _MAX_PATH+1;
						valbuf[0]=valnam[0]=0;
						ret = RegEnumValue(hk,i,valnam,&valnamsize,0,&typ,(BYTE*) valbuf,&valbufsize);
						_strupr(valbuf);
						if(typ==REG_SZ && valbuf[0]!=0 && !strcmp(valbuf,delbuf)){
							RegDeleteValue(hk,valnam);
							break;
						}
						if(ERROR_NO_MORE_ITEMS==ret)break;
					}
					RegCloseKey(hk);
					PathAddBackslash(delbuf);
					strcat(delbuf,"frhed.lnk");
					remove(delbuf);
				}
				SHDeleteEmptyKey(HKEY_CURRENT_USER, "Software\\frhed\\v"CURRENT_VERSION"." SUB_RELEASE_NO "\\links");
			}
			return TRUE;
		}
	case IDC_RELOAD:
		{
			//Reload links from the registry frhed\subreleaseno\links\ all values loaded & tested
			HKEY hk;
			char valnam[_MAX_PATH+1]="";
			DWORD valnamsize = _MAX_PATH+1,typ;
			char valbuf[_MAX_PATH+1]="";
			DWORD valbufsize = _MAX_PATH+1,ret;
			HWND list = GetDlgItem(hw,IDC_LIST);
			//Delete list
			ListView_DeleteAllItems(list);
			LVITEM item;
			ZeroMemory(&item,sizeof(item));
			item.mask=LVIF_TEXT;
			item.pszText = valbuf;
			if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\frhed\\v"CURRENT_VERSION"." SUB_RELEASE_NO "\\links",0,KEY_ALL_ACCESS,&hk)){
				//Load all the string values
				for(DWORD i = 0;;i++){
					typ=0;
					valnamsize = valbufsize = _MAX_PATH+1;
					valbuf[0]=valnam[0]=0;
					ret = RegEnumValue(hk,i,valnam,&valnamsize,0,&typ,(BYTE*) valbuf,&valbufsize);
					if(typ==REG_SZ && valbuf[0]!=0 && PathIsDirectory(valbuf)){//Valid dir
						//Add the string
						item.iItem = i;
						ListView_InsertItem(list, &item);
						PathAddBackslash(valbuf);
						strcat(valbuf,"frhed.lnk");
						CreateLink(_pgmptr,valbuf);
					}
					if(ERROR_NO_MORE_ITEMS==ret)break;
				}
				RegCloseKey(hk);
			}
			return TRUE;
		}
	case IDC_START:
	case IDC_PROGRAMS:
	case IDC_SENDTO:
	case IDC_DESKTOP:
		{
			//Create links in
			//HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders\Start Menu = C:\WINDOWS\Start Menu on my computer
			//HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders\Programs = C:\WINDOWS\Start Menu\Programs on my computer
			//HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders\SendTo = C:\WINDOWS\SendTo on my computer
			//HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders\Desktop = C:\WINDOWS\Desktop on my computer
			HKEY hk;
			if(ERROR_SUCCESS==RegOpenKey(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",&hk)){
				char szDir[_MAX_PATH+1]="";
				DWORD len = _MAX_PATH+1;
				//Get the path from the registry
				switch(LOWORD(w)){
					case IDC_START:
						RegQueryValueEx(hk,"Start Menu",0,NULL,(BYTE*)szDir,&len);
					break;
					case IDC_PROGRAMS:
						RegQueryValueEx(hk,"Programs",0,NULL,(BYTE*)szDir,&len);
					break;
					case IDC_SENDTO:
						RegQueryValueEx(hk,"SendTo",0,NULL,(BYTE*)szDir,&len);
					break;
					case IDC_DESKTOP:
						RegQueryValueEx(hk,"Desktop",0,NULL,(BYTE*)szDir,&len);
					break;
				}
				RegCloseKey(hk);

				HWND list = GetDlgItem(hw,IDC_LIST);
				int num = ListView_GetItemCount(list);//get the # items in the list
				int done = 0;
				char path[_MAX_PATH+1]="";
				char buf[_MAX_PATH+1]="";
				strcpy(path,szDir);
				_strupr(path);
				for(int i=0;i<num;i++){//loop num times
					ListView_GetItemText(list,i,0,buf,_MAX_PATH+1);//get the string
					_strupr(buf);//convert to upper since strcmp is case sensitive & Win32 is not
					if(!strcmp(buf,path)){
						done = 1;
						break;
					}
				}//end of the loop
				if(done){
					MessageBox(hw,"There is already a link in that folder","Add",MB_OK);
					//Just in case
					PathAddBackslash(szDir);
					strcat(szDir,"frhed.lnk");
					CreateLink(_pgmptr,szDir);
				}
				else{
					LVITEM item;
					ZeroMemory(&item,sizeof(item));
					item.mask=LVIF_TEXT;
					item.pszText = szDir;
					item.iItem = num;
					ListView_InsertItem(list, &item);
					char valnam[_MAX_PATH+1];
					if(ERROR_SUCCESS==RegCreateKey(HKEY_CURRENT_USER, "Software\\frhed\\v"CURRENT_VERSION"." SUB_RELEASE_NO "\\links",&hk)){
						//Find a value name that does not exist
						for(DWORD i = 0;;i++){
							sprintf(valnam,"%d",i);
							if(ERROR_FILE_NOT_FOUND==RegQueryValueEx(hk,valnam,0,NULL,NULL,NULL)){
								RegSetValueEx(hk,valnam,0,REG_SZ,(BYTE*)szDir,strlen(szDir)+1);//Add the value to the registry
								break;
							}
						}
						RegCloseKey(hk);
						PathAddBackslash(szDir);
						strcat(szDir,"frhed.lnk");
						CreateLink(_pgmptr,szDir);
					}
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ShortcutsDlg::OnNotify(HWND hw, WPARAM w, LPARAM l)
{
	NMLVKEYDOWN *nmh = ((NMLVKEYDOWN*)l);
	if (nmh->hdr.idFrom == IDC_LIST &&
		nmh->hdr.code == LVN_KEYDOWN &&
		nmh->wVKey == VK_DELETE)
	{
		OnCommand(hw, IDC_DELETE, 0);
	}
	return TRUE;
}

INT_PTR ShortcutsDlg::DlgProc(HWND hw, UINT m, WPARAM w, LPARAM l)
{
	switch (m)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hw);
	case WM_COMMAND:
		return OnCommand(hw, w, l);
	case WM_NOTIFY:
		return OnNotify(hw, w, l);
	}
	return FALSE;
}
