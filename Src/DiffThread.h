// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  DiffThread.h
 *
 * @brief Declaration file for CDiffThread
 */
#pragma once

#include <memory>
#include <functional>
#define POCO_NO_UNWINDOWS 1
#include <Poco/Thread.h>
#include <Poco/BasicEvent.h>
#include <Poco/Delegate.h>
#include "DiffContext.h"

namespace Poco
{
class Semaphore;
}
class DiffThreadAbortable;

/**
 * @brief Structure used in sending data to the threads.
 * As thread functions have only one parameter we must pack all
 * the data we need inside structure.
 */
struct DiffFuncStruct
{
	CDiffContext * context; /**< Compare context. */
	Poco::BasicEvent<int> m_listeners; /**< Event listeners */
	int nThreadState; /**< Thread state. */
	int nCollectThreadState; /**< Collect thread state. */
	int nThreadCount; /**< Number of threads used for compare. */
	DiffThreadAbortable * m_pAbortgate; /**< Interface for aborting compare. */
	Poco::Semaphore *pSemaphore; /**< Semaphore for synchronizing threads. */
	std::function<void (DiffFuncStruct*)> m_fncCollect;
	std::function<void (DiffFuncStruct*)> m_fncCompare;
	bool bMarkedRescan;	/**< Is the rescan due to "Refresh Selected"? */
	Poco::Event m_collectCompletedEvent;

	DiffFuncStruct()
		: context(nullptr)
		, nThreadState(0/*CDiffThread::THREAD_NOTSTARTED*/)
		, nThreadCount(1)
		, nCollectThreadState(0/*CDiffThread::THREAD_NOTSTARTED*/)
		, m_pAbortgate(nullptr)
		, pSemaphore(nullptr)
		, bMarkedRescan(false)
		{}
};

/**
 * @brief Class for threaded folder compare.
 * This class implements folder compare in two phases and in two threads:
 * - first thread collects items to compare to compare-time list
 *   (m_diffList).
 * - second threads compares items in the list.
 */
class CDiffThread
{
public:
	/** @brief Thread's states. */
	enum ThreadState
	{
		THREAD_NOTSTARTED = 0, /**< Thread not started, idle. */
		THREAD_COMPARING, /**< Thread running (comparing). */
		THREAD_COMPLETED, /**< Thread has completed its task. */
	};

	enum ThreadEvent
	{
		EVENT_COLLECT_COMPLETED = 2,
		EVENT_COMPARE_PROGRESSED = 1,
		EVENT_COMPARE_COMPLETED = 0,
	};

// creation and use, called on main thread
	CDiffThread();
	~CDiffThread();
	void SetContext(CDiffContext * pCtx);
	void SetThreadCount(int count);
	unsigned CompareDirectories();
	template<class T>
	void AddListener(T *pObj, void (T::*pMethod)(int& state)) {
		m_pDiffParm->m_listeners += Poco::delegate(pObj, pMethod);
	}
	template<class T>
	void RemoveListener(T *pObj, void (T::*pMethod)(int& state)) {
		m_pDiffParm->m_listeners -= Poco::delegate(pObj, pMethod);
	}
	void SetCollectFunction(std::function<void(DiffFuncStruct*)> func) { m_pDiffParm->m_fncCollect = func; }
	void SetCompareFunction(std::function<void(DiffFuncStruct*)> func) { m_pDiffParm->m_fncCompare = func; }
	void SetMarkedRescan(bool bMarkedRescan);
	bool IsMarkedRescan() const;

// runtime interface for main thread, called on main thread
	unsigned GetThreadState() const;
	unsigned GetCollectThreadState() const;
	void Abort() { m_bAborting = true; }
	bool IsAborting() const { return m_bAborting; }
	void Pause() { m_bPaused = true; }
	void Continue() { m_bPaused = false; }
	bool IsPaused() const { return m_bPaused; }

// runtime interface for child thread, called on child thread
	bool ShouldAbort() const;

private:
	CDiffContext * m_pDiffContext; /**< Compare context storing results. */
	Poco::Thread m_threads[2]; /**< Compare threads. */
	std::unique_ptr<DiffFuncStruct> m_pDiffParm; /**< Structure for sending data to threads. */
	std::unique_ptr<DiffThreadAbortable> m_pAbortgate;
	bool m_bAborting; /**< Is compare aborting? */
	bool m_bPaused; /**< Is compare paused? */
};

/**
 * @brief Sets context pointer forwarded to thread.
 * @param [in] pCtx Pointer to compare context.
 */
inline void CDiffThread::SetContext(CDiffContext * pCtx)
{
	m_pDiffContext = pCtx;
}

/**
 * @brief Sets number of threads used for compare.
 * @param [in] count Number of threads to use for compare.
 */
inline void CDiffThread::SetThreadCount(int count)
{
	m_pDiffParm->nThreadCount = count;
}

/**
 * @brief Returns thread's current state
 */
inline unsigned CDiffThread::GetThreadState() const
{
	return m_pDiffParm->nThreadState;
}

/**
 * @brief Returns collect thread's current state
 */
inline unsigned CDiffThread::GetCollectThreadState() const
{
	return m_pDiffParm->nCollectThreadState;
}

/**
 * @brief Sets whether the rescan is due to "Refresh Selected"
 * @param [in] bMarkedRescan Is the rescan due to "Refresh Selected"?
 */
inline void CDiffThread::SetMarkedRescan(bool bMarkedRescan)
{
	m_pDiffParm->bMarkedRescan = bMarkedRescan;
}

/**
 * @brief Returns whether the rescan is due to "Refresh Selected"
 */
inline bool CDiffThread::IsMarkedRescan() const
{
	return m_pDiffParm->bMarkedRescan;
}
