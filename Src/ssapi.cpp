// Machine generated IDispatch wrapper class(es) created with ClassWizard

#include "stdafx.h"
#include "ssapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// IVSSItemOld properties

/////////////////////////////////////////////////////////////////////////////
// IVSSItemOld operations

CString IVSSItemOld::GetSpec()
{
	CString result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

BOOL IVSSItemOld::GetBinary()
{
	BOOL result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void IVSSItemOld::SetBinary(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

BOOL IVSSItemOld::GetDeleted()
{
	BOOL result;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void IVSSItemOld::SetDeleted(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

long IVSSItemOld::GetType()
{
	long result;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

CString IVSSItemOld::GetLocalSpec()
{
	CString result;
	InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void IVSSItemOld::SetLocalSpec(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x5, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

CString IVSSItemOld::GetName()
{
	CString result;
	InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void IVSSItemOld::SetName(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x6, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

LPDISPATCH IVSSItemOld::GetParent()
{
	LPDISPATCH result;
	InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

long IVSSItemOld::GetVersionNumber()
{
	long result;
	InvokeHelper(0x8, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

LPDISPATCH IVSSItemOld::GetItems(BOOL IncludeDeleted)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x9, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms,
		IncludeDeleted);
	return result;
}

void IVSSItemOld::Get(BSTR* Local, long iFlags)
{
	static BYTE parms[] =
		VTS_PBSTR VTS_I4;
	InvokeHelper(0xa, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Local, iFlags);
}

void IVSSItemOld::Checkout(LPCTSTR Comment, LPCTSTR Local, long iFlags)
{
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_I4;
	InvokeHelper(0xb, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Comment, Local, iFlags);
}

void IVSSItemOld::Checkin(LPCTSTR Comment, LPCTSTR Local, long iFlags)
{
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_I4;
	InvokeHelper(0xc, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Comment, Local, iFlags);
}

void IVSSItemOld::UndoCheckout(LPCTSTR Local, long iFlags)
{
	static BYTE parms[] =
		VTS_BSTR VTS_I4;
	InvokeHelper(0xd, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Local, iFlags);
}

long IVSSItemOld::GetIsCheckedOut()
{
	long result;
	InvokeHelper(0xe, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

LPDISPATCH IVSSItemOld::GetCheckouts()
{
	LPDISPATCH result;
	InvokeHelper(0xf, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

BOOL IVSSItemOld::GetIsDifferent(LPCTSTR Local)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x10, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, parms,
		Local);
	return result;
}

LPDISPATCH IVSSItemOld::Add(LPCTSTR Local, LPCTSTR Comment, long iFlags)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_I4;
	InvokeHelper(0x11, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		Local, Comment, iFlags);
	return result;
}

LPDISPATCH IVSSItemOld::NewSubproject(LPCTSTR Name, LPCTSTR Comment)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x12, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		Name, Comment);
	return result;
}

void IVSSItemOld::Share(LPDISPATCH pIItem, LPCTSTR Comment, long iFlags)
{
	static BYTE parms[] =
		VTS_DISPATCH VTS_BSTR VTS_I4;
	InvokeHelper(0x13, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pIItem, Comment, iFlags);
}

void IVSSItemOld::Destroy()
{
	InvokeHelper(0x14, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IVSSItemOld::Move(LPDISPATCH pINewParent)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x15, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pINewParent);
}

void IVSSItemOld::Label(LPCTSTR Label, LPCTSTR Comment)
{
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x16, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Label, Comment);
}

LPDISPATCH IVSSItemOld::GetVersions(long iFlags)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x17, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms,
		iFlags);
	return result;
}

LPDISPATCH IVSSItemOld::GetVersion(const VARIANT& Version)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x18, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms,
		&Version);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IVSSItem properties

/////////////////////////////////////////////////////////////////////////////
// IVSSItem operations

CString IVSSItem::GetSpec()
{
	CString result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

BOOL IVSSItem::GetBinary()
{
	BOOL result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void IVSSItem::SetBinary(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

BOOL IVSSItem::GetDeleted()
{
	BOOL result;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void IVSSItem::SetDeleted(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

long IVSSItem::GetType()
{
	long result;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

CString IVSSItem::GetLocalSpec()
{
	CString result;
	InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void IVSSItem::SetLocalSpec(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x5, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

CString IVSSItem::GetName()
{
	CString result;
	InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void IVSSItem::SetName(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x6, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

LPDISPATCH IVSSItem::GetParent()
{
	LPDISPATCH result;
	InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

long IVSSItem::GetVersionNumber()
{
	long result;
	InvokeHelper(0x8, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

LPDISPATCH IVSSItem::GetItems(BOOL IncludeDeleted)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x9, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms,
		IncludeDeleted);
	return result;
}

void IVSSItem::Get(BSTR* Local, long iFlags)
{
	static BYTE parms[] =
		VTS_PBSTR VTS_I4;
	InvokeHelper(0xa, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Local, iFlags);
}

void IVSSItem::Checkout(LPCTSTR Comment, LPCTSTR Local, long iFlags)
{
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_I4;
	InvokeHelper(0xb, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Comment, Local, iFlags);
}

void IVSSItem::Checkin(LPCTSTR Comment, LPCTSTR Local, long iFlags)
{
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_I4;
	InvokeHelper(0xc, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Comment, Local, iFlags);
}

void IVSSItem::UndoCheckout(LPCTSTR Local, long iFlags)
{
	static BYTE parms[] =
		VTS_BSTR VTS_I4;
	InvokeHelper(0xd, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Local, iFlags);
}

long IVSSItem::GetIsCheckedOut()
{
	long result;
	InvokeHelper(0xe, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

LPDISPATCH IVSSItem::GetCheckouts()
{
	LPDISPATCH result;
	InvokeHelper(0xf, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

BOOL IVSSItem::GetIsDifferent(LPCTSTR Local)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x10, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, parms,
		Local);
	return result;
}

LPDISPATCH IVSSItem::Add(LPCTSTR Local, LPCTSTR Comment, long iFlags)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_I4;
	InvokeHelper(0x11, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		Local, Comment, iFlags);
	return result;
}

LPDISPATCH IVSSItem::NewSubproject(LPCTSTR Name, LPCTSTR Comment)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x12, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		Name, Comment);
	return result;
}

void IVSSItem::Share(LPDISPATCH pIItem, LPCTSTR Comment, long iFlags)
{
	static BYTE parms[] =
		VTS_DISPATCH VTS_BSTR VTS_I4;
	InvokeHelper(0x13, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pIItem, Comment, iFlags);
}

void IVSSItem::Destroy()
{
	InvokeHelper(0x14, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void IVSSItem::Move(LPDISPATCH pINewParent)
{
	static BYTE parms[] =
		VTS_DISPATCH;
	InvokeHelper(0x15, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pINewParent);
}

void IVSSItem::Label(LPCTSTR Label, LPCTSTR Comment)
{
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x16, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Label, Comment);
}

LPDISPATCH IVSSItem::GetVersions(long iFlags)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x17, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms,
		iFlags);
	return result;
}

LPDISPATCH IVSSItem::GetVersion(const VARIANT& Version)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x18, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms,
		&Version);
	return result;
}

LPDISPATCH IVSSItem::GetLinks()
{
	LPDISPATCH result;
	InvokeHelper(0x19, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH IVSSItem::Branch(LPCTSTR Comment, long iFlags)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_I4;
	InvokeHelper(0x1a, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		Comment, iFlags);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IVSSVersions properties

/////////////////////////////////////////////////////////////////////////////
// IVSSVersions operations


/////////////////////////////////////////////////////////////////////////////
// IVSSVersionOld properties

/////////////////////////////////////////////////////////////////////////////
// IVSSVersionOld operations

CString IVSSVersionOld::GetUsername()
{
	CString result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

long IVSSVersionOld::GetVersionNumber()
{
	long result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

CString IVSSVersionOld::GetAction()
{
	CString result;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

DATE IVSSVersionOld::GetDate()
{
	DATE result;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_DATE, (void*)&result, NULL);
	return result;
}

CString IVSSVersionOld::GetComment()
{
	CString result;
	InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IVSSVersionOld::GetLabel()
{
	CString result;
	InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

LPDISPATCH IVSSVersionOld::GetVSSItem()
{
	LPDISPATCH result;
	InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IVSSVersion properties

/////////////////////////////////////////////////////////////////////////////
// IVSSVersion operations

CString IVSSVersion::GetUsername()
{
	CString result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

long IVSSVersion::GetVersionNumber()
{
	long result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

CString IVSSVersion::GetAction()
{
	CString result;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

DATE IVSSVersion::GetDate()
{
	DATE result;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_DATE, (void*)&result, NULL);
	return result;
}

CString IVSSVersion::GetComment()
{
	CString result;
	InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IVSSVersion::GetLabel()
{
	CString result;
	InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

LPDISPATCH IVSSVersion::GetVSSItem()
{
	LPDISPATCH result;
	InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

CString IVSSVersion::GetLabelComment()
{
	CString result;
	InvokeHelper(0x8, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IVSSItems properties

/////////////////////////////////////////////////////////////////////////////
// IVSSItems operations

long IVSSItems::GetCount()
{
	long result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

LPDISPATCH IVSSItems::GetItem(const VARIANT& sItem)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms,
		&sItem);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IVSSCheckouts properties

/////////////////////////////////////////////////////////////////////////////
// IVSSCheckouts operations

long IVSSCheckouts::GetCount()
{
	long result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

LPDISPATCH IVSSCheckouts::GetItem(const VARIANT& sItem)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms,
		&sItem);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IVSSCheckout properties

/////////////////////////////////////////////////////////////////////////////
// IVSSCheckout operations

CString IVSSCheckout::GetUsername()
{
	CString result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

DATE IVSSCheckout::GetDate()
{
	DATE result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_DATE, (void*)&result, NULL);
	return result;
}

CString IVSSCheckout::GetLocalSpec()
{
	CString result;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IVSSCheckout::GetMachine()
{
	CString result;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IVSSCheckout::GetProject()
{
	CString result;
	InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IVSSCheckout::GetComment()
{
	CString result;
	InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

long IVSSCheckout::GetVersionNumber()
{
	long result;
	InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IVSSDatabaseOld properties

/////////////////////////////////////////////////////////////////////////////
// IVSSDatabaseOld operations

void IVSSDatabaseOld::Open(LPCTSTR SrcSafeIni, LPCTSTR Username, LPCTSTR Password)
{
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_BSTR;
	InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 SrcSafeIni, Username, Password);
}

CString IVSSDatabaseOld::GetSrcSafeIni()
{
	CString result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IVSSDatabaseOld::GetDatabaseName()
{
	CString result;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IVSSDatabaseOld::GetUsername()
{
	CString result;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IVSSDatabaseOld::GetCurrentProject()
{
	CString result;
	InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void IVSSDatabaseOld::SetCurrentProject(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x5, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

LPDISPATCH IVSSDatabaseOld::GetVSSItem(LPCTSTR Spec, BOOL Deleted)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BOOL;
	InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms,
		Spec, Deleted);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IVSSDatabase properties

/////////////////////////////////////////////////////////////////////////////
// IVSSDatabase operations

void IVSSDatabase::Open(LPCTSTR SrcSafeIni, LPCTSTR Username, LPCTSTR Password)
{
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_BSTR;
	InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 SrcSafeIni, Username, Password);
}

CString IVSSDatabase::GetSrcSafeIni()
{
	CString result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IVSSDatabase::GetDatabaseName()
{
	CString result;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IVSSDatabase::GetUsername()
{
	CString result;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString IVSSDatabase::GetCurrentProject()
{
	CString result;
	InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void IVSSDatabase::SetCurrentProject(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x5, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

LPDISPATCH IVSSDatabase::GetVSSItem(LPCTSTR Spec, BOOL Deleted)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BOOL;
	InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms,
		Spec, Deleted);
	return result;
}

LPDISPATCH IVSSDatabase::AddUser(LPCTSTR User, LPCTSTR Password, BOOL ReadOnly)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR VTS_BOOL;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms,
		User, Password, ReadOnly);
	return result;
}

LPDISPATCH IVSSDatabase::GetUser(LPCTSTR Name)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x8, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms,
		Name);
	return result;
}

LPDISPATCH IVSSDatabase::GetUsers()
{
	LPDISPATCH result;
	InvokeHelper(0x9, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

BOOL IVSSDatabase::GetProjectRightsEnabled()
{
	BOOL result;
	InvokeHelper(0xa, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void IVSSDatabase::SetProjectRightsEnabled(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0xa, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

long IVSSDatabase::GetDefaultProjectRights()
{
	long result;
	InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IVSSDatabase::SetDefaultProjectRights(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xb, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}


/////////////////////////////////////////////////////////////////////////////
// IVSSUser properties

/////////////////////////////////////////////////////////////////////////////
// IVSSUser operations

void IVSSUser::Delete()
{
	InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

CString IVSSUser::GetName()
{
	CString result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void IVSSUser::SetName(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

void IVSSUser::SetPassword(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

BOOL IVSSUser::GetReadOnly()
{
	BOOL result;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void IVSSUser::SetReadOnly(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x4, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

long IVSSUser::GetProjectRights(LPCTSTR Project)
{
	long result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms,
		Project);
	return result;
}

void IVSSUser::SetProjectRights(LPCTSTR Project, long nNewValue)
{
	static BYTE parms[] =
		VTS_BSTR VTS_I4;
	InvokeHelper(0x5, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Project, nNewValue);
}

void IVSSUser::RemoveProjectRights(LPCTSTR Project)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Project);
}


/////////////////////////////////////////////////////////////////////////////
// IVSSUsers properties

/////////////////////////////////////////////////////////////////////////////
// IVSSUsers operations

long IVSSUsers::GetCount()
{
	long result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

LPDISPATCH IVSSUsers::GetItem(const VARIANT& sItem)
{
	LPDISPATCH result;
	static BYTE parms[] =
		VTS_VARIANT;
	InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms,
		&sItem);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IVSS properties

/////////////////////////////////////////////////////////////////////////////
// IVSS operations

LPDISPATCH IVSS::GetVSSDatabase()
{
	LPDISPATCH result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}
