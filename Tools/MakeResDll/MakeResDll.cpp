/** 
 * @file  MakeResDll.cpp
 *
 * @brief Code to compile & link a language satellite resource DLL, using Visual Studio
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "MakeResDll.h"
// Following files included from WinMerge/Src/Common
#include "coretools.h"
#include "RegKey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

struct VcPaths
{
	CString sRCExe;
	CString sLinkExe;
	CString sVcBaseFolder;
	CString sIncludes;
	CString sLibs;
	bool needsInfo() const { return sRCExe.IsEmpty() || sLinkExe.IsEmpty() || sIncludes.IsEmpty() || sLibs.IsEmpty(); }
	void Clear() { sRCExe = _T(""); sLinkExe = _T(""); sVcBaseFolder = _T(""); sIncludes = _T(""); sLibs = _T(""); }
};

static VcPaths gVcPaths;

CWinApp theApp;
CString gsLang;
CString gsVcBin;
CString gsRCScript;
CString gsOutPath;
BOOL gbPause=FALSE;
BOOL gbBatch=FALSE;
BOOL gbSilent=FALSE;
BOOL gbVerbose=FALSE;

static BOOL BuildDll(LPCTSTR pszRCPath, LPCTSTR pszOutputPath, LPCTSTR pszOutputStem, CString& strOutFile);
static BOOL CheckCompiler();
static void Status(LPCTSTR szText);
static void Status(UINT idstrText, LPCTSTR szText1 = NULL, LPCTSTR szText2 = NULL);
static void InitModulePaths();
static void Usage();
static BOOL ProcessArgs(int argc, TCHAR* argv[]);
static void FixPath();
static bool DoesFileExist(LPCTSTR filepath);

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

		CString spath, sname, sext;
		SplitFilename(gsRCScript, &spath, &sname, &sext);
		if (gsOutPath.IsEmpty())
			gsOutPath = spath;

		Status(IDS_CHECK_COMPILER_FMT);
		if (!CheckCompiler())
			return 1;
		Status(_T("OK\r\n"));

		FixPath();

		CString s, strOutFile;
		BOOL ok = BuildDll(gsRCScript, gsOutPath, sname, strOutFile);
		if (!ok)
			nRetCode = 1;
		if (ok && !gbSilent && !gbBatch)
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

static void FixPath()
{
	CString strPath(getenv(_T("PATH")));
	CString spath;
	if (gbVerbose)
		_tprintf(_T("Initial path: %s\r\n"), strPath);

	strPath = _T("PATH=") + strPath;
	SplitFilename(gVcPaths.sRCExe, &spath, NULL, NULL);
	strPath += _T(";");
	strPath += spath;
	SplitFilename(gVcPaths.sLinkExe, &spath, NULL, NULL);
	strPath += _T(";");
	strPath += spath;
	strPath += _T(";");
	putenv(strPath);
	if (gbVerbose)
		_tprintf(_T("New path: %s\r\n"), strPath);
}

// Display status message saying this arg requires another arg
// and return FALSE
static BOOL MissingArg(LPCTSTR arg)
{
	Status(IDS_ERROR_MISSING_SWITCH_ARG, arg);
	return FALSE;
}

static BOOL ProcessArgs(int argc, TCHAR* argv[])
{
	gsLang.Format(_T("%04x"), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
	gsRCScript = _T("");

	for (int i=1; i < argc; i++)
	{
		if (!_tcsicmp(argv[i], _T("-r"))
			|| !_tcsicmp(argv[i], _T("/r")))
		{
			i++;
			if (i < argc-1)
				gVcPaths.sRCExe = argv[i];
			else
				return MissingArg(argv[i]);
		}
		else if (!_tcsicmp(argv[i], _T("-p"))
			|| !_tcsicmp(argv[i], _T("/p")))
		{
			gbPause=TRUE;
		}
		else if (!_tcsicmp(argv[i], _T("-b"))
			|| !_tcsicmp(argv[i], _T("/b")))
		{
			if (!gbBatch)
				gbBatch=TRUE;
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
				gVcPaths.sLinkExe = argv[i];
			else
				return MissingArg(argv[i]);
		}
		else if (!_tcsicmp(argv[i], _T("-g"))
			|| !_tcsicmp(argv[i], _T("/g")))
		{
			i++;
			if (i < argc-1)
				gsLang = argv[i];
			else
				return MissingArg(argv[i]);
		}
		else if (!_tcsicmp(argv[i], _T("-i"))
			|| !_tcsicmp(argv[i], _T("/i")))
		{
			i++;
			if (i < argc-1)
			{
				gVcPaths.sIncludes = argv[i];
			}
			else
				return MissingArg(argv[i]);
		}
		else if (!_tcsicmp(argv[i], _T("-o"))
			|| !_tcsicmp(argv[i], _T("/o")))
		{
			i++;
			if (i < argc-1)
			{
				gsOutPath = argv[i];
				if (!MkDirEx(gsOutPath))
				{
					Status(IDS_ERROR_MKDIR, gsOutPath);
					return FALSE;
				}
			}
			else
				return MissingArg(argv[i]);
		}
		else if (!_tcsicmp(argv[i], _T("-y"))
			|| !_tcsicmp(argv[i], _T("/y")))
		{
			i++;
			if (i < argc-1)
			{
				gVcPaths.sLibs = _T("");
				TCHAR temp[2048], *p;
				_tcscpy(temp, argv[i]);
				p = _tcstok(temp, ";\r\n\t");
				while (p != NULL)
				{
					gVcPaths.sLibs += _T("/libpath:\"");
					gVcPaths.sLibs += p;
					gVcPaths.sLibs += _T("\" ");
					p = _tcstok(NULL, ";\r\n\t");
				}

			}
			else
				return MissingArg(argv[i]);
		}
		else
		{
			if (i != argc-1)
			{
				Status(IDS_LAST_ARG_SHOULD_BE_RC, argv[i]);
				return FALSE;
			}
			gsRCScript = argv[i];
		}
	}
	if (gsRCScript.IsEmpty())
	{
		Status(IDS_LAST_ARG_SHOULD_BE_RC, argv[argc-1]);
		return FALSE;
	}
	return TRUE;
}

// Display resource string on its own line
static void displine(int nId)
{
	if (!nId)
	{
		_tprintf(_T("\n"));
		return;
	}
	CString str;
	str.LoadString(nId);
	_tprintf(_T("%s\n"), (LPCTSTR)str);
}

// Display usage information
static void Usage()
{
	if (gbSilent)
		return;

	displine(0);
	displine(IDS_USAGE_TITLE);
	displine(IDS_USAGE_OPTIONS);
	displine(IDS_USAGE_SLASH_P);
	displine(IDS_USAGE_SLASH_S);
	displine(IDS_USAGE_SLASH_B);
	displine(IDS_USAGE_SLASH_V);
	displine(IDS_USAGE_SLASH_O);
	displine(IDS_USAGE_SLASH_R);
	displine(IDS_USAGE_SLASH_I);
	displine(IDS_USAGE_SLASH_L);
	displine(IDS_USAGE_SLASH_Y);
	displine(IDS_USAGE_SLASH_G);
	displine(0);
}

static BOOL BuildDll(LPCTSTR pszRCPath, LPCTSTR pszOutputPath, LPCTSTR pszOutputStem, CString& strOutFile)
{
	CString strLinkArgs;
	CString libs;
	TCHAR temp[2048], *p;
	HANDLE hLink;
	CString strOutFolder(pszOutputPath);
	CString strStem(pszOutputStem);

	// Check RC file exists
	if (!DoesFileExist(pszRCPath))
	{
		Status(IDS_MISSING_RC_FILE, pszRCPath);
		return FALSE;
	}

	CString sScriptDir;
	SplitFilename(pszRCPath, &sScriptDir, NULL, NULL);

	Status(IDS_CREATE_OUTDIR);
	if (!MkDirEx(strOutFolder))
	{
		Status(IDS_ERROR_MKDIR, gsOutPath);
		return FALSE;
	}
	Status(_T("OK\r\n"));

	CString strRCArgs;
	strRCArgs.Format(_T("/l 0x%s /fo\"%s\\%s.res\" /i \"%s\" ")
					 _T("/d \"_AFXDLL\" /d \"CORTRON_BUILD\" \"%s\""),
		gsLang,
		strOutFolder,
		strStem,
		gVcPaths.sIncludes,
		pszRCPath);
	if (gbVerbose)
		_tprintf(_T("%s  %s\r\n\r\n"), gVcPaths.sRCExe, strRCArgs);

	Status(IDS_BUILD_RC);
	HANDLE hRC = RunIt(gVcPaths.sRCExe, strRCArgs, TRUE, FALSE);
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
	
	_tcscpy(temp, gVcPaths.sLibs);
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
		_tprintf(_T("%s  %s\r\n\r\n"), gVcPaths.sLinkExe, strLinkArgs);

	Status(IDS_LINK);
	hLink = RunIt(gVcPaths.sLinkExe, strLinkArgs, TRUE, FALSE);
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

static bool DoesFileExist(LPCTSTR filepath)
{
	CFileStatus status;
	return CFile::GetStatus(filepath, status) != FALSE;
}

static BOOL CheckCompiler()
{
	// look for the compiler
	if (!DoesFileExist(gVcPaths.sRCExe))
	{
		Status(IDS_BAD_RC_PATH_FMT, gVcPaths.sRCExe);
		Usage();
		return FALSE;
	}

	// look for the linker
	if (!DoesFileExist(gVcPaths.sLinkExe))
	{
		Status(IDS_BAD_LINK_PATH_FMT, gVcPaths.sLinkExe);
		Usage();
		return FALSE;
	}

	return TRUE;
}

static void Status(LPCTSTR szText)
{
	if (!gbSilent)
		_tprintf(_T("%s"), szText);
}

static void Status(UINT idstrText, LPCTSTR szText1 /*= NULL*/, LPCTSTR szText2 /*= NULL*/)
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

