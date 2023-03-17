/*
 * @file  DirWatcher.cpp
 *
 * @brief Implementation for DirWatcher
 */

#include "pch.h"
#include "DirWatcher.h"
#include "paths.h"
#include <vector>
#include <process.h>
#include <Windows.h>

struct DirEventListener
{
	uintptr_t id = 0;
	bool dir = false;
	String path;
	std::function<void(const String&, DirWatcher::ACTION)> callback;
};

struct DirWatchee
{
	String path;
	HANDLE hDir = nullptr;
	bool watchSubtree = false;
	std::unique_ptr<OVERLAPPED> pOverlapped;
	std::vector<BYTE> info;
	std::vector<DirEventListener> listeners;
};

struct DirRequest
{
	enum REQTYPE { ADD, DEL, EXIT = -1 };
	REQTYPE type = EXIT;
	DirEventListener listener;
};

class DirWatcher::Impl
{
public:
	Impl();
	~Impl();
	bool Add(uintptr_t id, bool dir, const String& path, std::function<void(const String&, ACTION)> callback);
	bool Remove(uintptr_t id);
	void Clear();
private:
	bool startThread();
	bool exitThread();
	unsigned DirWatcherThreadProc();
	static unsigned __stdcall DirWatcherThreadProcStatic(void* pParam);
	HANDLE m_hThread;
	HANDLE m_hEventReq;
	HANDLE m_hEventResp;
	HRESULT m_resp;
	std::unique_ptr<DirRequest> m_pReq;
	CRITICAL_SECTION m_cs;
};

DirWatcher::Impl::Impl()
	: m_hEventReq(nullptr)
	, m_hEventResp(nullptr)
	, m_hThread(nullptr)
	, m_resp(S_OK)
	, m_pReq{ new DirRequest()}
	, m_cs{}
{
	InitializeCriticalSection(&m_cs);
	m_hEventReq = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if (!m_hEventReq)
		return;
	m_hEventResp = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if (!m_hEventResp)
	{
		CloseHandle(m_hEventReq);
		return;
	}
}

DirWatcher::Impl::~Impl()
{
	Clear();
	if (m_hThread)
	{
		exitThread();
	}
	if (m_hEventReq)
		CloseHandle(m_hEventReq);
	if (m_hEventResp)
		CloseHandle(m_hEventResp);
	m_hEventReq = nullptr;
	m_hEventResp = nullptr;
	m_hThread = nullptr;
	DeleteCriticalSection(&m_cs);
}

bool DirWatcher::Impl::Add(uintptr_t id, bool dir, const String& path, std::function<void(const String&, ACTION)> callback)
{
	EnterCriticalSection(&m_cs);

	if (!m_hThread)
		startThread();

	m_pReq->type = DirRequest::ADD;
	m_pReq->listener = { id, dir, path, callback };
	SetEvent(m_hEventReq);

	WaitForSingleObject(m_hEventResp, INFINITE);
	ResetEvent(m_hEventResp);
	bool result = SUCCEEDED(m_resp);

	LeaveCriticalSection(&m_cs);
	return result;
}

bool DirWatcher::Impl::Remove(uintptr_t id)
{
	EnterCriticalSection(&m_cs);

	bool result = false;
	if (m_hThread)
	{
		m_pReq->type = DirRequest::DEL;
		m_pReq->listener.id = id;
		SetEvent(m_hEventReq);

		WaitForSingleObject(m_hEventResp, INFINITE);
		ResetEvent(m_hEventResp);
		result = SUCCEEDED(m_resp);
	}

	LeaveCriticalSection(&m_cs);
	return result;
}

bool DirWatcher::Impl::startThread()
{
	if (m_hThread)
		return false;

	unsigned dwThreadId = 0;
	m_hThread = reinterpret_cast<HANDLE>(
		_beginthreadex(nullptr, 0, DirWatcherThreadProcStatic,
			this, 0, &dwThreadId));
	return m_hThread != nullptr;
}

bool DirWatcher::Impl::exitThread()
{
	bool result = false;

	EnterCriticalSection(&m_cs);

	if (m_hThread)
	{
		m_pReq->type = DirRequest::EXIT;
		SetEvent(m_hEventReq);

		WaitForSingleObject(m_hEventResp, INFINITE);
		ResetEvent(m_hEventResp);
		result = SUCCEEDED(m_resp);

		CloseHandle(m_hThread);
		m_hThread = nullptr;
	}

	LeaveCriticalSection(&m_cs);

	return result;
}

void DirWatcher::Impl::Clear()
{
	Remove(static_cast<uintptr_t>(-1));
}

