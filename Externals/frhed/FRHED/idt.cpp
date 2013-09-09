/*
Frhed - Free hex editor
Copyright (C) 2000 Raihan Kibria

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

Last change: 2013-02-24 by Jochen Neubeck
*/
#include "precomp.h"
#include "idt.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "DragDropDlg.h"
#include "resource.h"
#include "StringTable.h"

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
	if (m_cRefCount != 0)
		printf("Deleting %s too early 0x%x.m_cRefCount = %d\n", "IDropTarget", this, m_cRefCount);
#endif //_DEBUG
	hexwnd.target = NULL;
}


//IUnknown methods
STDMETHODIMP CDropTarget::QueryInterface(REFIID iid, void** ppvObject)
{
#ifdef _DEBUG
	printf("IDropTarget::QueryInterface\n");
#endif //_DEBUG

	*ppvObject = NULL;

	if (iid == IID_IUnknown)
		*ppvObject = (IUnknown*)this;
	else if (iid == IID_IDropTarget)
		*ppvObject = (IDropTarget*)this;

	if (*ppvObject)
	{
		((IUnknown*)*ppvObject)->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CDropTarget::AddRef()
{
#ifdef _DEBUG
	printf("IDropTarget::AddRef\n");
#endif //_DEBUG
	return ++m_cRefCount;
}

STDMETHODIMP_(ULONG) CDropTarget::Release()
{
#ifdef _DEBUG
	printf("IDropTarget::Release\n");
#endif //_DEBUG
	if (--m_cRefCount == 0)
		delete this;
	return m_cRefCount;
}


//IDropTarget methods
STDMETHODIMP CDropTarget::DragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
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

	hexwnd.pwnd->CreateCaret((HBITMAP)1, 2, hexwnd.cyChar);

	return DragOver(grfKeyState, pt, pdwEffect);
}

STDMETHODIMP CDropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
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
	hexwnd.pwnd->ScreenToClient(&p);
	hexwnd.iMouseX = p.x;
	hexwnd.iMouseY = p.y;

	if (hdrop_present || *pdwEffect == DROPEFFECT_NONE)
		hexwnd.pwnd->HideCaret();
	else
		hexwnd.set_drag_caret(p.x, p.y, *pdwEffect == DROPEFFECT_COPY, (grfKeyState & MK_SHIFT) != 0);

	hexwnd.fix_scroll_timers(p.x,p.y);

	return S_OK;
}

STDMETHODIMP CDropTarget::DragLeave(void)
{
#ifdef _DEBUG
	printf("IDropTarget::DragLeave\n");
#endif //_DEBUG

	//This is the lesser of two evils
	//1. recreate the editing caret whenever the mouse leaves the client area
	//2. rather than have the caret disappear whenever the mouse leaves the client area (until set_focus is called)
	//#2 would just call CreateCaret
	hexwnd.pwnd->CreateCaret(NULL, hexwnd.cxChar, hexwnd.cyChar);
	hexwnd.set_caret_pos();
	if (HWindow::GetFocus() == hexwnd.pwnd)
		hexwnd.pwnd->ShowCaret();

	hexwnd.kill_scroll_timers();

	if (pDataObj)
	{
		pDataObj->Release();
		pDataObj = NULL;
	}

	return S_OK;
}

