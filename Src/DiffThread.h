#ifndef _DIFFTHREAD_H
#define _DIFFTHREAD_H

#include "diffcontext.h"

enum
{
	THREAD_NOTSTARTED = 0,
	THREAD_COMPARING,
	THREAD_COMPLETED
};

struct DiffFuncStruct
{
	CString path1;
	CString path2;
	CDiffContext * context;
	UINT msgUIUpdate;
	UINT msgStatusUpdate;
	HWND hWindow;
	UINT nThreadState;
};

class CDiffThread
{
public:
	CDiffThread();
	~CDiffThread();
	CDiffContext * SetContext(CDiffContext * pCtx);
	UINT CompareDirectories(CString dir1, CString dir2);
	void SetHwnd(HWND hWnd);
	void SetMessageIDs(UINT updateMsg, UINT statusMsg);
	UINT GetThreadState();

private:
	CDiffContext * m_pDiffContext;
	CWinThread * m_thread;
	UINT m_msgUpdateUI;
	UINT m_msgUpdateStatus;
	HWND m_hWnd;
};

UINT DiffThread(LPVOID lpParam);

#endif /* _DIFFTHREAD_H */