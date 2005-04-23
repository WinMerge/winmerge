/*
Module : SINSTANCE.H
Purpose: Defines the interface for an MFC wrapper class to do instance checking
Created: PJN / 29-07-1998

Copyright (c) 1998 - 2003 by PJ Naughter.  

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


#ifndef __SINSTANCE_H__
#define __SINSTANCE_H__

#ifndef __AFXMT_H__
#pragma message("to avoid this message, you should put afxmt.h in your PCH (normally stdafx.h)")
#include <afxmt.h>
#endif

class CInstanceChecker : public CObject
{
public:
//Constructors / Destructors
	CInstanceChecker(const CString& sMutexName);
	virtual ~CInstanceChecker();

//General functions
	void ActivateChecker();
	BOOL TrackFirstInstanceRunning();
	BOOL PreviousInstanceRunning();
	HWND ActivatePreviousInstance(); 
	void QuitPreviousInstance(int nExitCode = 0);

protected:
//Virtual methods
	virtual CString GetMMFFilename();
	virtual HWND GetWindowToTrack();

//Standard non-virtual methods
  void ReleaseLock();

//Data
	CMutex       m_instanceDataMutex;
	CMutex       m_executeMutex;
	CSingleLock* m_pExecuteLock;
  CString      m_sName;
};

#endif //__SINSTANCE_H__