int CDropTarget::PopupDropMenu(POINTL pt)
{
	HMenu *pMenu = HMenu::CreatePopupMenu();
	pMenu->InsertMenu(0, MF_BYPOSITION | MF_STRING, 1, GetLangString(IDS_DD_MENU_MOVE));
	pMenu->InsertMenu(1, MF_BYPOSITION | MF_STRING, 2, GetLangString(IDS_DD_MENU_COPY));
	pMenu->InsertMenu(2, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
	pMenu->InsertMenu(3, MF_BYPOSITION | MF_STRING, 0, GetLangString(IDS_DD_MENU_CANCEL));
	int choice = pMenu->TrackPopupMenuEx(TPM_NONOTIFY|TPM_RIGHTBUTTON|TPM_RETURNCMD, pt.x, pt.y, hexwnd.pwnd);
	pMenu->DestroyMenu();
	return choice;
}

STDMETHODIMP CDropTarget::Drop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
#ifdef _DEBUG
	printf("IDropTarget::Drop\n");
#endif //_DEBUG

	DWORD dwOKEffects = *pdwEffect;
	{
		//We do this because grfKeyState will always have the mouse button used off
		DWORD temp = LastKeyState;
		//Get the position of the drop
		DragOver(grfKeyState, pt, pdwEffect);
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
			int choice = PopupDropMenu(pt);
			if (choice == 0)
			{
				pDataObject->Release();
				*pdwEffect = DROPEFFECT_NONE;
				return S_OK;
			}
			copying = choice != 1;
		}
		int iMove1stEnd = hexwnd.iGetStartOfSelection();
		int iMove2ndEndorLen = hexwnd.iGetEndOfSelection();
		if (!copying && hexwnd.new_pos > iMove2ndEndorLen)
			hexwnd.new_pos += iMove1stEnd - iMove2ndEndorLen - 1;
		iMovePos = hexwnd.new_pos;
		iMoveOpTyp = copying ? OPTYP_COPY : OPTYP_MOVE;
		if (grfKeyState & MK_SHIFT) // Overwrite
		{
			const int len = iMove2ndEndorLen - iMove1stEnd + 1;
			if (copying)
			{
				//Just [realloc &] memmove
				if (iMovePos + len > hexwnd.m_dataArray.GetLength()) // Need more space
				{
					if (hexwnd.m_dataArray.SetSize(iMovePos + len))
					{
						hexwnd.m_dataArray.ExpandToSize();
						memmove(&hexwnd.m_dataArray[iMovePos], &hexwnd.m_dataArray[iMove1stEnd], len);
					}
				}
				else // Enough space
				{
					memmove(&hexwnd.m_dataArray[iMovePos], &hexwnd.m_dataArray[iMove1stEnd], len);
				}
			}
			else //Moving
			{
				if (iMovePos > iMove1stEnd) //Forward
				{
					hexwnd.CMD_move_copy(iMove1stEnd, iMove2ndEndorLen, 0);
					hexwnd.m_dataArray.RemoveAt(iMovePos+len,len);
				}
				else //Backward
				{
					memmove(&hexwnd.m_dataArray[iMovePos],&hexwnd.m_dataArray[iMove1stEnd],len);
					hexwnd.m_dataArray.RemoveAt((iMovePos-iMove1stEnd>=len?iMove1stEnd:iMovePos+len),len);
				}
			}
			hexwnd.iStartOfSelection = iMovePos;
			hexwnd.iEndOfSelection = iMovePos+len-1;
			hexwnd.iFileChanged = hexwnd.bFilestatusChanged = true;
			hexwnd.bSelected = true;
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
		pDataObject->EnumFormatEtc(DATADIR_GET, &iefe);
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
			void *temp = realloc(fel, (numfe + 1) * sizeof(FORMATETC));
			if (temp != NULL)
			{
				fel = (FORMATETC*) temp;
				temp = NULL;
				int r;
				r = iefe->Next(1, &fel[numfe], NULL);
				if (r != S_OK)
					break;//No more formats
				numfe++;
			}
			else if (fel == NULL)
			{
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
			MessageBox(hexwnd.pwnd, GetLangString(IDS_DD_NO_DATA), MB_OK);
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
			for (i = 0 ; i < numfe ; i++)
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
			params.allowable_effects = dwOKEffects & (DROPEFFECT_COPY | DROPEFFECT_MOVE);
			params.effect = copying;
			params.formatetcs = fel;
			params.numformatetcs = numfe;
			params.formats = NULL;
			params.numformats = 0;
			int ret = params.DoModal(hexwnd.pwnd);
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
			int choice = PopupDropMenu(pt);
			if (choice == 0)
			{
				err = S_OK;
				*pdwEffect = DROPEFFECT_NONE;
				goto ERR_ENUM;
			}
			copying = choice != 1;
		}

		if (IndexOfDataToInsert >= 0 && formats == NULL)
		{
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
						len = GlobalSize(stm.hGlobal);
						break;
#ifndef __CYGWIN__
					case TYMED_FILE:
					{
						int fh = _wopen(stm.lpszFileName, _O_BINARY | _O_RDONLY);
						if (fh != -1)
						{
							len = _filelength(fh);
							if (len == (size_t)-1)
								len = 0;
							_close(fh);
						}
					} break;
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
						{
							MessageBox(hexwnd.pwnd, GetLangString(IDS_DD_TYMED_NOTSUP), MB_OK);
						} 
						break; // IStorage*
					case TYMED_GDI:
						{
							len = GetObject(stm.hBitmap, 0, NULL);
							if (len)
							{
								DIBSECTION t;
								GetObject(stm.hBitmap, len, &t);
								len += t.dsBm.bmHeight*t.dsBm.bmWidthBytes*t.dsBm.bmPlanes;
							}
						}
						break; // HBITMAP
					case TYMED_MFPICT:
						{
							len = GlobalSize(stm.hMetaFilePict);
							METAFILEPICT *pMFP = (METAFILEPICT*)GlobalLock(stm.hMetaFilePict);
							if (pMFP)
							{
								len += GetMetaFileBitsEx(pMFP->hMF, 0, NULL);
								GlobalUnlock(stm.hMetaFilePict);
							}
						}
						break; // HMETAFILE
#ifndef __CYGWIN__
					case TYMED_ENHMF:
						{
							len = GetEnhMetaFileHeader(stm.hEnhMetaFile, 0, NULL);
							DWORD n = GetEnhMetaFileDescriptionW(stm.hEnhMetaFile, 0, NULL);
							if (n && n != GDI_ERROR)
								len += sizeof(WCHAR) * n;
							len += GetEnhMetaFileBits(stm.hEnhMetaFile, 0, NULL);
							n = GetEnhMetaFilePaletteEntries(stm.hEnhMetaFile, 0, NULL);
							if (n && n != GDI_ERROR)
								len += sizeof(LOGPALETTE) + (n - 1) * sizeof(PALETTEENTRY);
						}
						break; // HENHMETAFILE
#endif //__CYGWIN__
					//case TYMED_NULL:break;
				}
				if (!len)
					continue;

				/*Malloc
				We do this so that if the data access fails we only need free(data)
				and don't need to mess around with the m_dataArray.
				Perhaps in the future the m_dataArray can support undoable actions.*/
				BYTE* t = (BYTE*)realloc(data, len);
				if (!t)
					continue;
				data = t;
				memset(data, 0, len);

				//Get data
				switch (stm.tymed)
				{
					case TYMED_HGLOBAL:
						{
							if (LPVOID pmem = GlobalLock(stm.hGlobal))
							{
								memcpy(data, pmem, len);
								gotdata = true;
								GlobalUnlock(stm.hGlobal);
							}
						}
						break;
#ifndef __CYGWIN__
					case TYMED_FILE:
						{
							int fh = _wopen(stm.lpszFileName, _O_BINARY | _O_RDONLY);
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
							if (S_OK == stm.pstm->Seek(zero, STREAM_SEEK_CUR, &pos))
							{
								stm.pstm->Seek(zero, STREAM_SEEK_SET, NULL);
								if (S_OK == stm.pstm->Read(data, len, NULL))
									gotdata = true;
								stm.pstm->Seek(*(LARGE_INTEGER*)&pos, STREAM_SEEK_SET, NULL);
							}
						}
						break;
					//This case is going to be a bitch to implement so it can wait for a while
					//It will need to be a recursive method that stores the STATSTG structures (+ the name), contents/the bytes of data in streams/property sets
					case TYMED_ISTORAGE:
						{
							MessageBox(hexwnd.pwnd, GetLangString(IDS_DD_TYMED_NOTSUP), MB_OK);
							goto ERR_ENUM;
						}
						break;//IStorage*
					case TYMED_GDI:
						{
							int l = GetObject(stm.hBitmap, len, data);
							if (l)
							{
								BITMAP* bm = (BITMAP*)data;
								if (bm->bmBits)
									memcpy(&data[l], bm->bmBits, len-l);
								else
									GetBitmapBits(stm.hBitmap, len-l, &data[l]);
								gotdata = true;
							}
						} break; // HBITMAP
					case TYMED_MFPICT:
						{
							if (METAFILEPICT *pMFP = (METAFILEPICT *)GlobalLock(stm.hMetaFilePict))
							{
								memcpy(data, pMFP, sizeof *pMFP);
								GetMetaFileBitsEx(pMFP->hMF, len - sizeof(*pMFP), &data[sizeof(*pMFP)]);
								GlobalUnlock(stm.hMetaFilePict);
								gotdata = true;
							}
						} break;//HMETAFILE
#ifndef __CYGWIN__
					case TYMED_ENHMF:
						{
							DWORD i = 0, n = 0, l = len;
							n = GetEnhMetaFileHeader(stm.hEnhMetaFile, l, (ENHMETAHEADER*)&data[i]);
							l -= n;
							i += n;
							n = GetEnhMetaFileDescriptionW(stm.hEnhMetaFile,
									l / sizeof(WCHAR), (LPWSTR)&data[i]);
							if (n && n != GDI_ERROR)
							{
								n *= sizeof(WCHAR);l -= n; i += n;
							}
							n = GetEnhMetaFileBits(stm.hEnhMetaFile, l, &data[i]);
							l -= n; i += n;
							n = GetEnhMetaFilePaletteEntries(stm.hEnhMetaFile, 0, NULL);
							if (n && n != GDI_ERROR)
							{
								LOGPALETTE* lp = (LOGPALETTE*)&data[i];
								lp->palVersion = 0x300;
								lp->palNumEntries = (USHORT)n;
								l -= sizeof(lp->palVersion) + sizeof(lp->palNumEntries);
								n = GetEnhMetaFilePaletteEntries(stm.hEnhMetaFile,
										l / sizeof(PALETTEENTRY), &lp->palPalEntry[0]);
								i += n*sizeof(PALETTEENTRY);
							}
							if (i)
								gotdata = true;
						} break; // HENHMETAFILE
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
						len = sizeof(wchar_t)*wcslen((wchar_t*)data);
					}

					// Insert/overwrite data into m_dataArray
					if (grfKeyState&MK_SHIFT)
					{
						/* Overwite */
						DWORD upper = 1 + hexwnd.m_dataArray.GetUpperBound();
						if (hexwnd.new_pos+len > upper)
						{
							/* Need more space */
							if (hexwnd.m_dataArray.SetSize(hexwnd.new_pos + totallen + len))
							{
								hexwnd.m_dataArray.ExpandToSize();
								memcpy(&hexwnd.m_dataArray[hexwnd.new_pos +
										(int)totallen], DataToInsert, len);
								gotdata = true;
								totallen += len;
							}
						}
						else
						{
							/* Enough space */
							memcpy(&hexwnd.m_dataArray[hexwnd.new_pos +
									(int)totallen], DataToInsert, len);
							gotdata = true;
							totallen += len;
						}
					}
					else if (hexwnd.m_dataArray.InsertAtGrow(hexwnd.new_pos + totallen, DataToInsert, 0, len))
					{
						/* Insert */
						gotdata = true;
						totallen += len;
					}
				}
			}

		} // For each selected format

		// Release the data
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
			hexwnd.iFileChanged = hexwnd.bFilestatusChanged = true;
			hexwnd.bSelected = true;
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
