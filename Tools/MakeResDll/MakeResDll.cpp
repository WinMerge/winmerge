// MakeResDll.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MakeResDll.h"
#include "coretools.h"
#include "RegKey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;
CString gsRCExe;
CString gsLinkExe;
CString gsLang;
CString gsIncludes;
CString gsLibs;
CString gsVcBin;
CString gsSharedBin;
CString gsRCScript;
CString gsOutPath;
CString gsVcBaseFolder;
BOOL gbPause=FALSE;
BOOL gbSilent=FALSE;
BOOL gbVerbose=FALSE;

BOOL BuildDll(LPCTSTR pszRCPath, LPCTSTR pszOutputPath, LPCTSTR pszOutputStem, CString& strOutFile);
BOOL CheckCompiler();
void Status(LPCTSTR szText);
void Status(UINT idstrText, LPCTSTR szText1 = NULL, LPCTSTR szText2 = NULL);
void InitModulePaths();
void Usage();
BOOL ProcessArgs(int argc, TCHAR* argv[]);
void FixPath();

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print an error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		if (argc < 2)
		{
			Usage();
			return 1;
		}

		InitModulePaths();

		if (!ProcessArgs(argc, argv))
		{
			Usage();
			return 1;
		}

		TCHAR path[MAX_PATH], name[MAX_PATH], ext[MAX_PATH];
		split_filename(gsRCScript, path, name, ext);
		if (gsOutPath.IsEmpty())
			gsOutPath = path;

		Status(IDS_CHECK_COMPILER_FMT);
		if (!CheckCompiler())
			return 1;
		Status(_T("OK\r\n"));

		FixPath();

		CString s, strOutFile;
		if (BuildDll(gsRCScript, gsOutPath, name, strOutFile)
			&& !gbSilent)
		{
			AfxFormatString1(s, IDS_SUCCESS_FMT, strOutFile);
			AfxMessageBox(s, MB_ICONINFORMATION);
		}

		if (gbPause && !gbSilent)
		{
			Status(IDS_PRESS_ANY_KEY);
			Status(_T("\r\n"));
			getc(stdin);
		}
	}

	return nRetCode;
}

void FixPath()
{
	CString strPath(getenv(_T("PATH")));
	TCHAR path[MAX_PATH];
	if (gbVerbose)
		_tprintf(_T("Initial path: %s\r\n"), strPath);

	strPath = _T("PATH=") + strPath;
	split_filename(gsRCExe, path, NULL, NULL);
	strPath += _T(";");
	strPath += path;
	split_filename(gsLinkExe, path, NULL, NULL);
	strPath += _T(";");
	strPath += path;
	strPath += _T(";");
	putenv(strPath);
	if (gbVerbose)
		_tprintf(_T("New path: %s\r\n"), strPath);
}

