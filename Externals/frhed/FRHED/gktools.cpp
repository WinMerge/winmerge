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

Last change: 2013-04-10 by Jochen Neubeck
*/
#include "precomp.h"
#include "DllProxies.h"
#include <imagehlp.h>
#include <ctype.h>
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "gktools.h"
#include "simparr.h"
#include "StringTable.h"
#include "AnsiConvert.h"

static PList PartitionInfoList;

BOOL WINAPI GetDllExportNames(LPCTSTR pszFilename, ULONG* lpulOffset, ULONG* lpulSize)
{
	struct IMAGEHLP *IMAGEHLP = ::IMAGEHLP;
	if (IMAGEHLP == 0)
		return FALSE;

	LOADED_IMAGE li;
	if (!IMAGEHLP->MapAndLoad(const_cast<LPSTR>(&*static_cast<T2A>(pszFilename)), NULL, &li, TRUE, TRUE))
		return FALSE;

	BOOL bDone = FALSE;
	if (DWORD dw = li.FileHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress)
	{
		PIMAGE_EXPORT_DIRECTORY pExpDir = (PIMAGE_EXPORT_DIRECTORY)
			IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, dw, 0);
		if (pExpDir)
		{
			DWORD firstName = pExpDir->Name;
			DWORD lastName = pExpDir->Name;
			if (pExpDir->NumberOfNames)
			{
				DWORD *pExpNames = (DWORD *)
					IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, pExpDir->AddressOfNames, 0);
				if (pExpNames)
				{
					pExpNames += pExpDir->NumberOfNames - 1;
					lastName = *pExpNames;
				}
			}
			char *pszFirstName = static_cast<char *>(
				IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, firstName, 0));
			char *pszLastName = static_cast<char *>(
				IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, lastName, 0));
			if (pszFirstName && pszLastName)
			{
				*lpulOffset = pszFirstName - reinterpret_cast<char *>(li.MappedAddress);
				*lpulSize = pszLastName + strlen(pszLastName) + 1 - pszFirstName;
			}
		}
		bDone = TRUE;
	}
	IMAGEHLP->UnMapAndLoad(&li);
	return bDone;
}

BOOL WINAPI GetDllImportNames(LPCTSTR pszFilename, ULONG* lpulOffset, ULONG* lpulSize)
{
	struct IMAGEHLP *IMAGEHLP = ::IMAGEHLP;
	if (IMAGEHLP == 0)
		return FALSE;

	LOADED_IMAGE li;
	if (!IMAGEHLP->MapAndLoad(const_cast<LPSTR>(&*static_cast<T2A>(pszFilename)), NULL, &li, TRUE, TRUE))
		return FALSE;

	BOOL bDone = FALSE;
	if (DWORD dw = li.FileHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress)
	{
		IMAGE_IMPORT_DESCRIPTOR *pDescriptor = (IMAGE_IMPORT_DESCRIPTOR *)
			IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, dw, 0);
		char *upper = reinterpret_cast<char *>(li.MappedAddress);
		char *lower = upper + li.SizeOfImage;
		while (pDescriptor->FirstThunk)
		{
			if (char *name = static_cast<char *>(
				IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress,
				pDescriptor->Name, 0)))
			{
				if (lower > name)
					lower = name;
				char *end = name + strlen(name) + 1;
				if (upper < end)
					upper = end;
				if (IMAGE_THUNK_DATA *pThunk = static_cast<IMAGE_THUNK_DATA *>(
					IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress,
					pDescriptor->OriginalFirstThunk, 0)))
				{
					// https://github.com/adoxa/cmdkey/blob/master/edit.c
					while (pThunk->u1.AddressOfData)
					{
						IMAGE_IMPORT_BY_NAME *pImport = static_cast<IMAGE_IMPORT_BY_NAME *>(
							IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, pThunk->u1.AddressOfData, 0));
						if (pImport)
						{
							char *name = reinterpret_cast<char *>(pImport->Name);
							char *end = name + strlen(name);
							if (lower > name)
								lower = name;
							if (upper < end)
								upper = end;
						}
						++pThunk;
					}
				}
			}
			++pDescriptor;
		}
		if (lower < upper)
		{
			*lpulOffset = lower - reinterpret_cast<char *>(li.MappedAddress);
			*lpulSize = upper - lower;
			bDone = TRUE;
		}
	}
	IMAGEHLP->UnMapAndLoad(&li);
	return bDone;
}

