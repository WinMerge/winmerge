/** 
 * @file  MakeResDll.cpp
 *
 * @brief Code to compile & link a language satellite resource DLL, using Visual Studio
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "MakeResDll.h"
// Following files included from WinMerge/Src/Common
#include "UnicodeString.h"
#include "RegKey.h"
// Local files

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

static LPCTSTR VSRegVersionStrings[] =
{	// These must be laid out in the same order as VS_VERSION:
	// VS6, VS2002, ...
	_T("6"),
	_T("Net"),
	_T("Net2003"),
	_T("Net2005")
};

struct VcPaths
{
	CString sRCExe;
	CString sLinkExe;
	CString sVcBaseFolder;
	CString sIncludes;
	CString sLibs;
	CString sAdditionalPath;
	bool needsInfo() const { return sRCExe.IsEmpty() || sLinkExe.IsEmpty() || sIncludes.IsEmpty() || sLibs.IsEmpty(); }
};

// File-level globals
static VcPaths gVcPaths;
static CWinApp theApp;
static CString gsLang;
static CString gsVcBin;
static CString gsRCScript;
static CString gsOutPath;
static BOOL gbPause=FALSE;
BOOL static gbBatch=FALSE;
static BOOL gbSilent=FALSE;
static BOOL gbVerbose=FALSE;
static bool gbUi=false;
static LPCTSTR gVs80VcBaseDir = _T("SOFTWARE\\Microsoft\\VisualStudio\\8.0\\Setup\\VC");
static LPCTSTR gVs71VcBaseDir = _T("SOFTWARE\\Microsoft\\VisualStudio\\7.1\\Setup\\VC");
static LPCTSTR gVs70VcBaseDir = _T("SOFTWARE\\Microsoft\\VisualStudio\\7.0\\Setup\\VC");
static LPCTSTR gVs6VcBaseDir = _T("SOFTWARE\\Microsoft\\DevStudio\\6.0\\Products\\Microsoft Visual C++");
static LPCTSTR gVs5VcBaseDir = _T("Software\\Microsoft\\DevStudio\\5.0\\Directories");

// Static functions
static VS_VERSION MapRegistryValue(LPCTSTR value);
static BOOL MkDirEx(LPCTSTR filename);
static BOOL MyCreateDirectoryIfNeeded(LPCTSTR lpPathName);
static HANDLE RunIt(LPCTSTR szExeFile, LPCTSTR szArgs, BOOL bMinimized /*= TRUE*/, BOOL bNewConsole /*= FALSE*/);
static BOOL BuildDll(LPCTSTR pszRCPath, LPCTSTR pszOutputPath, LPCTSTR pszOutputStem, CString& strOutFile);
static BOOL CheckCompiler();
static void Status(LPCTSTR szText);
static void Status(UINT idstrText, LPCTSTR szText1 = NULL, LPCTSTR szText2 = NULL);
static void InitModulePaths(const CStringArray & VsBaseDirs);
static bool FindAndLoadVsVersion(const CStringArray & VsBaseDirs, VS_VERSION vsnum);
static void LoadVs2005Settings(CString & sProductDir);
static void LoadVs2003Settings(CString & sProductDir);
static void LoadVs2002Settings(CString & sProductDir);
static void LoadVs6Settings(CString & sProductDir);
static void Usage();
static BOOL ProcessArgs(int argc, TCHAR* argv[]);
static void FixPath();
static bool DoesFileExist(LPCTSTR filepath);
static void TrimPath(CString & sPath);
//static void DisplayUi(const CStringArray & VsBaseDirs);
static void LoadVsBaseDirs(CStringArray & VsBaseDirs);
static void SplitFilename(LPCTSTR path, TCHAR * folder, TCHAR * filename, TCHAR * ext);

using namespace std;

/**
 * @brief Map registry value to enum value, eg, "Net2003" => VS2003
 */
static VS_VERSION MapRegistryValue(LPCTSTR value)
{
	for (int i = 0; i < VS_COUNT; ++i)
	{
		if (_tcscmp(value, VSRegVersionStrings[i]) == 0)
		{
			return (VS_VERSION)i;
		}
	}
	return VS_NONE;
}