BOOL ProcessArgs(int argc, TCHAR* argv[])
{
	gsLang.Format(_T("%04x"), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
	gsRCScript = argv[argc-1];

	for (int i=1; i < argc-1; i++)
	{
		if (!_tcsicmp(argv[i], _T("-r"))
			|| !_tcsicmp(argv[i], _T("/r")))
		{
			i++;
			if (i < argc-1)
				gsRCExe = argv[i];
			else
				return FALSE;
		}
		else if (!_tcsicmp(argv[i], _T("-p"))
			|| !_tcsicmp(argv[i], _T("/p")))
		{
			gbPause=TRUE;
		}
		else if (!_tcsicmp(argv[i], _T("-s"))
			|| !_tcsicmp(argv[i], _T("/s")))
		{
			if (!gbVerbose)
				gbSilent=TRUE;
		}
		else if (!_tcsicmp(argv[i], _T("-v"))
			|| !_tcsicmp(argv[i], _T("/v")))
		{
			gbVerbose=TRUE;
			gbSilent=FALSE;
		}
		else if (!_tcsicmp(argv[i], _T("-l"))
			|| !_tcsicmp(argv[i], _T("/l")))
		{
			i++;
			if (i < argc-1)
				gsLinkExe = argv[i];
			else
				return FALSE;
		}
		else if (!_tcsicmp(argv[i], _T("-g"))
			|| !_tcsicmp(argv[i], _T("/g")))
		{
			i++;
			if (i < argc-1)
				gsLang = argv[i];
			else
				return FALSE;
		}
		else if (!_tcsicmp(argv[i], _T("-i"))
			|| !_tcsicmp(argv[i], _T("/i")))
		{
			i++;
			if (i < argc-1)
			{
				gsIncludes = argv[i];
			}
			else
				return FALSE;
		}
		else if (!_tcsicmp(argv[i], _T("-o"))
			|| !_tcsicmp(argv[i], _T("/o")))
		{
			i++;
			if (i < argc-1)
			{
				gsOutPath = argv[i];
				MkDirEx(gsOutPath);
			}
			else
				return FALSE;
		}
		else if (!_tcsicmp(argv[i], _T("-y"))
			|| !_tcsicmp(argv[i], _T("/y")))
		{
			i++;
			if (i < argc-1)
			{
				gsLibs = _T("");
				TCHAR temp[2048], *p;
				_tcscpy(temp, argv[i]);
				p = _tcstok(temp, ";\r\n\t");
				while (p != NULL)
				{
					gsLibs += _T("/libpath:\"");
					gsLibs += p;
					gsLibs += _T("\" ");
					p = _tcstok(NULL, ";\r\n\t");
				}

			}
			else
				return FALSE;
		}
	}
	return TRUE;
}


void Usage()
{
	if (gbSilent)
		return;

	_putts(_T("USAGE:  MakeResDll [options]  MyRcScript.rc\r\n"));
	_putts(_T("OPTIONS:\r\n"));
	_putts(_T("\t/p : Pause after build\r\n"));
	_putts(_T("\t/s : Run silently\r\n"));
	_putts(_T("\t/v : Verbose output\r\n"));
	_putts(_T("\t/o <output path>: Specify the output directory for the language DLL\r\n"));
	_putts(_T("\t/r <path to rc.exe>: Specify the path to the resource compiler executable\r\n"));
	_putts(_T("\t/l <path to link.exe>: Specify the path to the linker\r\n"));
	_putts(_T("\t/i <include paths separated by semicolons>: Specify include paths\r\n"));
	_putts(_T("\t/y <library paths separated by semicolons>: Specify library paths\r\n"));
	_putts(_T("\t/g <lang_id>: Specify the language ID\r\n"));
	_putts(_T("\r\n"));
}

BOOL BuildDll(LPCTSTR pszRCPath, LPCTSTR pszOutputPath, LPCTSTR pszOutputStem, CString& strOutFile)
{
	CString strLinkArgs;
	CString libs;
	TCHAR temp[2048], *p;
	CFileStatus status;
	HANDLE hLink;
	CString strOutFolder(pszOutputPath);
	CString strStem(pszOutputStem);

	TCHAR szScriptDir[MAX_PATH];
	split_filename(pszRCPath, szScriptDir, NULL, NULL);

	Status(IDS_CREATE_OUTDIR);
	MkDirEx(strOutFolder);
	Status(_T("OK\r\n"));

	CString strRCArgs;
	strRCArgs.Format(_T("/l 0x%s /fo\"%s\\%s.res\" /i \"%s\" ")
					 _T("/d \"_AFXDLL\" /d \"CORTRON_BUILD\" \"%s\""),
		gsLang,
		strOutFolder,
		strStem,
		gsIncludes,
		pszRCPath);
	if (gbVerbose)
		_tprintf(_T("%s  %s\r\n\r\n"), gsRCExe, strRCArgs);

	Status(IDS_BUILD_RC);
	HANDLE hRC = RunIt(gsRCExe, strRCArgs, TRUE, FALSE);
	if (hRC)
	{
		DWORD dwReturn;
		while (!HasExited(hRC, &dwReturn))
		{
		}
		if (dwReturn != 0)
		{
			Status(_T("Error\r\n"));
			goto build_failed;
		}
		Status(_T("Done\r\n"));
	}
	else
		Status(_T("Error creating process\r\n"));
	
	_tcscpy(temp, gsLibs);
	p = _tcstok(temp, ";\r\n\t");
	while (p != NULL)
	{
		libs += _T("/libpath:\"");
		libs += p;
		libs += _T("\" ");
		p = _tcstok(NULL, ";\r\n\t");
	}

	strOutFile = strOutFolder + _T("\\") + strStem + _T(".lang");

	strLinkArgs.Format(_T("/nologo /subsystem:console /dll ")
					   _T("/machine:I386 %s ")
					   _T("/noentry ")
					   _T("/out:\"%s\" ")
					   _T("\"%s\\%s.res\" "),
					   libs,
					   strOutFile,
					   strOutFolder,
					   strStem);						
	if (gbVerbose)
		_tprintf(_T("%s  %s\r\n\r\n"), gsLinkExe, strLinkArgs);

	Status(IDS_LINK);
	hLink = RunIt(gsLinkExe, strLinkArgs, TRUE, FALSE);
	if (hLink)
	{
		DWORD dwReturn;
		while (!HasExited(hLink, &dwReturn))
		{
		}
		if (dwReturn != 0)
		{
			Status(_T("Error\r\n"));
			goto build_failed;
		}
		Status(_T("Done\r\n"));
	}
	else
	{
		Status(_T("Error creating process\r\n"));
		goto build_failed;
	}

	Status(IDS_SUCCESS);
	Status(_T("\r\n"));
	Status(_T("    ")+strOutFile);
	Status(_T("\r\n"));
	return TRUE;

build_failed:
	Status(IDS_ABORT);
	return FALSE;
}

BOOL CheckCompiler()
{
	// look for the compiler
	CFileStatus status;
	
	if (!CFile::GetStatus(gsRCExe, status))
	{
		Status(IDS_BAD_RC_PATH_FMT, gsRCExe);
		Usage();
		return FALSE;
	}

	if (!CFile::GetStatus(gsLinkExe, status))
	{
		Status(IDS_BAD_LINK_PATH_FMT, gsLinkExe);
		Usage();
		return FALSE;
	}

	return TRUE;
}

void Status(LPCTSTR szText)
{
	if (!gbSilent)
		_tprintf(_T("%s"), szText);
}

void Status(UINT idstrText, LPCTSTR szText1 /*= NULL*/, LPCTSTR szText2 /*= NULL*/)
{
	if (gbSilent)
		return;

	TRY {
		CString s;
		if (szText1!=NULL && szText2!=NULL)
			AfxFormatString2(s, idstrText, szText1, szText2);
		else if (szText1!=NULL)
			AfxFormatString1(s, idstrText, szText1);
		else
			s.LoadString(idstrText);
		Status(s);
	}
	CATCH_ALL (e)
	{
	}
	END_CATCH_ALL;
}


void InitModulePaths()
{
	// read RC compiler locations
	CRegKeyEx reg;
	gsRCExe = _T("");
	gsLinkExe = _T("");
	gsVcBaseFolder = _T("");
	gsIncludes = _T("");
	gsLibs = _T("");
	if (reg.Open(HKEY_CURRENT_USER, _T("Software\\Thingamahoochie\\MakeResDll\\Settings")) == ERROR_SUCCESS)
	{
		gsVcBaseFolder = reg.ReadString(_T("VcBaseFolder"), _T(""));
		gsRCExe = reg.ReadString(_T("RCExe"), _T(""));
		gsLinkExe = reg.ReadString(_T("LinkExe"), _T(""));
		reg.Close();
	}

	// check for devstudio 6
	if (gsVcBaseFolder.IsEmpty()
		&& reg.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\DevStudio\\6.0\\Directories")) == ERROR_SUCCESS)
	{
		TCHAR path[MAX_PATH], path2[MAX_PATH], path3[MAX_PATH], name[MAX_PATH];
		gsVcBaseFolder = reg.ReadString(_T("Install Dirs"), _T(""));
		reg.Close();
		if (!gsVcBaseFolder.IsEmpty())
		{
			if (gsRCExe.IsEmpty())
				gsRCExe.Format(_T("%s\\rc.exe"), gsVcBaseFolder);

			split_filename(gsVcBaseFolder, path, name, NULL);
			split_filename(path, path2, name, NULL);
			split_filename(path2, path3, name, NULL);
			gsVcBaseFolder = path3;
			if (gsLinkExe.IsEmpty())
				gsLinkExe.Format(_T("%s\\vc98\\bin\\link.exe"), path3);
		}
	}

	// check for devstudio 5
	if (gsVcBaseFolder.IsEmpty()
		&& reg.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\DevStudio\\5.0\\Directories")) == ERROR_SUCCESS)
	{
		gsVcBaseFolder = reg.ReadString(_T("ProductDir"), _T(""));
		reg.Close();
		if (!gsVcBaseFolder.IsEmpty())
		{
			if (gsRCExe.IsEmpty())
				gsRCExe.Format(_T("%s\\SharedIDE\\bin\\rc.exe"), gsVcBaseFolder);
			if (gsLinkExe.IsEmpty())
				gsLinkExe.Format(_T("%s\\vc\\bin\\link.exe"), gsVcBaseFolder);
		}
	}



	if (reg.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\DevStudio\\5.0\\Build System\\Components\\Platforms\\Win32 (x86)\\Directories")) == ERROR_SUCCESS)
	{
		gsIncludes = reg.ReadString(_T("Include Dirs"), _T(""));
		gsLibs = reg.ReadString(_T("Library Dirs"), _T(""));
		reg.Close();
	}

	_tprintf(_T("Build paths:\r\n"));
	_tprintf(_T("	%s\r\n"), gsRCExe);
	_tprintf(_T("	%s\r\n"), gsLinkExe);
}