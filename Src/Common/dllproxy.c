/* File:	dllproxy.c
 * Author:	Jochen Tucht 2003/01/11
 *			Copyright (C) 2003 herbert dahm datensysteme GmbH
 *
 * Purpose:	Provide a prototyped proxy object for a DLL to be loaded on demand.
 *			Load the DLL when the proxy object is accessed for the first time.
 *			If the DLL lacks a function which is declared in the proxy object,
 *			issue an error message at load time, but allow program execution to
 *			continue. If the program tries to call a missing function, issue an
 *			error message, tell the user that it may be unsafe to continue
 *			program execution, but still allow program execution to continue.
 *
 * Remarks:	requires Win32
 *			
 * Options:	-DALLOCA_RECOVERY generates additional code to fix the stack pointer
 *			after an attempt to call a missing function. This is the default for
 *			MSC. Specify -DNO_ALLOCA_RECOVERY to explicitly cancel this option.
 *			The option makes use of _alloca(), which is intrinsic in MSC, but
 *			may not be supported by other compilers. Since _alloca() is not
 *			exactly designed for the given purpose, the approach may fail. In
 *			case of doubt, just cancel the option.
 *
 * License:	THIS FILE CONTAINS FREE SOURCE CODE. IT IS PROVIDED *AS IS*, WITHOUT
 *			WARRANTY OF ANY KIND. YOU MAY USE IT AT YOUR OWN RISK, AS LONG AS
 *			YOU KEEP IT IN A SEPARATE FILE AND PRESERVE THIS COMMENT.
 *			CHANGES MUST BE RECORDED IN THE MODIFICATION HISTORY BELOW SO THERE
 *			IS EVIDENCE THAT THE FILE DIFFERS FROM EARLIER RELEASES. THE LEVEL
 *			OF DETAIL IS UP TO YOU. YOU MAY SET THE BY: ENTRY TO "NOBODY@ALL"
 *			IF YOU DON'T WANT TO EXPOSE YOUR NAME. SUBSEQUENT CHANGES MAY BE
 *			REFLECTED BY A SINGLE RECORD CARRYING THE DATE OF THE LATEST CHANGE.
 *

DATE:		BY:					DESCRIPTION:
==========	==================	================================================
2003/03/15	J.Tucht				module itself & DLL proxies may reside in a DLL
2003/03/16	J.Tucht				DLL proxies may reside in stack frame
2003/03/29	J.Tucht				ALLOCA_RECOVERY now thread safe; does no longer
								rely on _alloca(); may also work with GCC
2007/08/11	Kimmo Varis			Add ifdef removing inline assembly from 64-bit
								builds. I know it breaks plugins but its more
								important to be able to build 64-bit now.

*/
// ID line follows -- this is updated by SVN
// $Id$

#if defined(_MSC_VER)&&!defined(NO_ALLOCA_RECOVERY)||defined(ALLOCA_RECOVERY)
#undef ALLOCA_RECOVERY
#define ALLOCA_RECOVERY(X) X
#else
#undef ALLOCA_RECOVERY
#define ALLOCA_RECOVERY(X)
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4100)
#define UNUSED(ARG) ARG
#else
#define UNUSED(ARG)
#endif

//#define _WIN32_IE		0x0300
//#define _WIN32_WINNT	0x0400	

#define NONAMELESSUNION		// avoid warning C4201

struct IShellView;			// avoid MSC warning C4115
struct _RPC_ASYNC_STATE;	// avoid MSC warning C4115

#include <shlobj.h>
#include <shlwapi.h>
#include "dllproxy.h"

#define IS_PTR(p) ((p) != (LPVOID)0)

#define MB_DLLPROXY MB_TASKMODAL | MB_TOPMOST | MB_ICONSTOP

static VOID NTAPI Complain(LPCSTR complain, LPCSTR caption)
{
	if (MessageBoxA(0, complain, caption, MB_DLLPROXY | MB_YESNO) != IDYES)
	{
		ExitProcess(3);
	}
}

