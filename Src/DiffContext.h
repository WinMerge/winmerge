// DiffContext.h: interface for the CDiffContext class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIFFCONTEXT_H__D3CC86BE_F11E_11D2_826C_00A024706EDC__INCLUDED_)
#define AFX_DIFFCONTEXT_H__D3CC86BE_F11E_11D2_826C_00A024706EDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RegExp.h"

struct dirdata
{
  char const **names;	/* Sorted names of files in dir, 0-terminated.  */
  char *data;	/* Allocated storage for file names.  */
};

// values for DIFFITEM.code
#define FILE_LUNIQUE     0
#define FILE_RUNIQUE     1
#define FILE_DIFF        2
#define FILE_SAME        3
#define FILE_ERROR       4
#define FILE_BINSAME     5
#define FILE_BINDIFF     6
#define FILE_LDIRUNIQUE  7
#define FILE_RDIRUNIQUE  8

struct DIFFITEM
{
	CString sfilename;
	CString sext;
	CString slpath;
	CString srpath;
	long ltime, rtime;
	BYTE code;
	DIFFITEM() : ltime(0), rtime(0), code(FILE_ERROR) { }
};

class IDiffStatus
{
public:
	virtual void rptFile(BYTE code)=0;
};

class CDiffContext  
{
public:
	CDiffContext(LPCTSTR pszLeft, LPCTSTR pszRight, IDiffStatus * piStatus);
	CDiffContext(LPCTSTR pszLeft, LPCTSTR pszRight, CDiffContext& src);
	virtual ~CDiffContext();

	void SetRegExp(LPCTSTR pszExp);

	// add & remove differences
	void AddDiff(LPCTSTR pszFilename, LPCTSTR pszLeftDir, LPCTSTR pszRightDir, long ltime, long rtime, BYTE code);
	void AddDiff(DIFFITEM di);
	void RemoveDiff(POSITION diffpos);
	void RemoveAll();

	// to iterate over all differences on list
	POSITION GetFirstDiffPosition();
	DIFFITEM GetNextDiffPosition(POSITION & diffpos);
	DIFFITEM GetDiffAt(POSITION diffpos);
	BYTE GetDiffStatus(POSITION diffpos);
	int GetDiffCount();

	// change an existing difference
	void UpdateStatusCode(POSITION diffpos, BYTE status);
	void ClearStatus() { m_piStatus=0; }


	BOOL m_bRecurse;
	CString m_strLeft;
	CString m_strRight;
	CRegExp m_rgx;
	CString m_strRegExp;
	IDiffStatus * m_piStatus;


	struct dirdata ddLeft, ddRight;
	char *pNamesLeft;
	char *pNamesRight;

private:
	CList<DIFFITEM,DIFFITEM> m_dirlist, *m_pList; // master list of differences
};

#endif // !defined(AFX_DIFFCONTEXT_H__D3CC86BE_F11E_11D2_826C_00A024706EDC__INCLUDED_)
