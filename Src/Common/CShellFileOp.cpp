//////////////////////////////////////////////////////////////////////
//
// CShellFileOp.cpp: implementation of the CShellFileOp class.
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
//  Mar 9, 2004: Version 1.1a: Fix compile with Visual Studio 2003
//  Fix from author, applied to WinMerge CVS by Kimmo Varis
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "shlobj.h"
#include "objbase.h"
#include "CShellFileOp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShellFileOp::CShellFileOp()
{
    ResetInternalData();
}

CShellFileOp::~CShellFileOp()
{
}


//////////////////////////////////////////////////////////////////////
// Public operations
//////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//
// Function:    CShellFileOp::AddSourceFile
//
// Description:
//  Adds a file/dir to the list of source files for the next file operation.
//
// Input:
//  szPath: [in] The path to the file/dir to be added.
//
// Returns:
//  TRUE if the path was added successfully.  A CMemoryException will be
//  thrown in an out-of-memory condition.
//
/////////////////////////////////////////////////////////////////////////////

BOOL CShellFileOp::AddSourceFile ( LPCTSTR szPath )
{
    ASSERT ( AfxIsValidString ( szPath ) );

    try
        {
        m_lcstrSourceFiles.AddTail ( szPath );
        }
    catch ( CMemoryException *)
        {
        TRACE0("Memory exception in CShellFileOp::AddSourceFile()!\n");
        throw;
        }

    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    CShellFileOp::AddDestFile
//
// Description:
//  Adds a file/dir to the list of destination files for the next file
//  operation.
//
// Input:
//  szPath: [in] The path to the file/dir to be added.
//
// Returns:
//  TRUE if the path was added successfully.  A CMemoryException will be
//  thrown in an out-of-memory condition.
//
/////////////////////////////////////////////////////////////////////////////

BOOL CShellFileOp::AddDestFile ( LPCTSTR szPath )
{
    ASSERT ( AfxIsValidString ( szPath ) );

    try
        {
        m_lcstrDestFiles.AddTail ( szPath );
        }
    catch ( CMemoryException *)
        {
        TRACE0("Memory exception in CShellFileOp::AddDestFile()!\n");
        throw;
        }

    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    CShellFileOp::GetSourceFileList
//
// Description:
//  Returns a reference to the internal list of source files for the next
//  file operation.
//
// Input:
//  Nothing.
//
// Returns:
//  A CStringList reference.  Mess with this at your own risk. :)
//
/////////////////////////////////////////////////////////////////////////////

const CStringList& CShellFileOp::GetSourceFileList()
{
    return m_lcstrSourceFiles;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    CShellFileOp::GetDestFileList
//
// Description:
//  Returns a reference to the internal list of destination files for the next
//  file operation.
//
// Input:
//  Nothing.
//
// Returns:
//  A CStringList reference.  Mess with this at your own risk. :)
//
/////////////////////////////////////////////////////////////////////////////

const CStringList& CShellFileOp::GetDestFileList()
{
    return m_lcstrDestFiles;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    CShellFileOp::SetOperationFlags
//
// Description:
//  Sets various parameters for the next file operation.
//
// Input:
//  uOpType: [in] FO_COPY, FO_DELETE, FO_MOVE, or FO_RENAME.
//  pWnd: [in] Pointer to the CWnd which will the parent window for any UI that
//             the shell displays during the operation.
//  Along with a bunch of other flags whose names I hope are self-explanatory.
//  They're explained in full in the documentation.
//
// Returns:
//  Nothing.
//
/////////////////////////////////////////////////////////////////////////////

void CShellFileOp::SetOperationFlags ( UINT  uOpType,
                                       CWnd* pWnd,
                                       BOOL  bSilent,
                                       BOOL  bAllowUndo,
                                       BOOL  bWildcardFilesOnly,
                                       BOOL  bNoConfirmation,
                                       BOOL  bNoConfirmationToMakeDir,
                                       BOOL  bRenameOnCollision,
                                       BOOL  bSimpleProgressDlg )
{
FILEOP_FLAGS fFlags = 0;

    // Validate the operation type.  If this assert fires, you sent in
    // an invalid op type.

    ASSERT ( uOpType == FO_COPY  ||  uOpType == FO_DELETE  ||
             uOpType == FO_MOVE  ||  uOpType == FO_RENAME );
    ASSERT_VALID ( pWnd );

                                        // store the op type
    m_rFOS.wFunc = uOpType;

                                        // store the parent window
    m_rFOS.hwnd = pWnd->GetSafeHwnd();

                                        // set the various flags...
    if ( bSilent )                  fFlags |= FOF_SILENT;
    if ( bAllowUndo )               fFlags |= FOF_ALLOWUNDO;
    if ( bWildcardFilesOnly )       fFlags |= FOF_FILESONLY;
    if ( bNoConfirmation )          fFlags |= FOF_NOCONFIRMATION;
    if ( bNoConfirmationToMakeDir ) fFlags |= FOF_NOCONFIRMMKDIR;
    if ( bRenameOnCollision )       fFlags |= FOF_RENAMEONCOLLISION;
    if ( bSimpleProgressDlg )       fFlags |= FOF_SIMPLEPROGRESS;

    m_rFOS.fFlags = fFlags;

    m_bFlagsSet = TRUE;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    CShellFileOp::SetOperationFlags
//
// Description:
//  Sets various parameters for the next file operation.
//
// Input:
//  uOpType: [in] FO_COPY, FO_DELETE, FO_MOVE, or FO_RENAME.
//  pWnd: [in] Pointer to the CWnd which will the parent window for any UI that
//             the shell displays during the operation.
//  fFlags: [in] Any legal combination of the FOF_* flags.  See the docs for
//               SHFileOperation() for info on the flags.
//
// Returns:
//  Nothing.
//
/////////////////////////////////////////////////////////////////////////////

void CShellFileOp::SetOperationFlags ( UINT uOpType, CWnd* pWnd,
                                       FILEOP_FLAGS fFlags )
{
    // Validate the op type.  If this assert fires, check the operation
    // type param that you're passing in.
    ASSERT ( uOpType == FO_COPY  ||  uOpType == FO_DELETE  ||
             uOpType == FO_MOVE  ||  uOpType == FO_RENAME );
    ASSERT_VALID ( pWnd );

                                        // store the op type
    m_rFOS.wFunc = uOpType;

                                        // store the parent window
    m_rFOS.hwnd = pWnd->GetSafeHwnd();

                                        // store the op flags
    m_rFOS.fFlags = fFlags;

    m_bFlagsSet = TRUE;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    CShellFileOp::SetProgressDlgTitle
//
// Description:
//  Sets the string to be used if the simple progress dialog is used for
//  the next file operation.
//
// Input:
//  szTitle: [in] The string to use.
//
// Returns:
//  Nothing.
//
// Note:
//  The object maintains its own copy of the string in a CString, so the
//  caller can destroy or reuse its string once this function returns.
//
/////////////////////////////////////////////////////////////////////////////

void CShellFileOp::SetProgressDlgTitle ( LPCTSTR szTitle )
{
    ASSERT ( AfxIsValidString ( szTitle ) );

    try
        {
        m_cstrProgressDlgTitle = szTitle;
        }
    catch ( CMemoryException *)
        {
        TRACE0("Memory exception in CShellFileOp::SetProgressDlgTitle()!\n");
        throw;
        }
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CShellFileOp::AnyOperationsAborted
//
// Description:
//  Returns a flag indicating whether the user canceled the last file operation.
//
// Input:
//  Nothing.
//
// Returns:
//  TRUE if the user canceled the last file op, or FALSE if not.
//
//////////////////////////////////////////////////////////////////////////

BOOL CShellFileOp::AnyOperationsAborted()
{
    // If this assert fires, it means you called this member function
    // before calling Go(), or you did call Go() but Go() couldn't call
    // the SHFileOperation() API due to incomplete or invalid infomation.
    // You should have gotten an assert in Go() in the latter case - that's
    // where you should be looking for the source of the trouble. :)

    ASSERT ( m_bGoCalledAPI );

    return m_rFOS.fAnyOperationsAborted;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CShellFileOp::Reset
//
// Description:
//  Resets internal data used for file operations.
//
// Input:
//  Nothing.
//
// Returns:
//  Nothing.
//
// Note:
//  If you are using a CShellFileOp object to do multiple operations, call
//  this function after one operation to clear out all data from the
//  operation, and "wipe the slate" for the next one.
//
//////////////////////////////////////////////////////////////////////////

void CShellFileOp::Reset()
{
    ResetInternalData();
}


//////////////////////////////////////////////////////////////////////
// The Go() function!
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//
// Function:    CShellFileOp::Go
//
// Description:
//  Validates data and starts a file operation.
//
// Input:
//  lpbOperationStarted: [out] Pointer to a BOOL that receives TRUE if the
//                       SHFileOperation() API was called to start the
//                       operation, or FALSE if the API was not called.
//  lpnAPIReturn: [out] Pointer to an int that receives the return value from
//                SHFileOperation() if it was called.  If the API is not called,
//                the variable pointed to is not changed.
//  lpbAnyOperationsAborted: [out] Pointer to a BOOL that receives TRUE if the
//                           user aborted the file operation, or FALSE if not.
//
// Returns:
//  TRUE if and only if SHFileOperation() was called and it returned 0 (success).
//
//////////////////////////////////////////////////////////////////////////
// Updated in v1.1 - Changed the two 'new' calls to allocate BYTEs insetad
// of TCHARs.
//////////////////////////////////////////////////////////////////////////

BOOL CShellFileOp::Go ( BOOL* lpbOperationStarted,
                        int*  lpnAPIReturn /*=NULL*/,
                        BOOL* lpbAnyOperationsAborted  /*=NULL*/ )
{
TCHAR* szzSourceFiles = NULL;
TCHAR* szzDestFiles = NULL;
DWORD  dwSourceBufferSize;
DWORD  dwDestBufferSize;
int    nAPIRet;

    // Validate the pointers....
    ASSERT ( AfxIsValidAddress ( lpbOperationStarted, sizeof(BOOL) ) );
    ASSERT ( lpnAPIReturn == NULL  ||
             AfxIsValidAddress ( lpnAPIReturn, sizeof(int) ) );
    ASSERT ( lpbAnyOperationsAborted == NULL  ||
             AfxIsValidAddress ( lpbAnyOperationsAborted, sizeof(BOOL) ) );


    m_bGoCalledAPI = FALSE;

    if ( NULL != lpbOperationStarted )
        {
        *lpbOperationStarted = FALSE;
        }

                                        // Do a bunch of validation before
                                        // calling the API.

                                        // 1. Did you call SetOperationFlags()?

    if ( ! m_bFlagsSet )
        {
        TRACE0("Go() aborting because SetOperationFlags() was not called first.\n");
        goto bailout;
        }

                                        // 2 Is the op type valid?

    if ( ! ( m_rFOS.wFunc == FO_COPY  ||  m_rFOS.wFunc == FO_DELETE  ||
             m_rFOS.wFunc == FO_MOVE  ||  m_rFOS.wFunc == FO_RENAME ) )
        {
        TRACE0("Go() aborting because the operation type was invalid.\n");
        goto bailout;
        }

                                        // 3 Is the source file list nonempty?

    if ( m_lcstrSourceFiles.IsEmpty() )
        {
        TRACE0("Go() aborting because the source file list is empty.\n");
        goto bailout;
        }

                                        // 4. Is the dest file list nonempty
                                        // if the op needs dest files?

    if ( m_rFOS.wFunc != FO_DELETE  &&  m_lcstrDestFiles.IsEmpty() )
        {
        TRACE0("Go() aborting because the destination file list is empty.\n");
        goto bailout;
        }

                                        // 5. Is the dest file list OK?  There
                                        // must either be one entry, or the same
                                        // number of entries as in the source
                                        // list.

    if ( m_rFOS.wFunc != FO_DELETE )
        {
        if ( m_lcstrDestFiles.GetCount() != 1  &&
             m_lcstrDestFiles.GetCount() != m_lcstrSourceFiles.GetCount() )
            {
            TRACE0("Go() aborting because the destination file list has the wrong number of strings.\n");
            goto bailout;
            }
        }


                                        // Everything checked out OK, so now
                                        // build the big double-null-terminated
                                        // buffers.

    dwSourceBufferSize = GetRequiredBufferSize ( m_lcstrSourceFiles );

    if ( m_rFOS.wFunc != FO_DELETE )
        {
        dwDestBufferSize = GetRequiredBufferSize ( m_lcstrDestFiles );
        }

    try
        {
        szzSourceFiles = (LPTSTR) new BYTE [ dwSourceBufferSize ];

        if ( m_rFOS.wFunc != FO_DELETE )
            {
            szzDestFiles = (LPTSTR) new BYTE [ dwDestBufferSize ];
            }
        }
    catch ( CMemoryException *)
        {
        TRACE0("Memory exception in CShellFileOp::Go()!\n");
        throw;
        }

    FillSzzBuffer ( szzSourceFiles, m_lcstrSourceFiles );

    if ( m_rFOS.wFunc != FO_DELETE )
        {
        FillSzzBuffer ( szzDestFiles, m_lcstrDestFiles );
        }

                                        // and now, the moment you've all been
                                        // waiting for

    m_rFOS.pFrom = szzSourceFiles;
    m_rFOS.pTo = szzDestFiles;
    m_rFOS.lpszProgressTitle = (LPCTSTR) m_cstrProgressDlgTitle;


                                        // If there are 2 or more strings in
                                        // the destination list, set the
                                        // MULTIDESTFILES flag.

    if ( m_lcstrDestFiles.GetCount() > 1 )
        {
        m_rFOS.fFlags |= FOF_MULTIDESTFILES;
        }


    m_bGoCalledAPI = TRUE;

    if ( NULL != lpbOperationStarted )
        {
        *lpbOperationStarted = TRUE;
        }

                                        // drum roll please....
    nAPIRet = SHFileOperation ( &m_rFOS );  // tah-dah!

                                        // Save the return value from the API.
    if ( NULL != lpnAPIReturn )
        {
        *lpnAPIReturn = nAPIRet;
        }

                                        // Check if the user cancelled the
                                        // operation.

    if ( NULL != lpbAnyOperationsAborted )
        {
        *lpbAnyOperationsAborted = m_rFOS.fAnyOperationsAborted;
        }

bailout:
    // If we got here via one of the gotos, fire off an assert.
    // If this assert fires, check the debug window for a TRACE output
    // line that describes the problem.
    ASSERT ( m_bGoCalledAPI );

                                        // Free buffers.
    if ( NULL != szzSourceFiles )
        {
        delete [] szzSourceFiles;
        }

    if ( NULL != szzDestFiles )
        {
        delete [] szzDestFiles;
        }


    return m_bGoCalledAPI  &&  0 == nAPIRet;
}


//////////////////////////////////////////////////////////////////////
// Private helper functions
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//
// Function:    CShellFileOp::ResetInternalData
//
// Description:
//  Clears the CShellFileOp object's member variables in preparation for a
//  new file operation.
//
// Input:
//  Nothing.
//
// Returns:
//  Nothing.
//
//////////////////////////////////////////////////////////////////////////

void CShellFileOp::ResetInternalData()
{
                                        // Empty the string lists
    m_lcstrSourceFiles.RemoveAll();
    m_lcstrDestFiles.RemoveAll();

                                        // Reset state variables
    m_bFlagsSet = FALSE;
    m_bGoCalledAPI = FALSE;

                                        // And clear out other stuff...
    m_cstrProgressDlgTitle.Empty();

    ZeroMemory ( &m_rFOS, sizeof ( m_rFOS ) );
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CShellFileOp::GetRequiredBufferSize
//
// Description:
//  Calculates the number of bytes required to hold the passed-in string
//  list in double-null-terminated character array form.
//
// Input:
//  list: [in] The string list to look at.
//
// Returns:
//  The number of bytes required.
//
//////////////////////////////////////////////////////////////////////////

DWORD CShellFileOp::GetRequiredBufferSize ( const CStringList& list )
{
DWORD    dwRetVal = 0;
POSITION pos;
CString  cstr;

    // If this assert fires, the passed-in list was empty. This ought to
    // never fire, actually, since Go() won't even call this function if
    // either list is empty.

    ASSERT ( !list.IsEmpty() );


    pos = list.GetHeadPosition();

    while ( NULL != pos )
        {
        cstr = list.GetNext ( pos );

        // **NOTE** The MFC docs for CString::GetLength() say that it returns
        // the number of bytes in the string, but that's wrong!!  In Unicode,
        // it returns the number of characters (which is half the number of
        // bytes).  Thus the multiplication by sizeof(TCHAR).

        dwRetVal += sizeof(TCHAR) * ( cstr.GetLength() + 1 );
        }

    return dwRetVal + sizeof(TCHAR);    // add one more for the final null
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CShellFileOp::FillSzzBuffer
//
// Description:
//  Copies a string list into a character array, making a double-null-terminated
//  list of strings.
//
// Input:
//  pBuffer: [out] The buffer that will hold the strings.
//  list: [in] The string list to read.
//
// Returns:
//  Nothing.
//
//////////////////////////////////////////////////////////////////////////

void CShellFileOp::FillSzzBuffer ( TCHAR* pBuffer, const CStringList& list )
{
TCHAR*   pCurrPos;
POSITION pos;
CString  cstr;

    // If this assert fires, the passed-in list was empty. This ought to
    // never fire, actually, since Go() won't even call this function if
    // either list is empty when it shouldn't be.

    ASSERT ( !list.IsEmpty() );

    ASSERT ( pBuffer != NULL );


    pCurrPos = pBuffer;

    pos = list.GetHeadPosition();

    while ( NULL != pos )
        {
        cstr = list.GetNext ( pos );

        _tcscpy ( pCurrPos, (LPCTSTR) cstr );

        pCurrPos = _tcsinc ( _tcschr ( pCurrPos, '\0' ) );
        }

                                        // Tack on the final null
    *pCurrPos = '\0';
}