/**
 * @brief Main entry point of (console mode) application
 */
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

		CStringArray VsBaseDirs;
		LoadVsBaseDirs(VsBaseDirs);

		InitModulePaths(VsBaseDirs);

		if (!ProcessArgs(argc, argv))
		{
			Usage();
			return 1;
		}

		if (gbUi)
		{
			// TODO: Add console GUI - after all this is console app!
			
			//DisplayUi(VsBaseDirs);
            return 0;
		}

		TCHAR spath[MAX_PATH] = {0};
		TCHAR sname[MAX_PATH] = {0};
		SplitFilename(gsRCScript, spath, sname, NULL);

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

/**
 * @brief Add paths to RC compiler & linker to the PATH environment variable
 */
static void FixPath()
{
	CString strPath(getenv(_T("PATH")));
	TCHAR spath[MAX_PATH] = {0};
	if (gbVerbose)
		_tprintf(_T("Initial path: %s\r\n"), strPath);

	strPath = _T("PATH=") + strPath;
	SplitFilename(gVcPaths.sRCExe, spath, NULL, NULL);
	strPath += _T(";");
	strPath += spath;
	SplitFilename(gVcPaths.sLinkExe, spath, NULL, NULL);
	strPath += _T(";");
	strPath += spath;
	strPath += _T(";");

	if (!gVcPaths.sAdditionalPath.IsEmpty())
	{
		strPath += gVcPaths.sAdditionalPath + _T(";");
	}
	putenv(strPath);
	if (gbVerbose)
		_tprintf(_T("New path: %s\r\n"), strPath);
}

/**
 * @brief Display status message saying this arg requires another arg and return FALSE
 */
static BOOL MissingArg(LPCTSTR arg)
{
	Status(IDS_ERROR_MISSING_SWITCH_ARG, arg);
	return FALSE;
}

/**
 * @brief Parse the arguments and set switches accordingly
 */
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
		else if (!_tcsicmp(argv[i], _T("-ui"))
			|| !_tcsicmp(argv[i], _T("/ui")))
		{
			gbUi = true;
			return TRUE; // ignore other arguments
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
	CString str = LoadResString(nId);
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
		WaitForSingleObject(hRC, INFINITE);
		GetExitCodeProcess(hRC, &dwReturn);

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
		WaitForSingleObject(hLink, INFINITE);
		GetExitCodeProcess(hLink, &dwReturn);

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
			s = LoadResString(idstrText);
		Status(s);
	}
	CATCH_ALL (e)
	{
	}
	END_CATCH_ALL;
}


// Find locations of RC compiler and linker
static void InitModulePaths(const CStringArray & VsBaseDirs)
{
	// Access to registry
	CRegKeyEx reg;

	// Strategy is that we keep looking as long as we need anything (in gVcPaths)

	String sVcVersion;

	// Check for user-configured overrides
	LPCTSTR settings = _T("Software\\Thingamahoochie\\MakeResDll\\Settings");
	if (RegOpenUser(reg, settings))
	{
		gVcPaths.sVcBaseFolder = reg.ReadString(_T("VcBaseFolder"), _T("")).c_str();
		if (gVcPaths.sRCExe.IsEmpty())
			gVcPaths.sRCExe = reg.ReadString(_T("RCExe"), _T("")).c_str();
		if (gVcPaths.sLinkExe.IsEmpty())
			gVcPaths.sLinkExe = reg.ReadString(_T("LinkExe"), _T("")).c_str();
		// This is the main way for the user to override these settings
		// VcVersion values handled:
		// Net2005 - Use Microsoft Visual Studio .NET 2005
		// Net2003 - Use Microsoft Visual Studio .NET 2003
		// Net     - Use Microsoft Visual Studio .NET (2002)
		// 6       - Use Microsoft Visual Studio 6
		// 5       - Use Microsoft Visual Studio 5
		sVcVersion = reg.ReadString(_T("VcVersion"), _T(""));
		reg.Close();
	}

	VS_VERSION vsnum = VS_NONE;
	// Check if user's choice is valid and installed
	if (!sVcVersion.empty())
	{
		VS_VERSION vstemp = MapRegistryValue(sVcVersion.c_str());
		if (vstemp != VS_NONE)
		{
			if (!VsBaseDirs[vstemp].IsEmpty())
				vsnum = vstemp;
		}
	}

	FindAndLoadVsVersion(VsBaseDirs, vsnum);

	_tprintf(_T("Build paths:\r\n"));
	_tprintf(_T("	%s\r\n"), gVcPaths.sRCExe);
	_tprintf(_T("	%s\r\n"), gVcPaths.sLinkExe);
	_tprintf(_T("  inc: %s\r\n"), gVcPaths.sIncludes);
	_tprintf(_T("  lib: %s\r\n"), gVcPaths.sLibs);
}


