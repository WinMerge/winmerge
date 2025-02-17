//
// TestApp.cpp
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#if defined(_WIN32)
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "Poco/PIDFile.h"

#include <string>
#include <iostream>
#include <cstdlib>
#include <signal.h>

#include "Poco/FileStreamRWLock.h"

#if defined(POCO_OS_FAMILY_UNIX)
#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#elif defined(POCO_OS_FAMILY_WINDOWS)
#include "Poco/Thread.h"
#include "Poco/Process.h"
#include "Poco/Event.h"
#include "Poco/NamedEvent.h"
#endif

using namespace Poco;

#if defined(POCO_OS_FAMILY_UNIX)
class MyRunnable: public Runnable
{
public:
	MyRunnable(): _ran(false)
	{
	}

	void run()
	{
		Thread* pThread = Thread::current();
		if (pThread)
		{
			_threadName = pThread->name();
		}
		_ran = true;
	}

	bool ran() const
	{
		return _ran;
	}

private:
	bool _ran;
	std::string _threadName;
};
#endif

#if POCO_OS != POCO_OS_ANDROID
class MyApp
{
public:
	MyApp() {}
	~MyApp() {}

	std::unique_ptr<PIDFile> _pPIDFile;
#if defined(POCO_OS_FAMILY_WINDOWS)
	static Poco::Event _terminated;
	static Poco::NamedEvent _terminate;


	static void terminate()
	{
		_terminate.set();
	}


	static BOOL WINAPI ConsoleCtrlHandler(DWORD ctrlType)
	{
		switch (ctrlType)
		{
		case CTRL_C_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_BREAK_EVENT:
			terminate();
			return _terminated.tryWait(10000) ? TRUE : FALSE;
		default:
			return FALSE;
		}
	}

	void waitForTerminationRequest()
	{
		SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
		_terminate.wait();
		_terminated.set();
	}

#elif defined(POCO_OS_FAMILY_UNIX)
	void waitForTerminationRequest()
	{
		sigset_t sset;
		sigemptyset(&sset);
		if (!std::getenv("POCO_ENABLE_DEBUGGER"))
		{
			sigaddset(&sset, SIGINT);
		}
		sigaddset(&sset, SIGQUIT);
		sigaddset(&sset, SIGTERM);
		sigprocmask(SIG_BLOCK, &sset, NULL);
		int sig;
		sigwait(&sset, &sig);
	}

	int runThreads(std::string pidPath)
	{
		_pPIDFile.reset(new PIDFile(pidPath, true));

		uint32_t sigMask = 4; // Block SIGINT

		Thread thread1(sigMask);
		Thread thread2(sigMask);
		Thread thread3(sigMask);
		Thread thread4(sigMask);

		MyRunnable r1;
		MyRunnable r2;
		MyRunnable r3;
		MyRunnable r4;

		thread1.start(r1);
		thread2.start(r2);
		thread3.start(r3);
		thread4.start(r4);

		waitForTerminationRequest();

		thread1.join();
		thread2.join();
		thread3.join();
		thread4.join();
		return 0;
	}
#endif

	int run(std::string pidPath)
	{
		_pPIDFile.reset(new PIDFile(pidPath, true));
		waitForTerminationRequest();
		return 0;
	}
};
#endif
#if defined(POCO_OS_FAMILY_WINDOWS)
Poco::Event MyApp::_terminated;
Poco::NamedEvent      MyApp::_terminate(Poco::ProcessImpl::terminationEventName(Poco::Process::id()));

#include "Poco/File.h"
#include "Poco/File_WIN32U.h"

