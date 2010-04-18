#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include "Merge7z.h"

int main()
{
	TCHAR tmppath[260];
	TCHAR *zip = _T("D:/dev/WinMerge/TEST/ArchiveTest/7z458.tbz2");
	Merge7z *pMerge7z;
#ifdef _UNICODE
	HMODULE hLibrary = LoadLibrary(_T("Merge7zXXXU.dll"));
#else
	HMODULE hLibrary = LoadLibrary(_T("Merge7zXXX.dll"));
#endif
	pMerge7z = (Merge7z *)GetProcAddress(hLibrary, "Merge7z");
	pMerge7z->Initialize();
	Merge7z::Format *pFormat = pMerge7z->GuessFormat(zip);
	GetTempPath(sizeof(tmppath)/sizeof(tmppath[0]), tmppath);
	HRESULT hr = pFormat->DeCompressArchive(NULL, zip, tmppath);

	// test case
	// zipファイルが存在しない
	zip = _T("D:/dev/WinMerge/TEST/ArchiveTest/DesktopXXXXX.zip");
	pFormat = pMerge7z->GuessFormat(zip);
	hr =  pFormat->DeCompressArchive(NULL, zip, tmppath);

	// 7z.exeが存在しない
	// 展開先のフォルダが作れない
	// パスワード付きで正常パスワード
	zip = _T("D:/dev/WinMerge/TEST/ArchiveTest/passwordtest.zip");
	pFormat = pMerge7z->GuessFormat(zip);
	hr =  pFormat->DeCompressArchive(NULL, zip, tmppath);

	// パスワード付きで不正パスワード
	// RPMファイルの展開
	// 壊れたzipファイル
	// 展開中にキャンセル
	// Unicode/ANSIビルド
	return 0;
}