/**
 * @brief Look for Visual Studio settings in registry
 * sVcVersion, if specified, is user's preference, so only try it
 * If it is blank, try all
 * Return 1 if success
 * Return 0 if not found
 * Return -1 if specified version not installed (so caller will retry)
 */
static bool FindAndLoadVsVersion(const CStringArray & VsBaseDirs, VS_VERSION vsnum)
{
	// Access to registry
	CRegKeyEx reg;

	// Check for versions in descending order
	for (int vi=VS_COUNT-1; vi>VS_NONE; --vi)
	{
		if (vsnum == VS_NONE || vsnum == vi)
		{
			CString sProductDir = VsBaseDirs[vi];
			if (!sProductDir.IsEmpty())
			{
				if (vi == VS_2005)
					LoadVs2005Settings(sProductDir);
				else if (vi == VS_2003)
					LoadVs2003Settings(sProductDir);
				else if (vi == VS_2002)
					LoadVs2002Settings(sProductDir);
				else if (vi == VS_6)
					LoadVs6Settings(sProductDir);
				return true;
			}
		}
	}
	return false;
}


/**
 * @brief Load VS.NET 2005 settings into global gVcPaths
 * Version 8.0
 */
static void LoadVs2005Settings(CString & sProductDir)
{
	// Access to registry
	CRegKeyEx reg;

	// Root directory of Visual C
	// eg, C:\Program Files\Microsoft Visual Studio 8\VC\ 
	if (gVcPaths.sVcBaseFolder.IsEmpty())
		gVcPaths.sVcBaseFolder = sProductDir;
	
	// Get root directory of Visual Studio
	LPCTSTR VsBaseKey = _T("SOFTWARE\\Microsoft\\VisualStudio\\8.0\\Setup\\VS");
	String sVsRoot;
	if (RegOpenMachine(reg, VsBaseKey))
	{
		// eg, C:\Program Files\Microsoft Visual Studio 8\ 
		sVsRoot = reg.ReadString(_T("ProductDir"), _T(""));
		reg.Close();
	}

	// Found MSVC .NET 2005, so grab resource compiler & linker
	if (gVcPaths.sRCExe.IsEmpty())
		gVcPaths.sRCExe.Format(_T("%sbin\\rc.exe"), gVcPaths.sVcBaseFolder);
	if (gVcPaths.sLinkExe.IsEmpty())
		gVcPaths.sLinkExe.Format(_T("%sbin\\link.exe"), gVcPaths.sVcBaseFolder);

	if (RegOpenMachine(reg, _T("SOFTWARE\\Microsoft\\VisualStudio\\8.0")))
	{
		// eg, C:\Program Files\Microsoft Visual Studio 8\Common7\IDE\ 
		gVcPaths.sAdditionalPath = reg.ReadString(_T("InstallDir"), _T("")).c_str();
		TrimPath(gVcPaths.sAdditionalPath);
	}

	// Now also grab includes & libs
	// The default installation ones are in HKLM
	// so we just take the default installation ones
	LPCTSTR bd71 = _T("SOFTWARE\\Microsoft\\VisualStudio\\7.1\\VC\\VC_OBJECTS_PLATFORM_INFO\\Win32\\Directories");
	if (RegOpenMachine(reg, bd71))
	{
		if (gVcPaths.sIncludes.IsEmpty())
		{
			gVcPaths.sIncludes = reg.ReadString(_T("Include Dirs"), _T("")).c_str();
			gVcPaths.sIncludes.Replace(_T("$(VCInstallDir)"), gVcPaths.sVcBaseFolder);
		}
		if (gVcPaths.sLibs.IsEmpty())
		{
			gVcPaths.sLibs = reg.ReadString(_T("Library Dirs"), _T("")).c_str();
			gVcPaths.sLibs.Replace(_T("$(VCInstallDir)"), gVcPaths.sVcBaseFolder);
			if (!sVsRoot.empty())
			{
				// eg C:\Program Files\Microsoft Visual Studio 8\SDK\v1.1"));
				String sFrameworkSdkDir = sVsRoot + _T("SDK\\v1.1");
				gVcPaths.sLibs.Replace(_T("$(FrameworkSDKDir)"), sFrameworkSdkDir.c_str());
			}
		}
		reg.Close();
	}
}

