/* Later delete this file and use Parallel Patterns Library instead */
#pragma once

namespace Concurrent
{
	template <class ResultType>
	class Task
	{
	private:
		struct TaskParams
		{
			std::function<ResultType()> m_func;
			HANDLE m_hThread = nullptr;
			ResultType m_result{};
			bool m_detached = false;
			bool m_completed = false;
		};

		explicit Task(TaskParams *pParams) : m_pParams(pParams)
		{
		}

	public:
		Task() : m_pParams(nullptr) {}
		Task(Task&& other)
		{
			m_pParams = other.m_pParams;
			other.m_pParams = nullptr;
		}

		Task& operator=(Task&& other)
		{
			m_pParams = other.m_pParams;
			other.m_pParams = nullptr;
			return *this;
		}

		~Task()
		{
			EnterCriticalSection(&g_cs);
			if (m_pParams)
			{
				if (m_pParams->m_completed)
				{
					CloseHandle(m_pParams->m_hThread);
					delete m_pParams;
				}
				else
				{
					m_pParams->m_detached = true;
				}
			}
			LeaveCriticalSection(&g_cs);
		}

	private:
		static unsigned __stdcall ThreadProc(void *pParam)
		{
			TaskParams *pParams = reinterpret_cast<TaskParams *>(pParam);
			pParams->m_result = pParams->m_func();
			EnterCriticalSection(&g_cs);
			if (pParams->m_detached)
			{
				CloseHandle(pParams->m_hThread);
				delete pParams;
			}
			else
			{
				pParams->m_completed = true;
			}
			LeaveCriticalSection(&g_cs);
			return 0;
		}

	public:
		template <typename FuncType>
		static auto Create(FuncType func)
		{
			unsigned threadId;
			TaskParams *pParams = new TaskParams();
			pParams->m_detached = false;
			pParams->m_completed = false;
			pParams->m_func = func;
			pParams->m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, ThreadProc, pParams, 0, &threadId));
			return Task<decltype(func())>(pParams);
		}

		ResultType Get()
		{
			WaitForSingleObject(m_pParams->m_hThread, INFINITE);
			return m_pParams->m_result;
		}

	private:
		TaskParams *m_pParams;
	};

	template <typename FuncType>
	auto CreateTask(FuncType func)
	{
		return Task<decltype(func())>::Create(func);
	}

	extern CRITICAL_SECTION g_cs;
	extern bool g_initialized;
}