HANDLE openFileWithRWAccess(const std::string& path)
{
	DWORD access = GENERIC_READ | GENERIC_WRITE;
	DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;

	HANDLE handle = CreateFileA(path.c_str(), access, shareMode, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (handle == INVALID_HANDLE_VALUE)
		Poco::File::handleLastError(path);

	return handle;
}
#endif

int main(int argc, char** argv)
{
	if (argc > 1)
	{
		std::string arg(argv[1]);
		if (arg == "-hello")
		{
			std::cout << "Hello, world!";
		}
		else if (arg == "-count")
		{
			int n = 0;
			int c = std::cin.get();
			while (c != -1) { ++n; c = std::cin.get(); }
			return n;
		}
		else if (arg == "-env")
		{
			const char* s = std::getenv("TESTENV");
			if (s)
			{
				std::cout << s;
				return 0;
			}
			else return 1;
		}
		else if (arg == "-raise-int")
		{
			signal(SIGINT, SIG_DFL);
			raise(SIGINT);
		}
		else if (arg == "-echo-args")
		{
			for (int i = 2; i < argc; ++i)
			{
				std::cout << argv[i] << std::endl;
			}
		}
#if defined(POCO_OS_FAMILY_UNIX)
		else if (argc > 2 && arg.find("--pidfile") != std::string::npos && std::string(argv[2]) == "--launch-thread") 
		{
			size_t equals_pos = arg.find('=');
			if (equals_pos != std::string::npos)
			{
				std::string pidPath = arg.substr(equals_pos + 1);
				MyApp myApp;
				int result = myApp.runThreads(pidPath);
				return result;
			}
		}
#endif
#if POCO_OS != POCO_OS_ANDROID
		else if (arg.find("--pidfile") != std::string::npos || arg.find("-p") != std::string::npos)
		{
			size_t equals_pos = arg.find('=');
			if (equals_pos != std::string::npos)
			{
				std::string pidPath = arg.substr(equals_pos + 1);
				MyApp myApp;
				int result = myApp.run(pidPath);
				return result;
			}
		}
#endif
		else if (argc > 2 && arg.find("--lock-file") != std::string::npos && std::string(argv[2]).find("--pidfile") != std::string::npos) 
		{
			std::string pidfArg = std::string(argv[2]);
			std::unique_ptr<PIDFile> pidF;
			size_t equals_pos = pidfArg.find('=');
			if (equals_pos != std::string::npos)
			{
				std::string pidPath = pidfArg.substr(equals_pos + 1);
				pidF = std::make_unique<PIDFile>(pidPath, true);
			}
			if (pidF == nullptr)
			{
				return -1;
			}
			equals_pos = arg.find('=');
			if (equals_pos != std::string::npos)
			{
				std::string fl = arg.substr(equals_pos + 1);
#if POCO_OS != POCO_OS_WINDOWS_NT
				FileStream fs(fl, std::ios::in | std::ios::out | std::ios::binary);
#else
				FileStream fs;
				fs.openHandle(openFileWithRWAccess(fl), std::ios::in | std::ios::out | std::ios::binary);
#endif // POCO_OS != POCO_OS_WINDOWS_NT
				Poco::Int32 ok = 1;
				Poco::Int32 lastCount = 0;
				Poco::Int32 counter = 0;
				FileStreamRWLock lock(fs, 0, sizeof(counter));
				for (int i = 0; i < 100; ++i)
				{
					lock.readLock();
					fs.seekg(0, std::ios::beg);
					fs.read((char *)&counter, sizeof(counter));
					lastCount = counter;
					for (int k = 0; k < 100; ++k)
					{
						if (counter != lastCount) ok = -1;
					}
					lock.unlock();
					lock.writeLock();
					for (int k = 0; k < 100; ++k)
					{
						counter = 0;
						fs.seekg(0, std::ios::beg);
						fs.read((char *)&counter, sizeof(counter));
						--counter;
						fs.seekp(0, std::ios::beg);
						fs.write((char *)&counter, sizeof(counter));
						fs.flushToDisk();
					}
					for (int k = 0; k < 100; ++k)
					{
						fs.seekg(0, std::ios::beg);
						fs.read((char *)&counter, sizeof(counter));
						++counter;
						fs.seekp(0, std::ios::beg);
						fs.write((char *)&counter, sizeof(counter));
						fs.flushToDisk();
					}
					fs.seekg(0, std::ios::beg);
					fs.read((char *)&counter, sizeof(counter));
					++counter;
					fs.seekp(0, std::ios::beg);
					fs.write((char *)&counter, sizeof(counter));
					fs.flushToDisk();
					if (counter <= lastCount) ok = -1;
					lock.unlock();
				}
				return ok * counter;
			}
			return -1;
		}
		else if (argc > 2 && arg.find("--trylock-file") != std::string::npos && std::string(argv[2]).find("--pidfile") != std::string::npos) 
		{
			std::string pidfArg = std::string(argv[2]);
			std::unique_ptr<PIDFile> pidF;
			size_t equals_pos = pidfArg.find('=');
			if (equals_pos != std::string::npos)
			{
				std::string pidPath = pidfArg.substr(equals_pos + 1);
				pidF = std::make_unique<PIDFile>(pidPath, true);
			}
			if (pidF == nullptr)
			{
				return -1;
			}
			equals_pos = arg.find('=');
			if (equals_pos != std::string::npos)
			{
				std::string fl = arg.substr(equals_pos + 1);
#if POCO_OS != POCO_OS_WINDOWS_NT
				FileStream fs(fl, std::ios::in | std::ios::out | std::ios::binary);
#else
				FileStream fs;
				fs.openHandle(openFileWithRWAccess(fl), std::ios::in | std::ios::out | std::ios::binary);
#endif // POCO_OS != POCO_OS_WINDOWS_NT
				Poco::Int32 ok = 1;
				Poco::Int32 lastCount = 0;
				Poco::Int32 counter = 0;
				FileStreamRWLock lock(fs, 0, sizeof(counter));
				for (int i = 0; i < 100; ++i)
				{
					while (!lock.tryReadLock()) Thread::yield();
					fs.seekg(0, std::ios::beg);
					fs.read((char *)&counter, sizeof(counter));
					lastCount = counter;
					for (int k = 0; k < 100; ++k)
					{
						if (counter != lastCount) ok = -1;
						Thread::yield();
					}
					lock.unlock();
					while (!lock.tryWriteLock()) Thread::yield();
					for (int k = 0; k < 100; ++k)
					{
						counter = 0;
						fs.seekg(0, std::ios::beg);
						fs.read((char *)&counter, sizeof(counter));
						--counter;
						fs.seekp(0, std::ios::beg);
						fs.write((char *)&counter, sizeof(counter));
						fs.flushToDisk();
						Thread::yield();
					}
					for (int k = 0; k < 100; ++k)
					{
						fs.seekg(0, std::ios::beg);
						fs.read((char *)&counter, sizeof(counter));
						++counter;
						fs.seekp(0, std::ios::beg);
						fs.write((char *)&counter, sizeof(counter));
						fs.flushToDisk();
						Thread::yield();
					}
					fs.seekg(0, std::ios::beg);
					fs.read((char *)&counter, sizeof(counter));
					++counter;
					fs.seekp(0, std::ios::beg);
					fs.write((char *)&counter, sizeof(counter));
					fs.flushToDisk();
					if (counter <= lastCount) ok = -1;
					lock.unlock();
				}
				return ok * counter;
			}
			return -1;
		}
	}
	return argc - 1;
}