/**
 * @brief Load Visual Studio .NET 2003 settings into global gVcPaths
 * Version 7.1
 */
static void LoadVs2003Settings(CString & sProductDir)
{
	// Access to registry
	CRegKeyEx reg;

	// Root directory of Visual C
	// eg, C:\Program Files\Microsoft Visual Studio 8\VC\ 
	if (gVcPaths.sVcBaseFolder.IsEmpty())
		gVcPaths.sVcBaseFolder = sProductDir;
	
	// Get root directory of Visual Studio
	LPCTSTR VsBaseKey = _T("SOFTWARE\\Microsoft\\VisualStudio\\7.1\\Setup\\VS");
	String sVsRoot;
	if (RegOpenMachine(reg, VsBaseKey))
	{
		// eg, C:\Program Files\Microsoft Visual Studio .NET 2003\ 
		sVsRoot = reg.ReadString(_T("ProductDir"), _T(""));
		reg.Close();
	}

	// Found MSVC .NET 2003, so grab resource compiler & linker
	if (gVcPaths.sRCExe.IsEmpty())
		gVcPaths.sRCExe.Format(_T("%sbin\\rc.exe"), gVcPaths.sVcBaseFolder);
	if (gVcPaths.sLinkExe.IsEmpty())
		gVcPaths.sLinkExe.Format(_T("%sbin\\link.exe"), gVcPaths.sVcBaseFolder);

	if (RegOpenMachine(reg, _T("SOFTWARE\\Microsoft\\VisualStudio\\7.1")))
	{
		// eg, C:\Program Files\Microsoft Visual Studio .NET 2003\Common7\IDE\ 
		gVcPaths.sAdditionalPath = reg.ReadString(_T("InstallDir"), _T("")).c_str();
		TrimPath(gVcPaths.sAdditionalPath);
	}

	// Now also grab includes & libs
	// The default installation ones are in HKLM
	// The user customized ones are not in the registry, but off in a DAT file under
	// ...\Local Settings\Application Data\Microsoft\VisualStudio\7.1
	// so we just take the default installation ones
	LPCTSTR bd71 = _T("SOFTWARE\\Microsoft\\VisualStudio\\7.1\\VC\\VC_OBJECTS_PLATFORM_INFO\\Win32\\Directories");
	if (RegOpenMachine(reg, bd71))
	{
		if (gVcPaths.sIncludes.IsEmpty())
		{
			gVcPaths.sIncludes = reg.ReadString(_T("Include Dirs"), _T("")).c_str();
			gVcPaths.sIncludes.Replace(_T("$(VCInstallDir)"), gVcPaths.sVcBaseFolder);
		}
		if (gVcPaths.sLibs.IsEmpty())
		{
			gVcPaths.sLibs = reg.ReadString(_T("Library Dirs"), _T("")).c_str();
			gVcPaths.sLibs.Replace(_T("$(VCInstallDir)"), gVcPaths.sVcBaseFolder);
			if (!sVsRoot.empty())
			{
				// eg C:\Program Files\Microsoft Visual Studio .NET 2003\SDK\v1.1"));
				String sFrameworkSdkDir = sVsRoot + _T("SDK\\v1.1");
				gVcPaths.sLibs.Replace(_T("$(FrameworkSDKDir)"), sFrameworkSdkDir.c_str());
			}
		}
		reg.Close();
	}
}

/**
 * @brief Load Visual Studio .NET (2002) settings into global gVcPaths
 * Version 7.0
 */