unsigned DirWatcher::Impl::DirWatcherThreadProc()
{
	std::vector<DirWatchee> watchedDirs;

	auto ReadDirAsync = [](DirWatchee& watchedDir) -> bool
	{
		DWORD dwBytesReturned = 0;
		if (0 == ReadDirectoryChangesW(watchedDir.hDir, watchedDir.info.data(),
			static_cast<DWORD>(watchedDir.info.size()), watchedDir.watchSubtree,
			FILE_NOTIFY_CHANGE_FILE_NAME |
			FILE_NOTIFY_CHANGE_LAST_WRITE |
			FILE_NOTIFY_CHANGE_SIZE |
			FILE_NOTIFY_CHANGE_ATTRIBUTES,
			&dwBytesReturned, watchedDir.pOverlapped.get(), nullptr))
			return false;
		return true;
	};

	auto OpenDir = [&](bool dir, const String& path, DirWatchee& watchedDir) -> bool
	{
		String path2 = dir ? path : paths::GetParentPath(path);
		HANDLE hDir = CreateFile(path2.c_str(), GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			nullptr, OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			nullptr);
		if (hDir != INVALID_HANDLE_VALUE)
		{
			HANDLE hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
			if (hEvent)
			{
				watchedDir.info.resize(sizeof(FILE_NOTIFY_INFORMATION) + MAX_PATH * sizeof(tchar_t));
				watchedDir.hDir = hDir;
				watchedDir.path = path2;
				watchedDir.watchSubtree = dir;
				watchedDir.pOverlapped = std::make_unique<OVERLAPPED>();
				watchedDir.pOverlapped->hEvent = hEvent;
				if (ReadDirAsync(watchedDir))
					return true;
				CloseHandle(hEvent);
			}
			CloseHandle(hDir);
		}
		return false;
	};

	auto OnAdd = [&](DirRequest& req)
	{
		DirEventListener& listener = req.listener;
		auto it = watchedDirs.begin();
		String path2 = listener.dir ? listener.path : paths::GetParentPath(listener.path);
		for (; it != watchedDirs.end(); ++it)
		{
			if (strutils::compare_nocase(it->path, path2) == 0 && it->watchSubtree == listener.dir)
				break;
		}
		if (it == watchedDirs.end())
		{
			auto& watchedDir = watchedDirs.emplace_back();
			if (OpenDir(listener.dir, listener.path, watchedDir))
			{
				watchedDir.listeners.push_back(listener);
			}
			else
			{
				watchedDirs.pop_back();
				m_resp = HRESULT_FROM_WIN32(GetLastError());
			}
		}
		else
		{
			it->listeners.push_back(listener);
		}
	};

	auto OnDel = [&](uintptr_t id)
	{
		for (auto itWatchedDirs = watchedDirs.begin(); itWatchedDirs != watchedDirs.end(); )
		{
			for (auto itListeners = itWatchedDirs->listeners.begin(); itListeners != itWatchedDirs->listeners.end();)
			{
				if (id == static_cast<uintptr_t>(-1) || itListeners->id == id)
					itListeners = itWatchedDirs->listeners.erase(itListeners);
				else
					++itListeners;
			}
			if (itWatchedDirs->listeners.empty())
			{
				CloseHandle(itWatchedDirs->hDir);
				CloseHandle(itWatchedDirs->pOverlapped->hEvent);
				itWatchedDirs = watchedDirs.erase(itWatchedDirs);
			}
			else
			{
				++itWatchedDirs;
			}
		}
	};

	auto OnEvent = [&](DirWatchee& watchedDir)
	{
		DWORD dwNumberOfBytesTransferred = 0;
		if (0 != GetOverlappedResult(watchedDir.hDir, watchedDir.pOverlapped.get(), &dwNumberOfBytesTransferred, TRUE))
		{
			ReadDirAsync(watchedDir);

			auto* pNotifyInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(watchedDir.info.data());
			String relpath(pNotifyInfo->FileName, pNotifyInfo->FileNameLength/sizeof(tchar_t));
			String path = paths::ConcatPath(watchedDir.path, relpath);
			for (auto& listener : watchedDir.listeners)
			{
				if (listener.dir || strutils::compare_nocase(listener.path, path) == 0)
				{
					listener.callback(path, static_cast<ACTION>(pNotifyInfo->Action));
				}
			}
		}
	};

	std::vector<HANDLE> handles;
	handles.push_back(m_hEventReq);
	bool fexit = false;

	while (!fexit)
	{
		DWORD dwResult = WaitForMultipleObjects(static_cast<DWORD>(handles.size()), handles.data(), FALSE, INFINITE);
		if (dwResult == WAIT_TIMEOUT || dwResult == WAIT_FAILED)
			break;
		unsigned n = dwResult - WAIT_OBJECT_0;
		if (n == 0)
		{
			ResetEvent(handles[0]);

			m_resp = S_OK;
			DirRequest& req = *m_pReq.get();
			if (req.type == DirRequest::ADD)
			{
				OnDel(req.listener.id);
				OnAdd(req);
			}
			else if (req.type == DirRequest::DEL)
			{
				OnDel(req.listener.id);
			}
			else
			{
				fexit = true;
			}

			handles.clear();
			handles.push_back(m_hEventReq);
			for (auto& watchedDir : watchedDirs)
				handles.push_back(watchedDir.pOverlapped->hEvent);

			SetEvent(m_hEventResp);
		}
		else
		{
			DirWatchee& watchedDir = watchedDirs[n - 1];
			OnEvent(watchedDir);
		}
	}
	return 0;
}

unsigned __stdcall DirWatcher::Impl::DirWatcherThreadProcStatic(void *pvThis)
{
	return reinterpret_cast<DirWatcher::Impl *>(pvThis)->DirWatcherThreadProc();
}

DirWatcher::DirWatcher() : m_pimpl(new DirWatcher::Impl()) {}
DirWatcher::~DirWatcher() = default;

bool DirWatcher::Add(uintptr_t id, bool dir, const String& path, std::function<void(const String&, ACTION)> callback)
{
	return m_pimpl->Add(id, dir, path, callback); 
}

bool DirWatcher::Remove(uintptr_t id)
{
	return m_pimpl->Remove(id); 
}
void DirWatcher::Clear()
{
	m_pimpl->Clear(); 
}

