// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// IVSSItemOld wrapper class

class IVSSItemOld : public COleDispatchDriver
{
public:
	IVSSItemOld() {}		// Calls COleDispatchDriver default constructor
	IVSSItemOld(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IVSSItemOld(const IVSSItemOld& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetSpec();
	BOOL GetBinary();
	void SetBinary(BOOL bNewValue);
	BOOL GetDeleted();
	void SetDeleted(BOOL bNewValue);
	long GetType();
	CString GetLocalSpec();
	void SetLocalSpec(LPCTSTR lpszNewValue);
	CString GetName();
	void SetName(LPCTSTR lpszNewValue);
	LPDISPATCH GetParent();
	long GetVersionNumber();
	LPDISPATCH GetItems(BOOL IncludeDeleted);
	void Get(BSTR* Local, long iFlags);
	void Checkout(LPCTSTR Comment, LPCTSTR Local, long iFlags);
	void Checkin(LPCTSTR Comment, LPCTSTR Local, long iFlags);
	void UndoCheckout(LPCTSTR Local, long iFlags);
	long GetIsCheckedOut();
	LPDISPATCH GetCheckouts();
	BOOL GetIsDifferent(LPCTSTR Local);
	LPDISPATCH Add(LPCTSTR Local, LPCTSTR Comment, long iFlags);
	LPDISPATCH NewSubproject(LPCTSTR Name, LPCTSTR Comment);
	void Share(LPDISPATCH pIItem, LPCTSTR Comment, long iFlags);
	void Destroy();
	void Move(LPDISPATCH pINewParent);
	void Label(LPCTSTR Label, LPCTSTR Comment);
	LPDISPATCH GetVersions(long iFlags);
	LPDISPATCH GetVersion(const VARIANT& Version);
};
/////////////////////////////////////////////////////////////////////////////
// IVSSItem wrapper class

class IVSSItem : public COleDispatchDriver
{
public:
	IVSSItem() {}		// Calls COleDispatchDriver default constructor
	IVSSItem(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IVSSItem(const IVSSItem& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetSpec();
	BOOL GetBinary();
	void SetBinary(BOOL bNewValue);
	BOOL GetDeleted();
	void SetDeleted(BOOL bNewValue);
	long GetType();
	CString GetLocalSpec();
	void SetLocalSpec(LPCTSTR lpszNewValue);
	CString GetName();
	void SetName(LPCTSTR lpszNewValue);
	LPDISPATCH GetParent();
	long GetVersionNumber();
	LPDISPATCH GetItems(BOOL IncludeDeleted);
	void Get(BSTR* Local, long iFlags);
	void Checkout(LPCTSTR Comment, LPCTSTR Local, long iFlags);
	void Checkin(LPCTSTR Comment, LPCTSTR Local, long iFlags);
	void UndoCheckout(LPCTSTR Local, long iFlags);
	long GetIsCheckedOut();
	LPDISPATCH GetCheckouts();
	BOOL GetIsDifferent(LPCTSTR Local);
	LPDISPATCH Add(LPCTSTR Local, LPCTSTR Comment, long iFlags);
	LPDISPATCH NewSubproject(LPCTSTR Name, LPCTSTR Comment);
	void Share(LPDISPATCH pIItem, LPCTSTR Comment, long iFlags);
	void Destroy();
	void Move(LPDISPATCH pINewParent);
	void Label(LPCTSTR Label, LPCTSTR Comment);
	LPDISPATCH GetVersions(long iFlags);
	LPDISPATCH GetVersion(const VARIANT& Version);
	LPDISPATCH GetLinks();
	LPDISPATCH Branch(LPCTSTR Comment, long iFlags);
};
/////////////////////////////////////////////////////////////////////////////
// IVSSVersions wrapper class

class IVSSVersions : public COleDispatchDriver
{
public:
	IVSSVersions() {}		// Calls COleDispatchDriver default constructor
	IVSSVersions(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IVSSVersions(const IVSSVersions& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
};
/////////////////////////////////////////////////////////////////////////////
// IVSSVersionOld wrapper class

class IVSSVersionOld : public COleDispatchDriver
{
public:
	IVSSVersionOld() {}		// Calls COleDispatchDriver default constructor
	IVSSVersionOld(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IVSSVersionOld(const IVSSVersionOld& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetUsername();
	long GetVersionNumber();
	CString GetAction();
	DATE GetDate();
	CString GetComment();
	CString GetLabel();
	LPDISPATCH GetVSSItem();
};
/////////////////////////////////////////////////////////////////////////////
// IVSSVersion wrapper class

class IVSSVersion : public COleDispatchDriver
{
public:
	IVSSVersion() {}		// Calls COleDispatchDriver default constructor
	IVSSVersion(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IVSSVersion(const IVSSVersion& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetUsername();
	long GetVersionNumber();
	CString GetAction();
	DATE GetDate();
	CString GetComment();
	CString GetLabel();
	LPDISPATCH GetVSSItem();
	CString GetLabelComment();
};
/////////////////////////////////////////////////////////////////////////////
// IVSSItems wrapper class

class IVSSItems : public COleDispatchDriver
{
public:
	IVSSItems() {}		// Calls COleDispatchDriver default constructor
	IVSSItems(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IVSSItems(const IVSSItems& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long GetCount();
	LPDISPATCH GetItem(const VARIANT& sItem);
};
/////////////////////////////////////////////////////////////////////////////
// IVSSCheckouts wrapper class

class IVSSCheckouts : public COleDispatchDriver
{
public:
	IVSSCheckouts() {}		// Calls COleDispatchDriver default constructor
	IVSSCheckouts(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IVSSCheckouts(const IVSSCheckouts& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long GetCount();
	LPDISPATCH GetItem(const VARIANT& sItem);
};
/////////////////////////////////////////////////////////////////////////////
// IVSSCheckout wrapper class

class IVSSCheckout : public COleDispatchDriver
{
public:
	IVSSCheckout() {}		// Calls COleDispatchDriver default constructor
	IVSSCheckout(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IVSSCheckout(const IVSSCheckout& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	CString GetUsername();
	DATE GetDate();
	CString GetLocalSpec();
	CString GetMachine();
	CString GetProject();
	CString GetComment();
	long GetVersionNumber();
};
/////////////////////////////////////////////////////////////////////////////
// IVSSDatabaseOld wrapper class

class IVSSDatabaseOld : public COleDispatchDriver
{
public:
	IVSSDatabaseOld() {}		// Calls COleDispatchDriver default constructor
	IVSSDatabaseOld(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IVSSDatabaseOld(const IVSSDatabaseOld& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	void Open(LPCTSTR SrcSafeIni, LPCTSTR Username, LPCTSTR Password);
	CString GetSrcSafeIni();
	CString GetDatabaseName();
	CString GetUsername();
	CString GetCurrentProject();
	void SetCurrentProject(LPCTSTR lpszNewValue);
	LPDISPATCH GetVSSItem(LPCTSTR Spec, BOOL Deleted);
};
/////////////////////////////////////////////////////////////////////////////
// IVSSDatabase wrapper class

class IVSSDatabase : public COleDispatchDriver
{
public:
	IVSSDatabase() {}		// Calls COleDispatchDriver default constructor
	IVSSDatabase(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IVSSDatabase(const IVSSDatabase& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	void Open(LPCTSTR SrcSafeIni, LPCTSTR Username, LPCTSTR Password);
	CString GetSrcSafeIni();
	CString GetDatabaseName();
	CString GetUsername();
	CString GetCurrentProject();
	void SetCurrentProject(LPCTSTR lpszNewValue);
	LPDISPATCH GetVSSItem(LPCTSTR Spec, BOOL Deleted);
	LPDISPATCH AddUser(LPCTSTR User, LPCTSTR Password, BOOL ReadOnly);
	LPDISPATCH GetUser(LPCTSTR Name);
	LPDISPATCH GetUsers();
	BOOL GetProjectRightsEnabled();
	void SetProjectRightsEnabled(BOOL bNewValue);
	long GetDefaultProjectRights();
	void SetDefaultProjectRights(long nNewValue);
};
/////////////////////////////////////////////////////////////////////////////
// IVSSUser wrapper class

class IVSSUser : public COleDispatchDriver
{
public:
	IVSSUser() {}		// Calls COleDispatchDriver default constructor
	IVSSUser(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IVSSUser(const IVSSUser& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	void Delete();
	CString GetName();
	void SetName(LPCTSTR lpszNewValue);
	void SetPassword(LPCTSTR lpszNewValue);
	BOOL GetReadOnly();
	void SetReadOnly(BOOL bNewValue);
	long GetProjectRights(LPCTSTR Project);
	void SetProjectRights(LPCTSTR Project, long nNewValue);
	void RemoveProjectRights(LPCTSTR Project);
};
/////////////////////////////////////////////////////////////////////////////
// IVSSUsers wrapper class

class IVSSUsers : public COleDispatchDriver
{
public:
	IVSSUsers() {}		// Calls COleDispatchDriver default constructor
	IVSSUsers(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IVSSUsers(const IVSSUsers& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long GetCount();
	LPDISPATCH GetItem(const VARIANT& sItem);
};
/////////////////////////////////////////////////////////////////////////////
// IVSS wrapper class

class IVSS : public COleDispatchDriver
{
public:
	IVSS() {}		// Calls COleDispatchDriver default constructor
	IVSS(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IVSS(const IVSS& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	LPDISPATCH GetVSSDatabase();
};