static void LoadVs2002Settings(CString & sProductDir)
{
	// Access to registry
	CRegKeyEx reg;

	// Get root directory of Visual C
	if (gVcPaths.sVcBaseFolder.IsEmpty())
		gVcPaths.sVcBaseFolder = sProductDir;
	
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
	LPCTSTR bd70 = _T("SOFTWARE\\Microsoft\\VisualStudio\\7.0\\VC\\VC_OBJECTS_PLATFORM_INFO\\Win32\\Directories");
	if (RegOpenMachine(reg, bd70))
	{
		String fmwk = gVcPaths.sVcBaseFolder+_T("FrameworkSDK\\");
		if (gVcPaths.sIncludes.IsEmpty())
		{
			gVcPaths.sIncludes = reg.ReadString(_T("Include Dirs"), _T("")).c_str();
			gVcPaths.sIncludes.Replace(_T("$(VCInstallDir)"), gVcPaths.sVcBaseFolder);
			gVcPaths.sIncludes.Replace(_T("$(FrameworkSDKDir)"), fmwk.c_str());
		}
		if (gVcPaths.sLibs.IsEmpty())
		{
			gVcPaths.sLibs = reg.ReadString(_T("Library Dirs"), _T("")).c_str();
			gVcPaths.sLibs.Replace(_T("$(VCInstallDir)"), gVcPaths.sVcBaseFolder);
			gVcPaths.sLibs.Replace(_T("$(FrameworkSDKDir)"), fmwk.c_str());
		}
		reg.Close();
	}
}

/**
 * @brief Load Visual Studio 6 settings into global gVcPaths
 * Version 6.0
 */
static void LoadVs6Settings(CString & sProductDir)
{
	// Access to registry
	CRegKeyEx reg;

	// Get root directory of Visual C
	// eg, C:\Program Files\Microsoft Visual Studio\VC98
	if (gVcPaths.sVcBaseFolder.IsEmpty())
		gVcPaths.sVcBaseFolder = sProductDir;

	LPCTSTR Dev6Dirs = _T("Software\\Microsoft\\DevStudio\\6.0\\Directories");
	if (RegOpenUser(reg, Dev6Dirs))
	{
		// eg, C:\Program Files\Microsoft Visual Studio\COMMON\MSDev98\Bin
		String sVsDevBin = reg.ReadString(_T("Install Dirs"), _T(""));
		if (gVcPaths.sRCExe.IsEmpty())
			gVcPaths.sRCExe.Format(_T("%s\\rc.exe"), sVsDevBin.c_str());
	}

	// Get linker
	// eg, C:\Program Files\Microsoft Visual Studio\VC98\bin\link.exe
	if (gVcPaths.sLinkExe.IsEmpty())
		gVcPaths.sLinkExe.Format(_T("%s\\bin\\link.exe"), gVcPaths.sVcBaseFolder);

	// Now also grab includes & libs
	LPCTSTR bd = _T("Software\\Microsoft\\DevStudio\\6.0\\Build System\\Components\\Platforms\\Win32 (x86)\\Directories");
	if (RegOpenUser(reg, bd))
	{
		if (gVcPaths.sIncludes.IsEmpty())
			gVcPaths.sIncludes = reg.ReadString(_T("Include Dirs"), _T("")).c_str();
		if (gVcPaths.sLibs.IsEmpty())
			gVcPaths.sLibs = reg.ReadString(_T("Library Dirs"), _T("")).c_str();
		reg.Close();
	}
}

/**
 * @brief Return true if character is a directory separator slash
 */
static bool IsSlash(TCHAR ch)
{
	return ch == '\\' || ch == '/';
}

/**
 * @brief Remove any trailing slashes
 */
static void TrimPath(CString & sPath)
{
	if (sPath.GetLength() && IsSlash(sPath[sPath.GetLength()-1]))
		sPath = sPath.Left(sPath.GetLength()-1);
}

/**
 * @brief Load string array of base product directories for all installed Visual Studio versions
 */
static void LoadVsBaseDirs(CStringArray & VsBaseDirs)
{
	VsBaseDirs.RemoveAll();
	VsBaseDirs.SetSize(VS_COUNT);

	// Access to registry
	CRegKeyEx reg;

	if (RegOpenMachine(reg, gVs80VcBaseDir))
	{
		VsBaseDirs[VS_2005] = reg.ReadString(_T("ProductDir"), _T("")).c_str();
		reg.Close();
	}
	if (RegOpenMachine(reg, gVs71VcBaseDir))
	{
		VsBaseDirs[VS_2003] = reg.ReadString(_T("ProductDir"), _T("")).c_str();
		reg.Close();
	}
	if (RegOpenMachine(reg, gVs70VcBaseDir))
	{
		VsBaseDirs[VS_2002] = reg.ReadString(_T("ProductDir"), _T("")).c_str();
		reg.Close();
	}
	if (RegOpenMachine(reg, gVs6VcBaseDir))
	{
		VsBaseDirs[VS_6] = reg.ReadString(_T("ProductDir"), _T("")).c_str();
		reg.Close();
	}
}

