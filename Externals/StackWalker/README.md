# StackWalker - Walking the callstack

This article describes the (documented) way to walk a callstack for any thread (own, other and remote). It has an abstraction layer, so the calling app does not need to know the internals.

This project was initially published on Codeproject (http://www.codeproject.com/KB/threads/StackWalker.aspx).
But it is hard to maintain the article and the source on codeproject,
so I was pushed to publish the source code on an "easier to modify" platform. Therefor I have chosen "codeplex" ;(

But time goes by, and codeplex went away ;)

So I now migrated to GitHub ;)

# Latest Build status

[![build result](https://ci.appveyor.com/api/projects/status/github/JochenKalmbach/stackwalker?branch=master&svg=true)](https://ci.appveyor.com/project/JochenKalmbach/stackwalker)

# Documentation

## Introduction

In some cases you need to display the callstack of the current thread or you are just interested in the callstack of other threads / processes. Therefore I wrote this project.

The goal for this project was the following:

* Simple interface to generate a callstack
* C++ based to allow overwrites of several methods
* Hiding the implementation details (API) from the class interface
* Support of x86, x64 and IA64 architecture
* Default output to debugger-output window (but can be customized)
* Support of user-provided read-memory-function
* Support of the widest range of development-IDEs (VC5-VC8)
* Most portable solution to walk the callstack

## Background

To walk the callstack there is a documented interface: [StackWalk64](http://msdn.microsoft.com/library/en-us/debug/base/stackwalk64.asp)
Starting with Win9x/W2K, this interface is in the *dbghelp.dll* library (on NT, it is in *imagehlp.dll*).
But the function name (`StackWalk64`) has changed starting with W2K (before it was called `StackWalk` (without the `64`))!
This project only supports the newer Xxx64-functions. If you need to use it on older systems, you can download the [redistributable for NT/W9x](http://www.microsoft.com/downloads/release.asp?releaseid=30682).

The latest *dbghelp.dll* can always be downloaded with the [Debugging Tools for Windows](http://www.microsoft.com/whdc/devtools/debugging/).
This also contains the *symsrv.dll* which enables the use of the public Microsoft symbols-server (can be used to retrieve debugging information for system-files; see below).

## Build

```
mkdir build-dir
cd build-dir

# batch
cmake -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=%cd%/root ..
# powershell
cmake -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX="$($(get-location).Path)/root" ..

cmake --build . --config RelWithDebInfo
ctest.exe -V -C RelWithDebInfo
cmake --build . --target install --config RelWithDebInfo
```

## Using the code

The usage of the class is very simple. For example if you want to display the callstack of the current thread, just instantiate a `StackWalk` object and call the `ShowCallstack` member:

```c++
#include <windows.h>
#include "StackWalker.h"

void Func5() { StackWalker sw; sw.ShowCallstack(); }
void Func4() { Func5(); }
void Func3() { Func4(); }
void Func2() { Func3(); }
void Func1() { Func2(); }

int main()
{
    Func1();
    return 0;
}
```

This produces the following output in the debugger-output window:

    [...] (output stripped)
    d:\privat\Articles\stackwalker\stackwalker.cpp (736): StackWalker::ShowCallstack
    d:\privat\Articles\stackwalker\main.cpp (4): Func5
    d:\privat\Articles\stackwalker\main.cpp (5): Func4
    d:\privat\Articles\stackwalker\main.cpp (6): Func3
    d:\privat\Articles\stackwalker\main.cpp (7): Func2
    d:\privat\Articles\stackwalker\main.cpp (8): Func1
    d:\privat\Articles\stackwalker\main.cpp (13): main
    f:\vs70builds\3077\vc\crtbld\crt\src\crt0.c (259): mainCRTStartup
    77E614C7 (kernel32): (filename not available): _BaseProcessStart@4

You can now double-click on a line and the IDE will automatically jump to the desired file/line.

### Providing own output-mechanism

If you want to direct the output to a file or want to use some other output-mechanism, you simply need to derive from the `StackWalker` class.
You have two options to do this: only overwrite the `OnOutput` method or overwrite each `OnXxx`-function.
The first solution (`OnOutput`) is very easy and uses the default-implementation of the other `OnXxx`-functions (which should be enough for most of the cases). To output also to the console, you need to do the following:

```c++
class MyStackWalker : public StackWalker
{
public:
    MyStackWalker() : StackWalker() {}
protected:
    virtual void OnOutput(LPCSTR szText) {
        printf(szText); StackWalker::OnOutput(szText);
    }
};
```

### Retrieving detailed callstack info

If you want detailed info about the callstack (like loaded-modules, addresses, errors, ...) you can overwrite the corresponding methods. The following methods are provided:

```c++
class StackWalker
{
protected:
    virtual void OnSymInit(LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName);
    virtual void OnLoadModule(LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size,
                              DWORD result, LPCSTR symType, LPCSTR pdbName,
                              ULONGLONG fileVersion);
    virtual void OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry);
    virtual void OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr);
};
```

These methods are called during the generation of the callstack.

### Various kinds of callstacks

In the constructor of the class, you need to specify if you want to generate callstacks for the current process or for another process. The following constructors are available:

```c++
{
public:
    StackWalker(int options = OptionsAll,
                LPCSTR szSymPath = NULL,
                DWORD dwProcessId = GetCurrentProcessId(),
                HANDLE hProcess = GetCurrentProcess());
    
    // Just for other processes with default-values for options and symPath
    StackWalker(DWORD dwProcessId, HANDLE hProcess);
    
    // For showing stack trace after __except or catch
    StackWalker(ExceptType extype, int options = OptionsAll, PEXCEPTION_POINTERS exp = NULL);
};
```

To do the actual stack-walking you need to call the following functions:

```c++
class StackWalker
{
public:
    BOOL ShowCallstack(HANDLE hThread = GetCurrentThread(), CONTEXT *context = NULL,
                       PReadProcessMemoryRoutine readMemoryFunction = NULL, LPVOID pUserData = NULL);
};
```

### Displaying the callstack of an exception

With this `StackWalker` you can also display the callstack inside an structured exception handler. You only need to write a filter-function which does the stack-walking:

```c++
// The exception filter function:
LONG WINAPI ExpFilter(EXCEPTION_POINTERS* pExp, DWORD dwExpCode)
{
    StackWalker sw;
    sw.ShowCallstack(GetCurrentThread(), pExp->ContextRecord);
    return EXCEPTION_EXECUTE_HANDLER;
}

// This is how to catch an exception:
__try
{
    // do some ugly stuff...
}
__except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))
{
}
```

Display the callstack inside an C++ exception handler (two ways):
```c++
// This is how to catch an exception:
try
{
    // do some ugly stuff...
}
catch (std::exception & ex)
{
    StackWalker sw;
    sw.ShowCallstack(GetCurrentThread(), sw.GetCurrentExceptionContext());
}
catch (...)
{
    StackWalker sw(StackWalker::AfterCatch);
    sw.ShowCallstack();
}
```

## Points of Interest

### Context and callstack

To walk the callstack of a given thread, you need at least two facts:

#### The context of the thread

The context is used to retrieve the current *Instruction Pointer* and the values for the *Stack Pointer (SP)* and sometimes the *Frame Pointer (FP)*.
The difference between SP and FP is in short: SP points to the latest address on the stack. FP is used to reference the arguments for a function. See also [Difference Between Stack Pointer and Frame Pointer](http://www.embeddedrelated.com/usenet/embedded/show/31646-1.php).
But only the SP is essential for the processor. The FP is only used by the compiler. You can also disable the usage of FP (see: (/Oy [Frame-Pointer Omission](http://msdn.microsoft.com/library/en-us/vccore/html/_core_.2f.oy.asp)).

#### The callstack
The callstack is a memory-region which contains all the data/addresses of the callers. This data must be used to retrieve the callstack (as the name says). The most important part is: this data-region **must not change** until the stack-walking is finished! This is also the reason why the thread must be in the *Suspended* state to retrieve a valid callstack. If you want to do a stack-walking for the current thread, then you must not change the callstack memory after the addresses which are declared in the context record.

### Initializing the [STACKFRAME64](http://msdn.microsoft.com/library/en-us/debug/base/stackframe64_str.asp]-structure)

To successfully walk the callstack with [StackWalk64](http://msdn.microsoft.com/library/en-us/debug/base/stackwalk64.asp), you need to initialize the [STACKFRAME64](http://msdn.microsoft.com/library/en-us/debug/base/stackframe64_str.asp)-structure with *meaningful* values. In the documentation of `StackWalk64` there is only a small note about this structure:

* *The first call to this function will fail if the `AddrPC` and `AddrFrame` members of the `STACKFRAME64` structure passed in the `StackFrame` parameter are not initialized.*

According to this documentation, most programs only initialize `AddrPC` and `AddrFrame` and this had worked until the newest *dbghelp.dll* (v5.6.3.7). Now, you also need to initialize `AddrStack`. After having some trouble with this (and other problems) I talked to the dbghelp-team and got the following answer (2005-08-02; my own comments are written in *italics*!):

* `AddrStack` should always be set to the stack pointer value for all platforms. You can certainly publish that `AddrStack` should be set. You&#39;re also welcome to say that new releases of dbghelp are now requiring this.
* Given a current dbghelp, your code should:
  1. Always use [StackWalk64](http://msdn.microsoft.com/library/en-us/debug/base/stackwalk64.asp)
  2. Always set `AddrPC` to the current instruction pointer (*`Eip` on x86, `Rip` on x64 and `StIIP` on IA64*)
  3. Always set `AddrStack` to the current stack pointer (*`Esp` on x86, `Rsp` on x64 and `IntSp` on IA64*)
  4. Set `AddrFrame` to the current frame pointer when meaningful. On x86 this is `Ebp`, on x64 you can use `Rbp` (*but is not used by VC2005B2; instead it uses `Rdi`!*) and on IA64 you can use `RsBSP`. [StackWalk64](http://msdn.microsoft.com/library/en-us/debug/base/stackwalk64.asp) will ignore the value when it isn&#39;t needed for unwinding.
  5. Set `AddrBStore` to `RsBSP` for IA64.

### Walking the callstack of the current thread

On x86 systems (prior to XP), there is no direct supported function to retrieve the context of the current thread.
The recommended way is to throw an exception and catch it. Now you will have a valid context-record.
The default way of capturing the context of the current thread is by doing some inline-assembler to retrieve `EIP`, `ESP` and `EBP`.
If you want to use the *documented* way, then you need to define `CURRENT_THREAD_VIA_EXCEPTION` for the project.
But you should be aware of the fact, that `GET_CURRENT_CONTEXT` is a macro which internally uses [`__try __except`](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vccelng/htm/key_s-z_4.asp).
Your function must be able to contain these statements.

Starting with XP and on x64 and IA64 systems, there is a documented function to retrieve the context of the current thread: [RtlCaptureContext](http://msdn.microsoft.com/library/en-us/debug/base/rtlcapturecontext.asp).

To do a stack-walking of the current thread, you simply need to do:

```c++
    StackWalker sw;
    sw.ShowCallstack();
```

### Walking the callstack of other threads in the same process

To walk the callstack of another thread inside the same process, you need to suspend the target thread (so the callstack will not change during the stack-walking).
But you should be aware that suspending a thread in the same process might lead to dead-locks! (See: [Why you never should call Suspend/TerminateThread (Part I)](http://blog.kalmbachnet.de/?postid=6), [Part II](http://blog.kalmbachnet.de/?postid=16, [Part III](http://blog.kalmbachnet.de/?postid=17))

If you have the handle to the thread, you can do the following to retrieve the callstack:

```c++
    MyStackWalker sw;
    sw.ShowCallstack(hThread);
```

For a complete sample to retrieve the callstack of another thread, you can take a look at the demo-project.

### Walking the callstack of other threads in other processes

The approach is almost the same as for walking the callstack for the current process.
You only need to provide the `ProcessID` and a handle to the process (`hProcess`). Then you also need to suspend the thread to do the stack-walking. A complete sample to retrieve the callstack of another process is in the demo-project.

### Reusing the `StackWalk` instance

It is no problem to reuse the `StackWalk` instance, as long as you want to do the stack-walking for the same process.
If you want to do a lot of stack-walking it is recommended to reuse the instance.
The reason is simple: if you create a new instance, then the symbol-files must be re-loaded for each instance.
And this is really time-consuming. Also it is not allowed to access the `StackWalk` functions from different threads (the *dbghelp.dll* is **not** thread-safe!). Therefore it makes no sense to create more than one instance...

### Symbol-Search-Path

By default, a symbol-search path (`SymBuildPath` and `SymUseSymSrv`) is provided to the *dbghelp.dll*. This path contains the following directories:

* The optionally provided `szSymPath`. If this parameter is provided, the option <code>SymBuildPath</code> is automatically set. Each path must be separated with a semi-colon &quot;;&quot;
* The current directory
* The directory of the EXE
* The environment variable `_NT_SYMBOL_PATH`
* The environment variable `_NT_ALTERNATE_SYMBOL_PATH`
* The environment variable `SYSTEMROOT`
* The environment variable `SYSTEMROOT` appended with &quot;*\system32*&quot;
* The public Microsoft symbol-server: *RV*%SYSTEMDRIVE%\websymbols*http://msdl.microsoft.com/download/symbols*

### Symbol-Server

If you want to use the public symbols for the OS-files from the [Microsoft-Symbol-Server](http://support.microsoft.com/?kbid=311503), you either need the [Debugging Tools for Windows](http://www.microsoft.com/whdc/devtools/debugging/) (then *symsrv.dll* and the latest *dbghelp.dll* will be found automatically) or you need to redistribute &quot;*dbghelp.dll*&quot; **and** &quot;*smysrv.dll*&quot; from this package!

### Loading the modules and symbols

To successfully walk the callstack of a thread, *dbghelp.dll* requires that the modules are known by the library. Therefore you need to &quot;register&quot; each module of the process via [`SymLoadModule64`](http://msdn.microsoft.com/library/en-us/debug/base/symloadmodule64.asp). To accomplish this you need to enumerate the modules of the given process.

Starting with Win9x and W2K, it is possible to use the [ToolHelp32-API](http://msdn.microsoft.com/library/en-us/perfmon/base/tool_help_library.asp). You need to make a [snapshot (`CreateToolhelp32Snapshot`)](http://msdn.microsoft.com/library/en-us/perfmon/base/createtoolhelp32snapshot.asp) of the process and then you can enumerate the modules via [Module32First](http://msdn.microsoft.com/library/en-us/perfmon/base/module32first.asp) and [Module32Next](http://msdn.microsoft.com/library/en-us/perfmon/base/module32next.asp). Normally the ToolHelp functions are located in the *kernel32.dll* but on Win9x it is located in a separate DLL: *tlhelp32.dll*. Therefore we need to check the functions in both DLLs.

If you have NT4, then the `ToolHelp32-API` is not available. But in NT4 you can use the PSAPI. To enumerate all modules you need to call [EnumProcessModules](), but you only get the handles to the modules. To feed [SymLoadModule64](http://msdn.microsoft.com/library/en-us/debug/base/symloadmodule64.asp) you also need to query the `ModuleBaseAddr`, `SizeOfImage` (via [GetModuleInformation](http://msdn.microsoft.com/library/en-us/perfmon/base/getmoduleinformation.asp)), `ModuleBaseName` (via [GetModuleBaseName](http://msdn.microsoft.com/library/en-us/perfmon/base/getmodulebasename.asp)) and `ModuleFileName(Path)` (via [GetModuleFileNameEx](http://msdn.microsoft.com/library/en-us/perfmon/base/getmodulefilenameex.asp)).

### *dbghelp.dll

There are a couple of issues with *dbghelp.dll*.

* The first is, there are two &quot;teams&quot; at Microsoft which redistribute the <i>dbghelp.dll</i>. One team is the *OS-team*, the other is the *Debugging-Tools-Team* (I don't know the real names...). In general you can say: The *dbghelp.dll* provided with the [Debugging Tools for Windows](http://www.microsoft.com/whdc/devtools/debugging/) is the most recent version.
One problem of this two teams is the different versioning of the *dbghelp.dll*. For example, for XP-SP1 the version is *5.1.2600.1106* dated *2002-08-29*. The version *6.0.0017.0* which was redistributed from the *debug-team* is dated *2002-04-31*. So there is at least a conflict in the date (the newer version is older). And it is even harder to decide which version is "better" (or has more functionality).
* Starting with Me/W2K, the *dbghelp.dll* file in the *system32* directory is protected by the [System File Protection](http://support.microsoft.com/?kbid=222193). So if you want to use a newer *dbghelp.dll* you need to redistribute the version from the *Debugging Tools for Windows* (put it in the same directory as your EXE).
This leads to a problem on W2K if you want to walk the callstack for an app which was built using VC7 or later. The VC7 compiler generates a new PDB-format (called [DIA](http://msdn.microsoft.com/library/en-us/diasdk/html/vsoriDebugInterfaceAccessSDK.asp)).
This PDB-format cannot be read with the *dbghelp.dll* which is installed with the OS. Therefore you will not get very useful callstacks (or at least with no debugging info like filename, line, function name, ...). To overcome this problem, you need to redistribute a newer *dbghelp.dll*.
* The *dbghelp.dll* version *6.5.3.7* has a *bug* or at least a *documentation change* of the [StackWalk64](http://msdn.microsoft.com/library/en-us/debug/base/stackwalk64.asp) function.
In the documentation you can read:
*The first call to this function will fail if the `AddrPC` and `AddrFrame` members of the `STACKFRAME64` structure passed in the `StackFrame` parameter are not initialized.*

and

* *[The `ContextRecord`] parameter is required only when the `MachineType` parameter is not `IMAGE_FILE_MACHINE_I386`.*

*But this is not true anymore.*
Now the callstack on x86-systems cannot be retrieved if you pass `NULL` as *`ContextRecord`*.
From my point of view this is a major documentation change.
Now you either need to initialize the `AddrStack` as well, or provide a valid *`ContextRecord`* which contains the `EIP`, `EBP` and `ESP` registers!
* See also comments in the *Initializing the STACKFRAME64-structure* chapter...

### Options

To do some kind of modification of the behavior, you can optionally specify some options. Here is the list of the available options:

```c++
typedef enum StackWalkOptions
{
    // No additional info will be retrieved
    // (only the address is available)
    RetrieveNone = 0,

    // Try to get the symbol name
    RetrieveSymbol = 1,

    // Try to get the line for this symbol
    RetrieveLine = 2,

    // Try to retrieve the module info
    RetrieveModuleInfo = 4,

    // Also retrieve the version for the DLL/EXE
    RetrieveFileVersion = 8,

    // Contains all the above
    RetrieveVerbose = 0xF,

    // Generate a "good" symbol-search-path
    SymBuildPath = 0x10,

    // Also use the public Microsoft-Symbol-Server
    SymUseSymSrv = 0x20,

    // Retrieve inline stack frames
    SymGetInlineFrames = 0x40,

    // Contains all the above "Sym"-options
    SymAll = 0x70,

    // Contains all options (default)
    OptionsAll = 0x7F
} StackWalkOptions;
```

## Known issues

* NT/Win9x: This project only support the `StackWalk64` function. If you need to use it on NT4/Win9x, you need to [redistribute the *dbghelp.dll* for this platform.](http://www.microsoft.com/downloads/release.asp?releaseid=30682)
* Currently only supports ANSI-names in callbacks (of course, the project can be compiled with UNICODE...).
* To open a remote thread I used `OpenThread` which is not available on NT4/W9x. To have an example of doing this in NT4/Win9x please refer to [Remote Library](http://www.codeproject.com/win32/Remote.asp).
* Walking mixed-mode callstacks (managed/unmanaged) does only return the unmanaged functions.
* Doesn't work when debugging with the `/DEBUG:fastlink` [option](https://blogs.msdn.microsoft.com/vcblog/2014/11/12/speeding-up-the-incremental-developer-build-scenario/)
