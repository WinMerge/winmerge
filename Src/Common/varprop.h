#ifndef varprop_h_included
#define varprop_h_included

namespace varprop {

typedef enum { VT_NULL, VT_INT, VT_FLOAT, VT_STRING, VT_TIME } VT_TYPE;

struct VariantValue
{
public:
	VariantValue() : vtype(VT_NULL), ivalue(0), fvalue(0) { }
	bool isInt() const { return vtype == VT_INT; }
	bool isFloat() const { return vtype == VT_FLOAT; }
	bool isString() const { return vtype == VT_STRING; }
	bool isTime() const { return vtype == VT_TIME; }
	VT_TYPE getType() const { return vtype; }

	void SetInt(int v) { Clear(); vtype = VT_INT; ivalue = v; }
	void SetFloat(double v) { Clear(); vtype = VT_FLOAT; fvalue = v; }
	void SetString(LPCTSTR sz) { Clear(); vtype = VT_STRING; svalue = sz; }
	void SetString(const CString & str) { Clear(); vtype = VT_STRING; svalue = str; }
	void SetTime(const COleDateTime & v) { Clear(); vtype = VT_TIME; tvalue = v; }

	void Clear() { vtype = VT_NULL; ivalue = 0; fvalue = 0; svalue = _T(""); tvalue.m_status = COleDateTime::null;}

	int getInt() const { ASSERT(vtype == VT_INT); return ivalue; }
	double getFloat() const { ASSERT(vtype == VT_FLOAT); return fvalue; }
	CString getString() const { ASSERT(vtype == VT_STRING); return svalue; }
	COleDateTime getTime() const { ASSERT(vtype == VT_TIME); return tvalue; }

private:
	VT_TYPE vtype;
	int ivalue;
	double fvalue;
	CString svalue;
	COleDateTime tvalue;
};

// Generic named property (name & value)
struct Property { CString name; VariantValue value; };

// Generic container of string properties, may be copied
class PropertySet : private CTypedPtrMap<CMapStringToPtr, CString, Property*>
{
public:
	// constructors, destructor, and copy operator
	PropertySet(int hashsize=-1);
	PropertySet(const PropertySet & src);
	PropertySet & operator=(const PropertySet & src);
	~PropertySet();
	void CopyFrom(const PropertySet & src);

	// use
	void SetProperty(const Property & property);
	void SetProperty(LPCTSTR name, COleDateTime time);
	void SetProperty(LPCTSTR name, LPCTSTR value);
	void SetProperty(LPCTSTR name, const CString & value);
	void SetProperty(LPCTSTR name, int value);
	Property * GetProperty(LPCTSTR szname);
	const Property * GetProperty(LPCTSTR szname) const;
private:
	void RemoveAll();
};

} // namespace

#endif // varprop_h_included
