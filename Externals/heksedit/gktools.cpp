#include "precomp.h"
#include "DllProxies.h"
#include <imagehlp.h>
#include <ctype.h>
#include "resource.h"
#include "hexwnd.h"
#include "gktools.h"
#include "simparr.h"

static PList PartitionInfoList;

BOOL WINAPI GetDllExportNames( LPCSTR pszFilename, ULONG* lpulOffset, ULONG* lpulSize )
{
	struct IMAGEHLP *IMAGEHLP = ::IMAGEHLP;
	if (IMAGEHLP == 0)
		return FALSE;

	LOADED_IMAGE li;
	if (!IMAGEHLP->MapAndLoad((LPSTR)pszFilename, NULL, &li, TRUE, TRUE))
		return FALSE;

	BOOL bDone = FALSE;
	if (DWORD dw = li.FileHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress)
	{
		PIMAGE_EXPORT_DIRECTORY pExpDir = (PIMAGE_EXPORT_DIRECTORY)
			IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, dw, 0);
		DWORD firstName = pExpDir->Name;
		DWORD lastName = pExpDir->Name;
		if (pExpDir->NumberOfNames)
		{
			DWORD *pExpNames = (DWORD *)
				IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, pExpDir->AddressOfNames, 0);
			pExpNames += pExpDir->NumberOfNames - 1;
			lastName = *pExpNames;
		}
		*lpulOffset = firstName;
		char *name = (char *)
			IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, lastName, 0);
		*lpulSize = lastName + strlen(name) + 1 - firstName;
		bDone = TRUE;
	}
	IMAGEHLP->UnMapAndLoad(&li);
	return bDone;
}

BOOL WINAPI GetDllImportNames( LPCSTR pszFilename, ULONG* lpulOffset, ULONG* lpulSize )
{
	struct IMAGEHLP *IMAGEHLP = ::IMAGEHLP;
	if (IMAGEHLP == 0)
		return FALSE;

	LOADED_IMAGE li;
	if (!IMAGEHLP->MapAndLoad((LPSTR)pszFilename, NULL, &li, TRUE, TRUE))
		return FALSE;

	BOOL bDone = FALSE;
	if (DWORD dw = li.FileHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress)
	{
		IMAGE_IMPORT_DESCRIPTOR *pDescriptor = (IMAGE_IMPORT_DESCRIPTOR *)
			IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, dw, 0);
		DWORD lower = ULONG_MAX;
		DWORD upper = 0;
		while (pDescriptor->FirstThunk)
		{
			if (lower > pDescriptor->Name) //OriginalFirstThunk)
				lower = pDescriptor->Name; //OriginalFirstThunk;
			char *name = (char *)
				IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, pDescriptor->Name, 0);
			DWORD end = pDescriptor->Name + strlen(name) + 1;
			if (upper < end)
				upper = end;
			DWORD *pEntry = (DWORD *)
				IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, pDescriptor->FirstThunk, 0);
			IMAGE_THUNK_DATA *pThunk = (IMAGE_THUNK_DATA *)
				IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, pDescriptor->OriginalFirstThunk, 0);
			while (pThunk->u1.Function)
			{
				if ((*pEntry & 0x80000000) == 0)
				{
					IMAGE_IMPORT_BY_NAME *pImport = (IMAGE_IMPORT_BY_NAME *)
						IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, *pEntry, 0);
					DWORD end = *pEntry + sizeof(IMAGE_IMPORT_BY_NAME) + strlen((char *)pImport->Name);
					if (lower > *pEntry)
						lower = *pEntry;
					if (upper < end)
						upper = end;
				}
				++pThunk;
				++pEntry;
			}
			++pDescriptor;
		}
		if (lower < upper)
		{
			*lpulOffset = lower;
			*lpulSize = upper - lower;
			bDone = TRUE;
		}
	}
	IMAGEHLP->UnMapAndLoad(&li);
	return bDone;
}

void WINAPI XorEncoder( MEMORY_CODING* p )
{
	LPBYTE q = p->lpbMemory;
	LPBYTE qMax = q+p->dwSize;
	while(q<qMax)
		*(q++)^=-1;
}

void WINAPI Rot13Encoder( LPMEMORY_CODING p )
{
	LPBYTE q = p->lpbMemory;
	LPBYTE qMax = q+p->dwSize;
	while(q<qMax)
		*(q++)=isalpha(*q)?(BYTE)(tolower(*q)<'n'?*q+13:*q-13):*q;
}

MEMORY_CODING_DESCRIPTION BuiltinEncoders[] =
{
	{ "ROT-13", Rot13Encoder },
	{ "XOR -1", XorEncoder },
	{ 0, 0 }
};

void AddEncoders(HWND hListbox, LPMEMORY_CODING_DESCRIPTION lpEncoders)
{
	for ( ; lpEncoders->lpszDescription ; ++lpEncoders)
	{
		int i = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)lpEncoders->lpszDescription);
		SendMessage(hListbox, LB_SETITEMDATA, i, (LPARAM)lpEncoders->fpEncodeFunc);
	}
}

