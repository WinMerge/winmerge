/** 
 * @file  DirWatcher.h
 *
 * @brief Declaration file for DirWatcher
 */
#pragma once

#include "UnicodeString.h"
#include <map>
#include <vector>
#include <functional>
#include <Windows.h>

struct DirRequest;

class DirWatcher
{
public:
	enum ACTION { ACTION_ADDED = 1, ACTION_REMOVED, ACTION_MODIFIED, ACTION_RENAMED_OLD_NAME, ACTION_RENAMED_NEW_NAME };
	DirWatcher();
	~DirWatcher();
	DirWatcher(const DirWatcher &) = delete;
	DirWatcher & operator=(const DirWatcher &) = delete;
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

