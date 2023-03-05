/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/**
 *  @file Exceptions.h
 *
 *  @brief Exceptions handlers (currently, only SE handler for try/catch)
 */ 
#pragma once

#ifdef _MSC_VER

#include <windows.h>
#include <strsafe.h>

#endif

#include "utils/ctchar.h"


/**
 * @brief C exception "wrapper" class for C++ try/catch
 *
 * @note : for the original idea, see MSDN help about _set_se_translator
 *   derive from CException::CException
 *     creator needs a flag bAutoDelete
 *     (when flag is set, exception is reserved on heap, avoid to use the stack as it may be wrecked)
 *     one single interface to catch SE_Exception and CException
 *   GetErrorMessage : avoid using CString during exception processing
 */
class SE_Exception
{
private:
	unsigned long nSE;
public:
	explicit SE_Exception(unsigned long n) : nSE(n) {}
	~SE_Exception() = default;

	unsigned long getSeNumber() { return nSE; }
#ifdef _MSC_VER
	const tchar_t *getSeMessage()
	{
		// known exceptions (from WINNT.H)
		#define EXCEPTION( x ) case EXCEPTION_##x: return _T(#x);

	  switch ( nSE )
	  {
			EXCEPTION( ACCESS_VIOLATION )
			EXCEPTION( DATATYPE_MISALIGNMENT )
			EXCEPTION( BREAKPOINT )
			EXCEPTION( SINGLE_STEP )
			EXCEPTION( ARRAY_BOUNDS_EXCEEDED )
			EXCEPTION( FLT_DENORMAL_OPERAND )
			EXCEPTION( FLT_DIVIDE_BY_ZERO )
			EXCEPTION( FLT_INEXACT_RESULT )
			EXCEPTION( FLT_INVALID_OPERATION )
			EXCEPTION( FLT_OVERFLOW )
			EXCEPTION( FLT_STACK_CHECK )
			EXCEPTION( FLT_UNDERFLOW )
			EXCEPTION( INT_DIVIDE_BY_ZERO )
			EXCEPTION( INT_OVERFLOW )
			EXCEPTION( PRIV_INSTRUCTION )
			EXCEPTION( IN_PAGE_ERROR )
			EXCEPTION( ILLEGAL_INSTRUCTION )
			EXCEPTION( NONCONTINUABLE_EXCEPTION )
			EXCEPTION( STACK_OVERFLOW )
			EXCEPTION( INVALID_DISPOSITION )
			EXCEPTION( GUARD_PAGE )
			EXCEPTION( INVALID_HANDLE )
		}

		// don't localize this as we do not localize the known exceptions
		return _T("Unknown structured exception");
	}
	virtual bool GetErrorMessage(tchar_t *lpszError, unsigned nMaxError, unsigned *pnHelpContext = nullptr)
	{
		StringCchPrintf(lpszError, nMaxError, _T("Exception %s (0x%.8x)"), getSeMessage(), static_cast<unsigned>(getSeNumber()));
		return true;
	}
#else
	virtual bool GetErrorMessage(tchar_t *lpszError, unsigned nMaxError, unsigned *pnHelpContext = nullptr)
	{
		return true;
	}
#endif
};


/**
 * @brief Set the structured exception translator during the life of the object.
 * Just add 'SE_Handler seh;' at the beginning of the function.
 *
 * @note May be created as a global variable. In fact, you need one instance
 * for each thread.
 */
class SE_Handler {
#ifdef _MSC_VER
private:
	_se_translator_function fnOld;
	static void seh_trans_func(unsigned u, EXCEPTION_POINTERS* pExp) 
	{
		unsigned dwCode = (pExp && pExp->ExceptionRecord) ? pExp->ExceptionRecord->ExceptionCode : 0;
		throw SE_Exception((long)dwCode);
	}
public:
	SE_Handler() : fnOld{_set_se_translator(seh_trans_func)} {}
	~SE_Handler() { _set_se_translator(fnOld); }
#else
public:
	SE_Handler() {}
#endif
};
