//////////////////////////////////////////////////////////////////////
//
// CShellFileOp.h: interface for the CShellFileOp class.
//
//////////////////////////////////////////////////////////////////////
//
// This class written and copyright by Michael Dunn (mdunn at inreach
// dot com).  You may freely use and redistribute this source code and
// accompanying documentation as long as this notice is retained.
//
// Contact me if you have any questions, comments, or bug reports.  My 
// homepage is at http://home.inreach.com/mdunn/
//
//////////////////////////////////////////////////////////////////////
// 
// Revision history:
//  Oct 11, 1998: Version 1.0: First release.
//
//  Feb 27, 2000: Version 1.1: Fixed a bug in CShellFileOp::Go() that
//  would allocate twice the memory needed in Unicode builds.  The 'new'
//  statements now allocate BYTEs instead of TCHARs.
//
//////////////////////////////////////////////////////////////////////

#ifndef __CSHELLFILEOP_H__
#define __CSHELLFILEOP_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "objbase.h"

//////////////////////////////////////////////////////////////////////
// The CShellFileOp class
//////////////////////////////////////////////////////////////////////

class CShellFileOp  
{
// Constructor and destructor
public:
	CShellFileOp();
    ~CShellFileOp();


// Operations
public:
    BOOL AddSourceFile ( LPCTSTR szPath );
    BOOL AddDestFile ( LPCTSTR szPath );

    const CStringList& GetSourceFileList();
    const CStringList& GetDestFileList();

    void SetOperationFlags (
        UINT  uOpType,
        CWnd* pWnd,
        BOOL  bSilent,
        BOOL  bAllowUndo,
        BOOL  bWildcardFilesOnly,
        BOOL  bNoConfirmation,
        BOOL  bNoConfirmationToMakeDir,
        BOOL  bRenameOnCollision,
        BOOL  bSimpleProgressDlg );

    void SetOperationFlags ( UINT uOpType, CWnd* pWnd, FILEOP_FLAGS fFlags );

    void SetProgressDlgTitle ( LPCTSTR szTitle );

    BOOL Go ( BOOL* lpbOperationStarted,
              int*  lpnAPIReturn = NULL,
              BOOL* lpbAnyOperationsAborted = NULL );

    BOOL AnyOperationsAborted();

    void Reset();


// Internal data
protected:
                                        // State flags that indicate whether
                                        // you've provided enough info before
                                        // calling Go() or AnyOperationsAborted().
    BOOL        m_bFlagsSet;
    BOOL        m_bGoCalledAPI;

                                        // file lists
    CStringList m_lcstrSourceFiles;
    CStringList m_lcstrDestFiles;

                                        // The file op struct passed to 
    SHFILEOPSTRUCT m_rFOS;              // SHFileOperation().


    CString     m_cstrProgressDlgTitle;


// Internal functions
protected:
    void  ResetInternalData();

    DWORD GetRequiredBufferSize ( const CStringList& );
    
    void  FillSzzBuffer ( TCHAR*, const CStringList& );
};


#endif // ndef __CSHELLFILEOP_H__
