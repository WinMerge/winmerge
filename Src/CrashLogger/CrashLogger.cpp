/**
 * @file  CrashLogger.cpp
 *
 * @brief Implementation of lightweight crash information logger
 */
#include "StdAfx.h"
#include "CrashLogger.h"
#include "../../Externals/StackWalker/Main/StackWalker/StackWalker.h"
#include "Logger.h"
#include "ConfigLog.h"
#include "paths.h"
#include "UniFile.h"
#include "DirItem.h"
#include "DirTravel.h"
#include "Environment.h"
#include <Windows.h>

namespace
{
	constexpr const tchar_t* CRASH_INFO_FILENAME = _T("WinMergeCrashInfo.txt");
	constexpr const tchar_t* CRASH_DUMP_PATTERN = _T("WinMerge*.dmp");

	static volatile bool g_bCrashLoggingEnabled = false;
	static void* g_hVectoredHandler = nullptr;

	/**
	 * @brief Get exception name from code
	 */
	const tchar_t* GetExceptionName(DWORD code)
	{
		switch (code)
		{
		case EXCEPTION_ACCESS_VIOLATION: return _T("Access Violation");
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return _T("Array Bounds Exceeded");
		case EXCEPTION_DATATYPE_MISALIGNMENT: return _T("Datatype Misalignment");
		case EXCEPTION_FLT_DIVIDE_BY_ZERO: return _T("Float Divide by Zero");
		case EXCEPTION_ILLEGAL_INSTRUCTION: return _T("Illegal Instruction");
		case EXCEPTION_INT_DIVIDE_BY_ZERO: return _T("Integer Divide by Zero");
		case EXCEPTION_STACK_OVERFLOW: return _T("Stack Overflow");
		default: return _T("Unknown Exception");
		}
	}

	/**
	 * @brief Get module information for address (XP-compatible, minimal API usage)
	 * @note Best-effort crash logging.
	 *       This function avoids heap allocation and STL where possible,
	 *       but StackWalker internally uses dbghelp and may allocate memory.
	 */
	bool GetModuleInfo(PVOID addr, tchar_t* name, size_t nameSize, PVOID& base, DWORD& offset)
	{
		// Default outputs
		if (name && nameSize)
			name[0] = 0;
		base = nullptr;
		offset = 0;

		// Resolve module base using VirtualQuery (XP safe)
		MEMORY_BASIC_INFORMATION mbi;
		if (!VirtualQuery(addr, &mbi, sizeof(mbi)))
			return false;

		HMODULE hModule = (HMODULE)mbi.AllocationBase;
		if (!hModule)
			return false;

		// Get module file name (kernel32, XP safe)
		if (name && nameSize)
		{
			DWORD len = GetModuleFileName(hModule, name, (DWORD)nameSize);
			if (len == 0)
				_tcsncpy_s(name, nameSize, _T("Unknown"), _TRUNCATE);
		}

		// Compute base + offset without Psapi
		base = (PVOID)hModule;
		offset = (DWORD)((BYTE*)addr - (BYTE*)base);
		return true;
	}

	static void DumpStackSafe(StackWalker* sw, HANDLE hFile,
		HANDLE hThread, CONTEXT* ctx)
	{
		__try
		{
			sw->ShowCallstack(hThread, ctx);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			const char* msg = "\n<Stack trace unavailable>\n";
			DWORD len;
			WriteFile(hFile, msg, (DWORD)strlen(msg), &len, nullptr);
		}
	}