static const MEMORY_CODING_DESCRIPTION BuiltinEncoders[] =
{
	{ "ROT-13", Rot13Encoder },
	{ "XOR -1", XorEncoder },
	{ 0, 0 }
};

static void AddEncoders(HListBox *pListbox, const MEMORY_CODING_DESCRIPTION *lpEncoders)
{
	for ( ; lpEncoders->lpszDescription ; ++lpEncoders)
	{
		int i = pListbox->AddString(static_cast<A2T>(lpEncoders->lpszDescription));
		pListbox->SetItemDataPtr(i, const_cast<MEMORY_CODING_DESCRIPTION *>(lpEncoders));
	}
}

INT_PTR EncodeDecodeDialog::DlgProc(HWindow *pDlg, UINT uMsg, WPARAM wParam, LPARAM)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			HListBox *pListbox = static_cast<HListBox *>(pDlg->GetDlgItem(IDC_ENCODE_LIST));
			AddEncoders(pListbox, BuiltinEncoders);
			TCHAR path[MAX_PATH];
			GetModuleFileName(NULL, path, MAX_PATH);
			PathRemoveFileSpec(path);
			PathAppend(path, _T("Encoders\\*.dll"));
			WIN32_FIND_DATA ff;
			HANDLE h = FindFirstFile(path, &ff);
			if (h != INVALID_HANDLE_VALUE)
			{
				do
				{
					PathRemoveFileSpec(path);
					PathAppend(path, ff.cFileName);
					// Add encoder dll name with subfolder name
					HMODULE hLibrary = GetModuleHandle(path);
					if (hLibrary == 0)
						hLibrary = LoadLibrary(path);
					if (hLibrary)
					{
						if (LPFNGetMemoryCodings callback = (LPFNGetMemoryCodings)
							GetProcAddress(hLibrary, "GetMemoryCodings"))
						{
							AddEncoders(pListbox, callback());
						}
					}
				} while (FindNextFile(h, &ff));
				FindClose(h);
			}
			pListbox->SetCurSel(0);
			pDlg->CheckDlgButton(IDC_ENCODE_ENC, BST_CHECKED);
		}
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			{
				MEMORY_CODING mc;
				String sBuffer;
				pDlg->GetDlgItemText(IDC_ENCODE_ARGS, sBuffer);
				mc.bEncode = pDlg->IsDlgButtonChecked(IDC_ENCODE_ENC);
				mc.lpszArguments = sBuffer.c_str();
				HListBox *pListbox = static_cast<HListBox *>(pDlg->GetDlgItem(IDC_ENCODE_LIST));
				int nCurSel = pListbox->GetCurSel();
				if (nCurSel < 0)
					return TRUE;
				mc.fpMcd = static_cast<const MEMORY_CODING_DESCRIPTION *>(pListbox->GetItemDataPtr(nCurSel));
				int lower = 0;
				int upper = m_dataArray.GetUpperBound();
				if (bSelected)
				{
					lower = iGetStartOfSelection();
					upper = iGetEndOfSelection();
				}
				mc.lpbMemory = &m_dataArray[lower];
				mc.dwSize = upper - lower + 1;
				mc.fpMcd->fpEncodeFunc(&mc);
				iFileChanged = TRUE;
				bFilestatusChanged = true;
				repaint();
			}
			// fall through
		case IDCANCEL:
			pDlg->EndDialog(wParam);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

