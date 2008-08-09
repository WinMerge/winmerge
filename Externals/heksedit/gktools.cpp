#include "precomp.h"
#include <imagehlp.h>
#include <ctype.h>
#include "resource.h"
#include "hexwnd.h"
#include "gktools.h"
#include "simparr.h"

static PList PartitionInfoList;

typedef BOOL (__stdcall* LPFNUnMapAndLoad)( PLOADED_IMAGE LoadedImage );
typedef PVOID (__stdcall* LPFNImageRvaToVa)(
	IN PIMAGE_NT_HEADERS NtHeaders,
	IN PVOID Base,
	IN ULONG Rva,
	IN OUT PIMAGE_SECTION_HEADER *LastRvaSection
);

typedef BOOL (__stdcall* LPFNMapAndLoad)(
	PSTR ImageName,
	PSTR DllPath,
	PLOADED_IMAGE LoadedImage,
	BOOL DotDll,
	BOOL ReadOnly
);

LPFNMapAndLoad fMapAndLoad = 0;
LPFNImageRvaToVa fImageRvaToVa = 0;
LPFNUnMapAndLoad fUnMapAndLoad = 0;

BOOL CanUseImagehelpDll()
{
	if( fUnMapAndLoad && fMapAndLoad && fImageRvaToVa )
		return TRUE;

	HMODULE hModule = LoadLibrary( "IMAGEHLP.DLL" );
	if( hModule )
	{
		fMapAndLoad = (LPFNMapAndLoad) GetProcAddress( hModule, "MapAndLoad" );
		fImageRvaToVa = (LPFNImageRvaToVa) GetProcAddress( hModule, "ImageRvaToVa" );
		fUnMapAndLoad = (LPFNUnMapAndLoad) GetProcAddress( hModule, "UnMapAndLoad" );

		return fUnMapAndLoad && fMapAndLoad && fImageRvaToVa;
	}
	return FALSE;
}

#define IRTV(x) fImageRvaToVa( li.FileHeader, li.MappedAddress, (DWORD)x, 0 )

BOOL WINAPI GetDllExportNames( LPCSTR pszFilename, ULONG* lpulOffset, ULONG* lpulSize )
{
	if( !CanUseImagehelpDll() )
		return FALSE;

	LOADED_IMAGE li;
	if( !fMapAndLoad( (LPSTR) pszFilename, NULL, &li, TRUE, TRUE ) )
		return FALSE;

	PIMAGE_EXPORT_DIRECTORY pExpDir = (PIMAGE_EXPORT_DIRECTORY)(li.FileHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	if( !pExpDir )
		return FALSE;

	pExpDir = (PIMAGE_EXPORT_DIRECTORY)IRTV(pExpDir);
	if( !pExpDir->NumberOfNames )
		return FALSE;

	PDWORD* pExpNames = (LPDWORD*) pExpDir->AddressOfNames;
	pExpNames = (LPDWORD*)IRTV(pExpNames);
	ULONG ulStart = (ULONG) IRTV(*pExpNames);
	*lpulOffset = ulStart - (ULONG) li.MappedAddress;
	pExpNames += pExpDir->NumberOfNames-1;
	ULONG ulStop = (ULONG) IRTV(*pExpNames);
	*lpulSize = ulStop - ulStart + strlen((LPCSTR)ulStop);	// hihi

	fUnMapAndLoad( &li );
	return TRUE;
}

// structures are undocumented
typedef struct
{
	// Addr +0 = start of import declaration
	// Addr +1,+2 are -1 always
	// Addr +3 = virtual name-of-dll
	// Addr +4 = ???
	ULONG Addr[5];
} IMPS0;

BOOL WINAPI GetDllImportNames( LPCSTR pszFilename, ULONG* lpulOffset, ULONG* lpulSize )
{
	if( !CanUseImagehelpDll() )
		return FALSE;

	LOADED_IMAGE li;
	if( !fMapAndLoad( (LPSTR) pszFilename, NULL, &li, TRUE, TRUE ) )
		return FALSE;

	PVOID pExpDir = (LPVOID)(li.FileHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	if( !pExpDir )
		return FALSE;

	IMPS0* p = (IMPS0*)IRTV(pExpDir);
	pExpDir = (PVOID)IRTV(p->Addr[0]);
	pExpDir = (PVOID)IRTV(*(ULONG*)pExpDir);
	*lpulOffset = (ULONG)pExpDir - (ULONG) li.MappedAddress;

	LPBYTE lpbEnd = (LPBYTE) pExpDir;
	while(!( !lpbEnd[0] && !lpbEnd[1] && !lpbEnd[2] && !lpbEnd[3] ))
	{
		lpbEnd++;
	}
	*lpulSize = (ULONG)lpbEnd - (ULONG) pExpDir;

	fUnMapAndLoad( &li );
	return TRUE;
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
			HWND hListbox = GetDlgItem(hDlg, IDC_LIST1);
			if (PartitionInfoList.IsEmpty())
			{
				if (IPhysicalDrive *Drive = CreatePhysicalDriveInstance())
				{
					Drive->GetPartitionInfo(&PartitionInfoList);
					delete Drive;
				}
			}
			ENUMERATE(&PartitionInfoList, PartitionInfo, pi)
			{
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