	/**
	 * @brief Write crash information to file
	 * @note Best-effort crash logging.
	 *       This function avoids heap allocation and STL where possible,
	 *       but StackWalker internally uses dbghelp and may allocate memory.
	 */
	void WriteCrashInfo(EXCEPTION_POINTERS* ex)
	{
		// Stack-only buffers (no heap allocation)
		tchar_t tempPath[MAX_PATH];
		if (GetTempPath(MAX_PATH, tempPath) == 0)
			return;

		tchar_t crashFilePath[MAX_PATH];
		PathCombine(crashFilePath, tempPath, CRASH_INFO_FILENAME);

		HANDLE hFile = CreateFile(crashFilePath, GENERIC_WRITE, 0, nullptr,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE)
			return;

		SYSTEMTIME st;
		GetLocalTime(&st);

		tchar_t moduleName[MAX_PATH] = _T("Unknown");
		PVOID moduleBase = nullptr;
		DWORD offset = 0;
		GetModuleInfo(ex->ExceptionRecord->ExceptionAddress, moduleName, MAX_PATH, moduleBase, offset);

		// Build crash info (stack buffer)
		char buffer[2048];
		int len = _snprintf_s(buffer, _TRUNCATE,
			"WinMerge Crash Information\r\n"
			"==========================\r\n"
			"Timestamp:        %04d-%02d-%02d %02d:%02d:%02d\r\n"
			"Process ID:       %u\r\n"
			"Thread ID:        %u\r\n"
			"Exception Code:   0x%08X (%S)\r\n"
			"Exception Addr:   0x%p\r\n"
			"Module Name:      %S\r\n"
			"Module Base:      0x%p\r\n"
			"Offset:           0x%08X\r\n"
			"Stack Info:\r\n",
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
			GetCurrentProcessId(), GetCurrentThreadId(),
			ex->ExceptionRecord->ExceptionCode, GetExceptionName(ex->ExceptionRecord->ExceptionCode),
			ex->ExceptionRecord->ExceptionAddress,
			PathFindFileName(moduleName), moduleBase, offset);

		DWORD written;
		if (len > 0)
			WriteFile(hFile, buffer, len, &written, nullptr);

		class MyStackWalker : public StackWalker
		{
		public:
			HANDLE hFile;
			MyStackWalker(HANDLE f) : hFile(f) {}
		protected:
			void OnOutput(LPCSTR text) override
			{
				if (hFile)
				{
					DWORD len = (DWORD)strlen(text);
					WriteFile(hFile, text, len, &len, nullptr);
				}
			}
		};

		const char* separator = "~~~\r\n";
		WriteFile(hFile, separator, (DWORD)strlen(separator), &written, nullptr);

		MyStackWalker sw(hFile);
		DumpStackSafe(&sw, hFile, GetCurrentThread(), ex->ContextRecord);

		WriteFile(hFile, separator, (DWORD)strlen(separator), &written, nullptr);

		CloseHandle(hFile);
	}

	/**
	 * @brief Vectored exception handler for crash logging
	 * @note This handler logs only the first crash per process lifetime to prevent
	 *       crash loops. The handling flag is intentionally never reset, ensuring
	 *       that if crash logging itself triggers an exception or if multiple crashes
	 *       occur in rapid succession, only the first is logged.
	 */
	LONG CALLBACK WinMergeVectoredExceptionHandler(EXCEPTION_POINTERS* ex)
	{
		if (!g_bCrashLoggingEnabled || !ex || !ex->ExceptionRecord)
			return EXCEPTION_CONTINUE_SEARCH;
		
		// One-time crash logging per process to prevent crash loops
		static volatile LONG handling = 0;
		if (InterlockedExchange(&handling, 1) == 1)
			 return EXCEPTION_CONTINUE_SEARCH;
		
		__try
		{
			WriteCrashInfo(ex);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			// Best-effort logging only
		}
		return EXCEPTION_CONTINUE_SEARCH;
	}

	/**
	 * @brief Get temp crash info path (safe to use outside crash handler)
	 */
	String GetTempCrashInfoPath()
	{
		tchar_t temp[MAX_PATH];
		if (!GetTempPath(MAX_PATH, temp))
			return _T("");
		tchar_t path[MAX_PATH];
		PathCombine(path, temp, CRASH_INFO_FILENAME);
		return path;
	}

	String GetAppDataCrashInfoDir()
	{
		return paths::ConcatPath(env::GetAppDataPath(), _T("WinMerge\\CrashInfo"));
	}

