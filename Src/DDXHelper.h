#pragma once

#include "UnicodeString.h"

// from http://stackoverflow.com/questions/6117270/mfc-stdstring-vs-cstring
class PopString : public CString
{
public:
	explicit PopString(String & final) : CString(final.c_str()), m_final(final)
	{
	}

	~PopString()
	{
		m_final = (PCTSTR) *this;
	}
private:
	PopString(const PopString &);  // private copy constructor to prevent copying
	PopString & operator=(const PopString &);  // private copy operator

	String & m_final;
};

inline void DDX_Check(CDataExchange* pDX, int nIDC, bool& value)
{
	BOOL BOOL_value = value;
	DDX_Check(pDX, nIDC, BOOL_value);
	value = !!BOOL_value;
}

inline void DDX_Radio(CDataExchange* pDX, int nIDC, bool& value)
{
	BOOL BOOL_value = value;
	DDX_Radio(pDX, nIDC, BOOL_value);
	value = !!BOOL_value;
}

inline void DDX_Text(CDataExchange* pDX, int nIDC, String& value)
{
	DDX_Text(pDX, nIDC, PopString(value));
}

inline void DDX_CBString(CDataExchange* pDX, int nIDC, String& value)
{
	DDX_CBString(pDX, nIDC, PopString(value));
}

inline void DDX_CBStringExact(CDataExchange* pDX, int nIDC, String& value)
{
	DDX_CBStringExact(pDX, nIDC, PopString(value));
}