// Read-only open of registry key under HKEY_LOCAL_MACHINE
static bool
QueryRegMachine(CRegKeyEx & reg, LPCTSTR key)
{
	return reg.OpenNoCreateWithAccess(HKEY_LOCAL_MACHINE, key, KEY_QUERY_VALUE) == ERROR_SUCCESS;
}

// Read-only open of registry key under HKEY_CURRENT_USER
static bool
QueryRegUser(CRegKeyEx & reg, LPCTSTR key)
{
	return reg.OpenNoCreateWithAccess(HKEY_CURRENT_USER, key, KEY_QUERY_VALUE) == ERROR_SUCCESS;
}


// Find locations of RC compiler and linker
static void InitModulePaths()
{
	// Initialize module variables
	gVcPaths.Clear();

	// All our work is looking for entries in the registry
	CRegKeyEx reg;

	// Strategy is that we keep looking as long as we need anything (in gVcPaths)

	CString sVcVersion;

	// Check for user-configured overrides
	LPCTSTR settings = _T("Software\\Thingamahoochie\\MakeResDll\\Settings");
	if (QueryRegUser(reg, settings))
	{
		gVcPaths.sVcBaseFolder = reg.ReadString(_T("VcBaseFolder"), _T(""));
		if (gVcPaths.sRCExe.IsEmpty())
			gVcPaths.sRCExe = reg.ReadString(_T("RCExe"), _T(""));
		if (gVcPaths.sLinkExe.IsEmpty())
			gVcPaths.sLinkExe = reg.ReadString(_T("LinkExe"), _T(""));
		sVcVersion = reg.ReadString(_T("VcVersion"), _T(""));
		reg.Close();
	}

	// NB
	// The default install includes for VisualStudio .NET do not include MFC
	// which appears to be
	// INCLUDE: %MSVCDir%\ATLMFC\INCLUDE
	// LIB: %MSVCDir%\ATLMFC\LIB

	// check for VisualStudio .NET 2003
	LPCTSTR dirs71 = _T("SOFTWARE\\Microsoft\\VisualStudio\\7.1\\Setup\\VC");
	if (gVcPaths.needsInfo()
		&& (sVcVersion.IsEmpty() || sVcVersion == _T("Net2003"))
		&& QueryRegMachine(reg, dirs71))
	{
		gVcPaths.sVcBaseFolder = reg.ReadString(_T("ProductDir"), _T(""));
		reg.Close();
		if (!gVcPaths.sVcBaseFolder.IsEmpty())
		{
			// Found MSVC .NET 2003, so grab resource compiler & linker
			if (gVcPaths.sRCExe.IsEmpty())
				gVcPaths.sRCExe.Format(_T("%sbin\\rc.exe"), gVcPaths.sVcBaseFolder);
			if (gVcPaths.sLinkExe.IsEmpty())
				gVcPaths.sLinkExe.Format(_T("%sbin\\link.exe"), gVcPaths.sVcBaseFolder);

			// NB: Following is speculative, based on observation of 7.0
			// Now also grab includes & libs
			// The default installation ones are in HKLM
			// The user customized ones are not in the registry, but off in a DAT file under
			// ...\Local Settings\Application Data\Microsoft\VisualStudio\7.1
			// so we just take the default installation ones
			LPCTSTR bd71 = _T("SOFTWARE\\Microsoft\\VisualStudio\\7.1\\VC\\VC_OBJECTS_PLATFORM_INFO\\Win32\\Directories");
			if (QueryRegMachine(reg, bd71))
			{
				if (gVcPaths.sIncludes.IsEmpty())
				{
					gVcPaths.sIncludes = reg.ReadString(_T("Include Dirs"), _T(""));
					gVcPaths.sIncludes.Replace(_T("$(VCInstallDir)"), gVcPaths.sVcBaseFolder);
				}
				if (gVcPaths.sLibs.IsEmpty())
				{
					gVcPaths.sLibs = reg.ReadString(_T("Library Dirs"), _T(""));
					gVcPaths.sLibs.Replace(_T("$(VCInstallDir)"), gVcPaths.sVcBaseFolder);
				}
				reg.Close();
			}
		}
	}

	// check for VisualStudio .NET
	LPCTSTR dirs70 = _T("SOFTWARE\\Microsoft\\VisualStudio\\7.0\\Setup\\VC");
	if (gVcPaths.needsInfo()
		&& (sVcVersion.IsEmpty() || sVcVersion == _T("Net"))
		&& QueryRegMachine(reg, dirs70))
	{
		gVcPaths.sVcBaseFolder = reg.ReadString(_T("ProductDir"), _T(""));
		reg.Close();
		if (!gVcPaths.sVcBaseFolder.IsEmpty())
		{
			// Found MSVC .NET, so grab resource compiler & linker
			if (gVcPaths.sRCExe.IsEmpty())
				gVcPaths.sRCExe.Format(_T("%sbin\\rc.exe"), gVcPaths.sVcBaseFolder);
			if (gVcPaths.sLinkExe.IsEmpty())
				gVcPaths.sLinkExe.Format(_T("%sbin\\link.exe"), gVcPaths.sVcBaseFolder);

			// Now also grab includes & libs
			// The default installation ones are in HKLM
			// The user customized ones are not in the registry, but off in a DAT file under
			// ...\Local Settings\Application Data\Microsoft\VisualStudio\7.0
			// so we just take the default installation ones
			// NB: The 7.1 version below is speculative
			LPCTSTR bd70 = _T("SOFTWARE\\Microsoft\\VisualStudio\\7.0\\VC\\VC_OBJECTS_PLATFORM_INFO\\Win32\\Directories");
			if (QueryRegMachine(reg, bd70))
			{
				CString fmwk = gVcPaths.sVcBaseFolder+_T("FrameworkSDK\\");
				if (gVcPaths.sIncludes.IsEmpty())
				{
					gVcPaths.sIncludes = reg.ReadString(_T("Include Dirs"), _T(""));
					gVcPaths.sIncludes.Replace(_T("$(VCInstallDir)"), gVcPaths.sVcBaseFolder);
					gVcPaths.sIncludes.Replace(_T("$(FrameworkSDKDir)"), fmwk);
				}
				if (gVcPaths.sLibs.IsEmpty())
				{
					gVcPaths.sLibs = reg.ReadString(_T("Library Dirs"), _T(""));
					gVcPaths.sLibs.Replace(_T("$(VCInstallDir)"), gVcPaths.sVcBaseFolder);
					gVcPaths.sLibs.Replace(_T("$(FrameworkSDKDir)"), fmwk);
				}
				reg.Close();
			}
		}
	}

	// check for devstudio 6
	LPCTSTR dirs6 = _T("Software\\Microsoft\\DevStudio\\6.0\\Directories");
	if (gVcPaths.needsInfo()
		&& (sVcVersion.IsEmpty() || sVcVersion == _T("6"))
		&& QueryRegUser(reg, dirs6))
	{
		gVcPaths.sVcBaseFolder = reg.ReadString(_T("Install Dirs"), _T(""));
		reg.Close();
		if (!gVcPaths.sVcBaseFolder.IsEmpty())
		{
			// Found MSVC6, so grab resource compiler & linker
			if (gVcPaths.sRCExe.IsEmpty())
				gVcPaths.sRCExe.Format(_T("%s\\rc.exe"), gVcPaths.sVcBaseFolder);

			CString spath, spath2, spath3, sname;
			SplitFilename(gVcPaths.sVcBaseFolder, &spath, &sname, NULL);
			SplitFilename(spath, &spath2, &sname, NULL);
			SplitFilename(spath2, &spath3, &sname, NULL);
			gVcPaths.sVcBaseFolder = spath3;
			if (gVcPaths.sLinkExe.IsEmpty())
				gVcPaths.sLinkExe.Format(_T("%s\\vc98\\bin\\link.exe"), spath3);

			// Now also grab includes & libs
			LPCTSTR bd = _T("Software\\Microsoft\\DevStudio\\6.0\\Build System\\Components\\Platforms\\Win32 (x86)\\Directories");
			if (QueryRegUser(reg, bd))
			{
				if (gVcPaths.sIncludes.IsEmpty())
					gVcPaths.sIncludes = reg.ReadString(_T("Include Dirs"), _T(""));
				if (gVcPaths.sLibs.IsEmpty())
					gVcPaths.sLibs = reg.ReadString(_T("Library Dirs"), _T(""));
				reg.Close();
			}

		}
	}

	// check for devstudio 5
	LPCTSTR dirs5 = _T("Software\\Microsoft\\DevStudio\\5.0\\Directories");
	if (gVcPaths.needsInfo()
		&& (sVcVersion.IsEmpty() || sVcVersion == _T("5"))
		&& QueryRegUser(reg, dirs5))
	{
		gVcPaths.sVcBaseFolder = reg.ReadString(_T("ProductDir"), _T(""));
		reg.Close();
		if (!gVcPaths.sVcBaseFolder.IsEmpty())
		{
			// Found MSVC5, so grab resource compiler & linker
			if (gVcPaths.sRCExe.IsEmpty())
				gVcPaths.sRCExe.Format(_T("%s\\SharedIDE\\bin\\rc.exe"), gVcPaths.sVcBaseFolder);
			if (gVcPaths.sLinkExe.IsEmpty())
				gVcPaths.sLinkExe.Format(_T("%s\\vc\\bin\\link.exe"), gVcPaths.sVcBaseFolder);

			// Now also grab includes & libs
			LPCTSTR bd = _T("Software\\Microsoft\\DevStudio\\5.0\\Build System\\Components\\Platforms\\Win32 (x86)\\Directories");
			if (QueryRegUser(reg, bd))
			{
				if (gVcPaths.sIncludes.IsEmpty())
					gVcPaths.sIncludes = reg.ReadString(_T("Include Dirs"), _T(""));
				if (gVcPaths.sLibs.IsEmpty())
					gVcPaths.sLibs = reg.ReadString(_T("Library Dirs"), _T(""));
				reg.Close();
			}

		}
	}



	_tprintf(_T("Build paths:\r\n"));
	_tprintf(_T("	%s\r\n"), gVcPaths.sRCExe);
	_tprintf(_T("	%s\r\n"), gVcPaths.sLinkExe);
	_tprintf(_T("  inc: %s\r\n"), gVcPaths.sIncludes);
	_tprintf(_T("  lib: %s\r\n"), gVcPaths.sLibs);
}