	String GetTimestampedPath(const String& dir, const tchar_t* prefix, const SYSTEMTIME& st)
	{
		return strutils::format(_T("%s\\%s-%04d%02d%02d%02d%02d%02d.txt"),
			dir.c_str(), prefix, 
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	}

	String GetAppDataCrashInfoPath(const SYSTEMTIME& st)
	{
		return GetTimestampedPath(GetAppDataCrashInfoDir(), _T("WinMergeCrashInfo"), st);
	}

	String GetAppDataCrashInfoConfigPath(const SYSTEMTIME& st)
	{
		return GetTimestampedPath(GetAppDataCrashInfoDir(), _T("WinMergeConfig"), st);
	}

	/**
	 * @brief Read file content (safe to use outside crash handler)
	 */
	String ReadCrashFile(const String& path)
	{
		String text;
		UniMemFile file;
		if (file.OpenReadOnly(path))
			file.ReadStringAll(text);
		return text;
	}

	/**
	 * @brief Find most recent WER crash dump
	 */
	String FindMostRecentCrashDump()
	{
		tchar_t localAppData[MAX_PATH];
		if (SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, localAppData) != S_OK)
			return _T("");

		const String dir = paths::ConcatPath(localAppData, _T("CrashDumps"));
		DirItemArray dirs, files;
		DirTravel::LoadFiles(dir, &dirs, &files, CRASH_DUMP_PATTERN);

		std::sort(files.begin(), files.end(),
			[](const DirItem& a, const DirItem& b) { return a.mtime < b.mtime; });

		if (files.empty())
			return _T("");

		return paths::ConcatPath(dir, files.back().filename.get());
	}

	/**
	 * @brief Create config log file in AppData crash info path
	 */
	String CreateConfigLog(const SYSTEMTIME& st)
	{
		String sError;
		CConfigLog configLog;
		if (!configLog.WriteLogFile(sError))
			return _T("");
		const String configLogPath = GetAppDataCrashInfoConfigPath(st);
		if (!CopyFile(configLog.GetFileName().c_str(), configLogPath.c_str(), FALSE))
			return configLog.GetFileName();
		return configLogPath;
	}

	/**
	 * @brief Output crash info to log
	 */
	void OutputCrashInfo(const String& path, const String& content, const String& configLogPath)
	{
		String message;
		message += _T("\n");
		message += content;

		message += _T("* Crash info: ");
		message += path;
		message += _T("\n");
		String dump = FindMostRecentCrashDump();
		if (dump.empty())
		{
			message += _T("* Crash dump: not found\n");
		}
		else
		{
			message += _T("* Crash dump: ");
			message += dump;
			message += _T("\n");
		}

		if (configLogPath.empty())
		{
			message += _T("* Config log: failed to create\n");
		}
		else
		{
			message += _T("* Config log: ");
			message += configLogPath;
		}

		RootLogger::Info(message);
	}
}

namespace CrashLogger
{
	/**
	 * @brief Install crash logger using Vectored Exception Handler
	 * @return true if handler was successfully installed
	 * @note Uses AddVectoredExceptionHandler instead of SetUnhandledExceptionFilter
	 *       because on Windows XP/7, the error reporting dialog can appear repeatedly
	 *       even after canceling it when using SetUnhandledExceptionFilter.
	 *       VEH with priority 1 ensures our handler runs early in the exception chain.
	 */
	bool Install()
	{
		if (!g_hVectoredHandler)
			g_hVectoredHandler = AddVectoredExceptionHandler(1, WinMergeVectoredExceptionHandler);
		g_bCrashLoggingEnabled = (g_hVectoredHandler != nullptr);
		return g_bCrashLoggingEnabled;
	}

	void Disable()
	{
		g_bCrashLoggingEnabled = false;
		if (g_hVectoredHandler)
		{
			RemoveVectoredExceptionHandler(g_hVectoredHandler);
			g_hVectoredHandler = nullptr;
		}
	}

	bool HasPreviousCrash()
	{
		const String path = GetTempCrashInfoPath();
		return !path.empty() && PathFileExists(path.c_str());
	}

	void CheckAndReportPreviousCrash()
	{
		const String tempPath = GetTempCrashInfoPath();
		if (tempPath.empty() || !PathFileExists(tempPath.c_str()))
			return;

		SYSTEMTIME st;
		GetLocalTime(&st);
		
		const String dir = GetAppDataCrashInfoDir();
		paths::CreateIfNeeded(dir);
		
		const String appDataPath = GetAppDataCrashInfoPath(st);
		const String configLogPath = CreateConfigLog(st);
		const String content = ReadCrashFile(tempPath);
		if (!appDataPath.empty() && CopyFile(tempPath.c_str(), appDataPath.c_str(), FALSE))
			OutputCrashInfo(appDataPath, content, configLogPath);
		else
			OutputCrashInfo(tempPath, content, configLogPath);

		DeleteFile(tempPath.c_str());
		RootLogger::Error(_("WinMerge detected a previous crash. Please report this if possible."));
	}
}
