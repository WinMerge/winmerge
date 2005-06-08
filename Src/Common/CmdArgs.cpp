/* The MIT License
Copyright (c) 2005 Perry Rapp
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/** 
 * @file  CmdArgs.cpp
 *
 * @brief Implementation of CmdArgs class
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "CmdArgs.h"

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// helper class StringMap

class StringMap : public CMap<CString, LPCTSTR, CString, CString>
{
};

/////////////////////////////////////////////////////////////////////////////
// main class CmdArgs

CmdArgs::CmdArgs(int argc, TCHAR *argv[])
{
	m_switches = new StringMap;
	m_switchesCapitalized = new StringMap;
	m_params = new CStringArray;

	Parse(argc, argv);
}
CmdArgs::~CmdArgs()
{
	delete m_switches; m_switches=0;
	delete m_switchesCapitalized; m_switchesCapitalized=0;
	delete m_params; m_params=0;
}

void CmdArgs::Clear()
{
	m_switches->RemoveAll();
	m_switchesCapitalized->RemoveAll();
	m_params->RemoveAll();
}

void CmdArgs::Parse(int argc, TCHAR *argv[])
{
	Clear();

	for (int i = 1; i < argc; i++)
	{
		CString arg = argv[i];
		if (arg.GetLength()>0 && (arg[0] == '-' || arg[0] == '/'))
		{
			CString value;
			// advance over flag specifier
			arg = arg.Mid(1);
			int index = arg.Find(':');
			if (index >= 0)
			{ // switch has value
				value = arg.Mid(index+1);
				arg = arg.Left(index);
			}
			m_switches->SetAt(arg, value);
			arg.MakeUpper();
			m_switchesCapitalized->SetAt(arg, value);
		}
		else
		{
			m_params->Add(arg);
		}
	}
}

int
CmdArgs::GetParamsCount() const
{
	return m_params->GetSize();
}

CString
CmdArgs::GetParam(int i) const
{
	return m_params->GetAt(i);
}

bool
CmdArgs::HasEmptySwitch(LPCTSTR name) const
{
	CString value;
	if (!m_switches->Lookup(name, value)) return false;
	return value.IsEmpty()!=FALSE;
}

bool
CmdArgs::HasEmptySwitchInsensitive(LPCTSTR name) const
{
	CString nameCap = name;
	nameCap.MakeUpper();
	CString value;
	if (!m_switchesCapitalized->Lookup(nameCap, value)) return false;
	return value.IsEmpty()!=FALSE;
}
