/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
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
	DiffThreadAbortable * m_pAbortgate; /**< Interface for aborting compare. */
	Poco::Semaphore *pSemaphore; /**< Semaphore for synchronizing threads. */
	std::function<void (DiffFuncStruct*)> m_fncCollect;
	std::function<void (DiffFuncStruct*)> m_fncCompare;

	DiffFuncStruct()
		: context(nullptr)
		, nThreadState(0/*CDiffThread::THREAD_NOTSTARTED*/)
		, m_pAbortgate(nullptr)
		, pSemaphore(nullptr)
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

// runtime interface for main thread, called on main thread
	unsigned GetThreadState() const;
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
 * @brief Returns thread's current state
 */
inline unsigned CDiffThread::GetThreadState() const
{
	return m_pDiffParm->nThreadState;
}
