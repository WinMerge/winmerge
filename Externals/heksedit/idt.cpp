#include "precomp.h"
#include "idt.h"
#include "hexwnd.h"
#include "AnsiConvert.h"
#include "resource.h"

/*The #ifndef __CYGWIN__s are there because cygwin/mingw doesn't yet have
certain APIs in their import libraries. Specifically _wremove, _wopen & GetEnhMetaFileBits.*/

class DragDropDlg
{
public:
	enum { IDD = IDD_DRAGDROP };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
	DWORD allowable_effects;
	bool effect;
	UINT numformatetcs;
	FORMATETC *formatetcs;
	UINT numformats;
	UINT *formats;
};

//TODO: update such that when the IDataObject changes the list box is re-created
INT_PTR DragDropDlg::DlgProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	switch (m)
	{
	case WM_INITDIALOG:
		{
			CheckDlgButton( h, effect ? IDC_COPY : IDC_MOVE, TRUE );
			if( !(allowable_effects & DROPEFFECT_MOVE) )
				EnableWindow( GetDlgItem( h, IDC_MOVE ), FALSE );
			if( !(allowable_effects & DROPEFFECT_COPY) )
				EnableWindow( GetDlgItem( h, IDC_COPY ), FALSE );
			HWND list = GetDlgItem(h, IDC_LIST);
			if (numformatetcs && formatetcs)
			{
				LVCOLUMN col;
				ZeroMemory(&col, sizeof col);
				ListView_InsertColumn(list, 0, &col);
				TCHAR szFormatName[100];
				UINT i;
				for (i = 0 ; i < numformatetcs ; i++)
				{
					CLIPFORMAT temp = formatetcs[i].cfFormat;
					LVITEM lvi;
					lvi.lParam = lvi.iItem = i;
					lvi.iSubItem = 0;
					lvi.pszText = szFormatName;
					// For registered formats, get the registered name.
					if (!GetClipboardFormatName(temp, szFormatName, sizeof szFormatName))
					{
						//Get the name of the standard clipboard format.
						switch (temp)
						{
							#define CASE(a) case a: lvi.pszText = _T(#a); break;
								CASE(+CF_TEXT)
								CASE(CF_BITMAP) CASE(CF_METAFILEPICT) CASE(CF_SYLK)
								CASE(CF_DIF) CASE(CF_TIFF) CASE(CF_OEMTEXT)
								CASE(CF_DIB) CASE(CF_PALETTE) CASE(CF_PENDATA)
								CASE(CF_RIFF) CASE(CF_WAVE) CASE(CF_UNICODETEXT)
								CASE(CF_ENHMETAFILE) CASE(CF_HDROP) CASE(CF_LOCALE)
								CASE(CF_OWNERDISPLAY) CASE(CF_DSPTEXT)
								CASE(CF_DSPBITMAP) CASE(CF_DSPMETAFILEPICT)
								CASE(CF_DSPENHMETAFILE) CASE(CF_PRIVATEFIRST)
								CASE(CF_PRIVATELAST) CASE(CF_GDIOBJFIRST)
								CASE(CF_GDIOBJLAST) CASE(CF_DIBV5)
							#undef CASE
							default:
								if (temp >= CF_PRIVATEFIRST && temp <= CF_PRIVATELAST)
								{
									_stprintf(szFormatName, _T("CF_PRIVATE_%d"), temp - CF_PRIVATEFIRST);
								}
								else if (temp >= CF_GDIOBJFIRST && temp <= CF_GDIOBJLAST)
								{
									_stprintf(szFormatName, _T("CF_GDIOBJ_%d"), temp - CF_GDIOBJFIRST);
								}
								//Format ideas for future: hex number, system/msdn constant, registered format, WM_ASKFORMATNAME, tickbox for delay rendered or not*/
								/*else if(temp>0xC000&&temp<0xFFFF)
								{
									sprintf(szFormatName,"CF_REGISTERED%d",temp-0xC000);
								}*/
								else
								{
									_stprintf(szFormatName, _T("0x%.8x"), temp);
								}
							break;
						}
					}
					//Insert into the list
					lvi.mask = LVIF_TEXT | LVIF_PARAM;
					if (*lvi.pszText == '+')
					{
						++lvi.pszText;
						lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
						lvi.state = lvi.stateMask = LVIS_SELECTED;
					}
					ListView_InsertItem(list, &lvi);
				}
				ListView_SetColumnWidth(list, 0, LVSCW_AUTOSIZE_USEHEADER);
			}
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(w))
		{
		case IDOK:
			{
				effect = !IsDlgButtonChecked(h, IDC_MOVE);
				HWND list = GetDlgItem(h, IDC_LIST);
				numformats = ListView_GetSelectedCount(list);
				if (numformats)
				{
					formats = (UINT*)malloc(numformats * sizeof *formats);
					if (formats == 0)
					{
						MessageBox(h, _T("Not enough memory"), _T("Drag-drop"), MB_ICONERROR);
						return TRUE;
					}
					LVITEM temp;
					ZeroMemory(&temp, sizeof temp);
					temp.mask = LVIF_PARAM;
					temp.iItem = -1;
					for (UINT i = 0 ; i < numformats ; i++)
					{
						temp.iItem = ListView_GetNextItem(list, temp.iItem, LVNI_SELECTED);
						ListView_GetItem(list, &temp);
						formats[i] = temp.lParam;
					}
				}
				EndDialog (h, 1);
			}
			return TRUE;
		case IDCANCEL:
			EndDialog(h, -1);
			return TRUE;
		case IDC_UP:
		case IDC_DOWN:
			{
				HWND list = GetDlgItem(h, IDC_LIST);
				LVITEM item[2];
				ZeroMemory(item, sizeof item);
				//If anyone knows a better way to swap two items please send a patch
				item[0].iItem = ListView_GetNextItem(list, -1, LVNI_SELECTED);
				if (item[0].iItem == -1)
					item[0].iItem = ListView_GetNextItem(list, -1, LVNI_FOCUSED);
				if (item[0].iItem == -1)
				{
					MessageBox(h, _T("Select an item to move."), _T("Drag-drop"), MB_OK);
					SetFocus(list);
					return TRUE;
				}
				item[0].mask = LVIF_TEXT|LVIF_PARAM|LVIF_STATE;
				item[0].stateMask = (UINT)-1;
				TCHAR text[2][100];
				item[0].pszText = text[0];
				item[0].cchTextMax = RTL_NUMBER_OF(text[0]);
				item[1] = item[0];
				item[1].pszText = text[1];
				item[1].cchTextMax = RTL_NUMBER_OF(text[1]);
				if (LOWORD(w) == IDC_UP)
				{
					if (item[1].iItem == 0)
						item[1].iItem = numformatetcs;
					--item[1].iItem;
				}
				else
				{
					++item[1].iItem;
					if (item[1].iItem == numformatetcs)
						item[1].iItem = 0;
				}
				ListView_GetItem(list, &item[0]);
				ListView_GetItem(list, &item[1]);
				swap(item[0].iItem, item[1].iItem);
				item[0].state |= LVIS_FOCUSED|LVIS_SELECTED;
				item[1].state &= ~(LVIS_FOCUSED|LVIS_SELECTED);
				ListView_SetItem(list, &item[0]);
				ListView_SetItem(list, &item[1]);
				SetFocus(list);
			}
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

//Members
CDropTarget::CDropTarget(HexEditorWindow &hexwnd)
: hexwnd(hexwnd)
{
#ifdef _DEBUG
	printf("IDropTarget::IDropTarget\n");
#endif //_DEBUG
	m_cRefCount = 0;
}

CDropTarget::~CDropTarget()
{
#ifdef _DEBUG
	printf("IDropTarget::~IDropTarget\n");
	if( m_cRefCount != 0 )
		printf("Deleting %s too early 0x%x.m_cRefCount = %d\n", "IDropTarget", this, m_cRefCount);
#endif //_DEBUG
	hexwnd.target = NULL;
}


//IUnknown methods
STDMETHODIMP CDropTarget::QueryInterface( REFIID iid, void** ppvObject )
{
#ifdef _DEBUG
	printf("IDropTarget::QueryInterface\n");
#endif //_DEBUG

	*ppvObject = NULL;

	if ( iid == IID_IUnknown ) *ppvObject = (IUnknown*)this;
	else if ( iid == IID_IDropTarget ) *ppvObject = (IDropTarget*)this;

	if(*ppvObject){
		((IUnknown*)*ppvObject)->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CDropTarget::AddRef( void )
{
#ifdef _DEBUG
	printf("IDropTarget::AddRef\n");
#endif //_DEBUG
	return ++m_cRefCount;
}

STDMETHODIMP_(ULONG) CDropTarget::Release( void )
{
#ifdef _DEBUG
	printf("IDropTarget::Release\n");
#endif //_DEBUG
	if( --m_cRefCount == 0) delete this;
	return m_cRefCount;
}


//IDropTarget methods
STDMETHODIMP CDropTarget::DragEnter( IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
#ifdef _DEBUG
	printf("IDropTarget::DragEnter\n");
#endif //_DEBUG
	pDataObj = pDataObject;
	pDataObject->AddRef();
	hdrop_present = false;
	if (hexwnd.prefer_CF_HDROP)
	{
		FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, 0xffffffff };
		STGMEDIUM stm;
		if (S_OK == pDataObject->QueryGetData(&fe))
		{
			hdrop_present = true;
			*pdwEffect = DROPEFFECT_COPY;
			return S_OK;
		}
		else if (S_OK == pDataObject->GetData(&fe, &stm))
		{
			hdrop_present = true;
			ReleaseStgMedium(&stm);
			*pdwEffect = DROPEFFECT_COPY;
			return S_OK;
		}
	}

	CreateCaret( hexwnd.hwnd, (HBITMAP)1, 2, hexwnd.cyChar );

	return DragOver( grfKeyState, pt, pdwEffect );
}

STDMETHODIMP CDropTarget::DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
#ifdef _DEBUG
	printf("IDropTarget::DragOver\n");
#endif //_DEBUG

	LastKeyState = grfKeyState;

	DWORD dwOKEffects = *pdwEffect;

	//We only move or copy if we are not read-only
	//And only if the source supports it
	//but in read-only mode we accept drag-drop loading (CF_HDROP)
	//this is handled by OLE it seems if the window is DragAcceptFiles( hwnd, TRUE )
	if (hexwnd.bReadOnly)
		*pdwEffect = DROPEFFECT_NONE;
	else if (hdrop_present)
		*pdwEffect = DROPEFFECT_COPY;
	else if (grfKeyState & MK_CONTROL)
		*pdwEffect =
			dwOKEffects & DROPEFFECT_COPY ? DROPEFFECT_COPY :
			dwOKEffects & DROPEFFECT_MOVE ? DROPEFFECT_MOVE :
			DROPEFFECT_NONE;
	else
		*pdwEffect =
			dwOKEffects & DROPEFFECT_MOVE ? DROPEFFECT_MOVE :
			dwOKEffects & DROPEFFECT_COPY ? DROPEFFECT_COPY :
			DROPEFFECT_NONE;

	POINT p = { pt.x, pt.y };
	ScreenToClient(hexwnd.hwnd, &p);
	hexwnd.iMouseX = p.x;
	hexwnd.iMouseY = p.y;

	if (hdrop_present || *pdwEffect == DROPEFFECT_NONE)
		HideCaret(hexwnd.hwnd);
	else
		hexwnd.set_drag_caret(p.x, p.y, *pdwEffect == DROPEFFECT_COPY, (grfKeyState & MK_SHIFT) != 0);

	hexwnd.fix_scroll_timers(p.x,p.y);

	return S_OK;
}

STDMETHODIMP CDropTarget::DragLeave ( void )
{
#ifdef _DEBUG
	printf("IDropTarget::DragLeave\n");
#endif //_DEBUG

	//This is the lesser of two evils
	//1. recreate the editing caret whenever the mouse leaves the client area
	//2. rather than have the caret disappear whenever the mouse leaves the client area (until set_focus is called)
	//#2 would just call CreateCaret
	CreateCaret(hexwnd.hwnd, NULL, hexwnd.cxChar, hexwnd.cyChar);
	hexwnd.set_caret_pos();
	if (GetFocus() == hexwnd.hwnd)
		ShowCaret(hexwnd.hwnd);

	hexwnd.kill_scroll_timers();

	if (pDataObj)
	{
		pDataObj->Release();
		pDataObj = NULL;
	}

	return S_OK;
}

STDMETHODIMP CDropTarget::Drop( IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect )
{
#ifdef _DEBUG
	printf("IDropTarget::Drop\n");
#endif //_DEBUG

	DWORD dwOKEffects = *pdwEffect;
	{
		//We do this because grfKeyState will always have the mouse button used off
		DWORD temp = LastKeyState;
		//Get the position of the drop
		DragOver( grfKeyState, pt, pdwEffect );
		LastKeyState = temp;
	}
	//Remove the effects
	pDataObj = NULL;
	DragLeave();

	bool copying = (*pdwEffect & DROPEFFECT_COPY) != 0;

	size_t totallen = 0;
	BYTE *data = NULL;
	bool gotdata = false;

	UINT *formats = NULL;
	UINT numformats = 0;

	FORMATETC *fel = NULL;
	UINT numfe = 0;

	bool NeedToChooseFormat = true;
	int IndexOfDataToInsert = -1;
	bool NeedToChooseMoveorCopy = (LastKeyState | grfKeyState) & (MK_MBUTTON | MK_RBUTTON) || hexwnd.always_pick_move_copy;

	if (hexwnd.dragging) // Internal data
	{
		hexwnd.dragging = FALSE;
		if (NeedToChooseMoveorCopy)
		{
			HMENU hm = GetSubMenu(hexwnd.hMenuContext, 2);
			BOOL mi = TrackPopupMenuEx(hm, TPM_NONOTIFY|TPM_RIGHTBUTTON|TPM_RETURNCMD, pt.x, pt.y, hexwnd.hwnd, NULL);
			if (mi == 0)
			{
				pDataObject->Release();
				*pdwEffect = DROPEFFECT_NONE;
				return S_OK;
			}
			copying = mi != 1;
		}
		int iMove1stEnd = hexwnd.iGetStartOfSelection();
		int iMove2ndEndorLen = hexwnd.iGetEndOfSelection();
		if (!copying && hexwnd.new_pos > iMove2ndEndorLen)
			hexwnd.new_pos += iMove1stEnd - iMove2ndEndorLen - 1;
		iMovePos = hexwnd.new_pos;
		iMoveOpTyp = copying ? OPTYP_COPY : OPTYP_MOVE;
		if (grfKeyState & MK_SHIFT) // Overwrite
		{
			int len = iMove2ndEndorLen - iMove1stEnd + 1;
			if (copying)
			{
				//Just [realloc &] memmove
				if (iMovePos + len > hexwnd.DataArray.GetLength()) // Need more space
				{
					if (hexwnd.DataArray.SetSize(iMovePos + len))
					{
						hexwnd.DataArray.ExpandToSize();
						memmove(&hexwnd.DataArray[iMovePos], &hexwnd.DataArray[iMove1stEnd], len);
					}
				}
				else // Enough space
				{
					memmove(&hexwnd.DataArray[iMovePos], &hexwnd.DataArray[iMove1stEnd], len);
				}
			}
			else //Moving
			{
				if (iMovePos > iMove1stEnd) //Forward
				{
					hexwnd.CMD_move_copy(iMove1stEnd, iMove2ndEndorLen, 0);
					hexwnd.DataArray.RemoveAt(iMovePos+len,len);
				}
				else //Backward
				{
					memmove(&hexwnd.DataArray[iMovePos],&hexwnd.DataArray[iMove1stEnd],len);
					hexwnd.DataArray.RemoveAt((iMovePos-iMove1stEnd>=len?iMove1stEnd:iMovePos+len),len);
				}
			}
			hexwnd.iStartOfSelection = iMovePos;
			hexwnd.iEndOfSelection = iMovePos+len-1;
			hexwnd.iFileChanged = hexwnd.bFilestatusChanged = hexwnd.bSelected = TRUE;
			hexwnd.resize_window();
		}
		else // Insert
		{
			hexwnd.CMD_move_copy(iMove1stEnd, iMove2ndEndorLen, 1);
		}
	}
	else // External data
	{
		STGMEDIUM stm;

		HRESULT err = E_UNEXPECTED;

		//Get the formats enumerator
		IEnumFORMATETC *iefe = 0;
		pDataObject->EnumFormatEtc( DATADIR_GET, &iefe );
		if (iefe == 0)
		{
#ifdef _DEBUG
			printf("Unable to create a drag-drop data enumerator\n");
#endif //_DEBUG
			goto ERR;
		}
		iefe->Reset();

		//Get the available formats
		for(;;)
		{
			void *temp = realloc(fel, ( numfe + 1 ) * sizeof(FORMATETC));
			if( temp != NULL ){
				fel = (FORMATETC*) temp;
				temp = NULL;
				int r;
				r = iefe->Next( 1, &fel[numfe], NULL);
				if( r != S_OK ) break;//No more formats
				numfe++;
			} else if( fel == NULL ) {
				//We only go here if nothing could be allocated
#ifdef _DEBUG
				printf("Not enough memory for the drag-drop format list\n");
#endif //_DEBUG
				goto ERR_ENUM;
			}
		}
		UINT i;
		/*Check which format should be inserted according to user preferences*/
		if (numfe == 0)
		{
			MessageBox(hexwnd.hwnd, _T("No data to insert"), _T("Drag-drop"), MB_OK);
			err = S_OK;
			*pdwEffect = DROPEFFECT_NONE;
			goto ERR_ENUM;
		}
		if (hexwnd.prefer_CF_HDROP)
		{
			for (i = 0 ; i < numfe ; i++)
			{
				if (fel[i].cfFormat == CF_HDROP)
				{
					//Return no effect & let shell32 handle it
					if (S_OK == pDataObject->GetData(&fel[i], &stm))
					{
						hexwnd.dropfiles((HDROP)stm.hGlobal);
					}
					err = S_OK;
					*pdwEffect = DROPEFFECT_NONE;
					goto ERR_ENUM;
				}
			}
		}
		if (numfe == 1)
		{
			IndexOfDataToInsert = 0;
		}
		else if (hexwnd.prefer_CF_BINARYDATA)
		{
			for (i = 0 ; i < numfe ; i++)
			{
				if (fel[i].cfFormat == CF_BINARYDATA)
				{
					NeedToChooseFormat = false;
					IndexOfDataToInsert = i;
					break;
				}
			}
		}
		else if (hexwnd.prefer_CF_TEXT)
		{
			for (i = 0 ; i < numfe ; i++ )
			{
				if (fel[i].cfFormat == CF_TEXT)
				{
					NeedToChooseFormat = false;
					IndexOfDataToInsert = i;
					break;
				}
			}
		}
		if (NeedToChooseFormat)
		{
			dialog<DragDropDlg> params;
			params.allowable_effects = dwOKEffects & ( DROPEFFECT_COPY | DROPEFFECT_MOVE );
			params.effect = copying;
			params.formatetcs = fel;
			params.numformatetcs = numfe;
			params.formats = NULL;
			params.numformats = 0;
			int ret = params.DoModal(hexwnd.hwnd);
			if (ret < 0)
			{
				//An error occured or the user canceled the operation
				err = S_OK;
				*pdwEffect = DROPEFFECT_NONE;
				goto ERR_ENUM;
			}
			numformats = params.numformats;
			formats = params.formats;
			copying = params.effect;
		}
		else if (NeedToChooseMoveorCopy)
		{
			HMENU hm = GetSubMenu(hexwnd.hMenuContext, 2);
			BOOL mi = TrackPopupMenuEx(hm, TPM_NONOTIFY|TPM_RIGHTBUTTON|TPM_RETURNCMD, pt.x, pt.y, hexwnd.hwnd, NULL);
			if (mi == 0)
			{
				err = S_OK;
				*pdwEffect = DROPEFFECT_NONE;
				goto ERR_ENUM;
			}
			copying = mi != 1;
		}

		if (IndexOfDataToInsert >= 0 && formats == NULL) {
			formats = (UINT*)&IndexOfDataToInsert;
			numformats = 1;
		}

		//for each selected format
		for (i = 0 ; i < numformats ; i++)
		{
			FORMATETC fe = fel[formats[i]];
			/*It is important that when debugging (with M$VC at least) you do not step __into__ the below GetData call
			  If you do the app providing the data source will die & GetData will return OLE_E_NOTRUNNING or E_FAIL
			  The solution is to step over the call
			  It is also possible that a debugger will be opened & attach itself to the data provider
			*/
			if (pDataObject->GetData(&fe, &stm) == S_OK)
			{
				//Get len
				size_t len = 0;
				switch (stm.tymed)
				{
				case TYMED_HGLOBAL:
					len = GlobalSize( stm.hGlobal );
					break;
#ifndef __CYGWIN__
				case TYMED_FILE:
					{
						int fh = _topen(static_cast<W2T>(stm.lpszFileName), _O_BINARY | _O_RDONLY);
						if (fh != -1)
						{
							len = _filelength(fh);
							if (len == (size_t)-1)
								len = 0;
							_close(fh);
						}
					}
					break;
#endif //__CYGWIN__
				case TYMED_ISTREAM:
					{
						STATSTG stat;
						if (S_OK == stm.pstm->Stat(&stat, STATFLAG_NONAME))
							len = (size_t)stat.cbSize.LowPart;
					}
					break;
					//This case is going to be a bitch to implement so it can wait for a while
					//It will need to be a recursive method that stores the STATSTG structures (+ the name), contents/the bytes of data in streams/property sets
				case TYMED_ISTORAGE:
					MessageBox(hexwnd.hwnd, _T("TYMED_ISTORAGE is not yet supported for drag-drop.\nPlease don't hesitate to write a patch & send in a diff."), _T("Drag-drop"), MB_OK);
					break;//IStorage*
				case TYMED_GDI:
					len = GetObject(stm.hBitmap, 0, NULL);
					if (len)
					{
						DIBSECTION t;
						GetObject(stm.hBitmap, len, &t);
						len += t.dsBm.bmHeight * t.dsBm.bmWidthBytes * t.dsBm.bmPlanes;
					}
					break;//HBITMAP
				case TYMED_MFPICT:
					len = GlobalSize(stm.hMetaFilePict);
					if (METAFILEPICT *pMFP = (METAFILEPICT*)GlobalLock(stm.hMetaFilePict))
					{
						len += GetMetaFileBitsEx(pMFP->hMF, 0, NULL);
						GlobalUnlock(stm.hMetaFilePict);
					}
					break;//HMETAFILE
#ifndef __CYGWIN__
				case TYMED_ENHMF:
					{
						len = GetEnhMetaFileHeader( stm.hEnhMetaFile, 0, NULL );
						DWORD n = GetEnhMetaFileDescriptionW( stm.hEnhMetaFile, 0, NULL );
						if( n && n != GDI_ERROR ) len += sizeof(WCHAR)*n;
						len += GetEnhMetaFileBits( stm.hEnhMetaFile, 0, NULL );
						n = GetEnhMetaFilePaletteEntries( stm.hEnhMetaFile, 0, NULL );
						if( n && n != GDI_ERROR ) len += sizeof(LOGPALETTE)+(n-1)*sizeof(PALETTEENTRY);
					}
					break;//HENHMETAFILE
#endif //__CYGWIN__
					//case TYMED_NULL:break;
				}
				if( !len ) continue;

				/*Malloc
				We do this so that if the data access fails we only need free(data)
				and don't need to mess around with the DataArray.
				Perhaps in the future the DataArray can support undoable actions.*/
				BYTE* t = (BYTE*)realloc(data, len);
				if( !t ) continue;
				data = t;
				memset( data, 0, len );

				//Get data
				switch (stm.tymed)
				{
				case TYMED_HGLOBAL:
					if (LPVOID pmem = GlobalLock(stm.hGlobal))
					{
						memcpy(data, pmem, len);
						gotdata = true;
						GlobalUnlock(stm.hGlobal);
					}
					break;
#ifndef __CYGWIN__
				case TYMED_FILE:
					{
						int fh = _topen(static_cast<W2T>(stm.lpszFileName), _O_BINARY | _O_RDONLY);
						if (fh != -1)
						{
							if (0 < _read(fh, data, len))
								gotdata = true;
							_close(fh);
						}
					}
					break;
#endif //__CYGWIN__
				case TYMED_ISTREAM:
					{
						LARGE_INTEGER zero = { 0 };
						ULARGE_INTEGER pos;
						if( S_OK == stm.pstm->Seek( zero, STREAM_SEEK_CUR, &pos ) ){
							stm.pstm->Seek( zero, STREAM_SEEK_SET, NULL );
							if( S_OK == stm.pstm->Read( data, len, NULL ) ) gotdata = true;
							stm.pstm->Seek( *(LARGE_INTEGER*)&pos, STREAM_SEEK_SET, NULL );
						}
					}
					break;
					//This case is going to be a bitch to implement so it can wait for a while
					//It will need to be a recursive method that stores the STATSTG structures (+ the name), contents/the bytes of data in streams/property sets
				case TYMED_ISTORAGE:
					MessageBox(hexwnd.hwnd, _T("TYMED_ISTORAGE is not yet supported for drag-drop.\nPlease don't hesitate to write a patch & send in a diff."), _T("Drag-drop"), MB_OK );
					goto ERR_ENUM;//IStorage*

				case TYMED_GDI:
					{
						int l = GetObject(stm.hBitmap, len, data);
						if (l)
						{
							BITMAP* bm = (BITMAP*)data;
							if (bm->bmBits)
								memcpy(&data[l], bm->bmBits, len - l);
							else
								GetBitmapBits(stm.hBitmap, len - l, &data[l]);
							gotdata = true;
						}
					}
					break;//HBITMAP
				case TYMED_MFPICT:
					if (METAFILEPICT *pMFP = (METAFILEPICT *)GlobalLock(stm.hMetaFilePict))
					{
						memcpy(data, pMFP, sizeof *pMFP);
						GetMetaFileBitsEx( pMFP->hMF, len - sizeof(*pMFP), &data[sizeof(*pMFP)] );
						GlobalUnlock( stm.hMetaFilePict );
						gotdata = true;
					}
					break;//HMETAFILE
#ifndef __CYGWIN__
				case TYMED_ENHMF:
					{
						DWORD i = 0, n = 0, l = len;
						n = GetEnhMetaFileHeader( stm.hEnhMetaFile, l, (ENHMETAHEADER*)&data[i] );
						l -= n; i += n;
						n = GetEnhMetaFileDescriptionW( stm.hEnhMetaFile, l/sizeof(WCHAR), (LPWSTR)&data[i] );
						if( n && n != GDI_ERROR ){
							n *= sizeof(WCHAR);l -= n; i += n;
						}
						n = GetEnhMetaFileBits( stm.hEnhMetaFile, l, &data[i] );
						l -= n; i += n;
						n = GetEnhMetaFilePaletteEntries( stm.hEnhMetaFile, 0, NULL );
						if( n && n != GDI_ERROR ){
							LOGPALETTE* lp = (LOGPALETTE*)&data[i];
							lp->palVersion = 0x300;
							lp->palNumEntries = (USHORT)n;
							l -=sizeof(lp->palVersion)+sizeof(lp->palNumEntries);
							n = GetEnhMetaFilePaletteEntries( stm.hEnhMetaFile, l/sizeof(PALETTEENTRY), &lp->palPalEntry[0] );
							i += n*sizeof(PALETTEENTRY);
						}
						if( i ) gotdata = true;
					}
					break;//HENHMETAFILE
#endif //__CYGWIN__
					//case TYMED_NULL:break;
				}

				ReleaseStgMedium(&stm);

				if (gotdata)
				{
					BYTE* DataToInsert = data;
					if (fe.cfFormat == CF_BINARYDATA)
					{
						len = *(DWORD*)data;
						DataToInsert += 4;
					}
					else if (fe.cfFormat == CF_TEXT || fe.cfFormat == CF_OEMTEXT)
					{
						len = strlen((char*)data);
					}
					else if (fe.cfFormat == CF_UNICODETEXT)
					{
						len = sizeof(wchar_t) * wcslen((wchar_t*)data);
					}
					//Insert/overwrite data into DataArray
					if (/*Overwite*/ grfKeyState & MK_SHIFT)
					{
						DWORD upper = 1+hexwnd.DataArray.GetUpperBound();
						if( /*Need more space*/ hexwnd.new_pos+len > upper )
						{
							if (hexwnd.DataArray.SetSize(hexwnd.new_pos + totallen + len))
							{
								hexwnd.DataArray.ExpandToSize();
								memcpy(&hexwnd.DataArray[hexwnd.new_pos + totallen], DataToInsert, len);
								gotdata = true;
								totallen += len;
							}
						}
						else /*Enough space*/
						{
							memcpy(&hexwnd.DataArray[hexwnd.new_pos + totallen], DataToInsert, len);
							gotdata = true;
							totallen += len;
						}
					}
					else /*Insert*/ if (hexwnd.DataArray.InsertAtGrow(hexwnd.new_pos + totallen, DataToInsert, 0, len))
					{
						gotdata = true;
						totallen += len;
					}
				}
			}
		} //for each selected format

		//Release the data
		free(data);
		data = NULL;
		if (IndexOfDataToInsert < 0)
		{
			free(formats);
			formats = NULL;
		}

		if (gotdata)
		{
			hexwnd.iStartOfSelection = hexwnd.new_pos;
			hexwnd.iEndOfSelection = hexwnd.new_pos + totallen - 1;
			hexwnd.iFileChanged = hexwnd.bFilestatusChanged = hexwnd.bSelected = TRUE;
			hexwnd.resize_window();
			hexwnd.synch_sibling();
		}

		*pdwEffect = copying  ? DROPEFFECT_COPY : DROPEFFECT_MOVE;

		err = S_OK;

ERR_ENUM:
		iefe->Release();
		free(fel);
ERR:
		pDataObject->Release();
		return err;
	}
	pDataObject->Release();
	return S_OK;
}