INT_PTR OpenDriveDialog::DlgProc(HWindow *pDlg, UINT uMsg, WPARAM wParam, LPARAM)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			pDlg->ShowWindow(SW_SHOW);
			if (PartitionInfoList.IsEmpty())
			{
				if (IPhysicalDrive *Drive = CreatePhysicalDriveInstance())
				{
					Drive->GetPartitionInfo(&PartitionInfoList);
					delete Drive;
				}
			}
			HListBox *list = static_cast<HListBox *>(pDlg->GetDlgItem(IDC_DRIVE_LIST));
			LIST_ENTRY *Flink = &PartitionInfoList;
			while ((Flink = Flink->Flink) != &PartitionInfoList)
			{
				PartitionInfo *pi = static_cast<PartitionInfo *>(Flink);
				int i = list->AddString(pi->GetNameAsString());
				list->SetItemDataPtr(i, pi);
			}
			list->SetCurSel(0);
		}
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			{
				HListBox *list = static_cast<HListBox *>(pDlg->GetDlgItem(IDC_DRIVE_LIST));
				int nCurSel = list->GetCurSel();
				if (nCurSel < 0)
					return TRUE;
				PartitionInfo *SelectedPartitionInfo =
					static_cast<PartitionInfo *>(list->GetItemDataPtr(nCurSel));

				IPhysicalDrive *Drive = CreatePhysicalDriveInstance();
				if (Drive == 0 || !Drive->Open(SelectedPartitionInfo->m_dwDrive))
				{
					MessageBox(pwnd, GetLangString(IDS_DRIVES_ERR_OPEN), MB_ICONERROR);
					delete Drive;
					return TRUE;
				}

				delete HexEditorWindow::Drive;
				HexEditorWindow::Drive = Drive;
				HexEditorWindow::SelectedPartitionInfo = SelectedPartitionInfo;
				EnableDriveButtons(TRUE);
				if (Track.GetObjectSize() != SelectedPartitionInfo->m_dwBytesPerSector)
					Track.Create(SelectedPartitionInfo->m_dwBytesPerSector);
				CurrentSectorNumber = 0;
				RefreshCurrentTrack();
			}
			// fall through
		case IDCANCEL:
			pDlg->EndDialog(wParam);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

INT_PTR GotoTrackDialog::DlgProc(HWindow *pDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			TCHAR szTempBuffer[10240];

			_stprintf(szTempBuffer, _T("%I64d"), CurrentSectorNumber);
			pDlg->SetDlgItemText(IDC_DRIVE_TRACK, szTempBuffer);

			DISK_GEOMETRY dg;
			Drive->GetDriveGeometry(&dg);

			INT64 TotalSizeInBytes = dg.SectorsPerTrack;
			TotalSizeInBytes *= dg.BytesPerSector;
			TotalSizeInBytes *= dg.TracksPerCylinder;
			TotalSizeInBytes *= dg.Cylinders.QuadPart;

			_stprintf(szTempBuffer,
				_T("%s = %I64d\r\n")
				_T("%s = %I64d\r\n")
				_T("%s = %ld\r\n")
				_T("%s = %ld\r\n")
				_T("%s = %ld\r\n")
				_T("%s = %I64d\r\n"),
				GetLangString(IDS_DRIVES_CYLINDERS),
				dg.Cylinders.QuadPart,
				GetLangString(IDS_DRIVES_SECTORS),
				SelectedPartitionInfo->m_NumberOfSectors,
				GetLangString(IDS_DRIVES_TRACSPERCYL),
				dg.TracksPerCylinder,
				GetLangString(IDS_DRIVES_SECTPERTRACK),
				dg.SectorsPerTrack,
				GetLangString(IDS_DRIVES_BYTESPERSECT),
				dg.BytesPerSector,
				GetLangString(IDS_DRIVES_TOTALBYTES),
				TotalSizeInBytes);

			pDlg->SetDlgItemText(IDC_DRIVE_INFO, szTempBuffer);
		}
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD (wParam))
		{
		case IDOK:
			{
				TCHAR szBuffer[256];
				pDlg->GetDlgItemText(IDC_DRIVE_TRACK, szBuffer, RTL_NUMBER_OF(szBuffer));

				INT64 TempCurrentSectorNumber = 0;
				_stscanf(szBuffer, _T("%I64d"), &TempCurrentSectorNumber);
				if (TempCurrentSectorNumber < 0 ||
					TempCurrentSectorNumber >= SelectedPartitionInfo->m_NumberOfSectors)
				{
					return TRUE;
				}
				CurrentSectorNumber = TempCurrentSectorNumber;
				RefreshCurrentTrack();
			}
			// fall through
		case IDCANCEL:
			pDlg->EndDialog(wParam);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
