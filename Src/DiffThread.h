#ifndef _DIFFTHREAD_H
#define _DIFFTHREAD_H

#include "diffcontext.h"

struct DiffFuncStruct
{
	CString path1;
	CString path2;
	CDiffContext * context;
	UINT msgUIUpdate;
	UINT msgStatusUpdate;
	HWND window;
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

private:
	CDiffContext * m_pDiffContext;
	UINT m_msgUpdateUI;
	UINT m_msgUpdateStatus;
	HWND m_hWnd;
};

UINT DiffThread(LPVOID lpParam);

#endif /* _DIFFTHREAD_H */