#if ALLOCA_RECOVERY(TRUE)-0
#ifdef _MSC_VER
#pragma message("ALLOCA_RECOVERY(TRUE)")
#endif

static DWORD ThreadTopOfStack = 0xFFFFFFFF;

static LPVOID NTAPI ComplainUnresolved()
{
	Complain(DllProxy_ModuleState.Complain.Invoke, "DLLPROXY");
	return TlsGetValue(ThreadTopOfStack);
}

static INT NTAPI Unresolved()
{
	// declare a variable to produce a stack frame
	int Unresolved = 0;

#ifndef _WIN64 // 64-bit compiler errors at inline asm
#ifdef _MSC_VER
	__asm
	{
		call ComplainUnresolved
		mov esp, ebp
		pop ebp
		pop edx
		mov esp, eax
		xor eax, eax
		jmp edx
	}
#endif
#ifdef __MINGW_H
	asm
	(
		"\n	call *%0"				// call ComplainUnresolved
		"\n	mov %%ebp, %%esp"
		"\n	pop %%ebp"
		"\n	pop %%edx"
		"\n	mov %%eax, %%esp"
		"\n	xor %%eax, %%eax"
		"\n	push %%edx"				// as warns about jmp %edx
		"\n	ret"
	:	/* no explicit outputs */
	:	"o" (ComplainUnresolved)
	);
#endif
#endif // _WIN64
	return Unresolved;
}

#else
#ifdef _MSC_VER
#pragma message("ALLOCA_RECOVERY(FALSE)")
#endif

static INT NTAPI Unresolved()
{
	Complain(DllProxy_ModuleState.Complain.Invoke, "DLLPROXY");
	return 0;
}

#endif

static VOID NTAPI Load(HMODULE *phModule)
{
#if ALLOCA_RECOVERY(TRUE)-0
	if (ThreadTopOfStack == 0xFFFFFFFF)
		ThreadTopOfStack = TlsAlloc();
	TlsSetValue(ThreadTopOfStack, &phModule + 1);
#endif
	if (*phModule == 0)
	{
		LPCSTR p = (LPCSTR)(phModule + 1);
		LPCSTR cModule = StrRChrA(p, 0, ';');
		FARPROC *ppfn = (FARPROC *)p;
		HMODULE hModule = LoadLibraryA(cModule + 1);
		CHAR cPath[260];
		GetModuleFileNameA(hModule, cPath, sizeof cPath);
		if (hModule == 0)
		{
			Complain(DllProxy_ModuleState.Complain.LoadLibrary, cModule);
		}
		cModule = PathFindFileNameA(cPath);
		while IS_PTR(p = StrChrA(p,'*'))
		{
			CHAR cExport[260];
			LPCSTR q = StrChrA(p,')');
			FARPROC pfn = GetProcAddress(hModule, lstrcpynA(cExport, p + 1, q - p));
			p = StrChrA(q,';') + 1;
			if (pfn == 0)
			{
				pfn = Unresolved;
				if (hModule)
				{
					CHAR cMsg[2600];
					wsprintfA(cMsg, "%s:\n%s%s", cPath, cExport,
						DllProxy_ModuleState.Complain.GetProcAddress);
					Complain(cMsg, cModule);
				}
			}
			*ppfn++ = pfn;
		}
		*phModule = GetModuleHandleA(cPath);
	}
}

struct DllProxy_ModuleState DllProxy_ModuleState =
{
	{
		// Complain.LoadLibrary
		"Failed to load library!\n"
		"Continue anyway?",
		// Complain.GetProcAddress
		"() unresolved!\n"
		"Continue anyway?",
		// Complain.Invoke
		"Attempt to call unresolved external!\n"
		"It may be unsafe to continue!\n"
		"Continue anyway?"
	},
	// Load()
	Load,
	// Unresolved()
	Unresolved,
};