static BOOL MkDirEx(LPCTSTR filename)
{
	TCHAR tempPath[_MAX_PATH] = {0};
	LPTSTR p;

	_tcscpy(tempPath, filename);
	if (*_tcsinc(filename)==_T(':'))
		p=_tcschr(_tcsninc(tempPath,3),_T('\\'));
	else if (*filename==_T('\\'))
		p=_tcschr(_tcsinc(tempPath),_T('\\'));
	else
		p=tempPath;
	if (p!=NULL)
		for (; *p != _T('\0'); p = _tcsinc(p))
		{
			if (*p == _T('\\'))
			{
				_tccpy(p, _T("\0"));
				if (0 && _tcscmp(tempPath, _T(".")) == 0)
				{
					// Don't call CreateDirectory(".")
				}
				else
				{
					if (!MyCreateDirectoryIfNeeded(tempPath)
						&& !MyCreateDirectoryIfNeeded(tempPath))
						TRACE(_T("Failed to create folder %s\n"), tempPath);
					_tccpy(p, _T("\\"));
				}
			}

		}

		if (!MyCreateDirectoryIfNeeded(filename)
			&& !MyCreateDirectoryIfNeeded(filename))
			TRACE(_T("Failed to create folder %s\n"), filename);

	CFileStatus status;
	return (CFile::GetStatus(filename, status));
}

// Create directory (via Win32 API)
// if success, or already exists, return TRUE
// if failure, return FALSE
// (NB: Win32 CreateDirectory reports failure if already exists)
static BOOL MyCreateDirectoryIfNeeded(LPCTSTR lpPathName)
{
	LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL;
	int rtn = CreateDirectory(lpPathName, lpSecurityAttributes);
	if (!rtn)
	{
		int errnum = GetLastError();
		// Consider it success if directory already exists
		if (errnum == ERROR_ALREADY_EXISTS)
			return TRUE;
	}
	return rtn;
}

static HANDLE RunIt(LPCTSTR szExeFile, LPCTSTR szArgs, BOOL bMinimized /*= TRUE*/, BOOL bNewConsole /*= FALSE*/)
{
    STARTUPINFO si;
	PROCESS_INFORMATION procInfo;

    si.cb = sizeof(STARTUPINFO);
    si.lpReserved=NULL;
    si.lpDesktop = _T("");
    si.lpTitle = NULL;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = (WORD)(SW_HIDE);
    si.cbReserved2 = 0;
    si.lpReserved2 = NULL;

	TCHAR args[4096];
	_stprintf(args,_T("\"%s\" %s"), szExeFile, szArgs);
    if (CreateProcess(szExeFile, args, NULL, NULL,
		FALSE, NORMAL_PRIORITY_CLASS|(bNewConsole? CREATE_NEW_CONSOLE:0),
                         NULL, _T(".\\"), &si, &procInfo))
	{
		CloseHandle(procInfo.hThread);
		return procInfo.hProcess;
	}

	return INVALID_HANDLE_VALUE;
}

static void SplitFilename(LPCTSTR path, TCHAR * folder, TCHAR * filename, TCHAR * ext)
{
	TCHAR spath[MAX_PATH] = {0};
	TCHAR sname[MAX_PATH] = {0};
	TCHAR sdrive[_MAX_DRIVE] = {0};
	TCHAR sdir[_MAX_PATH] = {0};
	TCHAR sext[MAX_PATH] = {0};

	_tsplitpath(path, sdrive, sdir, sname, sext);
	_tcscat(spath, sdrive);
	_tcscat(spath, sdir);

	if (folder != NULL)
		_tcscpy(folder, spath);
	if (filename != NULL)
		_tcscpy(filename, sname);
	if (ext != NULL)
		_tcscpy(ext, sext);
}