INT_PTR EncodeDecodeDialog::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		{
			SimpleString buffer((LPSTR)(LPCSTR)EncodeDlls);
			LPCSTR lpszToken = strtok(buffer, ";");
			HWND hListbox = GetDlgItem(hDlg,IDC_LIST1);
			AddEncoders(hListbox, BuiltinEncoders);
			while (lpszToken)
			{
				HMODULE hLibrary = GetModuleHandle(lpszToken);
				if (hLibrary == 0)
					hLibrary = LoadLibrary(lpszToken);
				if (hLibrary)
				{
					if (LPFNGetMemoryCodings callback = (LPFNGetMemoryCodings)
						GetProcAddress(hLibrary, "GetMemoryCodings"))
					{
						AddEncoders(hListbox, callback());
					}
				}
				lpszToken = strtok(0, ";");
			}
			SendMessage(hListbox, LB_SETCURSEL, 0, 0);
			CheckDlgButton(hDlg, IDC_RADIO1, BST_CHECKED);
		}
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			{
				MEMORY_CODING mc;
				CHAR szBuffer[1024];
				GetDlgItemText(hDlg, IDC_EDIT1, szBuffer, sizeof(szBuffer));
				mc.bEncode = IsDlgButtonChecked(hDlg, IDC_RADIO1);
				mc.lpszArguments = szBuffer;
				HWND hListbox = GetDlgItem(hDlg,IDC_LIST1);
				int nCurSel = SendMessage(hListbox, LB_GETCURSEL, 0, 0);
				if (nCurSel < 0)
					return TRUE;
				mc.fpEncodeFunc = (LPFNEncodeMemoryFunction) SendMessage(hListbox,LB_GETITEMDATA,nCurSel,0);
				if (bSelected)
				{
					mc.lpbMemory = &DataArray[iStartOfSelection];
					mc.dwSize = iEndOfSelection - iStartOfSelection + 1;
				}
				else
				{
					mc.lpbMemory = DataArray;
					mc.dwSize = DataArray.GetLength();
				}
				mc.fpEncodeFunc(&mc);
				iFileChanged = TRUE;
				bFilestatusChanged = TRUE;
				repaint();
			}
			// fall through
		case IDCANCEL:
			EndDialog(hDlg, wParam);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

INT_PTR OpenDriveDialog::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		{
			ShowWindow(hDlg, SW_SHOW);
			if (PartitionInfoList.IsEmpty())
			{
				if (IPhysicalDrive *Drive = CreatePhysicalDriveInstance())
				{
					Drive->GetPartitionInfo(&PartitionInfoList);
					delete Drive;
				}
			}
			HWND hListbox = GetDlgItem(hDlg, IDC_LIST1);
			LIST_ENTRY *Flink = &PartitionInfoList;
			while ((Flink = Flink->Flink) != &PartitionInfoList)
			{
				PartitionInfo *pi = static_cast<PartitionInfo *>(Flink);
				int i = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)pi->GetNameAsString());
				SendMessage(hListbox, LB_SETITEMDATA, i, (LPARAM)pi);
			}
			SendMessage(hListbox, LB_SETCURSEL, 0, 0);
		}
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			{
				HWND hListbox = GetDlgItem(hDlg, IDC_LIST1);
				int nCurSel = SendMessage(hListbox, LB_GETCURSEL, 0, 0);
				if (nCurSel < 0)
					return TRUE;
				PartitionInfo *SelectedPartitionInfo = (PartitionInfo *)SendMessage(hListbox, LB_GETITEMDATA, nCurSel, 0);

				IPhysicalDrive *Drive = CreatePhysicalDriveInstance();
				if (Drive == 0 || !Drive->Open(SelectedPartitionInfo->m_dwDrive))
				{
					MessageBox(hwnd, "Unable to open drive", "Open Drive", MB_ICONERROR);
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
			EndDialog(hDlg, wParam);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

INT_PTR GotoTrackDialog::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		{
			CHAR szTempBuffer[10240];

			sprintf(szTempBuffer, "%I64d", CurrentSectorNumber);
			SetDlgItemText(hDlg, IDC_EDIT1, szTempBuffer);

			DISK_GEOMETRY dg;
			Drive->GetDriveGeometry(&dg);

			INT64 TotalSizeInBytes = dg.SectorsPerTrack;
			TotalSizeInBytes *= dg.BytesPerSector;
			TotalSizeInBytes *= dg.TracksPerCylinder;
			TotalSizeInBytes *= dg.Cylinders.QuadPart;

			sprintf(szTempBuffer,
				"Cylinders = %I64d\r\n"
				"Sectors = %I64d\r\n"
				"TracksPerCylinder = %ld\r\n"
				"SectorsPerTrack = %ld\r\n"
				"BytesPerSector = %ld\r\n"
				"TotalSizeInBytes = %I64d\r\n",
				dg.Cylinders.QuadPart,
				SelectedPartitionInfo->m_NumberOfSectors,
				dg.TracksPerCylinder,
				dg.SectorsPerTrack,
				dg.BytesPerSector,
				TotalSizeInBytes);

			SetDlgItemText(hDlg, IDC_EDIT3, szTempBuffer);
		}
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD (wParam))
		{
		case IDOK:
			{
				CHAR szBuffer[256];
				GetDlgItemText(hDlg,IDC_EDIT1, szBuffer, sizeof(szBuffer) );

				INT64 TempCurrentSectorNumber = 0;
				sscanf(szBuffer, "%I64d", &TempCurrentSectorNumber);
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
			EndDialog(hDlg, wParam);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
