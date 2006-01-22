// diff2winmerge.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "diff2winmerge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Local functions

static int ProcessAndCall(int argc, TCHAR* argv[]);
static HANDLE RunIt(LPCTSTR szProgram, LPCTSTR szArgs, BOOL bMinimized=TRUE, BOOL bNewConsole=FALSE);

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{

	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		return -1;
	}
	try {

		return ProcessAndCall(argc, argv);

	} catch(CException * pExc) {
		pExc->ReportError();
		pExc->Delete();
	}
	return -1;
}

static LPCTSTR argmap[] = {
	_T("i"), _T("--ignore-case"), _T("/ignorecase:1")
	// -E  --ignore-tab-expansion unavailable in WinMerge
	, _T("b"), _T("--ignore-space-change"), _T("/ignorews:1")
	, _T("w"), _T("--ignore-all-space"), _T("/ignorews:2")
	, _T("B"), _T("--ignore-blank-lines"), _T("/ignoreblanklines:1")
};

static bool
isShortSwitch(const CString & arg)
{
	return (arg.GetLength()>=1 && arg[0] == '-');
}

static bool
isLongSwitch(const CString & arg)
{
	return (arg.GetLength()>=2 && arg[0] == '-' && arg[1] == '-');
}

static void
AppendArg(CString &argline, LPCTSTR newarg)
{
	if (!argline.IsEmpty())
		argline += _T(" ");
	argline += newarg;
}

static int
ProcessAndCall(int argc, TCHAR* argv[])
{
	if (argc<2) return -1;
	LPCTSTR target = argv[1];

	CString argline;

	for (int i=2; i<argc; ++i)
	{
		CString arg = argv[i];
		if (isShortSwitch(arg))
		{
			for (int x=1; x<arg.GetLength(); ++x)
			{
				for (int j=0; j<sizeof(argmap)/sizeof(argmap[0]); j += 3)
				{
					LPCTSTR shortSwitch = argmap[j];
					LPCTSTR winmergeSwitch = argmap[j+2];
					if (arg[x] == shortSwitch[0])
					{
						AppendArg(argline, winmergeSwitch);
						break;
					}
				}
			}
		}
		else if (isLongSwitch(arg))
		{
			for (int j=0; j<sizeof(argmap)/sizeof(argmap[0]); j += 3)
			{
				LPCTSTR longSwitch = argmap[j+1];
				LPCTSTR winmergeSwitch = argmap[j+2];
				if (arg == longSwitch)
				{
					AppendArg(argline, winmergeSwitch);
					break;
				}
			}
		}
		else
		{
			AppendArg(argline, arg);
		}
	}

	CString cmdline = target;
	if (!argline.IsEmpty())
	{
		cmdline += _T(" ");
		cmdline += argline;
	}
	HANDLE hprocess = RunIt(target, argline, FALSE);

	DWORD dwExitCode = -1;
	if (hprocess != INVALID_HANDLE_VALUE)
	{
		// Wait for WinMerge to complete
		WaitForSingleObject(hprocess, INFINITE );
		GetExitCodeProcess(hprocess, &dwExitCode );
	}
	return dwExitCode;
}

static TCHAR buffer[4096];

/**
 * @brief Run specified commandline as new process
 */
static HANDLE
RunIt(LPCTSTR szProgram, LPCTSTR szArgs, BOOL bMinimized /*= TRUE*/, BOOL bNewConsole /*= FALSE*/)
{
	STARTUPINFO si;
	PROCESS_INFORMATION procInfo;

	si.cb = sizeof(STARTUPINFO);
	si.lpReserved=NULL;
	si.lpDesktop = _T("");
	si.lpTitle = NULL;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = (WORD)(SW_SHOW);
	si.cbReserved2 = 0;
	si.lpReserved2 = NULL;

	CString sArgs = szProgram;
	sArgs += _T(" ");
	sArgs += szArgs;
	LPTSTR szArgsBuff = sArgs.GetBuffer(0);

	_tcscpy(buffer, szProgram);
	if (szArgs)
	{
		_tcscat(buffer, _T(" "));
		_tcscat(buffer, szArgs);
	}



	if (!CreateProcess(NULL, buffer, NULL, NULL,
		FALSE, NORMAL_PRIORITY_CLASS|(bNewConsole? CREATE_NEW_CONSOLE:0),
		NULL, _T(".\\"), &si, &procInfo))
	{
		return INVALID_HANDLE_VALUE;
	}

	CloseHandle(procInfo.hThread);
	return procInfo.hProcess;
}

