#include <oleauto.h>
#include <afxtempl.h>

#include "dllpstub.h"

struct ICONV
{
//	ICONV dll interface
	struct Proxy;

	HMODULE BEGIN;

	HANDLE (*iconv_open)(const char *tocode, const char *fromcode);
	size_t (*iconv)(HANDLE, const char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);
	int (*iconv_close)(HANDLE);
	int (*iconvctl)(HANDLE, int request, void *argument);
	void (*iconvlist)
	(
		int (*do_one)(unsigned int namescount, const char *const *names, void *data),
		void *data
	);
	int *_libiconv_version;

	HMODULE END;
};

extern struct ICONV::Proxy
{
//	ICONV dll proxy
	DLLPSTUB stub;
	LPCSTR ICONV[&((ICONV*)0)->END - &((ICONV*)0)->BEGIN];
	HMODULE handle;
	struct ICONV *operator->()
	{
		stub.Load();
		return (struct ICONV *) ICONV;
	}
} ICONV;

//	Provide CMap template with reasonable HashKey() overload for BSTR
template<> UINT AFXAPI HashKey(BSTR);

class CMarkdown
{
//	Core class
public:
	class Converter
	{
	//	ICONV wrapper
	public:
		HANDLE handle;
		Converter(const char *tocode, const char *fromcode);
		~Converter();
		size_t iconv(const char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft) const;
	};
	class EntityString
	{
	//	Helper class to represent an EntityMap's KEYs and VALUEs. No orthogonal
	//	string class interface, just the methods required to make the CMap
	//	template happy. class EntityMap takes care of freeing the BSTRs.
	private:
		EntityString(const EntityString &);
	public:
		BSTR B;
		operator BSTR()
		{
			return B;
		}
		EntityString(BSTR B = 0):B(B)
		{
		}
		void operator=(BSTR B)
		{
			EntityString::B = B;
		}
		void operator=(const EntityString &r)
		{
			B = r.B;
		}
		bool operator==(BSTR B) const
		{
			return lstrcmpW(EntityString::B, B) == 0;
		}
	};
	class EntityMap : public CMap<EntityString, BSTR, EntityString, BSTR>
	{
	//	Lookup table for entity substitution
	public:
		void Load();
		void Load(CMarkdown &, const Converter &);
		~EntityMap()
		{
			EntityString key;
			EntityString val;
			POSITION pos = GetStartPosition();
			while (pos != NULL)
			{
				GetNextAssoc(pos, key, val);
				SysFreeString(key.B);
				SysFreeString(val.B);
			}
		}
		void SetAt(BSTR key, BSTR value)
		{
			EntityString &val = (*this)[key];
			SysFreeString(val.B);
			val.B = value;
		}
	};
	class FileImage;
	class File;
	typedef union _HSTR
	{
		OLECHAR B[20];
		CHAR A[20];
		TCHAR T[20];
		WCHAR W[20];
		// Unicode(codepage) converts string using MultiByteToWideChar()
		_HSTR *Unicode(UINT); // passing 1200 makes this a NOP
		// Octets(codepage) converts string using WideCharToMultiByte()
		_HSTR *Octets(UINT); // passing 1200 makes this a NOP
		// Convert(converter) converts string using an ICONV descriptor
		_HSTR *Convert(const Converter &);
		_HSTR *Resolve(const EntityMap &);
		_HSTR *Trim(const OLECHAR *);
	} *HSTR;
	union String
	{
	//	No orthogonal string class interface, just the methods required.
	//	Based on BSTR as it supports both octet strings and wide strings.
	//	The parser will always return octet strings, which can be converted to
	//	wide strings using Unicode() or Convert(), which can be converted back
	//	to octet strings using Octets() or Convert(). Convert() is the most
	//	versatile of these methods, but requires ICONV.DLL to be in place.
	//	Use Unicode() and Octets() if you must get away without ICONV.DLL.
	private:
		String(const String &);
		String &operator=(const String &);
	public:
		HSTR H;
		BSTR B;
		LPSTR A;
		LPTSTR T;
		LPWSTR W;
		String(HSTR H = 0):H(H)
		{
		}
		void operator=(HSTR H)
		{
			SysFreeString(B);
			B = H->B;
		}
		bool operator==(const char *A)
		{
			return lstrcmpA(String::A, A) == 0;
		}
		~String()
		{
			SysFreeString(B);
		}
		class AppendStream
		{
		//	Concatenation helper
			OLECHAR B[1];
		public:
			AppendStream *Append(const OLECHAR *B, UINT b)
			{
				memcpy(AppendStream::B, B, b * sizeof(OLECHAR));
				return (class AppendStream *)(AppendStream::B + b);
			}
		} *AppendStream(UINT cchGrow)
		{
			UINT b = SysStringLen(B);
			if (!SysReAllocStringLen(&B, B, b + cchGrow))
			{
				AfxThrowMemoryException();
			}
			return (class AppendStream *)(B + b);
		}
	};
	const char *first;	// first char of current markup (valid after Move)
	const char *lower;	// beginning of enclosed text (valid after Move)
	const char *upper;	// end of enclosed text (initially beginning of file)
	const char *ahead;	// last char of file
	CMarkdown(const char *upper, const char *ahead);
	operator bool();				// is node ahead?
	void Scan();					// find closing tag
	CMarkdown &Move();				// move to next node
	CMarkdown &Move(const char *);	// move to next node with given name
	bool Pull();					// pull child nodes into view
	CMarkdown &Pop();				// irreversible pull for chained calls
	bool Push();					// reverse pull
	HSTR GetTagName();				// tag name
	HSTR GetTagText();				// tag name plus attributes
	HSTR GetInnerText();			// text between enclosing tags
	HSTR GetOuterText();			// text including enclosing tags
	HSTR GetAttribute(const char *, const void * = 0); // random or enumerate
private:
	class Token;
};

class CMarkdown::FileImage
{
//	Map a file into process memory. Optionally convert UCS2 source to UTF8.
public:
	DWORD cbImage;
	LPVOID pImage;
	enum
	{
		Octets = 1
	};
	FileImage(LPCTSTR, DWORD trunc = 0, int flags = 0);
	~FileImage();
	static LPVOID NTAPI MapFile(HANDLE hFile, DWORD dwSize);
};

class CMarkdown::File : public CMarkdown::FileImage, public CMarkdown
{
//	Construct CMarkdown object from file.
public:
	File(LPCTSTR path, DWORD trunc = 0):
	CMarkdown::FileImage(path, trunc, Octets),
	CMarkdown((const char *)pImage, (const char *)pImage + cbImage)
	{
	}
};
