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

// Implementation but available for reparsing
	void Parse(int argc, TCHAR *argv[]);
	void Clear();

// Querying parsed results
	bool HasEmptySwitch(LPCTSTR name) const;
	bool HasEmptySwitchInsensitive(LPCTSTR name) const;

	int GetParamsCount() const;
	CString GetParam(int i) const;

// Implementation data
private:
	StringMap * m_switches;
	StringMap * m_switchesCapitalized;
	CStringArray * m_params;
};

#endif // CmdArgs_h_included
