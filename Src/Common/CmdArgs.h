/** 
 * @file  CmdArgs.h
 *
 * @brief Defines the CmdArgs class, which parses & stores commandline arguments for querying
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef CmdArgs_h_included
#define CmdArgs_h_included

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif

class StringMap;

class CmdArgs
{
public:
	CmdArgs(int argc, TCHAR *argv[]);
	~CmdArgs();

// Settings
	void SetCaseSensitive(bool CaseSensitive=true) { m_CaseSensitive = CaseSensitive; }

// Implementation but available for reparsing
	void Parse(int argc, TCHAR *argv[]);
	void Clear();

// Querying parsed results
	bool HasEmptySwitch(LPCTSTR name, bool CaseSensitive=true) const;
	bool HasSwitch(LPCTSTR name, bool CaseSensitive=true) const;
	bool GetSwitch(LPCTSTR name, CString & value, bool CaseSensitive=true) const;

	int GetParamsCount() const;
	CString GetParam(int i) const;

// Implementation methods
private:
	BOOL Lookup(LPCTSTR key, CString & value, bool CaseSensitive) const;

// Implementation data
private:
	StringMap * m_switches;
	StringMap * m_switchesCapitalized;
	CStringArray * m_params;
	bool m_CaseSensitive;
};

#endif // CmdArgs_h_included
